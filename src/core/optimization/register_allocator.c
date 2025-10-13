#include "../../include/core/optimization/register_allocator.h"
#include "../../include/utils/shared_utilities.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

// ============================================================================
// REGISTER ALLOCATOR IMPLEMENTATION
// ============================================================================

RegisterAllocationContext* register_allocator_create(RegisterProgram* program, RegisterAllocationStrategy strategy) {
    if (!program) return NULL;
    
    RegisterAllocationContext* context = (RegisterAllocationContext*)shared_malloc_safe(
        sizeof(RegisterAllocationContext), "register_allocator", "create", 0);
    if (!context) return NULL;
    
    context->program = program;
    context->lifetimes = NULL;
    context->lifetime_count = 0;
    context->physical_registers = NULL;
    context->physical_register_count = 16; // Default: 16 physical registers
    context->spill_slots = NULL;
    context->spill_slot_count = 0;
    context->strategy = strategy;
    context->optimize_for_speed = 1;
    context->allow_spilling = 1;
    context->prioritize_hot_registers = 1;
    context->total_allocations = 0;
    context->spill_count = 0;
    context->move_count = 0;
    context->allocation_time_ms = 0.0;
    
    return context;
}

void register_allocator_free(RegisterAllocationContext* context) {
    if (!context) return;
    
    // Free lifetimes
    if (context->lifetimes) {
        for (size_t i = 0; i < context->lifetime_count; i++) {
            if (context->lifetimes[i].interfering_registers) {
                shared_free_safe(context->lifetimes[i].interfering_registers, 
                               "register_allocator", "free", 0);
            }
        }
        shared_free_safe(context->lifetimes, "register_allocator", "free", 0);
    }
    
    // Free physical registers
    if (context->physical_registers) {
        shared_free_safe(context->physical_registers, "register_allocator", "free", 0);
    }
    
    // Free spill slots
    if (context->spill_slots) {
        shared_free_safe(context->spill_slots, "register_allocator", "free", 0);
    }
    
    shared_free_safe(context, "register_allocator", "free", 0);
}

RegisterAllocationResult register_allocate(RegisterAllocationContext* context) {
    RegisterAllocationResult result = {0};
    
    if (!context || !context->program) {
        result.success = 0;
        result.error_message = "Invalid allocation context";
        return result;
    }
    
    // Analyze lifetimes
    if (!register_analyze_lifetimes(context)) {
        result.success = 0;
        result.error_message = "Failed to analyze register lifetimes";
        return result;
    }
    
    // Build interference graph
    if (!register_build_interference_graph(context)) {
        result.success = 0;
        result.error_message = "Failed to build interference graph";
        return result;
    }
    
    // Perform allocation based on strategy
    int allocation_success = 0;
    switch (context->strategy) {
        case REG_ALLOC_LINEAR_SCAN:
            allocation_success = register_linear_scan_allocate(context);
            break;
        case REG_ALLOC_GRAPH_COLORING:
            allocation_success = register_graph_coloring_allocate(context);
            break;
        default:
            allocation_success = register_linear_scan_allocate(context);
            break;
    }
    
    if (!allocation_success) {
        result.success = 0;
        result.error_message = "Register allocation failed";
        return result;
    }
    
    // Calculate results
    result.success = 1;
    result.physical_register_count = context->physical_register_count;
    result.spill_slot_count = context->spill_slot_count;
    result.move_instruction_count = context->move_count;
    result.allocation_quality = 1.0 - ((double)context->spill_count / context->lifetime_count);
    result.estimated_performance = 1.0 - (context->spill_count * 0.1); // Rough estimate
    result.error_message = NULL;
    
    return result;
}

