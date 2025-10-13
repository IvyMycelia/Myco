/**
 * @file register_allocator.h
 * @brief Advanced register allocation for Myco register VM
 * 
 * This module implements sophisticated register allocation algorithms for the
 * Myco register VM, including linear scan allocation, lifetime analysis, and
 * spill code generation. It optimizes register usage to maximize performance
 * and minimize memory access overhead.
 * 
 * Key Features:
 * - Linear scan register allocation (fast and effective)
 * - Lifetime analysis for optimal register reuse
 * - Spill code minimization
 * - Hot variable prioritization
 * - SSA-form intermediate representation support
 * 
 * Architecture:
 * - Supports 256 virtual registers per frame
 * - Optimizes for register pressure reduction
 * - Generates efficient spill code when needed
 * - Integrates with trace-based JIT compilation
 * 
 * @author Myco Optimization Team
 * @version 2.0
 * @date 2024
 */

#ifndef REGISTER_ALLOCATOR_H
#define REGISTER_ALLOCATOR_H

#include "register_vm.h"
#include "../interpreter/interpreter_core.h"
#include <stdint.h>
#include <stddef.h>

// ============================================================================
// REGISTER ALLOCATION DATA STRUCTURES
// ============================================================================

/**
 * @brief Register allocation strategy
 * 
 * Defines the strategy used for register allocation. Different strategies
 * are optimized for different scenarios and performance requirements.
 */
typedef enum {
    REG_ALLOC_LINEAR_SCAN = 0,     // Linear scan allocation (default)
    REG_ALLOC_GRAPH_COLORING = 1,  // Graph coloring allocation
    REG_ALLOC_ITERATED_COALESCING = 2, // Iterated coalescing
    REG_ALLOC_OPTIMAL = 3,         // Optimal allocation (slow but best)
    REG_ALLOC_FAST = 4             // Fast allocation (quick but suboptimal)
} RegisterAllocationStrategy;

/**
 * @brief Register lifetime information
 * 
 * Tracks the lifetime of a virtual register, including its definition,
 * uses, and interference with other registers.
 */
typedef struct {
    size_t virtual_register;       // Virtual register number
    size_t definition_point;       // Instruction where register is defined
    size_t last_use_point;         // Instruction where register is last used
    size_t live_range_start;       // Start of live range
    size_t live_range_end;         // End of live range
    int is_hot;                    // Is this register in a hot path?
    int is_spilled;                // Has this register been spilled?
    int physical_register;         // Assigned physical register (-1 if spilled)
    int spill_slot;                // Spill slot number (-1 if not spilled)
    double hotness_score;          // Hotness score (0.0-1.0)
    int interference_count;        // Number of interfering registers
    size_t* interfering_registers; // Array of interfering register numbers
} RegisterLifetime;

/**
 * @brief Register allocation context
 * 
 * Contains all information needed for register allocation, including
 * the program being allocated, lifetime information, and allocation state.
 */
typedef struct {
    RegisterProgram* program;      // Program being allocated
    RegisterLifetime* lifetimes;   // Lifetime information for each register
    size_t lifetime_count;         // Number of lifetimes
    
    int* physical_registers;       // Physical register allocation
    size_t physical_register_count; // Number of physical registers available
    int* spill_slots;              // Spill slot allocation
    size_t spill_slot_count;       // Number of spill slots used
    
    RegisterAllocationStrategy strategy; // Allocation strategy
    int optimize_for_speed;        // Optimize for speed vs. quality
    int allow_spilling;            // Allow register spilling
    int prioritize_hot_registers;  // Prioritize hot registers
    
    // Statistics
    size_t total_allocations;      // Total number of allocations
    size_t spill_count;            // Number of registers spilled
    size_t move_count;             // Number of move instructions generated
    double allocation_time_ms;     // Time taken for allocation (ms)
} RegisterAllocationContext;

/**
 * @brief Register allocation result
 * 
 * Contains the results of register allocation, including the final
 * allocation, spill information, and performance metrics.
 */
typedef struct {
    int success;                   // Allocation success flag
    size_t physical_register_count; // Number of physical registers used
    size_t spill_slot_count;       // Number of spill slots used
    size_t move_instruction_count; // Number of move instructions generated
    double allocation_quality;     // Allocation quality score (0.0-1.0)
    double estimated_performance;  // Estimated performance improvement
    char* error_message;           // Error message (if allocation failed)
} RegisterAllocationResult;

