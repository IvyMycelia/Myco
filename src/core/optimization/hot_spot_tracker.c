#include "../../include/core/optimization/hot_spot_tracker.h"
#include "../../include/core/interpreter/interpreter_core.h"
#include "../../include/core/ast.h"
#include "../../include/utils/shared_utilities.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

// ============================================================================
// HOT SPOT TRACKER IMPLEMENTATION
// ============================================================================

HotSpotTracker* hot_spot_tracker_create(void) {
    HotSpotTracker* tracker = (HotSpotTracker*)shared_malloc_safe(
        sizeof(HotSpotTracker), "hot_spot", "tracker_create", 0);
    if (!tracker) return NULL;
    
    // Initialize counters hash map
    tracker->counters.keys = NULL;
    tracker->counters.values = NULL;
    tracker->counters.count = 0;
    tracker->counters.capacity = 0;
    
    // Initialize hot spots hash map
    tracker->hot_spots.keys = NULL;
    tracker->hot_spots.values = NULL;
    tracker->hot_spots.count = 0;
    tracker->hot_spots.capacity = 0;
    
    // Initialize configuration
    tracker->hot_threshold = HOT_SPOT_THRESHOLD_DEFAULT;
    tracker->adaptive_threshold = HOT_SPOT_THRESHOLD_DEFAULT;
    tracker->enable_adaptive = 1;
    tracker->enable_profiling = 1;
    
    // Initialize statistics
    tracker->total_executions = 0;
    tracker->hot_spot_count = 0;
    tracker->optimization_count = 0;
    tracker->deoptimization_count = 0;
    tracker->profiling_overhead_ns = 0;
    tracker->last_cleanup_ns = get_current_time_ns();
    
    return tracker;
}

void hot_spot_tracker_free(HotSpotTracker* tracker) {
    if (!tracker) return;
    
    // Free counters
    if (tracker->counters.keys) {
        shared_free_safe(tracker->counters.keys, "hot_spot", "tracker_free", 0);
    }
    if (tracker->counters.values) {
        for (size_t i = 0; i < tracker->counters.count; i++) {
            execution_counter_free(&tracker->counters.values[i]);
        }
        shared_free_safe(tracker->counters.values, "hot_spot", "tracker_free", 0);
    }
    
    // Free hot spots
    if (tracker->hot_spots.keys) {
        shared_free_safe(tracker->hot_spots.keys, "hot_spot", "tracker_free", 0);
    }
    if (tracker->hot_spots.values) {
        for (size_t i = 0; i < tracker->hot_spots.count; i++) {
            hot_spot_info_free(&tracker->hot_spots.values[i]);
        }
        shared_free_safe(tracker->hot_spots.values, "hot_spot", "tracker_free", 0);
    }
    
    shared_free_safe(tracker, "hot_spot", "tracker_free", 0);
}

void hot_spot_tracker_reset(HotSpotTracker* tracker) {
    if (!tracker) return;
    
    // Reset all counters
    for (size_t i = 0; i < tracker->counters.count; i++) {
        ExecutionCounter* counter = &tracker->counters.values[i];
        counter->execution_count = 0;
        counter->total_time_ns = 0;
        counter->last_execution_ns = 0;
        counter->peak_time_ns = 0;
        counter->avg_time_ns = 0;
        counter->is_hot = 0;
        counter->optimization_level = 0;
        counter->deoptimization_count = 0;
    }
    
    // Reset statistics
    tracker->total_executions = 0;
    tracker->hot_spot_count = 0;
    tracker->optimization_count = 0;
    tracker->deoptimization_count = 0;
    tracker->profiling_overhead_ns = 0;
    tracker->last_cleanup_ns = get_current_time_ns();
}

// ============================================================================
// CONFIGURATION FUNCTIONS
// ============================================================================

void hot_spot_tracker_set_threshold(HotSpotTracker* tracker, uint64_t threshold) {
    if (!tracker) return;
    
    if (threshold < HOT_SPOT_THRESHOLD_MIN) {
        threshold = HOT_SPOT_THRESHOLD_MIN;
    } else if (threshold > HOT_SPOT_THRESHOLD_MAX) {
        threshold = HOT_SPOT_THRESHOLD_MAX;
    }
    
    tracker->hot_threshold = threshold;
    tracker->adaptive_threshold = threshold;
}

void hot_spot_tracker_set_adaptive(HotSpotTracker* tracker, int enabled) {
    if (tracker) {
        tracker->enable_adaptive = enabled ? 1 : 0;
    }
}

