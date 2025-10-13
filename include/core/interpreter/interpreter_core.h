#ifndef INTERPRETER_CORE_H
#define INTERPRETER_CORE_H

#include "../ast.h"
#include "../jit_compiler.h"
#include "../../runtime/runtime.h"
#include "../../utils/shared_utilities.h"
#include <stddef.h>
#include <stdint.h>

// ============================================================================
// INTERPRETER CORE STRUCTURES AND LIFECYCLE
// ============================================================================

// Forward declarations
struct Environment;
typedef struct Environment Environment;

// Value optimization flags
#define VALUE_FLAG_CACHED     0x01    // Value has cached data
#define VALUE_FLAG_IMMUTABLE  0x02    // Value cannot be modified
#define VALUE_FLAG_REFCOUNTED   0x04    // Value uses reference counting
#define VALUE_FLAG_POOLED     0x08    // Value allocated from pool

// Value types
typedef enum {
    VALUE_NULL,
    VALUE_BOOLEAN,
    VALUE_NUMBER,
    VALUE_STRING,
    VALUE_RANGE,
    VALUE_ARRAY,
    VALUE_OBJECT,
    VALUE_HASH_MAP,
    VALUE_SET,
    VALUE_FUNCTION,
    VALUE_ASYNC_FUNCTION,
    VALUE_PROMISE,
    VALUE_CLASS,
    VALUE_MODULE,
    VALUE_ERROR
} ValueType;

// Value union
typedef union {
    int boolean_value;
    double number_value;
    char* string_value;
    struct {
        double start;
        double end;
        double step;
        int inclusive;
    } range_value;
    struct {
        void** elements;
        size_t count;
        size_t capacity;
    } array_value;
    struct {
        char** keys;
        void** values;
        size_t count;
        size_t capacity;
    } object_value;
    struct {
        void** keys;
        void** values;
        size_t count;
        size_t capacity;
    } hash_map_value;
    struct {
        void** elements;
        size_t count;
        size_t capacity;
    } set_value;
    struct {
        ASTNode* body;
        ASTNode** parameters;
        size_t parameter_count;
        char* return_type;
        struct Environment* captured_environment;  // For closures
    } function_value;
    struct {
        ASTNode* body;
        ASTNode** parameters;
        size_t parameter_count;
        char* return_type;
        struct Environment* captured_environment;  // For closures
    } async_function_value;
    struct {
        int is_resolved;
        int is_rejected;
        char* error_message;
        char* resolved_data;  // Simple string representation for now
    } promise_value;
    struct {
        char* class_name;
        char* parent_class_name;  // Name of parent class for inheritance
        ASTNode* class_body;  // The class definition body (fields and methods)
        struct Environment* class_environment;  // Environment for class-level variables
    } class_value;
    struct {
        char* module_name;
        void* exports;
    } module_value;
    struct {
        char* error_message;
        int error_code;
    } error_value;
} ValueData;

// Optimized Value structure with better memory layout
typedef struct {
    ValueType type;
    uint8_t flags;          // Flags for optimization (cached, immutable, etc.)
    uint16_t ref_count;       // Reference count for garbage collection
    ValueData data;
    // Cache for frequently accessed data
    union {
        size_t cached_length;    // For strings, arrays
        double cached_numeric;   // For numbers
        void* cached_ptr;        // For objects, functions
    } cache;
} Value;

// Environment for variable storage
struct Environment {
    struct Environment* parent;
    char** names;
    Value* values;
    size_t count;
    size_t capacity;
};

// Call stack frame for stack traces
typedef struct CallFrame {
    const char* function_name;    // Function or method name
    const char* file_name;        // Source file name
    int line;                     // Line number
    int column;                   // Column number
    const char* source_line;      // Actual source code line content
    struct CallFrame* next;       // Next frame in stack
} CallFrame;

