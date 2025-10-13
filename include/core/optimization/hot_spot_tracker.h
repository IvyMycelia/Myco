#ifndef HOT_SPOT_TRACKER_H
#define HOT_SPOT_TRACKER_H

#include "../interpreter/interpreter_core.h"
#include "../ast.h"
#include <stdint.h>
#include <stddef.h>

// ============================================================================
// HOT SPOT DETECTION SYSTEM
// ============================================================================

// Hot spot detection thresholds
#define HOT_SPOT_THRESHOLD_DEFAULT 100      // Default execution threshold
#define HOT_SPOT_THRESHOLD_MIN 10           // Minimum threshold
#define HOT_SPOT_THRESHOLD_MAX 10000        // Maximum threshold
#define HOT_SPOT_ADAPTIVE_FACTOR 0.1        // Adaptive threshold factor

// Hot spot types
typedef enum {
    HOT_SPOT_FUNCTION = 0,      // Function-level hot spot
    HOT_SPOT_LOOP = 1,          // Loop-level hot spot
    HOT_SPOT_EXPRESSION = 2,    // Expression-level hot spot
    HOT_SPOT_BLOCK = 3          // Block-level hot spot
} HotSpotType;

// Hot spot execution counter
typedef struct {
    uint64_t execution_count;   // Number of times executed
    uint64_t total_time_ns;     // Total execution time in nanoseconds
    uint64_t last_execution_ns; // Timestamp of last execution
    uint64_t peak_time_ns;      // Peak execution time
    uint64_t avg_time_ns;       // Average execution time
    int is_hot;                 // Is currently considered hot
    int optimization_level;     // Current optimization level (0-2)
    int deoptimization_count;   // Number of times deoptimized
} ExecutionCounter;

// Hot spot metadata for optimization decisions
typedef struct {
    HotSpotType type;           // Type of hot spot
    ASTNode* ast_node;          // Associated AST node
    void* bytecode;             // Cached bytecode (if any)
    void* jit_code;             // Cached JIT code (if any)
    
    // Type specialization data
    ValueType* parameter_types; // Parameter types for functions
    size_t parameter_count;     // Number of parameters
    ValueType return_type;      // Return type
    
    // Branch prediction data
    int* branch_taken;          // Branch taken counts
    int* branch_not_taken;      // Branch not taken counts
    size_t branch_count;        // Number of branches
    
    // Loop-specific data
    uint64_t loop_iterations;   // Total loop iterations
    uint64_t avg_iterations;    // Average iterations per execution
    uint64_t max_iterations;    // Maximum iterations in single execution
    
    // Performance metrics
    double speedup_factor;      // Measured speedup from optimization
    int stability_score;        // Stability score (0-100)
    uint64_t last_optimized_ns; // When last optimized
} HotSpotInfo;

// Hot spot tracker main structure
typedef struct {
    // Execution counters (hash map: AST node -> counter)
    struct {
        ASTNode** keys;         // AST node pointers
        ExecutionCounter* values; // Execution counters
        size_t count;           // Number of entries
        size_t capacity;        // Allocated capacity
    } counters;
    
    // Hot spot information (hash map: AST node -> info)
    struct {
        ASTNode** keys;         // AST node pointers
        HotSpotInfo* values;    // Hot spot info
        size_t count;           // Number of entries
        size_t capacity;        // Allocated capacity
    } hot_spots;
    
    // Configuration
    uint64_t hot_threshold;     // Current hot spot threshold
    uint64_t adaptive_threshold; // Adaptive threshold
    int enable_adaptive;        // Enable adaptive thresholding
    int enable_profiling;       // Enable detailed profiling
    
    // Statistics
    uint64_t total_executions;  // Total executions tracked
    uint64_t hot_spot_count;    // Number of hot spots detected
    uint64_t optimization_count; // Number of optimizations applied
    uint64_t deoptimization_count; // Number of deoptimizations
    
    // Performance tracking
    uint64_t profiling_overhead_ns; // Profiling overhead in nanoseconds
    uint64_t last_cleanup_ns;   // Last cleanup timestamp
} HotSpotTracker;

// ============================================================================
// HOT SPOT TRACKER FUNCTIONS
// ============================================================================

