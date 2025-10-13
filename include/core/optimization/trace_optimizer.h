/**
 * @file trace_optimizer.h
 * @brief Trace optimization algorithms
 * 
 * Loop-invariant code motion, CSE, constant propagation, dead code elimination,
 * strength reduction, vectorization preparation.
 */

#ifndef TRACE_OPTIMIZER_H
#define TRACE_OPTIMIZER_H

#include "trace_recorder.h"
#include "register_vm.h"
#include <stdint.h>
#include <stddef.h>

// ============================================================================
// TRACE OPTIMIZATION DATA STRUCTURES
// ============================================================================

/**
 * @brief Optimization level
 * 
 * Defines the level of optimization to apply to traces.
 * Higher levels provide better performance but take longer to optimize.
 */
typedef enum {
    OPT_LEVEL_NONE = 0,            // No optimization
    OPT_LEVEL_BASIC = 1,           // Basic optimizations only
    OPT_LEVEL_STANDARD = 2,        // Standard optimizations
    OPT_LEVEL_AGGRESSIVE = 3,      // Aggressive optimizations
    OPT_LEVEL_MAXIMUM = 4          // Maximum optimizations
} OptimizationLevel;

/**
 * @brief Optimization pass
 * 
 * Represents a single optimization pass that can be applied to a trace.
 */
typedef enum {
    OPT_PASS_LOOP_INVARIANT = 0,   // Loop-invariant code motion
    OPT_PASS_CSE = 1,              // Common subexpression elimination
    OPT_PASS_CONSTANT_PROP = 2,    // Constant propagation
    OPT_PASS_CONSTANT_FOLD = 3,    // Constant folding
    OPT_PASS_DEAD_CODE = 4,        // Dead code elimination
    OPT_PASS_STRENGTH_RED = 5,     // Strength reduction
    OPT_PASS_ALGEBRAIC = 6,        // Algebraic simplification
    OPT_PASS_VECTORIZATION = 7,    // Vectorization preparation
    OPT_PASS_INLINING = 8,         // Function inlining
    OPT_PASS_SPECIALIZATION = 9,   // Type specialization
    OPT_PASS_PEEPHOLE = 10,        // Peephole optimization
    OPT_PASS_REGISTER_ALLOC = 11   // Register allocation
} OptimizationPass;

/**
 * @brief Optimization statistics
 * 
 * Tracks statistics about optimization passes and their effectiveness.
 */
typedef struct {
    uint32_t pass_count;           // Number of optimization passes applied
    uint32_t instruction_count_before; // Instructions before optimization
    uint32_t instruction_count_after;  // Instructions after optimization
    uint32_t instructions_eliminated;  // Number of instructions eliminated
    uint32_t constants_folded;     // Number of constants folded
    uint32_t dead_code_eliminated; // Number of dead instructions eliminated
    uint32_t loop_invariants_moved; // Number of loop invariants moved
    uint32_t common_subexprs_eliminated; // Number of CSEs eliminated
    uint32_t strength_reductions;  // Number of strength reductions applied
    uint32_t algebraic_simplifications; // Number of algebraic simplifications
    uint32_t vectorization_opportunities; // Number of vectorization opportunities
    uint32_t inlining_opportunities; // Number of inlining opportunities
    uint32_t specialization_opportunities; // Number of specialization opportunities
    
    double optimization_time_ms;   // Time spent optimizing (milliseconds)
    double speedup_estimate;       // Estimated speedup from optimization
    double code_size_reduction;    // Code size reduction percentage
    double register_pressure_reduction; // Register pressure reduction
    double memory_access_reduction; // Memory access reduction
} OptimizationStatistics;

/**
 * @brief Optimized trace
 * 
 * Contains an optimized trace with improved performance characteristics.
 * This is the output of the trace optimization process.
 */
typedef struct {
    uint32_t original_trace_id;    // ID of the original trace
    uint32_t optimized_trace_id;   // ID of the optimized trace
    
    TraceInstruction* instructions; // Optimized instructions
    uint32_t instruction_count;    // Number of optimized instructions
    uint32_t instruction_capacity; // Capacity of instruction array
    
    TraceGuard* guards;            // Optimized guards
    uint32_t guard_count;          // Number of optimized guards
    uint32_t guard_capacity;       // Capacity of guard array
    
    // Optimization metadata
    OptimizationStatistics stats;  // Optimization statistics
    uint32_t* optimization_applied; // Which optimizations were applied
    uint32_t optimization_count;   // Number of optimizations applied
    
    // Performance characteristics
    double estimated_speedup;      // Estimated speedup over original
    double hotness_score;          // Hotness score of optimized trace
    int is_vectorizable;           // Can this trace be vectorized?
    int is_inlinable;              // Can this trace be inlined?
    int is_specializable;          // Can this trace be specialized?
    
    // Memory management
    int is_owned;                  // Does this trace own its memory?
} OptimizedTrace;

