#ifndef ADAPTIVE_EXECUTOR_H
#define ADAPTIVE_EXECUTOR_H

#include "../interpreter/interpreter_core.h"
#include "../ast.h"
#include "bytecode_engine.h"
#include "hot_spot_tracker.h"
#include "micro_jit.h"
#include "value_specializer.h"
#include "performance_profiler.h"
#include <stdint.h>
#include <stddef.h>

// ============================================================================
// ADAPTIVE EXECUTOR COORDINATOR
// ============================================================================

// Execution tiers (6-tier system)
typedef enum {
    EXECUTION_TIER_AST = 0,         // AST interpreter (baseline)
    EXECUTION_TIER_BYTECODE = 1,    // Register bytecode (4-6x faster)
    EXECUTION_TIER_TRACE_RECORDING = 2, // Trace recording (begin recording)
    EXECUTION_TIER_TRACE_COMPILED = 3,  // Trace compilation (15-25x faster)
    EXECUTION_TIER_SPECIALIZED = 4, // Predictive specialization (20-30x faster)
    EXECUTION_TIER_VECTORIZED = 5,  // Vectorized code (30-50x faster)
    EXECUTION_TIER_COUNT = 6        // Total number of tiers
} ExecutionTier;

// Optimization levels
typedef enum {
    OPTIMIZATION_LEVEL_NONE = 0,    // No optimization
    OPTIMIZATION_LEVEL_BASIC = 1,   // Bytecode only
    OPTIMIZATION_LEVEL_AGGRESSIVE = 2, // Bytecode + Micro-JIT
    OPTIMIZATION_LEVEL_MAXIMUM = 3  // All optimizations enabled
} OptimizationLevel;

// Execution decision reasons
typedef enum {
    DECISION_FIRST_EXECUTION = 0,   // First time executing
    DECISION_HOT_SPOT = 1,          // Hot spot detected
    DECISION_TYPE_STABLE = 2,       // Type specialization possible
    DECISION_GUARD_FAILED = 3,      // Guard failed, deoptimize
    DECISION_CACHE_MISS = 4,        // Cache miss, fallback
    DECISION_ERROR_FALLBACK = 5,    // Error occurred, fallback
    DECISION_RESOURCE_LIMIT = 6,    // Resource limit reached
    DECISION_MANUAL_OVERRIDE = 7    // Manual override
} DecisionReason;

// Execution statistics
typedef struct {
    uint64_t ast_executions;        // Number of AST executions
    uint64_t bytecode_executions;   // Number of bytecode executions
    uint64_t trace_recording_executions; // Number of trace recording executions
    uint64_t trace_compiled_executions;  // Number of trace compiled executions
    uint64_t specialized_executions; // Number of specialized executions
    uint64_t vectorized_executions; // Number of vectorized executions
    
    uint64_t ast_time_ns;           // Total time in AST interpreter
    uint64_t bytecode_time_ns;      // Total time in bytecode interpreter
    uint64_t trace_recording_time_ns; // Total time in trace recording
    uint64_t trace_compiled_time_ns;  // Total time in trace compiled
    uint64_t jit_time_ns;           // Total time in JIT code
    uint64_t specialized_time_ns;   // Total time in specialized code
    uint64_t vectorized_time_ns;    // Total time in vectorized code
    
    uint64_t tier_transitions;      // Number of tier transitions
    uint64_t deoptimizations;       // Number of deoptimizations
    uint64_t compilation_failures;  // Number of compilation failures
    uint64_t guard_failures;        // Number of guard failures
    
    double avg_ast_time_ns;         // Average AST execution time
    double avg_bytecode_time_ns;    // Average bytecode execution time
    double avg_jit_time_ns;         // Average JIT execution time
    double avg_specialized_time_ns; // Average specialized execution time
    
    double overall_speedup;         // Overall speedup factor
    double bytecode_speedup;        // Bytecode vs AST speedup
    double trace_recording_speedup; // Trace recording vs AST speedup
    double trace_compiled_speedup;  // Trace compiled vs AST speedup
    double jit_speedup;             // JIT vs bytecode speedup
    double specialized_speedup;     // Specialized vs bytecode speedup
    double vectorized_speedup;      // Vectorized vs AST speedup
} ExecutionStatistics;