// Tracker lifecycle
HotSpotTracker* hot_spot_tracker_create(void);
void hot_spot_tracker_free(HotSpotTracker* tracker);
void hot_spot_tracker_reset(HotSpotTracker* tracker);

// Configuration
void hot_spot_tracker_set_threshold(HotSpotTracker* tracker, uint64_t threshold);
void hot_spot_tracker_set_adaptive(HotSpotTracker* tracker, int enabled);
void hot_spot_tracker_set_profiling(HotSpotTracker* tracker, int enabled);
uint64_t hot_spot_tracker_get_threshold(HotSpotTracker* tracker);

// Execution tracking
void hot_spot_tracker_record_execution(HotSpotTracker* tracker, ASTNode* node, uint64_t execution_time_ns);
void hot_spot_tracker_record_function_call(HotSpotTracker* tracker, ASTNode* func_node, Value* args, size_t arg_count, uint64_t execution_time_ns);
void hot_spot_tracker_record_loop_iteration(HotSpotTracker* tracker, ASTNode* loop_node, uint64_t iteration_time_ns);
void hot_spot_tracker_record_expression(HotSpotTracker* tracker, ASTNode* expr_node, uint64_t execution_time_ns);

// Hot spot detection
int hot_spot_tracker_is_hot(HotSpotTracker* tracker, ASTNode* node);
HotSpotInfo* hot_spot_tracker_get_info(HotSpotTracker* tracker, ASTNode* node);
ExecutionCounter* hot_spot_tracker_get_counter(HotSpotTracker* tracker, ASTNode* node);

// Hot spot management
void hot_spot_tracker_mark_hot(HotSpotTracker* tracker, ASTNode* node, HotSpotType type);
void hot_spot_tracker_mark_cold(HotSpotTracker* tracker, ASTNode* node);
void hot_spot_tracker_update_optimization_level(HotSpotTracker* tracker, ASTNode* node, int level);

// Type specialization tracking
void hot_spot_tracker_record_parameter_types(HotSpotTracker* tracker, ASTNode* func_node, Value* args, size_t arg_count);
void hot_spot_tracker_record_return_type(HotSpotTracker* tracker, ASTNode* func_node, ValueType return_type);
void hot_spot_tracker_record_branch_taken(HotSpotTracker* tracker, ASTNode* branch_node, int taken);

// Performance analysis
void hot_spot_tracker_analyze_performance(HotSpotTracker* tracker);
void hot_spot_tracker_cleanup_cold_spots(HotSpotTracker* tracker);
void hot_spot_tracker_optimize_thresholds(HotSpotTracker* tracker);

// Statistics and reporting
uint64_t hot_spot_tracker_get_total_executions(HotSpotTracker* tracker);
uint64_t hot_spot_tracker_get_hot_spot_count(HotSpotTracker* tracker);
uint64_t hot_spot_tracker_get_optimization_count(HotSpotTracker* tracker);
double hot_spot_tracker_get_avg_speedup(HotSpotTracker* tracker);
void hot_spot_tracker_print_statistics(HotSpotTracker* tracker);
void hot_spot_tracker_print_hot_spots(HotSpotTracker* tracker);

// Integration with interpreter
void hot_spot_tracker_initialize_for_interpreter(Interpreter* interpreter);
void hot_spot_tracker_cleanup_for_interpreter(Interpreter* interpreter);
HotSpotTracker* hot_spot_tracker_get_from_interpreter(Interpreter* interpreter);

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

// Hash functions for AST node pointers
uint64_t ast_node_hash(ASTNode* node);
int ast_node_equals(ASTNode* a, ASTNode* b);

// Time utilities
uint64_t get_current_time_ns(void);
uint64_t calculate_execution_time_ns(uint64_t start_time, uint64_t end_time);

// Hot spot analysis
int should_optimize_hot_spot(HotSpotInfo* info);
int is_hot_spot_stable(HotSpotInfo* info);
double calculate_optimization_potential(HotSpotInfo* info);

// Memory management
void hot_spot_info_free(HotSpotInfo* info);
void execution_counter_free(ExecutionCounter* counter);

#endif // HOT_SPOT_TRACKER_H
