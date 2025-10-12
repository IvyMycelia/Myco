#include "interpreter/value_operations.h"
#include "../../include/core/interpreter.h"
#include "../../include/utils/shared_utilities.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// ============================================================================
// VALUE CONVERSION FUNCTIONS
// ============================================================================

Value value_to_string(Value* value) { 
    if (!value) return value_create_string(""); 
    char buf[64]; 
    switch (value->type) { 
        case VALUE_STRING: return value_clone(value); 
        case VALUE_NUMBER: {
            // Check if it's an integer or float
            if (value->data.number_value == (long long)value->data.number_value) {
                // It's an integer, use %lld to avoid scientific notation
                snprintf(buf, sizeof(buf), "%lld", (long long)value->data.number_value);
            } else {
                // It's a float, use %.6f to avoid scientific notation
                snprintf(buf, sizeof(buf), "%.6f", value->data.number_value);
            }
            return value_create_string(buf);
        }
        case VALUE_BOOLEAN: return value_create_string(value->data.boolean_value ? "True" : "False"); 
        case VALUE_RANGE: {
            // Format range values without scientific notation
            char start_buf[32], end_buf[32];
            if (value->data.range_value.start == (long long)value->data.range_value.start) {
                snprintf(start_buf, sizeof(start_buf), "%lld", (long long)value->data.range_value.start);
            } else {
                snprintf(start_buf, sizeof(start_buf), "%.6f", value->data.range_value.start);
            }
            if (value->data.range_value.end == (long long)value->data.range_value.end) {
                snprintf(end_buf, sizeof(end_buf), "%lld", (long long)value->data.range_value.end);
            } else {
                snprintf(end_buf, sizeof(end_buf), "%.6f", value->data.range_value.end);
            }
            snprintf(buf, sizeof(buf), "%s..%s", start_buf, end_buf);
            return value_create_string(buf);
        }
        case VALUE_NULL: return value_create_string("Null"); 
        case VALUE_ARRAY: {
            // Format array as [item1, item2, item3, ...]
            char* result = shared_malloc_safe(2, "interpreter", "value_to_string", 0); // Start with just "["
            if (!result) return value_create_string("[]");
            result[0] = '[';
            result[1] = '\0';
            size_t result_len = 1;
            
            for (size_t i = 0; i < value->data.array_value.count; i++) {
                Value* element = (Value*)value->data.array_value.elements[i];
                if (!element) {
                    // Handle NULL element
                    result = shared_realloc_safe(result, result_len + 5, "interpreter", "value_to_string", 0); // +5 for "null, "
                    if (!result) {
                        return value_create_string("[]");
                    }
                    if (i > 0) {
                        strcat(result, ", ");
                        result_len += 2;
                    }
                    strcat(result, "null");
                    result_len += 4;
                    continue;
                }
                
                Value element_str = value_to_string(element);
                
                if (element_str.type == VALUE_STRING && element_str.data.string_value) {
                    size_t element_len = strlen(element_str.data.string_value);
                    result = shared_realloc_safe(result, result_len + element_len + 3, "interpreter", "value_to_string", 0); // +3 for ", " or "]"
                    if (!result) {
                        value_free(&element_str);
                        return value_create_string("[]");
                    }
                    
                    if (i > 0) {
                        strcat(result, ", ");
                        result_len += 2;
                    }
                    strcat(result, element_str.data.string_value);
                    result_len += element_len;
                } else {
                    // Handle case where element_str conversion failed
                    result = shared_realloc_safe(result, result_len + 5, "interpreter", "value_to_string", 0); // +5 for "null, "
                    if (!result) {
                        value_free(&element_str);
                        return value_create_string("[]");
                    }
                    if (i > 0) {
                        strcat(result, ", ");
                        result_len += 2;
                    }
                    strcat(result, "null");
                    result_len += 4;
                }
                value_free(&element_str);
            }
            
            // Add closing bracket
            result = shared_realloc_safe(result, result_len + 2, "interpreter", "value_to_string", 0);
            if (!result) return value_create_string("[]");
            strcat(result, "]");
            
            Value result_value = value_create_string(result);
            shared_free_safe(result, "interpreter", "value_to_string", 0);
            return result_value;
        }
        case VALUE_HASH_MAP: {
            // Format hash map as {key1: value1, key2: value2, ...}
            char* result = shared_malloc_safe(2, "interpreter", "value_to_string", 0); // Start with just "{"
            if (!result) return value_create_string("{}");
            result[0] = '{';
            result[1] = '\0';
            size_t result_len = 1;
            
            for (size_t i = 0; i < value->data.hash_map_value.count; i++) {
                Value* key = (Value*)value->data.hash_map_value.keys[i];
                Value* map_value = (Value*)value->data.hash_map_value.values[i];
                
                if (key && map_value) {
                    // Add comma separator
                    if (i > 0) {
                        result = shared_realloc_safe(result, result_len + 2, "interpreter", "value_to_string", 0);
                        if (!result) return value_create_string("{}");
                        strcat(result, ", ");
                        result_len += 2;
                    }
                    
                    // Add key
                    Value key_str = value_to_string(key);
                    if (key_str.type == VALUE_STRING && key_str.data.string_value) {
                        size_t key_len = strlen(key_str.data.string_value);
                        result = shared_realloc_safe(result, result_len + key_len + 1, "interpreter", "value_to_string", 0);
                        if (!result) {
                            value_free(&key_str);
                            return value_create_string("{}");
                        }
                        strcat(result, key_str.data.string_value);
                        result_len += key_len;
                        value_free(&key_str);
                    } else {
                        value_free(&key_str);
                        return value_create_string("{}");
                    }
                    
                    // Add colon
                    result = shared_realloc_safe(result, result_len + 2, "interpreter", "value_to_string", 0);
                    if (!result) return value_create_string("{}");
                    strcat(result, ": ");
                    result_len += 2;
                    
                    // Add value
                    Value value_str = value_to_string(map_value);
                    if (value_str.type == VALUE_STRING && value_str.data.string_value) {
                        size_t val_len = strlen(value_str.data.string_value);
                        result = shared_realloc_safe(result, result_len + val_len + 1, "interpreter", "value_to_string", 0);
                        if (!result) {
                            value_free(&value_str);
                            return value_create_string("{}");
                        }
                        strcat(result, value_str.data.string_value);
                        result_len += val_len;
                    } else {
                        result = shared_realloc_safe(result, result_len + 5, "interpreter", "value_to_string", 0);
                        if (!result) {
                            value_free(&value_str);
                            return value_create_string("{}");
                        }
                        strcat(result, "null");
                        result_len += 4;
                    }
                    value_free(&value_str);
                }
            }
            
            // Add closing brace
            result = shared_realloc_safe(result, result_len + 2, "interpreter", "value_to_string", 0);
            if (!result) return value_create_string("{}");
            strcat(result, "}");
            
            Value result_value = value_create_string(result);
            shared_free_safe(result, "interpreter", "value_to_string", 0);
            return result_value;
        }
        case VALUE_SET: {
            // Format set as {item1, item2, item3, ...}
            char* result = shared_malloc_safe(2, "interpreter", "value_to_string", 0); // Start with just "{"
            if (!result) return value_create_string("{}");
            result[0] = '{';
            result[1] = '\0';
            size_t result_len = 1;
            
            for (size_t i = 0; i < value->data.set_value.count; i++) {
                Value* element = (Value*)value->data.set_value.elements[i];
                
                if (element) {
                    // Add comma separator
                    if (i > 0) {
                        result = shared_realloc_safe(result, result_len + 2, "interpreter", "value_to_string", 0);
                        if (!result) return value_create_string("{}");
                        strcat(result, ", ");
                        result_len += 2;
                    }
                    
                    // Add element
                    Value element_str = value_to_string(element);
                    if (element_str.type == VALUE_STRING && element_str.data.string_value) {
                        size_t elem_len = strlen(element_str.data.string_value);
                        result = shared_realloc_safe(result, result_len + elem_len + 1, "interpreter", "value_to_string", 0);
                        if (!result) {
                            value_free(&element_str);
                            return value_create_string("{}");
                        }
                        strcat(result, element_str.data.string_value);
                        result_len += elem_len;
                    } else {
                        result = shared_realloc_safe(result, result_len + 5, "interpreter", "value_to_string", 0);
                        if (!result) {
                            value_free(&element_str);
                            return value_create_string("{}");
                        }
                        strcat(result, "null");
                        result_len += 4;
                    }
                    value_free(&element_str);
                }
            }
            
            // Add closing brace
            result = shared_realloc_safe(result, result_len + 2, "interpreter", "value_to_string", 0);
            if (!result) return value_create_string("{}");
            strcat(result, "}");
            
            Value result_value = value_create_string(result);
            shared_free_safe(result, "interpreter", "value_to_string", 0);
            return result_value;
        }
        default: return value_create_string("<Value>"); 
    } 
}

