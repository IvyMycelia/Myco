#ifndef INTERPRETER_CORE_H
#define INTERPRETER_CORE_H

#include "../ast.h"
#include "../jit_compiler.h"
#include "../../runtime/runtime.h"
#include "../../utils/shared_utilities.h"
#include <stddef.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h>

// ============================================================================
// INTERPRETER CORE STRUCTURES AND LIFECYCLE
// ============================================================================

// Forward declarations
struct Environment;
typedef struct Environment Environment;
// Value is forward declared here to avoid circular dependency
// The full definition is in value_operations.h which is included later
struct Value;
typedef struct Value Value;
// BytecodeProgram is defined in bytecode.h, which is included via ast.h

// Class metadata structure (compiled from AST at definition time)
typedef struct {
    const char* name;
    const char* type;  // Type annotation (e.g., "Int", "String")
    ASTNode* default_value;  // Default value AST (compiled on instantiation)
} ClassFieldMetadata;

typedef struct {
    const char* name;
    int bytecode_func_id;  // Bytecode function ID
    const char** param_names;
    size_t param_count;
    const char* return_type;
} ClassMethodMetadata;

typedef struct {
    ClassFieldMetadata* fields;
    size_t field_count;
    ClassMethodMetadata* methods;
    size_t method_count;
} ClassMetadata;

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
        uint64_t promise_id;    // Unique ID for this promise (for registry lookup)
        int is_resolved;
        int is_rejected;
        Value* resolved_value;  // Actual resolved value (NULL if not resolved)
        Value* rejected_value;  // Actual rejected/error value (NULL if not rejected)
        void** then_callbacks;  // Array of callback functions for .then()
        void** catch_callbacks; // Array of callback functions for .catch()
        size_t then_count;
        size_t catch_count;
        size_t then_capacity;
        size_t catch_capacity;
    } promise_value;
    struct {
        char* class_name;
        char* parent_class_name;  // Name of parent class for inheritance
        ASTNode* class_body;  // The class definition body (fields and methods) - DEPRECATED, use metadata instead
        struct Environment* class_environment;  // Environment for class scope
        ClassMetadata* metadata;  // NULL if not compiled yet, non-NULL if compiled to bytecode
    } class_value;
    struct {
        char* module_name;
        char* module_path;
        struct Environment* module_environment;  // Environment for module scope
        struct Environment* exports;  // Exported symbols
        int is_loaded;  // Whether the module has been loaded
    } module_value;
    struct {
        char* error_message;
        char* error_type;  // Type of error (e.g., "TypeError", "ValueError")
        int error_line;
        int error_column;
    } error_value;
} ValueData;

// Value cache structure
typedef struct {
    void* cached_ptr;
    double cached_numeric;
    size_t cached_length;
} ValueCache;

// Value structure
typedef struct Value {
    ValueType type;
    ValueData data;
    uint8_t flags;  // Optimization flags
    uint32_t ref_count;  // Reference count for memory management
    ValueCache cache;  // Cached data for optimization
} Value;

// ============================================================================
// ENVIRONMENT STRUCTURE
// ============================================================================

struct Environment {
    struct Environment* parent;
    char** names;
    Value* values;
    size_t count;
    size_t capacity;
};

// ============================================================================
// MODULE CACHE STRUCTURES (Phase 4) - Defined after Value and Environment
// ============================================================================

// Module cache entry - stores cached module data
typedef struct ModuleCacheEntry {
    char* file_path;              // Normalized file path (key)
    char* file_hash;              // Hash of file content for invalidation
    time_t file_mtime;            // File modification time
    Environment* module_env;      // Cached module environment
    void* module_value_storage;   // Cached module value (Value*) - stored as void* to avoid circular dependency
    void* module_bytecode_program; // Cached module bytecode program (BytecodeProgram*) - stored as void* to avoid circular dependency
    int is_valid;                 // Whether cache entry is still valid
} ModuleCacheEntry;

// Import chain entry - tracks import path for circular detection
typedef struct ImportChain {
    char* module_path;            // Path of module being imported
    struct ImportChain* next;     // Next in chain (parent import)
} ImportChain;

// ============================================================================
// CALL FRAME STRUCTURE
// ============================================================================

typedef struct CallFrame {
    const char* function_name;
    const char* file_name;
    int line;
    int column;
    const char* source_line;  // The actual source code line
    struct CallFrame* next;
} CallFrame;

// ============================================================================
// ASYNC TASK STRUCTURE
// ============================================================================

typedef struct AsyncTask {
    Value* promise_ptr;         // Pointer to promise associated with this task (so we can update it)
    Value promise_copy;          // Copy of promise for reference (used for cleanup)
    void* program;              // Bytecode program to execute (BytecodeProgram*)
    int function_id;            // Function ID to execute (or -1 for main)
    Value* args;                // Arguments for the function
    size_t arg_count;           // Number of arguments
    Environment* environment;    // Environment for execution
    int is_resolved;            // Whether the task has completed
    Value result;               // Result value (if resolved)
} AsyncTask;

// ============================================================================
// INTERPRETER STRUCTURE
// ============================================================================

