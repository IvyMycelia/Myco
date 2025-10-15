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
        struct Environment* class_environment;  // Environment for class scope
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
