#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Myco Runtime Headers
#include "include/myco_runtime.h"

// Myco built-in functions
void myco_print(const char* str) {
    printf("%s\n", str);
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

char* myco_number_to_string(double number) {
    char* result = malloc(64);
    if (result) {
        snprintf(result, 64, "%g", number);
    }
    return result;
}

// Main program entry point
int main(void) {
    myco_print(    "Testing type checker..."    )    ;
    Int     x     =     42.000000    ;
    String     y     =     "hello"    ;
    Bool     z     =     1    ;
    myco_print(    myco_string_concat(    "x = "    ,     myco_number_to_string(    x    )    )    )    ;
    myco_print(    myco_string_concat(    "y = "    ,     myco_number_to_string(    y    )    )    )    ;
    myco_print(    myco_string_concat(    "z = "    ,     myco_number_to_string(    z    )    )    )    ;
    double     a     =     10.000000    ;
    double     b     =     3.140000    ;
    char*     c     =     "world"    ;
    myco_print(    myco_string_concat(    "a = "    ,     myco_number_to_string(    a    )    )    )    ;
    myco_print(    myco_string_concat(    "b = "    ,     myco_number_to_string(    b    )    )    )    ;
    myco_print(    myco_string_concat(    "c = "    ,     myco_number_to_string(    c    )    )    )    ;
    Int     d     =     100.000000    ;
    myco_print(    "Test completed!"    )    ;
    return 0;
}