void hot_spot_tracker_set_profiling(HotSpotTracker* tracker, int enabled) {
    if (tracker) {
        tracker->enable_profiling = enabled ? 1 : 0;
    }
}

uint64_t hot_spot_tracker_get_threshold(HotSpotTracker* tracker) {
    return tracker ? tracker->hot_threshold : HOT_SPOT_THRESHOLD_DEFAULT;
}

// ============================================================================
// HASH MAP OPERATIONS
// ============================================================================

// Simple hash map implementation for AST node -> counter mapping
static ExecutionCounter* find_counter(HotSpotTracker* tracker, ASTNode* node) {
    if (!tracker || !node) return NULL;
    
    uint64_t hash = ast_node_hash(node);
    size_t index = hash % tracker->counters.capacity;
    
    // Linear probing
    for (size_t i = 0; i < tracker->counters.capacity; i++) {
        size_t probe_index = (index + i) % tracker->counters.capacity;
        if (tracker->counters.keys[probe_index] == node) {
            return &tracker->counters.values[probe_index];
        }
        if (tracker->counters.keys[probe_index] == NULL) {
            break; // Not found
        }
    }
    
    return NULL;
}

static ExecutionCounter* get_or_create_counter(HotSpotTracker* tracker, ASTNode* node) {
    if (!tracker || !node) return NULL;
    
    // Check if counter exists
    ExecutionCounter* counter = find_counter(tracker, node);
    if (counter) return counter;
    
    // Grow hash map if needed
    if (tracker->counters.count >= tracker->counters.capacity) {
        size_t new_capacity = tracker->counters.capacity == 0 ? 16 : tracker->counters.capacity * 2;
        ASTNode** new_keys = (ASTNode**)shared_malloc_safe(
            new_capacity * sizeof(ASTNode*), "hot_spot", "get_or_create_counter", 0);
        ExecutionCounter* new_values = (ExecutionCounter*)shared_malloc_safe(
            new_capacity * sizeof(ExecutionCounter), "hot_spot", "get_or_create_counter", 0);
        
        if (!new_keys || !new_values) {
            if (new_keys) shared_free_safe(new_keys, "hot_spot", "get_or_create_counter", 0);
            if (new_values) shared_free_safe(new_values, "hot_spot", "get_or_create_counter", 0);
            return NULL;
        }
        
        // Initialize new arrays
        memset(new_keys, 0, new_capacity * sizeof(ASTNode*));
        memset(new_values, 0, new_capacity * sizeof(ExecutionCounter));
        
        // Rehash existing entries
        for (size_t i = 0; i < tracker->counters.capacity; i++) {
            if (tracker->counters.keys[i]) {
                uint64_t hash = ast_node_hash(tracker->counters.keys[i]);
                size_t new_index = hash % new_capacity;
                
                // Find empty slot
                while (new_keys[new_index] != NULL) {
                    new_index = (new_index + 1) % new_capacity;
                }
                
                new_keys[new_index] = tracker->counters.keys[i];
                new_values[new_index] = tracker->counters.values[i];
            }
        }
        
        // Free old arrays
        if (tracker->counters.keys) {
            shared_free_safe(tracker->counters.keys, "hot_spot", "get_or_create_counter", 0);
        }
        if (tracker->counters.values) {
            shared_free_safe(tracker->counters.values, "hot_spot", "get_or_create_counter", 0);
        }
        
        tracker->counters.keys = new_keys;
        tracker->counters.values = new_values;
        tracker->counters.capacity = new_capacity;
    }
    
    // Find empty slot for new entry
    uint64_t hash = ast_node_hash(node);
    size_t index = hash % tracker->counters.capacity;
    
    while (tracker->counters.keys[index] != NULL) {
        index = (index + 1) % tracker->counters.capacity;
    }
    
    // Add new entry
    tracker->counters.keys[index] = node;
    ExecutionCounter* new_counter = &tracker->counters.values[index];
    memset(new_counter, 0, sizeof(ExecutionCounter));
    tracker->counters.count++;
    
    return new_counter;
}

// ============================================================================
// EXECUTION TRACKING
// ============================================================================

