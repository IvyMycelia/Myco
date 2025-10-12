#include "interpreter/value_operations.h"
#include "../../include/core/interpreter.h"
#include "../../include/utils/shared_utilities.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

// ============================================================================
// ARITHMETIC OPERATIONS
// ============================================================================

Value value_add(Value* a, Value* b) {
    if (!a || !b) {
        return value_create_null();
    }
    
    // String concatenation
    if (a->type == VALUE_STRING || b->type == VALUE_STRING) {
        Value sa = value_to_string(a);
        Value sb = value_to_string(b);
        
        // Get string lengths safely
        size_t la = 0;
        size_t lb = 0;
        if (sa.type == VALUE_STRING && sa.data.string_value) {
            la = strlen(sa.data.string_value);
        }
        if (sb.type == VALUE_STRING && sb.data.string_value) {
            lb = strlen(sb.data.string_value);
        }
        
        // Allocate memory for concatenated string
        char* out = (char*)shared_malloc_safe(la + lb + 1, "interpreter", "value_add", 0);
        if (!out) {
            value_free(&sa);
            value_free(&sb);
            return value_create_string("");
        }
        
        // Copy strings safely
        if (la > 0 && sa.type == VALUE_STRING && sa.data.string_value) {
            memcpy(out, sa.data.string_value, la);
        }
        if (lb > 0 && sb.type == VALUE_STRING && sb.data.string_value) {
            memcpy(out + la, sb.data.string_value, lb);
        }
        out[la + lb] = '\0';
        
        // Clean up temporary values
        value_free(&sa);
        value_free(&sb);
        
        // Create result and free temporary buffer
        Value result = value_create_string(out);
        shared_free_safe(out, "interpreter", "value_add", 0);
        return result;
    }
    
    // Array concatenation
    if (a->type == VALUE_ARRAY && b->type == VALUE_ARRAY) {
        // Create a new array with combined elements
        Value result = value_create_array(0);
        
        // Add elements from first array
        for (size_t i = 0; i < a->data.array_value.count; i++) {
            Value* element = (Value*)a->data.array_value.elements[i];
            if (element) {
                Value cloned_element = value_clone(element);
                value_array_push(&result, cloned_element);
            }
        }
        
        // Add elements from second array
        for (size_t i = 0; i < b->data.array_value.count; i++) {
            Value* element = (Value*)b->data.array_value.elements[i];
            if (element) {
                Value cloned_element = value_clone(element);
                value_array_push(&result, cloned_element);
            }
        }
        
        return result;
    }
    
    // Array + single element (append)
    if (a->type == VALUE_ARRAY) {
        Value result = value_create_array(0);
        
        // Add elements from array
        for (size_t i = 0; i < a->data.array_value.count; i++) {
            Value* element = (Value*)a->data.array_value.elements[i];
            if (element) {
                Value cloned_element = value_clone(element);
                value_array_push(&result, cloned_element);
            }
        }
        
        // Add the single element
        Value cloned_b = value_clone(b);
        value_array_push(&result, cloned_b);
        
        return result;
    }
    
    // Numeric addition
    if (a->type == VALUE_NUMBER && b->type == VALUE_NUMBER) {
        return value_create_number(a->data.number_value + b->data.number_value);
    }
    
    return value_create_null();
}

Value value_subtract(Value* a, Value* b) {
    if (!a || !b) {
        return value_create_null();
    }
    
    if (a->type == VALUE_NUMBER && b->type == VALUE_NUMBER) {
        return value_create_number(a->data.number_value - b->data.number_value);
    }
    
    return value_create_null();
}

Value value_multiply(Value* a, Value* b) { 
    if (a->type == VALUE_NUMBER && b->type == VALUE_NUMBER) {
        return value_create_number(a->data.number_value * b->data.number_value);
    }
    return value_create_null(); 
}

Value value_divide(Value* a, Value* b) { 
    if (a->type == VALUE_NUMBER && b->type == VALUE_NUMBER) {
        if (b->data.number_value == 0.0) {
            // Set error state for try/catch handling
            // Note: We need access to the interpreter to set error state
            // For now, return null and let the caller handle it
            return value_create_null(); // Division by zero
        }
        return value_create_number(a->data.number_value / b->data.number_value);
    }
    return value_create_null(); 
}

Value value_modulo(Value* a, Value* b) { 
    if (a->type == VALUE_NUMBER && b->type == VALUE_NUMBER) {
        if (b->data.number_value == 0.0) {
            return value_create_null(); // Division by zero
        }
        return value_create_number(fmod(a->data.number_value, b->data.number_value));
    }
    return value_create_null(); 
}

Value value_power(Value* a, Value* b) { 
    if (a->type == VALUE_NUMBER && b->type == VALUE_NUMBER) {
        return value_create_number(pow(a->data.number_value, b->data.number_value));
    }
    return value_create_null(); 
}

// ============================================================================
// COMPARISON OPERATIONS
// ============================================================================

Value value_equal(Value* a, Value* b) {
    if (!a || !b) {
        return value_create_boolean(0);
    }
    
    // Use the existing value_equals function and convert to boolean
    int equals = value_equals(a, b);
    return value_create_boolean(equals);
}

Value value_not_equal(Value* a, Value* b) { 
    if (!a || !b) {
        return value_create_boolean(1);
    }
    
    int equals = value_equals(a, b);
    return value_create_boolean(!equals);
}

Value value_less_than(Value* a, Value* b) { 
    if (!a || !b) {
        return value_create_boolean(0);
    }
    
    if (a->type == VALUE_NUMBER && b->type == VALUE_NUMBER) {
        return value_create_boolean(a->data.number_value < b->data.number_value);
    }
    
    return value_create_boolean(0);
}

