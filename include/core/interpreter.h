#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "ast.h"
#include <stddef.h>

// Value types
typedef enum {
    VALUE_NULL,
    VALUE_BOOLEAN,
    VALUE_NUMBER,
    VALUE_STRING,
    VALUE_RANGE,
    VALUE_ARRAY,
    VALUE_OBJECT,
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
        ASTNode* body;
        char** parameters;
        size_t parameter_count;
        char* return_type;
    } function_value;
    struct {
        char* class_name;
        void* instance;
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
Value value_create_function(ASTNode* body, char** params, size_t param_count, const char* return_type);
Value value_create_class(const char* name, void* instance);
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

// Function execution
Value value_function_call(Value* func, Value* args, size_t arg_count, Interpreter* interpreter, int line, int column);

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
