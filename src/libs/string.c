#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "../../include/core/interpreter.h"
#include "../../include/core/ast.h"
#include "../../include/core/standardized_errors.h"
#include "../../include/utils/shared_utilities.h"

// String utility functions
Value builtin_string_upper(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_ARGUMENT_COUNT, "string", "builtin_string_upper", 
                        "upper() requires exactly 1 argument", line, column);
        return value_create_null();
    }
    
    Value arg = args[0];
    if (arg.type != VALUE_STRING || !arg.data.string_value) {
        std_error_report(ERROR_INVALID_ARGUMENT, "string", "builtin_string_upper", 
                        "upper() argument must be a string", line, column);
        return value_create_null();
    }
    
    size_t len = strlen(arg.data.string_value);
    char* result = shared_malloc_safe(len + 1, "string", "unknown_function", 25);
    if (!result) {
        std_error_report(ERROR_OUT_OF_MEMORY, "string", "unknown_function", "Out of memory in upper()", line, column);
        return value_create_null();
    }
    
    for (size_t i = 0; i < len; i++) {
        result[i] = toupper((unsigned char)arg.data.string_value[i]);
    }
    result[len] = '\0';
    
    Value ret = value_create_string(result);
    shared_free_safe(result, "string", "unknown_function", 37);
    return ret;
}

Value builtin_string_lower(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_ARGUMENT_COUNT, "string", "unknown_function", "lower() requires exactly 1 argument", line, column);
        return value_create_null();
    }
    
    Value arg = args[0];
    if (arg.type != VALUE_STRING || !arg.data.string_value) {
        std_error_report(ERROR_INVALID_ARGUMENT, "string", "unknown_function", "lower() argument must be a string", line, column);
        return value_create_null();
    }
    
    size_t len = strlen(arg.data.string_value);
    char* result = shared_malloc_safe(len + 1, "string", "unknown_function", 54);
    if (!result) {
        std_error_report(ERROR_OUT_OF_MEMORY, "string", "unknown_function", "Out of memory in lower()", line, column);
        return value_create_null();
    }
    
    for (size_t i = 0; i < len; i++) {
        result[i] = tolower((unsigned char)arg.data.string_value[i]);
    }
    result[len] = '\0';
    
    Value ret = value_create_string(result);
    shared_free_safe(result, "string", "unknown_function", 66);
    return ret;
}

Value builtin_string_trim(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_ARGUMENT_COUNT, "string", "unknown_function", "trim() requires exactly 1 argument", line, column);
        return value_create_null();
    }
    
    Value arg = args[0];
    if (arg.type != VALUE_STRING || !arg.data.string_value) {
        std_error_report(ERROR_INVALID_ARGUMENT, "string", "unknown_function", "trim() argument must be a string", line, column);
        return value_create_null();
    }
    
    const char* str = arg.data.string_value;
    size_t len = strlen(str);
    
    // Find start of non-whitespace
    size_t start = 0;
    while (start < len && isspace((unsigned char)str[start])) {
        start++;
    }
    
    // Find end of non-whitespace
    size_t end = len;
    while (end > start && isspace((unsigned char)str[end - 1])) {
        end--;
    }
    
    if (start >= end) {
        return value_create_string("");
    }
    
    size_t result_len = end - start;
    char* result = shared_malloc_safe(result_len + 1, "string", "unknown_function", 102);
    if (!result) {
        std_error_report(ERROR_OUT_OF_MEMORY, "string", "unknown_function", "Out of memory in trim()", line, column);
        return value_create_null();
    }
    
    strncpy(result, str + start, result_len);
    result[result_len] = '\0';
    
    Value ret = value_create_string(result);
    shared_free_safe(result, "string", "unknown_function", 112);
    return ret;
}

Value builtin_string_split(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        std_error_report(ERROR_ARGUMENT_COUNT, "string", "unknown_function", "split() requires exactly 2 arguments", line, column);
        return value_create_null();
    }
    
    Value str_arg = args[0];
    Value delim_arg = args[1];
    
    if (str_arg.type != VALUE_STRING || !str_arg.data.string_value ||
        delim_arg.type != VALUE_STRING || !delim_arg.data.string_value) {
        std_error_report(ERROR_INTERNAL_ERROR, "string", "unknown_function", "split() arguments must be strings", line, column);
        return value_create_null();
    }
    
    const char* str = str_arg.data.string_value;
    const char* delimiter = delim_arg.data.string_value;
    
    if (strlen(delimiter) == 0) {
        std_error_report(ERROR_INTERNAL_ERROR, "string", "unknown_function", "split() delimiter cannot be empty", line, column);
        return value_create_null();
    }
    
    // Count tokens
    size_t token_count = 0;
    const char* temp = str;
    while ((temp = strstr(temp, delimiter)) != NULL) {
        token_count++;
        temp += strlen(delimiter);
    }
    token_count++; // Add one for the last token
    
    // Create array
    Value array = value_create_array(token_count);
    if (array.type != VALUE_ARRAY) {
        std_error_report(ERROR_INTERNAL_ERROR, "string", "unknown_function", "Failed to create array in split()", line, column);
        return value_create_null();
    }
    
    // Split the string
    temp = str;
    size_t current_pos = 0;
    size_t token_index = 0;
    
    while (token_index < token_count) {
        const char* next_delim = strstr(temp, delimiter);
        size_t token_len;
        
        if (next_delim) {
            token_len = next_delim - temp;
        } else {
            token_len = strlen(temp);
        }
        
        if (token_len > 0) {
            char* token = shared_malloc_safe(token_len + 1, "string", "unknown_function", 171);
            if (token) {
                strncpy(token, temp, token_len);
                token[token_len] = '\0';
                
                Value token_value = value_create_string(token);
                value_array_push(&array, token_value);
                value_free(&token_value);
                shared_free_safe(token, "string", "unknown_function", 179);
            }
        } else {
            // Empty token
            Value empty_token = value_create_string("");
            value_array_push(&array, empty_token);
            value_free(&empty_token);
        }
        
        if (next_delim) {
            temp = next_delim + strlen(delimiter);
        } else {
            break;
        }
        token_index++;
    }
    
    return array;
}

