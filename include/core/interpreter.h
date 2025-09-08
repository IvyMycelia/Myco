#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "ast.h"
#include <stddef.h>

// Forward declarations
typedef struct Environment Environment;

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
        Environment* captured_environment;  // For closures
    } function_value;
    struct {
        char* class_name;
        char* parent_class_name;  // Name of parent class for inheritance
        ASTNode* class_body;  // The class definition body (fields and methods)
        Environment* class_environment;  // Environment for class-level variables
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

// Value structure
typedef struct {
    ValueType type;
    ValueData data;
} Value;

// Environment for variable storage
typedef struct Environment {
    struct Environment* parent;
    char** names;
    Value* values;
    size_t count;
    size_t capacity;
} Environment;

// Call stack frame for stack traces
typedef struct CallFrame {
    const char* function_name;    // Function or method name
    const char* file_name;        // Source file name
    int line;                     // Line number
    int column;                   // Column number
    struct CallFrame* next;       // Next frame in stack
} CallFrame;

// Interpreter structure
typedef struct {
    Environment* global_environment;
    Environment* current_environment;
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
} Interpreter;

// Interpreter initialization and cleanup
Interpreter* interpreter_create(void);
void interpreter_free(Interpreter* interpreter);
void interpreter_reset(Interpreter* interpreter);

// Environment management
Environment* environment_create(Environment* parent);
void environment_free(Environment* env);
void environment_define(Environment* env, const char* name, Value value);
Value environment_get(Environment* env, const char* name);
void environment_assign(Environment* env, const char* name, Value value);
Environment* environment_copy(Environment* env);
int environment_exists(Environment* env, const char* name);

// Value creation and management
Value value_create_null(void);
Value value_create_boolean(int value);
Value value_create_number(double value);
Value value_create_builtin_function(Value (*func)(Interpreter*, Value*, size_t, int, int));
Value value_create_string(const char* value);
Value value_create_range(double start, double end, double step, int inclusive);
Value value_create_array(size_t initial_capacity);
Value value_create_object(size_t initial_capacity);
void value_object_set_member(Value* object, const char* member_name, Value member_value);
Value value_create_function(ASTNode* body, ASTNode** params, size_t param_count, const char* return_type, Environment* captured_env);
Value value_create_class(const char* name, const char* parent_name, ASTNode* class_body, Environment* class_env);
Value value_create_module(const char* name, void* exports);
Value value_create_error(const char* message, int code);

// Value operations
void value_free(Value* value);
Value value_clone(Value* value);
int value_is_truthy(Value* value);
int value_equals(Value* a, Value* b);
Value value_less_than(Value* a, Value* b);
Value value_greater_than(Value* a, Value* b);

// Value conversion
Value value_to_boolean(Value* value);
Value value_to_number(Value* value);
Value value_to_string(Value* value);

// Arithmetic operations
Value value_add(Value* a, Value* b);
Value value_subtract(Value* a, Value* b);
Value value_multiply(Value* a, Value* b);
Value value_divide(Value* a, Value* b);
Value value_modulo(Value* a, Value* b);
Value value_power(Value* a, Value* b);

// Comparison operations
Value value_equal(Value* a, Value* b);
Value value_not_equal(Value* a, Value* b);
Value value_less_than(Value* a, Value* b);
Value value_less_equal(Value* a, Value* b);
Value value_greater_than(Value* a, Value* b);
Value value_greater_equal(Value* a, Value* b);

// Logical operations
Value value_logical_and(Value* a, Value* b);
Value value_logical_or(Value* a, Value* b);
Value value_logical_xor(Value* a, Value* b);
Value value_logical_not(Value* a);

// Bitwise operations
Value value_bitwise_and(Value* a, Value* b);
Value value_bitwise_or(Value* a, Value* b);
Value value_bitwise_xor(Value* a, Value* b);
Value value_bitwise_not(Value* a);
Value value_left_shift(Value* a, Value* b);
Value value_right_shift(Value* a, Value* b);

// Array operations
void value_array_push(Value* array, Value element);
Value value_array_pop(Value* array);
Value value_array_get(Value* array, size_t index);
void value_array_set(Value* array, size_t index, Value element);
size_t value_array_length(Value* array);

// Object operations
void value_object_set(Value* obj, const char* key, Value value);
Value value_object_get(Value* obj, const char* key);
int value_object_has(Value* obj, const char* key);
void value_object_delete(Value* obj, const char* key);
char** value_object_keys(Value* obj, size_t* count);

// Hash map operations
Value value_create_hash_map(size_t initial_capacity);
void value_hash_map_set(Value* map, Value key, Value value);
Value value_hash_map_get(Value* map, Value key);
int value_hash_map_has(Value* map, Value key);
void value_hash_map_delete(Value* map, Value key);
Value* value_hash_map_keys(Value* map, size_t* count);
size_t value_hash_map_size(Value* map);
int value_equals(Value* a, Value* b);  // Compare two values for equality

// Set operations
Value value_create_set(size_t initial_capacity);
void value_set_add(Value* set, Value element);
int value_set_has(Value* set, Value element);
void value_set_remove(Value* set, Value element);
size_t value_set_size(Value* set);
Value value_set_to_array(Value* set);

// Function execution
Value value_function_call(Value* func, Value* args, size_t arg_count, Interpreter* interpreter, int line, int column);
Value value_function_call_with_self(Value* func, Value* args, size_t arg_count, Interpreter* interpreter, Value* self, int line, int column);

// Method call handling
Value handle_method_call(Interpreter* interpreter, ASTNode* call_node, Value object);
Value handle_super_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name);
Value handle_tree_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object);
Value handle_graph_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object);
Value handle_heap_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object);
Value handle_queue_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object);
Value handle_stack_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object);
Value handle_server_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object);
Value handle_request_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object);
Value handle_response_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object);