int register_analyze_lifetimes(RegisterAllocationContext* context) {
    if (!context || !context->program) return 0;
    
    // Allocate lifetimes array
    context->lifetime_count = context->program->register_count;
    if (context->lifetime_count == 0) return 1; // No registers to analyze
    
    context->lifetimes = (RegisterLifetime*)shared_malloc_safe(
        sizeof(RegisterLifetime) * context->lifetime_count,
        "register_allocator", "analyze_lifetimes", 0);
    if (!context->lifetimes) return 0;
    
    // Initialize lifetimes
    for (size_t i = 0; i < context->lifetime_count; i++) {
        context->lifetimes[i].virtual_register = i;
        context->lifetimes[i].definition_point = 0;
        context->lifetimes[i].last_use_point = 0;
        context->lifetimes[i].live_range_start = 0;
        context->lifetimes[i].live_range_end = 0;
        context->lifetimes[i].is_hot = 0;
        context->lifetimes[i].is_spilled = 0;
        context->lifetimes[i].physical_register = -1;
        context->lifetimes[i].spill_slot = -1;
        context->lifetimes[i].hotness_score = 0.0;
        context->lifetimes[i].interference_count = 0;
        context->lifetimes[i].interfering_registers = NULL;
    }
    
    // Analyze instruction sequence to determine lifetimes
    for (size_t i = 0; i < context->program->instruction_count; i++) {
        RegisterInstruction instr = context->program->instructions[i];
        
        // Update definition point for destination register
        if (instr.dst < context->lifetime_count) {
            if (context->lifetimes[instr.dst].definition_point == 0) {
                context->lifetimes[instr.dst].definition_point = i;
            }
            context->lifetimes[instr.dst].last_use_point = i;
        }
        
        // Update use points for source registers
        if (instr.src1 < context->lifetime_count) {
            context->lifetimes[instr.src1].last_use_point = i;
        }
        if (instr.src2 < context->lifetime_count) {
            context->lifetimes[instr.src2].last_use_point = i;
        }
        if (instr.src3 < context->lifetime_count) {
            context->lifetimes[instr.src3].last_use_point = i;
        }
    }
    
    // Calculate live ranges
    for (size_t i = 0; i < context->lifetime_count; i++) {
        context->lifetimes[i].live_range_start = context->lifetimes[i].definition_point;
        context->lifetimes[i].live_range_end = context->lifetimes[i].last_use_point;
    }
    
    return 1;
}

int register_build_interference_graph(RegisterAllocationContext* context) {
    if (!context || !context->lifetimes) return 0;
    
    // Count interferences for each register
    for (size_t i = 0; i < context->lifetime_count; i++) {
        context->lifetimes[i].interference_count = 0;
        
        // Count interferences with other registers
        for (size_t j = 0; j < context->lifetime_count; j++) {
            if (i != j && register_calculate_interference(context, i, j)) {
                context->lifetimes[i].interference_count++;
            }
        }
        
        // Allocate interference array
        if (context->lifetimes[i].interference_count > 0) {
            context->lifetimes[i].interfering_registers = (size_t*)shared_malloc_safe(
                sizeof(size_t) * context->lifetimes[i].interference_count,
                "register_allocator", "build_interference_graph", 0);
            if (!context->lifetimes[i].interfering_registers) return 0;
            
            // Fill interference array
            size_t interference_index = 0;
            for (size_t j = 0; j < context->lifetime_count; j++) {
                if (i != j && register_calculate_interference(context, i, j)) {
                    context->lifetimes[i].interfering_registers[interference_index] = j;
                    interference_index++;
                }
            }
        }
    }
    
    return 1;
}

