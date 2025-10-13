/**
 * @file trace_recorder.h
 * @brief Trace-based JIT recording system
 * 
 * Captures hot execution paths for JIT compilation. Loop detection,
 * type stability analysis, linear trace IR construction.
 */

#ifndef TRACE_RECORDER_H
#define TRACE_RECORDER_H

#include "register_vm.h"
#include "../interpreter/interpreter_core.h"
#include "../ast.h"
#include <stdint.h>
#include <stddef.h>

// ============================================================================
// TRACE RECORDING DATA STRUCTURES
// ============================================================================

/**
 * @brief Trace recording mode
 * 
 * Defines the mode of operation for the trace recorder.
 * Different modes optimize for different scenarios.
 */
typedef enum {
    TRACE_MODE_DISABLED = 0,       // Trace recording disabled
    TRACE_MODE_LOOPS_ONLY = 1,     // Record only loop traces
    TRACE_MODE_FUNCTIONS_ONLY = 2, // Record only function traces
    TRACE_MODE_ALL = 3,            // Record all eligible traces
    TRACE_MODE_AGGRESSIVE = 4      // Aggressive recording for maximum coverage
} TraceRecordingMode;

/**
 * @brief Trace type classification
 * 
 * Classifies traces based on their structure and optimization potential.
 */
typedef enum {
    TRACE_TYPE_LOOP = 0,           // Loop trace (most common)
    TRACE_TYPE_FUNCTION = 1,       // Function trace
    TRACE_TYPE_BRANCH = 2,         // Branch trace
    TRACE_TYPE_ARITHMETIC = 3,     // Arithmetic-heavy trace
    TRACE_TYPE_MEMORY = 4,         // Memory-intensive trace
    TRACE_TYPE_MIXED = 5           // Mixed trace type
} TraceType;

/**
 * @brief Trace recording state
 * 
 * Tracks the current state of trace recording for a specific execution context.
 */
typedef enum {
    TRACE_STATE_IDLE = 0,          // Not recording
    TRACE_STATE_RECORDING = 1,     // Actively recording
    TRACE_STATE_PAUSED = 2,        // Recording paused
    TRACE_STATE_COMPLETE = 3,      // Recording complete
    TRACE_STATE_ABORTED = 4        // Recording aborted
} TraceRecordingState;

/**
 * @brief Trace instruction
 * 
 * Represents a single instruction in a trace. Traces are linear sequences
 * of instructions without branches, making them ideal for optimization.
 */
typedef struct {
    uint32_t instruction_id;       // Unique instruction identifier
    uint32_t opcode;               // Instruction opcode
    uint32_t dst_reg;              // Destination register
    uint32_t src1_reg;             // First source register
    uint32_t src2_reg;             // Second source register
    uint32_t src3_reg;             // Third source register
    uint64_t immediate;            // Immediate value
    uint32_t offset;               // Jump offset (if applicable)
    
    // Type information
    uint8_t dst_type;              // Destination register type
    uint8_t src1_type;             // First source register type
    uint8_t src2_type;             // Second source register type
    uint8_t src3_type;             // Third source register type
    
    // Execution metadata
    uint64_t execution_count;      // Number of times executed
    double hotness_score;          // Hotness score (0.0-1.0)
    int is_loop_invariant;         // Is this instruction loop-invariant?
    int is_constant;               // Is this instruction constant?
    int is_side_effect_free;       // Is this instruction side-effect free?
    
    // Optimization hints
    int can_vectorize;             // Can this instruction be vectorized?
    int can_inline;                // Can this instruction be inlined?
    int can_specialize;            // Can this instruction be specialized?
    int can_eliminate;             // Can this instruction be eliminated?
} TraceInstruction;

/**
 * @brief Trace guard
 * 
 * Represents a guard condition that must be satisfied for a trace to be valid.
 * Guards are used to ensure type stability and control flow consistency.
 */
typedef struct {
    uint32_t guard_id;             // Unique guard identifier
    uint32_t instruction_id;       // Instruction that created the guard
    uint32_t guard_type;           // Type of guard (type, value, range, etc.)
    uint32_t register_id;          // Register being guarded
    uint64_t expected_value;       // Expected value (for value guards)
    uint8_t expected_type;         // Expected type (for type guards)
    double confidence;             // Guard confidence (0.0-1.0)
    int is_essential;              // Is this guard essential for correctness?
    int has_failed;                // Has this guard ever failed?
    uint64_t failure_count;        // Number of times guard has failed
} TraceGuard;

