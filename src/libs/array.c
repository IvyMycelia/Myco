#include <stdlib.h>
#include <string.h>
#include "../../include/core/interpreter.h"
#include "../../include/core/ast.h"
#include "../../include/core/standardized_errors.h"
#include "../../include/utils/shared_utilities.h"

// Array utility functions
Value builtin_array_push(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        std_error_report(ERROR_ARGUMENT_COUNT, "array", "builtin_array_push", 
                        "push() requires exactly 2 arguments", line, column);
        return value_create_null();
    }
    
    Value array_arg = args[0];
    Value element = args[1];
    
    if (array_arg.type != VALUE_ARRAY) {
        std_error_report(ERROR_INVALID_ARGUMENT, "array", "builtin_array_push", 
                        "push() first argument must be an array", line, column);
        return value_create_null();
    }
    
    // Create a new array with all existing elements plus the new element
    Value result = value_create_array(array_arg.data.array_value.count + 1);
    
    // Copy all existing elements
    for (size_t i = 0; i < array_arg.data.array_value.count; i++) {
        Value* existing_element = (Value*)array_arg.data.array_value.elements[i];
        if (existing_element) {
            Value cloned_element = value_clone(existing_element);
            value_array_push(&result, cloned_element);
            // Don't free the cloned element - it's now owned by the array
        }
    }
    
    // Add the new element
    Value cloned_new_element = value_clone(&element);
    value_array_push(&result, cloned_new_element);
    // Don't free the cloned element - it's now owned by the array
    
    return result;
}

Value builtin_array_pop(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_ARGUMENT_COUNT, "array", "builtin_array_pop", 
                        "pop() requires exactly 1 argument", line, column);
        return value_create_null();
    }
    
    Value array_arg = args[0];
    
    if (array_arg.type != VALUE_ARRAY) {
        std_error_report(ERROR_INVALID_ARGUMENT, "array", "unknown_function", "pop() argument must be an array", line, column);
        return value_create_null();
    }
    
    if (array_arg.data.array_value.count == 0) {
        std_error_report(ERROR_INTERNAL_ERROR, "array", "unknown_function", "Cannot pop from empty array", line, column);
        return value_create_null();
    }
    
    return value_array_pop(&array_arg);
}

Value builtin_array_insert(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 3) {
        std_error_report(ERROR_ARGUMENT_COUNT, "array", "unknown_function", "insert() requires exactly 3 arguments", line, column);
        return value_create_null();
    }
    
    Value array_arg = args[0];
    Value index_arg = args[1];
    Value element = args[2];
    
    if (array_arg.type != VALUE_ARRAY) {
        std_error_report(ERROR_INVALID_ARGUMENT, "array", "unknown_function", "insert() first argument must be an array", line, column);
        return value_create_null();
    }
    
    if (index_arg.type != VALUE_NUMBER) {
        std_error_report(ERROR_INVALID_ARGUMENT, "array", "unknown_function", "insert() second argument must be a number", line, column);
        return value_create_null();
    }
    
    int index = (int)index_arg.data.number_value;
    size_t array_len = array_arg.data.array_value.count;
    
    if (index < 0 || index > (int)array_len) {
        std_error_report(ERROR_INTERNAL_ERROR, "array", "unknown_function", "insert() index out of bounds", line, column);
        return value_create_null();
    }
    
    // Clone the element
    Value cloned_element = value_clone(&element);
    
    // Expand array if needed
    if (array_len >= array_arg.data.array_value.capacity) {
        size_t new_capacity = array_arg.data.array_value.capacity == 0 ? 4 : array_arg.data.array_value.capacity * 2;
        void** new_elements = shared_realloc_safe(array_arg.data.array_value.elements, new_capacity * sizeof(void*), "array", "unknown_function", 102);
        if (!new_elements) {
            std_error_report(ERROR_OUT_OF_MEMORY, "array", "unknown_function", "Out of memory in insert()", line, column);
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
    array_arg.data.array_value.elements[index] = shared_malloc_safe(sizeof(Value), "array", "unknown_function", 118);
    if (!array_arg.data.array_value.elements[index]) {
        std_error_report(ERROR_OUT_OF_MEMORY, "array", "unknown_function", "Out of memory in insert()", line, column);
        value_free(&cloned_element);
        return value_create_null();
    }
    
    memcpy(array_arg.data.array_value.elements[index], &cloned_element, sizeof(Value));
    array_arg.data.array_value.count++;
    
    return value_clone(&array_arg);
}

Value builtin_array_remove(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        std_error_report(ERROR_ARGUMENT_COUNT, "array", "unknown_function", "remove() requires exactly 2 arguments", line, column);
        return value_create_null();
    }
    
    Value array_arg = args[0];
    Value index_arg = args[1];
    
    if (array_arg.type != VALUE_ARRAY) {
        std_error_report(ERROR_INVALID_ARGUMENT, "array", "unknown_function", "remove() first argument must be an array", line, column);
        return value_create_null();
    }
    
    if (index_arg.type != VALUE_NUMBER) {
        std_error_report(ERROR_INVALID_ARGUMENT, "array", "unknown_function", "remove() second argument must be a number", line, column);
        return value_create_null();
    }
    
    int index = (int)index_arg.data.number_value;
    size_t array_len = array_arg.data.array_value.count;
    
    if (index < 0 || index >= (int)array_len) {
        std_error_report(ERROR_INTERNAL_ERROR, "array", "unknown_function", "remove() index out of bounds", line, column);
        return value_create_null();
    }
    
    // Free the element at the index
    Value* element = (Value*)array_arg.data.array_value.elements[index];
    if (element) {
        value_free(element);
        shared_free_safe(element, "array", "unknown_function", 162);
    }
    
    // Shift remaining elements
    for (int i = index; i < (int)array_len - 1; i++) {
        array_arg.data.array_value.elements[i] = array_arg.data.array_value.elements[i + 1];
    }
    
    array_arg.data.array_value.count--;
    
    return value_clone(&array_arg);
}