// Request body parsing function declarations
Value builtin_request_json(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_request_form(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_request_query(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);

// Execution functions
Value interpreter_execute(Interpreter* interpreter, ASTNode* node);
Value interpreter_execute_program(Interpreter* interpreter, ASTNode* node);
Value interpreter_execute_statement(Interpreter* interpreter, ASTNode* node);
Value interpreter_execute_expression(Interpreter* interpreter, ASTNode* node);
Value interpreter_execute_binary_op(Interpreter* interpreter, ASTNode* node);
Value interpreter_execute_unary_op(Interpreter* interpreter, ASTNode* node);
Value interpreter_execute_assignment(Interpreter* interpreter, ASTNode* node);
Value interpreter_execute_function_call(Interpreter* interpreter, ASTNode* node);
Value interpreter_execute_variable_declaration(Interpreter* interpreter, ASTNode* node);
Value interpreter_execute_if_statement(Interpreter* interpreter, ASTNode* node);
Value interpreter_execute_while_loop(Interpreter* interpreter, ASTNode* node);
Value interpreter_execute_for_loop(Interpreter* interpreter, ASTNode* node);
Value interpreter_execute_try_catch(Interpreter* interpreter, ASTNode* node);
Value interpreter_execute_switch(Interpreter* interpreter, ASTNode* node);
Value interpreter_execute_match(Interpreter* interpreter, ASTNode* node);
Value interpreter_execute_block(Interpreter* interpreter, ASTNode* node);
Value interpreter_execute_return(Interpreter* interpreter, ASTNode* node);
Value interpreter_execute_break(Interpreter* interpreter, ASTNode* node);
Value interpreter_execute_continue(Interpreter* interpreter, ASTNode* node);
Value interpreter_execute_throw(Interpreter* interpreter, ASTNode* node);
Value interpreter_execute_function_declaration(Interpreter* interpreter, ASTNode* node);
Value interpreter_execute_class_declaration(Interpreter* interpreter, ASTNode* node);
Value interpreter_execute_import(Interpreter* interpreter, ASTNode* node);
Value interpreter_execute_module(Interpreter* interpreter, ASTNode* node);
Value interpreter_execute_package(Interpreter* interpreter, ASTNode* node);

// Control flow
void interpreter_set_return(Interpreter* interpreter, Value value);
void interpreter_set_error(Interpreter* interpreter, const char* message, int line, int column);
void interpreter_clear_error(Interpreter* interpreter);
int interpreter_has_error(Interpreter* interpreter);
int interpreter_has_return(Interpreter* interpreter);

// Enhanced error handling with stack traces
void interpreter_push_call_frame(Interpreter* interpreter, const char* function_name, const char* file_name, int line, int column);
void interpreter_pop_call_frame(Interpreter* interpreter);
void interpreter_print_stack_trace(Interpreter* interpreter);
void interpreter_set_error_with_stack(Interpreter* interpreter, const char* message, int line, int column);

// Exception handling
void interpreter_throw_exception(Interpreter* interpreter, const char* message, int line, int column);
int interpreter_has_exception(Interpreter* interpreter);
void interpreter_clear_exception(Interpreter* interpreter);

// Built-in functions
Value builtin_print(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_input(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_len(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_type(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_assert(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_str(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_int(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_float(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_bool(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);

// Utility functions
void interpreter_register_builtins(Interpreter* interpreter);
const char* value_type_to_string(ValueType type);
void value_print(Value* value);
void value_print_debug(Value* value);

#endif // INTERPRETER_H