void hot_spot_tracker_record_execution(HotSpotTracker* tracker, ASTNode* node, uint64_t execution_time_ns) {
    if (!tracker || !tracker->enable_profiling) return;
    
    // Allow NULL node for simplified tracking
    if (!node) return;
    
    uint64_t start_time = get_current_time_ns();
    
    ExecutionCounter* counter = get_or_create_counter(tracker, node);
    if (!counter) return;
    
    // Update counter
    counter->execution_count++;
    counter->total_time_ns += execution_time_ns;
    counter->last_execution_ns = get_current_time_ns();
    
    if (execution_time_ns > counter->peak_time_ns) {
        counter->peak_time_ns = execution_time_ns;
    }
    
    counter->avg_time_ns = counter->total_time_ns / counter->execution_count;
    
    // Check if this is now a hot spot
    uint64_t threshold = tracker->enable_adaptive ? tracker->adaptive_threshold : tracker->hot_threshold;
    if (counter->execution_count >= threshold && !counter->is_hot) {
        counter->is_hot = 1;
        tracker->hot_spot_count++;
        
        // Mark as hot spot
        hot_spot_tracker_mark_hot(tracker, node, HOT_SPOT_EXPRESSION);
    }
    
    tracker->total_executions++;
    
    // Record profiling overhead
    uint64_t end_time = get_current_time_ns();
    tracker->profiling_overhead_ns += (end_time - start_time);
}

void hot_spot_tracker_record_function_call(HotSpotTracker* tracker, ASTNode* func_node, Value* args, size_t arg_count, uint64_t execution_time_ns) {
    if (!tracker || !func_node) return;
    
    // Record general execution
    hot_spot_tracker_record_execution(tracker, func_node, execution_time_ns);
    
    // Record parameter types for specialization
    if (args && arg_count > 0) {
        hot_spot_tracker_record_parameter_types(tracker, func_node, args, arg_count);
    }
}

void hot_spot_tracker_record_loop_iteration(HotSpotTracker* tracker, ASTNode* loop_node, uint64_t iteration_time_ns) {
    if (!tracker || !loop_node) return;
    
    // Record general execution
    hot_spot_tracker_record_execution(tracker, loop_node, iteration_time_ns);
    
    // Update loop-specific data
    HotSpotInfo* info = hot_spot_tracker_get_info(tracker, loop_node);
    if (info) {
        info->loop_iterations++;
        info->avg_iterations = info->loop_iterations / info->type; // Rough approximation
        if (info->loop_iterations > info->max_iterations) {
            info->max_iterations = info->loop_iterations;
        }
    }
}

void hot_spot_tracker_record_expression(HotSpotTracker* tracker, ASTNode* expr_node, uint64_t execution_time_ns) {
    if (!tracker || !expr_node) return;
    
    hot_spot_tracker_record_execution(tracker, expr_node, execution_time_ns);
}

// ============================================================================
// HOT SPOT DETECTION
// ============================================================================

int hot_spot_tracker_is_hot(HotSpotTracker* tracker, ASTNode* node) {
    if (!tracker || !node) return 0;
    
    ExecutionCounter* counter = find_counter(tracker, node);
    return counter ? counter->is_hot : 0;
}

HotSpotInfo* hot_spot_tracker_get_info(HotSpotTracker* tracker, ASTNode* node) {
    if (!tracker || !node) return NULL;
    
    // For now, return NULL - hot spot info will be implemented later
    return NULL;
}

ExecutionCounter* hot_spot_tracker_get_counter(HotSpotTracker* tracker, ASTNode* node) {
    if (!tracker || !node) return NULL;
    
    return find_counter(tracker, node);
}

// ============================================================================
// HOT SPOT MANAGEMENT
// ============================================================================

void hot_spot_tracker_mark_hot(HotSpotTracker* tracker, ASTNode* node, HotSpotType type) {
    if (!tracker || !node) return;
    
    // Implementation will be added in later phases
    // For now, just mark the counter as hot
    ExecutionCounter* counter = find_counter(tracker, node);
    if (counter) {
        counter->is_hot = 1;
    }
}

void hot_spot_tracker_mark_cold(HotSpotTracker* tracker, ASTNode* node) {
    if (!tracker || !node) return;
    
    ExecutionCounter* counter = find_counter(tracker, node);
    if (counter && counter->is_hot) {
        counter->is_hot = 0;
        tracker->hot_spot_count--;
    }
}

void hot_spot_tracker_update_optimization_level(HotSpotTracker* tracker, ASTNode* node, int level) {
    if (!tracker || !node) return;
    
    ExecutionCounter* counter = find_counter(tracker, node);
    if (counter) {
        counter->optimization_level = level;
        if (level > 0) {
            tracker->optimization_count++;
        }
    }
}

