#include "../../include/core/optimization/adaptive_executor.h"
#include "../../include/core/interpreter/interpreter_core.h"
#include "../../include/core/ast.h"
#include "../../include/core/optimization/bytecode_engine.h"
#include "../../include/core/optimization/hot_spot_tracker.h"
#include "../../include/core/optimization/micro_jit.h"
#include "../../include/core/optimization/value_specializer.h"
#include "../../include/core/interpreter/value_operations.h"
#include "../../include/core/interpreter/eval_engine.h"
#include "../../include/utils/shared_utilities.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

// ============================================================================
// ADAPTIVE EXECUTOR IMPLEMENTATION
// ============================================================================

AdaptiveExecutor* adaptive_executor_create(Interpreter* interpreter) {
    if (!interpreter) return NULL;
    
    AdaptiveExecutor* executor = (AdaptiveExecutor*)shared_malloc_safe(
        sizeof(AdaptiveExecutor), "adaptive_executor", "create", 0);
    if (!executor) return NULL;
    
    // Initialize core components
    executor->hot_spot_tracker = (HotSpotTracker*)interpreter->hot_spot_tracker;
    executor->micro_jit_context = (MicroJitContext*)interpreter->micro_jit_context;
    executor->value_specializer = (ValueSpecializer*)interpreter->value_specializer;
    
    // Initialize execution state
    executor->current_tier = EXECUTION_TIER_AST;
    executor->optimization_level = OPTIMIZATION_LEVEL_BASIC;
    executor->optimization_enabled = 1;
    executor->adaptive_mode = 1;
    
    // Initialize statistics
    memset(&executor->stats, 0, sizeof(ExecutionStatistics));
    executor->stats.overall_speedup = 1.0;
    executor->stats.bytecode_speedup = 1.0;
    executor->stats.jit_speedup = 1.0;
    executor->stats.specialized_speedup = 1.0;
    
    // Initialize policy with defaults
    executor->policy = *adaptive_executor_get_default_policy();
    
    // Initialize decision making
    executor->last_decision_time_ns = 0;
    executor->last_decision_reason = DECISION_FIRST_EXECUTION;
    executor->last_optimized_node = NULL;
    
    // Initialize resource management
    executor->total_code_size = 0;
    executor->total_specializations = 0;
    executor->total_memory_usage = 0;
    
    // Initialize performance tracking
    executor->total_execution_time_ns = 0;
    executor->total_compilation_time_ns = 0;
    executor->efficiency_ratio = 1.0;
    
    // Initialize error handling
    executor->error_count = 0;
    executor->consecutive_failures = 0;
    executor->last_error_time_ns = 0;
    
    return executor;
}

void adaptive_executor_free(AdaptiveExecutor* executor) {
    if (!executor) return;
    
    // Free resources
    adaptive_executor_cleanup_resources(executor);
    
    shared_free_safe(executor, "adaptive_executor", "free", 0);
}

void adaptive_executor_reset(AdaptiveExecutor* executor) {
    if (!executor) return;
    
    // Reset statistics
    memset(&executor->stats, 0, sizeof(ExecutionStatistics));
    executor->stats.overall_speedup = 1.0;
    executor->stats.bytecode_speedup = 1.0;
    executor->stats.jit_speedup = 1.0;
    executor->stats.specialized_speedup = 1.0;
    
    // Reset execution state
    executor->current_tier = EXECUTION_TIER_AST;
    executor->last_decision_time_ns = 0;
    executor->last_decision_reason = DECISION_FIRST_EXECUTION;
    executor->last_optimized_node = NULL;
    
    // Reset resource management
    executor->total_code_size = 0;
    executor->total_specializations = 0;
    executor->total_memory_usage = 0;
    
    // Reset performance tracking
    executor->total_execution_time_ns = 0;
    executor->total_compilation_time_ns = 0;
    executor->efficiency_ratio = 1.0;
    
    // Reset error handling
    executor->error_count = 0;
    executor->consecutive_failures = 0;
    executor->last_error_time_ns = 0;
}

// ============================================================================
// CONFIGURATION
// ============================================================================

