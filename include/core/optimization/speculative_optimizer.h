/**
 * @file speculative_optimizer.h
 * @brief Speculative optimization
 * 
 * Speculative inlining, guard insertion, quick deoptimization on failure,
 * learning from failed speculation.
 */

#ifndef SPECULATIVE_OPTIMIZER_H
#define SPECULATIVE_OPTIMIZER_H

#include "type_predictor.h"
#include "trace_recorder.h"
#include "register_vm.h"
#include "../interpreter/interpreter_core.h"
#include <stdint.h>
#include <stddef.h>

// ============================================================================
// SPECULATIVE OPTIMIZATION DATA STRUCTURES
// ============================================================================

/**
 * @brief Speculation type
 * 
 * Defines the type of speculative optimization being performed.
 */
typedef enum {
    SPECULATION_TYPE_INLINING = 0,      // Function inlining speculation
    SPECULATION_TYPE_TYPE_SPECIALIZATION = 1, // Type specialization speculation
    SPECULATION_TYPE_LOOP_UNROLLING = 2, // Loop unrolling speculation
    SPECULATION_TYPE_CONSTANT_FOLDING = 3, // Constant folding speculation
    SPECULATION_TYPE_DEAD_CODE_ELIMINATION = 4, // Dead code elimination speculation
    SPECULATION_TYPE_VECTORIZATION = 5, // Vectorization speculation
    SPECULATION_TYPE_BRANCH_PREDICTION = 6, // Branch prediction speculation
    SPECULATION_TYPE_MEMORY_ACCESS = 7  // Memory access pattern speculation
} SpeculationType;

/**
 * @brief Guard type
 * 
 * Defines the type of guard condition for speculative optimization.
 */
typedef enum {
    GUARD_TYPE_TYPE_CHECK = 0,          // Type checking guard
    GUARD_TYPE_VALUE_CHECK = 1,         // Value checking guard
    GUARD_TYPE_RANGE_CHECK = 2,         // Range checking guard
    GUARD_TYPE_NULL_CHECK = 3,          // Null checking guard
    GUARD_TYPE_ARRAY_BOUNDS = 4,        // Array bounds checking guard
    GUARD_TYPE_OBJECT_PROPERTY = 5,     // Object property checking guard
    GUARD_TYPE_FUNCTION_EXISTS = 6,     // Function existence checking guard
    GUARD_TYPE_CONDITION = 7            // General condition checking guard
} GuardType;

/**
 * @brief Speculation result
 * 
 * Represents the result of a speculative optimization.
 */
typedef enum {
    SPECULATION_RESULT_SUCCESS = 0,     // Speculation succeeded
    SPECULATION_RESULT_FAILURE = 1,     // Speculation failed
    SPECULATION_RESULT_DEOPTIMIZED = 2, // Speculation was deoptimized
    SPECULATION_RESULT_ABORTED = 3      // Speculation was aborted
} SpeculationResult;

/**
 * @brief Speculation guard
 * 
 * Represents a guard condition that must be satisfied for speculation to be valid.
 */
typedef struct {
    uint32_t guard_id;                  // Unique guard identifier
    GuardType guard_type;               // Type of guard
    uint32_t instruction_id;            // Instruction that created the guard
    uint32_t register_id;               // Register being guarded
    uint64_t expected_value;            // Expected value (for value guards)
    uint8_t expected_type;              // Expected type (for type guards)
    uint64_t min_value;                 // Minimum value (for range guards)
    uint64_t max_value;                 // Maximum value (for range guards)
    double confidence;                  // Guard confidence (0.0-1.0)
    int is_essential;                   // Is this guard essential for correctness?
    int has_failed;                     // Has this guard ever failed?
    uint64_t failure_count;             // Number of times guard has failed
    uint64_t success_count;             // Number of times guard has succeeded
    double success_rate;                // Success rate (0.0-1.0)
    uint64_t last_check_time;           // Timestamp of last check
    uint64_t total_check_time;          // Total time spent checking
    double average_check_time;          // Average check time (nanoseconds)
} SpeculationGuard;

/**
 * @brief Speculation context
 * 
 * Contains information about a speculative optimization context.
 */
