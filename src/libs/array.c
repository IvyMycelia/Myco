#include <stdlib.h>
#include <string.h>
#include "../../include/core/interpreter.h"
#include "../../include/core/ast.h"

// Array utility functions
Value builtin_array_push(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        interpreter_set_error(interpreter, "push() requires exactly 2 arguments", line, column);
        return value_create_null();
    }
    
    Value array_arg = args[0];
    Value element = args[1];
    
    if (array_arg.type != VALUE_ARRAY) {
        interpreter_set_error(interpreter, "push() first argument must be an array", line, column);
        return value_create_null();
    }
    
    // Clone the element to avoid memory issues
    Value cloned_element = value_clone(&element);
    
    // Add to array
    value_array_push(&array_arg, cloned_element);
    
    // Return the modified array
    return value_clone(&array_arg);
}

Value builtin_array_pop(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "pop() requires exactly 1 argument", line, column);
        return value_create_null();
    }
    
    Value array_arg = args[0];
    
    if (array_arg.type != VALUE_ARRAY) {
        interpreter_set_error(interpreter, "pop() argument must be an array", line, column);
        return value_create_null();
    }
    
    if (array_arg.data.array_value.count == 0) {
        interpreter_set_error(interpreter, "Cannot pop from empty array", line, column);
        return value_create_null();
    }
    
    return value_array_pop(&array_arg);
}

Value builtin_array_insert(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 3) {
        interpreter_set_error(interpreter, "insert() requires exactly 3 arguments", line, column);
        return value_create_null();
    }
    
    Value array_arg = args[0];
    Value index_arg = args[1];
    Value element = args[2];
    
    if (array_arg.type != VALUE_ARRAY) {
        interpreter_set_error(interpreter, "insert() first argument must be an array", line, column);
        return value_create_null();
    }
    
    if (index_arg.type != VALUE_NUMBER) {
        interpreter_set_error(interpreter, "insert() second argument must be a number", line, column);
        return value_create_null();
    }
    
    int index = (int)index_arg.data.number_value;
    size_t array_len = array_arg.data.array_value.count;
    
    if (index < 0 || index > (int)array_len) {
        interpreter_set_error(interpreter, "insert() index out of bounds", line, column);
        return value_create_null();
    }
    
    // Clone the element
    Value cloned_element = value_clone(&element);
    
    // Expand array if needed
    if (array_len >= array_arg.data.array_value.capacity) {
        size_t new_capacity = array_arg.data.array_value.capacity == 0 ? 4 : array_arg.data.array_value.capacity * 2;
        void** new_elements = realloc(array_arg.data.array_value.elements, new_capacity * sizeof(void*));
        if (!new_elements) {
            interpreter_set_error(interpreter, "Out of memory in insert()", line, column);
            value_free(&cloned_element);
            return value_create_null();
        }
        array_arg.data.array_value.elements = new_elements;
        array_arg.data.array_value.capacity = new_capacity;
    }
    
    // Shift elements to make room
    for (int i = (int)array_len; i > index; i--) {
        array_arg.data.array_value.elements[i] = array_arg.data.array_value.elements[i - 1];
    }
    
    // Insert element
    array_arg.data.array_value.elements[index] = malloc(sizeof(Value));
    if (!array_arg.data.array_value.elements[index]) {
        interpreter_set_error(interpreter, "Out of memory in insert()", line, column);
        value_free(&cloned_element);
        return value_create_null();
    }
    
    memcpy(array_arg.data.array_value.elements[index], &cloned_element, sizeof(Value));
    array_arg.data.array_value.count++;
    
    return value_create_null();
}

Value builtin_array_remove(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        interpreter_set_error(interpreter, "remove() requires exactly 2 arguments", line, column);
        return value_create_null();
    }
    
    Value array_arg = args[0];
    Value index_arg = args[1];
    
    if (array_arg.type != VALUE_ARRAY) {
        interpreter_set_error(interpreter, "remove() first argument must be an array", line, column);
        return value_create_null();
    }
    
    if (index_arg.type != VALUE_NUMBER) {
        interpreter_set_error(interpreter, "remove() second argument must be a number", line, column);
        return value_create_null();
    }
    
    int index = (int)index_arg.data.number_value;
    size_t array_len = array_arg.data.array_value.count;
    
    if (index < 0 || index >= (int)array_len) {
        interpreter_set_error(interpreter, "remove() index out of bounds", line, column);
        return value_create_null();
    }
    
    // Free the element at the index
    Value* element = (Value*)array_arg.data.array_value.elements[index];
    if (element) {
        value_free(element);
        free(element);
    }
    
    // Shift remaining elements
    for (int i = index; i < (int)array_len - 1; i++) {
        array_arg.data.array_value.elements[i] = array_arg.data.array_value.elements[i + 1];
    }
    
    array_arg.data.array_value.count--;
    
    return value_create_null();
}