/**
 * @brief Trace optimizer context
 * 
 * Contains the state and configuration for trace optimization.
 * This is the main interface for the trace optimization system.
 */
typedef struct {
    // Configuration
    OptimizationLevel level;       // Optimization level
    uint32_t max_optimization_passes; // Maximum number of optimization passes
    uint32_t max_instruction_count; // Maximum instruction count for optimization
    double optimization_timeout_ms; // Optimization timeout (milliseconds)
    int enable_vectorization;      // Enable vectorization optimizations
    int enable_inlining;           // Enable inlining optimizations
    int enable_specialization;     // Enable specialization optimizations
    int enable_aggressive_opt;     // Enable aggressive optimizations
    
    // Optimization state
    uint32_t current_pass;         // Current optimization pass
    uint32_t current_trace_id;     // Current trace being optimized
    int optimization_in_progress;  // Is optimization in progress?
    
    // Optimized traces
    OptimizedTrace* optimized_traces; // Array of optimized traces
    uint32_t optimized_trace_count; // Number of optimized traces
    uint32_t max_optimized_traces; // Maximum number of optimized traces
    
    // Statistics
    uint64_t total_traces_optimized; // Total number of traces optimized
    uint64_t total_instructions_optimized; // Total instructions optimized
    uint64_t total_optimization_time; // Total optimization time (nanoseconds)
    double average_optimization_time; // Average optimization time per trace
    double average_speedup;        // Average speedup achieved
    double average_code_reduction; // Average code size reduction
    
    // Performance tracking
    uint64_t optimization_start_time; // Optimization start time
    uint64_t optimization_end_time;   // Optimization end time
    double total_optimization_time_ms; // Total optimization time (milliseconds)
    double optimization_overhead;     // Optimization overhead percentage
} TraceOptimizerContext;

// ============================================================================
// TRACE OPTIMIZATION FUNCTIONS
// ============================================================================

/**
 * @brief Core optimization functions
 * 
 * These functions handle the main trace optimization process,
 * from initialization to trace completion.
 */

/**
 * @brief Create a new trace optimizer context
 * @param level Optimization level to use
 * @return New TraceOptimizerContext, or NULL on failure
 * @note The context is initialized with default settings
 */
TraceOptimizerContext* trace_optimizer_create(OptimizationLevel level);

/**
 * @brief Free a trace optimizer context
 * @param context Context to free (can be NULL)
 * @note This function is safe to call with NULL
 */
void trace_optimizer_free(TraceOptimizerContext* context);

/**
 * @brief Optimize a recorded trace
 * @param context Optimizer context
 * @param trace Trace to optimize
 * @return Optimized trace, or NULL on failure
 * @note This is the main entry point for trace optimization
 */
OptimizedTrace* trace_optimizer_optimize_trace(TraceOptimizerContext* context, RecordedTrace* trace);

/**
 * @brief Apply a specific optimization pass to a trace
 * @param context Optimizer context
 * @param trace Trace to optimize
 * @param pass Optimization pass to apply
 * @return 1 on success, 0 on failure
 * @note This function applies a single optimization pass
 */
int trace_optimizer_apply_pass(TraceOptimizerContext* context, OptimizedTrace* trace, OptimizationPass pass);

/**
 * @brief Run all optimization passes on a trace
 * @param context Optimizer context
 * @param trace Trace to optimize
 * @return 1 on success, 0 on failure
 * @note This function runs all applicable optimization passes
 */
int trace_optimizer_run_all_passes(TraceOptimizerContext* context, OptimizedTrace* trace);

/**
 * @brief Optimization pass functions
 * 
 * These functions implement specific optimization passes.
 */

/**
 * @brief Apply loop-invariant code motion
 * @param context Optimizer context
 * @param trace Trace to optimize
 * @return 1 on success, 0 on failure
 * @note Moves loop-invariant instructions outside of loops
 */
int trace_optimizer_loop_invariant_motion(TraceOptimizerContext* context, OptimizedTrace* trace);

/**
 * @brief Apply common subexpression elimination
 * @param context Optimizer context
 * @param trace Trace to optimize
 * @return 1 on success, 0 on failure
 * @note Eliminates redundant computations
 */