void adaptive_executor_set_optimization_level(AdaptiveExecutor* executor, OptimizationLevel level) {
    if (!executor) return;
    
    executor->optimization_level = level;
    
    // Adjust policy based on optimization level
    switch (level) {
        case OPTIMIZATION_LEVEL_NONE:
            executor->policy.enable_aggressive_optimization = 0;
            executor->policy.enable_specialization = 0;
            executor->policy.enable_jit_compilation = 0;
            break;
        case OPTIMIZATION_LEVEL_BASIC:
            executor->policy.enable_aggressive_optimization = 0;
            executor->policy.enable_specialization = 0;
            executor->policy.enable_jit_compilation = 0;
            break;
        case OPTIMIZATION_LEVEL_AGGRESSIVE:
            executor->policy.enable_aggressive_optimization = 1;
            executor->policy.enable_specialization = 1;
            executor->policy.enable_jit_compilation = 1;
            break;
        case OPTIMIZATION_LEVEL_MAXIMUM:
            executor->policy.enable_aggressive_optimization = 1;
            executor->policy.enable_specialization = 1;
            executor->policy.enable_jit_compilation = 1;
            break;
    }
}

void adaptive_executor_set_adaptive_mode(AdaptiveExecutor* executor, int enabled) {
    if (executor) {
        executor->adaptive_mode = enabled ? 1 : 0;
    }
}

void adaptive_executor_set_policy(AdaptiveExecutor* executor, TierTransitionPolicy* policy) {
    if (executor && policy) {
        executor->policy = *policy;
    }
}

void adaptive_executor_set_thresholds(AdaptiveExecutor* executor, 
                                    uint64_t bytecode_threshold,
                                    uint64_t jit_threshold,
                                    uint64_t specialization_threshold) {
    if (!executor) return;
    
    executor->policy.bytecode_threshold = bytecode_threshold;
    executor->policy.jit_threshold = jit_threshold;
    executor->policy.specialization_threshold = specialization_threshold;
}

// ============================================================================
// MAIN EXECUTION INTERFACE
// ============================================================================

Value adaptive_executor_execute(AdaptiveExecutor* executor, 
                               Interpreter* interpreter, 
                               ASTNode* node) {
    if (!executor || !interpreter || !node) {
        return value_create_null();
    }
    
    // Check if optimization is disabled
    if (!executor->optimization_enabled) {
        return adaptive_executor_execute_ast(executor, interpreter, node);
    }
    
    // Decide execution tier
    DecisionReason reason;
    ExecutionTier tier = adaptive_executor_decide_tier(executor, node, &reason);
    
    // Record decision
    executor->last_decision_time_ns = get_current_time_ns();
    executor->last_decision_reason = reason;
    executor->last_optimized_node = node;
    
    // Execute based on tier
    Value result;
    uint64_t start_time = get_current_time_ns();
    
    switch (tier) {
        case EXECUTION_TIER_AST:
            result = adaptive_executor_execute_ast(executor, interpreter, node);
            break;
        case EXECUTION_TIER_BYTECODE:
            result = adaptive_executor_execute_bytecode(executor, interpreter, node);
            break;
        case EXECUTION_TIER_MICRO_JIT:
            result = adaptive_executor_execute_jit(executor, interpreter, node);
            break;
        case EXECUTION_TIER_SPECIALIZED:
            result = adaptive_executor_execute_specialized(executor, interpreter, node);
            break;
        default:
            result = adaptive_executor_execute_ast(executor, interpreter, node);
            break;
    }
    
    uint64_t end_time = get_current_time_ns();
    uint64_t execution_time = end_time - start_time;
    
    // Update statistics
    adaptive_executor_update_statistics(executor, tier, execution_time);
    
    // Check for errors and handle fallback
    if (interpreter->has_error) {
        adaptive_executor_record_error(executor, DECISION_ERROR_FALLBACK);
        
        // Fallback to AST interpreter
        if (tier != EXECUTION_TIER_AST) {
            interpreter->has_error = 0; // Clear error
            result = adaptive_executor_execute_ast(executor, interpreter, node);
        }
    }
    
    return result;
}

// ============================================================================
// TIER DECISION MAKING
// ============================================================================

