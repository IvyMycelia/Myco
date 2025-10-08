#include "myco_runtime.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include "../../include/utils/shared_utilities.h"

// Myco runtime value creation
MycoValue myco_value_number(double value) {
    MycoValue v;
    v.type = MYCO_TYPE_NUMBER;
    v.data.number_value = value;
    return v;
}

MycoValue myco_value_string(const char* value) {
    MycoValue v;
    v.type = MYCO_TYPE_STRING;
    v.data.string_value = (value ? strdup(value) : NULL);
    return v;
}

MycoValue myco_value_bool(int value) {
    MycoValue v;
    v.type = MYCO_TYPE_BOOL;
    v.data.bool_value = value;
    return v;
}

MycoValue myco_value_null(void) {
    MycoValue v;
    v.type = MYCO_TYPE_NULL;
    return v;
}

// Myco runtime value cleanup
void myco_value_free(MycoValue value) {
    if (value.type == MYCO_TYPE_STRING && value.data.string_value) {
        shared_free_safe(value.data.string_value, "unknown", "unknown_function", 40);
    }
}

// Convert Myco value to string
char* myco_value_to_string(MycoValue value) {
    switch (value.type) {
        case MYCO_TYPE_NUMBER:
            return myco_string_from_number(value.data.number_value);
        case MYCO_TYPE_STRING:
            return (value.data.string_value ? strdup(value.data.string_value) : NULL);
        case MYCO_TYPE_BOOL:
            return myco_string_from_bool(value.data.bool_value);
        case MYCO_TYPE_NULL:
            return strdup("null");
        default:
            return strdup("unknown");
    }
}

// String operations
char* myco_string_concat(const char* str1, const char* str2) {
    if (!str1) str1 = "";
    if (!str2) str2 = "";
    
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);
    char* result = shared_malloc_safe(len1 + len2 + 1, "unknown", "unknown_function", 67);
    
    if (result) {
        strcpy(result, str1);
        strcat(result, str2);
    }
    
    return result;
}

char* myco_string_from_number(double number) {
    char* result = shared_malloc_safe(64, "unknown", "unknown_function", 78);
    if (result) {
        snprintf(result, 64, "%.6g", number);
    }
    return result;
}

char* myco_string_from_bool(int bool_value) {
    return strdup(bool_value ? "True" : "False");
}

char* myco_number_to_string(double number) {
    char* result = shared_malloc_safe(64, "unknown", "unknown_function", 90);
    if (result) {
        // Check if the number is a whole number (integer)
        if (number == (int)number) {
            snprintf(result, 64, "%d", (int)number);
        } else {
            snprintf(result, 64, "%.6f", number);
        }
    }
    return result;
}

char* myco_to_string(void* value) {
    if (!value) {
        return strdup("null");
    }
    // This is a placeholder - in a real implementation, we would need to
    // determine the type of the value and convert accordingly
    // For now, assume it's a string and return it
    return ((const char*)value) ? strdup((const char*)value) : NULL;
}

// Built-in functions
void myco_print(const char* str) {
    if (str) {
        printf("%s\n", str);
    }
}

void myco_print_number(double number) {
    printf("%.6g\n", number);
}

void myco_print_bool(int bool_value) {
    printf("%s\n", bool_value ? "true" : "false");
}

// Type checking functions
int isString(void* value) {
    // Simple heuristic: if it's a string literal or char*, return 1
    // This is a placeholder implementation
    return (value != NULL && (intptr_t)value > 1000) ? 1 : 0;
}

int isInt(void* value) {
    // Check if the value is a whole number
    if (value == NULL) return 0;
    
    // If it's a small integer cast to void*, check if it's a whole number
    if ((intptr_t)value < 1000) {
        return 1; // Small integers are treated as integers
    }
    
    // For larger values, we can't determine if they're integers with this approach
    return 0;
}

int isInt_double(double value) {
    // Check if the double value is a whole number
    return (value == (int)value) ? 1 : 0;
}

int isFloat(void* value) {
    // Check if the value is a floating point number
    if (value == NULL) return 0;
    
    // If it's a small integer cast to void*, it's not a float
    if ((intptr_t)value < 1000) {
        return 0; // Small integers are not floats
    }
    
    // For larger values, we can't determine if they're floats with this approach
    return 0;
}

int isFloat_double(double value) {
    // Check if the double value has a decimal part
    return (value != (int)value) ? 1 : 0;
}

int isBool(void* value) {
    // Simple heuristic: if it's 0 or 1, return 1
    // This is a placeholder implementation
    return (value != NULL && ((intptr_t)value == 0 || (intptr_t)value == 1)) ? 1 : 0;
}

int isArray(void* value) {
    // Simple heuristic: if it's not NULL and not a small integer, it might be an array
    // But exclude string literals by checking if it looks like a string
    if (value == NULL || (intptr_t)value <= 1000) {
        return 0;
    }
    
    // Check if it's a string literal by looking at the first few characters
    char* str = (char*)value;
    if (str[0] >= 32 && str[0] <= 126) { // Printable ASCII range
        // This looks like a string literal, not an array
        return 0;
    }
    
    // Otherwise, assume it's an array
    return 1;
}

int isNull(void* value) {
    return (value == NULL) ? 1 : 0;
}

int isNumber(void* value) {
    // Simple heuristic: if it's not NULL and not a string, return 1
    // This is a placeholder implementation
    return (value != NULL && (intptr_t)value < 1000) ? 1 : 0;
}

// Get type name as string
char* myco_get_type_name(void* value) {
    if (value == NULL) {
        return strdup("Null");
    } else if (isString(value)) {
        return strdup("String");
    } else if (isNumber(value)) {
        return strdup("Int"); // For simplicity, treat all numbers as Int
    } else if (isBool(value)) {
        return strdup("Boolean");
    } else if (isArray(value)) {
        return strdup("Array");
    } else {
        return strdup("Unknown");
    }
}

// Memory management
void* myco_malloc(size_t size) {
    return shared_malloc_safe(size, "unknown", "unknown_function", 223);
}

void myco_free(void* ptr) {
    shared_free_safe(ptr, "unknown", "unknown_function", 227);
}

char* myco_safe_to_string(void* value) {
    if (value == NULL) {
        return "Null";
    } else if (isString(value)) {
        return (char*)value;
    } else if (isNumber(value)) {
        // Check if this is a small integer cast to void* (common pattern in generated code)
        uintptr_t int_value = (uintptr_t)value;
        if (int_value < 1000) { // Small integers are likely cast to void* directly
            return myco_number_to_string((double)int_value);
        } else {
            // Try to dereference as double* (for actual double values)
            return myco_number_to_string(*(double*)value);
        }
    } else if (isArray(value)) {
        // For arrays, return a simple representation
        return "[Array]";
    } else {
        // For string literals cast to void*, just return them as strings
        return (char*)value;
    }
}