Value value_to_boolean(Value* value) { 
    if (!value) return value_create_boolean(0);
    
    switch (value->type) {
        case VALUE_BOOLEAN:
            return value_create_boolean(value->data.boolean_value);
        case VALUE_NUMBER:
            // 0 is false, everything else is true
            return value_create_boolean(value->data.number_value != 0.0);
        case VALUE_STRING:
            // Empty string is false, everything else is true
            return value_create_boolean(value->data.string_value && strlen(value->data.string_value) > 0);
        case VALUE_NULL:
            return value_create_boolean(0);
        default:
            // For other types, consider them true
            return value_create_boolean(1);
    }
}

Value value_to_number(Value* value) { 
    Value v = {0}; 
    return v; 
}

// ============================================================================
// VALUE UTILITY FUNCTIONS
// ============================================================================

const char* value_type_string(ValueType type) {
    switch (type) {
        case VALUE_NULL: return "Null";
        case VALUE_NUMBER: return "Number";
        case VALUE_STRING: return "String";
        case VALUE_BOOLEAN: return "Boolean";
        case VALUE_ARRAY: return "Array";
        case VALUE_OBJECT: return "Object";
        case VALUE_HASH_MAP: return "Map";
        case VALUE_SET: return "Set";
        case VALUE_FUNCTION: return "Function";
        case VALUE_ASYNC_FUNCTION: return "AsyncFunction";
        case VALUE_PROMISE: return "Promise";
        case VALUE_RANGE: return "Range";
        case VALUE_CLASS: return "Class";
        case VALUE_MODULE: return "Object";
        case VALUE_ERROR: return "Error";
        default: return "Unknown";
    }
}