ExecutionTier adaptive_executor_decide_tier(AdaptiveExecutor* executor, 
                                          ASTNode* node,
                                          DecisionReason* reason) {
    if (!executor || !node || !reason) {
        *reason = DECISION_ERROR_FALLBACK;
        return EXECUTION_TIER_AST;
    }
    
    // Check for manual override or disabled optimization
    if (!executor->optimization_enabled || executor->optimization_level == OPTIMIZATION_LEVEL_NONE) {
        *reason = DECISION_MANUAL_OVERRIDE;
        return EXECUTION_TIER_AST;
    }
    
    // Check for consecutive failures
    if (adaptive_executor_should_disable_optimization(executor)) {
        *reason = DECISION_ERROR_FALLBACK;
        return EXECUTION_TIER_AST;
    }
    
    // Check resource limits
    if (!adaptive_executor_check_resource_limits(executor)) {
        *reason = DECISION_RESOURCE_LIMIT;
        return EXECUTION_TIER_AST;
    }
    
    // Check if this is the first execution
    if (!node->cached_bytecode) {
        *reason = DECISION_FIRST_EXECUTION;
        return EXECUTION_TIER_AST;
    }
    
    // Check for hot spot
    if (adaptive_executor_is_hot_spot(executor, node)) {
        // Check for type specialization opportunity
        if (executor->policy.enable_specialization && 
            adaptive_executor_is_type_stable(executor, node)) {
            *reason = DECISION_TYPE_STABLE;
            return EXECUTION_TIER_SPECIALIZED;
        }
        
        // Check for JIT compilation opportunity
        if (executor->policy.enable_jit_compilation && 
            adaptive_executor_is_very_hot(executor, node)) {
            *reason = DECISION_HOT_SPOT;
            return EXECUTION_TIER_MICRO_JIT;
        }
        
        // Use bytecode for hot spots
        *reason = DECISION_HOT_SPOT;
        return EXECUTION_TIER_BYTECODE;
    }
    
    // Default to bytecode for subsequent executions
    *reason = DECISION_FIRST_EXECUTION;
    return EXECUTION_TIER_BYTECODE;
}

int adaptive_executor_should_compile_bytecode(AdaptiveExecutor* executor, ASTNode* node) {
    if (!executor || !node) return 0;
    
    // Check if already compiled
    if (node->cached_bytecode) return 0;
    
    // Check execution count
    uint64_t execution_count = adaptive_executor_get_execution_count(executor, node);
    if (execution_count < executor->policy.bytecode_threshold) return 0;
    
    // Check if compilation is worthwhile
    return adaptive_executor_is_compilation_worthwhile(executor, node);
}

int adaptive_executor_should_compile_jit(AdaptiveExecutor* executor, ASTNode* node) {
    if (!executor || !node) return 0;
    
    // Check if JIT is enabled
    if (!executor->policy.enable_jit_compilation) return 0;
    
    // Check if very hot
    if (!adaptive_executor_is_very_hot(executor, node)) return 0;
    
    // Check resource limits
    if (!adaptive_executor_has_code_cache_space(executor)) return 0;
    
    // Check if compilation is worthwhile
    return adaptive_executor_is_compilation_worthwhile(executor, node);
}

int adaptive_executor_should_specialize(AdaptiveExecutor* executor, ASTNode* node) {
    if (!executor || !node) return 0;
    
    // Check if specialization is enabled
    if (!executor->policy.enable_specialization) return 0;
    
    // Check if type is stable
    if (!adaptive_executor_is_type_stable(executor, node)) return 0;
    
    // Check execution count
    uint64_t execution_count = adaptive_executor_get_execution_count(executor, node);
    if (execution_count < executor->policy.specialization_threshold) return 0;
    
    return 1;
}

int adaptive_executor_should_deoptimize(AdaptiveExecutor* executor, ASTNode* node) {
    if (!executor || !node) return 0;
    
    // Check for too many guard failures
    if (executor->consecutive_failures > 5) return 1;
    
    // Check for resource pressure
    if (!adaptive_executor_check_resource_limits(executor)) return 1;
    
    return 0;
}

// ============================================================================
// EXECUTION ROUTING
// ============================================================================

Value adaptive_executor_execute_ast(AdaptiveExecutor* executor, 
                                  Interpreter* interpreter, 
                                  ASTNode* node) {
    if (!executor || !interpreter || !node) {
        return value_create_null();
    }
    
    // Use the original AST interpreter
    return eval_node(interpreter, node);
}

