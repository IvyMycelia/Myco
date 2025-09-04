// Generated C code from Myco
// Cross-platform compatible

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

// Myco runtime includes
#include "../include/core/interpreter.h"
#include "../include/libs/builtin_libs.h"

// Myco type definitions
typedef struct {
    char* data;
    size_t length;
} MycoString;

typedef struct {
    void** elements;
    size_t count;
    size_t capacity;
} MycoArray;

typedef struct {
    char** keys;
    void** values;
    size_t count;
    size_t capacity;
} MycoObject;

// Function declarations
void myco_print(const char* str);
void myco_uprint(const char* str);
void myco_print_int(int value);
void myco_print_float(double value);
void myco_print_bool(bool value);
char* myco_input(void);
int myco_len_string(const char* str);
int myco_len_array(MycoArray* arr);
char* myco_type_to_string(int type);
char* myco_str_int(int value);
char* myco_str_float(double value);
char* myco_str_bool(bool value);
char* myco_string_concat(const char* str1, const char* str2);

// Myco built-in function implementations
void myco_print(const char* str) {
    printf("%s\n", str);
}

void myco_uprint(const char* str) {
    printf("%s", str);
}

void myco_print_int(int value) {
    printf("%d\n", value);
}

void myco_print_float(double value) {
    printf("%.6f\n", value);
}

void myco_print_bool(bool value) {
    printf("%s\n", value ? "True" : "False");
}

char* myco_input(void) {
    char* buffer = malloc(1024);
    if (fgets(buffer, 1024, stdin)) {
        // Remove newline
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }
        return buffer;
    }
    return NULL;
}

int myco_len_string(const char* str) {
    return str ? (int)strlen(str) : 0;
}

int myco_len_array(MycoArray* arr) {
    return arr ? (int)arr->count : 0;
}

char* myco_type_to_string(int type) {
    switch (type) {
        case 0: return "Null";
        case 1: return "Boolean";
        case 2: return "Int";
        case 3: return "Float";
        case 4: return "String";
        case 5: return "Array";
        case 6: return "Object";
        default: return "Unknown";
    }
}

char* myco_str_int(int value) {
    char* buffer = malloc(32);
    snprintf(buffer, 32, "%d", value);
    return buffer;
}

char* myco_str_float(double value) {
    char* buffer = malloc(32);
    snprintf(buffer, 32, "%.6f", value);
    return buffer;
}

char* myco_str_bool(bool value) {
    return value ? strdup("True") : strdup("False");
}

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

// Main program entry point
int main(void) {
    myco_print    (    "=== Myco Comprehensive Test Suite ==="    )    ;
    myco_print    (    "Testing EVERY feature and edge case in Myco"    )    ;
    myco_print    (    ""    )    ;
    double total_tests     =     0.000000    ;
    double tests_passed     =     0.000000    ;
    double tests_failed     = 