// Tier transition policy
typedef struct {
    // Thresholds for tier promotion
    uint64_t bytecode_threshold;    // Executions needed for bytecode
    uint64_t jit_threshold;         // Executions needed for JIT
    uint64_t specialization_threshold; // Executions needed for specialization
    
    // Time thresholds
    uint64_t min_execution_time_ns; // Minimum execution time for optimization
    uint64_t max_compilation_time_ns; // Maximum time to spend compiling
    
    // Resource limits
    size_t max_code_cache_size;     // Maximum code cache size
    size_t max_specialization_count; // Maximum number of specializations
    uint64_t max_memory_usage;      // Maximum memory usage
    
    // Stability requirements
    double min_type_stability;      // Minimum type stability for specialization
    uint64_t min_observations;      // Minimum observations before optimization
    double max_deoptimization_rate; // Maximum deoptimization rate
    
    // Adaptive behavior
    int enable_adaptive_thresholds; // Enable adaptive threshold adjustment
    int enable_aggressive_optimization; // Enable aggressive optimization
    int enable_specialization;      // Enable value specialization
    int enable_jit_compilation;     // Enable JIT compilation
} TierTransitionPolicy;

// Adaptive executor context
typedef struct {
    // Core components
    HotSpotTracker* hot_spot_tracker;
    MicroJitContext* micro_jit_context;
    ValueSpecializer* value_specializer;
    PerformanceProfiler* performance_profiler;
    
    // Execution state
    ExecutionTier current_tier;
    OptimizationLevel optimization_level;
    int optimization_enabled;
    int adaptive_mode;
    
    // Statistics
    ExecutionStatistics stats;
    TierTransitionPolicy policy;
    
    // Decision making
    uint64_t last_decision_time_ns;
    DecisionReason last_decision_reason;
    ASTNode* last_optimized_node;
    
    // Resource management
    size_t total_code_size;
    size_t total_specializations;
    uint64_t total_memory_usage;
    
    // Performance tracking
    uint64_t total_execution_time_ns;
    uint64_t total_compilation_time_ns;
    double efficiency_ratio;
    
    // Error handling
    int error_count;
    int consecutive_failures;
    uint64_t last_error_time_ns;
} AdaptiveExecutor;

// ============================================================================
// ADAPTIVE EXECUTOR FUNCTIONS
// ============================================================================

// Context management
AdaptiveExecutor* adaptive_executor_create(Interpreter* interpreter);
void adaptive_executor_free(AdaptiveExecutor* executor);
void adaptive_executor_reset(AdaptiveExecutor* executor);

// Configuration
void adaptive_executor_set_optimization_level(AdaptiveExecutor* executor, OptimizationLevel level);
void adaptive_executor_set_adaptive_mode(AdaptiveExecutor* executor, int enabled);
void adaptive_executor_set_policy(AdaptiveExecutor* executor, TierTransitionPolicy* policy);
void adaptive_executor_set_thresholds(AdaptiveExecutor* executor, 
                                    uint64_t bytecode_threshold,
                                    uint64_t jit_threshold,
                                    uint64_t specialization_threshold);

// Main execution interface
Value adaptive_executor_execute(AdaptiveExecutor* executor, 
                               Interpreter* interpreter, 
                               ASTNode* node);

// Tier decision making
ExecutionTier adaptive_executor_decide_tier(AdaptiveExecutor* executor, 
                                          ASTNode* node,
                                          DecisionReason* reason);

int adaptive_executor_should_compile_bytecode(AdaptiveExecutor* executor, ASTNode* node);
int adaptive_executor_should_compile_jit(AdaptiveExecutor* executor, ASTNode* node);
int adaptive_executor_should_specialize(AdaptiveExecutor* executor, ASTNode* node);
int adaptive_executor_should_deoptimize(AdaptiveExecutor* executor, ASTNode* node);

// Tier transitions
int adaptive_executor_promote_to_bytecode(AdaptiveExecutor* executor, ASTNode* node);
int adaptive_executor_promote_to_jit(AdaptiveExecutor* executor, ASTNode* node);
int adaptive_executor_promote_to_specialized(AdaptiveExecutor* executor, ASTNode* node);
int adaptive_executor_demote_tier(AdaptiveExecutor* executor, ASTNode* node, DecisionReason reason);

// Execution routing
Value adaptive_executor_execute_ast(AdaptiveExecutor* executor, 
                                  Interpreter* interpreter, 
                                  ASTNode* node);

Value adaptive_executor_execute_bytecode(AdaptiveExecutor* executor, 
                                       Interpreter* interpreter, 
                                       ASTNode* node);

Value adaptive_executor_execute_jit(AdaptiveExecutor* executor, 
                                  Interpreter* interpreter, 
                                  ASTNode* node);

Value adaptive_executor_execute_specialized(AdaptiveExecutor* executor, 
                                          Interpreter* interpreter, 
                                          ASTNode* node);

// Resource management
int adaptive_executor_check_resource_limits(AdaptiveExecutor* executor);
void adaptive_executor_cleanup_resources(AdaptiveExecutor* executor);
void adaptive_executor_evict_cold_code(AdaptiveExecutor* executor);