/**
 * @brief Trace metadata
 * 
 * Contains metadata about a recorded trace, including statistics,
 * optimization opportunities, and execution characteristics.
 */
typedef struct {
    uint32_t trace_id;             // Unique trace identifier
    TraceType trace_type;          // Type of trace
    uint32_t instruction_count;    // Number of instructions in trace
    uint32_t guard_count;          // Number of guards in trace
    uint64_t execution_count;      // Number of times trace executed
    uint64_t total_execution_time; // Total execution time (nanoseconds)
    double average_execution_time; // Average execution time per execution
    double hotness_score;          // Overall hotness score (0.0-1.0)
    
    // Type stability
    int is_type_stable;            // Are all types stable in this trace?
    int is_monomorphic;            // Is this trace monomorphic?
    int is_polymorphic;            // Is this trace polymorphic?
    int is_megamorphic;            // Is this trace megamorphic?
    
    // Optimization opportunities
    int has_loop_invariants;       // Does this trace have loop invariants?
    int has_constant_folding;      // Can this trace be constant folded?
    int has_vectorization;         // Can this trace be vectorized?
    int has_inlining;              // Can this trace be inlined?
    int has_specialization;        // Can this trace be specialized?
    
    // Performance characteristics
    double arithmetic_ratio;       // Ratio of arithmetic instructions
    double memory_ratio;           // Ratio of memory instructions
    double control_ratio;          // Ratio of control flow instructions
    double branch_prediction_rate; // Branch prediction success rate
    
    // Compilation metadata
    int is_compiled;               // Has this trace been compiled?
    int compilation_attempts;      // Number of compilation attempts
    int compilation_successes;     // Number of successful compilations
    double compilation_time;       // Time spent compiling (milliseconds)
    double compilation_quality;    // Quality of compiled code (0.0-1.0)
} TraceMetadata;

/**
 * @brief Recorded trace
 * 
 * Contains a complete recorded trace with instructions, guards, and metadata.
 * This is the primary output of the trace recording process.
 */
typedef struct {
    TraceMetadata metadata;        // Trace metadata
    TraceInstruction* instructions; // Array of trace instructions
    TraceGuard* guards;            // Array of trace guards
    uint32_t* loop_headers;        // Loop header instruction IDs
    uint32_t loop_header_count;    // Number of loop headers
    
    // Source mapping
    ASTNode* source_ast;           // Source AST node (if available)
    uint32_t* source_line_numbers; // Source line numbers for instructions
    uint32_t* source_column_numbers; // Source column numbers for instructions
    
    // Optimization data
    uint32_t* optimization_hints;  // Optimization hints for each instruction
    uint32_t* vectorization_info;  // Vectorization information
    uint32_t* inlining_info;       // Inlining information
    
    // Memory management
    size_t instruction_capacity;   // Capacity of instruction array
    size_t guard_capacity;         // Capacity of guard array
    int is_owned;                  // Does this trace own its memory?
} RecordedTrace;

/**
 * @brief Trace recorder context
 * 
 * Contains the state and configuration for trace recording.
 * This is the main interface for the trace recording system.
 */
typedef struct {
    // Configuration
    TraceRecordingMode mode;       // Recording mode
    uint32_t max_trace_length;     // Maximum trace length
    uint32_t max_guards_per_trace; // Maximum guards per trace
    uint64_t hot_spot_threshold;   // Hot spot detection threshold
    double type_stability_threshold; // Type stability threshold
    
    // Recording state
    TraceRecordingState state;     // Current recording state
    uint32_t current_trace_id;     // Current trace being recorded
    uint32_t current_instruction_id; // Current instruction ID
    uint32_t current_guard_id;     // Current guard ID
    
    // Active traces
    RecordedTrace* active_traces;  // Array of active traces
    uint32_t active_trace_count;   // Number of active traces
    uint32_t max_active_traces;    // Maximum number of active traces
    
    // Trace storage
    RecordedTrace* recorded_traces; // Array of recorded traces
    uint32_t recorded_trace_count; // Number of recorded traces
    uint32_t max_recorded_traces;  // Maximum number of recorded traces
    
    // Statistics
    uint64_t total_traces_recorded; // Total number of traces recorded
    uint64_t total_instructions_recorded; // Total instructions recorded
    uint64_t total_guards_recorded; // Total guards recorded
    double average_trace_length;   // Average trace length
    double average_trace_hotness;  // Average trace hotness
    
    // Performance tracking
    uint64_t recording_start_time; // Recording start time (nanoseconds)
    uint64_t recording_end_time;   // Recording end time (nanoseconds)
    double total_recording_time;   // Total recording time (milliseconds)
    double recording_overhead;     // Recording overhead percentage
} TraceRecorderContext;