Value builtin_array_reverse(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "reverse() requires exactly 1 argument", line, column);
        return value_create_null();
    }
    
    Value array_arg = args[0];
    
    if (array_arg.type != VALUE_ARRAY) {
        interpreter_set_error(interpreter, "reverse() argument must be an array", line, column);
        return value_create_null();
    }
    
    size_t array_len = array_arg.data.array_value.count;
    
    // Reverse the array in place
    for (size_t i = 0; i < array_len / 2; i++) {
        void* temp = array_arg.data.array_value.elements[i];
        array_arg.data.array_value.elements[i] = array_arg.data.array_value.elements[array_len - 1 - i];
        array_arg.data.array_value.elements[array_len - 1 - i] = temp;
    }
    
    return value_create_null();
}

Value builtin_array_sort(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "sort() requires exactly 1 argument", line, column);
        return value_create_null();
    }
    
    Value array_arg = args[0];
    
    if (array_arg.type != VALUE_ARRAY) {
        interpreter_set_error(interpreter, "sort() argument must be an array", line, column);
        return value_create_null();
    }
    
    size_t array_len = array_arg.data.array_value.count;
    
    // Simple bubble sort for now
    for (size_t i = 0; i < array_len - 1; i++) {
        for (size_t j = 0; j < array_len - i - 1; j++) {
            Value* val1 = (Value*)array_arg.data.array_value.elements[j];
            Value* val2 = (Value*)array_arg.data.array_value.elements[j + 1];
            
            if (val1 && val2 && val1->type == VALUE_NUMBER && val2->type == VALUE_NUMBER) {
                if (val1->data.number_value > val2->data.number_value) {
                    // Swap elements
                    void* temp = array_arg.data.array_value.elements[j];
                    array_arg.data.array_value.elements[j] = array_arg.data.array_value.elements[j + 1];
                    array_arg.data.array_value.elements[j + 1] = temp;
                }
            }
        }
    }
    
    return value_create_null();
}

Value builtin_array_filter(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        interpreter_set_error(interpreter, "filter() requires exactly 2 arguments", line, column);
        return value_create_null();
    }
    
    Value array_arg = args[0];
    Value predicate_arg = args[1];
    
    if (array_arg.type != VALUE_ARRAY) {
        interpreter_set_error(interpreter, "filter() first argument must be an array", line, column);
        return value_create_null();
    }
    
    if (predicate_arg.type != VALUE_FUNCTION) {
        interpreter_set_error(interpreter, "filter() second argument must be a function", line, column);
        return value_create_null();
    }
    
    size_t array_len = array_arg.data.array_value.count;
    Value result = value_create_array(0);
    
    // Filter elements based on predicate function
    for (size_t i = 0; i < array_len; i++) {
        Value* element = (Value*)array_arg.data.array_value.elements[i];
        if (element) {
            // Call predicate function with element
            Value predicate_args[1] = {*element};
            Value predicate_result = value_function_call(&predicate_arg, predicate_args, 1, interpreter, line, column);
            
            if (predicate_result.type == VALUE_BOOLEAN && predicate_result.data.boolean_value) {
                Value cloned_element = value_clone(element);
                value_array_push(&result, cloned_element);
            }
            
            value_free(&predicate_result);
        }
    }
    
    return result;
}

Value builtin_array_map(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        interpreter_set_error(interpreter, "map() requires exactly 2 arguments", line, column);
        return value_create_null();
    }
    
    Value array_arg = args[0];
    Value transform_arg = args[1];
    
    if (array_arg.type != VALUE_ARRAY) {
        interpreter_set_error(interpreter, "map() first argument must be an array", line, column);
        return value_create_null();
    }
    
    if (transform_arg.type != VALUE_FUNCTION) {
        interpreter_set_error(interpreter, "map() second argument must be a function", line, column);
        return value_create_null();
    }
    
    size_t array_len = array_arg.data.array_value.count;
    Value result = value_create_array(array_len);
    
    // Transform each element using the function
    for (size_t i = 0; i < array_len; i++) {
        Value* element = (Value*)array_arg.data.array_value.elements[i];
        if (element) {
            // Call transform function with element
            Value transform_args[1] = {*element};
            Value transform_result = value_function_call(&transform_arg, transform_args, 1, interpreter, line, column);
            
            value_array_push(&result, transform_result);
        }
    }
    
    return result;
}