// Performance analysis
void adaptive_executor_analyze_performance(AdaptiveExecutor* executor);
void adaptive_executor_update_statistics(AdaptiveExecutor* executor, 
                                       ExecutionTier tier, 
                                       uint64_t execution_time_ns);
void adaptive_executor_calculate_speedups(AdaptiveExecutor* executor);

// Error handling
void adaptive_executor_record_error(AdaptiveExecutor* executor, DecisionReason reason);
int adaptive_executor_should_disable_optimization(AdaptiveExecutor* executor);
void adaptive_executor_handle_compilation_failure(AdaptiveExecutor* executor, ASTNode* node);

// Statistics and reporting
void adaptive_executor_print_statistics(AdaptiveExecutor* executor);
void adaptive_executor_print_tier_distribution(AdaptiveExecutor* executor);
void adaptive_executor_print_performance_report(AdaptiveExecutor* executor);
double adaptive_executor_get_overall_speedup(AdaptiveExecutor* executor);

// Integration with interpreter
void adaptive_executor_initialize_for_interpreter(Interpreter* interpreter);
void adaptive_executor_cleanup_for_interpreter(Interpreter* interpreter);
AdaptiveExecutor* adaptive_executor_get_from_interpreter(Interpreter* interpreter);

// ============================================================================
// DECISION MAKING UTILITIES
// ============================================================================

// Hot spot analysis
int adaptive_executor_is_hot_spot(AdaptiveExecutor* executor, ASTNode* node);
int adaptive_executor_is_very_hot(AdaptiveExecutor* executor, ASTNode* node);
uint64_t adaptive_executor_get_execution_count(AdaptiveExecutor* executor, ASTNode* node);

// Type analysis
int adaptive_executor_is_type_stable(AdaptiveExecutor* executor, ASTNode* node);
int adaptive_executor_is_monomorphic(AdaptiveExecutor* executor, ASTNode* node);
int adaptive_executor_is_polymorphic(AdaptiveExecutor* executor, ASTNode* node);

// Resource analysis
int adaptive_executor_has_code_cache_space(AdaptiveExecutor* executor);
int adaptive_executor_has_memory_space(AdaptiveExecutor* executor);
int adaptive_executor_is_compilation_worthwhile(AdaptiveExecutor* executor, ASTNode* node);

// Performance analysis
double adaptive_executor_estimate_compilation_cost(AdaptiveExecutor* executor, ASTNode* node);
double adaptive_executor_estimate_execution_benefit(AdaptiveExecutor* executor, ASTNode* node);
int adaptive_executor_is_optimization_profitable(AdaptiveExecutor* executor, ASTNode* node);

// ============================================================================
// POLICY CONFIGURATION
// ============================================================================

// Default policies
TierTransitionPolicy* adaptive_executor_get_default_policy(void);
TierTransitionPolicy* adaptive_executor_get_conservative_policy(void);
TierTransitionPolicy* adaptive_executor_get_aggressive_policy(void);

// Policy modification
void adaptive_executor_set_bytecode_threshold(AdaptiveExecutor* executor, uint64_t threshold);
void adaptive_executor_set_jit_threshold(AdaptiveExecutor* executor, uint64_t threshold);
void adaptive_executor_set_specialization_threshold(AdaptiveExecutor* executor, uint64_t threshold);
void adaptive_executor_set_memory_limit(AdaptiveExecutor* executor, uint64_t limit);

// Adaptive threshold adjustment
void adaptive_executor_adjust_thresholds(AdaptiveExecutor* executor);
void adaptive_executor_learn_from_performance(AdaptiveExecutor* executor);

// ============================================================================
// DEBUGGING AND DIAGNOSTICS
// ============================================================================

// Debug information
void adaptive_executor_print_decision_tree(AdaptiveExecutor* executor, ASTNode* node);
void adaptive_executor_print_tier_history(AdaptiveExecutor* executor);
void adaptive_executor_print_resource_usage(AdaptiveExecutor* executor);

// Diagnostic functions
int adaptive_executor_validate_integration(AdaptiveExecutor* executor);
int adaptive_executor_test_all_tiers(AdaptiveExecutor* executor);
void adaptive_executor_benchmark_decision_making(AdaptiveExecutor* executor);

// 6-tier system functions
int adaptive_executor_record_tier_execution(AdaptiveExecutor* executor,
                                            ExecutionTier tier,
                                            uint64_t execution_time_ns,
                                            uint64_t instruction_count);
ExecutionTier adaptive_executor_get_optimal_tier(AdaptiveExecutor* executor,
                                                 ASTNode* function_node);
int adaptive_executor_should_promote_tier(AdaptiveExecutor* executor,
                                          ExecutionTier current_tier,
                                          ASTNode* function_node);
void adaptive_executor_update_tier_effectiveness(AdaptiveExecutor* executor);

#endif // ADAPTIVE_EXECUTOR_H