Value builtin_array_reverse(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_ARGUMENT_COUNT, "array", "unknown_function", "reverse() requires exactly 1 argument", line, column);
        return value_create_null();
    }
    
    Value array_arg = args[0];
    
    if (array_arg.type != VALUE_ARRAY) {
        std_error_report(ERROR_INVALID_ARGUMENT, "array", "unknown_function", "reverse() argument must be an array", line, column);
        return value_create_null();
    }
    
    size_t array_len = array_arg.data.array_value.count;
    
    // Reverse the array in place
    for (size_t i = 0; i < array_len / 2; i++) {
        void* temp = array_arg.data.array_value.elements[i];
        array_arg.data.array_value.elements[i] = array_arg.data.array_value.elements[array_len - 1 - i];
        array_arg.data.array_value.elements[array_len - 1 - i] = temp;
    }
    
    return value_clone(&array_arg);
}

Value builtin_array_sort(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_ARGUMENT_COUNT, "array", "unknown_function", "sort() requires exactly 1 argument", line, column);
        return value_create_null();
    }
    
    Value array_arg = args[0];
    
    if (array_arg.type != VALUE_ARRAY) {
        std_error_report(ERROR_INVALID_ARGUMENT, "array", "unknown_function", "sort() argument must be an array", line, column);
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
    
    return value_clone(&array_arg);
}