// ============================================================================
// TRACE RECORDING FUNCTIONS
// ============================================================================

/**
 * @brief Core recording functions
 * 
 * These functions handle the main trace recording process,
 * from initialization to trace completion.
 */

/**
 * @brief Create a new trace recorder context
 * @param mode Recording mode to use
 * @return New TraceRecorderContext, or NULL on failure
 * @note The context is initialized with default settings
 */
TraceRecorderContext* trace_recorder_create(TraceRecordingMode mode);

/**
 * @brief Free a trace recorder context
 * @param context Context to free (can be NULL)
 * @note This function is safe to call with NULL
 */
void trace_recorder_free(TraceRecorderContext* context);

/**
 * @brief Start recording a new trace
 * @param context Recorder context
 * @param trace_type Type of trace to record
 * @param source_ast Source AST node (optional)
 * @return Trace ID, or 0 on failure
 * @note This function initializes a new trace for recording
 */
uint32_t trace_recorder_start_trace(TraceRecorderContext* context, TraceType trace_type, ASTNode* source_ast);

/**
 * @brief Record an instruction in the current trace
 * @param context Recorder context
 * @param instruction Instruction to record
 * @return 1 on success, 0 on failure
 * @note This function adds an instruction to the current trace
 */
int trace_recorder_record_instruction(TraceRecorderContext* context, TraceInstruction instruction);

/**
 * @brief Record a guard condition in the current trace
 * @param context Recorder context
 * @param guard Guard to record
 * @return 1 on success, 0 on failure
 * @note This function adds a guard to the current trace
 */
int trace_recorder_record_guard(TraceRecorderContext* context, TraceGuard guard);

/**
 * @brief Complete the current trace
 * @param context Recorder context
 * @return 1 on success, 0 on failure
 * @note This function finalizes the current trace and makes it available for compilation
 */
int trace_recorder_complete_trace(TraceRecorderContext* context);

/**
 * @brief Abort the current trace
 * @param context Recorder context
 * @param reason Reason for aborting
 * @note This function aborts the current trace and discards it
 */
void trace_recorder_abort_trace(TraceRecorderContext* context, const char* reason);

/**
 * @brief Analysis functions
 * 
 * These functions analyze traces and execution patterns to provide
 * information for optimization decisions.
 */

/**
 * @brief Analyze trace hotness and optimization potential
 * @param context Recorder context
 * @param trace_id Trace to analyze
 * @return 1 on success, 0 on failure
 * @note This function analyzes a trace for optimization opportunities
 */
int trace_recorder_analyze_trace(TraceRecorderContext* context, uint32_t trace_id);

/**
 * @brief Detect loop structures in a trace
 * @param context Recorder context
 * @param trace_id Trace to analyze
 * @return 1 on success, 0 on failure
 * @note This function identifies loops and their characteristics
 */
int trace_recorder_detect_loops(TraceRecorderContext* context, uint32_t trace_id);

/**
 * @brief Analyze type stability in a trace
 * @param context Recorder context
 * @param trace_id Trace to analyze
 * @return 1 on success, 0 on failure
 * @note This function analyzes type stability across trace executions
 */
int trace_recorder_analyze_type_stability(TraceRecorderContext* context, uint32_t trace_id);

/**
 * @brief Identify optimization opportunities in a trace
 * @param context Recorder context
 * @param trace_id Trace to analyze
 * @return 1 on success, 0 on failure
 * @note This function identifies specific optimization opportunities
 */
int trace_recorder_identify_optimizations(TraceRecorderContext* context, uint32_t trace_id);

/**
 * @brief Calculate trace performance metrics
 * @param context Recorder context
 * @param trace_id Trace to analyze
 * @return 1 on success, 0 on failure
 * @note This function calculates performance metrics for a trace
 */
int trace_recorder_calculate_metrics(TraceRecorderContext* context, uint32_t trace_id);

/**
 * @brief Management functions
 * 
 * These functions manage traces and the recording process.
 */