// ============================================================================
// TYPE SPECIALIZATION TRACKING
// ============================================================================

void hot_spot_tracker_record_parameter_types(HotSpotTracker* tracker, ASTNode* func_node, Value* args, size_t arg_count) {
    if (!tracker || !func_node || !args) return;
    
    // Implementation will be added in Phase 5 (Value Specialization)
    // For now, this is a placeholder
}

void hot_spot_tracker_record_return_type(HotSpotTracker* tracker, ASTNode* func_node, ValueType return_type) {
    if (!tracker || !func_node) return;
    
    // Implementation will be added in Phase 5 (Value Specialization)
    // For now, this is a placeholder
}

void hot_spot_tracker_record_branch_taken(HotSpotTracker* tracker, ASTNode* branch_node, int taken) {
    if (!tracker || !branch_node) return;
    
    // Implementation will be added in Phase 5 (Value Specialization)
    // For now, this is a placeholder
}

// ============================================================================
// PERFORMANCE ANALYSIS
// ============================================================================

void hot_spot_tracker_analyze_performance(HotSpotTracker* tracker) {
    if (!tracker) return;
    
    // Analyze all hot spots and update optimization levels
    for (size_t i = 0; i < tracker->counters.capacity; i++) {
        if (tracker->counters.keys[i] && tracker->counters.values[i].is_hot) {
            ExecutionCounter* counter = &tracker->counters.values[i];
            
            // Simple analysis: increase optimization level based on execution count
            if (counter->execution_count > tracker->hot_threshold * 10) {
                hot_spot_tracker_update_optimization_level(tracker, tracker->counters.keys[i], 2);
            } else if (counter->execution_count > tracker->hot_threshold * 2) {
                hot_spot_tracker_update_optimization_level(tracker, tracker->counters.keys[i], 1);
            }
        }
    }
}

void hot_spot_tracker_cleanup_cold_spots(HotSpotTracker* tracker) {
    if (!tracker) return;
    
    uint64_t current_time = get_current_time_ns();
    uint64_t cleanup_interval = 1000000000; // 1 second in nanoseconds
    
    if (current_time - tracker->last_cleanup_ns < cleanup_interval) {
        return; // Too soon for cleanup
    }
    
    // Mark cold spots as not hot
    for (size_t i = 0; i < tracker->counters.capacity; i++) {
        if (tracker->counters.keys[i]) {
            ExecutionCounter* counter = &tracker->counters.values[i];
            if (counter->is_hot && counter->execution_count < tracker->hot_threshold / 2) {
                hot_spot_tracker_mark_cold(tracker, tracker->counters.keys[i]);
            }
        }
    }
    
    tracker->last_cleanup_ns = current_time;
}

void hot_spot_tracker_optimize_thresholds(HotSpotTracker* tracker) {
    if (!tracker || !tracker->enable_adaptive) return;
    
    // Simple adaptive thresholding
    if (tracker->hot_spot_count > 50) {
        // Too many hot spots, increase threshold
        tracker->adaptive_threshold = (uint64_t)(tracker->adaptive_threshold * 1.1);
    } else if (tracker->hot_spot_count < 5) {
        // Too few hot spots, decrease threshold
        tracker->adaptive_threshold = (uint64_t)(tracker->adaptive_threshold * 0.9);
    }
    
    // Keep within bounds
    if (tracker->adaptive_threshold < HOT_SPOT_THRESHOLD_MIN) {
        tracker->adaptive_threshold = HOT_SPOT_THRESHOLD_MIN;
    } else if (tracker->adaptive_threshold > HOT_SPOT_THRESHOLD_MAX) {
        tracker->adaptive_threshold = HOT_SPOT_THRESHOLD_MAX;
    }
}

// ============================================================================
// STATISTICS AND REPORTING
// ============================================================================

uint64_t hot_spot_tracker_get_total_executions(HotSpotTracker* tracker) {
    return tracker ? tracker->total_executions : 0;
}

uint64_t hot_spot_tracker_get_hot_spot_count(HotSpotTracker* tracker) {
    return tracker ? tracker->hot_spot_count : 0;
}

uint64_t hot_spot_tracker_get_optimization_count(HotSpotTracker* tracker) {
    return tracker ? tracker->optimization_count : 0;
}