Value adaptive_executor_execute_bytecode(AdaptiveExecutor* executor, 
                                       Interpreter* interpreter, 
                                       ASTNode* node) {
    if (!executor || !interpreter || !node) {
        return value_create_null();
    }
    
    // Check if bytecode is available
    if (!node->cached_bytecode) {
        // Try to compile to bytecode
        if (adaptive_executor_should_compile_bytecode(executor, node)) {
            BytecodeProgram* bytecode = bytecode_compile_ast(node, interpreter);
            if (bytecode) {
                ast_node_set_bytecode(node, bytecode);
            }
        }
        
        // Fallback to AST if compilation failed
        if (!node->cached_bytecode) {
            return adaptive_executor_execute_ast(executor, interpreter, node);
        }
    }
    
    // Execute bytecode
    BytecodeProgram* bytecode = (BytecodeProgram*)node->cached_bytecode;
    return interpreter_execute_bytecode(interpreter, bytecode);
}

Value adaptive_executor_execute_jit(AdaptiveExecutor* executor, 
                                  Interpreter* interpreter, 
                                  ASTNode* node) {
    if (!executor || !interpreter || !node) {
        return value_create_null();
    }
    
    // Check if JIT compilation is available
    if (!executor->micro_jit_context) {
        return adaptive_executor_execute_bytecode(executor, interpreter, node);
    }
    
    // Try to find compiled JIT function
    MicroJitContext* jit_context = (MicroJitContext*)executor->micro_jit_context;
    JitCompiledFunction* compiled_func = micro_jit_find_function(jit_context, node);
    
    if (compiled_func && compiled_func->is_valid) {
        // Execute JIT compiled function
        // TODO: Implement JIT function execution with proper arguments
        return value_create_null();
    }
    
    // Fallback to bytecode
    return adaptive_executor_execute_bytecode(executor, interpreter, node);
}

Value adaptive_executor_execute_specialized(AdaptiveExecutor* executor, 
                                          Interpreter* interpreter, 
                                          ASTNode* node) {
    if (!executor || !interpreter || !node) {
        return value_create_null();
    }
    
    // Check if value specialization is available
    if (!executor->value_specializer) {
        return adaptive_executor_execute_bytecode(executor, interpreter, node);
    }
    
    // Check type guards
    ValueSpecializer* specializer = (ValueSpecializer*)executor->value_specializer;
    // TODO: Implement type guard checking and specialized execution
    
    // Fallback to bytecode
    return adaptive_executor_execute_bytecode(executor, interpreter, node);
}

// ============================================================================
// RESOURCE MANAGEMENT
// ============================================================================

int adaptive_executor_check_resource_limits(AdaptiveExecutor* executor) {
    if (!executor) return 0;
    
    // Check code cache size
    if (executor->total_code_size > executor->policy.max_code_cache_size) {
        return 0;
    }
    
    // Check memory usage
    if (executor->total_memory_usage > executor->policy.max_memory_usage) {
        return 0;
    }
    
    // Check specialization count
    if (executor->total_specializations > executor->policy.max_specialization_count) {
        return 0;
    }
    
    return 1;
}

void adaptive_executor_cleanup_resources(AdaptiveExecutor* executor) {
    if (!executor) return;
    
    // Clean up code cache
    if (executor->micro_jit_context) {
        micro_jit_clear_cache((MicroJitContext*)executor->micro_jit_context);
    }
    
    // Clean up specializations
    if (executor->value_specializer) {
        value_specializer_deoptimize_all((ValueSpecializer*)executor->value_specializer);
    }
    
    // Reset resource counters
    executor->total_code_size = 0;
    executor->total_specializations = 0;
    executor->total_memory_usage = 0;
}

void adaptive_executor_evict_cold_code(AdaptiveExecutor* executor) {
    if (!executor) return;
    
    // Evict cold functions from JIT cache
    if (executor->micro_jit_context) {
        micro_jit_evict_cold_functions((MicroJitContext*)executor->micro_jit_context);
    }
    
    // Clean up cold specializations
    if (executor->value_specializer) {
        // TODO: Implement cold specialization cleanup
    }
}

// ============================================================================
// PERFORMANCE ANALYSIS
// ============================================================================