typedef struct Interpreter {
    Environment* global_environment;
    Environment* current_environment;
    
    // Return handling
    int has_return;
    Value return_value;
    
    // Error handling
    int has_error;
    char* error_message;
    int error_line;
    int error_column;
    
    // Control flow
    int break_depth;
    int continue_depth;
    int try_depth;
    
    // Type checking
    char* current_function_return_type;
    
    // Class context
    Value* self_context;  // Current 'self' object for method calls
    
    // Enhanced error handling
    CallFrame* call_stack;
    size_t stack_depth;
    size_t max_stack_depth;
    size_t recursion_count;
    
    // Test mode - allows continued execution after errors
    int test_mode;
    size_t max_recursion_depth;
    
    // JIT compilation
    JitContext* jit_context;
    int jit_enabled;
    int jit_mode;
    
    // Macro system
    struct MacroExpander* macro_expander;
    
    // Source tracking
    const char* current_source;
    const char* current_file;
    
    // Performance optimization features
    void* adaptive_executor;
    void* hot_spot_tracker;
    void* micro_jit_context;
    void* value_specializer;
    int benchmark_mode;
    
    // Timing functions (forward declarations)
    void (*interpreter_start_timing)(struct Interpreter*);
    void (*interpreter_end_timing)(struct Interpreter*);
    
    // Bytecode program cache - stores the last compiled program for function calls
    // This allows bytecode functions to be called even after the program is "freed"
    // The program is actually kept alive as long as functions reference it
    struct BytecodeProgram* bytecode_program_cache;
    
    // Module cache - Phase 4: cache parsed modules to avoid re-parsing
    struct ModuleCacheEntry* module_cache;
    size_t module_cache_count;
    size_t module_cache_capacity;
    
    // Circular import detection - track import chain
    struct ImportChain* import_chain;
    
    // Async/await support - event loop and task queue
    struct AsyncTask** task_queue;  // Queue of pending async tasks
    size_t task_queue_size;
    size_t task_queue_capacity;
    int async_enabled;  // Whether async execution is enabled
    
    // Concurrency support
    pthread_t* worker_threads;  // Worker threads for concurrent task execution
    size_t worker_thread_count;  // Number of worker threads
    pthread_mutex_t task_queue_mutex;  // Mutex for task queue access
    pthread_mutex_t promise_registry_mutex;  // Mutex for promise registry access
    pthread_cond_t task_available;  // Condition variable for task availability
    int shutdown_workers;  // Flag to signal worker threads to shutdown
    
    // Promise registry - maps promise IDs to promise Values (so we can update them)
    Value* promise_registry;  // Array of promise Values
    size_t promise_registry_size;
    size_t promise_registry_capacity;
    uint64_t next_promise_id;  // Next promise ID to assign
} Interpreter;

// ============================================================================
// INTERPRETER LIFECYCLE FUNCTIONS
// ============================================================================

Interpreter* interpreter_create(void);
void interpreter_free(Interpreter* interpreter);
void interpreter_reset(Interpreter* interpreter);

// ============================================================================
// INTERPRETER EXECUTION FUNCTIONS
// ============================================================================

Value interpreter_execute(Interpreter* interpreter, ASTNode* node);
Value interpreter_eval_file(Interpreter* interpreter, const char* filename);
Value interpreter_eval_string(Interpreter* interpreter, const char* source);
Value interpreter_execute_program(Interpreter* interpreter, ASTNode* node);

// ============================================================================
// INTERPRETER ERROR HANDLING FUNCTIONS
// ============================================================================

void interpreter_set_error(Interpreter* interpreter, const char* message, int line, int column);
void interpreter_clear_error(Interpreter* interpreter);
int interpreter_has_error(Interpreter* interpreter);
void interpreter_set_test_mode(Interpreter* interpreter, int test_mode);
const char* interpreter_get_error(Interpreter* interpreter);

// ============================================================================
// INTERPRETER CALL STACK FUNCTIONS
// ============================================================================

void interpreter_push_call_frame(Interpreter* interpreter, const char* function_name, const char* file_name, int line, int column);
void interpreter_pop_call_frame(Interpreter* interpreter);
void interpreter_print_stack_trace(Interpreter* interpreter);

// ============================================================================
// ENVIRONMENT FUNCTIONS
// ============================================================================

Environment* environment_create(Environment* parent);
void environment_free(Environment* env);
void environment_define(Environment* env, const char* name, Value value);
Value environment_get(Environment* env, const char* name);
int environment_set(Environment* env, const char* name, Value value);
int environment_exists(Environment* env, const char* name);

// ============================================================================
// INTERPRETER CONTEXT FUNCTIONS
// ============================================================================

void interpreter_set_self_context(Interpreter* interpreter, Value* self);
Value* interpreter_get_self_context(Interpreter* interpreter);

// ============================================================================
// INTERPRETER EXCEPTION FUNCTIONS
// ============================================================================

void interpreter_throw_exception(Interpreter* interpreter, const char* message, int line, int column);
void interpreter_set_source(Interpreter* interpreter, const char* source, const char* filename);

// ============================================================================
// VALUE UTILITY FUNCTIONS
// ============================================================================

void value_print(Value* value);
Value value_to_string(Value* value);
Value value_clone(Value* value);
void value_free(Value* value);
int value_equals(Value* a, Value* b);

#endif // INTERPRETER_CORE_H