int value_matches_type(Value* value, const char* type_name, Interpreter* interpreter) {
    if (!value || !type_name) return 0;
    
    // Handle common type aliases
    if (strcmp(type_name, "Int") == 0 || strcmp(type_name, "Integer") == 0 || strcmp(type_name, "Number") == 0) {
        return value->type == VALUE_NUMBER;
    }
    if (strcmp(type_name, "Float") == 0 || strcmp(type_name, "Double") == 0) {
        return value->type == VALUE_NUMBER;
    }
    if (strcmp(type_name, "Bool") == 0) {
        return value->type == VALUE_BOOLEAN;
    }
    if (strcmp(type_name, "Str") == 0 || strcmp(type_name, "String") == 0) {
        return value->type == VALUE_STRING;
    }
    if (strcmp(type_name, "Array") == 0) {
        return value->type == VALUE_ARRAY;
    }
    if (strcmp(type_name, "Object") == 0) {
        return value->type == VALUE_OBJECT;
    }
    if (strcmp(type_name, "Function") == 0) {
        return value->type == VALUE_FUNCTION;
    }
    if (strcmp(type_name, "Null") == 0) {
        return value->type == VALUE_NULL;
    }
    
    // Check for union types like "String | Int"
    char* type_copy = strdup(type_name);
    if (!type_copy) return 0;
    
    char* token = strtok(type_copy, "|");
    while (token) {
        // Trim whitespace
        while (*token == ' ' || *token == '\t') token++;
        char* end = token + strlen(token) - 1;
        while (end > token && (*end == ' ' || *end == '\t')) {
            *end = '\0';
            end--;
        }
        
        if (strcmp(token, "Int") == 0 || strcmp(token, "Integer") == 0 || strcmp(token, "Number") == 0) {
            if (value->type == VALUE_NUMBER) {
                shared_free_safe(type_copy, "interpreter", "value_matches_type", 0);
                return 1;
            }
        } else if (strcmp(token, "Float") == 0 || strcmp(token, "Double") == 0) {
            if (value->type == VALUE_NUMBER) {
                shared_free_safe(type_copy, "interpreter", "value_matches_type", 0);
                return 1;
            }
        } else if (strcmp(token, "Bool") == 0) {
            if (value->type == VALUE_BOOLEAN) {
                shared_free_safe(type_copy, "interpreter", "value_matches_type", 0);
                return 1;
            }
        } else if (strcmp(token, "Str") == 0 || strcmp(token, "String") == 0) {
            if (value->type == VALUE_STRING) {
                shared_free_safe(type_copy, "interpreter", "value_matches_type", 0);
                return 1;
            }
        } else if (strcmp(token, "Array") == 0) {
            if (value->type == VALUE_ARRAY) {
                shared_free_safe(type_copy, "interpreter", "value_matches_type", 0);
                return 1;
            }
        } else if (strcmp(token, "Object") == 0) {
            if (value->type == VALUE_OBJECT) {
                shared_free_safe(type_copy, "interpreter", "value_matches_type", 0);
                return 1;
            }
        } else if (strcmp(token, "Function") == 0) {
            if (value->type == VALUE_FUNCTION) {
                shared_free_safe(type_copy, "interpreter", "value_matches_type", 0);
                return 1;
            }
        } else if (strcmp(token, "Null") == 0) {
            if (value->type == VALUE_NULL) {
                shared_free_safe(type_copy, "interpreter", "value_matches_type", 0);
                return 1;
            }
        }
        
        token = strtok(NULL, "|");
    }
    
    shared_free_safe(type_copy, "interpreter", "value_matches_type", 0);
    return 0;
}