void adaptive_executor_analyze_performance(AdaptiveExecutor* executor) {
    if (!executor) return;
    
    // Calculate speedups
    adaptive_executor_calculate_speedups(executor);
    
    // Adjust thresholds based on performance
    if (executor->adaptive_mode) {
        adaptive_executor_adjust_thresholds(executor);
    }
    
    // Clean up resources if needed
    if (!adaptive_executor_check_resource_limits(executor)) {
        adaptive_executor_evict_cold_code(executor);
    }
}

void adaptive_executor_update_statistics(AdaptiveExecutor* executor, 
                                       ExecutionTier tier, 
                                       uint64_t execution_time_ns) {
    if (!executor) return;
    
    // Update execution counts
    switch (tier) {
        case EXECUTION_TIER_AST:
            executor->stats.ast_executions++;
            executor->stats.ast_time_ns += execution_time_ns;
            break;
        case EXECUTION_TIER_BYTECODE:
            executor->stats.bytecode_executions++;
            executor->stats.bytecode_time_ns += execution_time_ns;
            break;
        case EXECUTION_TIER_MICRO_JIT:
            executor->stats.jit_executions++;
            executor->stats.jit_time_ns += execution_time_ns;
            break;
        case EXECUTION_TIER_SPECIALIZED:
            executor->stats.specialized_executions++;
            executor->stats.specialized_time_ns += execution_time_ns;
            break;
    }
    
    // Update total execution time
    executor->total_execution_time_ns += execution_time_ns;
    
    // Calculate average times
    if (executor->stats.ast_executions > 0) {
        executor->stats.avg_ast_time_ns = (double)executor->stats.ast_time_ns / executor->stats.ast_executions;
    }
    if (executor->stats.bytecode_executions > 0) {
        executor->stats.avg_bytecode_time_ns = (double)executor->stats.bytecode_time_ns / executor->stats.bytecode_executions;
    }
    if (executor->stats.jit_executions > 0) {
        executor->stats.avg_jit_time_ns = (double)executor->stats.jit_time_ns / executor->stats.jit_executions;
    }
    if (executor->stats.specialized_executions > 0) {
        executor->stats.avg_specialized_time_ns = (double)executor->stats.specialized_time_ns / executor->stats.specialized_executions;
    }
}

void adaptive_executor_calculate_speedups(AdaptiveExecutor* executor) {
    if (!executor) return;
    
    // Calculate bytecode speedup vs AST
    if (executor->stats.ast_executions > 0 && executor->stats.bytecode_executions > 0) {
        executor->stats.bytecode_speedup = executor->stats.avg_ast_time_ns / executor->stats.avg_bytecode_time_ns;
    }
    
    // Calculate JIT speedup vs bytecode
    if (executor->stats.bytecode_executions > 0 && executor->stats.jit_executions > 0) {
        executor->stats.jit_speedup = executor->stats.avg_bytecode_time_ns / executor->stats.avg_jit_time_ns;
    }
    
    // Calculate specialized speedup vs bytecode
    if (executor->stats.bytecode_executions > 0 && executor->stats.specialized_executions > 0) {
        executor->stats.specialized_speedup = executor->stats.avg_bytecode_time_ns / executor->stats.avg_specialized_time_ns;
    }
    
    // Calculate overall speedup
    if (executor->stats.ast_executions > 0) {
        double total_ast_time = executor->stats.ast_time_ns;
        double total_optimized_time = executor->stats.bytecode_time_ns + 
                                    executor->stats.jit_time_ns + 
                                    executor->stats.specialized_time_ns;
        executor->stats.overall_speedup = total_ast_time / total_optimized_time;
    }
}

// ============================================================================
// ERROR HANDLING
// ============================================================================

void adaptive_executor_record_error(AdaptiveExecutor* executor, DecisionReason reason) {
    if (!executor) return;
    
    executor->error_count++;
    executor->consecutive_failures++;
    executor->last_error_time_ns = get_current_time_ns();
    
    // Update statistics
    switch (reason) {
        case DECISION_GUARD_FAILED:
            executor->stats.guard_failures++;
            break;
        case DECISION_ERROR_FALLBACK:
            executor->stats.compilation_failures++;
            break;
        default:
            break;
    }
}