int trace_optimizer_common_subexpression_elimination(TraceOptimizerContext* context, OptimizedTrace* trace);

/**
 * @brief Apply constant propagation
 * @param context Optimizer context
 * @param trace Trace to optimize
 * @return 1 on success, 0 on failure
 * @note Propagates constant values through the trace
 */
int trace_optimizer_constant_propagation(TraceOptimizerContext* context, OptimizedTrace* trace);

/**
 * @brief Apply constant folding
 * @param context Optimizer context
 * @param trace Trace to optimize
 * @return 1 on success, 0 on failure
 * @note Evaluates constant expressions at compile time
 */
int trace_optimizer_constant_folding(TraceOptimizerContext* context, OptimizedTrace* trace);

/**
 * @brief Apply dead code elimination
 * @param context Optimizer context
 * @param trace Trace to optimize
 * @return 1 on success, 0 on failure
 * @note Removes unreachable and unused code
 */
int trace_optimizer_dead_code_elimination(TraceOptimizerContext* context, OptimizedTrace* trace);

/**
 * @brief Apply strength reduction
 * @param context Optimizer context
 * @param trace Trace to optimize
 * @return 1 on success, 0 on failure
 * @note Replaces expensive operations with cheaper ones
 */
int trace_optimizer_strength_reduction(TraceOptimizerContext* context, OptimizedTrace* trace);

/**
 * @brief Apply algebraic simplification
 * @param context Optimizer context
 * @param trace Trace to optimize
 * @return 1 on success, 0 on failure
 * @note Simplifies algebraic expressions
 */
int trace_optimizer_algebraic_simplification(TraceOptimizerContext* context, OptimizedTrace* trace);

/**
 * @brief Prepare trace for vectorization
 * @param context Optimizer context
 * @param trace Trace to optimize
 * @return 1 on success, 0 on failure
 * @note Prepares the trace for SIMD vectorization
 */
int trace_optimizer_vectorization_preparation(TraceOptimizerContext* context, OptimizedTrace* trace);

/**
 * @brief Apply function inlining
 * @param context Optimizer context
 * @param trace Trace to optimize
 * @return 1 on success, 0 on failure
 * @note Inlines small functions to eliminate call overhead
 */
int trace_optimizer_function_inlining(TraceOptimizerContext* context, OptimizedTrace* trace);

/**
 * @brief Apply type specialization
 * @param context Optimizer context
 * @param trace Trace to optimize
 * @return 1 on success, 0 on failure
 * @note Specializes instructions for specific types
 */
int trace_optimizer_type_specialization(TraceOptimizerContext* context, OptimizedTrace* trace);

/**
 * @brief Apply peephole optimization
 * @param context Optimizer context
 * @param trace Trace to optimize
 * @return 1 on success, 0 on failure
 * @note Applies local optimizations to instruction sequences
 */
int trace_optimizer_peephole_optimization(TraceOptimizerContext* context, OptimizedTrace* trace);

/**
 * @brief Apply register allocation optimization
 * @param context Optimizer context
 * @param trace Trace to optimize
 * @return 1 on success, 0 on failure
 * @note Optimizes register usage and reduces register pressure
 */
int trace_optimizer_register_allocation(TraceOptimizerContext* context, OptimizedTrace* trace);

/**
 * @brief Analysis functions
 * 
 * These functions analyze traces to provide information for
 * optimization decisions.
 */

/**
 * @brief Analyze trace for optimization opportunities
 * @param context Optimizer context
 * @param trace Trace to analyze
 * @return 1 on success, 0 on failure
 * @note Analyzes a trace to identify optimization opportunities
 */
int trace_optimizer_analyze_trace(TraceOptimizerContext* context, OptimizedTrace* trace);

/**
 * @brief Calculate optimization potential
 * @param context Optimizer context
 * @param trace Trace to analyze
 * @return Optimization potential score (0.0-1.0)
 * @note Calculates how much a trace can be optimized
 */
double trace_optimizer_calculate_potential(TraceOptimizerContext* context, OptimizedTrace* trace);

/**
 * @brief Estimate optimization speedup
 * @param context Optimizer context
 * @param trace Trace to analyze
 * @return Estimated speedup factor
 * @note Estimates the speedup from optimizing a trace
 */
double trace_optimizer_estimate_speedup(TraceOptimizerContext* context, OptimizedTrace* trace);

/**
 * @brief Check if trace is optimizable
 * @param context Optimizer context
 * @param trace Trace to check
 * @return 1 if optimizable, 0 otherwise
 * @note Checks if a trace is suitable for optimization
 */