int value_is_truthy(Value* value) { 
    if (!value) return 0; 
    switch (value->type) { 
        case VALUE_BOOLEAN: return value->data.boolean_value != 0; 
        case VALUE_NUMBER: return value->data.number_value != 0.0; 
        case VALUE_STRING: return value->data.string_value && value->data.string_value[0] != '\0'; 
        default: return 0; 
    } 
}

int value_equals(Value* a, Value* b) { 
    if (!a || !b) return 0;
    if (a->type != b->type) return 0;
    
    switch (a->type) {
        case VALUE_NULL: return 1;
        case VALUE_BOOLEAN: return a->data.boolean_value == b->data.boolean_value;
        case VALUE_NUMBER: return a->data.number_value == b->data.number_value;
        case VALUE_STRING: 
            if (!a->data.string_value || !b->data.string_value) return 0;
            return strcmp(a->data.string_value, b->data.string_value) == 0;
        case VALUE_RANGE:
            return a->data.range_value.start == b->data.range_value.start && 
                   a->data.range_value.end == b->data.range_value.end;
        default: return 0;
    }
}

Value value_clone(Value* value) { 
    if (!value) { Value v = {0}; return v; } 
    switch (value->type) { 
        case VALUE_NULL: return value_create_null();
        case VALUE_NUMBER: return value_create_number(value->data.number_value); 
        case VALUE_BOOLEAN: return value_create_boolean(value->data.boolean_value); 
        case VALUE_STRING: {
            if (value->data.string_value) {
                return value_create_string(value->data.string_value);
            } else {
                return value_create_string("");
            }
        } 
        case VALUE_RANGE: return value_create_range(value->data.range_value.start, value->data.range_value.end, value->data.range_value.step, value->data.range_value.inclusive); 
        case VALUE_ARRAY: {
            // Deep copy array with all elements
            Value v = value_create_array(value->data.array_value.count);
            for (size_t i = 0; i < value->data.array_value.count; i++) {
                Value* element = (Value*)value->data.array_value.elements[i];
                if (element) {
                    Value cloned_element = value_clone(element);
                    value_array_push(&v, cloned_element);
                    // Don't free the cloned element - it's now owned by the array
                }
            }
            return v;
        }
        case VALUE_OBJECT: {
            // Deep copy object with all members
            Value v = value_create_object(value->data.object_value.count);
            for (size_t i = 0; i < value->data.object_value.count; i++) {
                const char* key = value->data.object_value.keys[i];
                Value* member_value = (Value*)value->data.object_value.values[i];
                if (key && member_value) {
                    Value cloned_member = value_clone(member_value);
                    value_object_set_member(&v, key, cloned_member);
                }
            }
            return v;
        }
        case VALUE_FUNCTION: {
            // Clone function with all its data
            Value v = value_create_function(
                value->data.function_value.body,
                value->data.function_value.parameters,
                value->data.function_value.parameter_count,
                value->data.function_value.return_type,
                value->data.function_value.captured_environment
            );
            return v;
        }
        case VALUE_ASYNC_FUNCTION: {
            // Clone async function with all its data
            Value v = value_create_async_function(
                NULL, // name not stored in value
                value->data.async_function_value.parameters,
                value->data.async_function_value.parameter_count,
                value->data.async_function_value.return_type,
                value->data.async_function_value.body,
                value->data.async_function_value.captured_environment
            );
            return v;
        }
        case VALUE_PROMISE: {
            // Clone promise
            Value resolved_value = value_create_string(value->data.promise_value.resolved_data ? value->data.promise_value.resolved_data : "");
            Value error_value = value_create_string(value->data.promise_value.error_message ? value->data.promise_value.error_message : "");
            Value v = value_create_promise(resolved_value, value->data.promise_value.is_resolved, error_value);
            value_free(&resolved_value);
            value_free(&error_value);
            return v;
        }
        case VALUE_CLASS: {
            // Clone class
            Value v = value_create_class(
                value->data.class_value.class_name,
                value->data.class_value.parent_class_name,
                value->data.class_value.class_body,
                value->data.class_value.class_environment
            );
            return v;
        }
        case VALUE_MODULE: {
            // Clone module
            Value v = value_create_module(
                value->data.module_value.module_name,
                value->data.module_value.exports
            );
            return v;
        }
        case VALUE_HASH_MAP: {
            // Deep copy hash map with all key-value pairs
            Value v = value_create_hash_map(value->data.hash_map_value.count);
            for (size_t i = 0; i < value->data.hash_map_value.count; i++) {
                Value* key = (Value*)value->data.hash_map_value.keys[i];
                Value* map_value = (Value*)value->data.hash_map_value.values[i];
                if (key && map_value) {
                    Value cloned_key = value_clone(key);
                    Value cloned_value = value_clone(map_value);
                    value_hash_map_set(&v, cloned_key, cloned_value);
                }
            }
            return v;
        }
        case VALUE_SET: {
            // Deep copy set with all elements
            Value v = value_create_set(value->data.set_value.count);
            for (size_t i = 0; i < value->data.set_value.count; i++) {
                Value* element = (Value*)value->data.set_value.elements[i];
                if (element) {
                    Value cloned_element = value_clone(element);
                    value_set_add(&v, cloned_element);
                }
            }
            return v;
        }
        default: return value_create_null(); 
    } 
}