/**
 * @brief Get a recorded trace by ID
 * @param context Recorder context
 * @param trace_id Trace ID
 * @return Recorded trace, or NULL if not found
 * @note This function retrieves a trace by its ID
 */
RecordedTrace* trace_recorder_get_trace(TraceRecorderContext* context, uint32_t trace_id);

/**
 * @brief Get all recorded traces
 * @param context Recorder context
 * @param traces Array to store traces
 * @param max_traces Maximum number of traces to return
 * @return Number of traces returned
 * @note This function retrieves all recorded traces
 */
uint32_t trace_recorder_get_all_traces(TraceRecorderContext* context, RecordedTrace** traces, uint32_t max_traces);

/**
 * @brief Get hot traces (traces above hotness threshold)
 * @param context Recorder context
 * @param traces Array to store hot traces
 * @param max_traces Maximum number of traces to return
 * @return Number of hot traces returned
 * @note This function retrieves traces that are above the hotness threshold
 */
uint32_t trace_recorder_get_hot_traces(TraceRecorderContext* context, RecordedTrace** traces, uint32_t max_traces);

/**
 * @brief Remove a trace from the recorder
 * @param context Recorder context
 * @param trace_id Trace ID to remove
 * @return 1 on success, 0 on failure
 * @note This function removes a trace from the recorder
 */
int trace_recorder_remove_trace(TraceRecorderContext* context, uint32_t trace_id);

/**
 * @brief Clear all traces from the recorder
 * @param context Recorder context
 * @note This function removes all traces from the recorder
 */
void trace_recorder_clear_traces(TraceRecorderContext* context);

/**
 * @brief Configuration functions
 * 
 * These functions configure the trace recorder behavior and settings.
 */

/**
 * @brief Set recording mode
 * @param context Recorder context
 * @param mode New recording mode
 * @note This function changes the recording mode
 */
void trace_recorder_set_mode(TraceRecorderContext* context, TraceRecordingMode mode);

/**
 * @brief Set maximum trace length
 * @param context Recorder context
 * @param max_length Maximum trace length
 * @note This function sets the maximum length for traces
 */
void trace_recorder_set_max_length(TraceRecorderContext* context, uint32_t max_length);

/**
 * @brief Set hot spot threshold
 * @param context Recorder context
 * @param threshold Hot spot threshold
 * @note This function sets the threshold for hot spot detection
 */
void trace_recorder_set_hot_spot_threshold(TraceRecorderContext* context, uint64_t threshold);

/**
 * @brief Set type stability threshold
 * @param context Recorder context
 * @param threshold Type stability threshold
 * @note This function sets the threshold for type stability analysis
 */
void trace_recorder_set_type_stability_threshold(TraceRecorderContext* context, double threshold);

/**
 * @brief Utility functions
 * 
 * These functions provide utility functionality for working with
 * trace recorder contexts and traces.
 */

/**
 * @brief Get recording statistics
 * @param context Recorder context
 * @return Statistics string
 * @note Returns a formatted string with recording statistics
 */
char* trace_recorder_get_statistics(TraceRecorderContext* context);

/**
 * @brief Print trace information
 * @param context Recorder context
 * @param trace_id Trace ID to print
 * @note Prints human-readable representation of a trace
 */
void trace_recorder_print_trace(TraceRecorderContext* context, uint32_t trace_id);

/**
 * @brief Print all traces
 * @param context Recorder context
 * @note Prints human-readable representation of all traces
 */
void trace_recorder_print_all_traces(TraceRecorderContext* context);

/**
 * @brief Validate trace consistency
 * @param context Recorder context
 * @param trace_id Trace ID to validate
 * @return 1 if valid, 0 if invalid
 * @note Validates that a trace is consistent and correct
 */
int trace_recorder_validate_trace(TraceRecorderContext* context, uint32_t trace_id);

/**
 * @brief Export trace to file
 * @param context Recorder context
 * @param trace_id Trace ID to export
 * @param filename Output filename
 * @return 1 on success, 0 on failure
 * @note Exports a trace to a file for analysis
 */
int trace_recorder_export_trace(TraceRecorderContext* context, uint32_t trace_id, const char* filename);

/**
 * @brief Import trace from file
 * @param context Recorder context
 * @param filename Input filename
 * @return Trace ID, or 0 on failure
 * @note Imports a trace from a file
 */
uint32_t trace_recorder_import_trace(TraceRecorderContext* context, const char* filename);

#endif // TRACE_RECORDER_H