Value builtin_array_filter(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        std_error_report(ERROR_ARGUMENT_COUNT, "array", "unknown_function", "filter() requires exactly 2 arguments", line, column);
        return value_create_null();
    }
    
    Value array_arg = args[0];
    Value predicate_arg = args[1];
    
    if (array_arg.type != VALUE_ARRAY) {
        std_error_report(ERROR_INVALID_ARGUMENT, "array", "unknown_function", "filter() first argument must be an array", line, column);
        return value_create_null();
    }
    
    if (predicate_arg.type != VALUE_FUNCTION) {
        std_error_report(ERROR_INVALID_ARGUMENT, "array", "unknown_function", "filter() second argument must be a function", line, column);
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
        std_error_report(ERROR_ARGUMENT_COUNT, "array", "unknown_function", "map() requires exactly 2 arguments", line, column);
        return value_create_null();
    }
    
    Value array_arg = args[0];
    Value transform_arg = args[1];
    
    if (array_arg.type != VALUE_ARRAY) {
        std_error_report(ERROR_INVALID_ARGUMENT, "array", "unknown_function", "map() first argument must be an array", line, column);
        return value_create_null();
    }
    
    if (transform_arg.type != VALUE_FUNCTION) {
        std_error_report(ERROR_INVALID_ARGUMENT, "array", "unknown_function", "map() second argument must be a function", line, column);
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
        std_error_report(ERROR_ARGUMENT_COUNT, "array", "unknown_function", "reduce() requires exactly 3 arguments", line, column);
        return value_create_null();
    }
    
    Value array_arg = args[0];
    Value reducer_arg = args[1];
    Value initial_arg = args[2];
    
    if (array_arg.type != VALUE_ARRAY) {
        std_error_report(ERROR_INVALID_ARGUMENT, "array", "unknown_function", "reduce() first argument must be an array", line, column);
        return value_create_null();
    }
    
    if (reducer_arg.type != VALUE_FUNCTION) {
        std_error_report(ERROR_INVALID_ARGUMENT, "array", "unknown_function", "reduce() second argument must be a function", line, column);
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
        std_error_report(ERROR_ARGUMENT_COUNT, "array", "unknown_function", "find() requires exactly 2 arguments", line, column);
        return value_create_null();
    }
    
    Value array_arg = args[0];
    Value predicate_arg = args[1];
    
    if (array_arg.type != VALUE_ARRAY) {
        std_error_report(ERROR_INVALID_ARGUMENT, "array", "unknown_function", "find() first argument must be an array", line, column);
        return value_create_null();
    }
    
    if (predicate_arg.type != VALUE_FUNCTION) {
        std_error_report(ERROR_INVALID_ARGUMENT, "array", "unknown_function", "find() second argument must be a function", line, column);
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
        std_error_report(ERROR_ARGUMENT_COUNT, "array", "unknown_function", "slice() requires exactly 3 arguments", line, column);
        return value_create_null();
    }
    
    Value array_arg = args[0];
    Value start_arg = args[1];
    Value end_arg = args[2];
    
    if (array_arg.type != VALUE_ARRAY) {
        std_error_report(ERROR_INVALID_ARGUMENT, "array", "unknown_function", "slice() first argument must be an array", line, column);
        return value_create_null();
    }
    
    if (start_arg.type != VALUE_NUMBER || end_arg.type != VALUE_NUMBER) {
        std_error_report(ERROR_INTERNAL_ERROR, "array", "unknown_function", "slice() start and end arguments must be numbers", line, column);
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

Value builtin_array_join(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        std_error_report(ERROR_ARGUMENT_COUNT, "array", "builtin_array_join", "join() requires exactly 2 arguments: array and separator", line, column);
        return value_create_null();
    }
    
    Value array_arg = args[0];
    Value separator_arg = args[1];
    
    if (array_arg.type != VALUE_ARRAY) {
        std_error_report(ERROR_INVALID_ARGUMENT, "array", "builtin_array_join", "join() first argument must be an array", line, column);
        return value_create_null();
    }
    
    if (separator_arg.type != VALUE_STRING) {
        std_error_report(ERROR_INVALID_ARGUMENT, "array", "builtin_array_join", "join() second argument must be a string", line, column);
        return value_create_null();
    }
    
    size_t array_len = array_arg.data.array_value.count;
    
    if (array_len == 0) {
        return value_create_string(strdup(""));
    }
    
    // Calculate total length needed
    size_t total_length = 0;
    for (size_t i = 0; i < array_len; i++) {
        Value* element = (Value*)array_arg.data.array_value.elements[i];
        if (element && element->type == VALUE_STRING) {
            total_length += strlen(element->data.string_value);
        } else {
            // Convert non-string elements to string for joining
            total_length += 10; // Estimate for non-string elements
        }
    }
    
    // Add space for separators
    total_length += (array_len - 1) * strlen(separator_arg.data.string_value);
    total_length += 1; // Null terminator
    
    char* result_str = shared_malloc_safe(total_length, "array", "builtin_array_join", 485);
    if (!result_str) {
        std_error_report(ERROR_OUT_OF_MEMORY, "array", "builtin_array_join", "Out of memory in join()", line, column);
        return value_create_null();
    }
    
    result_str[0] = '\0';
    
    // Join elements
    for (size_t i = 0; i < array_len; i++) {
        Value* element = (Value*)array_arg.data.array_value.elements[i];
        if (element) {
            if (element->type == VALUE_STRING && element->data.string_value) {
                strcat(result_str, element->data.string_value);
            } else {
                // Convert to string representation
                Value element_str_value = value_to_string(element);
                if (element_str_value.type == VALUE_STRING && element_str_value.data.string_value) {
                    strcat(result_str, element_str_value.data.string_value);
                }
                value_free(&element_str_value);
            }
        }
        
        // Add separator (except for last element)
        if (i < array_len - 1 && separator_arg.data.string_value) {
            strcat(result_str, separator_arg.data.string_value);
        }
    }
    
    return value_create_string(result_str);
}

Value builtin_array_contains(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        std_error_report(ERROR_ARGUMENT_COUNT, "array", "unknown_function", "contains() requires exactly 2 arguments: array and value", line, column);
        return value_create_null();
    }
    
    Value array_arg = args[0];
    Value search_value = args[1];
    
    if (array_arg.type != VALUE_ARRAY) {
        std_error_report(ERROR_INVALID_ARGUMENT, "array", "unknown_function", "contains() first argument must be an array", line, column);
        return value_create_null();
    }
    
    size_t array_len = array_arg.data.array_value.count;
    
    // Search for the value
    for (size_t i = 0; i < array_len; i++) {
        Value* element = (Value*)array_arg.data.array_value.elements[i];
        if (element && value_equals(element, &search_value)) {
            return value_create_boolean(1);
        }
    }
    
    return value_create_boolean(0);
}