Value value_greater_than(Value* a, Value* b) { 
    if (!a || !b) {
        return value_create_boolean(0);
    }
    
    if (a->type == VALUE_NUMBER && b->type == VALUE_NUMBER) {
        return value_create_boolean(a->data.number_value > b->data.number_value);
    }
    
    return value_create_boolean(0);
}

Value value_less_equal(Value* a, Value* b) { 
    if (!a || !b) {
        return value_create_boolean(0);
    }
    
    if (a->type == VALUE_NUMBER && b->type == VALUE_NUMBER) {
        return value_create_boolean(a->data.number_value <= b->data.number_value);
    }
    
    return value_create_boolean(0);
}

Value value_greater_equal(Value* a, Value* b) { 
    if (!a || !b) {
        return value_create_boolean(0);
    }
    
    if (a->type == VALUE_NUMBER && b->type == VALUE_NUMBER) {
        return value_create_boolean(a->data.number_value >= b->data.number_value);
    }
    
    return value_create_boolean(0);
}

// ============================================================================
// LOGICAL OPERATIONS
// ============================================================================

Value value_logical_and(Value* a, Value* b) { 
    if (!a || !b) {
        return value_create_boolean(0);
    }
    
    Value bool_a = value_to_boolean(a);
    Value bool_b = value_to_boolean(b);
    
    int result = (bool_a.type == VALUE_BOOLEAN && bool_a.data.boolean_value) &&
                 (bool_b.type == VALUE_BOOLEAN && bool_b.data.boolean_value);
    
    value_free(&bool_a);
    value_free(&bool_b);
    
    return value_create_boolean(result);
}

Value value_logical_or(Value* a, Value* b) { 
    if (!a || !b) {
        return value_create_boolean(0);
    }
    
    Value bool_a = value_to_boolean(a);
    Value bool_b = value_to_boolean(b);
    
    int result = (bool_a.type == VALUE_BOOLEAN && bool_a.data.boolean_value) ||
                 (bool_b.type == VALUE_BOOLEAN && bool_b.data.boolean_value);
    
    value_free(&bool_a);
    value_free(&bool_b);
    
    return value_create_boolean(result);
}

Value value_logical_xor(Value* a, Value* b) { 
    if (!a || !b) {
        return value_create_boolean(0);
    }
    
    Value bool_a = value_to_boolean(a);
    Value bool_b = value_to_boolean(b);
    
    int a_val = (bool_a.type == VALUE_BOOLEAN && bool_a.data.boolean_value) ? 1 : 0;
    int b_val = (bool_b.type == VALUE_BOOLEAN && bool_b.data.boolean_value) ? 1 : 0;
    
    value_free(&bool_a);
    value_free(&bool_b);
    
    return value_create_boolean(a_val ^ b_val);
}

Value value_logical_not(Value* a) { 
    if (!a) {
        return value_create_boolean(1);
    }
    
    Value bool_a = value_to_boolean(a);
    int result = !(bool_a.type == VALUE_BOOLEAN && bool_a.data.boolean_value);
    
    value_free(&bool_a);
    
    return value_create_boolean(result);
}

// ============================================================================
// BITWISE OPERATIONS
// ============================================================================

Value value_bitwise_and(Value* a, Value* b) { 
    if (!a || !b) {
        return value_create_null();
    }
    
    if (a->type == VALUE_NUMBER && b->type == VALUE_NUMBER) {
        long long int_a = (long long)a->data.number_value;
        long long int_b = (long long)b->data.number_value;
        return value_create_number((double)(int_a & int_b));
    }
    
    return value_create_null();
}

Value value_bitwise_or(Value* a, Value* b) { 
    if (!a || !b) {
        return value_create_null();
    }
    
    if (a->type == VALUE_NUMBER && b->type == VALUE_NUMBER) {
        long long int_a = (long long)a->data.number_value;
        long long int_b = (long long)b->data.number_value;
        return value_create_number((double)(int_a | int_b));
    }
    
    return value_create_null();
}

Value value_bitwise_xor(Value* a, Value* b) { 
    if (!a || !b) {
        return value_create_null();
    }
    
    if (a->type == VALUE_NUMBER && b->type == VALUE_NUMBER) {
        long long int_a = (long long)a->data.number_value;
        long long int_b = (long long)b->data.number_value;
        return value_create_number((double)(int_a ^ int_b));
    }
    
    return value_create_null();
}

Value value_bitwise_not(Value* a) { 
    if (!a) {
        return value_create_null();
    }
    
    if (a->type == VALUE_NUMBER) {
        long long int_a = (long long)a->data.number_value;
        return value_create_number((double)(~int_a));
    }
    
    return value_create_null();
}

Value value_left_shift(Value* a, Value* b) { 
    if (!a || !b) {
        return value_create_null();
    }
    
    if (a->type == VALUE_NUMBER && b->type == VALUE_NUMBER) {
        long long int_a = (long long)a->data.number_value;
        long long int_b = (long long)b->data.number_value;
        return value_create_number((double)(int_a << int_b));
    }
    
    return value_create_null();
}

Value value_right_shift(Value* a, Value* b) { 
    if (!a || !b) {
        return value_create_null();
    }
    
    if (a->type == VALUE_NUMBER && b->type == VALUE_NUMBER) {
        long long int_a = (long long)a->data.number_value;
        long long int_b = (long long)b->data.number_value;
        return value_create_number((double)(int_a >> int_b));
    }
    
    return value_create_null();
}