// ============================================================================
// REGISTER ALLOCATION FUNCTIONS
// ============================================================================

/**
 * @brief Core allocation functions
 * 
 * These functions handle the main register allocation process,
 * from analysis to final allocation.
 */

/**
 * @brief Create a new register allocation context
 * @param program Program to allocate registers for
 * @param strategy Allocation strategy to use
 * @return New RegisterAllocationContext, or NULL on failure
 * @note The context is initialized with default settings
 */
RegisterAllocationContext* register_allocator_create(RegisterProgram* program, RegisterAllocationStrategy strategy);

/**
 * @brief Free a register allocation context
 * @param context Context to free (can be NULL)
 * @note This function is safe to call with NULL
 */
void register_allocator_free(RegisterAllocationContext* context);

/**
 * @brief Perform register allocation on a program
 * @param context Allocation context
 * @return Allocation result with success status and metrics
 * @note This is the main entry point for register allocation
 */
RegisterAllocationResult register_allocate(RegisterAllocationContext* context);

/**
 * @brief Analyze register lifetimes in a program
 * @param context Allocation context
 * @return 1 on success, 0 on failure
 * @note This function must be called before allocation
 */
int register_analyze_lifetimes(RegisterAllocationContext* context);

/**
 * @brief Build interference graph for registers
 * @param context Allocation context
 * @return 1 on success, 0 on failure
 * @note This function builds the interference graph used for allocation
 */
int register_build_interference_graph(RegisterAllocationContext* context);

/**
 * @brief Linear scan register allocation
 * @param context Allocation context
 * @return 1 on success, 0 on failure
 * @note Fast and effective allocation algorithm
 */
int register_linear_scan_allocate(RegisterAllocationContext* context);

/**
 * @brief Graph coloring register allocation
 * @param context Allocation context
 * @return 1 on success, 0 on failure
 * @note Higher quality but slower allocation algorithm
 */
int register_graph_coloring_allocate(RegisterAllocationContext* context);

/**
 * @brief Spill code generation functions
 * 
 * These functions handle the generation of spill code when registers
 * cannot be allocated to physical registers.
 */

/**
 * @brief Generate spill code for a register
 * @param context Allocation context
 * @param virtual_register Virtual register to spill
 * @return 1 on success, 0 on failure
 * @note Generates load/store instructions for spilled registers
 */
int register_generate_spill_code(RegisterAllocationContext* context, size_t virtual_register);

/**
 * @brief Generate move instructions for register coalescing
 * @param context Allocation context
 * @param src_register Source register
 * @param dst_register Destination register
 * @return 1 on success, 0 on failure
 * @note Generates move instructions to eliminate unnecessary copies
 */
int register_generate_move_code(RegisterAllocationContext* context, size_t src_register, size_t dst_register);

/**
 * @brief Optimize spill code placement
 * @param context Allocation context
 * @return 1 on success, 0 on failure
 * @note Optimizes the placement of spill code for better performance
 */
int register_optimize_spill_placement(RegisterAllocationContext* context);

/**
 * @brief Analysis functions
 * 
 * These functions analyze programs and registers to provide information
 * for register allocation decisions.
 */

/**
 * @brief Analyze register pressure in a program
 * @param context Allocation context
 * @return Maximum register pressure encountered
 * @note Returns the maximum number of live registers at any point
 */
size_t register_analyze_pressure(RegisterAllocationContext* context);

/**
 * @brief Analyze hot spots in register usage
 * @param context Allocation context
 * @return 1 on success, 0 on failure
 * @note Identifies hot spots where register allocation is critical
 */
int register_analyze_hot_spots(RegisterAllocationContext* context);

/**
 * @brief Calculate register interference
 * @param context Allocation context
 * @param reg1 First register
 * @param reg2 Second register
 * @return 1 if registers interfere, 0 otherwise
 * @note Two registers interfere if they are live at the same time
 */
int register_calculate_interference(RegisterAllocationContext* context, size_t reg1, size_t reg2);

/**
 * @brief Find optimal spill candidates
 * @param context Allocation context
 * @param candidates Array to store spill candidates
 * @param max_candidates Maximum number of candidates to find
 * @return Number of spill candidates found
 * @note Identifies the best registers to spill when allocation fails
 */
size_t register_find_spill_candidates(RegisterAllocationContext* context, size_t* candidates, size_t max_candidates);

