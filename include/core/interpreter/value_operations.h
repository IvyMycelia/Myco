#ifndef VALUE_OPERATIONS_H
#define VALUE_OPERATIONS_H

#include "interpreter_core.h"
#include <stddef.h>

// ============================================================================
// PRIMITIVE VALUE CREATION FUNCTIONS
// ============================================================================

// Basic value creation
Value value_create_null(void);
Value value_create_boolean(int value);
Value value_create_number(double value);
Value value_create_string(const char* value);
Value value_create_range(double start, double end, double step, int inclusive);

// Optimized value creation
Value value_create_optimized(ValueType type, uint8_t flags);
Value value_create_cached_string(const char* value);
Value value_create_immutable_number(double value);
Value value_create_pooled_array(size_t initial_capacity);

// Reference counting
void value_increment_ref(Value* value);
void value_decrement_ref(Value* value);

// Value flag checking
int value_is_cached(Value* value);
int value_is_immutable(Value* value);
int value_is_pooled(Value* value);

// ============================================================================
// COLLECTION VALUE CREATION FUNCTIONS
// ============================================================================

// Array operations
Value value_create_array(size_t initial_capacity);
void value_array_push(Value* array, Value element);
Value value_array_pop(Value* array, int index);
Value value_array_get(Value* array, size_t index);
void value_array_set(Value* array, size_t index, Value element);
size_t value_array_length(Value* array);

// Object operations
Value value_create_object(size_t initial_capacity);
void value_object_set_member(Value* object, const char* member_name, Value member_value);
void value_object_set(Value* obj, const char* key, Value value);
Value value_object_get(Value* obj, const char* key);
int value_object_has(Value* obj, const char* key);
void value_object_delete(Value* obj, const char* key);
size_t value_object_size(Value* obj);
char** value_object_keys(Value* obj, size_t* count);

// Hash map operations
Value value_create_hash_map(size_t initial_capacity);
void value_hash_map_set(Value* map, Value key, Value value);
Value value_hash_map_get(Value* map, Value key);
int value_hash_map_has(Value* map, Value key);
void value_hash_map_delete(Value* map, Value key);
Value* value_hash_map_keys(Value* map, size_t* count);
size_t value_hash_map_size(Value* map);

// Set operations
Value value_create_set(size_t initial_capacity);
void value_set_add(Value* set, Value element);
int value_set_has(Value* set, Value element);
void value_set_remove(Value* set, Value element);
size_t value_set_size(Value* set);
Value value_set_to_array(Value* set);

// ============================================================================
// FUNCTION VALUE CREATION FUNCTIONS
// ============================================================================

// Function creation
Value value_create_function(ASTNode* body, ASTNode** params, size_t param_count, const char* return_type, Environment* captured_env);
Value value_create_async_function(const char* name, ASTNode** params, size_t param_count, const char* return_type, ASTNode* body, Environment* captured_env);
Value value_create_promise(Value resolved_value, int is_resolved, Value error_value);
Value value_create_builtin_function(Value (*func)(Interpreter*, Value*, size_t, int, int));

// Function calling
Value value_function_call(Value* func, Value* args, size_t arg_count, Interpreter* interpreter, int line, int column);
Value value_function_call_with_self(Value* func, Value* args, size_t arg_count, Interpreter* interpreter, Value* self, int line, int column);

// ============================================================================
// CLASS VALUE CREATION FUNCTIONS
// ============================================================================

// Class creation
Value value_create_class(const char* name, const char* parent_name, ASTNode* class_body, Environment* class_env);
void compile_class_metadata(Interpreter* interpreter, Value* class_value);

// Class instantiation
Value create_class_instance(Interpreter* interpreter, Value* class_value, ASTNode* call_node);
void collect_inherited_fields(Interpreter* interpreter, Value* class_value, ASTNode*** all_fields, size_t* field_count, size_t* field_capacity);
Value find_method_in_inheritance_chain(Interpreter* interpreter, Value* class_value, const char* method_name);

// ============================================================================
// MODULE VALUE CREATION FUNCTIONS
// ============================================================================

// Module creation
Value value_create_module(const char* name, void* exports);

// ============================================================================
// ARITHMETIC OPERATIONS
// ============================================================================

// Basic arithmetic
Value value_add(Value* a, Value* b);
Value value_subtract(Value* a, Value* b);
Value value_multiply(Value* a, Value* b);
Value value_divide(Value* a, Value* b);
Value value_modulo(Value* a, Value* b);
Value value_power(Value* a, Value* b);

// ============================================================================
// COMPARISON OPERATIONS
// ============================================================================

// Comparison operations
Value value_equal(Value* a, Value* b);
Value value_not_equal(Value* a, Value* b);
Value value_less_than(Value* a, Value* b);
Value value_greater_than(Value* a, Value* b);
Value value_less_equal(Value* a, Value* b);
Value value_greater_equal(Value* a, Value* b);

// ============================================================================
// LOGICAL OPERATIONS
// ============================================================================

// Logical operations
Value value_logical_and(Value* a, Value* b);
Value value_logical_or(Value* a, Value* b);
Value value_logical_xor(Value* a, Value* b);
Value value_logical_not(Value* a);

// ============================================================================
// BITWISE OPERATIONS
// ============================================================================

// Bitwise operations
Value value_bitwise_and(Value* a, Value* b);
Value value_bitwise_or(Value* a, Value* b);
Value value_bitwise_xor(Value* a, Value* b);
Value value_bitwise_not(Value* a);
Value value_left_shift(Value* a, Value* b);
Value value_right_shift(Value* a, Value* b);

// ============================================================================
// VALUE CONVERSION FUNCTIONS
// ============================================================================

// Type conversions
Value value_to_string(Value* value);
Value value_to_boolean(Value* value);
Value value_to_number(Value* value);

// ============================================================================
// VALUE UTILITY FUNCTIONS
// ============================================================================

// Type checking and utilities
const char* value_type_string(ValueType type);
const char* value_type_to_string(ValueType type);
int value_matches_type(Value* value, const char* type_name, Interpreter* interpreter);
int value_is_truthy(Value* value);
int value_equals(Value* a, Value* b);
Value value_clone(Value* value);
void value_free(Value* value);

#endif // VALUE_OPERATIONS_H