Value builtin_array_reduce(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 3) {
        interpreter_set_error(interpreter, "reduce() requires exactly 3 arguments", line, column);
        return value_create_null();
    }
    
    Value array_arg = args[0];
    Value reducer_arg = args[1];
    Value initial_arg = args[2];
    
    if (array_arg.type != VALUE_ARRAY) {
        interpreter_set_error(interpreter, "reduce() first argument must be an array", line, column);
        return value_create_null();
    }
    
    if (reducer_arg.type != VALUE_FUNCTION) {
        interpreter_set_error(interpreter, "reduce() second argument must be a function", line, column);
        return value_create_null();
    }
    
    size_t array_len = array_arg.data.array_value.count;
    Value accumulator = value_clone(&initial_arg);
    
    // Reduce array using the reducer function
    for (size_t i = 0; i < array_len; i++) {
        Value* element = (Value*)array_arg.data.array_value.elements[i];
        if (element) {
            // Call reducer function with accumulator and element
            Value reducer_args[2] = {accumulator, *element};
            Value reducer_result = value_function_call(&reducer_arg, reducer_args, 2, interpreter, line, column);
            
            value_free(&accumulator);
            accumulator = reducer_result;
        }
    }
    
    return accumulator;
}

Value builtin_array_find(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        interpreter_set_error(interpreter, "find() requires exactly 2 arguments", line, column);
        return value_create_null();
    }
    
    Value array_arg = args[0];
    Value predicate_arg = args[1];
    
    if (array_arg.type != VALUE_ARRAY) {
        interpreter_set_error(interpreter, "find() first argument must be an array", line, column);
        return value_create_null();
    }
    
    if (predicate_arg.type != VALUE_FUNCTION) {
        interpreter_set_error(interpreter, "find() second argument must be a function", line, column);
        return value_create_null();
    }
    
    size_t array_len = array_arg.data.array_value.count;
    
    // Find first element that satisfies predicate
    for (size_t i = 0; i < array_len; i++) {
        Value* element = (Value*)array_arg.data.array_value.elements[i];
        if (element) {
            // Call predicate function with element
            Value predicate_args[1] = {*element};
            Value predicate_result = value_function_call(&predicate_arg, predicate_args, 1, interpreter, line, column);
            
            if (predicate_result.type == VALUE_BOOLEAN && predicate_result.data.boolean_value) {
                value_free(&predicate_result);
                return value_clone(element);
            }
            
            value_free(&predicate_result);
        }
    }
    
    return value_create_null();
}

Value builtin_array_slice(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 3) {
        interpreter_set_error(interpreter, "slice() requires exactly 3 arguments", line, column);
        return value_create_null();
    }
    
    Value array_arg = args[0];
    Value start_arg = args[1];
    Value end_arg = args[2];
    
    if (array_arg.type != VALUE_ARRAY) {
        interpreter_set_error(interpreter, "slice() first argument must be an array", line, column);
        return value_create_null();
    }
    
    if (start_arg.type != VALUE_NUMBER || end_arg.type != VALUE_NUMBER) {
        interpreter_set_error(interpreter, "slice() start and end arguments must be numbers", line, column);
        return value_create_null();
    }
    
    int start = (int)start_arg.data.number_value;
    int end = (int)end_arg.data.number_value;
    size_t array_len = array_arg.data.array_value.count;
    
    // Handle negative indices
    if (start < 0) start = (int)array_len + start;
    if (end < 0) end = (int)array_len + end;
    
    // Clamp indices
    if (start < 0) start = 0;
    if (end > (int)array_len) end = (int)array_len;
    if (start >= end) {
        return value_create_array(0);
    }
    
    size_t slice_len = end - start;
    Value result = value_create_array(slice_len);
    
    // Copy elements from start to end
    for (int i = start; i < end; i++) {
        Value* element = (Value*)array_arg.data.array_value.elements[i];
        if (element) {
            Value cloned_element = value_clone(element);
            value_array_push(&result, cloned_element);
        }
    }
    
    return result;
}

// Register array library with interpreter
void array_library_register(Interpreter* interpreter) {
    if (!interpreter || !interpreter->global_environment) return;
    
    // Create array object with methods
    Value array_obj = value_create_object(16);
    
    // Add methods to array object
    value_object_set_member(&array_obj, "push", value_create_builtin_function(builtin_array_push));
    value_object_set_member(&array_obj, "pop", value_create_builtin_function(builtin_array_pop));
    value_object_set_member(&array_obj, "insert", value_create_builtin_function(builtin_array_insert));
    value_object_set_member(&array_obj, "remove", value_create_builtin_function(builtin_array_remove));
    value_object_set_member(&array_obj, "reverse", value_create_builtin_function(builtin_array_reverse));
    value_object_set_member(&array_obj, "sort", value_create_builtin_function(builtin_array_sort));
    value_object_set_member(&array_obj, "filter", value_create_builtin_function(builtin_array_filter));
    value_object_set_member(&array_obj, "map", value_create_builtin_function(builtin_array_map));
    value_object_set_member(&array_obj, "reduce", value_create_builtin_function(builtin_array_reduce));
    value_object_set_member(&array_obj, "find", value_create_builtin_function(builtin_array_find));
    value_object_set_member(&array_obj, "slice", value_create_builtin_function(builtin_array_slice));
    
    // Register the array object
    environment_define(interpreter->global_environment, "array", array_obj);
}
