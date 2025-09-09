#include "myco_runtime.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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
    v.data.string_value = strdup(value);
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
        free(value.data.string_value);
    }
}

// Convert Myco value to string
char* myco_value_to_string(MycoValue value) {
    switch (value.type) {
        case MYCO_TYPE_NUMBER:
            return myco_string_from_number(value.data.number_value);
        case MYCO_TYPE_STRING:
            return strdup(value.data.string_value);
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
    char* result = malloc(len1 + len2 + 1);
    
    if (result) {
        strcpy(result, str1);
        strcat(result, str2);
    }
    
    return result;
}

char* myco_string_from_number(double number) {
    char* result = malloc(64);
    if (result) {
        snprintf(result, 64, "%.6g", number);
    }
    return result;
}

char* myco_string_from_bool(int bool_value) {
    return strdup(bool_value ? "true" : "false");
}

char* myco_number_to_string(double number) {
    char* result = malloc(64);
    if (result) {
        snprintf(result, 64, "%g", number);
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
    return strdup((const char*)value);
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

// Memory management
void* myco_malloc(size_t size) {
    return malloc(size);
}

void myco_free(void* ptr) {
    free(ptr);
}