Value builtin_string_join(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        std_error_report(ERROR_ARGUMENT_COUNT, "string", "unknown_function", "join() requires exactly 2 arguments", line, column);
        return value_create_null();
    }
    
    Value array_arg = args[0];
    Value separator_arg = args[1];
    
    if (array_arg.type != VALUE_ARRAY || separator_arg.type != VALUE_STRING || !separator_arg.data.string_value) {
        std_error_report(ERROR_INVALID_ARGUMENT, "string", "unknown_function", "join() first argument must be array, second must be string", line, column);
        return value_create_null();
    }
    
    const char* separator = separator_arg.data.string_value;
    size_t array_len = array_arg.data.array_value.count;
    
    if (array_len == 0) {
        return value_create_string("");
    }
    
    // Calculate total length needed
    size_t total_len = 0;
    for (size_t i = 0; i < array_len; i++) {
        Value* element = (Value*)array_arg.data.array_value.elements[i];
        if (element && element->type == VALUE_STRING && element->data.string_value) {
            total_len += strlen(element->data.string_value);
        }
        if (i < array_len - 1) {
            total_len += strlen(separator);
        }
    }
    
    // Build result string
    char* result = shared_malloc_safe(total_len + 1, "string", "unknown_function", 233);
    if (!result) {
        std_error_report(ERROR_OUT_OF_MEMORY, "string", "unknown_function", "Out of memory in join()", line, column);
        return value_create_null();
    }
    
    result[0] = '\0';
    for (size_t i = 0; i < array_len; i++) {
        Value* element = (Value*)array_arg.data.array_value.elements[i];
        if (element && element->type == VALUE_STRING && element->data.string_value) {
            strcat(result, element->data.string_value);
        }
        if (i < array_len - 1) {
            strcat(result, separator);
        }
    }
    
    Value ret = value_create_string(result);
    shared_free_safe(result, "string", "unknown_function", 251);
    return ret;
}

Value builtin_string_contains(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        std_error_report(ERROR_ARGUMENT_COUNT, "string", "unknown_function", "contains() requires exactly 2 arguments", line, column);
        return value_create_null();
    }
    
    Value str_arg = args[0];
    Value substr_arg = args[1];
    
    if (str_arg.type != VALUE_STRING || !str_arg.data.string_value ||
        substr_arg.type != VALUE_STRING || !substr_arg.data.string_value) {
        std_error_report(ERROR_INTERNAL_ERROR, "string", "unknown_function", "contains() arguments must be strings", line, column);
        return value_create_null();
    }
    
    const char* str = str_arg.data.string_value;
    const char* substr = substr_arg.data.string_value;
    
    return value_create_boolean(strstr(str, substr) != NULL);
}

Value builtin_string_starts_with(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        std_error_report(ERROR_ARGUMENT_COUNT, "string", "unknown_function", "starts_with() requires exactly 2 arguments", line, column);
        return value_create_null();
    }
    
    Value str_arg = args[0];
    Value prefix_arg = args[1];
    
    if (str_arg.type != VALUE_STRING || !str_arg.data.string_value ||
        prefix_arg.type != VALUE_STRING || !prefix_arg.data.string_value) {
        std_error_report(ERROR_INTERNAL_ERROR, "string", "unknown_function", "starts_with() arguments must be strings", line, column);
        return value_create_null();
    }
    
    const char* str = str_arg.data.string_value;
    const char* prefix = prefix_arg.data.string_value;
    
    size_t prefix_len = strlen(prefix);
    if (strlen(str) < prefix_len) {
        return value_create_boolean(0);
    }
    
    return value_create_boolean(strncmp(str, prefix, prefix_len) == 0);
}