typedef struct {
    uint32_t speculation_id;            // Unique speculation identifier
    SpeculationType speculation_type;   // Type of speculation
    uint32_t call_site_id;              // Call site being speculated
    TypePattern* predicted_pattern;     // Predicted type pattern
    double confidence;                  // Confidence in speculation (0.0-1.0)
    uint64_t execution_count;           // Number of times executed
    uint64_t success_count;             // Number of successful executions
    uint64_t failure_count;             // Number of failed executions
    double success_rate;                // Success rate (0.0-1.0)
    
    // Optimization metadata
    uint32_t optimized_instruction_count; // Number of optimized instructions
    uint32_t guard_count;               // Number of guards
    SpeculationGuard* guards;           // Array of guards
    uint32_t guard_capacity;            // Capacity of guards array
    
    // Performance metrics
    uint64_t total_execution_time;      // Total execution time (nanoseconds)
    uint64_t optimized_execution_time;  // Optimized execution time (nanoseconds)
    double speedup;                     // Speedup from speculation
    double overhead;                    // Overhead from guards
    double net_benefit;                 // Net benefit from speculation
    
    // Deoptimization
    int is_deoptimized;                 // Has this speculation been deoptimized?
    uint64_t deoptimization_time;       // Time when deoptimized
    const char* deoptimization_reason;  // Reason for deoptimization
    
    // Learning
    int learning_enabled;               // Is learning enabled for this speculation?
    double learning_rate;               // Learning rate for updates
    uint64_t learning_updates;          // Number of learning updates
    double prediction_accuracy;         // Prediction accuracy (0.0-1.0)
} SpeculationContext;

/**
 * @brief Speculative optimizer context
 * 
 * Contains the state and configuration for speculative optimization.
 */
typedef struct {
    // Configuration
    double confidence_threshold;        // Minimum confidence for speculation
    double success_rate_threshold;      // Minimum success rate to continue speculation
    uint32_t max_guards_per_speculation; // Maximum guards per speculation
    uint32_t max_speculations;          // Maximum number of active speculations
    double learning_rate;               // Learning rate for updates
    int enable_aggressive_speculation;  // Enable aggressive speculation
    int enable_learning;                // Enable learning from failures
    
    // Active speculations
    SpeculationContext* speculations;   // Array of active speculations
    uint32_t speculation_count;         // Number of active speculations
    uint32_t speculation_capacity;      // Capacity of speculations array
    
    // Guards
    SpeculationGuard* global_guards;    // Global guards array
    uint32_t global_guard_count;        // Number of global guards
    uint32_t max_global_guards;         // Maximum number of global guards
    
    // Statistics
    uint64_t total_speculations;        // Total number of speculations created
    uint64_t successful_speculations;   // Number of successful speculations
    uint64_t failed_speculations;       // Number of failed speculations
    uint64_t deoptimized_speculations;  // Number of deoptimized speculations
    double overall_success_rate;        // Overall success rate
    double average_speedup;             // Average speedup from speculation
    double average_overhead;            // Average overhead from guards
    double net_benefit;                 // Net benefit from all speculations
    
    // Performance tracking
    uint64_t speculation_start_time;    // Speculation start time
    uint64_t speculation_end_time;      // Speculation end time
    double total_speculation_time_ms;   // Total speculation time (milliseconds)
    double speculation_overhead;        // Speculation overhead percentage
} SpeculativeOptimizerContext;

// ============================================================================
// SPECULATIVE OPTIMIZATION FUNCTIONS
// ============================================================================

/**
 * @brief Core speculation functions
 * 
 * These functions handle the main speculative optimization process,
 * from initialization to speculation execution.
 */

/**
 * @brief Create a new speculative optimizer context
 * @return New SpeculativeOptimizerContext, or NULL on failure
 * @note The context is initialized with default settings
 */
SpeculativeOptimizerContext* speculative_optimizer_create(void);

/**
 * @brief Free a speculative optimizer context
 * @param context Context to free (can be NULL)
 * @note This function is safe to call with NULL
 */
void speculative_optimizer_free(SpeculativeOptimizerContext* context);

/**
 * @brief Create a new speculation
 * @param context Optimizer context
 * @param speculation_type Type of speculation
 * @param call_site_id Call site ID
 * @param predicted_pattern Predicted type pattern
 * @param confidence Confidence in speculation
 * @return Speculation ID, or 0 on failure
 * @note This function creates a new speculative optimization
 */
uint32_t speculative_optimizer_create_speculation(SpeculativeOptimizerContext* context,
                                                 SpeculationType speculation_type,
                                                 uint32_t call_site_id,
                                                 TypePattern* predicted_pattern,
                                                 double confidence);

/**
 * @brief Execute a speculation
 * @param context Optimizer context
 * @param speculation_id Speculation ID
 * @param input_data Input data for execution
 * @param input_size Size of input data
 * @return Speculation result
 * @note This function executes a speculative optimization
 */