void value_free(Value* value) { 
    if (!value) return; 
    
    switch (value->type) {
        case VALUE_STRING:
            if (value->data.string_value) {
                shared_free_safe(value->data.string_value, "interpreter", "value_free", 0);
                value->data.string_value = NULL;
            }
            break;
        case VALUE_ARRAY:
            if (value->data.array_value.elements) {
                for (size_t i = 0; i < value->data.array_value.count; i++) {
                    Value* element = (Value*)value->data.array_value.elements[i];
                    if (element) {
                        value_free(element);
                        shared_free_safe(element, "interpreter", "value_free", 0);
                    }
                }
                shared_free_safe(value->data.array_value.elements, "interpreter", "value_free", 0);
                value->data.array_value.elements = NULL;
            }
            break;
        case VALUE_OBJECT:
            if (value->data.object_value.keys) {
                for (size_t i = 0; i < value->data.object_value.count; i++) {
                    if (value->data.object_value.keys[i]) {
                        shared_free_safe(value->data.object_value.keys[i], "interpreter", "value_free", 0);
                    }
                    Value* member_value = (Value*)value->data.object_value.values[i];
                    if (member_value) {
                        value_free(member_value);
                        shared_free_safe(member_value, "interpreter", "value_free", 0);
                    }
                }
                shared_free_safe(value->data.object_value.keys, "interpreter", "value_free", 0);
                shared_free_safe(value->data.object_value.values, "interpreter", "value_free", 0);
                value->data.object_value.keys = NULL;
                value->data.object_value.values = NULL;
            }
            break;
        case VALUE_FUNCTION:
            if (value->data.function_value.return_type) {
                shared_free_safe(value->data.function_value.return_type, "interpreter", "value_free", 0);
                value->data.function_value.return_type = NULL;
            }
            if (value->data.function_value.parameters) {
                for (size_t i = 0; i < value->data.function_value.parameter_count; i++) {
                    if (value->data.function_value.parameters[i]) {
                        // Don't free the AST nodes - they're managed elsewhere
                    }
                }
                shared_free_safe(value->data.function_value.parameters, "interpreter", "value_free", 0);
                value->data.function_value.parameters = NULL;
            }
            break;
        case VALUE_ASYNC_FUNCTION:
            if (value->data.async_function_value.return_type) {
                shared_free_safe(value->data.async_function_value.return_type, "interpreter", "value_free", 0);
                value->data.async_function_value.return_type = NULL;
            }
            if (value->data.async_function_value.parameters) {
                for (size_t i = 0; i < value->data.async_function_value.parameter_count; i++) {
                    if (value->data.async_function_value.parameters[i]) {
                        // Don't free the AST nodes - they're managed elsewhere
                    }
                }
                shared_free_safe(value->data.async_function_value.parameters, "interpreter", "value_free", 0);
                value->data.async_function_value.parameters = NULL;
            }
            break;
        case VALUE_PROMISE:
            if (value->data.promise_value.resolved_data) {
                shared_free_safe(value->data.promise_value.resolved_data, "interpreter", "value_free", 0);
                value->data.promise_value.resolved_data = NULL;
            }
            if (value->data.promise_value.error_message) {
                shared_free_safe(value->data.promise_value.error_message, "interpreter", "value_free", 0);
                value->data.promise_value.error_message = NULL;
            }
            break;
        case VALUE_CLASS:
            if (value->data.class_value.class_name) {
                shared_free_safe(value->data.class_value.class_name, "interpreter", "value_free", 0);
                value->data.class_value.class_name = NULL;
            }
            if (value->data.class_value.parent_class_name) {
                shared_free_safe(value->data.class_value.parent_class_name, "interpreter", "value_free", 0);
                value->data.class_value.parent_class_name = NULL;
            }
            break;
        case VALUE_MODULE:
            if (value->data.module_value.module_name) {
                shared_free_safe(value->data.module_value.module_name, "interpreter", "value_free", 0);
                value->data.module_value.module_name = NULL;
            }
            break;
        case VALUE_HASH_MAP:
            if (value->data.hash_map_value.keys) {
                for (size_t i = 0; i < value->data.hash_map_value.count; i++) {
                    Value* key = (Value*)value->data.hash_map_value.keys[i];
                    if (key) {
                        value_free(key);
                        shared_free_safe(key, "interpreter", "value_free", 0);
                    }
                    Value* map_value = (Value*)value->data.hash_map_value.values[i];
                    if (map_value) {
                        value_free(map_value);
                        shared_free_safe(map_value, "interpreter", "value_free", 0);
                    }
                }
                shared_free_safe(value->data.hash_map_value.keys, "interpreter", "value_free", 0);
                shared_free_safe(value->data.hash_map_value.values, "interpreter", "value_free", 0);
                value->data.hash_map_value.keys = NULL;
                value->data.hash_map_value.values = NULL;
            }
            break;
        case VALUE_SET:
            if (value->data.set_value.elements) {
                for (size_t i = 0; i < value->data.set_value.count; i++) {
                    Value* element = (Value*)value->data.set_value.elements[i];
                    if (element) {
                        value_free(element);
                        shared_free_safe(element, "interpreter", "value_free", 0);
                    }
                }
                shared_free_safe(value->data.set_value.elements, "interpreter", "value_free", 0);
                value->data.set_value.elements = NULL;
            }
            break;
        default:
            // For other types, no special cleanup needed
            break;
    }
    
    // Reset the value to null
    value->type = VALUE_NULL;
    memset(&value->data, 0, sizeof(value->data));
}

const char* value_type_to_string(ValueType type) {
    return value_type_string(type);
}