Value builtin_array_index_of(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        std_error_report(ERROR_ARGUMENT_COUNT, "array", "unknown_function", "indexOf() requires exactly 2 arguments: array and value", line, column);
        return value_create_null();
    }
    
    Value array_arg = args[0];
    Value search_value = args[1];
    
    if (array_arg.type != VALUE_ARRAY) {
        std_error_report(ERROR_INVALID_ARGUMENT, "array", "unknown_function", "indexOf() first argument must be an array", line, column);
        return value_create_null();
    }
    
    size_t array_len = array_arg.data.array_value.count;
    
    // Search for the value
    for (size_t i = 0; i < array_len; i++) {
        Value* element = (Value*)array_arg.data.array_value.elements[i];
        if (element && value_equals(element, &search_value)) {
            return value_create_number((double)i);
        }
    }
    
    return value_create_number(-1.0);
}

Value builtin_array_unique(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_ARGUMENT_COUNT, "array", "unknown_function", "unique() requires exactly 1 argument: array", line, column);
        return value_create_null();
    }
    
    Value array_arg = args[0];
    
    if (array_arg.type != VALUE_ARRAY) {
        std_error_report(ERROR_INVALID_ARGUMENT, "array", "unknown_function", "unique() argument must be an array", line, column);
        return value_create_null();
    }
    
    size_t array_len = array_arg.data.array_value.count;
    Value result = value_create_array(0);
    
    // Add unique elements to result
    for (size_t i = 0; i < array_len; i++) {
        Value* element = (Value*)array_arg.data.array_value.elements[i];
        if (element) {
            // Check if element already exists in result
            int is_duplicate = 0;
            for (size_t j = 0; j < result.data.array_value.count; j++) {
                Value* existing = (Value*)result.data.array_value.elements[j];
                if (existing && value_equals(existing, element)) {
                    is_duplicate = 1;
                    break;
                }
            }
            
            if (!is_duplicate) {
                Value cloned_element = value_clone(element);
                value_array_push(&result, cloned_element);
            }
        }
    }
    
    return result;
}

Value builtin_array_concat(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        std_error_report(ERROR_ARGUMENT_COUNT, "array", "unknown_function", "concat() requires exactly 2 arguments: array1 and array2", line, column);
        return value_create_null();
    }
    
    Value array1_arg = args[0];
    Value array2_arg = args[1];
    
    if (array1_arg.type != VALUE_ARRAY || array2_arg.type != VALUE_ARRAY) {
        std_error_report(ERROR_INTERNAL_ERROR, "array", "unknown_function", "concat() both arguments must be arrays", line, column);
        return value_create_null();
    }
    
    size_t len1 = array1_arg.data.array_value.count;
    size_t len2 = array2_arg.data.array_value.count;
    size_t total_len = len1 + len2;
    
    Value result = value_create_array(total_len);
    
    // Copy elements from first array
    for (size_t i = 0; i < len1; i++) {
        Value* element = (Value*)array1_arg.data.array_value.elements[i];
        if (element) {
            Value cloned_element = value_clone(element);
            value_array_push(&result, cloned_element);
        }
    }
    
    // Copy elements from second array
    for (size_t i = 0; i < len2; i++) {
        Value* element = (Value*)array2_arg.data.array_value.elements[i];
        if (element) {
            Value cloned_element = value_clone(element);
            value_array_push(&result, cloned_element);
        }
    }
    
    return result;
}

Value builtin_array_fill(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 3) {
        std_error_report(ERROR_ARGUMENT_COUNT, "array", "unknown_function", "fill() requires exactly 3 arguments: array, value, and count", line, column);
        return value_create_null();
    }
    
    Value array_arg = args[0];
    Value fill_value = args[1];
    Value count_arg = args[2];
    
    if (array_arg.type != VALUE_ARRAY) {
        std_error_report(ERROR_INVALID_ARGUMENT, "array", "unknown_function", "fill() first argument must be an array", line, column);
        return value_create_null();
    }
    
    if (count_arg.type != VALUE_NUMBER) {
        std_error_report(ERROR_INVALID_ARGUMENT, "array", "unknown_function", "fill() third argument must be a number", line, column);
        return value_create_null();
    }
    
    int count = (int)count_arg.data.number_value;
    if (count < 0) {
        std_error_report(ERROR_INTERNAL_ERROR, "array", "unknown_function", "fill() count must be non-negative", line, column);
        return value_create_null();
    }
    
    // Clear existing array and fill with new values
    array_arg.data.array_value.count = 0;
    
    for (int i = 0; i < count; i++) {
        Value cloned_value = value_clone(&fill_value);
        value_array_push(&array_arg, cloned_value);
    }
    
    return value_clone(&array_arg);
}

// Register array library with interpreter
void array_library_register(Interpreter* interpreter) {
    if (!interpreter || !interpreter->global_environment) return;
    
    // Array methods are now called directly on array values, not as global functions
    // No need to register global functions or objects anymore
}