SpeculationResult speculative_optimizer_execute_speculation(SpeculativeOptimizerContext* context,
                                                           uint32_t speculation_id,
                                                           const void* input_data,
                                                           size_t input_size);

/**
 * @brief Deoptimize a speculation
 * @param context Optimizer context
 * @param speculation_id Speculation ID
 * @param reason Reason for deoptimization
 * @return 1 on success, 0 on failure
 * @note This function deoptimizes a speculation and falls back to safe code
 */
int speculative_optimizer_deoptimize_speculation(SpeculativeOptimizerContext* context,
                                                uint32_t speculation_id,
                                                const char* reason);

/**
 * @brief Guard management functions
 * 
 * These functions handle the creation and management of speculation guards.
 */

/**
 * @brief Add a guard to a speculation
 * @param context Optimizer context
 * @param speculation_id Speculation ID
 * @param guard_type Type of guard
 * @param register_id Register being guarded
 * @param expected_value Expected value (for value guards)
 * @param expected_type Expected type (for type guards)
 * @return Guard ID, or 0 on failure
 * @note This function adds a guard condition to a speculation
 */
uint32_t speculative_optimizer_add_guard(SpeculativeOptimizerContext* context,
                                        uint32_t speculation_id,
                                        GuardType guard_type,
                                        uint32_t register_id,
                                        uint64_t expected_value,
                                        uint8_t expected_type);

/**
 * @brief Check a guard condition
 * @param context Optimizer context
 * @param guard_id Guard ID
 * @param actual_value Actual value to check
 * @param actual_type Actual type to check
 * @return 1 if guard passes, 0 if guard fails
 * @note This function checks whether a guard condition is satisfied
 */
int speculative_optimizer_check_guard(SpeculativeOptimizerContext* context,
                                     uint32_t guard_id,
                                     uint64_t actual_value,
                                     uint8_t actual_type);

/**
 * @brief Remove a guard from a speculation
 * @param context Optimizer context
 * @param speculation_id Speculation ID
 * @param guard_id Guard ID to remove
 * @return 1 on success, 0 on failure
 * @note This function removes a guard from a speculation
 */
int speculative_optimizer_remove_guard(SpeculativeOptimizerContext* context,
                                      uint32_t speculation_id,
                                      uint32_t guard_id);

/**
 * @brief Get all guards for a speculation
 * @param context Optimizer context
 * @param speculation_id Speculation ID
 * @param guards Array to store guards
 * @param max_guards Maximum number of guards to return
 * @return Number of guards returned
 * @note This function retrieves all guards for a speculation
 */
uint32_t speculative_optimizer_get_guards(SpeculativeOptimizerContext* context,
                                         uint32_t speculation_id,
                                         SpeculationGuard** guards,
                                         uint32_t max_guards);

/**
 * @brief Learning functions
 * 
 * These functions handle learning from speculation results.
 */

/**
 * @brief Learn from speculation success
 * @param context Optimizer context
 * @param speculation_id Speculation ID
 * @param execution_time Execution time (nanoseconds)
 * @return 1 on success, 0 on failure
 * @note This function updates learning based on successful speculation
 */
int speculative_optimizer_learn_success(SpeculativeOptimizerContext* context,
                                       uint32_t speculation_id,
                                       uint64_t execution_time);

/**
 * @brief Learn from speculation failure
 * @param context Optimizer context
 * @param speculation_id Speculation ID
 * @param failure_reason Reason for failure
 * @return 1 on success, 0 on failure
 * @note This function updates learning based on failed speculation
 */
int speculative_optimizer_learn_failure(SpeculativeOptimizerContext* context,
                                       uint32_t speculation_id,
                                       const char* failure_reason);

/**
 * @brief Update speculation confidence
 * @param context Optimizer context
 * @param speculation_id Speculation ID
 * @param new_confidence New confidence level
 * @return 1 on success, 0 on failure
 * @note This function updates the confidence level of a speculation
 */
int speculative_optimizer_update_confidence(SpeculativeOptimizerContext* context,
                                           uint32_t speculation_id,
                                           double new_confidence);

/**
 * @brief Management functions
 * 
 * These functions manage speculations and their lifecycle.
 */

/**
 * @brief Get a speculation by ID
 * @param context Optimizer context
 * @param speculation_id Speculation ID
 * @return Speculation context, or NULL if not found
 * @note This function retrieves a speculation by its ID
 */
SpeculationContext* speculative_optimizer_get_speculation(SpeculativeOptimizerContext* context,
                                                         uint32_t speculation_id);