int adaptive_executor_should_disable_optimization(AdaptiveExecutor* executor) {
    if (!executor) return 0;
    
    // Disable if too many consecutive failures
    if (executor->consecutive_failures > 10) return 1;
    
    // Disable if error rate is too high
    uint64_t total_executions = executor->stats.ast_executions + 
                               executor->stats.bytecode_executions + 
                               executor->stats.jit_executions + 
                               executor->stats.specialized_executions;
    
    if (total_executions > 100) {
        double error_rate = (double)executor->error_count / total_executions;
        if (error_rate > 0.1) { // 10% error rate
            return 1;
        }
    }
    
    return 0;
}

void adaptive_executor_handle_compilation_failure(AdaptiveExecutor* executor, ASTNode* node) {
    if (!executor || !node) return;
    
    // Record the failure
    adaptive_executor_record_error(executor, DECISION_ERROR_FALLBACK);
    
    // Clear any cached optimizations for this node
    if (node->cached_bytecode) {
        // TODO: Free bytecode
        node->cached_bytecode = NULL;
    }
}

// ============================================================================
// STATISTICS AND REPORTING
// ============================================================================

void adaptive_executor_print_statistics(AdaptiveExecutor* executor) {
    if (!executor) return;
    
    printf("Adaptive Executor Statistics:\n");
    printf("  Current tier: %d\n", executor->current_tier);
    printf("  Optimization level: %d\n", executor->optimization_level);
    printf("  Adaptive mode: %s\n", executor->adaptive_mode ? "Enabled" : "Disabled");
    printf("  Optimization enabled: %s\n", executor->optimization_enabled ? "Yes" : "No");
    printf("\n");
    
    printf("Execution Counts:\n");
    printf("  AST executions: %llu\n", (unsigned long long)executor->stats.ast_executions);
    printf("  Bytecode executions: %llu\n", (unsigned long long)executor->stats.bytecode_executions);
    printf("  JIT executions: %llu\n", (unsigned long long)executor->stats.jit_executions);
    printf("  Specialized executions: %llu\n", (unsigned long long)executor->stats.specialized_executions);
    printf("\n");
    
    printf("Performance Metrics:\n");
    printf("  Overall speedup: %.2fx\n", executor->stats.overall_speedup);
    printf("  Bytecode speedup: %.2fx\n", executor->stats.bytecode_speedup);
    printf("  JIT speedup: %.2fx\n", executor->stats.jit_speedup);
    printf("  Specialized speedup: %.2fx\n", executor->stats.specialized_speedup);
    printf("\n");
    
    printf("Error Statistics:\n");
    printf("  Total errors: %llu\n", (unsigned long long)executor->error_count);
    printf("  Consecutive failures: %llu\n", (unsigned long long)executor->consecutive_failures);
    printf("  Guard failures: %llu\n", (unsigned long long)executor->stats.guard_failures);
    printf("  Compilation failures: %llu\n", (unsigned long long)executor->stats.compilation_failures);
}

void adaptive_executor_print_tier_distribution(AdaptiveExecutor* executor) {
    if (!executor) return;
    
    uint64_t total = executor->stats.ast_executions + 
                    executor->stats.bytecode_executions + 
                    executor->stats.jit_executions + 
                    executor->stats.specialized_executions;
    
    if (total == 0) {
        printf("No executions recorded yet.\n");
        return;
    }
    
    printf("Execution Tier Distribution:\n");
    printf("  AST: %.1f%% (%llu executions)\n", 
           (double)executor->stats.ast_executions / total * 100,
           (unsigned long long)executor->stats.ast_executions);
    printf("  Bytecode: %.1f%% (%llu executions)\n", 
           (double)executor->stats.bytecode_executions / total * 100,
           (unsigned long long)executor->stats.bytecode_executions);
    printf("  JIT: %.1f%% (%llu executions)\n", 
           (double)executor->stats.jit_executions / total * 100,
           (unsigned long long)executor->stats.jit_executions);
    printf("  Specialized: %.1f%% (%llu executions)\n", 
           (double)executor->stats.specialized_executions / total * 100,
           (unsigned long long)executor->stats.specialized_executions);
}