int register_linear_scan_allocate(RegisterAllocationContext* context) {
    if (!context || !context->lifetimes) return 0;
    
    // Allocate physical registers array
    context->physical_registers = (int*)shared_malloc_safe(
        sizeof(int) * context->physical_register_count,
        "register_allocator", "linear_scan_allocate", 0);
    if (!context->physical_registers) return 0;
    
    // Initialize physical registers as free
    for (size_t i = 0; i < context->physical_register_count; i++) {
        context->physical_registers[i] = -1; // -1 means free
    }
    
    // Sort lifetimes by live range start
    // TODO: Implement proper sorting
    
    // Allocate registers using linear scan
    for (size_t i = 0; i < context->lifetime_count; i++) {
        RegisterLifetime* lifetime = &context->lifetimes[i];
        
        // Find a free physical register
        int physical_reg = -1;
        for (size_t j = 0; j < context->physical_register_count; j++) {
            if (context->physical_registers[j] == -1) {
                physical_reg = j;
                break;
            }
        }
        
        if (physical_reg >= 0) {
            // Allocate physical register
            lifetime->physical_register = physical_reg;
            context->physical_registers[physical_reg] = lifetime->virtual_register;
            lifetime->is_spilled = 0;
        } else {
            // No free register, need to spill
            if (context->allow_spilling) {
                // Find spill candidate
                size_t spill_candidates[16];
                size_t candidate_count = register_find_spill_candidates(context, spill_candidates, 16);
                
                if (candidate_count > 0) {
                    // Spill the first candidate
                    size_t spill_reg = spill_candidates[0];
                    RegisterLifetime* spill_lifetime = &context->lifetimes[spill_reg];
                    
                    // Free the physical register
                    context->physical_registers[spill_lifetime->physical_register] = -1;
                    spill_lifetime->physical_register = -1;
                    spill_lifetime->is_spilled = 1;
                    spill_lifetime->spill_slot = context->spill_slot_count++;
                    
                    // Allocate to the new register
                    lifetime->physical_register = spill_lifetime->physical_register;
                    context->physical_registers[lifetime->physical_register] = lifetime->virtual_register;
                    lifetime->is_spilled = 0;
                    
                    context->spill_count++;
                } else {
                    // Cannot spill, allocation failed
                    return 0;
                }
            } else {
                // Spilling not allowed, allocation failed
                return 0;
            }
        }
    }
    
    return 1;
}

int register_graph_coloring_allocate(RegisterAllocationContext* context) {
    if (!context || !context->lifetimes) return 0;
    
    // TODO: Implement graph coloring allocation
    // This is a placeholder implementation
    
    // For now, fall back to linear scan
    return register_linear_scan_allocate(context);
}

int register_generate_spill_code(RegisterAllocationContext* context, size_t virtual_register) {
    if (!context || !context->program) return 0;
    
    // TODO: Implement spill code generation
    // This would generate load/store instructions for spilled registers
    
    return 1;
}

int register_generate_move_code(RegisterAllocationContext* context, size_t src_register, size_t dst_register) {
    if (!context || !context->program) return 0;
    
    // TODO: Implement move code generation
    // This would generate move instructions for register coalescing
    
    context->move_count++;
    return 1;
}

int register_optimize_spill_placement(RegisterAllocationContext* context) {
    if (!context) return 0;
    
    // TODO: Implement spill placement optimization
    // This would optimize where spill code is placed
    
    return 1;
}

size_t register_analyze_pressure(RegisterAllocationContext* context) {
    if (!context || !context->lifetimes) return 0;
    
    size_t max_pressure = 0;
    
    // TODO: Implement proper pressure analysis
    // This would analyze the maximum number of live registers at any point
    
    return max_pressure;
}

int register_analyze_hot_spots(RegisterAllocationContext* context) {
    if (!context || !context->lifetimes) return 0;
    
    // TODO: Implement hot spot analysis
    // This would identify hot spots where register allocation is critical
    
    return 1;
}

int register_calculate_interference(RegisterAllocationContext* context, size_t reg1, size_t reg2) {
    if (!context || !context->lifetimes || reg1 >= context->lifetime_count || reg2 >= context->lifetime_count) {
        return 0;
    }
    
    RegisterLifetime* lifetime1 = &context->lifetimes[reg1];
    RegisterLifetime* lifetime2 = &context->lifetimes[reg2];
    
    // Two registers interfere if their live ranges overlap
    return (lifetime1->live_range_start <= lifetime2->live_range_end &&
            lifetime2->live_range_start <= lifetime1->live_range_end);
}

size_t register_find_spill_candidates(RegisterAllocationContext* context, size_t* candidates, size_t max_candidates) {
    if (!context || !context->lifetimes || !candidates) return 0;
    
    size_t candidate_count = 0;
    
    // Find registers with the highest interference count
    for (size_t i = 0; i < context->lifetime_count && candidate_count < max_candidates; i++) {
        if (context->lifetimes[i].physical_register >= 0 && !context->lifetimes[i].is_spilled) {
            candidates[candidate_count] = i;
            candidate_count++;
        }
    }
    
    return candidate_count;
}

int register_optimize_hot_paths(RegisterAllocationContext* context) {
    if (!context || !context->lifetimes) return 0;
    
    // TODO: Implement hot path optimization
    // This would prioritize hot registers for better performance
    
    return 1;
}

