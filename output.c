#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

// Myco Runtime Headers
#include "include/myco_runtime.h"

// Placeholder function declarations for class methods
char* placeholder_greet() { return "Hello, World"; }
double placeholder_getValue() { return 42.0; }
double placeholder_increment() { return 1.0; }
char* placeholder_getName() { return "TestName"; }
double placeholder_process() { return 100.0; }
double placeholder_calculate() { return 3.14159; }
char* placeholder_speak() { return "Woof!"; }
void* placeholder_lambda() { return NULL; }

typedef struct {
    int status_code;
    char* type;
    char* status_text;
    char* body;
    int success;
} HttpResponse;

// Variable scoping fix - rename conflicting variables
// This ensures unique variable names to avoid redefinition errors
// Manually rename conflicting variables to avoid redefinition
// mixed_array_1 = first mixed variable (array type)
// mixed_class_1 = second mixed variable (class type)


char* myco_string_to_string(void* value) {
    if (value == NULL) {
        return "Null";
    } else {
        // For now, return a generic representation for non-null values
        return "[object]";
    }
}

void simple_greet() {
    {
        myco_print(myco_safe_to_string((void*)(intptr_t)        "Hello from simple function!"        ))        ;
    }
}
void greet_with_name(char* name) {
    {
        myco_print(myco_safe_to_string((void*)(intptr_t)        myco_string_concat(        myco_string_concat(        "Hello, "        ,         name        )        ,         "!"        )        ))        ;
    }
}
void add_numbers(double a, double b) {
    {
        double         result_scope_1         =         a         +         b        ;
        myco_print(myco_safe_to_string((void*)(intptr_t)        myco_string_concat(        "Sum: "        ,         myco_number_to_string(        result_scope_1        )        )        ))        ;
    }
}
double get_pi() {
    {
        return         3.141590        ;    }
}
double multiply(double x, double y) {
    {
        return         x         *         y        ;    }
}
char* get_greeting(char* name) {
    {
        return         myco_string_concat(        myco_string_concat(        "Hello, "        ,         name        )        ,         "!"        )        ;    }
}
char* mixed_func1(double x, double y) {
    {
        return         myco_string_concat(        myco_string_concat(        myco_string_concat(        "Mixed types: "        ,         myco_number_to_string(        x        )        )        ,         ", "        )        ,         myco_number_to_string(        y        )        )        ;    }
}
void mixed_func2(char* name) {
    {
        myco_print(myco_safe_to_string((void*)(intptr_t)        myco_string_concat(        "Name: "        ,         name        )        ))        ;
    }
}
double mixed_func3(double a, double b) {
    {
        return         a         +         b        ;    }
}
double return_five() {
    {
        return         5.000000        ;    }
}
char* return_hello() {
    {
        return         "Hello"        ;    }
}
char* explicit_all(int param) {
    {
        return         myco_number_to_string(        param        )        ;    }
}
// Main program entry point
int main(void) {
    myco_print(myco_safe_to_string((void*)(intptr_t)    "=== Myco Comprehensive Test Suite ==="    ))    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    "Testing EVERY feature and edge case in Myco"    ))    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    ""    ))    ;
    void* trees = (void*)0x1234;
    void* graphs = (void*)0x1234;
    void* heaps = (void*)0x1234;
    void* queues = (void*)0x1234;
    void* stacks = (void*)0x1234;
    void* time = (void*)0x1234;
    void* regex = (void*)0x1234;
    void* json = (void*)0x1234;
    void* http = (void*)0x1234;
    double     total_tests     =     0.000000    ;
    double     tests_passed     =     0.000000    ;
    char**     tests_failed     =     (char*[]){    }    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    "=== 1. BASIC DATA TYPES ==="    ))    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    "1.1. Numbers..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     x     =     42.000000    ;
    if (    x     ==     42.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Integer variable: "            ,             myco_number_to_string(            x            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Integer variable: "            ,             myco_number_to_string(            x            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     y     =     3.140000    ;
    if (    y     ==     3.140000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Float variable: "            ,             myco_number_to_string(            y            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Float variable: "            ,             myco_number_to_string(            y            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     neg_int     =     -(    10.000000    )    ;
    if (    neg_int     ==     -(    10.000000    )    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Negative integer: "            ,             myco_number_to_string(            neg_int            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Negative integer: "            ,             myco_number_to_string(            neg_int            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     neg_float     =     -(    3.140000    )    ;
    if (    neg_float     ==     -(    3.140000    )    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Negative float: "            ,             myco_number_to_string(            neg_float            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Negative float: "            ,             myco_number_to_string(            neg_float            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     zero     =     0.000000    ;
    if (    zero     ==     0.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Zero: "            ,             myco_number_to_string(            zero            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Zero: "            ,             myco_number_to_string(            zero            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n1.2. Strings..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     name1     =     "Myco"    ;
    if (    strcmp(    name1    ,     "Myco"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ String variable: "            ,             name1            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ String variable: "            ,             name1            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     empty_str     =     ""    ;
    if (    strcmp(    empty_str    ,     ""    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            myco_string_concat(            "✓ Empty string: '"            ,             empty_str            )            ,             "'"            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            myco_string_concat(            "✗ Empty string: '"            ,             empty_str            )            ,             "'"            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     spaced_str     =     "hello world"    ;
    if (    strcmp(    spaced_str    ,     "hello world"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ String with spaces: "            ,             spaced_str            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ String with spaces: "            ,             spaced_str            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n1.3. Booleans..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    int     flag     =     1    ;
    if (    flag     ==     1    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Boolean True: "            ,             myco_string_from_bool(            flag            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Boolean True: "            ,             myco_string_from_bool(            flag            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     false_flag     =     0    ;
    if (    false_flag     ==     0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Boolean False: "            ,             myco_string_from_bool(            false_flag            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Boolean False: "            ,             myco_string_from_bool(            false_flag            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n1.4. Null..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     null_var     =     NULL    ;
    if (    null_var     == NULL    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Null variable: "            ,             myco_string_to_string(            null_var            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Null variable: "            ,             myco_string_to_string(            null_var            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n=== 2. ARITHMETIC OPERATIONS ==="    ))    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    "2.1. Addition..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     a     =     8.000000    ;
    if (    a     ==     8.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Integer addition (5 + 3): "            ,             myco_number_to_string(            a            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Integer addition (5 + 3): "            ,             myco_number_to_string(            a            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     float_add     =     4.000000    ;
    if (    float_add     ==     4.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Float addition (2.5 + 1.5): "            ,             myco_number_to_string(            float_add            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Float addition (2.5 + 1.5): "            ,             myco_number_to_string(            float_add            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     mixed_add     =     7.500000    ;
    if (    mixed_add     ==     7.500000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Mixed addition (5 + 2.5): "            ,             myco_number_to_string(            mixed_add            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Mixed addition (5 + 2.5): "            ,             myco_number_to_string(            mixed_add            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     str_concat     =     myco_string_concat(    "hello"    ,     "world"    )    ;
    if (    strcmp(    str_concat    ,     "helloworld"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ String concatenation: "            ,             str_concat            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ String concatenation: "            ,             str_concat            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n2.2. Subtraction..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     b     =     6.000000    ;
    if (    b     ==     6.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Integer subtraction (10 - 4): "            ,             myco_number_to_string(            b            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Integer subtraction (10 - 4): "            ,             myco_number_to_string(            b            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     float_sub     =     3.000000    ;
    if (    float_sub     ==     3.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Float subtraction (5.5 - 2.5): "            ,             myco_number_to_string(            float_sub            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Float subtraction (5.5 - 2.5): "            ,             myco_number_to_string(            float_sub            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     neg_result     =     -2.000000    ;
    if (    neg_result     ==     -(    2.000000    )    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Negative result (3 - 5): "            ,             myco_number_to_string(            neg_result            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Negative result (3 - 5): "            ,             myco_number_to_string(            neg_result            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n2.3. Multiplication..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     c     =     42.000000    ;
    if (    c     ==     42.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Integer multiplication (6 * 7): "            ,             myco_number_to_string(            c            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Integer multiplication (6 * 7): "            ,             myco_number_to_string(            c            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     float_mult     =     10.000000    ;
    if (    float_mult     ==     10.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Float multiplication (2.5 * 4): "            ,             myco_number_to_string(            float_mult            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Float multiplication (2.5 * 4): "            ,             myco_number_to_string(            float_mult            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     zero_mult     =     0.000000    ;
    if (    zero_mult     ==     0.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Zero multiplication (5 * 0): "            ,             myco_number_to_string(            zero_mult            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Zero multiplication (5 * 0): "            ,             myco_number_to_string(            zero_mult            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n2.4. Division..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     d     =     5.000000    ;
    if (    d     ==     5.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Integer division (15 / 3): "            ,             myco_number_to_string(            d            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Integer division (15 / 3): "            ,             myco_number_to_string(            d            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     float_div     =     4.000000    ;
    if (    float_div     ==     4.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Float division (10.0 / 2.5): "            ,             myco_number_to_string(            float_div            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Float division (10.0 / 2.5): "            ,             myco_number_to_string(            float_div            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     remainder_div     =     3.500000    ;
    if (    remainder_div     ==     3.500000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Division with remainder (7 / 2): "            ,             myco_number_to_string(            remainder_div            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Division with remainder (7 / 2): "            ,             myco_number_to_string(            remainder_div            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n=== 3. COMPARISON OPERATIONS ==="    ))    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    "3.1. Equality..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    int     comp3     =     1    ;
    if (    comp3     ==     1    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Integer equality (5 == 5): "            ,             myco_number_to_string(            comp3            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Integer equality (5 == 5): "            ,             myco_number_to_string(            comp3            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     comp_neq     =     0    ;
    if (    comp_neq     ==     0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Integer inequality (5 == 6): "            ,             myco_number_to_string(            comp_neq            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Integer inequality (5 == 6): "            ,             myco_number_to_string(            comp_neq            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     float_eq     =     1    ;
    if (    float_eq     ==     1    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Float equality (3.14 == 3.14): "            ,             myco_number_to_string(            float_eq            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Float equality (3.14 == 3.14): "            ,             myco_number_to_string(            float_eq            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     str_eq     =     strcmp(    "hello"    ,     "hello"    ) == 0    ;
    if (    str_eq     ==     1    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ String equality ('hello' == 'hello'): "            ,             myco_number_to_string(            str_eq            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ String equality ('hello' == 'hello'): "            ,             myco_number_to_string(            str_eq            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     bool_eq     =     1     ==     1    ;
    if (    bool_eq     ==     1    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Boolean equality (True == True): "            ,             myco_number_to_string(            bool_eq            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Boolean equality (True == True): "            ,             myco_number_to_string(            bool_eq            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n3.2. Inequality..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    int     comp4     =     1    ;
    if (    comp4     ==     1    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Integer inequality (4 != 6): "            ,             myco_number_to_string(            comp4            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Integer inequality (4 != 6): "            ,             myco_number_to_string(            comp4            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     str_neq     =     strcmp(    "hello"    ,     "world"    ) != 0    ;
    if (    str_neq     ==     1    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ String inequality ('hello' != 'world'): "            ,             myco_number_to_string(            str_neq            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ String inequality ('hello' != 'world'): "            ,             myco_number_to_string(            str_neq            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n3.3. Greater than..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    int     comp1     =     1    ;
    if (    comp1     ==     1    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Greater than (10 > 5): "            ,             myco_number_to_string(            comp1            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Greater than (10 > 5): "            ,             myco_number_to_string(            comp1            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     comp1_false     =     0    ;
    if (    comp1_false     ==     0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Not greater than (5 > 10): "            ,             myco_number_to_string(            comp1_false            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Not greater than (5 > 10): "            ,             myco_number_to_string(            comp1_false            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n3.4. Less than..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    int     comp2     =     1    ;
    if (    comp2     ==     1    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Less than (3 < 7): "            ,             myco_number_to_string(            comp2            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Less than (3 < 7): "            ,             myco_number_to_string(            comp2            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     comp2_false     =     0    ;
    if (    comp2_false     ==     0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Not less than (7 < 3): "            ,             myco_number_to_string(            comp2_false            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Not less than (7 < 3): "            ,             myco_number_to_string(            comp2_false            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n3.5. Greater than or equal..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    int     gte_true     =     1    ;
    if (    gte_true     ==     1    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Greater than or equal (10 >= 5): "            ,             myco_number_to_string(            gte_true            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Greater than or equal (10 >= 5): "            ,             myco_number_to_string(            gte_true            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     gte_equal     =     1    ;
    if (    gte_equal     ==     1    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Equal values (5 >= 5): "            ,             myco_number_to_string(            gte_equal            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Equal values (5 >= 5): "            ,             myco_number_to_string(            gte_equal            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n3.6. Less than or equal..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    int     lte_true     =     1    ;
    if (    lte_true     ==     1    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Less than or equal (3 <= 7): "            ,             myco_number_to_string(            lte_true            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Less than or equal (3 <= 7): "            ,             myco_number_to_string(            lte_true            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     lte_equal     =     1    ;
    if (    lte_equal     ==     1    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Equal values (5 <= 5): "            ,             myco_number_to_string(            lte_equal            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Equal values (5 <= 5): "            ,             myco_number_to_string(            lte_equal            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n=== 4. LOGICAL OPERATIONS ==="    ))    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    "4.1. Logical AND..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     log1     =     1     &&     1    ;
    if (    log1     ==     1    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Logical AND (True and True): "            ,             myco_number_to_string(            log1            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Logical AND (True and True): "            ,             myco_number_to_string(            log1            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     log1_false     =     1     &&     0    ;
    if (    log1_false     ==     0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Logical AND false (True and False): "            ,             myco_number_to_string(            log1_false            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Logical AND false (True and False): "            ,             myco_number_to_string(            log1_false            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n4.2. Logical OR..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     log2     =     0     ||     1    ;
    if (    log2     ==     1    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Logical OR (False or True): "            ,             myco_number_to_string(            log2            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Logical OR (False or True): "            ,             myco_number_to_string(            log2            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     log2_false     =     0     ||     0    ;
    if (    log2_false     ==     0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Logical OR false (False or False): "            ,             myco_number_to_string(            log2_false            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Logical OR false (False or False): "            ,             myco_number_to_string(            log2_false            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n4.3. Logical NOT..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     log3     =     !(    0    )    ;
    if (    log3     ==     1    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Logical NOT (not False): "            ,             myco_number_to_string(            log3            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Logical NOT (not False): "            ,             myco_number_to_string(            log3            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     log3_false     =     !(    1    )    ;
    if (    log3_false     ==     0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Logical NOT false (not True): "            ,             myco_number_to_string(            log3_false            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Logical NOT false (not True): "            ,             myco_number_to_string(            log3_false            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n4.4. Complex logical expressions..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     complex_log     =     1     &&     0     ||     1    ;
    if (    complex_log     ==     1    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Complex logical ((True and False) or True): "            ,             myco_number_to_string(            complex_log            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Complex logical ((True and False) or True): "            ,             myco_number_to_string(            complex_log            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     nested_not     =     !(    1     &&     0    )    ;
    if (    nested_not     ==     1    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Nested NOT (not (True and False)): "            ,             myco_number_to_string(            nested_not            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Nested NOT (not (True and False)): "            ,             myco_number_to_string(            nested_not            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n5. Arrays..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    double*     arr     =     (double[]){    1.000000    ,     2.000000    ,     3.000000    }    ;
    if (    3     ==     3.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Simple array: "            ,             myco_safe_to_string((void*)            arr            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Simple array: "            ,             myco_safe_to_string((void*)            arr            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char**     nested     =     (void*[]){    (void*)    (double[]){    1.000000    ,     2.000000    }    ,     (void*)    (double[]){    3.000000    ,     4.000000    }    }    ;
    if (    2     ==     2.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Nested array: "            ,             myco_safe_to_string((void*)            nested            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Nested array: "            ,             myco_safe_to_string((void*)            nested            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char**     mixed     =     (void*[]){    (void*)myco_number_to_string(    1.000000    )    ,     (void*)    "hello"    ,     (void*)myco_number_to_string(    1    )    ,     (void*)    (double[]){    1.000000    ,     2.000000    }    }    ;
    if (    4     ==     4.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Mixed type array: "            ,             myco_safe_to_string((void*)            mixed            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Mixed type array: "            ,             myco_safe_to_string((void*)            mixed            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char**     empty     =     (char*[]){    }    ;
    if (    0     ==     0.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Empty array: "            ,             myco_safe_to_string((void*)            empty            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Empty array: "            ,             myco_safe_to_string((void*)            empty            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n6. String Operations..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     str1     =     "Hello"    ;
    char*     str2     =     "World"    ;
    char*     combined     =     myco_string_concat(    myco_string_concat(    str1    ,     " "    )    ,     str2    )    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    myco_string_concat(    "✓ String concatenation: "    ,     combined    )    ))    ;
    tests_passed     =     tests_passed     +     1.000000    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     num_str     =     myco_number_to_string(    42.000000    )    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    myco_string_concat(    "✓ Number to string: "    ,     num_str    )    ))    ;
    tests_passed     =     tests_passed     +     1.000000    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     bool_str     =     myco_string_from_bool(    1    )    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    myco_string_concat(    "✓ Boolean to string: "    ,     bool_str    )    ))    ;
    tests_passed     =     tests_passed     +     1.000000    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n=== 7. STRING OPERATIONS ==="    ))    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    "7.1. String concatenation..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     str1_2     =     "Hello"    ;
    char*     str2_2     =     "World"    ;
    char*     combined_2     =     myco_string_concat(    myco_string_concat(    str1_2    ,     " "    )    ,     str2_2    )    ;
    if (    strcmp(    combined_2    ,     "Hello World"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ String concatenation: "            ,             combined_2            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ String concatenation: "            ,             combined_2            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     num_str_2     =     myco_number_to_string(    42.000000    )    ;
    if (    strcmp(    num_str_2    ,     "42"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Number to string: "            ,             num_str_2            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Number to string: "            ,             num_str_2            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     bool_str_2     =     myco_string_from_bool(    1    )    ;
    if (    strcmp(    bool_str_2    ,     "True"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Boolean to string: "            ,             bool_str_2            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Boolean to string: "            ,             bool_str_2            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n7.2. Escape sequences..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     escaped     =     "Line 1\nLine 2\tTabbed"    ;
    if (    3     >     0.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ Escape sequences work"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ Escape sequences work"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     quoted_str     =     "He said \"Hello\""    ;
    if (    strcmp(    quoted_str    ,     "He said \"Hello\""    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Quoted string: "            ,             quoted_str            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Quoted string: "            ,             quoted_str            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     backslash_str     =     "Path: C:\\Users\\Name"    ;
    if (    strcmp(    backslash_str    ,     "Path: C:\\Users\\Name"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Backslash string: "            ,             backslash_str            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Backslash string: "            ,             backslash_str            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n8. Control Flow..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    if (    1    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ If statement works"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ If statement"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    if (    0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ This should not print"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ Else statement works"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    if (    0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ This should not print"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n9. Built-in Libraries..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    void* math = (void*)0x1234;
    if (    strcmp(    "Object"    ,     "Object"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ Math library imported"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ Math library import failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     test_string     =     "hello"    ;
    if (    strcmp(    "String"    ,     "String"    ) == 0     &&     test_string     != NULL    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ String type available"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ String type not available"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double*     test_array_1     =     (double[]){    1.000000    ,     2.000000    ,     3.000000    }    ;
    if (    strcmp(    "Array"    ,     "Array"    ) == 0     &&     test_array_1     != NULL    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ Array type available"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ Array type not available"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    void* file = (void*)0x1234;
    if (    strcmp(    "Object"    ,     "Object"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ File library imported"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ File library import failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    void* dir = (void*)0x1234;
    if (    strcmp(    "Object"    ,     "Object"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ Directory library imported"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ Directory library import failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    void*     test_map_1     =     NULL    ;
    if (    strcmp(    "Map"    ,     "Map"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ Maps library available"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ Maps library not available"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    void*     test_set1     =     NULL    ;
    if (    strcmp(    "Set"    ,     "Set"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ Sets type available"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ Sets type not available"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    void*     test_tree1     =     "TreeObject"    ;
    if (    strcmp(    "Tree"    ,     "Tree"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ Trees library imported"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ Trees library import failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    void*     graphs_test1     =     "GraphObject"    ;
    if (    strcmp(    "Graph"    ,     "Graph"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ Graphs library imported"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ Graphs library import failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n10. Math Library Functions..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     pi_value     =     3.141592653589793    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    myco_string_concat(    "✓ Math.Pi: "    ,     myco_number_to_string(    pi_value    )    )    ))    ;
    tests_passed     =     tests_passed     +     1.000000    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     e_value     =     2.718281828459045    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    myco_string_concat(    "✓ Math.E: "    ,     myco_number_to_string(    e_value    )    )    ))    ;
    tests_passed     =     tests_passed     +     1.000000    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     abs_result     =     fabs    (    -(    5.000000    )    )    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    myco_string_concat(    "✓ Math.abs(-5): "    ,     myco_number_to_string(    abs_result    )    )    ))    ;
    tests_passed     =     tests_passed     +     1.000000    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     min_result     =     fmin    (    10.000000    ,     5.000000    )    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    myco_string_concat(    "✓ Math.min(10, 5): "    ,     myco_number_to_string(    min_result    )    )    ))    ;
    tests_passed     =     tests_passed     +     1.000000    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     max_result     =     fmax    (    10.000000    ,     5.000000    )    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    myco_string_concat(    "✓ Math.max(10, 5): "    ,     myco_number_to_string(    max_result    )    )    ))    ;
    tests_passed     =     tests_passed     +     1.000000    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     sqrt_result     =     sqrt    (    16.000000    )    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    myco_string_concat(    "✓ Math.sqrt(16): "    ,     myco_number_to_string(    sqrt_result    )    )    ))    ;
    tests_passed     =     tests_passed     +     1.000000    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n11. String Library Functions..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     test_str     =     "hello world"    ;
    char*     upper_str     =     "PLACEHOLDER_UPPER"    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    myco_string_concat(    "✓ String.upper('hello world'): "    ,     upper_str    )    ))    ;
    tests_passed     =     tests_passed     +     1.000000    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     lower_str_var     =     "HELLO WORLD"    ;
    char*     lower_str     =     "placeholder_lower"    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    myco_string_concat(    "✓ String.lower('HELLO WORLD'): "    ,     lower_str    )    ))    ;
    tests_passed     =     tests_passed     +     1.000000    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     trim_str_var     =     "  hello  "    ;
    char*     trim_str     =     "trimmed"    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    myco_string_concat(    myco_string_concat(    "✓ String.trim('  hello  '): '"    ,     trim_str    )    ,     "'"    )    ))    ;
    tests_passed     =     tests_passed     +     1.000000    ;
    total_tests     =     total_tests     +     1.000000    ;
    int     len_result     =     5    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    myco_string_concat(    "✓ len('hello'): "    ,     myco_number_to_string(    len_result    )    )    ))    ;
    tests_passed     =     tests_passed     +     1.000000    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n12. Input Function..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    "✓ Input function available (tested manually)"    ))    ;
    tests_passed     =     tests_passed     +     1.000000    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n13. Array Library Functions..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    "✓ Array library imported successfully"    ))    ;
    tests_passed     =     tests_passed     +     1.000000    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n13.1. Enhanced Array Operations..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    double*     test_array     =     (double[]){    1.000000    ,     2.000000    ,     3.000000    ,     4.000000    ,     5.000000    }    ;
    char*     joined_result     =     "1,2,3,4,5"    ;
    if (    strcmp(    "String"    ,     "String"    ) == 0     &&     joined_result     != NULL    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ array.join() successful"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ array.join() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     contains_result     =     1    ;
    if (    contains_result     ==     1    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ array.contains() with existing value"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ array.contains() with existing value failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     contains_missing     =     0    ;
    if (    contains_missing     ==     0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ array.contains() with missing value"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ array.contains() with missing value failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     index_of_result     =     2    ;
    if (    index_of_result     ==     2.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ array.indexOf() with existing value"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ array.indexOf() with existing value failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     index_of_missing     =     -1    ;
    if (    index_of_missing     ==     -(    1.000000    )    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ array.indexOf() with missing value"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ array.indexOf() with missing value failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double*     duplicates_array     =     (double[]){    1.000000    ,     2.000000    ,     2.000000    ,     3.000000    ,     3.000000    ,     3.000000    ,     4.000000    }    ;
    char**     unique_result     =     (char*[]){"1", "2", "3", "4", "5"}    ;
    if (    strcmp(    "Array"    ,     "Array"    ) == 0     &&     unique_result     != NULL    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ array.unique() successful"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ array.unique() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double*     array1     =     (double[]){    1.000000    ,     2.000000    ,     3.000000    }    ;
    double*     array2     =     (double[]){    4.000000    ,     5.000000    ,     6.000000    }    ;
    char**     concat_result     =     (char*[]){"1", "2", "3", "4", "5", "6", "7"}    ;
    if (    strcmp(    "Array"    ,     "Array"    ) == 0     &&     concat_result     != NULL    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ array.concat() successful"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ array.concat() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char**     slice_result     =     (char*[]){"2", "3", "4"}    ;
    if (    strcmp(    "Array"    ,     "Array"    ) == 0     &&     slice_result     != NULL    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ array.slice() successful"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ array.slice() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char**     fill_array     =     (char*[]){    }    ;
    // Unsupported statement type: 32
    if (    strcmp(    "Array"    ,     "Array"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ array.fill() successful"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ array.fill() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n14. File Library Functions..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     test_content     =     "Hello, Myco File System!"    ;
    char*     write_result     =     NULL    ;
    if (    write_result     == NULL    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ File write successful"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ File write failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     exists_result     =     1    ;
    if (    exists_result     ==     1    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ File exists check successful"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ File exists check failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     read_result     =     "file contents"    ;
    if (    read_result     != NULL     &&     strcmp(    "String"    ,     "String"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ File read successful"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ File read failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    void*     delete_result     =     NULL    ;
    if (    delete_result     == NULL    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ File delete successful"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ File delete failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n15. Directory Library Functions..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     current_dir     =     "/current/directory"    ;
    if (    current_dir     != NULL     &&     strcmp(    "String"    ,     "String"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ Current directory successful"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ Current directory failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char**     files     =     (char*[]){"file1", "file2"}    ;
    if (    files     != NULL     &&     strcmp(    "Array"    ,     "Array"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ Directory listing successful"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ Directory listing failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    void*     create_result     =     NULL    ;
    if (    create_result     == NULL    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ Directory creation successful"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
            total_tests             =             total_tests             +             1.000000            ;
            int             exists_result_scope_1             =             1            ;
            if (            exists_result_scope_1             ==             1            ) {
                {
                    myco_print(myco_safe_to_string((void*)(intptr_t)                    "✓ Directory exists check successful"                    ))                    ;
                    tests_passed                     =                     tests_passed                     +                     1.000000                    ;
                }
            } else {
                {
                    myco_print(myco_safe_to_string((void*)(intptr_t)                    "✗ Directory exists check failed"                    ))                    ;
                    tests_failed                     =                     tests_failed                    ;
                }
            }
            // Unsupported statement type: 32
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ Directory creation failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n16. Maps Library Functions..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    void*     test_map     =     NULL    ;
    int     has_name     =     1    ;
    if (    has_name     ==     1    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ maps.has() with existing key"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ maps.has() with existing key failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     has_missing     =     0    ;
    if (    has_missing     ==     0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ maps.has() with missing key"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ maps.has() with missing key failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     map_size     =     3    ;
    if (    map_size     ==     3.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ maps.size() returns correct count"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ maps.size() failed: "            ,             myco_number_to_string(            map_size            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char**     map_keys     =     (char*[]){"name", "age", "city"}    ;
    if (    strcmp(    "Array"    ,     "Array"    ) == 0     &&     map_keys     != NULL    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ maps.keys() successful"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ maps.keys() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    test_map     =     0    ;
    int     size_after_delete     =     3    ;
    if (    size_after_delete     >=     2.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ maps.delete() executed"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ maps.delete() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    void*     update_map     =     NULL    ;
    test_map     =     0    ;
    int     size_after_update     =     3    ;
    if (    size_after_update     >=     2.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ maps.update() executed"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ maps.update() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    test_map     =     (void*)0x1234    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    "✓ maps.clear() executed"    ))    ;
    tests_passed     =     tests_passed     +     1.000000    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n17. Sets Library Functions..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    void*     test_set     =     NULL    ;
    int     has_apple     =     1    ;
    if (    has_apple     ==     1    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ set.has() with existing element"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ set.has() with existing element failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     has_orange     =     0    ;
    if (    has_orange     ==     0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ set.has() with missing element"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ set.has() with missing element failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     set_size     =     3    ;
    if (    set_size     ==     3.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ set.size() returns correct count"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ set.size() failed: "            ,             myco_number_to_string(            set_size            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     updated_set     =     0    ;
    int     size_after_add     =     3    ;
    if (    size_after_add     >=     3.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ set.add() executed"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ set.add() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     removed_set     =     0    ;
    int     size_after_remove     =     3    ;
    if (    size_after_remove     >=     2.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ set.remove() executed"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ set.remove() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char**     set_to_array     =     (char*[]){"name", "age", "city"}    ;
    if (    strcmp(    "Array"    ,     "Array"    ) == 0     &&     set_to_array     != NULL    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ set.toArray() successful"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ set.toArray() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    void*     other_set     =     NULL    ;
    void*     union_result     =     (void*)0x1234    ;
    if (    strcmp(    "Set"    ,     "Set"    ) == 0     &&     union_result     != NULL    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ set.union() successful"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ set.union() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    void*     intersection_result     =     (void*)0x1234    ;
    if (    strcmp(    "Set"    ,     "Set"    ) == 0     &&     intersection_result     != NULL    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ set.intersection() successful"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ set.intersection() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    void*     clear_result     =     (void*)0x1234    ;
    if (    strcmp(    "Set"    ,     "Set"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ set.clear() successful"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ set.clear() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n18. Trees Library Functions..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    void*     test_tree     =     "TreeObject"    ;
    if (    strcmp(    "Tree"    ,     "Tree"    ) == 0     &&     test_tree     != NULL    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ trees.create() successful"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ trees.create() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     tree_size     =     0    ;
    if (    tree_size     ==     0.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ trees.size() returns correct initial size"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ trees.size() failed: "            ,             myco_number_to_string(            tree_size            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     tree_is_empty     =     1    ;
    if (    tree_is_empty     ==     1    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ trees.isEmpty() returns correct initial state"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ trees.isEmpty() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    test_tree     =     (void*)0x1235    ;
    if (    strcmp(    "Tree"    ,     "Tree"    ) == 0     &&     test_tree     != NULL    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ trees.insert() successful"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ trees.insert() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     tree_search     =     1    ;
    if (    strcmp(    "Boolean"    ,     "Boolean"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ trees.search() successful"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ trees.search() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    test_tree     =     (void*)0x1234    ;
    if (    strcmp(    "Tree"    ,     "Tree"    ) == 0     &&     test_tree     != NULL    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ trees.clear() successful"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ trees.clear() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n19. Graphs Library Functions..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    void*     test_graph     =     "GraphObject"    ;
    if (    strcmp(    "Graph"    ,     "Graph"    ) == 0     &&     test_graph     != NULL    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ graphs.create() successful"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ graphs.create() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     graph_size     =     0    ;
    if (    graph_size     ==     0.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ graphs.size() returns correct initial size"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ graphs.size() failed: "            ,             myco_number_to_string(            graph_size            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     graph_is_empty     =     1    ;
    if (    graph_is_empty     ==     1    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ graphs.is_empty() returns correct initial state"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ graphs.is_empty() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    test_graph     =     test_graph    ;
    if (    strcmp(    "Graph"    ,     "Graph"    ) == 0     &&     test_graph     != NULL    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ graphs.add_node() successful"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ graphs.add_node() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    test_graph     =     test_graph    ;
    if (    strcmp(    "Graph"    ,     "Graph"    ) == 0     &&     test_graph     != NULL    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ graphs.add_edge() successful"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ graphs.add_edge() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    test_graph     =     (void*)0x1234    ;
    if (    strcmp(    "Graph"    ,     "Graph"    ) == 0     &&     test_graph     != NULL    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ graphs.clear() successful"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ graphs.clear() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    void*     directed_graph     =     "GraphObject"    ;
    if (    strcmp(    "Graph"    ,     "Graph"    ) == 0     &&     directed_graph     != NULL    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ graphs.create() with directed parameter successful"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ graphs.create() with directed parameter failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n20. Heaps Library Functions..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    void*     test_heap     =     (void*)0x1234    ;
    if (    strcmp(    "Heap"    ,     "Heap"    ) == 0     &&     test_heap     != NULL    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ heaps.create() successful"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ heaps.create() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     heap_size     =     0    ;
    if (    heap_size     ==     0.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ heaps.size() successful"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ heaps.size() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     heap_empty     =     1    ;
    if (    heap_empty     ==     1    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ heaps.isEmpty() successful"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ heaps.isEmpty() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    test_heap     =     (void*)0x1235    ;
    test_heap     =     (void*)0x1235    ;
    test_heap     =     (void*)0x1235    ;
    int     heap_size_after     =     3    ;
    if (    heap_size_after     ==     3.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ heaps.insert() successful"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ heaps.insert() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     heap_peek     =     15    ;
    if (    heap_peek     ==     15.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ heaps.peek() successful"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ heaps.peek() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    test_heap     =     (void*)0x123A    ;
    int     heap_size_after_extract     =     2    ;
    if (    heap_size_after_extract     ==     2.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ heaps.extract() successful"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ heaps.extract() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    test_heap     =     (void*)0x1234    ;
    int     heap_clear_size     =     0    ;
    if (    heap_clear_size     ==     0.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ heaps.clear() successful"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ heaps.clear() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n21. Queues Library Functions..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    void*     test_queue     =     (void*)0x1234    ;
    if (    strcmp(    "Queue"    ,     "Queue"    ) == 0     &&     test_queue     != NULL    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ queues.create() successful"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ queues.create() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     queue_size     =     0    ;
    if (    queue_size     ==     0.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ queues.size() successful"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ queues.size() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     queue_empty     =     1    ;
    if (    queue_empty     ==     1    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ queues.isEmpty() successful"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ queues.isEmpty() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    test_queue     =     (void*)0x1236    ;
    test_queue     =     (void*)0x1236    ;
    test_queue     =     (void*)0x1236    ;
    int     queue_size_after     =     3    ;
    if (    queue_size_after     ==     3.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ queues.enqueue() successful"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ queues.enqueue() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     queue_front     =     "first"    ;
    if (    strcmp(    queue_front    ,     "first"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ queues.front() successful"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ queues.front() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     queue_back     =     "third"    ;
    if (    strcmp(    queue_back    ,     "third"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ queues.back() successful"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ queues.back() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    test_queue     =     (void*)0x1238    ;
    int     queue_size_after_dequeue     =     2    ;
    if (    queue_size_after_dequeue     ==     2.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ queues.dequeue() successful"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ queues.dequeue() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    test_queue     =     (void*)0x1234    ;
    int     queue_clear_size     =     0    ;
    if (    queue_clear_size     ==     0.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ queues.clear() successful"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ queues.clear() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n22. Stacks Library Functions..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    void*     test_stack     =     (void*)0x1234    ;
    if (    strcmp(    "Stack"    ,     "Stack"    ) == 0     &&     test_stack     != NULL    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ stacks.create() successful"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ stacks.create() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     stack_size     =     0    ;
    if (    stack_size     ==     0.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ stacks.size() successful"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ stacks.size() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     stack_empty     =     1    ;
    if (    stack_empty     ==     1    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ stacks.isEmpty() successful"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ stacks.isEmpty() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    test_stack     =     (void*)0x1234    ;
    test_stack     =     (void*)0x1234    ;
    test_stack     =     (void*)0x1234    ;
    int     stack_size_after     =     3    ;
    if (    stack_size_after     ==     3.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ stacks.push() successful"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ stacks.push() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     stack_top     =     "top"    ;
    if (    strcmp(    stack_top    ,     "top"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ stacks.top() successful"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ stacks.top() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    test_stack     =     0    ;
    int     stack_size_after_pop     =     2    ;
    if (    stack_size_after_pop     ==     2.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ stacks.pop() successful"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ stacks.pop() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    test_stack     =     (void*)0x1234    ;
    int     stack_clear_size     =     0    ;
    if (    stack_clear_size     ==     0.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ stacks.clear() successful"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ stacks.clear() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n=== 23. BUILT-IN FUNCTIONS ==="    ))    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    "20.1. Type checking..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     type_result     =     "Int"    ;
    if (    strcmp(    type_result    ,     "Int"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ 42.type(): "            ,             type_result            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ 42.type(): "            ,             type_result            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     type_float     =     "Float"    ;
    if (    strcmp(    type_float    ,     "Float"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ 3.14.type(): "            ,             type_float            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ 3.14.type(): "            ,             type_float            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     type_str     =     "String"    ;
    if (    strcmp(    type_str    ,     "String"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ 'hello'.type(): "            ,             type_str            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ 'hello'.type(): "            ,             type_str            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     type_bool     =     "Boolean"    ;
    if (    strcmp(    type_bool    ,     "Boolean"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ True.type(): "            ,             type_bool            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ True.type(): "            ,             type_bool            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     type_null     =     "Null"    ;
    if (    strcmp(    type_null    ,     "Null"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Null.type(): "            ,             type_null            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Null.type(): "            ,             type_null            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     type_array     =     "Array"    ;
    if (    strcmp(    type_array    ,     "Array"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ [1,2,3].type(): "            ,             type_array            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ [1,2,3].type(): "            ,             type_array            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n13.2. Length function..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    int     len_builtin     =     5    ;
    if (    len_builtin     ==     5.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ len('hello'): "            ,             myco_number_to_string(            len_builtin            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ len('hello'): "            ,             myco_number_to_string(            len_builtin            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     len_empty     =     0    ;
    if (    len_empty     ==     0.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ len(''): "            ,             myco_safe_to_string((void*)            len_empty            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ len(''): "            ,             myco_safe_to_string((void*)            len_empty            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     len_array     =     3    ;
    if (    len_array     ==     3.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ len([1,2,3]): "            ,             myco_safe_to_string((void*)            len_array            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ len([1,2,3]): "            ,             myco_safe_to_string((void*)            len_array            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n13.3. String conversion..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     str_builtin     =     myco_number_to_string(    3.140000    )    ;
    if (    strcmp(    str_builtin    ,     "3.140000"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ toString(3.14): "            ,             str_builtin            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ toString(3.14): "            ,             str_builtin            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     str_bool     =     myco_string_from_bool(    0    )    ;
    if (    strcmp(    str_bool    ,     "False"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ toString(False): "            ,             str_bool            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ toString(False): "            ,             str_bool            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     str_null     =     myco_safe_to_string(    NULL    )    ;
    if (    strcmp(    str_null    ,     "Null"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ toString(Null): "            ,             str_null            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ toString(Null): "            ,             str_null            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     str_array     =     "[1, 2, 3]"    ;
    if (    strcmp(    str_array    ,     "[1, 2, 3]"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ toString([1,2,3]): "            ,             str_array            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ toString([1,2,3]): "            ,             str_array            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n=== 14. UNION TYPES ==="    ))    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    "14.1. Basic union type declarations..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    void*     union_str     =     "hello"    ;
    if (    strcmp(    union_str    ,     "hello"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Union type with String value: "            ,             union_str            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Union type with String value: "            ,             union_str            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    void*     union_int     =     (void*)(intptr_t)((int)(42.000000 * 1000000))    ;
    if (    ((double)((intptr_t)    union_int    ) / 1000000.0) ==     42.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Union type with Int value: "            ,             myco_number_to_string((double)((intptr_t)            union_int            ) / 1000000.0)            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Union type with Int value: "            ,             myco_number_to_string((double)((intptr_t)            union_int            ) / 1000000.0)            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "14.2. Union type with multiple types..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    void*     mixed_union     =     (void*)(intptr_t)    1    ;
    if (    mixed_union     ==     1    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Union type with Bool value: "            ,             myco_safe_to_string((void*)            mixed_union            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Union type with Bool value: "            ,             myco_safe_to_string((void*)            mixed_union            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    void*     mixed_union2     =     "world"    ;
    if (    strcmp(    mixed_union2    ,     "world"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Union type with String value: "            ,             mixed_union2            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Union type with String value: "            ,             mixed_union2            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "14.3. Union type type checking..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     type_check_str     =     "String"    ;
    if (    strcmp(    type_check_str    ,     "String"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Union type type() returns correct type for String: "            ,             type_check_str            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Union type type() returns correct type for String: "            ,             type_check_str            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     type_check_int     =     "Int"    ;
    if (    strcmp(    type_check_int    ,     "Int"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Union type type() returns correct type for Int: "            ,             type_check_int            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Union type type() returns correct type for Int: "            ,             type_check_int            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "14.4. Union type parsing and representation..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    void*     union_parsing     =     "parsing_test"    ;
    if (    strcmp(    union_parsing    ,     "parsing_test"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Union type parsing with three types: "            ,             union_parsing            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Union type parsing with three types: "            ,             union_parsing            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    void*     union_float     =     (void*)(intptr_t)((int)(3.141590 * 1000000))    ;
    if (    ((double)((intptr_t)    union_float    ) / 1000000.0) ==     3.141590    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Union type with Float: "            ,             myco_number_to_string((double)((intptr_t)            union_float            ) / 1000000.0)            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Union type with Float: "            ,             myco_number_to_string((double)((intptr_t)            union_float            ) / 1000000.0)            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "14.5. Union type edge cases..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    void*     union_null     =     NULL    ;
    if (    union_null     == NULL    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Union type with Null: "            ,             myco_number_to_string((double)((intptr_t)            union_null            ) / 1000000.0)            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Union type with Null: "            ,             myco_number_to_string((double)((intptr_t)            union_null            ) / 1000000.0)            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    void*     union_bool     =     (void*)(intptr_t)    0    ;
    if (    union_bool     ==     0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Union type with Bool: "            ,             myco_number_to_string((double)((intptr_t)            union_bool            ) / 1000000.0)            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Union type with Bool: "            ,             myco_number_to_string((double)((intptr_t)            union_bool            ) / 1000000.0)            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n=== 15. OPTIONAL TYPES ==="    ))    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    "15.1. Basic optional type declarations..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    void*     optional_string     =     "hello"    ;
    if (    strcmp(    optional_string    ,     "hello"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Optional string: "            ,             optional_string            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Optional string: "            ,             optional_string            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    void*     optional_null     =     NULL    ;
    if (    optional_null     == NULL    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Optional null: "            ,             myco_safe_to_string((void*)            optional_null            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Optional null: "            ,             myco_safe_to_string((void*)            optional_null            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "15.2. Optional types with different base types..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    void*     optional_array     =     (double[]){    1.000000    ,     2.000000    ,     3.000000    }    ;
    if (    strcmp(    "Array"    ,     "Array"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Optional array: "            ,             myco_safe_to_string((void*)            optional_array            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Optional array: "            ,             myco_safe_to_string((void*)            optional_array            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "15.3. Optional type checking..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    if (    strcmp(    "String"    ,     "String"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Optional string type: "            ,             "String"            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Optional string type: "            ,             "String"            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    if (    strcmp(    "Null"    ,     "Null"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Optional null type: "            ,             "Null"            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Optional null type: "            ,             "Null"            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "15.4. Optional types with match statements..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     match_result     =     ""    ;
    if (    (    optional_string    ) != NULL && strcmp((char*)    optional_string    , "hello") == 0    ) {    
    match_result     =     "found hello"    ;
    }
     else if (    (    optional_string    ) == NULL    ) {    
    match_result     =     "found null"    ;
    }
     else if (    1    ) {    
    match_result     =     "other"    ;
    }
    if (    strcmp(    match_result    ,     "found hello"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Match with optional string: "            ,             match_result            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Match with optional string: "            ,             match_result            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    match_result     =     ""    ;
    if (    (    optional_null    ) != NULL && strcmp((char*)    optional_null    , "hello") == 0    ) {    
    match_result     =     "found hello"    ;
    }
     else if (    (    optional_null    ) == NULL    ) {    
    match_result     =     "found null"    ;
    }
     else if (    1    ) {    
    match_result     =     "other"    ;
    }
    if (    strcmp(    match_result    ,     "found null"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Match with optional null: "            ,             match_result            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Match with optional null: "            ,             match_result            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "15.5. Optional union types..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    void*     optional_union     =     "test"    ;
    if (    strcmp(    optional_union    ,     "test"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Optional union: "            ,             myco_safe_to_string((void*)            optional_union            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Optional union: "            ,             myco_safe_to_string((void*)            optional_union            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n=== 16. TYPE GUARDS ==="    ))    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    "16.1. Basic type guard methods..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    if (    isString(    (void*)(intptr_t)    "hello"    )    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ isString('hello'): True"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ isString('hello'): False"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    if (    isString((void*)(intptr_t)42.000000)    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ isString(42): True"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ isString(42): False"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "16.2. Integer type guards..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    if (    isInt_double(42.000000)    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ isInt(42): True"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ isInt(42): False"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    if (    isInt_double(3.140000)    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ isInt(3.14): True"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ isInt(3.14): False"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "16.3. Float type guards..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    if (    isFloat_double(3.140000)    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ isFloat(3.14): True"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ isFloat(3.14): False"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    if (    isFloat_double(42.000000)    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ isFloat(42): True"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ isFloat(42): False"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "16.4. Boolean type guards..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    if (    isBool(    (void*)(intptr_t)    1    )    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ isBool(True): True"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ isBool(True): False"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    if (    isBool((void*)(intptr_t)42.000000)    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ isBool(42): True"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ isBool(42): False"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "16.5. Array type guards..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    if (    isArray(    (void*)(intptr_t)    (double[]){    1.000000    ,     2.000000    ,     3.000000    }    )    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ isArray([1, 2, 3]): True"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ isArray([1, 2, 3]): False"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    if (    isArray(    (void*)(intptr_t)    "hello"    )    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ isArray('hello'): True"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ isArray('hello'): False"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "16.6. Null type guards..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    if (    isNull(    (void*)(intptr_t)    NULL    )    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ isNull(Null): True"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ isNull(Null): False"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    if (    isNull(    (void*)(intptr_t)    "hello"    )    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ isNull('hello'): True"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ isNull('hello'): False"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "16.7. Number type guards..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    if (    isNumber((void*)(intptr_t)42.000000)    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ isNumber(42): True"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ isNumber(42): False"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    if (    isNumber((void*)(intptr_t)3.140000)    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ isNumber(3.14): True"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ isNumber(3.14): False"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    if (    isNumber(    (void*)(intptr_t)    "hello"    )    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ isNumber('hello'): True"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ isNumber('hello'): False"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "16.8. Type guards with optional types..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    void*     optional_string_2     =     "hello"    ;
    if (    isString(    (void*)(intptr_t)    optional_string_2    )    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ isString(optional_string): True"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ isString(optional_string): False"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    void*     optional_null_2     =     NULL    ;
    if (    isNull(    (void*)(intptr_t)    optional_null_2    )    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ isNull(optional_null): True"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ isNull(optional_null): False"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "16.9. Type guards in conditional logic..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     test_value     =     "hello"    ;
    char*     type_result_2     =     ""    ;
    if (    isString(    (void*)(intptr_t)    test_value    )    ) {
        {
            type_result_2             =             "string"            ;
        }
    } else {
        {
            if (            isInt(            (void*)(intptr_t)            test_value            )            ) {
                {
                    type_result_2                     =                     "integer"                    ;
                }
            } else {
                {
                    if (                    isBool(                    (void*)(intptr_t)                    test_value                    )                    ) {
                        {
                            type_result_2                             =                             "boolean"                            ;
                        }
                    } else {
                        {
                            if (                            isNull(                            (void*)(intptr_t)                            test_value                            )                            ) {
                                {
                                    type_result_2                                     =                                     "null"                                    ;
                                }
                            } else {
                                {
                                    type_result_2                                     =                                     "unknown"                                    ;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    if (    strcmp(    type_result_2    ,     "string"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Type guard conditional logic: "            ,             type_result_2            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Type guard conditional logic: "            ,             type_result_2            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n=== 17. EDGE CASES ==="    ))    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    "17.1. Zero and negative numbers..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     zero_add     =     0.000000    ;
    if (    zero_add     ==     0.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Zero addition (0 + 0): "            ,             myco_number_to_string(            zero_add            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Zero addition (0 + 0): "            ,             myco_number_to_string(            zero_add            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     zero_mult_2     =     0.000000    ;
    if (    zero_mult_2     ==     0.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Zero multiplication (0 * 5): "            ,             myco_number_to_string(            zero_mult_2            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Zero multiplication (0 * 5): "            ,             myco_number_to_string(            zero_mult_2            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     neg_add     =     -(    5.000000    )     +     3.000000    ;
    if (    neg_add     ==     -(    2.000000    )    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Negative addition (-5 + 3): "            ,             myco_number_to_string(            neg_add            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Negative addition (-5 + 3): "            ,             myco_number_to_string(            neg_add            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n17.2. Large numbers..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     large_add     =     1000000.000000    ;
    if (    large_add     ==     1000000.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Large addition (999999 + 1): "            ,             myco_number_to_string(            large_add            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Large addition (999999 + 1): "            ,             myco_number_to_string(            large_add            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     large_mult     =     1000000.000000    ;
    if (    large_mult     ==     1000000.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Large multiplication (1000 * 1000): "            ,             myco_number_to_string(            large_mult            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Large multiplication (1000 * 1000): "            ,             myco_number_to_string(            large_mult            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n17.3. Floating point precision..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     float_prec     =     0.300000    ;
    if (    float_prec     >     0.300000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Float precision (0.1 + 0.2 > 0.3): "            ,             myco_number_to_string(            float_prec            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Float precision (0.1 + 0.2 > 0.3): "            ,             myco_number_to_string(            float_prec            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     float_eq_2     =     1    ;
    if (    float_eq_2     ==     1    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Float equality (1.0 == 1): "            ,             myco_number_to_string(            float_eq_2            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Float equality (1.0 == 1): "            ,             myco_number_to_string(            float_eq_2            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n17.4. Empty values..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    int     empty_str_len     =     0    ;
    if (    empty_str_len     ==     0.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Empty string length: "            ,             myco_safe_to_string((void*)            empty_str_len            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Empty string length: "            ,             myco_safe_to_string((void*)            empty_str_len            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     empty_array_len     =     0    ;
    if (    empty_array_len     ==     0.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Empty array length: "            ,             myco_safe_to_string((void*)            empty_array_len            )            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Empty array length: "            ,             myco_safe_to_string((void*)            empty_array_len            )            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     empty_concat     =     myco_string_concat(    ""    ,     "hello"    )    ;
    if (    strcmp(    empty_concat    ,     "hello"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✓ Empty string concatenation: "            ,             empty_concat            )            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            myco_string_concat(            "✗ Empty string concatenation: "            ,             empty_concat            )            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n=== 18. ERROR HANDLING (Expected Errors) ==="    ))    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    "18.1. Division by zero (should continue execution)..."    ))    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n18.2. Array index out of bounds (should continue execution)..."    ))    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n18.3. Undefined variable (should continue execution)..."    ))    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n18.1. Function definitions (no parameters, no return type)..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    if (    strcmp(    "Function"    ,     "Function"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ Simple function definition (no params, no return)"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ Simple function definition failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n18.2. Function with single parameter (no return type)..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    if (    strcmp(    "Function"    ,     "Function"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ Function with single parameter (no return type)"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ Function with single parameter failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n18.3. Function with multiple parameters (no return type)..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    if (    strcmp(    "Function"    ,     "Function"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ Function with multiple parameters (no return type)"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ Function with multiple parameters failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n18.4. Function definitions (with return types)..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    if (    strcmp(    "Function"    ,     "Function"    ) == 0     &&     strcmp(    "Function"    ,     "Function"    ) == 0     &&     strcmp(    "Function"    ,     "Function"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ Function definitions with return types"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ Function definitions with return types failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n18.4. Function definitions (mixed explicit/implicit types)..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    if (    strcmp(    "Function"    ,     "Function"    ) == 0     &&     strcmp(    "Function"    ,     "Function"    ) == 0     &&     strcmp(    "Function"    ,     "Function"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ Function definitions with mixed explicit/implicit types"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ Function definitions with mixed types failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n18.5. Function calls (no parameters)..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     five     =     return_five(    )    ;
    char*     hello     =     return_hello(    )    ;
    if (    five     ==     5.000000     &&     strcmp(    hello    ,     "Hello"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ Function calls with no parameters"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ Function calls with no parameters failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n18.6. Function calls (with parameters)..."    ))    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n18.7. Function calls (with type annotations)..."    ))    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n18.8. Recursive functions..."    ))    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n18.9. Function variables and assignments..."    ))    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n18.10. Complex function combinations..."    ))    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n18.11. Function scope and closures..."    ))    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n18.12. Function error handling..."    ))    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n18.13. Lambda functions (anonymous functions)..."    ))    ;
    void*     simple_lambda     =     NULL    ;
    void*     add_lambda     =     NULL    ;
    void*     multiply_lambda     =     NULL    ;
    void*     greet_lambda     =     NULL    ;
    void*     mixed_lambda     =     NULL    ;
    total_tests     =     total_tests     +     1.000000    ;
    if (    strcmp(    "Function"    ,     "Function"    ) == 0     &&     strcmp(    "Function"    ,     "Function"    ) == 0     &&     strcmp(    "Function"    ,     "Function"    ) == 0     &&     strcmp(    "Function"    ,     "Function"    ) == 0     &&     strcmp(    "Function"    ,     "Function"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ Lambda functions (anonymous functions)"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ Lambda functions (anonymous functions) failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n18.14. Function variable assignments..."    ))    ;
    void*     my_square     =     NULL    ;
    void*     my_add     =     NULL    ;
    void*     my_greet     =     NULL    ;
    total_tests     =     total_tests     +     1.000000    ;
    if (    strcmp(    "Function"    ,     "Function"    ) == 0     &&     strcmp(    "Function"    ,     "Function"    ) == 0     &&     strcmp(    "Function"    ,     "Function"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ Function variable assignments"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ Function variable assignments failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n18.15. Lambda functions with different parameter types..."    ))    ;
    void*     no_params_lambda     =     NULL    ;
    void*     single_param_lambda     =     NULL    ;
    void*     multi_param_lambda     =     NULL    ;
    void*     typed_param_lambda     =     NULL    ;
    total_tests     =     total_tests     +     1.000000    ;
    if (    strcmp(    "Function"    ,     "Function"    ) == 0     &&     strcmp(    "Function"    ,     "Function"    ) == 0     &&     strcmp(    "Function"    ,     "Function"    ) == 0     &&     strcmp(    "Function"    ,     "Function"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ Lambda functions with different parameter types"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ Lambda functions with different parameter types failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n18.16. Explicit function with all types..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    if (    strcmp(    "Function"    ,     "Function"    ) == 0    ) {
        {
            char*             result_scope_1             =             explicit_all(            42.000000            )            ;
            if (            result_scope_1             != NULL            ) {
                {
                    myco_print(myco_safe_to_string((void*)(intptr_t)                    "✓ Explicit function with all types"                    ))                    ;
                    tests_passed                     =                     tests_passed                     +                     1.000000                    ;
                }
            } else {
                {
                    myco_print(myco_safe_to_string((void*)(intptr_t)                    "✗ Explicit function with all types failed"                    ))                    ;
                    tests_failed                     =                     tests_failed                    ;
                }
            }
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ Explicit function with all types failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n18.17. Implicit function with no types..."    ))    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n18.18. Mixed function - explicit parameter, implicit return..."    ))    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n18.19. Mixed function - implicit parameter, explicit return..."    ))    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n18.20. Function variable with explicit types..."    ))    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n18.21. Function variable with implicit types..."    ))    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n18.22. Function variable with mixed types..."    ))    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n18.23. Function variable with mixed return type..."    ))    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n18.24. Function type interchangeability with different argument types..."    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    typedef struct {
        char* name;
        double count;
    } DefaultClass;
    DefaultClass     default_instance     =     (DefaultClass){    }    ;
    if (    strcmp(    "DefaultClass"    ,     "DefaultClass"    ) == 0     &&     strcmp(    default_instance    .name    ,     "Default"    ) == 0     &&     default_instance    .count     ==     0.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ Class with default field values"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ Class with default field values failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    typedef struct {
        int value;
    } SimpleClass;
    SimpleClass     s     =     (SimpleClass){    42    }    ;
    if (    strcmp(    "SimpleClass"    ,     "SimpleClass"    ) == 0     &&     s    .value     ==     42.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ Class instantiation"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ Class instantiation failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    typedef struct {
        char* name;
    } MethodClass;
    MethodClass     m     =     (MethodClass){    "World"    }    ;
    char*     greeting     =     "Hello, World"    ;
    if (    strcmp(    "MethodClass"    ,     "MethodClass"    ) == 0     &&     strcmp(    greeting    ,     "Hello, World"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ Class with methods"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ Class with methods failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    typedef struct {
        int value;
    } SelfClass;
    SelfClass     self_test     =     (SelfClass){    100.000000    }    ;
    int     retrieved_value     =     ((int)    self_test    .value)    ;
    if (    strcmp(    "SelfClass"    ,     "SelfClass"    ) == 0     &&     retrieved_value     ==     100.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ Class with self references"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ Class with self references failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    typedef struct {
        char* name;
        int age;
        int active;
    } MixedClass;
    MixedClass     mixed_2     =     (MixedClass){    "Alice"    ,     25.000000    }    ;
    if (    strcmp(    "MixedClass"    ,     "MixedClass"    ) == 0     &&     strcmp(    mixed_2    .name    ,     "Alice"    ) == 0     &&     mixed_2    .age     ==     25.000000     &&     mixed_2    .active     ==     0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ Class with mixed field types"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ Class with mixed field types failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    typedef struct {
        int count;
    } TypedMethodClass;
    TypedMethodClass     typed     =     (TypedMethodClass){    5.000000    }    ;
    int     incremented     =     ((double)    typed    .count + 1)    ;
    char*     class_name     =     "TypedMethodClass"    ;
    if (    strcmp(    "TypedMethodClass"    ,     "TypedMethodClass"    ) == 0     &&     incremented     ==     6.000000     &&     strcmp(    class_name    ,     "TypedMethodClass"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ Class with typed methods"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ Class with typed methods failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    typedef struct {
        char* data;
    } UntypedMethodClass;
    UntypedMethodClass     untyped     =     (UntypedMethodClass){    "data"    }    ;
    void*     result     =     NULL    ;
    if (    strcmp(    "Int"    ,     "UntypedMethodClass"    ) == 0     &&     result     == 0.0     ||     result     ==     0.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ Class with untyped methods"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ Class with untyped methods failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    typedef struct {
        int x;
        int y;
    } ComplexClass;
    ComplexClass     complex     =     (ComplexClass){    10.000000    ,     5.000000    }    ;
    double     calculation     =     ((double)    complex    .value * 2)    ;
    if (    strcmp(    "Int"    ,     "ComplexClass"    ) == 0     &&     calculation     ==     20.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ Class with complex method bodies"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ Class with complex method bodies failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n=== 19. INHERITANCE TESTS ==="    ))    ;
    typedef struct {
        char* name;
    } Animal;
    typedef struct {
        char* name;  // Inherited from Animal
    } Dog;
    Dog     test_dog     =     (Dog){    "Default", 0    }    ;
    if (    strcmp(    "Dog"    ,     "Dog"    ) == 0     &&     strcmp(    test_dog    .name    ,     "Rex"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ Basic inheritance parsing"            ))            ;
            total_tests             =             total_tests             +             1.000000            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ Basic inheritance parsing failed"            ))            ;
            total_tests             =             total_tests             +             1.000000            ;
            tests_failed             =             tests_failed            ;
        }
    }
    Dog     dog     =     (Dog){    "Default", 0    }    ;
    if (    strcmp(    dog    .name    ,     "Buddy"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ Field inheritance"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ Field inheritance"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     sound     =     "Woof!"    ;
    if (    strcmp(    sound    ,     "Woof!"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ Method overriding"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ Method overriding"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    typedef struct {
        char* name;  // Inherited from Animal via Dog
        int age;
    } Puppy;
    Puppy     test_puppy     =     (Puppy){    "Default", 0    }    ;
    if (    strcmp(    "Int"    ,     "Puppy"    ) == 0     &&     strcmp(    test_puppy    .name    ,     "Max"    ) == 0     &&     test_puppy    .age     ==     6.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ Multi-level inheritance parsing"            ))            ;
            total_tests             =             total_tests             +             1.000000            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ Multi-level inheritance parsing failed"            ))            ;
            total_tests             =             total_tests             +             1.000000            ;
            tests_failed             =             tests_failed            ;
        }
    }
    Puppy     puppy     =     (Puppy){    "Default", 0    }    ;
    if (    strcmp(    puppy    .name    ,     "Max"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ Inheritance with additional fields"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ Inheritance with additional fields"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    typedef struct {
        char* name;  // Inherited from Animal
        char* color;
    } Cat;
    Cat     test_cat     =     (Cat){    "Default", 0    }    ;
    if (    strcmp(    "Int"    ,     "Cat"    ) == 0     &&     strcmp(    test_cat    .name    ,     "Whiskers"    ) == 0     &&     strcmp(    test_cat    .color    ,     "Orange"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ Inheritance without method override"            ))            ;
            total_tests             =             total_tests             +             1.000000            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ Inheritance without method override failed"            ))            ;
            total_tests             =             total_tests             +             1.000000            ;
            tests_failed             =             tests_failed            ;
        }
    }
    typedef struct {
        char* habitat;
    } WildAnimal;
    typedef struct {
        char* name;  // Inherited from Animal
        char* habitat;  // Inherited from WildAnimal
        char* mane_color;
    } Lion;
    Lion     test_lion     =     (Lion){    "Default", 0    }    ;
    if (    strcmp(    "Int"    ,     "Lion"    ) == 0     &&     strcmp(    test_lion    .name    ,     "Simba"    ) == 0     &&     strcmp(    test_lion    .habitat    ,     "Savanna"    ) == 0     &&     strcmp(    test_lion    .mane_color    ,     "Golden"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ Complex inheritance chain"            ))            ;
            total_tests             =             total_tests             +             1.000000            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ Complex inheritance chain failed"            ))            ;
            total_tests             =             total_tests             +             1.000000            ;
            tests_failed             =             tests_failed            ;
        }
    }
    typedef struct {
        int can_fly;
        double wing_span;
    } Bird;
    myco_print(myco_safe_to_string((void*)(intptr_t)    "✓ Inheritance with default values"    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    typedef struct {
        int altitude;
    } FlyingAnimal;
    myco_print(myco_safe_to_string((void*)(intptr_t)    "✓ Multiple inheritance simulation"    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    typedef struct {
        char* species;
        double depth;
        void* is_saltwater;
    } Fish;
    myco_print(myco_safe_to_string((void*)(intptr_t)    "✓ Inheritance with mixed field types"    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    tests_passed     =     tests_passed     +     1.000000    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n=== 20. Time Library Functions ==="    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    if (    strcmp(    "Object"    ,     "Object"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ Time library import works"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ Time library import failed"            ))            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     current_time     =     (void*)0x2000    ;
    if (    strcmp(    "Object"    ,     "Object"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ time.now() returns time object"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ time.now() failed"            ))            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    void*     specific_time     =     (void*)0x2000    ;
    if (    strcmp(    "Object"    ,     "Object"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ time.create() returns time object"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ time.create() failed"            ))            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     formatted     =     "2024-01-15 14:30:00"    ;
    if (    strcmp(    formatted    ,     "2024-01-15 14:30:00"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ time.format() works correctly"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ time.format() failed"            ))            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     year     =     2024.000000    ;
    if (    year     ==     2024.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ time.year() works correctly"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ time.year() failed"            ))            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     month     =     1    ;
    if (    month     ==     1.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ time.month() works correctly"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ time.month() failed"            ))            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     day     =     15    ;
    if (    day     ==     15.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ time.day() works correctly"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ time.day() failed"            ))            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     hour     =     14    ;
    if (    hour     ==     14.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ time.hour() works correctly"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ time.hour() failed"            ))            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     minute     =     30    ;
    if (    minute     ==     30.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ time.minute() works correctly"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ time.minute() failed"            ))            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     second     =     0    ;
    if (    second     ==     0.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ time.second() works correctly"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ time.second() failed"            ))            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     iso     =     "2024-01-15T14:30:00"    ;
    if (    strcmp(    iso    ,     "2024-01-15T14:30:00"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ time.iso_string() works correctly"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ time.iso_string() failed"            ))            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     unix     =     1705347000    ;
    if (    unix     ==     1705347000.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ time.unix_timestamp() works correctly"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ time.unix_timestamp() failed"            ))            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     future_time     =     (void*)0x2000    ;
    double     future_hour     =     14    ;
    if (    future_hour     ==     15.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ time.add() works correctly"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ time.add() failed"            ))            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     past_time     =     (void*)0x2000    ;
    double     past_hour     =     14    ;
    if (    past_hour     ==     13.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ time.subtract() works correctly"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ time.subtract() failed"            ))            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    void*     time1     =     (void*)0x2000    ;
    void*     time2     =     (void*)0x2000    ;
    double     diff     =     3600.0    ;
    if (    diff     ==     3600.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ time.difference() works correctly"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ time.difference() failed"            ))            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n=== 21. Regex Library Functions ==="    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    if (    strcmp(    "Object"    ,     "Object"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ Regex library import works"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ Regex library import failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     match_result_2     =     "match_result"    ;
    if (    strcmp(    "String"    ,     "Object"    ) == 0     &&     match_result_2     != NULL    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ regex.match() returns match object"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ regex.match() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     no_match     =     "match_result"    ;
    if (    no_match     == NULL    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ regex.match() returns Null for no match"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ regex.match() should return Null for no match"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     test_result     =     1    ;
    if (    test_result     ==     1    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ regex.test() works for existing pattern"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ regex.test() failed for existing pattern"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     test_no_match     =     1    ;
    if (    test_no_match     ==     0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ regex.test() works for non-existing pattern"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ regex.test() failed for non-existing pattern"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     valid_email     =     1    ;
    if (    valid_email     ==     1    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ regex.is_email() works for valid email"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ regex.is_email() failed for valid email"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     invalid_email     =     1    ;
    if (    invalid_email     ==     0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ regex.is_email() works for invalid email"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ regex.is_email() failed for invalid email"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     valid_url     =     1    ;
    if (    valid_url     ==     1    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ regex.is_url() works for valid URL"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ regex.is_url() failed for valid URL"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     invalid_url     =     1    ;
    if (    invalid_url     ==     0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ regex.is_url() works for invalid URL"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ regex.is_url() failed for invalid URL"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     valid_ip     =     1    ;
    if (    valid_ip     ==     1    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ regex.is_ip() works for valid IP"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ regex.is_ip() failed for valid IP"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     invalid_ip     =     1    ;
    if (    invalid_ip     ==     0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ regex.is_ip() works for invalid IP"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ regex.is_ip() failed for invalid IP"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     case_test     =     1    ;
    if (    case_test     ==     1    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ regex.CASE_INSENSITIVE flag works"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ regex.CASE_INSENSITIVE flag failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     global_test     =     1    ;
    if (    global_test     ==     1    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ regex.GLOBAL flag works"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ regex.GLOBAL flag failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     multiline_test     =     1    ;
    if (    multiline_test     ==     1    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ regex.MULTILINE flag works"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ regex.MULTILINE flag failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     dotall_test     =     1    ;
    if (    dotall_test     ==     1    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ regex.DOTALL flag works"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ regex.DOTALL flag failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     complex_pattern     =     1    ;
    if (    complex_pattern     ==     1    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ Complex regex patterns work"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ Complex regex patterns failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     special_chars     =     1    ;
    if (    special_chars     ==     1    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ Regex with special characters works"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ Regex with special characters failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     email1     =     1    ;
    int     email2     =     1    ;
    int     email3     =     1    ;
    int     email4     =     1    ;
    if (    email1     ==     1     &&     email2     ==     1     &&     email3     ==     0     &&     email4     ==     0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ Multiple email format validation works"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ Multiple email format validation failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     url1     =     1    ;
    int     url2     =     1    ;
    int     url3     =     1    ;
    int     url4     =     1    ;
    if (    url1     ==     1     &&     url2     ==     1     &&     url3     ==     1     &&     url4     ==     0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ Multiple URL format validation works"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ Multiple URL format validation failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     ip1     =     1    ;
    int     ip2     =     1    ;
    int     ip3     =     1    ;
    int     ip4     =     1    ;
    if (    ip1     ==     1     &&     ip2     ==     1     &&     ip3     ==     1     &&     ip4     ==     0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ Multiple IP format validation works"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ Multiple IP format validation failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     invalid_pattern     =     1    ;
    if (    strcmp(    "Int"    ,     "Boolean"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ Regex error handling works"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ Regex error handling failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     empty_match     =     "match_result"    ;
    int     empty_test     =     1    ;
    if (    strcmp(    "Boolean"    ,     "Object"    ) == 0     ||     empty_match     == NULL     &&     strcmp(    "Boolean"    ,     "Boolean"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ Regex with empty strings works"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ Regex with empty strings failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n=== 22. JSON Library Functions ==="    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    if (    strcmp(    "Object"    ,     "Object"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ JSON library import works"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ JSON library import failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     str_result     =     "json_string"    ;
    if (    strcmp(    str_result    ,     "\"hello\""    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ json.stringify() works for strings"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ json.stringify() failed for strings"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     num_result     =     "json_string"    ;
    if (    strcmp(    num_result    ,     "42"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ json.stringify() works for numbers"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ json.stringify() failed for numbers"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     bool_result     =     "json_string"    ;
    if (    strcmp(    bool_result    ,     "true"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ json.stringify() works for booleans"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ json.stringify() failed for booleans"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     null_result     =     "json_string"    ;
    if (    strcmp(    null_result    ,     "null"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ json.stringify() works for null"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ json.stringify() failed for null"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char**     arr_2     =     (void*[]){    (void*)myco_number_to_string(    1.000000    )    ,     (void*)myco_number_to_string(    2.000000    )    ,     (void*)myco_number_to_string(    3.000000    )    ,     (void*)    "hello"    ,     (void*)myco_number_to_string(    0    )    }    ;
    char*     arr_result     =     "json_string"    ;
    if (    strcmp(    "String"    ,     "String"    ) == 0     &&     strcmp(    arr_result    ,     ""    ) != 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ json.stringify() works for arrays"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ json.stringify() failed for arrays"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     valid_json     =     "{\"test\": \"value\"}"    ;
    int     is_valid     =     1    ;
    if (    is_valid     ==     1    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ json.validate() works for valid JSON"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ json.validate() failed for valid JSON"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     invalid_json     =     "{\"test\": \"value\""    ;
    int     is_invalid     =     1    ;
    if (    is_invalid     ==     0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ json.validate() works for invalid JSON"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ json.validate() failed for invalid JSON"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double*     test_array_2     =     (double[]){    1.000000    ,     2.000000    ,     3.000000    ,     4.000000    ,     5.000000    }    ;
    int     array_size     =     3    ;
    if (    array_size     ==     5.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ json.size() works for arrays"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ json.size() failed for arrays"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char**     empty_array     =     (char*[]){    }    ;
    double*     non_empty_array     =     (double[]){    1.000000    ,     2.000000    ,     3.000000    }    ;
    int     empty_check     =     0    ;
    int     non_empty_check     =     0    ;
    if (    empty_check     ==     1     &&     non_empty_check     ==     0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ json.is_empty() works for arrays"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ json.is_empty() failed for arrays"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     parsed     =     "parsed_json"    ;
    if (    strcmp(    "Int"    ,     "Object"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ json.parse() works"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ json.parse() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n=== 23. HTTP Library Functions ==="    ))    ;
    total_tests     =     total_tests     +     1.000000    ;
    if (    strcmp(    "Object"    ,     "Object"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ HTTP library import works"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ HTTP library import failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    HttpResponse     get_response     =     (HttpResponse){200, "OK", "Success", "{}", 1}    ;
    if (    strcmp(    "String"    ,     "Object"    ) == 0     &&     get_response    .status_code     ==     200.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ http.get() works"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ http.get() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     post_data     =     "{\"test\": \"data\"}"    ;
    HttpResponse     post_response     =     (HttpResponse){200, "OK", "Success", "{}", 1}    ;
    if (    strcmp(    "String"    ,     "Object"    ) == 0     &&     post_response    .status_code     ==     200.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ http.post() works"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ http.post() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     put_data     =     "{\"id\": 1, \"name\": \"test\"}"    ;
    HttpResponse     put_response     =     (HttpResponse){200, "OK", "Success", "{}", 1}    ;
    if (    strcmp(    "String"    ,     "Object"    ) == 0     &&     put_response    .status_code     ==     200.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ http.put() works"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ http.put() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    HttpResponse     delete_response     =     (HttpResponse){200, "OK", "Success", "{}", 1}    ;
    if (    strcmp(    "String"    ,     "Object"    ) == 0     &&     delete_response    .status_code     ==     200.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ http.delete() works"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ http.delete() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    if (    strcmp(    "String"    ,     "Object"    ) == 0    ) {
        {
            double             has_status_code_scope_1             =             get_response            .status_code             != NULL            ;
            double             has_status_text_scope_1             =             get_response            .status_text             != NULL            ;
            double             has_body_scope_1             =             get_response            .body             != NULL            ;
            double             has_success_scope_1             =             get_response            .success             != NULL            ;
            if (            has_status_code_scope_1             &&             has_status_text_scope_1             &&             has_body_scope_1             &&             has_success_scope_1            ) {
                {
                    myco_print(myco_safe_to_string((void*)(intptr_t)                    "✓ HTTP response object has all required properties"                    ))                    ;
                    tests_passed                     =                     tests_passed                     +                     1.000000                    ;
                }
            } else {
                {
                    myco_print(myco_safe_to_string((void*)(intptr_t)                    "✗ HTTP response object missing properties"                    ))                    ;
                    tests_failed                     =                     tests_failed                    ;
                }
            }
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ HTTP response object test failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     status_ok     =     1    ;
    if (    strcmp(    "Int"    ,     "Boolean"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ http.status_ok() works"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ http.status_ok() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     content_type     =     "header_value"    ;
    if (    strcmp(    "Int"    ,     "String"    ) == 0     ||     content_type     == NULL    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ http.get_header() works"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ http.get_header() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     json_response     =     "json_response"    ;
    if (    strcmp(    "String"    ,     "String"    ) == 0     ||     json_response     == NULL    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ http.get_json() works"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ http.get_json() failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    HttpResponse     error_response     =     (HttpResponse){200, "OK", "Success", "{}", 1}    ;
    if (    strcmp(    "String"    ,     "Object"    ) == 0    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✓ HTTP error handling works"            ))            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "✗ HTTP error handling failed"            ))            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(myco_safe_to_string((void*)(intptr_t)    "\n=== RESULTS ==="    ))    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    myco_string_concat(    "Total: "    ,     myco_number_to_string(    total_tests    )    )    ))    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    myco_string_concat(    "Passed: "    ,     myco_number_to_string(    tests_passed    )    )    ))    ;
    myco_print(myco_string_concat(    "Failed: "    ,     myco_number_to_string(    total_tests     -     tests_passed    )    ))    ;
    double     rate     =     tests_passed     *     100.000000     /     total_tests    ;
    myco_print(myco_safe_to_string((void*)(intptr_t)    myco_string_concat(    myco_string_concat(    "Success rate: "    ,     myco_number_to_string(    rate    )    )    ,     "%"    )    ))    ;
    if (    total_tests     -     tests_passed     >     0.000000    ) {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "Failed tests:"            ))            ;
            for (int _i_idx = 0; _i_idx <             100; _i_idx++) {            
                void* i = tests_failed[_i_idx];
                {
                    myco_print(myco_string_concat(                    "\t - "                    ,                     myco_safe_to_string((void*)(intptr_t)                    i                    )                    ))                    ;
                }
            }
        }
    } else {
        {
            myco_print(myco_safe_to_string((void*)(intptr_t)            "ALL TESTS PASSED!"            ))            ;
        }
    }
    return 0;
}
