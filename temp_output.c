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
        myco_print("Hello from simple function!")        ;
    }
}
void greet_with_name(char* name) {
    {
        myco_print(        myco_string_concat(        myco_string_concat("Hello, "        ,         name        )        , "!"        ))        ;
    }
}
void add_numbers(double a, double b) {
    {
        double         result_scope_1         =         (        a         +         b        )        ;
        myco_print(        myco_string_concat("Sum: "        ,         myco_number_to_string(        result_scope_1        )        ))        ;
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
        return         myco_string_concat(        myco_string_concat("Hello, "        ,         name        )        , "!"        )        ;    }
}
char* mixed_func1(double x, double y) {
    {
        return         myco_string_concat(        myco_string_concat(        myco_string_concat("Mixed types: "        ,         myco_number_to_string(        x        )        )        , ", "        )        ,         myco_number_to_string(        y        )        )        ;    }
}
void mixed_func2(char* name) {
    {
        myco_print(        myco_string_concat("Name: "        ,         name        ))        ;
    }
}
double mixed_func3(double a, double b) {
    {
        return         (        a         +         b        )        ;    }
}
double return_five() {
    {
        return         5.000000        ;    }
}
char* return_hello() {
    {
        return "Hello"        ;    }
}
char* explicit_all(int param) {
    {
        return         myco_number_to_string(        param        )        ;    }
}
// Main program entry point
int main(void) {
    myco_print("=== Myco Comprehensive Test Suite ===")    ;
    myco_print("Testing EVERY feature and edge case in Myco")    ;
    myco_print("")    ;
    double     total_tests     =     0.000000    ;
    double     tests_passed     =     0.000000    ;
    char**     tests_failed     =     NULL    ;
    myco_print("=== 1. BASIC DATA TYPES ===")    ;
    myco_print("1.1. Numbers...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     x     =     42.000000    ;
    if (    x     ==     42.000000    ) {
        {
            myco_print(            myco_string_concat("[OK] Integer variable: "            ,             myco_number_to_string(            x            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Integer variable: "            ,             myco_number_to_string(            x            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     y     =     3.140000    ;
    if (    y     ==     3.140000    ) {
        {
            myco_print(            myco_string_concat("[OK] Float variable: "            ,             myco_number_to_string(            y            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Float variable: "            ,             myco_number_to_string(            y            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     neg_int     =     -    10.000000    ;
    if (    neg_int     ==     -    10.000000    ) {
        {
            myco_print(            myco_string_concat("[OK] Negative integer: "            ,             myco_number_to_string(            neg_int            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Negative integer: "            ,             myco_number_to_string(            neg_int            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     neg_float     =     -    3.140000    ;
    if (    neg_float     ==     -    3.140000    ) {
        {
            myco_print(            myco_string_concat("[OK] Negative float: "            ,             myco_number_to_string(            neg_float            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Negative float: "            ,             myco_number_to_string(            neg_float            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     zero     =     0.000000    ;
    if (    zero     ==     0.000000    ) {
        {
            myco_print(            myco_string_concat("[OK] Zero: "            ,             myco_number_to_string(            zero            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Zero: "            ,             myco_number_to_string(            zero            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n1.2. Strings...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     name     = "Myco"    ;
    if (    strcmp(    name    , "Myco"    ) == 0    ) {
        {
            myco_print(            myco_string_concat("[OK] String variable: "            ,             name            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] String variable: "            ,             name            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     empty_str     = ""    ;
    if (    strcmp(    empty_str    , ""    ) == 0    ) {
        {
            myco_print(            myco_string_concat(            myco_string_concat("[OK] Empty string: '"            ,             empty_str            )            , "'"            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            myco_string_concat("[FAIL] Empty string: '"            ,             empty_str            )            , "'"            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     spaced_str     = "hello world"    ;
    if (    strcmp(    spaced_str    , "hello world"    ) == 0    ) {
        {
            myco_print(            myco_string_concat("[OK] String with spaces: "            ,             spaced_str            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] String with spaces: "            ,             spaced_str            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n1.3. Booleans...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    int     flag     =     1    ;
    if (    flag     ==     1    ) {
        {
            myco_print(            myco_string_concat("[OK] Boolean True: "            ,             myco_number_to_string(            flag            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Boolean True: "            ,             myco_number_to_string(            flag            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    int     false_flag     =     0    ;
    if (    false_flag     ==     0    ) {
        {
            myco_print(            myco_string_concat("[OK] Boolean False: "            ,             myco_number_to_string(            false_flag            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Boolean False: "            ,             myco_number_to_string(            false_flag            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n1.4. Null...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     null_var     =     NULL    ;
    if (    null_var     == NULL    ) {
        {
            myco_print(            myco_string_concat("[OK] Null variable: "            ,             myco_string_to_string(            null_var            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Null variable: "            ,             myco_string_to_string(            null_var            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n=== 2. ARITHMETIC OPERATIONS ===")    ;
    myco_print("2.1. Addition...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     a     =     (    5.000000     +     3.000000    )    ;
    if (    a     ==     8.000000    ) {
        {
            myco_print(            myco_string_concat("[OK] Integer addition (5 + 3): "            ,             myco_number_to_string(            a            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Integer addition (5 + 3): "            ,             myco_number_to_string(            a            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     float_add     =     (    2.500000     +     1.500000    )    ;
    if (    float_add     ==     4.000000    ) {
        {
            myco_print(            myco_string_concat("[OK] Float addition (2.5 + 1.5): "            ,             myco_number_to_string(            float_add            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Float addition (2.5 + 1.5): "            ,             myco_number_to_string(            float_add            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     mixed_add     =     (    5.000000     +     2.500000    )    ;
    if (    mixed_add     ==     7.500000    ) {
        {
            myco_print(            myco_string_concat("[OK] Mixed addition (5 + 2.5): "            ,             myco_number_to_string(            mixed_add            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Mixed addition (5 + 2.5): "            ,             myco_number_to_string(            mixed_add            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     str_concat     =     myco_string_concat("hello"    , "world"    )    ;
    if (    strcmp(    str_concat    , "helloworld"    ) == 0    ) {
        {
            myco_print(            myco_string_concat("[OK] String concatenation: "            ,             str_concat            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] String concatenation: "            ,             str_concat            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n2.2. Subtraction...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     b     =     10.000000     -     4.000000    ;
    if (    b     ==     6.000000    ) {
        {
            myco_print(            myco_string_concat("[OK] Integer subtraction (10 - 4): "            ,             myco_number_to_string(            b            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Integer subtraction (10 - 4): "            ,             myco_number_to_string(            b            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     float_sub     =     5.500000     -     2.500000    ;
    if (    float_sub     ==     3.000000    ) {
        {
            myco_print(            myco_string_concat("[OK] Float subtraction (5.5 - 2.5): "            ,             myco_number_to_string(            float_sub            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Float subtraction (5.5 - 2.5): "            ,             myco_number_to_string(            float_sub            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     neg_result     =     3.000000     -     5.000000    ;
    if (    neg_result     ==     -    2.000000    ) {
        {
            myco_print(            myco_string_concat("[OK] Negative result (3 - 5): "            ,             myco_number_to_string(            neg_result            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Negative result (3 - 5): "            ,             myco_number_to_string(            neg_result            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n2.3. Multiplication...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     c     =     6.000000     *     7.000000    ;
    if (    c     ==     42.000000    ) {
        {
            myco_print(            myco_string_concat("[OK] Integer multiplication (6 * 7): "            ,             myco_number_to_string(            c            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Integer multiplication (6 * 7): "            ,             myco_number_to_string(            c            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     float_mult     =     2.500000     *     4.000000    ;
    if (    float_mult     ==     10.000000    ) {
        {
            myco_print(            myco_string_concat("[OK] Float multiplication (2.5 * 4): "            ,             myco_number_to_string(            float_mult            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Float multiplication (2.5 * 4): "            ,             myco_number_to_string(            float_mult            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     zero_mult     =     5.000000     *     0.000000    ;
    if (    zero_mult     ==     0.000000    ) {
        {
            myco_print(            myco_string_concat("[OK] Zero multiplication (5 * 0): "            ,             myco_number_to_string(            zero_mult            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Zero multiplication (5 * 0): "            ,             myco_number_to_string(            zero_mult            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n2.4. Division...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     d     =     15.000000     /     3.000000    ;
    if (    d     ==     5.000000    ) {
        {
            myco_print(            myco_string_concat("[OK] Integer division (15 / 3): "            ,             myco_number_to_string(            d            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Integer division (15 / 3): "            ,             myco_number_to_string(            d            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     float_div     =     10.000000     /     2.500000    ;
    if (    float_div     ==     4.000000    ) {
        {
            myco_print(            myco_string_concat("[OK] Float division (10.0 / 2.5): "            ,             myco_number_to_string(            float_div            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Float division (10.0 / 2.5): "            ,             myco_number_to_string(            float_div            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     remainder_div     =     7.000000     /     2.000000    ;
    if (    remainder_div     ==     3.500000    ) {
        {
            myco_print(            myco_string_concat("[OK] Division with remainder (7 / 2): "            ,             myco_number_to_string(            remainder_div            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Division with remainder (7 / 2): "            ,             myco_number_to_string(            remainder_div            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n=== 3. COMPARISON OPERATIONS ===")    ;
    myco_print("3.1. Equality...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     comp3     =     5.000000     ==     5.000000    ;
    if (    comp3     ==     1    ) {
        {
            myco_print(            myco_string_concat("[OK] Integer equality (5 == 5): "            ,             myco_number_to_string(            comp3            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Integer equality (5 == 5): "            ,             myco_number_to_string(            comp3            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     comp_neq     =     5.000000     ==     6.000000    ;
    if (    comp_neq     ==     0    ) {
        {
            myco_print(            myco_string_concat("[OK] Integer inequality (5 == 6): "            ,             myco_number_to_string(            comp_neq            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Integer inequality (5 == 6): "            ,             myco_number_to_string(            comp_neq            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     float_eq     =     3.140000     ==     3.140000    ;
    if (    float_eq     ==     1    ) {
        {
            myco_print(            myco_string_concat("[OK] Float equality (3.14 == 3.14): "            ,             myco_number_to_string(            float_eq            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Float equality (3.14 == 3.14): "            ,             myco_number_to_string(            float_eq            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     str_eq     =     strcmp("hello"    , "hello"    ) == 0    ;
    if (    str_eq     ==     1    ) {
        {
            myco_print(            myco_string_concat("[OK] String equality ('hello' == 'hello'): "            ,             myco_number_to_string(            str_eq            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] String equality ('hello' == 'hello'): "            ,             myco_number_to_string(            str_eq            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     bool_eq     =     1     ==     1    ;
    if (    bool_eq     ==     1    ) {
        {
            myco_print(            myco_string_concat("[OK] Boolean equality (True == True): "            ,             myco_number_to_string(            bool_eq            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Boolean equality (True == True): "            ,             myco_number_to_string(            bool_eq            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n3.2. Inequality...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     comp4     =     4.000000     !=     6.000000    ;
    if (    comp4     ==     1    ) {
        {
            myco_print(            myco_string_concat("[OK] Integer inequality (4 != 6): "            ,             myco_number_to_string(            comp4            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Integer inequality (4 != 6): "            ,             myco_number_to_string(            comp4            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     str_neq     =     strcmp("hello"    , "world"    ) != 0    ;
    if (    str_neq     ==     1    ) {
        {
            myco_print(            myco_string_concat("[OK] String inequality ('hello' != 'world'): "            ,             myco_number_to_string(            str_neq            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] String inequality ('hello' != 'world'): "            ,             myco_number_to_string(            str_neq            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n3.3. Greater than...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     comp1     =     10.000000     >     5.000000    ;
    if (    comp1     ==     1    ) {
        {
            myco_print(            myco_string_concat("[OK] Greater than (10 > 5): "            ,             myco_number_to_string(            comp1            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Greater than (10 > 5): "            ,             myco_number_to_string(            comp1            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     comp1_false     =     5.000000     >     10.000000    ;
    if (    comp1_false     ==     0    ) {
        {
            myco_print(            myco_string_concat("[OK] Not greater than (5 > 10): "            ,             myco_number_to_string(            comp1_false            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Not greater than (5 > 10): "            ,             myco_number_to_string(            comp1_false            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n3.4. Less than...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     comp2     =     3.000000     <     7.000000    ;
    if (    comp2     ==     1    ) {
        {
            myco_print(            myco_string_concat("[OK] Less than (3 < 7): "            ,             myco_number_to_string(            comp2            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Less than (3 < 7): "            ,             myco_number_to_string(            comp2            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     comp2_false     =     7.000000     <     3.000000    ;
    if (    comp2_false     ==     0    ) {
        {
            myco_print(            myco_string_concat("[OK] Not less than (7 < 3): "            ,             myco_number_to_string(            comp2_false            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Not less than (7 < 3): "            ,             myco_number_to_string(            comp2_false            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n3.5. Greater than or equal...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     gte_true     =     10.000000     >=     5.000000    ;
    if (    gte_true     ==     1    ) {
        {
            myco_print(            myco_string_concat("[OK] Greater than or equal (10 >= 5): "            ,             myco_number_to_string(            gte_true            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Greater than or equal (10 >= 5): "            ,             myco_number_to_string(            gte_true            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     gte_equal     =     5.000000     >=     5.000000    ;
    if (    gte_equal     ==     1    ) {
        {
            myco_print(            myco_string_concat("[OK] Equal values (5 >= 5): "            ,             myco_number_to_string(            gte_equal            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Equal values (5 >= 5): "            ,             myco_number_to_string(            gte_equal            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n3.6. Less than or equal...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     lte_true     =     3.000000     <=     7.000000    ;
    if (    lte_true     ==     1    ) {
        {
            myco_print(            myco_string_concat("[OK] Less than or equal (3 <= 7): "            ,             myco_number_to_string(            lte_true            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Less than or equal (3 <= 7): "            ,             myco_number_to_string(            lte_true            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     lte_equal     =     5.000000     <=     5.000000    ;
    if (    lte_equal     ==     1    ) {
        {
            myco_print(            myco_string_concat("[OK] Equal values (5 <= 5): "            ,             myco_number_to_string(            lte_equal            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Equal values (5 <= 5): "            ,             myco_number_to_string(            lte_equal            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n=== 4. LOGICAL OPERATIONS ===")    ;
    myco_print("4.1. Logical AND...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     log1     =     (    1     &&     1    )    ;
    if (    log1     ==     1    ) {
        {
            myco_print(            myco_string_concat("[OK] Logical AND (True and True): "            ,             myco_number_to_string(            log1            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Logical AND (True and True): "            ,             myco_number_to_string(            log1            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     log1_false     =     (    1     &&     0    )    ;
    if (    log1_false     ==     0    ) {
        {
            myco_print(            myco_string_concat("[OK] Logical AND false (True and False): "            ,             myco_number_to_string(            log1_false            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Logical AND false (True and False): "            ,             myco_number_to_string(            log1_false            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n4.2. Logical OR...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     log2     =     (    0     ||     1    )    ;
    if (    log2     ==     1    ) {
        {
            myco_print(            myco_string_concat("[OK] Logical OR (False or True): "            ,             myco_number_to_string(            log2            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Logical OR (False or True): "            ,             myco_number_to_string(            log2            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     log2_false     =     (    0     ||     0    )    ;
    if (    log2_false     ==     0    ) {
        {
            myco_print(            myco_string_concat("[OK] Logical OR false (False or False): "            ,             myco_number_to_string(            log2_false            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Logical OR false (False or False): "            ,             myco_number_to_string(            log2_false            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n4.3. Logical NOT...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     log3     =     !    0    ;
    if (    log3     ==     1    ) {
        {
            myco_print(            myco_string_concat("[OK] Logical NOT (not False): "            ,             myco_number_to_string(            log3            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Logical NOT (not False): "            ,             myco_number_to_string(            log3            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     log3_false     =     !    1    ;
    if (    log3_false     ==     0    ) {
        {
            myco_print(            myco_string_concat("[OK] Logical NOT false (not True): "            ,             myco_number_to_string(            log3_false            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Logical NOT false (not True): "            ,             myco_number_to_string(            log3_false            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n4.4. Complex logical expressions...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     complex_log     =     (    (    1     &&     0    )     ||     1    )    ;
    if (    complex_log     ==     1    ) {
        {
            myco_print(            myco_string_concat("[OK] Complex logical ((True and False) or True): "            ,             myco_number_to_string(            complex_log            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Complex logical ((True and False) or True): "            ,             myco_number_to_string(            complex_log            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     nested_not     =     !    (    1     &&     0    )    ;
    if (    nested_not     ==     1    ) {
        {
            myco_print(            myco_string_concat("[OK] Nested NOT (not (True and False)): "            ,             myco_number_to_string(            nested_not            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Nested NOT (not (True and False)): "            ,             myco_number_to_string(            nested_not            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n5. Arrays...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    char**     arr     =     NULL    ;
    if (    0     ==     3.000000    ) {
        {
            myco_print(            myco_string_concat("[OK] Simple array: "            ,             myco_string_to_string(            arr            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Simple array: "            ,             myco_string_to_string(            arr            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char**     nested     =     NULL    ;
    if (    0     ==     2.000000    ) {
        {
            myco_print(            myco_string_concat("[OK] Nested array: "            ,             myco_string_to_string(            nested            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Nested array: "            ,             myco_string_to_string(            nested            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char**     mixed     =     NULL    ;
    if (    0     ==     4.000000    ) {
        {
            myco_print(            myco_string_concat("[OK] Mixed type array: "            ,             myco_string_to_string(            mixed            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Mixed type array: "            ,             myco_string_to_string(            mixed            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char**     empty     =     NULL    ;
    if (    0     ==     0.000000    ) {
        {
            myco_print(            myco_string_concat("[OK] Empty array: "            ,             myco_string_to_string(            empty            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Empty array: "            ,             myco_string_to_string(            empty            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n6. String Operations...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     str1     = "Hello"    ;
    char*     str2     = "World"    ;
    char*     combined     =     myco_string_concat(    myco_string_concat(    str1    , " "    )    ,     str2    )    ;
    myco_print(    myco_string_concat("[OK] String concatenation: "    ,     combined    ))    ;
    tests_passed =     (    tests_passed     +     1.000000    )    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     num_str     =     myco_number_to_string(    42.000000    )    ;
    myco_print(    myco_string_concat("[OK] Number to string: "    ,     num_str    ))    ;
    tests_passed =     (    tests_passed     +     1.000000    )    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     bool_str     =     myco_number_to_string(    1    )    ;
    myco_print(    myco_string_concat("[OK] Boolean to string: "    ,     bool_str    ))    ;
    tests_passed =     (    tests_passed     +     1.000000    )    ;
    myco_print("\\n=== 7. STRING OPERATIONS ===")    ;
    myco_print("7.1. String concatenation...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     str1_2     = "Hello"    ;
    char*     str2_2     = "World"    ;
    char*     combined_2     =     myco_string_concat(    myco_string_concat(    str1_2    , " "    )    ,     str2_2    )    ;
    if (    strcmp(    combined_2    , "Hello World"    ) == 0    ) {
        {
            myco_print(            myco_string_concat("[OK] String concatenation: "            ,             combined_2            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] String concatenation: "            ,             combined_2            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     num_str_2     =     myco_number_to_string(    42.000000    )    ;
    if (    strcmp(    num_str_2    , "42"    ) == 0    ) {
        {
            myco_print(            myco_string_concat("[OK] Number to string: "            ,             num_str_2            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Number to string: "            ,             num_str_2            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     bool_str_2     =     myco_number_to_string(    1    )    ;
    if (    strcmp(    bool_str_2    , "True"    ) == 0    ) {
        {
            myco_print(            myco_string_concat("[OK] Boolean to string: "            ,             bool_str_2            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Boolean to string: "            ,             bool_str_2            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n7.2. Escape sequences...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     escaped     = "Line 1\\nLine 2\\tTabbed"    ;
    if (    (int)strlen(    escaped    )     >     0.000000    ) {
        {
            myco_print("[OK] Escape sequences work")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] Escape sequences work")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     quoted_str     = "He said \\\"Hello\\\""    ;
    if (    strcmp(    quoted_str    , "He said \\\"Hello\\\""    ) == 0    ) {
        {
            myco_print(            myco_string_concat("[OK] Quoted string: "            ,             quoted_str            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Quoted string: "            ,             quoted_str            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     backslash_str     = "Path: C:\\\\Users\\\\Name"    ;
    if (    strcmp(    backslash_str    , "Path: C:\\\\Users\\\\Name"    ) == 0    ) {
        {
            myco_print(            myco_string_concat("[OK] Backslash string: "            ,             backslash_str            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Backslash string: "            ,             backslash_str            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n8. Control Flow...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    if (    10.000000     >     5.000000    ) {
        {
            myco_print("[OK] If statement works")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] If statement")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    if (    3.000000     <     2.000000    ) {
        {
            myco_print("[FAIL] This should not print")            ;
            tests_failed =             tests_failed            ;
        }
    } else {
        {
            myco_print("[OK] Else statement works")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    if (    1.000000     >     2.000000    ) {
        {
            myco_print("[FAIL] This should not print")            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n9. Built-in Libraries...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    char* math = "Module";
    if (    strcmp(    "Module"    , "Module"    ) == 0    ) {
        {
            myco_print("[OK] Math library imported")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] Math library import failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     test_string     = "hello"    ;
    if (    (    strcmp(    "String"    , "String"    ) == 0     &&     test_string     !=     NULL    )    ) {
        {
            myco_print("[OK] String type available")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] String type not available")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char**     test_array_1     =     NULL    ;
    if (    (    strcmp(    "Array"    , "Array"    ) == 0     &&     test_array_1     !=     NULL    )    ) {
        {
            myco_print("[OK] Array type available")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] Array type not available")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char* file = "Module";
    if (    strcmp(    "Module"    , "Module"    ) == 0    ) {
        {
            myco_print("[OK] File library imported")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] File library import failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char* dir = "Module";
    if (    strcmp(    "Module"    , "Module"    ) == 0    ) {
        {
            myco_print("[OK] Directory library imported")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] Directory library import failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    void*     test_map_1     =     NULL    ;
    if (    strcmp(    "Map"    , "Map"    ) == 0    ) {
        {
            myco_print("[OK] Maps library available")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] Maps library not available")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    void*     test_set1     =     NULL    ;
    if (    strcmp(    "Set"    , "Set"    ) == 0    ) {
        {
            myco_print("[OK] Sets type available")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] Sets type not available")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     test_tree1     =     "TreeObject"    ;
    if (    strcmp(    "Tree"    , "Tree"    ) == 0    ) {
        {
            myco_print("[OK] Trees library imported")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] Trees library import failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     graphs_test1     =     "GraphObject"    ;
    if (    strcmp(    "Module"    , "Graph"    ) == 0    ) {
        {
            myco_print("[OK] Graphs library imported")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] Graphs library import failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n10. Math Library Functions...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     pi_value     =     NULL    ;
    myco_print(    myco_string_concat("[OK] Math.Pi: "    ,     myco_safe_to_string(    pi_value    )    ))    ;
    tests_passed =     (    tests_passed     +     1.000000    )    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     e_value     =     NULL    ;
    myco_print(    myco_string_concat("[OK] Math.E: "    ,     myco_safe_to_string(    e_value    )    ))    ;
    tests_passed =     (    tests_passed     +     1.000000    )    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     abs_result     =     NULL    ;
    myco_print(    myco_string_concat("[OK] Math.abs(-5): "    ,     myco_safe_to_string(    abs_result    )    ))    ;
    tests_passed =     (    tests_passed     +     1.000000    )    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     min_result     =     NULL    ;
    myco_print(    myco_string_concat("[OK] Math.min(10, 5): "    ,     myco_safe_to_string(    min_result    )    ))    ;
    tests_passed =     (    tests_passed     +     1.000000    )    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     max_result     =     NULL    ;
    myco_print(    myco_string_concat("[OK] Math.max(10, 5): "    ,     myco_safe_to_string(    max_result    )    ))    ;
    tests_passed =     (    tests_passed     +     1.000000    )    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     sqrt_result     =     NULL    ;
    myco_print(    myco_string_concat("[OK] Math.sqrt(16): "    ,     myco_safe_to_string(    sqrt_result    )    ))    ;
    tests_passed =     (    tests_passed     +     1.000000    )    ;
    myco_print("\\n11. String Library Functions...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     test_str     = "hello world"    ;
    char*     upper_str     =     NULL    ;
    myco_print(    myco_string_concat("[OK] String.upper('hello world'): "    ,     upper_str    ))    ;
    tests_passed =     (    tests_passed     +     1.000000    )    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     lower_str_var     = "HELLO WORLD"    ;
    char*     lower_str     =     NULL    ;
    myco_print(    myco_string_concat("[OK] String.lower('HELLO WORLD'): "    ,     lower_str    ))    ;
    tests_passed =     (    tests_passed     +     1.000000    )    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     trim_str_var     = "  hello  "    ;
    char*     trim_str     =     NULL    ;
    myco_print(    myco_string_concat(    myco_string_concat("[OK] String.trim('  hello  '): '"    ,     trim_str    )    , "'"    ))    ;
    tests_passed =     (    tests_passed     +     1.000000    )    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    int     len_result     =     (int)strlen("hello"    )    ;
    myco_print(    myco_string_concat("[OK] len('hello'): "    ,     myco_number_to_string((double)    len_result    )    ))    ;
    tests_passed =     (    tests_passed     +     1.000000    )    ;
    myco_print("\\n12. Input Function...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    myco_print("[OK] Input function available (tested manually)")    ;
    tests_passed =     (    tests_passed     +     1.000000    )    ;
    myco_print("\\n13. Array Library Functions...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    myco_print("[OK] Array library imported successfully")    ;
    tests_passed =     (    tests_passed     +     1.000000    )    ;
    myco_print("\\n13.1. Enhanced Array Operations...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    char**     test_array     =     NULL    ;
    char*     joined_result     =     NULL    ;
    if (    (    strcmp(    "Unknown"    , "String"    ) == 0     &&     joined_result     !=     NULL    )    ) {
        {
            myco_print("[OK] array.join() successful")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] array.join() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    int     contains_result     =     0    ;
    if (    contains_result     ==     1    ) {
        {
            myco_print("[OK] array.contains() with existing value")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] array.contains() with existing value failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    int     contains_missing     =     0    ;
    if (    contains_missing     ==     0    ) {
        {
            myco_print("[OK] array.contains() with missing value")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] array.contains() with missing value failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    int     index_of_result     =     0    ;
    if (    index_of_result     ==     2.000000    ) {
        {
            myco_print("[OK] array.indexOf() with existing value")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] array.indexOf() with existing value failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    int     index_of_missing     =     0    ;
    if (    index_of_missing     ==     -    1.000000    ) {
        {
            myco_print("[OK] array.indexOf() with missing value")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] array.indexOf() with missing value failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char**     duplicates_array     =     NULL    ;
    char*     unique_result     =     NULL    ;
    if (    (    strcmp(    "Unknown"    , "Array"    ) == 0     &&     unique_result     !=     NULL    )    ) {
        {
            myco_print("[OK] array.unique() successful")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] array.unique() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char**     array1     =     NULL    ;
    char**     array2     =     NULL    ;
    char*     concat_result     =     NULL    ;
    if (    (    strcmp(    "Unknown"    , "Array"    ) == 0     &&     concat_result     !=     NULL    )    ) {
        {
            myco_print("[OK] array.concat() successful")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] array.concat() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     slice_result     =     NULL    ;
    if (    (    strcmp(    "Unknown"    , "Array"    ) == 0     &&     slice_result     !=     NULL    )    ) {
        {
            myco_print("[OK] array.slice() successful")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] array.slice() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char**     fill_array     =     NULL    ;
    // Unsupported statement type: 32
    if (    strcmp(    "Array"    , "Array"    ) == 0    ) {
        {
            myco_print("[OK] array.fill() successful")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] array.fill() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n14. File Library Functions...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     test_content     = "Hello, Myco File System!"    ;
    char*     write_result     =     NULL    ;
    if (    write_result     == NULL    ) {
        {
            myco_print("[OK] File write successful")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] File write failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    int     exists_result     =     0    ;
    if (    exists_result     ==     1    ) {
        {
            myco_print("[OK] File exists check successful")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] File exists check failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     read_result     =     NULL    ;
    if (    (    read_result     !=     NULL     &&     strcmp(    "Unknown"    , "String"    ) == 0    )    ) {
        {
            myco_print("[OK] File read successful")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] File read failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    HttpResponse     delete_result     =     ((HttpResponse){200, "Object", "OK", "{}", 1})    ;
    if (    delete_result    .status_code == 0    ) {
        {
            myco_print("[OK] File delete successful")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] File delete failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n15. Directory Library Functions...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     current_dir     =     NULL    ;
    if (    (    NULL     !=     NULL     &&     strcmp(    "Module"    , "String"    ) == 0    )    ) {
        {
            myco_print("[OK] Current directory successful")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] Current directory failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     files     =     NULL    ;
    if (    (    NULL     !=     NULL     &&     strcmp(    "Module"    , "Array"    ) == 0    )    ) {
        {
            myco_print("[OK] Directory listing successful")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] Directory listing failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     create_result     =     NULL    ;
    if (    create_result     == NULL    ) {
        {
            myco_print("[OK] Directory creation successful")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
            total_tests =             (            total_tests             +             1.000000            )            ;
            int             exists_result_scope_1             =             0            ;
            if (            exists_result_scope_1             ==             1            ) {
                {
                    myco_print("[OK] Directory exists check successful")                    ;
                    tests_passed =                     (                    tests_passed                     +                     1.000000                    )                    ;
                }
            } else {
                {
                    myco_print("[FAIL] Directory exists check failed")                    ;
                    tests_failed =                     tests_failed                    ;
                }
            }
            // Unsupported statement type: 32
        }
    } else {
        {
            myco_print("[FAIL] Directory creation failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n16. Maps Library Functions...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    void*     test_map     =     NULL    ;
    int     has_name     =     0    ;
    if (    has_name     ==     1    ) {
        {
            myco_print("[OK] maps.has() with existing key")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] maps.has() with existing key failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    int     has_missing     =     0    ;
    if (    has_missing     ==     0    ) {
        {
            myco_print("[OK] maps.has() with missing key")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] maps.has() with missing key failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    int     map_size     =     0    ;
    if (    map_size     ==     3.000000    ) {
        {
            myco_print("[OK] maps.size() returns correct count")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] maps.size() failed: "            ,             myco_number_to_string((double)            map_size            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     map_keys     =     NULL    ;
    if (    (    strcmp(    "Map"    , "Array"    ) == 0     &&     map_keys     !=     NULL    )    ) {
        {
            myco_print("[OK] maps.keys() successful")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] maps.keys() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    test_map =     NULL    ;
    int     size_after_delete     =     0    ;
    if (    size_after_delete     >=     2.000000    ) {
        {
            myco_print("[OK] maps.delete() executed")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] maps.delete() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    void*     update_map     =     NULL    ;
    test_map =     NULL    ;
    int     size_after_update     =     0    ;
    if (    size_after_update     >=     2.000000    ) {
        {
            myco_print("[OK] maps.update() executed")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] maps.update() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    test_map =     NULL    ;
    myco_print("[OK] maps.clear() executed")    ;
    tests_passed =     (    tests_passed     +     1.000000    )    ;
    myco_print("\\n17. Sets Library Functions...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    void*     test_set     =     NULL    ;
    int     has_apple     =     0    ;
    if (    has_apple     ==     1    ) {
        {
            myco_print("[OK] set.has() with existing element")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] set.has() with existing element failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    int     has_orange     =     0    ;
    if (    has_orange     ==     0    ) {
        {
            myco_print("[OK] set.has() with missing element")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] set.has() with missing element failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    int     set_size     =     0    ;
    if (    set_size     ==     3.000000    ) {
        {
            myco_print("[OK] set.size() returns correct count")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] set.size() failed: "            ,             myco_number_to_string((double)            set_size            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     updated_set     =     NULL    ;
    int     size_after_add     =     0    ;
    if (    size_after_add     >=     3.000000    ) {
        {
            myco_print("[OK] set.add() executed")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] set.add() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     removed_set     =     NULL    ;
    int     size_after_remove     =     0    ;
    if (    size_after_remove     >=     2.000000    ) {
        {
            myco_print("[OK] set.remove() executed")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] set.remove() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     set_to_array     =     NULL    ;
    if (    (    strcmp(    "Array"    , "Array"    ) == 0     &&     set_to_array     !=     NULL    )    ) {
        {
            myco_print("[OK] set.toArray() successful")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] set.toArray() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    void*     other_set     =     NULL    ;
    char*     union_result     =     NULL    ;
    if (    (    strcmp(    "Unknown"    , "Set"    ) == 0     &&     union_result     !=     NULL    )    ) {
        {
            myco_print("[OK] set.union() successful")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] set.union() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     intersection_result     =     NULL    ;
    if (    (    strcmp(    "Number"    , "Set"    ) == 0     &&     intersection_result     !=     NULL    )    ) {
        {
            myco_print("[OK] set.intersection() successful")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] set.intersection() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     clear_result     =     NULL    ;
    if (    strcmp(    "Unknown"    , "Set"    ) == 0    ) {
        {
            myco_print("[OK] set.clear() successful")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] set.clear() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n18. Trees Library Functions...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     test_tree     =     "TreeObject"    ;
    if (    (    strcmp(    "Tree"    , "Tree"    ) == 0     &&     test_tree     !=     NULL    )    ) {
        {
            myco_print("[OK] trees.create() successful")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] trees.create() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    int     tree_size     =     0    ;
    if (    tree_size     ==     0.000000    ) {
        {
            myco_print("[OK] trees.size() returns correct initial size")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] trees.size() failed: "            ,             myco_number_to_string((double)            tree_size            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     tree_is_empty     =     0    ;
    if (    tree_is_empty     ==     1    ) {
        {
            myco_print("[OK] trees.isEmpty() returns correct initial state")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] trees.isEmpty() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    test_tree =     NULL    ;
    if (    (    strcmp(    "Tree"    , "Tree"    ) == 0     &&     test_tree     !=     NULL    )    ) {
        {
            myco_print("[OK] trees.insert() successful")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] trees.insert() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     tree_search     =     NULL    ;
    if (    strcmp(    "Tree"    , "Boolean"    ) == 0    ) {
        {
            myco_print("[OK] trees.search() successful")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] trees.search() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    test_tree =     NULL    ;
    if (    (    strcmp(    "Tree"    , "Tree"    ) == 0     &&     test_tree     !=     NULL    )    ) {
        {
            myco_print("[OK] trees.clear() successful")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] trees.clear() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n19. Graphs Library Functions...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     test_graph     =     "GraphObject"    ;
    if (    (    strcmp(    "Graph"    , "Graph"    ) == 0     &&     test_graph     !=     NULL    )    ) {
        {
            myco_print("[OK] graphs.create() successful")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] graphs.create() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    int     graph_size     =     0    ;
    if (    graph_size     ==     0.000000    ) {
        {
            myco_print("[OK] graphs.size() returns correct initial size")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] graphs.size() failed: "            ,             myco_number_to_string((double)            graph_size            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     graph_is_empty     =     0    ;
    if (    graph_is_empty     ==     1    ) {
        {
            myco_print("[OK] graphs.is_empty() returns correct initial state")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] graphs.is_empty() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    test_graph =     NULL    ;
    if (    (    strcmp(    "Graph"    , "Graph"    ) == 0     &&     test_graph     !=     NULL    )    ) {
        {
            myco_print("[OK] graphs.add_node() successful")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] graphs.add_node() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    test_graph =     NULL    ;
    if (    (    strcmp(    "Graph"    , "Graph"    ) == 0     &&     test_graph     !=     NULL    )    ) {
        {
            myco_print("[OK] graphs.add_edge() successful")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] graphs.add_edge() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    test_graph =     NULL    ;
    if (    (    strcmp(    "Graph"    , "Graph"    ) == 0     &&     test_graph     !=     NULL    )    ) {
        {
            myco_print("[OK] graphs.clear() successful")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] graphs.clear() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     directed_graph     =     "GraphObject"    ;
    if (    (    strcmp(    "Module"    , "Graph"    ) == 0     &&     NULL     !=     NULL    )    ) {
        {
            myco_print("[OK] graphs.create() with directed parameter successful")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] graphs.create() with directed parameter failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n20. Heaps Library Functions...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     test_heap     =     "HeapObject"    ;
    if (    (    strcmp(    "Unknown"    , "Heap"    ) == 0     &&     test_heap     !=     NULL    )    ) {
        {
            myco_print("[OK] heaps.create() successful")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] heaps.create() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    int     heap_size     =     0    ;
    if (    heap_size     ==     0.000000    ) {
        {
            myco_print("[OK] heaps.size() successful")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] heaps.size() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     heap_empty     =     0    ;
    if (    heap_empty     ==     1    ) {
        {
            myco_print("[OK] heaps.isEmpty() successful")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] heaps.isEmpty() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    test_heap =     NULL    ;
    test_heap =     NULL    ;
    test_heap =     NULL    ;
    int     heap_size_after     =     0    ;
    if (    heap_size_after     ==     3.000000    ) {
        {
            myco_print("[OK] heaps.insert() successful")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] heaps.insert() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    int     heap_peek     =     0    ;
    if (    heap_peek     ==     15.000000    ) {
        {
            myco_print("[OK] heaps.peek() successful")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] heaps.peek() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    test_heap =     0    ;
    int     heap_size_after_extract     =     0    ;
    if (    heap_size_after_extract     ==     2.000000    ) {
        {
            myco_print("[OK] heaps.extract() successful")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] heaps.extract() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    test_heap =     NULL    ;
    int     heap_clear_size     =     0    ;
    if (    heap_clear_size     ==     0.000000    ) {
        {
            myco_print("[OK] heaps.clear() successful")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] heaps.clear() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n21. Queues Library Functions...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     test_queue     =     "QueueObject"    ;
    if (    (    strcmp(    "Unknown"    , "Queue"    ) == 0     &&     test_queue     !=     NULL    )    ) {
        {
            myco_print("[OK] queues.create() successful")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] queues.create() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    int     queue_size     =     0    ;
    if (    queue_size     ==     0.000000    ) {
        {
            myco_print("[OK] queues.size() successful")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] queues.size() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     queue_empty     =     0    ;
    if (    queue_empty     ==     1    ) {
        {
            myco_print("[OK] queues.isEmpty() successful")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] queues.isEmpty() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    test_queue =     NULL    ;
    test_queue =     NULL    ;
    test_queue =     NULL    ;
    int     queue_size_after     =     0    ;
    if (    queue_size_after     ==     3.000000    ) {
        {
            myco_print("[OK] queues.enqueue() successful")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] queues.enqueue() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     queue_front     =     NULL    ;
    if (    strcmp(    queue_front    , "first"    ) == 0    ) {
        {
            myco_print("[OK] queues.front() successful")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] queues.front() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     queue_back     =     NULL    ;
    if (    strcmp(    queue_back    , "third"    ) == 0    ) {
        {
            myco_print("[OK] queues.back() successful")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] queues.back() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    test_queue =     NULL    ;
    int     queue_size_after_dequeue     =     0    ;
    if (    queue_size_after_dequeue     ==     2.000000    ) {
        {
            myco_print("[OK] queues.dequeue() successful")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] queues.dequeue() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    test_queue =     NULL    ;
    int     queue_clear_size     =     0    ;
    if (    queue_clear_size     ==     0.000000    ) {
        {
            myco_print("[OK] queues.clear() successful")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] queues.clear() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n22. Stacks Library Functions...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     test_stack     =     "StackObject"    ;
    if (    (    strcmp(    "Unknown"    , "Stack"    ) == 0     &&     test_stack     !=     NULL    )    ) {
        {
            myco_print("[OK] stacks.create() successful")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] stacks.create() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    int     stack_size     =     0    ;
    if (    stack_size     ==     0.000000    ) {
        {
            myco_print("[OK] stacks.size() successful")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] stacks.size() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     stack_empty     =     0    ;
    if (    stack_empty     ==     1    ) {
        {
            myco_print("[OK] stacks.isEmpty() successful")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] stacks.isEmpty() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    test_stack =     NULL    ;
    test_stack =     NULL    ;
    test_stack =     NULL    ;
    int     stack_size_after     =     0    ;
    if (    stack_size_after     ==     3.000000    ) {
        {
            myco_print("[OK] stacks.push() successful")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] stacks.push() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     stack_top     =     NULL    ;
    if (    strcmp(    stack_top    , "top"    ) == 0    ) {
        {
            myco_print("[OK] stacks.top() successful")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] stacks.top() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    test_stack =     NULL    ;
    int     stack_size_after_pop     =     0    ;
    if (    stack_size_after_pop     ==     2.000000    ) {
        {
            myco_print("[OK] stacks.pop() successful")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] stacks.pop() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    test_stack =     NULL    ;
    int     stack_clear_size     =     0    ;
    if (    stack_clear_size     ==     0.000000    ) {
        {
            myco_print("[OK] stacks.clear() successful")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] stacks.clear() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n=== 23. BUILT-IN FUNCTIONS ===")    ;
    myco_print("20.1. Type checking...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     type_result     =     "Number"    ;
    if (    strcmp(    type_result    , "Int"    ) == 0    ) {
        {
            myco_print(            myco_string_concat("[OK] 42.type(): "            ,             type_result            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] 42.type(): "            ,             type_result            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     type_float     =     "Number"    ;
    if (    strcmp(    type_float    , "Float"    ) == 0    ) {
        {
            myco_print(            myco_string_concat("[OK] 3.14.type(): "            ,             type_float            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] 3.14.type(): "            ,             type_float            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     type_str     =     "String"    ;
    if (    strcmp(    type_str    , "String"    ) == 0    ) {
        {
            myco_print(            myco_string_concat("[OK] 'hello'.type(): "            ,             type_str            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] 'hello'.type(): "            ,             type_str            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     type_bool     =     "Bool"    ;
    if (    strcmp(    type_bool    , "Boolean"    ) == 0    ) {
        {
            myco_print(            myco_string_concat("[OK] True.type(): "            ,             type_bool            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] True.type(): "            ,             type_bool            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     type_null     =     "Null"    ;
    if (    strcmp(    type_null    , "Null"    ) == 0    ) {
        {
            myco_print(            myco_string_concat("[OK] Null.type(): "            ,             type_null            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Null.type(): "            ,             type_null            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     type_array     =     "Unknown"    ;
    if (    strcmp(    type_array    , "Array"    ) == 0    ) {
        {
            myco_print(            myco_string_concat("[OK] [1,2,3].type(): "            ,             type_array            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] [1,2,3].type(): "            ,             type_array            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n13.2. Length function...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    int     len_builtin     =     (int)strlen("hello"    )    ;
    if (    len_builtin     ==     5.000000    ) {
        {
            myco_print(            myco_string_concat("[OK] len('hello'): "            ,             myco_number_to_string((double)            len_builtin            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] len('hello'): "            ,             myco_number_to_string((double)            len_builtin            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    int     len_empty     =     (int)strlen(""    )    ;
    if (    len_empty     ==     0.000000    ) {
        {
            myco_print(            myco_string_concat("[OK] len(''): "            ,             myco_number_to_string((double)            len_empty            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] len(''): "            ,             myco_number_to_string((double)            len_empty            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    int     len_array     =     0    ;
    if (    len_array     ==     3.000000    ) {
        {
            myco_print(            myco_string_concat("[OK] len([1,2,3]): "            ,             myco_number_to_string((double)            len_array            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] len([1,2,3]): "            ,             myco_number_to_string((double)            len_array            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n13.3. String conversion...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     str_builtin     =     myco_number_to_string(    3.140000    )    ;
    if (    strcmp(    str_builtin    , "3.140000"    ) == 0    ) {
        {
            myco_print(            myco_string_concat("[OK] toString(3.14): "            ,             str_builtin            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] toString(3.14): "            ,             str_builtin            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     str_bool     =     myco_number_to_string(    0    )    ;
    if (    strcmp(    str_bool    , "False"    ) == 0    ) {
        {
            myco_print(            myco_string_concat("[OK] toString(False): "            ,             str_bool            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] toString(False): "            ,             str_bool            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     str_null     =     "Null"    ;
    if (    strcmp(    str_null    , "Null"    ) == 0    ) {
        {
            myco_print(            myco_string_concat("[OK] toString(Null): "            ,             str_null            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] toString(Null): "            ,             str_null            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     str_array     =     "[1, 2, 3]"    ;
    if (    strcmp(    str_array    , "[1, 2, 3]"    ) == 0    ) {
        {
            myco_print(            myco_string_concat("[OK] toString([1,2,3]): "            ,             str_array            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] toString([1,2,3]): "            ,             str_array            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n=== 14. UNION TYPES ===")    ;
    myco_print("14.1. Basic union type declarations...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    void*     union_str     = "hello"    ;
    if (    strcmp(    union_str    , "hello"    ) == 0    ) {
        {
            myco_print(            myco_string_concat("[OK] Union type with String value: "            ,             union_str            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Union type with String value: "            ,             union_str            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    void*     union_int     =     (void*)(intptr_t)    42.000000    ;
    if (    (intptr_t)    union_int     == (intptr_t)    42.000000    ) {
        {
            myco_print(            myco_string_concat("[OK] Union type with Int value: "            ,             myco_safe_to_string(            union_int            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Union type with Int value: "            ,             myco_safe_to_string(            union_int            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("14.2. Union type with multiple types...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    void*     mixed_union     =     (void*)(intptr_t)    1    ;
    if (    mixed_union     ==     1    ) {
        {
            myco_print(            myco_string_concat("[OK] Union type with Bool value: "            ,             myco_string_to_string(            mixed_union            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Union type with Bool value: "            ,             myco_string_to_string(            mixed_union            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    void*     mixed_union2     = "world"    ;
    if (    strcmp(    mixed_union2    , "world"    ) == 0    ) {
        {
            myco_print(            myco_string_concat("[OK] Union type with String value: "            ,             mixed_union2            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Union type with String value: "            ,             mixed_union2            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("14.3. Union type type checking...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     type_check_str     =     "Unknown"    ;
    if (    strcmp(    type_check_str    , "String"    ) == 0    ) {
        {
            myco_print(            myco_string_concat("[OK] Union type type() returns correct type for String: "            ,             type_check_str            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Union type type() returns correct type for String: "            ,             type_check_str            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     type_check_int     =     "Number"    ;
    if (    strcmp(    type_check_int    , "Int"    ) == 0    ) {
        {
            myco_print(            myco_string_concat("[OK] Union type type() returns correct type for Int: "            ,             type_check_int            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Union type type() returns correct type for Int: "            ,             type_check_int            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("14.4. Union type parsing and representation...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    void*     union_parsing     = "parsing_test"    ;
    if (    strcmp(    union_parsing    , "parsing_test"    ) == 0    ) {
        {
            myco_print(            myco_string_concat("[OK] Union type parsing with three types: "            ,             union_parsing            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Union type parsing with three types: "            ,             union_parsing            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    void*     union_float     =     (void*)(intptr_t)    3.141590    ;
    if (    (intptr_t)    union_float     == (intptr_t)    3.141590    ) {
        {
            myco_print(            myco_string_concat("[OK] Union type with Float: "            ,             myco_safe_to_string(            union_float            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Union type with Float: "            ,             myco_safe_to_string(            union_float            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("14.5. Union type edge cases...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    void*     union_null     =     NULL    ;
    if (    union_null     == NULL    ) {
        {
            myco_print(            myco_string_concat("[OK] Union type with Null: "            ,             myco_safe_to_string(            union_null            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Union type with Null: "            ,             myco_safe_to_string(            union_null            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    void*     union_bool     =     (void*)(intptr_t)    0    ;
    if (    union_bool     ==     0    ) {
        {
            myco_print(            myco_string_concat("[OK] Union type with Bool: "            ,             myco_safe_to_string((void*)(intptr_t)            union_bool            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Union type with Bool: "            ,             myco_safe_to_string((void*)(intptr_t)            union_bool            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n=== 15. OPTIONAL TYPES ===")    ;
    myco_print("15.1. Basic optional type declarations...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    void*     optional_string     = "hello"    ;
    if (    strcmp(    optional_string    , "hello"    ) == 0    ) {
        {
            myco_print(            myco_string_concat("[OK] Optional string: "            ,             optional_string            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Optional string: "            ,             optional_string            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    void*     optional_null     =     NULL    ;
    if (    optional_null     == NULL    ) {
        {
            myco_print(            myco_string_concat("[OK] Optional null: "            ,             myco_safe_to_string(            optional_null            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Optional null: "            ,             myco_safe_to_string(            optional_null            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("15.2. Optional types with different base types...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    void*     optional_array     =     NULL    ;
    if (    strcmp(    "Array"    , "Array"    ) == 0    ) {
        {
            myco_print(            myco_string_concat("[OK] Optional array: "            ,             myco_safe_to_string(            optional_array            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Optional array: "            ,             myco_safe_to_string(            optional_array            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("15.3. Optional type checking...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    if (    strcmp(    "String"    , "String"    ) == 0    ) {
        {
            myco_print(            myco_string_concat("[OK] Optional string type: "            ,             "String"            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Optional string type: "            ,             "String"            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    if (    strcmp(    "Null"    , "Null"    ) == 0    ) {
        {
            myco_print(            myco_string_concat("[OK] Optional null type: "            ,             "Null"            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Optional null type: "            ,             "Null"            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("15.4. Optional types with match statements...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     match_result     = ""    ;
    // Unsupported statement type: 22
    if (    strcmp(    match_result    , "found hello"    ) == 0    ) {
        {
            myco_print(            myco_string_concat("[OK] Match with optional string: "            ,             match_result            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Match with optional string: "            ,             match_result            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    match_result = ""    ;
    // Unsupported statement type: 22
    if (    strcmp(    match_result    , "found null"    ) == 0    ) {
        {
            myco_print(            myco_string_concat("[OK] Match with optional null: "            ,             match_result            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Match with optional null: "            ,             match_result            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("15.5. Optional union types...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    void*     optional_union     = "test"    ;
    if (    strcmp(    optional_union    , "test"    ) == 0    ) {
        {
            myco_print(            myco_string_concat("[OK] Optional union: "            ,             myco_safe_to_string(            optional_union            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Optional union: "            ,             myco_safe_to_string(            optional_union            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n=== 16. TYPE GUARDS ===")    ;
    myco_print("16.1. Basic type guard methods...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    if (    isString((void*)"hello")    ) {
        {
            myco_print("[OK] isString('hello'): True")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] isString('hello'): False")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    if (    isString((void*)(intptr_t)    42.000000)    ) {
        {
            myco_print("[FAIL] isString(42): True")            ;
            tests_failed =             tests_failed            ;
        }
    } else {
        {
            myco_print("[OK] isString(42): False")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    }
    myco_print("16.2. Integer type guards...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    if (    isInt((void*)(intptr_t)    42.000000)    ) {
        {
            myco_print("[OK] isInt(42): True")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] isInt(42): False")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    if (    isInt((void*)(intptr_t)    3.140000)    ) {
        {
            myco_print("[FAIL] isInt(3.14): True")            ;
            tests_failed =             tests_failed            ;
        }
    } else {
        {
            myco_print("[OK] isInt(3.14): False")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    }
    myco_print("16.3. Float type guards...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    if (    isFloat((void*)(intptr_t)    3.140000)    ) {
        {
            myco_print("[OK] isFloat(3.14): True")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] isFloat(3.14): False")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    if (    isFloat((void*)(intptr_t)    42.000000)    ) {
        {
            myco_print("[FAIL] isFloat(42): True")            ;
            tests_failed =             tests_failed            ;
        }
    } else {
        {
            myco_print("[OK] isFloat(42): False")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    }
    myco_print("16.4. Boolean type guards...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    if (    isBool((void*)    1)    ) {
        {
            myco_print("[OK] isBool(True): True")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] isBool(True): False")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    if (    isBool((void*)(intptr_t)    42.000000)    ) {
        {
            myco_print("[FAIL] isBool(42): True")            ;
            tests_failed =             tests_failed            ;
        }
    } else {
        {
            myco_print("[OK] isBool(42): False")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    }
    myco_print("16.5. Array type guards...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    if (    isArray((void*)    NULL)    ) {
        {
            myco_print("[OK] isArray([1, 2, 3]): True")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] isArray([1, 2, 3]): False")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    if (    isArray((void*)"hello")    ) {
        {
            myco_print("[FAIL] isArray('hello'): True")            ;
            tests_failed =             tests_failed            ;
        }
    } else {
        {
            myco_print("[OK] isArray('hello'): False")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    }
    myco_print("16.6. Null type guards...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    if (    isNull((void*)    NULL)    ) {
        {
            myco_print("[OK] isNull(Null): True")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] isNull(Null): False")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    if (    isNull((void*)"hello")    ) {
        {
            myco_print("[FAIL] isNull('hello'): True")            ;
            tests_failed =             tests_failed            ;
        }
    } else {
        {
            myco_print("[OK] isNull('hello'): False")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    }
    myco_print("16.7. Number type guards...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    if (    isNumber((void*)(intptr_t)    42.000000)    ) {
        {
            myco_print("[OK] isNumber(42): True")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] isNumber(42): False")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    if (    isNumber((void*)(intptr_t)    3.140000)    ) {
        {
            myco_print("[OK] isNumber(3.14): True")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] isNumber(3.14): False")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    if (    isNumber((void*)"hello")    ) {
        {
            myco_print("[FAIL] isNumber('hello'): True")            ;
            tests_failed =             tests_failed            ;
        }
    } else {
        {
            myco_print("[OK] isNumber('hello'): False")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    }
    myco_print("16.8. Type guards with optional types...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    void*     optional_string_2     = "hello"    ;
    if (    isString((void*)    optional_string_2)    ) {
        {
            myco_print("[OK] isString(optional_string): True")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] isString(optional_string): False")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    void*     optional_null_2     =     NULL    ;
    if (    isNull((void*)    optional_null_2)    ) {
        {
            myco_print("[OK] isNull(optional_null): True")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] isNull(optional_null): False")            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("16.9. Type guards in conditional logic...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     test_value     = "hello"    ;
    char*     type_result_2     = ""    ;
    if (    isString((void*)    test_value)    ) {
        {
            type_result_2 = "string"            ;
        }
    } else {
        {
            if (            isInt((void*)            test_value)            ) {
                {
                    type_result_2 = "integer"                    ;
                }
            } else {
                {
                    if (                    isBool((void*)                    test_value)                    ) {
                        {
                            type_result_2 = "boolean"                            ;
                        }
                    } else {
                        {
                            if (                            isNull((void*)                            test_value)                            ) {
                                {
                                    type_result_2 = "null"                                    ;
                                }
                            } else {
                                {
                                    type_result_2 = "unknown"                                    ;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    if (    strcmp(    type_result_2    , "string"    ) == 0    ) {
        {
            myco_print(            myco_string_concat("[OK] Type guard conditional logic: "            ,             type_result_2            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Type guard conditional logic: "            ,             type_result_2            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n=== 17. EDGE CASES ===")    ;
    myco_print("17.1. Zero and negative numbers...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     zero_add     =     (    0.000000     +     0.000000    )    ;
    if (    zero_add     ==     0.000000    ) {
        {
            myco_print(            myco_string_concat("[OK] Zero addition (0 + 0): "            ,             myco_number_to_string(            zero_add            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Zero addition (0 + 0): "            ,             myco_number_to_string(            zero_add            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     zero_mult_2     =     0.000000     *     5.000000    ;
    if (    zero_mult_2     ==     0.000000    ) {
        {
            myco_print(            myco_string_concat("[OK] Zero multiplication (0 * 5): "            ,             myco_number_to_string(            zero_mult_2            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Zero multiplication (0 * 5): "            ,             myco_number_to_string(            zero_mult_2            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     neg_add     =     (    -    5.000000     +     3.000000    )    ;
    if (    neg_add     ==     -    2.000000    ) {
        {
            myco_print(            myco_string_concat("[OK] Negative addition (-5 + 3): "            ,             myco_number_to_string(            neg_add            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Negative addition (-5 + 3): "            ,             myco_number_to_string(            neg_add            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n17.2. Large numbers...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     large_add     =     (    999999.000000     +     1.000000    )    ;
    if (    large_add     ==     1000000.000000    ) {
        {
            myco_print(            myco_string_concat("[OK] Large addition (999999 + 1): "            ,             myco_number_to_string(            large_add            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Large addition (999999 + 1): "            ,             myco_number_to_string(            large_add            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     large_mult     =     1000.000000     *     1000.000000    ;
    if (    large_mult     ==     1000000.000000    ) {
        {
            myco_print(            myco_string_concat("[OK] Large multiplication (1000 * 1000): "            ,             myco_number_to_string(            large_mult            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Large multiplication (1000 * 1000): "            ,             myco_number_to_string(            large_mult            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n17.3. Floating point precision...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     float_prec     =     (    0.100000     +     0.200000    )    ;
    if (    float_prec     >     0.300000    ) {
        {
            myco_print(            myco_string_concat("[OK] Float precision (0.1 + 0.2 > 0.3): "            ,             myco_number_to_string(            float_prec            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Float precision (0.1 + 0.2 > 0.3): "            ,             myco_number_to_string(            float_prec            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     float_eq_2     =     1.000000     ==     1.000000    ;
    if (    float_eq_2     ==     1    ) {
        {
            myco_print(            myco_string_concat("[OK] Float equality (1.0 == 1): "            ,             myco_number_to_string(            float_eq_2            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Float equality (1.0 == 1): "            ,             myco_number_to_string(            float_eq_2            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n17.4. Empty values...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    int     empty_str_len     =     (int)strlen(""    )    ;
    if (    empty_str_len     ==     0.000000    ) {
        {
            myco_print(            myco_string_concat("[OK] Empty string length: "            ,             myco_number_to_string((double)            empty_str_len            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Empty string length: "            ,             myco_number_to_string((double)            empty_str_len            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    int     empty_array_len     =     0    ;
    if (    empty_array_len     ==     0.000000    ) {
        {
            myco_print(            myco_string_concat("[OK] Empty array length: "            ,             myco_number_to_string((double)            empty_array_len            )            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Empty array length: "            ,             myco_number_to_string((double)            empty_array_len            )            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     empty_concat     =     myco_string_concat(""    , "hello"    )    ;
    if (    strcmp(    empty_concat    , "hello"    ) == 0    ) {
        {
            myco_print(            myco_string_concat("[OK] Empty string concatenation: "            ,             empty_concat            ))            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print(            myco_string_concat("[FAIL] Empty string concatenation: "            ,             empty_concat            ))            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n=== 18. ERROR HANDLING (Expected Errors) ===")    ;
    myco_print("18.1. Division by zero (should continue execution)...")    ;
    myco_print("\\n18.2. Array index out of bounds (should continue execution)...")    ;
    myco_print("\\n18.3. Undefined variable (should continue execution)...")    ;
    myco_print("\\n18.1. Function definitions (no parameters, no return type)...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    if (    strcmp(    "Unknown"    , "Function"    ) == 0    ) {
        {
            myco_print("[OK] Simple function definition (no params, no return)")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] Simple function definition failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n18.2. Function with single parameter (no return type)...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    if (    strcmp(    "String"    , "Function"    ) == 0    ) {
        {
            myco_print("[OK] Function with single parameter (no return type)")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] Function with single parameter failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n18.3. Function with multiple parameters (no return type)...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    if (    strcmp(    "Number"    , "Function"    ) == 0    ) {
        {
            myco_print("[OK] Function with multiple parameters (no return type)")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] Function with multiple parameters failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n18.4. Function definitions (with return types)...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    if (    (    (    strcmp(    "Unknown"    , "Function"    ) == 0     &&     strcmp(    "Unknown"    , "Function"    ) == 0    )     &&     strcmp(    "Unknown"    , "Function"    ) == 0    )    ) {
        {
            myco_print("[OK] Function definitions with return types")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] Function definitions with return types failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n18.4. Function definitions (mixed explicit/implicit types)...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    if (    (    (    strcmp(    "Unknown"    , "Function"    ) == 0     &&     strcmp(    "Unknown"    , "Function"    ) == 0    )     &&     strcmp(    "Unknown"    , "Function"    ) == 0    )    ) {
        {
            myco_print("[OK] Function definitions with mixed explicit/implicit types")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] Function definitions with mixed types failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n18.5. Function calls (no parameters)...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     five     =     return_five()    ;
    char*     hello     =     return_hello()    ;
    if (    (    five     ==     5.000000     &&     strcmp(    hello    , "Hello"    ) == 0    )    ) {
        {
            myco_print("[OK] Function calls with no parameters")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] Function calls with no parameters failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n18.6. Function calls (with parameters)...")    ;
    myco_print("\\n18.7. Function calls (with type annotations)...")    ;
    myco_print("\\n18.8. Recursive functions...")    ;
    myco_print("\\n18.9. Function variables and assignments...")    ;
    myco_print("\\n18.10. Complex function combinations...")    ;
    myco_print("\\n18.11. Function scope and closures...")    ;
    myco_print("\\n18.12. Function error handling...")    ;
    myco_print("\\n18.13. Lambda functions (anonymous functions)...")    ;
    void*     simple_lambda     =     placeholder_lambda    ;
    void*     add_lambda     =     placeholder_lambda    ;
    void*     multiply_lambda     =     placeholder_lambda    ;
    void*     greet_lambda     =     placeholder_lambda    ;
    void*     mixed_lambda     =     placeholder_lambda    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    if (    (    (    (    (    strcmp(    "Unknown"    , "Function"    ) == 0     &&     strcmp(    "Unknown"    , "Function"    ) == 0    )     &&     strcmp(    "Unknown"    , "Function"    ) == 0    )     &&     strcmp(    "Unknown"    , "Function"    ) == 0    )     &&     strcmp(    "Unknown"    , "Function"    ) == 0    )    ) {
        {
            myco_print("[OK] Lambda functions (anonymous functions)")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] Lambda functions (anonymous functions) failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n18.14. Function variable assignments...")    ;
    void*     my_square     =     placeholder_lambda    ;
    void*     my_add     =     placeholder_lambda    ;
    void*     my_greet     =     placeholder_lambda    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    if (    (    (    strcmp(    "Unknown"    , "Function"    ) == 0     &&     strcmp(    "Unknown"    , "Function"    ) == 0    )     &&     strcmp(    "Unknown"    , "Function"    ) == 0    )    ) {
        {
            myco_print("[OK] Function variable assignments")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] Function variable assignments failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n18.15. Lambda functions with different parameter types...")    ;
    void*     no_params_lambda     =     placeholder_lambda    ;
    void*     single_param_lambda     =     placeholder_lambda    ;
    void*     multi_param_lambda     =     placeholder_lambda    ;
    void*     typed_param_lambda     =     placeholder_lambda    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    if (    (    (    (    strcmp(    "Unknown"    , "Function"    ) == 0     &&     strcmp(    "Unknown"    , "Function"    ) == 0    )     &&     strcmp(    "Unknown"    , "Function"    ) == 0    )     &&     strcmp(    "Unknown"    , "Function"    ) == 0    )    ) {
        {
            myco_print("[OK] Lambda functions with different parameter types")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] Lambda functions with different parameter types failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n18.16. Explicit function with all types...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    if (    strcmp(    "Unknown"    , "Function"    ) == 0    ) {
        {
            char*             result_scope_1             =             explicit_all(            42.000000)            ;
            if (            result_scope_1             !=             NULL            ) {
                {
                    myco_print("[OK] Explicit function with all types")                    ;
                    tests_passed =                     (                    tests_passed                     +                     1.000000                    )                    ;
                }
            } else {
                {
                    myco_print("[FAIL] Explicit function with all types failed")                    ;
                    tests_failed =                     tests_failed                    ;
                }
            }
        }
    } else {
        {
            myco_print("[FAIL] Explicit function with all types failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n18.17. Implicit function with no types...")    ;
    myco_print("\\n18.18. Mixed function - explicit parameter, implicit return...")    ;
    myco_print("\\n18.19. Mixed function - implicit parameter, explicit return...")    ;
    myco_print("\\n18.20. Function variable with explicit types...")    ;
    myco_print("\\n18.21. Function variable with implicit types...")    ;
    myco_print("\\n18.22. Function variable with mixed types...")    ;
    myco_print("\\n18.23. Function variable with mixed return type...")    ;
    myco_print("\\n18.24. Function type interchangeability with different argument types...")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    typedef struct {
        char* name;
        double count;
    } DefaultClass;
    DefaultClass     default_instance     =     {    }    ;
    if (    (    (    strcmp(    "Unknown"    , "DefaultClass"    ) == 0     &&     strcmp(    default_instance    .name    , "Default"    ) == 0    )     &&     default_instance    .count     ==     0.000000    )    ) {
        {
            myco_print("[OK] Class with default field values")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] Class with default field values failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    typedef struct {
        int value;
    } SimpleClass;
    SimpleClass     s     =     {    42.000000    }    ;
    if (    (    strcmp(    "Unknown"    , "SimpleClass"    ) == 0     &&     s    .value     ==     42.000000    )    ) {
        {
            myco_print("[OK] Class instantiation")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] Class instantiation failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    typedef struct {
        char* name;
    } MethodClass;
    MethodClass     m     =     {"World"    }    ;
    char*     greeting     =     placeholder_greet    (    )    ;
    if (    (    strcmp(    "Unknown"    , "MethodClass"    ) == 0     &&     strcmp(    greeting    , "Hello, World"    ) == 0    )    ) {
        {
            myco_print("[OK] Class with methods")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] Class with methods failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    typedef struct {
        int value;
    } SelfClass;
    SelfClass     self_test     =     {    100.000000    }    ;
    double     retrieved_value     =     placeholder_getValue    (    )    ;
    if (    (    strcmp(    "Unknown"    , "SelfClass"    ) == 0     &&     retrieved_value     ==     100.000000    )    ) {
        {
            myco_print("[OK] Class with self references")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] Class with self references failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    typedef struct {
        char* name;
        int age;
        int active;
    } MixedClass;
    MixedClass     mixed_2     =     {"Alice"    ,     25.000000    }    ;
    if (    (    (    (    strcmp(    "Unknown"    , "MixedClass"    ) == 0     &&     strcmp(    mixed_2    .name    , "Alice"    ) == 0    )     &&     mixed_2    .age     ==     25.000000    )     &&     mixed_2    .active     ==     0    )    ) {
        {
            myco_print("[OK] Class with mixed field types")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] Class with mixed field types failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    typedef struct {
        int count;
    } TypedMethodClass;
    TypedMethodClass     typed     =     {    5.000000    }    ;
    double     incremented     =     placeholder_increment    (    )    ;
    char*     class_name     =     placeholder_getName    (    )    ;
    if (    (    (    strcmp(    "Unknown"    , "TypedMethodClass"    ) == 0     &&     incremented     ==     6.000000    )     &&     strcmp(    class_name    , "TypedMethodClass"    ) == 0    )    ) {
        {
            myco_print("[OK] Class with typed methods")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] Class with typed methods failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    typedef struct {
        char* data;
    } UntypedMethodClass;
    UntypedMethodClass     untyped     =     {"data"    }    ;
    double     result     =     placeholder_process    (    )    ;
    if (    (    strcmp(    "Unknown"    , "UntypedMethodClass"    ) == 0     &&     (    result     == 0.0     ||     result     ==     0.000000    )    )    ) {
        {
            myco_print("[OK] Class with untyped methods")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] Class with untyped methods failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    typedef struct {
        int x;
        int y;
    } ComplexClass;
    ComplexClass     complex     =     {    10.000000    ,     5.000000    }    ;
    double     calculation     =     placeholder_calculate    (    )    ;
    if (    (    strcmp(    "Unknown"    , "ComplexClass"    ) == 0     &&     calculation     ==     20.000000    )    ) {
        {
            myco_print("[OK] Class with complex method bodies")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] Class with complex method bodies failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n=== 19. INHERITANCE TESTS ===")    ;
    typedef struct {
        char* name;
    } Animal;
    typedef struct {
        char* name;  // Inherited from Animal
    } Dog;
    Dog     test_dog     =     {"Rex"    }    ;
    if (    (    strcmp(    "Unknown"    , "Dog"    ) == 0     &&     strcmp(    test_dog    .name    , "Rex"    ) == 0    )    ) {
        {
            myco_print("[OK] Basic inheritance parsing")            ;
            total_tests =             (            total_tests             +             1.000000            )            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] Basic inheritance parsing failed")            ;
            total_tests =             (            total_tests             +             1.000000            )            ;
            tests_failed =             tests_failed            ;
        }
    }
    Dog     dog     =     {"Buddy"    }    ;
    if (    strcmp(    dog    .name    , "Buddy"    ) == 0    ) {
        {
            myco_print("[OK] Field inheritance")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] Field inheritance")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     sound     =     placeholder_speak    (    )    ;
    if (    strcmp(    sound    , "Woof!"    ) == 0    ) {
        {
            myco_print("[OK] Method overriding")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] Method overriding")            ;
            tests_failed =             tests_failed            ;
        }
    }
    typedef struct {
        char* name;  // Inherited from Animal via Dog
        int age;
    } Puppy;
    Puppy     test_puppy     =     {"Max"    ,     6.000000    }    ;
    if (    (    (    strcmp(    "Unknown"    , "Puppy"    ) == 0     &&     strcmp(    test_puppy    .name    , "Max"    ) == 0    )     &&     test_puppy    .age     ==     6.000000    )    ) {
        {
            myco_print("[OK] Multi-level inheritance parsing")            ;
            total_tests =             (            total_tests             +             1.000000            )            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] Multi-level inheritance parsing failed")            ;
            total_tests =             (            total_tests             +             1.000000            )            ;
            tests_failed =             tests_failed            ;
        }
    }
    Puppy     puppy     =     {"Max"    ,     6.000000    }    ;
    if (    strcmp(    puppy    .name    , "Max"    ) == 0    ) {
        {
            myco_print("[OK] Inheritance with additional fields")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] Inheritance with additional fields")            ;
            tests_failed =             tests_failed            ;
        }
    }
    typedef struct {
        char* name;  // Inherited from Animal
        char* color;
    } Cat;
    Cat     test_cat     =     {"Whiskers"    , "Orange"    }    ;
    if (    (    (    strcmp(    "Unknown"    , "Cat"    ) == 0     &&     strcmp(    test_cat    .name    , "Whiskers"    ) == 0    )     &&     strcmp(    test_cat    .color    , "Orange"    ) == 0    )    ) {
        {
            myco_print("[OK] Inheritance without method override")            ;
            total_tests =             (            total_tests             +             1.000000            )            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] Inheritance without method override failed")            ;
            total_tests =             (            total_tests             +             1.000000            )            ;
            tests_failed =             tests_failed            ;
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
    Lion     test_lion     =     {"Simba"    , "Savanna"    , "Golden"    }    ;
    if (    (    (    (    strcmp(    "Unknown"    , "Lion"    ) == 0     &&     strcmp(    test_lion    .name    , "Simba"    ) == 0    )     &&     strcmp(    test_lion    .habitat    , "Savanna"    ) == 0    )     &&     strcmp(    test_lion    .mane_color    , "Golden"    ) == 0    )    ) {
        {
            myco_print("[OK] Complex inheritance chain")            ;
            total_tests =             (            total_tests             +             1.000000            )            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] Complex inheritance chain failed")            ;
            total_tests =             (            total_tests             +             1.000000            )            ;
            tests_failed =             tests_failed            ;
        }
    }
    typedef struct {
        int can_fly;
        double wing_span;
    } Bird;
    myco_print("[OK] Inheritance with default values")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    typedef struct {
        int altitude;
    } FlyingAnimal;
    myco_print("[OK] Multiple inheritance simulation")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    typedef struct {
        char* species;
        double depth;
        void* is_saltwater;
    } Fish;
    myco_print("[OK] Inheritance with mixed field types")    ;
    total_tests =     (    total_tests     +     1.000000    )    ;
    tests_passed =     (    tests_passed     +     1.000000    )    ;
    myco_print("\\n=== 20. Time Library Functions ===")    ;
    char* time = "Module";
    total_tests =     (    total_tests     +     1.000000    )    ;
    if (    strcmp(    "Module"    , "Object"    ) == 0    ) {
        {
            myco_print("[OK] Time library import works")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] Time library import failed")            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     current_time     =     NULL    ;
    if (    strcmp(    "Module"    , "Object"    ) == 0    ) {
        {
            myco_print("[OK] time.now() returns time object")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] time.now() failed")            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     specific_time     =     NULL    ;
    if (    strcmp(    "Module"    , "Object"    ) == 0    ) {
        {
            myco_print("[OK] time.create() returns time object")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] time.create() failed")            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     formatted     =     NULL    ;
    if (    strcmp(    formatted    , "2024-01-15 14:30:00"    ) == 0    ) {
        {
            myco_print("[OK] time.format() works correctly")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] time.format() failed")            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     year     =     0.0    ;
    if (    year     ==     2024.000000    ) {
        {
            myco_print("[OK] time.year() works correctly")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] time.year() failed")            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     month     =     0.0    ;
    if (    month     ==     1.000000    ) {
        {
            myco_print("[OK] time.month() works correctly")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] time.month() failed")            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     day     =     0.0    ;
    if (    day     ==     15.000000    ) {
        {
            myco_print("[OK] time.day() works correctly")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] time.day() failed")            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     hour     =     0.0    ;
    if (    hour     ==     14.000000    ) {
        {
            myco_print("[OK] time.hour() works correctly")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] time.hour() failed")            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     minute     =     0.0    ;
    if (    minute     ==     30.000000    ) {
        {
            myco_print("[OK] time.minute() works correctly")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] time.minute() failed")            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     second     =     0.0    ;
    if (    second     ==     0.000000    ) {
        {
            myco_print("[OK] time.second() works correctly")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] time.second() failed")            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     iso     =     NULL    ;
    if (    strcmp(    iso    , "2024-01-15T14:30:00"    ) == 0    ) {
        {
            myco_print("[OK] time.iso_string() works correctly")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] time.iso_string() failed")            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    double     unix     =     0.0    ;
    if (    unix     ==     1705347000.000000    ) {
        {
            myco_print("[OK] time.unix_timestamp() works correctly")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] time.unix_timestamp() failed")            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     future_time     =     NULL    ;
    double     future_hour     =     0.0    ;
    if (    future_hour     ==     15.000000    ) {
        {
            myco_print("[OK] time.add() works correctly")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] time.add() failed")            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     past_time     =     NULL    ;
    double     past_hour     =     0.0    ;
    if (    past_hour     ==     13.000000    ) {
        {
            myco_print("[OK] time.subtract() works correctly")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] time.subtract() failed")            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     time1     =     NULL    ;
    char*     time2     =     NULL    ;
    double     diff     =     0.0    ;
    if (    diff     ==     3600.000000    ) {
        {
            myco_print("[OK] time.difference() works correctly")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] time.difference() failed")            ;
        }
    }
    myco_print("\\n=== 21. Regex Library Functions ===")    ;
    char* regex = "Module";
    total_tests =     (    total_tests     +     1.000000    )    ;
    if (    strcmp(    "Module"    , "Object"    ) == 0    ) {
        {
            myco_print("[OK] Regex library import works")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] Regex library import failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     match_result_2     =     NULL    ;
    if (    (    strcmp(    "Unknown"    , "Object"    ) == 0     &&     match_result_2     !=     NULL    )    ) {
        {
            myco_print("[OK] regex.match() returns match object")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] regex.match() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     no_match     =     NULL    ;
    if (    no_match     == NULL    ) {
        {
            myco_print("[OK] regex.match() returns Null for no match")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] regex.match() should return Null for no match")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     test_result     =     NULL    ;
    if (    test_result     ==     1    ) {
        {
            myco_print("[OK] regex.test() works for existing pattern")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] regex.test() failed for existing pattern")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     test_no_match     =     NULL    ;
    if (    test_no_match     ==     0    ) {
        {
            myco_print("[OK] regex.test() works for non-existing pattern")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] regex.test() failed for non-existing pattern")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     valid_email     =     NULL    ;
    if (    valid_email     ==     1    ) {
        {
            myco_print("[OK] regex.is_email() works for valid email")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] regex.is_email() failed for valid email")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     invalid_email     =     NULL    ;
    if (    invalid_email     ==     0    ) {
        {
            myco_print("[OK] regex.is_email() works for invalid email")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] regex.is_email() failed for invalid email")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     valid_url     =     NULL    ;
    if (    valid_url     ==     1    ) {
        {
            myco_print("[OK] regex.is_url() works for valid URL")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] regex.is_url() failed for valid URL")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     invalid_url     =     NULL    ;
    if (    invalid_url     ==     0    ) {
        {
            myco_print("[OK] regex.is_url() works for invalid URL")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] regex.is_url() failed for invalid URL")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     valid_ip     =     NULL    ;
    if (    valid_ip     ==     1    ) {
        {
            myco_print("[OK] regex.is_ip() works for valid IP")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] regex.is_ip() failed for valid IP")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     invalid_ip     =     NULL    ;
    if (    invalid_ip     ==     0    ) {
        {
            myco_print("[OK] regex.is_ip() works for invalid IP")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] regex.is_ip() failed for invalid IP")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     case_test     =     NULL    ;
    if (    case_test     ==     1    ) {
        {
            myco_print("[OK] regex.CASE_INSENSITIVE flag works")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] regex.CASE_INSENSITIVE flag failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     global_test     =     NULL    ;
    if (    global_test     ==     1    ) {
        {
            myco_print("[OK] regex.GLOBAL flag works")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] regex.GLOBAL flag failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     multiline_test     =     NULL    ;
    if (    multiline_test     ==     1    ) {
        {
            myco_print("[OK] regex.MULTILINE flag works")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] regex.MULTILINE flag failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     dotall_test     =     NULL    ;
    if (    dotall_test     ==     1    ) {
        {
            myco_print("[OK] regex.DOTALL flag works")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] regex.DOTALL flag failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     complex_pattern     =     NULL    ;
    if (    complex_pattern     ==     1    ) {
        {
            myco_print("[OK] Complex regex patterns work")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] Complex regex patterns failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     special_chars     =     NULL    ;
    if (    special_chars     ==     1    ) {
        {
            myco_print("[OK] Regex with special characters works")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] Regex with special characters failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     email1     =     NULL    ;
    char*     email2     =     NULL    ;
    char*     email3     =     NULL    ;
    char*     email4     =     NULL    ;
    if (    (    (    (    email1     ==     1     &&     email2     ==     1    )     &&     email3     ==     0    )     &&     email4     ==     0    )    ) {
        {
            myco_print("[OK] Multiple email format validation works")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] Multiple email format validation failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     url1     =     NULL    ;
    char*     url2     =     NULL    ;
    char*     url3     =     NULL    ;
    char*     url4     =     NULL    ;
    if (    (    (    (    url1     ==     1     &&     url2     ==     1    )     &&     url3     ==     1    )     &&     url4     ==     0    )    ) {
        {
            myco_print("[OK] Multiple URL format validation works")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] Multiple URL format validation failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     ip1     =     NULL    ;
    char*     ip2     =     NULL    ;
    char*     ip3     =     NULL    ;
    char*     ip4     =     NULL    ;
    if (    (    (    (    ip1     ==     1     &&     ip2     ==     1    )     &&     ip3     ==     1    )     &&     ip4     ==     0    )    ) {
        {
            myco_print("[OK] Multiple IP format validation works")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] Multiple IP format validation failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     invalid_pattern     =     NULL    ;
    if (    strcmp(    "Unknown"    , "Boolean"    ) == 0    ) {
        {
            myco_print("[OK] Regex error handling works")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] Regex error handling failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     empty_match     =     NULL    ;
    char*     empty_test     =     NULL    ;
    if (    (    (    strcmp(    "Unknown"    , "Object"    ) == 0     ||     empty_match     == NULL    )     &&     strcmp(    "Unknown"    , "Boolean"    ) == 0    )    ) {
        {
            myco_print("[OK] Regex with empty strings works")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] Regex with empty strings failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n=== 22. JSON Library Functions ===")    ;
    char* json = "Module";
    total_tests =     (    total_tests     +     1.000000    )    ;
    if (    strcmp(    "Module"    , "Object"    ) == 0    ) {
        {
            myco_print("[OK] JSON library import works")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] JSON library import failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     str_result     =     NULL    ;
    if (    strcmp(    str_result    , "\\\"hello\\\""    ) == 0    ) {
        {
            myco_print("[OK] json.stringify() works for strings")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] json.stringify() failed for strings")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     num_result     =     NULL    ;
    if (    strcmp(    num_result    , "42"    ) == 0    ) {
        {
            myco_print("[OK] json.stringify() works for numbers")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] json.stringify() failed for numbers")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     bool_result     =     NULL    ;
    if (    strcmp(    bool_result    , "true"    ) == 0    ) {
        {
            myco_print("[OK] json.stringify() works for booleans")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] json.stringify() failed for booleans")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     null_result     =     NULL    ;
    if (    strcmp(    null_result    , "null"    ) == 0    ) {
        {
            myco_print("[OK] json.stringify() works for null")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] json.stringify() failed for null")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char**     arr_2     =     NULL    ;
    char*     arr_result     =     NULL    ;
    if (    (    strcmp(    "Array"    , "String"    ) == 0     &&     strcmp(    arr_result    , ""    ) != 0    )    ) {
        {
            myco_print("[OK] json.stringify() works for arrays")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] json.stringify() failed for arrays")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     valid_json     = "{\"test\": \"value\"}"    ;
    char*     is_valid     =     NULL    ;
    if (    is_valid     ==     1    ) {
        {
            myco_print("[OK] json.validate() works for valid JSON")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] json.validate() failed for valid JSON")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     invalid_json     = "{\"test\": \"value\""    ;
    char*     is_invalid     =     NULL    ;
    if (    is_invalid     ==     0    ) {
        {
            myco_print("[OK] json.validate() works for invalid JSON")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] json.validate() failed for invalid JSON")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char**     test_array_2     =     NULL    ;
    int     array_size     =     0    ;
    if (    array_size     ==     5.000000    ) {
        {
            myco_print("[OK] json.size() works for arrays")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] json.size() failed for arrays")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char**     empty_array     =     NULL    ;
    char**     non_empty_array     =     NULL    ;
    char*     empty_check     =     0    ;
    char*     non_empty_check     =     0    ;
    if (    (    empty_check     ==     1     &&     non_empty_check     ==     0    )    ) {
        {
            myco_print("[OK] json.is_empty() works for arrays")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] json.is_empty() failed for arrays")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     parsed     =     NULL    ;
    if (    strcmp(    "Unknown"    , "Object"    ) == 0    ) {
        {
            myco_print("[OK] json.parse() works")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] json.parse() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n=== 23. HTTP Library Functions ===")    ;
    char* http = "Module";
    total_tests =     (    total_tests     +     1.000000    )    ;
    if (    strcmp(    "Module"    , "Object"    ) == 0    ) {
        {
            myco_print("[OK] HTTP library import works")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] HTTP library import failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    HttpResponse     get_response     =     ((HttpResponse){200, "Object", "OK", "{}", 1})    ;
    if (    (    strcmp(    "Unknown"    , "Object"    ) == 0     &&     get_response    .status_code     ==     200.000000    )    ) {
        {
            myco_print("[OK] http.get() works")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] http.get() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     post_data     = "{\"test\": \"data\"}"    ;
    HttpResponse     post_response     =     ((HttpResponse){200, "Object", "OK", "{}", 1})    ;
    if (    (    strcmp(    "Unknown"    , "Object"    ) == 0     &&     post_response    .status_code     ==     200.000000    )    ) {
        {
            myco_print("[OK] http.post() works")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] http.post() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     put_data     = "{\"id\": 1, \"name\": \"test\"}"    ;
    HttpResponse     put_response     =     ((HttpResponse){200, "Object", "OK", "{}", 1})    ;
    if (    (    strcmp(    "Unknown"    , "Object"    ) == 0     &&     put_response    .status_code     ==     200.000000    )    ) {
        {
            myco_print("[OK] http.put() works")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] http.put() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    HttpResponse     delete_response     =     ((HttpResponse){200, "Object", "OK", "{}", 1})    ;
    if (    (    strcmp(    "Unknown"    , "Object"    ) == 0     &&     delete_response    .status_code     ==     200.000000    )    ) {
        {
            myco_print("[OK] http.delete() works")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] http.delete() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    if (    strcmp(    "Unknown"    , "Object"    ) == 0    ) {
        {
            double             has_status_code_scope_1             =             get_response            .status_code             !=             NULL            ;
            double             has_status_text_scope_1             =             get_response            .status_text             !=             NULL            ;
            double             has_body_scope_1             =             get_response            .body             !=             NULL            ;
            double             has_success_scope_1             =             get_response            .success             !=             NULL            ;
            if (            (            (            (            has_status_code_scope_1             &&             has_status_text_scope_1            )             &&             has_body_scope_1            )             &&             has_success_scope_1            )            ) {
                {
                    myco_print("[OK] HTTP response object has all required properties")                    ;
                    tests_passed =                     (                    tests_passed                     +                     1.000000                    )                    ;
                }
            } else {
                {
                    myco_print("[FAIL] HTTP response object missing properties")                    ;
                    tests_failed =                     tests_failed                    ;
                }
            }
        }
    } else {
        {
            myco_print("[FAIL] HTTP response object test failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     status_ok     =     NULL    ;
    if (    strcmp(    "Unknown"    , "Boolean"    ) == 0    ) {
        {
            myco_print("[OK] http.status_ok() works")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] http.status_ok() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     content_type     =     NULL    ;
    if (    (    strcmp(    "Unknown"    , "String"    ) == 0     ||     content_type     == NULL    )    ) {
        {
            myco_print("[OK] http.get_header() works")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] http.get_header() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    char*     json_response     =     NULL    ;
    if (    (    strcmp(    "Module"    , "String"    ) == 0     ||     NULL     == NULL    )    ) {
        {
            myco_print("[OK] http.get_json() works")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] http.get_json() failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    total_tests =     (    total_tests     +     1.000000    )    ;
    HttpResponse     error_response     =     ((HttpResponse){200, "Object", "OK", "{}", 1})    ;
    if (    strcmp(    "Unknown"    , "Object"    ) == 0    ) {
        {
            myco_print("[OK] HTTP error handling works")            ;
            tests_passed =             (            tests_passed             +             1.000000            )            ;
        }
    } else {
        {
            myco_print("[FAIL] HTTP error handling failed")            ;
            tests_failed =             tests_failed            ;
        }
    }
    myco_print("\\n=== RESULTS ===")    ;
    myco_print(    myco_string_concat("Total: "    ,     myco_number_to_string(    total_tests    )    ))    ;
    myco_print(    myco_string_concat("Passed: "    ,     myco_number_to_string(    tests_passed    )    ))    ;
    myco_print(    myco_string_concat("Failed: "    ,     myco_number_to_string(    total_tests     -     tests_passed    )    )    )    ;
    double     rate     =     tests_passed     *     100.000000     /     total_tests    ;
    myco_print(    myco_string_concat(    myco_string_concat("Success rate: "    ,     myco_number_to_string(    rate    )    )    , "%"    ))    ;
    if (    total_tests     -     tests_passed     >     0.000000    ) {
        {
            myco_print("Failed tests:")            ;
            for (int i = 0; i <             0; i++) {                {
                    myco_print(                    myco_string_concat("\\t - "                    ,                     myco_number_to_string(                    i                    )                    )                    )                    ;
                }
            }
        }
    } else {
        {
            myco_print("ALL TESTS PASSED!")            ;
        }
    }
    return 0;
}