void adaptive_executor_print_performance_report(AdaptiveExecutor* executor) {
    if (!executor) return;
    
    printf("Performance Report:\n");
    printf("  Total execution time: %llu ns\n", (unsigned long long)executor->total_execution_time_ns);
    printf("  Total compilation time: %llu ns\n", (unsigned long long)executor->total_compilation_time_ns);
    printf("  Efficiency ratio: %.2f\n", executor->efficiency_ratio);
    printf("\n");
    
    printf("Average Execution Times:\n");
    printf("  AST: %.2f ns\n", executor->stats.avg_ast_time_ns);
    printf("  Bytecode: %.2f ns\n", executor->stats.avg_bytecode_time_ns);
    printf("  JIT: %.2f ns\n", executor->stats.avg_jit_time_ns);
    printf("  Specialized: %.2f ns\n", executor->stats.avg_specialized_time_ns);
}

double adaptive_executor_get_overall_speedup(AdaptiveExecutor* executor) {
    return executor ? executor->stats.overall_speedup : 1.0;
}

// ============================================================================
// INTEGRATION WITH INTERPRETER
// ============================================================================

void adaptive_executor_initialize_for_interpreter(Interpreter* interpreter) {
    if (!interpreter) return;
    
    // Initialize adaptive executor
    // This will be implemented when we modify the Interpreter struct
}

void adaptive_executor_cleanup_for_interpreter(Interpreter* interpreter) {
    if (!interpreter) return;
    
    // Cleanup adaptive executor
    // This will be implemented when we modify the Interpreter struct
}

AdaptiveExecutor* adaptive_executor_get_from_interpreter(Interpreter* interpreter) {
    if (!interpreter) return NULL;
    
    // Return adaptive executor from interpreter
    // This will be implemented when we modify the Interpreter struct
    return NULL;
}

// ============================================================================
// DECISION MAKING UTILITIES
// ============================================================================

int adaptive_executor_is_hot_spot(AdaptiveExecutor* executor, ASTNode* node) {
    if (!executor || !node || !executor->hot_spot_tracker) return 0;
    
    return hot_spot_tracker_is_hot((HotSpotTracker*)executor->hot_spot_tracker, node);
}

int adaptive_executor_is_very_hot(AdaptiveExecutor* executor, ASTNode* node) {
    if (!executor || !node || !executor->hot_spot_tracker) return 0;
    
    ExecutionCounter* counter = hot_spot_tracker_get_counter(
        (HotSpotTracker*)executor->hot_spot_tracker, node);
    
    if (!counter) return 0;
    
    return counter->execution_count >= executor->policy.jit_threshold;
}

uint64_t adaptive_executor_get_execution_count(AdaptiveExecutor* executor, ASTNode* node) {
    if (!executor || !node || !executor->hot_spot_tracker) return 0;
    
    ExecutionCounter* counter = hot_spot_tracker_get_counter(
        (HotSpotTracker*)executor->hot_spot_tracker, node);
    
    return counter ? counter->execution_count : 0;
}

int adaptive_executor_is_type_stable(AdaptiveExecutor* executor, ASTNode* node) {
    if (!executor || !node || !executor->value_specializer) return 0;
    
    // For now, use a simple heuristic based on execution count
    uint64_t execution_count = adaptive_executor_get_execution_count(executor, node);
    return execution_count >= executor->policy.specialization_threshold;
}

int adaptive_executor_is_monomorphic(AdaptiveExecutor* executor, ASTNode* node) {
    if (!executor || !node || !executor->value_specializer) return 0;
    
    // For now, assume monomorphic if type is stable
    return adaptive_executor_is_type_stable(executor, node);
}

int adaptive_executor_is_polymorphic(AdaptiveExecutor* executor, ASTNode* node) {
    if (!executor || !node || !executor->value_specializer) return 0;
    
    // For now, assume not polymorphic
    return 0;
}

int adaptive_executor_has_code_cache_space(AdaptiveExecutor* executor) {
    if (!executor || !executor->micro_jit_context) return 0;
    
    // TODO: Check JIT code cache space
    return 1;
}

int adaptive_executor_has_memory_space(AdaptiveExecutor* executor) {
    if (!executor) return 0;
    
    return adaptive_executor_check_resource_limits(executor);
}

int adaptive_executor_is_compilation_worthwhile(AdaptiveExecutor* executor, ASTNode* node) {
    if (!executor || !node) return 0;
    
    // Simple heuristic: compile if execution count is high enough
    uint64_t execution_count = adaptive_executor_get_execution_count(executor, node);
    return execution_count >= executor->policy.bytecode_threshold;
}