double hot_spot_tracker_get_avg_speedup(HotSpotTracker* tracker) {
    if (!tracker || tracker->optimization_count == 0) return 1.0;
    
    // Simple calculation - will be improved in later phases
    return 2.0; // Placeholder
}

void hot_spot_tracker_print_statistics(HotSpotTracker* tracker) {
    if (!tracker) return;
    
    printf("Hot Spot Tracker Statistics:\n");
    printf("  Total executions: %llu\n", (unsigned long long)tracker->total_executions);
    printf("  Hot spots detected: %llu\n", (unsigned long long)tracker->hot_spot_count);
    printf("  Optimizations applied: %llu\n", (unsigned long long)tracker->optimization_count);
    printf("  Deoptimizations: %llu\n", (unsigned long long)tracker->deoptimization_count);
    printf("  Current threshold: %llu\n", (unsigned long long)tracker->hot_threshold);
    printf("  Adaptive threshold: %llu\n", (unsigned long long)tracker->adaptive_threshold);
    printf("  Profiling overhead: %llu ns\n", (unsigned long long)tracker->profiling_overhead_ns);
    printf("  Average speedup: %.2fx\n", hot_spot_tracker_get_avg_speedup(tracker));
}

void hot_spot_tracker_print_hot_spots(HotSpotTracker* tracker) {
    if (!tracker) return;
    
    printf("Hot Spots:\n");
    for (size_t i = 0; i < tracker->counters.capacity; i++) {
        if (tracker->counters.keys[i] && tracker->counters.values[i].is_hot) {
            ExecutionCounter* counter = &tracker->counters.values[i];
            printf("  Node %p: %llu executions, %llu ns avg, level %d\n",
                   (void*)tracker->counters.keys[i],
                   (unsigned long long)counter->execution_count,
                   (unsigned long long)counter->avg_time_ns,
                   counter->optimization_level);
        }
    }
}

// ============================================================================
// INTERPRETER INTEGRATION
// ============================================================================

void hot_spot_tracker_initialize_for_interpreter(Interpreter* interpreter) {
    if (!interpreter) return;
    
    // Add hot spot tracker to interpreter
    // This will be implemented when we modify the Interpreter struct
}

void hot_spot_tracker_cleanup_for_interpreter(Interpreter* interpreter) {
    if (!interpreter) return;
    
    // Cleanup hot spot tracker from interpreter
    // This will be implemented when we modify the Interpreter struct
}

HotSpotTracker* hot_spot_tracker_get_from_interpreter(Interpreter* interpreter) {
    if (!interpreter) return NULL;
    
    // Return hot spot tracker from interpreter
    // This will be implemented when we modify the Interpreter struct
    return NULL;
}

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

uint64_t ast_node_hash(ASTNode* node) {
    if (!node) return 0;
    
    // Simple hash based on node type and line number
    return (uint64_t)node->type * 31 + (uint64_t)node->line;
}

int ast_node_equals(ASTNode* a, ASTNode* b) {
    if (!a || !b) return a == b;
    return a == b; // Pointer comparison for now
}

uint64_t get_current_time_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

uint64_t calculate_execution_time_ns(uint64_t start_time, uint64_t end_time) {
    return end_time > start_time ? end_time - start_time : 0;
}

int should_optimize_hot_spot(HotSpotInfo* info) {
    if (!info) return 0;
    
    // Simple heuristic - will be improved in later phases
    return info->stability_score > 50;
}

int is_hot_spot_stable(HotSpotInfo* info) {
    if (!info) return 0;
    
    // Simple heuristic - will be improved in later phases
    return info->stability_score > 70;
}

double calculate_optimization_potential(HotSpotInfo* info) {
    if (!info) return 0.0;
    
    // Simple calculation - will be improved in later phases
    return 1.5; // Placeholder
}

void hot_spot_info_free(HotSpotInfo* info) {
    if (!info) return;
    
    if (info->parameter_types) {
        shared_free_safe(info->parameter_types, "hot_spot", "info_free", 0);
    }
    if (info->branch_taken) {
        shared_free_safe(info->branch_taken, "hot_spot", "info_free", 0);
    }
    if (info->branch_not_taken) {
        shared_free_safe(info->branch_not_taken, "hot_spot", "info_free", 0);
    }
}

void execution_counter_free(ExecutionCounter* counter) {
    // ExecutionCounter doesn't have dynamic memory, so nothing to free
    (void)counter; // Suppress unused parameter warning
}