// Interpreter structure
typedef struct Interpreter {
    struct Environment* global_environment;
    struct Environment* current_environment;
    Value return_value;
    int has_return;
    int has_error;
    char* error_message;
    int error_line;
    int error_column;
    int break_depth;
    int continue_depth;
    int try_depth;
    const char* current_function_return_type;  // Track return type for type checking
    Value* self_context; // Current self object for method calls
    
    // Enhanced error handling
    CallFrame* call_stack;        // Call stack for stack traces
    int stack_depth;              // Current stack depth
    int max_stack_depth;          // Maximum allowed stack depth
    int recursion_count;          // Recursion counter for limit detection
    int max_recursion_depth;      // Maximum recursion depth
    
    // Source text for line extraction
    const char* current_source;   // Current source text (for line extraction)
    const char* current_filename; // Current filename (stdin, file path, etc.)
    
    // JIT compilation support
    JitContext* jit_context;
    int jit_enabled;
    int jit_mode;
    
    // Macro system support
    struct MacroExpander* macro_expander;
    
    // Compile-time evaluation support
    struct CompileTimeEvaluator* compile_time_evaluator;
    
    // Benchmark timing support
    int benchmark_mode;           // Enable timing instrumentation
    uint64_t execution_time_ns;   // Total execution time in nanoseconds
    uint64_t start_time_ns;       // Start time for current execution
    
    // Hot spot tracking support
    void* hot_spot_tracker;       // HotSpotTracker instance
    
    // Micro-JIT compiler support
    void* micro_jit_context;      // MicroJitContext instance
    
    // Value specialization support
    void* value_specializer;      // ValueSpecializer instance
    
    // Adaptive executor support
    void* adaptive_executor;      // AdaptiveExecutor instance
} Interpreter;

// Environment management
Environment* environment_create(Environment* parent);
void environment_free(Environment* env);
void environment_define(Environment* env, const char* name, Value value);
Value environment_get(Environment* env, const char* name);
void environment_assign(Environment* env, const char* name, Value value);
Environment* environment_copy(Environment* env);
int environment_exists(Environment* env, const char* name);
void environment_set(Environment* env, const char* name, Value value);

// Interpreter lifecycle functions
Interpreter* interpreter_create(void);
void interpreter_free(Interpreter* interpreter);
void interpreter_reset(Interpreter* interpreter);

// Global system initialization
void interpreter_initialize_global_systems(void);
void interpreter_cleanup_global_systems(void);

// Environment management
Environment* interpreter_get_global_environment(Interpreter* interpreter);
Environment* interpreter_get_current_environment(Interpreter* interpreter);
void interpreter_set_current_environment(Interpreter* interpreter, Environment* env);

// Error handling
int interpreter_has_error(Interpreter* interpreter);
const char* interpreter_get_error(Interpreter* interpreter);
void interpreter_set_error(Interpreter* interpreter, const char* message, int line, int column);
void interpreter_clear_error(Interpreter* interpreter);
void interpreter_throw_exception(Interpreter* interpreter, const char* message, int line, int column);
int interpreter_has_exception(Interpreter* interpreter);
void interpreter_clear_exception(Interpreter* interpreter);

// Return value management
Value interpreter_get_return_value(Interpreter* interpreter);
void interpreter_set_return_value(Interpreter* interpreter, Value value);
int interpreter_has_return(Interpreter* interpreter);
void interpreter_set_return(Interpreter* interpreter, int has_return);

// Self context for method calls
Value* interpreter_get_self_context(Interpreter* interpreter);
void interpreter_set_self_context(Interpreter* interpreter, Value* self);

// File context
const char* interpreter_get_current_filename(Interpreter* interpreter);
void interpreter_set_current_filename(Interpreter* interpreter, const char* filename);

// Call stack management
void interpreter_push_call_frame(Interpreter* interpreter, const char* function_name, const char* file_name, int line, int column);
void interpreter_pop_call_frame(Interpreter* interpreter);

// Try-catch depth
int interpreter_get_try_depth(Interpreter* interpreter);
void interpreter_set_try_depth(Interpreter* interpreter, int depth);

// JIT and macro system integration
void interpreter_initialize_jit(Interpreter* interpreter);
void interpreter_initialize_macros(Interpreter* interpreter);

// Source management
void interpreter_set_source(Interpreter* interpreter, const char* source, const char* filename);

// Program execution
Value interpreter_execute_program(Interpreter* interpreter, ASTNode* node);

// Value printing
void value_print(Value* value);
void value_print_debug(Value* value);

// Benchmark timing functions
void interpreter_set_benchmark_mode(Interpreter* interpreter, int enabled);
uint64_t interpreter_get_execution_time_ns(Interpreter* interpreter);
void interpreter_start_timing(Interpreter* interpreter);
void interpreter_stop_timing(Interpreter* interpreter);

#endif // INTERPRETER_CORE_H
