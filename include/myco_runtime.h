#ifndef MYCO_RUNTIME_H
#define MYCO_RUNTIME_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Myco runtime types
typedef enum {
    MYCO_TYPE_NUMBER,
    MYCO_TYPE_STRING,
    MYCO_TYPE_BOOL,
    MYCO_TYPE_NULL,
    MYCO_TYPE_ARRAY,
    MYCO_TYPE_OBJECT
} MycoType;

typedef struct {
    MycoType type;
    union {
        double number_value;
        char* string_value;
        int bool_value;
    } data;
} MycoValue;

// Myco runtime functions
MycoValue myco_value_number(double value);
MycoValue myco_value_string(const char* value);
MycoValue myco_value_bool(int value);
MycoValue myco_value_null(void);

void myco_value_free(MycoValue value);
char* myco_value_to_string(MycoValue value);

// String operations
char* myco_string_concat(const char* str1, const char* str2);
char* myco_string_from_number(double number);
char* myco_string_from_bool(int bool_value);
char* myco_number_to_string(double number);
char* myco_to_string(void* value); // General string conversion

// Built-in functions
void myco_print(const char* str);
void myco_print_number(double number);
void myco_print_bool(int bool_value);

// Type checking functions
int isString(void* value);
int isInt(void* value);
int isFloat(void* value);
const char* myco_get_type(MycoValue value);
int isBool(void* value);
int isArray(void* value);
int isNull(void* value);
int isNumber(void* value);

// Type checking functions for double values
int isInt_double(double value);
int isFloat_double(double value);

// Get type name as string
char* myco_get_type_name(void* value);

char* myco_safe_to_string(void* value);

// Memory management
void* myco_malloc(size_t size);
void myco_free(void* ptr);

#endif // MYCO_RUNTIME_H