int register_coalesce_moves(RegisterAllocationContext* context) {
    if (!context) return 0;
    
    // TODO: Implement move coalescing
    // This would eliminate unnecessary move instructions
    
    return 0;
}

int register_optimize_spill_slots(RegisterAllocationContext* context) {
    if (!context) return 0;
    
    // TODO: Implement spill slot optimization
    // This would optimize the use of spill slots
    
    return 1;
}

int register_apply_allocation_hints(RegisterAllocationContext* context) {
    if (!context) return 0;
    
    // TODO: Implement allocation hints
    // This would apply hints from the compiler
    
    return 1;
}

int register_get_physical_register(RegisterAllocationContext* context, size_t virtual_register) {
    if (!context || !context->lifetimes || virtual_register >= context->lifetime_count) {
        return -1;
    }
    
    return context->lifetimes[virtual_register].physical_register;
}

int register_get_spill_slot(RegisterAllocationContext* context, size_t virtual_register) {
    if (!context || !context->lifetimes || virtual_register >= context->lifetime_count) {
        return -1;
    }
    
    return context->lifetimes[virtual_register].spill_slot;
}

int register_is_spilled(RegisterAllocationContext* context, size_t virtual_register) {
    if (!context || !context->lifetimes || virtual_register >= context->lifetime_count) {
        return 0;
    }
    
    return context->lifetimes[virtual_register].is_spilled;
}

char* register_get_allocation_stats(RegisterAllocationContext* context) {
    if (!context) return NULL;
    
    // TODO: Implement allocation statistics
    // This would return a formatted string with allocation statistics
    
    return NULL;
}

void register_print_allocation(RegisterAllocationContext* context) {
    if (!context) {
        printf("RegisterAllocationContext: NULL\n");
        return;
    }
    
    printf("RegisterAllocationContext:\n");
    printf("  Strategy: %d\n", context->strategy);
    printf("  Physical Registers: %zu\n", context->physical_register_count);
    printf("  Spill Slots: %zu\n", context->spill_slot_count);
    printf("  Total Allocations: %zu\n", context->total_allocations);
    printf("  Spill Count: %zu\n", context->spill_count);
    printf("  Move Count: %zu\n", context->move_count);
    printf("  Allocation Time: %.2f ms\n", context->allocation_time_ms);
}

void register_print_lifetimes(RegisterAllocationContext* context) {
    if (!context || !context->lifetimes) {
        printf("RegisterLifetimes: NULL\n");
        return;
    }
    
    printf("RegisterLifetimes:\n");
    for (size_t i = 0; i < context->lifetime_count; i++) {
        RegisterLifetime* lifetime = &context->lifetimes[i];
        printf("  Reg %zu: def=%zu, last_use=%zu, live_range=[%zu,%zu], "
               "phys_reg=%d, spilled=%d, spill_slot=%d, interferences=%d\n",
               lifetime->virtual_register,
               lifetime->definition_point,
               lifetime->last_use_point,
               lifetime->live_range_start,
               lifetime->live_range_end,
               lifetime->physical_register,
               lifetime->is_spilled,
               lifetime->spill_slot,
               lifetime->interference_count);
    }
}

int register_validate_allocation(RegisterAllocationContext* context) {
    if (!context || !context->lifetimes) return 0;
    
    // TODO: Implement allocation validation
    // This would validate that the allocation is correct and consistent
    
    return 1;
}

void register_set_allocation_strategy(RegisterAllocationContext* context, RegisterAllocationStrategy strategy) {
    if (!context) return;
    context->strategy = strategy;
}

void register_set_optimization_preferences(RegisterAllocationContext* context, 
                                         int optimize_for_speed, 
                                         int allow_spilling, 
                                         int prioritize_hot) {
    if (!context) return;
    context->optimize_for_speed = optimize_for_speed;
    context->allow_spilling = allow_spilling;
    context->prioritize_hot_registers = prioritize_hot;
}

void register_set_physical_register_count(RegisterAllocationContext* context, size_t count) {
    if (!context) return;
    context->physical_register_count = count;
}

void register_set_spill_slot_count(RegisterAllocationContext* context, size_t count) {
    if (!context) return;
    context->spill_slot_count = count;
}