/**
 * @brief Optimization functions
 * 
 * These functions optimize register allocation for better performance
 * and code quality.
 */

/**
 * @brief Optimize register allocation for hot paths
 * @param context Allocation context
 * @return 1 on success, 0 on failure
 * @note Prioritizes hot registers for better performance
 */
int register_optimize_hot_paths(RegisterAllocationContext* context);

/**
 * @brief Coalesce move instructions
 * @param context Allocation context
 * @return Number of moves coalesced
 * @note Eliminates unnecessary move instructions
 */
int register_coalesce_moves(RegisterAllocationContext* context);

/**
 * @brief Optimize spill slot usage
 * @param context Allocation context
 * @return 1 on success, 0 on failure
 * @note Optimizes the use of spill slots to minimize memory access
 */
int register_optimize_spill_slots(RegisterAllocationContext* context);

/**
 * @brief Apply register allocation hints
 * @param context Allocation context
 * @return 1 on success, 0 on failure
 * @note Applies hints from the compiler to improve allocation
 */
int register_apply_allocation_hints(RegisterAllocationContext* context);

/**
 * @brief Utility functions
 * 
 * These functions provide utility functionality for working with
 * register allocation contexts and results.
 */

/**
 * @brief Get the physical register assigned to a virtual register
 * @param context Allocation context
 * @param virtual_register Virtual register number
 * @return Physical register number, or -1 if spilled
 * @note Returns -1 if the register was spilled to memory
 */
int register_get_physical_register(RegisterAllocationContext* context, size_t virtual_register);

/**
 * @brief Get the spill slot assigned to a virtual register
 * @param context Allocation context
 * @param virtual_register Virtual register number
 * @return Spill slot number, or -1 if not spilled
 * @note Returns -1 if the register was allocated to a physical register
 */
int register_get_spill_slot(RegisterAllocationContext* context, size_t virtual_register);

/**
 * @brief Check if a register was spilled
 * @param context Allocation context
 * @param virtual_register Virtual register number
 * @return 1 if spilled, 0 otherwise
 * @note Returns 1 if the register was spilled to memory
 */
int register_is_spilled(RegisterAllocationContext* context, size_t virtual_register);

/**
 * @brief Get allocation statistics
 * @param context Allocation context
 * @return Allocation statistics string
 * @note Returns a formatted string with allocation statistics
 */
char* register_get_allocation_stats(RegisterAllocationContext* context);

/**
 * @brief Print register allocation results
 * @param context Allocation context
 * @note Prints human-readable representation of allocation results
 */
void register_print_allocation(RegisterAllocationContext* context);

/**
 * @brief Print register lifetime information
 * @param context Allocation context
 * @note Prints human-readable representation of register lifetimes
 */
void register_print_lifetimes(RegisterAllocationContext* context);

/**
 * @brief Validate register allocation
 * @param context Allocation context
 * @return 1 if valid, 0 if invalid
 * @note Validates that the allocation is correct and consistent
 */
int register_validate_allocation(RegisterAllocationContext* context);

/**
 * @brief Configuration functions
 * 
 * These functions configure register allocation behavior and settings.
 */

/**
 * @brief Set allocation strategy
 * @param context Allocation context
 * @param strategy New allocation strategy
 * @note Changes the allocation strategy for the context
 */
void register_set_allocation_strategy(RegisterAllocationContext* context, RegisterAllocationStrategy strategy);

/**
 * @brief Set optimization preferences
 * @param context Allocation context
 * @param optimize_for_speed 1 to optimize for speed, 0 for quality
 * @param allow_spilling 1 to allow spilling, 0 to disallow
 * @param prioritize_hot 1 to prioritize hot registers, 0 otherwise
 * @note Configures optimization preferences for allocation
 */
void register_set_optimization_preferences(RegisterAllocationContext* context, 
                                         int optimize_for_speed, 
                                         int allow_spilling, 
                                         int prioritize_hot);

/**
 * @brief Set physical register count
 * @param context Allocation context
 * @param count Number of physical registers available
 * @note Sets the number of physical registers available for allocation
 */
void register_set_physical_register_count(RegisterAllocationContext* context, size_t count);

/**
 * @brief Set spill slot count
 * @param context Allocation context
 * @param count Number of spill slots available
 * @note Sets the number of spill slots available for spilled registers
 */
void register_set_spill_slot_count(RegisterAllocationContext* context, size_t count);

#endif // REGISTER_ALLOCATOR_H