/**
 * @brief Get all active speculations
 * @param context Optimizer context
 * @param speculations Array to store speculations
 * @param max_speculations Maximum number of speculations to return
 * @return Number of speculations returned
 * @note This function retrieves all active speculations
 */
uint32_t speculative_optimizer_get_all_speculations(SpeculativeOptimizerContext* context,
                                                   SpeculationContext** speculations,
                                                   uint32_t max_speculations);

/**
 * @brief Remove a speculation
 * @param context Optimizer context
 * @param speculation_id Speculation ID to remove
 * @return 1 on success, 0 on failure
 * @note This function removes a speculation and cleans up its resources
 */
int speculative_optimizer_remove_speculation(SpeculativeOptimizerContext* context,
                                            uint32_t speculation_id);

/**
 * @brief Clear all speculations
 * @param context Optimizer context
 * @note This function removes all speculations
 */
void speculative_optimizer_clear_speculations(SpeculativeOptimizerContext* context);

/**
 * @brief Configuration functions
 * 
 * These functions configure the speculative optimizer behavior.
 */

/**
 * @brief Set confidence threshold
 * @param context Optimizer context
 * @param threshold Confidence threshold (0.0-1.0)
 * @note This function sets the minimum confidence for speculation
 */
void speculative_optimizer_set_confidence_threshold(SpeculativeOptimizerContext* context, double threshold);

/**
 * @brief Set success rate threshold
 * @param context Optimizer context
 * @param threshold Success rate threshold (0.0-1.0)
 * @note This function sets the minimum success rate to continue speculation
 */
void speculative_optimizer_set_success_rate_threshold(SpeculativeOptimizerContext* context, double threshold);

/**
 * @brief Set learning rate
 * @param context Optimizer context
 * @param rate Learning rate (0.0-1.0)
 * @note This function sets the learning rate for updates
 */
void speculative_optimizer_set_learning_rate(SpeculativeOptimizerContext* context, double rate);

/**
 * @brief Enable or disable aggressive speculation
 * @param context Optimizer context
 * @param enable 1 to enable, 0 to disable
 * @note This function enables or disables aggressive speculation
 */
void speculative_optimizer_set_aggressive_speculation(SpeculativeOptimizerContext* context, int enable);

/**
 * @brief Enable or disable learning
 * @param context Optimizer context
 * @param enable 1 to enable, 0 to disable
 * @note This function enables or disables learning from failures
 */
void speculative_optimizer_set_learning(SpeculativeOptimizerContext* context, int enable);

/**
 * @brief Utility functions
 * 
 * These functions provide utility functionality for working with
 * speculative optimizer contexts and speculations.
 */

/**
 * @brief Get speculation statistics
 * @param context Optimizer context
 * @return Statistics string
 * @note Returns a formatted string with speculation statistics
 */
char* speculative_optimizer_get_statistics(SpeculativeOptimizerContext* context);

/**
 * @brief Print speculation information
 * @param context Optimizer context
 * @param speculation_id Speculation ID to print
 * @note Prints human-readable representation of a speculation
 */
void speculative_optimizer_print_speculation(SpeculativeOptimizerContext* context, uint32_t speculation_id);

/**
 * @brief Print all speculations
 * @param context Optimizer context
 * @note Prints human-readable representation of all speculations
 */
void speculative_optimizer_print_all_speculations(SpeculativeOptimizerContext* context);

/**
 * @brief Print guard information
 * @param context Optimizer context
 * @param guard_id Guard ID to print
 * @note Prints human-readable representation of a guard
 */
void speculative_optimizer_print_guard(SpeculativeOptimizerContext* context, uint32_t guard_id);

/**
 * @brief Validate speculation
 * @param context Optimizer context
 * @param speculation_id Speculation ID to validate
 * @return 1 if valid, 0 if invalid
 * @note Validates that a speculation is correct and safe
 */
int speculative_optimizer_validate_speculation(SpeculativeOptimizerContext* context, uint32_t speculation_id);

/**
 * @brief Export speculation data to file
 * @param context Optimizer context
 * @param filename Output filename
 * @return 1 on success, 0 on failure
 * @note Exports speculation data to a file for analysis
 */
int speculative_optimizer_export_data(SpeculativeOptimizerContext* context, const char* filename);

/**
 * @brief Import speculation data from file
 * @param context Optimizer context
 * @param filename Input filename
 * @return 1 on success, 0 on failure
 * @note Imports speculation data from a file
 */
int speculative_optimizer_import_data(SpeculativeOptimizerContext* context, const char* filename);

#endif // SPECULATIVE_OPTIMIZER_H