int trace_optimizer_is_optimizable(TraceOptimizerContext* context, OptimizedTrace* trace);

/**
 * @brief Management functions
 * 
 * These functions manage optimized traces and the optimization process.
 */

/**
 * @brief Get an optimized trace by ID
 * @param context Optimizer context
 * @param trace_id Trace ID
 * @return Optimized trace, or NULL if not found
 * @note This function retrieves an optimized trace by its ID
 */
OptimizedTrace* trace_optimizer_get_optimized_trace(TraceOptimizerContext* context, uint32_t trace_id);

/**
 * @brief Get all optimized traces
 * @param context Optimizer context
 * @param traces Array to store traces
 * @param max_traces Maximum number of traces to return
 * @return Number of traces returned
 * @note This function retrieves all optimized traces
 */
uint32_t trace_optimizer_get_all_optimized_traces(TraceOptimizerContext* context, OptimizedTrace** traces, uint32_t max_traces);

/**
 * @brief Remove an optimized trace
 * @param context Optimizer context
 * @param trace_id Trace ID to remove
 * @return 1 on success, 0 on failure
 * @note This function removes an optimized trace
 */
int trace_optimizer_remove_optimized_trace(TraceOptimizerContext* context, uint32_t trace_id);

/**
 * @brief Clear all optimized traces
 * @param context Optimizer context
 * @note This function removes all optimized traces
 */
void trace_optimizer_clear_optimized_traces(TraceOptimizerContext* context);

/**
 * @brief Configuration functions
 * 
 * These functions configure the trace optimizer behavior and settings.
 */

/**
 * @brief Set optimization level
 * @param context Optimizer context
 * @param level New optimization level
 * @note This function changes the optimization level
 */
void trace_optimizer_set_level(TraceOptimizerContext* context, OptimizationLevel level);

/**
 * @brief Set optimization timeout
 * @param context Optimizer context
 * @param timeout_ms Timeout in milliseconds
 * @note This function sets the optimization timeout
 */
void trace_optimizer_set_timeout(TraceOptimizerContext* context, double timeout_ms);

/**
 * @brief Enable or disable specific optimizations
 * @param context Optimizer context
 * @param enable_vectorization Enable vectorization
 * @param enable_inlining Enable inlining
 * @param enable_specialization Enable specialization
 * @param enable_aggressive Enable aggressive optimizations
 * @note This function configures which optimizations are enabled
 */
void trace_optimizer_set_optimizations(TraceOptimizerContext* context, 
                                      int enable_vectorization,
                                      int enable_inlining,
                                      int enable_specialization,
                                      int enable_aggressive);

/**
 * @brief Utility functions
 * 
 * These functions provide utility functionality for working with
 * trace optimizer contexts and optimized traces.
 */

/**
 * @brief Get optimization statistics
 * @param context Optimizer context
 * @return Statistics string
 * @note Returns a formatted string with optimization statistics
 */
char* trace_optimizer_get_statistics(TraceOptimizerContext* context);

/**
 * @brief Print optimized trace information
 * @param context Optimizer context
 * @param trace_id Trace ID to print
 * @note Prints human-readable representation of an optimized trace
 */
void trace_optimizer_print_optimized_trace(TraceOptimizerContext* context, uint32_t trace_id);

/**
 * @brief Print all optimized traces
 * @param context Optimizer context
 * @note Prints human-readable representation of all optimized traces
 */
void trace_optimizer_print_all_optimized_traces(TraceOptimizerContext* context);

/**
 * @brief Validate optimized trace
 * @param context Optimizer context
 * @param trace_id Trace ID to validate
 * @return 1 if valid, 0 if invalid
 * @note Validates that an optimized trace is correct
 */
int trace_optimizer_validate_optimized_trace(TraceOptimizerContext* context, uint32_t trace_id);

/**
 * @brief Export optimized trace to file
 * @param context Optimizer context
 * @param trace_id Trace ID to export
 * @param filename Output filename
 * @return 1 on success, 0 on failure
 * @note Exports an optimized trace to a file for analysis
 */
int trace_optimizer_export_optimized_trace(TraceOptimizerContext* context, uint32_t trace_id, const char* filename);

/**
 * @brief Import optimized trace from file
 * @param context Optimizer context
 * @param filename Input filename
 * @return Trace ID, or 0 on failure
 * @note Imports an optimized trace from a file
 */
uint32_t trace_optimizer_import_optimized_trace(TraceOptimizerContext* context, const char* filename);

#endif // TRACE_OPTIMIZER_H