Value builtin_string_ends_with(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        std_error_report(ERROR_ARGUMENT_COUNT, "string", "unknown_function", "ends_with() requires exactly 2 arguments", line, column);
        return value_create_null();
    }
    
    Value str_arg = args[0];
    Value suffix_arg = args[1];
    
    if (str_arg.type != VALUE_STRING || !str_arg.data.string_value ||
        suffix_arg.type != VALUE_STRING || !suffix_arg.data.string_value) {
        std_error_report(ERROR_INTERNAL_ERROR, "string", "unknown_function", "ends_with() arguments must be strings", line, column);
        return value_create_null();
    }
    
    const char* str = str_arg.data.string_value;
    const char* suffix = suffix_arg.data.string_value;
    
    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);
    
    if (str_len < suffix_len) {
        return value_create_boolean(0);
    }
    
    return value_create_boolean(strcmp(str + str_len - suffix_len, suffix) == 0);
}

Value builtin_string_replace(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 3) {
        std_error_report(ERROR_ARGUMENT_COUNT, "string", "unknown_function", "replace() requires exactly 3 arguments", line, column);
        return value_create_null();
    }
    
    Value str_arg = args[0];
    Value old_arg = args[1];
    Value new_arg = args[2];
    
    if (str_arg.type != VALUE_STRING || !str_arg.data.string_value ||
        old_arg.type != VALUE_STRING || !old_arg.data.string_value ||
        new_arg.type != VALUE_STRING || !new_arg.data.string_value) {
        std_error_report(ERROR_INTERNAL_ERROR, "string", "unknown_function", "replace() arguments must be strings", line, column);
        return value_create_null();
    }
    
    const char* str = str_arg.data.string_value;
    const char* old_str = old_arg.data.string_value;
    const char* new_str = new_arg.data.string_value;
    
    if (strlen(old_str) == 0) {
        return value_create_string(str);
    }
    
    // Count occurrences
    size_t count = 0;
    const char* temp = str;
    while ((temp = strstr(temp, old_str)) != NULL) {
        count++;
        temp += strlen(old_str);
    }
    
    if (count == 0) {
        return value_create_string(str);
    }
    
    // Calculate new length
    size_t old_len = strlen(old_str);
    size_t new_len = strlen(new_str);
    size_t str_len = strlen(str);
    size_t result_len = str_len + count * (new_len - old_len);
    
    // Allocate result string
    char* result = shared_malloc_safe(result_len + 1, "string", "unknown_function", 374);
    if (!result) {
        std_error_report(ERROR_OUT_OF_MEMORY, "string", "unknown_function", "Out of memory in replace()", line, column);
        return value_create_null();
    }
    
    // Build result
    char* dest = result;
    const char* src = str;
    
    while (*src) {
        if (strncmp(src, old_str, old_len) == 0) {
            strcpy(dest, new_str);
            dest += new_len;
            src += old_len;
        } else {
            *dest++ = *src++;
        }
    }
    *dest = '\0';
    
    Value ret = value_create_string(result);
    shared_free_safe(result, "string", "unknown_function", 396);
    return ret;
}

Value builtin_string_repeat(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        std_error_report(ERROR_ARGUMENT_COUNT, "string", "unknown_function", "repeat() requires exactly 2 arguments", line, column);
        return value_create_null();
    }
    
    Value str_arg = args[0];
    Value count_arg = args[1];
    
    if (str_arg.type != VALUE_STRING || !str_arg.data.string_value ||
        count_arg.type != VALUE_NUMBER) {
        std_error_report(ERROR_INVALID_ARGUMENT, "string", "unknown_function", "repeat() first argument must be string, second must be number", line, column);
        return value_create_null();
    }
    
    const char* str = str_arg.data.string_value;
    int count = (int)count_arg.data.number_value;
    
    if (count < 0) {
        std_error_report(ERROR_INTERNAL_ERROR, "string", "unknown_function", "repeat() count cannot be negative", line, column);
        return value_create_null();
    }
    
    if (count == 0) {
        return value_create_string("");
    }
    
    size_t str_len = strlen(str);
    size_t result_len = str_len * count;
    
    char* result = shared_malloc_safe(result_len + 1, "string", "unknown_function", 430);
    if (!result) {
        std_error_report(ERROR_OUT_OF_MEMORY, "string", "unknown_function", "Out of memory in repeat()", line, column);
        return value_create_null();
    }
    
    result[0] = '\0';
    for (int i = 0; i < count; i++) {
        strcat(result, str);
    }
    
    Value ret = value_create_string(result);
    shared_free_safe(result, "string", "unknown_function", 442);
    return ret;
}

// toString() method for converting any value to string
Value builtin_toString(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_ARGUMENT_COUNT, "string", "unknown_function", "toString() requires exactly 1 argument", line, column);
        return value_create_null();
    }
    
    return value_to_string(&args[0]);
}

// Register string library with interpreter
void string_library_register(Interpreter* interpreter) {
    if (!interpreter || !interpreter->global_environment) return;
    
    // toString is now handled as a method on all value types (e.g., value.toString())
    // No longer registered as a global function
}
