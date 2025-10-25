#include "interpreter/value_operations.h"
#include "../../include/core/interpreter.h"
#include "../../include/utils/shared_utilities.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

/**
 * @brief Process escape sequences in a string
 * 
 * @param input The input string with escape sequences
 * @return A new string with escape sequences processed
 */
static char* process_escape_sequences(const char* input) {
    if (!input) return NULL;
    
    size_t input_len = strlen(input);
    char* output = shared_malloc_safe(input_len + 1, "interpreter", "unknown_function", 214);  // Output might be shorter due to \n -> single char
    if (!output) return NULL;
    
    size_t output_pos = 0;
    size_t input_pos = 0;
    
    while (input_pos < input_len) {
        if (input[input_pos] == '\\' && input_pos + 1 < input_len) {
            // Process escape sequence
            input_pos++;  // Skip the backslash
            switch (input[input_pos]) {
                case 'n':
                    output[output_pos++] = '\n';
                    break;
                case 't':
                    output[output_pos++] = '\t';
                    break;
                case 'r':
                    output[output_pos++] = '\r';
                    break;
                case '\\':
                    output[output_pos++] = '\\';
                    break;
                case '"':
                    output[output_pos++] = '"';
                    break;
                default:
                    // Unknown escape sequence, treat as literal
                    output[output_pos++] = '\\';
                    output[output_pos++] = input[input_pos];
                    break;
            }
            input_pos++;
        } else {
            // Copy character as-is
            output[output_pos++] = input[input_pos++];
        }
    }
    
    output[output_pos] = '\0';
    return output;
}

// ============================================================================
// PRIMITIVE VALUE CREATION FUNCTIONS
// ============================================================================

Value value_create_null(void) { 
    Value v = {0}; 
    v.type = VALUE_NULL; 
    v.flags = VALUE_FLAG_IMMUTABLE;
    v.ref_count = 1;
    v.cache.cached_ptr = NULL;
    return v; 
}

Value value_create_boolean(int value) { 
    Value v = {0};  // Initialize entire struct to zero
    v.type = VALUE_BOOLEAN; 
    v.flags = VALUE_FLAG_IMMUTABLE;
    v.ref_count = 1;
    v.data.boolean_value = value ? 1 : 0; 
    v.cache.cached_numeric = value ? 1.0 : 0.0;
    return v; 
}

Value value_create_number(double value) { 
    Value v = {0};  // Initialize entire struct to zero
    v.type = VALUE_NUMBER; 
    v.flags = VALUE_FLAG_IMMUTABLE | VALUE_FLAG_CACHED;
    v.ref_count = 1;
    v.data.number_value = value; 
    v.cache.cached_numeric = value;
    return v; 
}

// Optimized Value creation functions
Value value_create_optimized(ValueType type, uint8_t flags) {
    Value value;
    value.type = type;
    value.flags = flags;
    value.ref_count = 1;
    value.cache.cached_ptr = NULL;
    return value;
}

Value value_create_cached_string(const char* value) {
    Value val = value_create_optimized(VALUE_STRING, VALUE_FLAG_CACHED | VALUE_FLAG_REFCOUNTED);
    val.data.string_value = shared_malloc_safe(strlen(value) + 1, "interpreter", "value_create_cached_string", 0);
    if (val.data.string_value) {
        strcpy(val.data.string_value, value);
        val.cache.cached_length = strlen(value);
    }
    return val;
}

Value value_create_immutable_number(double value) {
    Value val = value_create_optimized(VALUE_NUMBER, VALUE_FLAG_IMMUTABLE | VALUE_FLAG_CACHED);
    val.data.number_value = value;
    val.cache.cached_numeric = value;
    return val;
}

Value value_create_pooled_array(size_t initial_capacity) {
    Value val = value_create_optimized(VALUE_ARRAY, VALUE_FLAG_POOLED | VALUE_FLAG_REFCOUNTED);
    val.data.array_value.elements = shared_malloc_safe(sizeof(Value) * initial_capacity, "interpreter", "value_create_pooled_array", 0);
    val.data.array_value.count = 0;
    val.data.array_value.capacity = initial_capacity;
    val.cache.cached_length = 0;
    return val;
}

Value value_create_string(const char* value) { 
    Value v = {0}; 
    v.type = VALUE_STRING; 
    v.flags = VALUE_FLAG_IMMUTABLE | VALUE_FLAG_CACHED;
    v.ref_count = 1;
    
    if (value) {
        // Process escape sequences when creating the string
        char* processed_value = process_escape_sequences(value);
        if (processed_value) {
            v.data.string_value = processed_value;
            v.cache.cached_length = strlen(processed_value);
        } else {
            // Fallback to original value if processing fails
            v.data.string_value = (value ? shared_strdup(value) : NULL);
            v.cache.cached_length = value ? strlen(value) : 0;
        }
    } else {
        v.data.string_value = shared_strdup("");  // Use empty string instead of NULL
        v.cache.cached_length = 0;
    }
    v.cache.cached_ptr = NULL;
    
    return v; 
}

Value value_create_range(double start, double end, double step, int inclusive) { 
    Value v; 
    v.type = VALUE_RANGE; 
    v.flags = 0;
    v.ref_count = 0;
    v.data.range_value.start = start; 
    v.data.range_value.end = end; 
    v.data.range_value.step = step; 
    v.data.range_value.inclusive = inclusive; 
    return v; 
}

// Reference counting functions
void value_increment_ref(Value* value) {
    if (value && (value->flags & VALUE_FLAG_REFCOUNTED)) {
        value->ref_count++;
    }
}

void value_decrement_ref(Value* value) {
    if (value && (value->flags & VALUE_FLAG_REFCOUNTED)) {
        value->ref_count--;
        if (value->ref_count == 0) {
            value_free(value);
        }
    }
}

// Value flag checking functions
int value_is_cached(Value* value) {
    return value && (value->flags & VALUE_FLAG_CACHED);
}

int value_is_immutable(Value* value) {
    return value && (value->flags & VALUE_FLAG_IMMUTABLE);
}

int value_is_pooled(Value* value) {
    return value && (value->flags & VALUE_FLAG_POOLED);
}