// ============================================================================
// POLICY CONFIGURATION
// ============================================================================

TierTransitionPolicy* adaptive_executor_get_default_policy(void) {
    static TierTransitionPolicy policy = {
        .bytecode_threshold = 10,
        .jit_threshold = 100,
        .specialization_threshold = 50,
        .min_execution_time_ns = 1000,
        .max_compilation_time_ns = 1000000,
        .max_code_cache_size = 1024 * 1024, // 1MB
        .max_specialization_count = 1000,
        .max_memory_usage = 10 * 1024 * 1024, // 10MB
        .min_type_stability = 0.8,
        .min_observations = 10,
        .max_deoptimization_rate = 0.1,
        .enable_adaptive_thresholds = 1,
        .enable_aggressive_optimization = 0,
        .enable_specialization = 1,
        .enable_jit_compilation = 1
    };
    
    return &policy;
}

TierTransitionPolicy* adaptive_executor_get_conservative_policy(void) {
    static TierTransitionPolicy policy = {
        .bytecode_threshold = 50,
        .jit_threshold = 500,
        .specialization_threshold = 200,
        .min_execution_time_ns = 5000,
        .max_compilation_time_ns = 5000000,
        .max_code_cache_size = 512 * 1024, // 512KB
        .max_specialization_count = 500,
        .max_memory_usage = 5 * 1024 * 1024, // 5MB
        .min_type_stability = 0.9,
        .min_observations = 50,
        .max_deoptimization_rate = 0.05,
        .enable_adaptive_thresholds = 1,
        .enable_aggressive_optimization = 0,
        .enable_specialization = 1,
        .enable_jit_compilation = 0
    };
    
    return &policy;
}

TierTransitionPolicy* adaptive_executor_get_aggressive_policy(void) {
    static TierTransitionPolicy policy = {
        .bytecode_threshold = 5,
        .jit_threshold = 50,
        .specialization_threshold = 20,
        .min_execution_time_ns = 100,
        .max_compilation_time_ns = 2000000,
        .max_code_cache_size = 2 * 1024 * 1024, // 2MB
        .max_specialization_count = 2000,
        .max_memory_usage = 20 * 1024 * 1024, // 20MB
        .min_type_stability = 0.7,
        .min_observations = 5,
        .max_deoptimization_rate = 0.2,
        .enable_adaptive_thresholds = 1,
        .enable_aggressive_optimization = 1,
        .enable_specialization = 1,
        .enable_jit_compilation = 1
    };
    
    return &policy;
}

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

// get_current_time_ns is defined in hot_spot_tracker.c

// ============================================================================
// ADAPTIVE THRESHOLD ADJUSTMENT
// ============================================================================

void adaptive_executor_adjust_thresholds(AdaptiveExecutor* executor) {
    if (!executor) return;
    
    // Simple adaptive threshold adjustment based on performance
    if (executor->stats.overall_speedup > 2.0) {
        // Performance is good, be more aggressive
        executor->policy.bytecode_threshold = executor->policy.bytecode_threshold / 2;
        executor->policy.jit_threshold = executor->policy.jit_threshold / 2;
        executor->policy.specialization_threshold = executor->policy.specialization_threshold / 2;
    } else if (executor->stats.overall_speedup < 1.5) {
        // Performance is poor, be more conservative
        executor->policy.bytecode_threshold = executor->policy.bytecode_threshold * 2;
        executor->policy.jit_threshold = executor->policy.jit_threshold * 2;
        executor->policy.specialization_threshold = executor->policy.specialization_threshold * 2;
    }
    
    // Ensure thresholds don't go below minimums
    if (executor->policy.bytecode_threshold < 5) executor->policy.bytecode_threshold = 5;
    if (executor->policy.jit_threshold < 20) executor->policy.jit_threshold = 20;
    if (executor->policy.specialization_threshold < 10) executor->policy.specialization_threshold = 10;
}

void adaptive_executor_learn_from_performance(AdaptiveExecutor* executor) {
    if (!executor) return;
    
    // Learn from performance patterns
    // This is a placeholder for more sophisticated learning algorithms
    adaptive_executor_adjust_thresholds(executor);
}
