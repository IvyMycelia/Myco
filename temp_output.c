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

// Main program entry point
int main(void) {
    myco_print(    "    =    =    =         M    y    c    o         C    o    m    p    r    e    h    e    n    s    i    v    e         T    e    s    t         S    u    i    t    e         =    =    =    "    )    ;
    myco_print(    "    T    e    s    t    i    n    g         E    V    E    R    Y         f    e    a    t    u    r    e         a    n    d         e    d    g    e         c    a    s    e         i    n         M    y    c    o    "    )    ;
    myco_print(    "    "    )    ;
    double     total_tests     =     0.000000    ;
    double     tests_passed     =     0.000000    ;
    char**     tests_failed     =     (double[]){    }    ;
    myco_print(    "    =    =    =         1    .         B    A    S    I    C         D    A    T    A         T    Y    P    E    S         =    =    =    "    )    ;
    myco_print(    "    1    .    1    .         N    u    m    b    e    r    s    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     x     =     42.000000    ;
    if (    x     ==     42.000000    ) {
        {
            myco_print(            "            â            œ            “                         I            n            t            e            g            e            r                         v            a            r            i            a            b            l            e            :                         "             +             x            .toString            (            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         I            n            t            e            g            e            r                         v            a            r            i            a            b            l            e            :                         "             +             x            .toString            (            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     y     =     3.140000    ;
    if (    y     ==     3.140000    ) {
        {
            myco_print(            "            â            œ            “                         F            l            o            a            t                         v            a            r            i            a            b            l            e            :                         "             +             y            .toString            (            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         F            l            o            a            t                         v            a            r            i            a            b            l            e            :                         "             +             y            .toString            (            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     neg_int     =     -    10.000000    ;
    if (    neg_int     ==     -    10.000000    ) {
        {
            myco_print(            "            â            œ            “                         N            e            g            a            t            i            v            e                         i            n            t            e            g            e            r            :                         "             +             neg_int            .toString            (            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         N            e            g            a            t            i            v            e                         i            n            t            e            g            e            r            :                         "             +             neg_int            .toString            (            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     neg_float     =     -    3.140000    ;
    if (    neg_float     ==     -    3.140000    ) {
        {
            myco_print(            "            â            œ            “                         N            e            g            a            t            i            v            e                         f            l            o            a            t            :                         "             +             neg_float            .toString            (            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         N            e            g            a            t            i            v            e                         f            l            o            a            t            :                         "             +             neg_float            .toString            (            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     zero     =     0.000000    ;
    if (    zero     ==     0.000000    ) {
        {
            myco_print(            "            â            œ            “                         Z            e            r            o            :                         "             +             zero            .toString            (            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         Z            e            r            o            :                         "             +             zero            .toString            (            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    1    .    2    .         S    t    r    i    n    g    s    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     name     =     "    M    y    c    o    "    ;
    if (    name     ==     "    M    y    c    o    "    ) {
        {
            myco_print(            "            â            œ            “                         S            t            r            i            n            g                         v            a            r            i            a            b            l            e            :                         "             +             name            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         S            t            r            i            n            g                         v            a            r            i            a            b            l            e            :                         "             +             name            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     empty_str     =     "    "    ;
    if (    empty_str     ==     "    "    ) {
        {
            myco_print(            "            â            œ            “                         E            m            p            t            y                         s            t            r            i            n            g            :                         '            "             +             empty_str             +             "            '            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         E            m            p            t            y                         s            t            r            i            n            g            :                         '            "             +             empty_str             +             "            '            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     spaced_str     =     "    h    e    l    l    o         w    o    r    l    d    "    ;
    if (    spaced_str     ==     "    h    e    l    l    o         w    o    r    l    d    "    ) {
        {
            myco_print(            "            â            œ            “                         S            t            r            i            n            g                         w            i            t            h                         s            p            a            c            e            s            :                         "             +             spaced_str            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         S            t            r            i            n            g                         w            i            t            h                         s            p            a            c            e            s            :                         "             +             spaced_str            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    1    .    3    .         B    o    o    l    e    a    n    s    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    int     flag     =     1    ;
    if (    flag     ==     1    ) {
        {
            myco_print(            "            â            œ            “                         B            o            o            l            e            a            n                         T            r            u            e            :                         "             +             flag            .toString            (            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         B            o            o            l            e            a            n                         T            r            u            e            :                         "             +             flag            .toString            (            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     false_flag     =     0    ;
    if (    false_flag     ==     0    ) {
        {
            myco_print(            "            â            œ            “                         B            o            o            l            e            a            n                         F            a            l            s            e            :                         "             +             false_flag            .toString            (            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         B            o            o            l            e            a            n                         F            a            l            s            e            :                         "             +             false_flag            .toString            (            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    1    .    4    .         N    u    l    l    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     null_var     =     NULL    ;
    if (    null_var     ==     NULL    ) {
        {
            myco_print(            "            â            œ            “                         N            u            l            l                         v            a            r            i            a            b            l            e            :                         "             +             null_var            .toString            (            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         N            u            l            l                         v            a            r            i            a            b            l            e            :                         "             +             null_var            .toString            (            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    =    =    =         2    .         A    R    I    T    H    M    E    T    I    C         O    P    E    R    A    T    I    O    N    S         =    =    =    "    )    ;
    myco_print(    "    2    .    1    .         A    d    d    i    t    i    o    n    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     a     =     5.000000     +     3.000000    ;
    if (    a     ==     8.000000    ) {
        {
            myco_print(            "            â            œ            “                         I            n            t            e            g            e            r                         a            d            d            i            t            i            o            n                         (            5                         +                         3            )            :                         "             +             a            .toString            (            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         I            n            t            e            g            e            r                         a            d            d            i            t            i            o            n                         (            5                         +                         3            )            :                         "             +             a            .toString            (            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     float_add     =     2.500000     +     1.500000    ;
    if (    float_add     ==     4.000000    ) {
        {
            myco_print(            "            â            œ            “                         F            l            o            a            t                         a            d            d            i            t            i            o            n                         (            2            .            5                         +                         1            .            5            )            :                         "             +             float_add            .toString            (            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         F            l            o            a            t                         a            d            d            i            t            i            o            n                         (            2            .            5                         +                         1            .            5            )            :                         "             +             float_add            .toString            (            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     mixed_add     =     5.000000     +     2.500000    ;
    if (    mixed_add     ==     7.500000    ) {
        {
            myco_print(            "            â            œ            “                         M            i            x            e            d                         a            d            d            i            t            i            o            n                         (            5                         +                         2            .            5            )            :                         "             +             mixed_add            .toString            (            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         M            i            x            e            d                         a            d            d            i            t            i            o            n                         (            5                         +                         2            .            5            )            :                         "             +             mixed_add            .toString            (            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     str_concat     =     myco_string_concat(    "    h    e    l    l    o    "    ,     "    w    o    r    l    d    "    )    ;
    if (    str_concat     ==     "    h    e    l    l    o    w    o    r    l    d    "    ) {
        {
            myco_print(            "            â            œ            “                         S            t            r            i            n            g                         c            o            n            c            a            t            e            n            a            t            i            o            n            :                         "             +             str_concat            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         S            t            r            i            n            g                         c            o            n            c            a            t            e            n            a            t            i            o            n            :                         "             +             str_concat            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    2    .    2    .         S    u    b    t    r    a    c    t    i    o    n    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     b     =     10.000000     -     4.000000    ;
    if (    b     ==     6.000000    ) {
        {
            myco_print(            "            â            œ            “                         I            n            t            e            g            e            r                         s            u            b            t            r            a            c            t            i            o            n                         (            1            0                         -                         4            )            :                         "             +             b            .toString            (            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         I            n            t            e            g            e            r                         s            u            b            t            r            a            c            t            i            o            n                         (            1            0                         -                         4            )            :                         "             +             b            .toString            (            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     float_sub     =     5.500000     -     2.500000    ;
    if (    float_sub     ==     3.000000    ) {
        {
            myco_print(            "            â            œ            “                         F            l            o            a            t                         s            u            b            t            r            a            c            t            i            o            n                         (            5            .            5                         -                         2            .            5            )            :                         "             +             float_sub            .toString            (            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         F            l            o            a            t                         s            u            b            t            r            a            c            t            i            o            n                         (            5            .            5                         -                         2            .            5            )            :                         "             +             float_sub            .toString            (            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     neg_result     =     3.000000     -     5.000000    ;
    if (    neg_result     ==     -    2.000000    ) {
        {
            myco_print(            "            â            œ            “                         N            e            g            a            t            i            v            e                         r            e            s            u            l            t                         (            3                         -                         5            )            :                         "             +             neg_result            .toString            (            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         N            e            g            a            t            i            v            e                         r            e            s            u            l            t                         (            3                         -                         5            )            :                         "             +             neg_result            .toString            (            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    2    .    3    .         M    u    l    t    i    p    l    i    c    a    t    i    o    n    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     c     =     6.000000     *     7.000000    ;
    if (    c     ==     42.000000    ) {
        {
            myco_print(            "            â            œ            “                         I            n            t            e            g            e            r                         m            u            l            t            i            p            l            i            c            a            t            i            o            n                         (            6                         *                         7            )            :                         "             +             c            .toString            (            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         I            n            t            e            g            e            r                         m            u            l            t            i            p            l            i            c            a            t            i            o            n                         (            6                         *                         7            )            :                         "             +             c            .toString            (            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     float_mult     =     2.500000     *     4.000000    ;
    if (    float_mult     ==     10.000000    ) {
        {
            myco_print(            "            â            œ            “                         F            l            o            a            t                         m            u            l            t            i            p            l            i            c            a            t            i            o            n                         (            2            .            5                         *                         4            )            :                         "             +             float_mult            .toString            (            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         F            l            o            a            t                         m            u            l            t            i            p            l            i            c            a            t            i            o            n                         (            2            .            5                         *                         4            )            :                         "             +             float_mult            .toString            (            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     zero_mult     =     5.000000     *     0.000000    ;
    if (    zero_mult     ==     0.000000    ) {
        {
            myco_print(            "            â            œ            “                         Z            e            r            o                         m            u            l            t            i            p            l            i            c            a            t            i            o            n                         (            5                         *                         0            )            :                         "             +             zero_mult            .toString            (            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         Z            e            r            o                         m            u            l            t            i            p            l            i            c            a            t            i            o            n                         (            5                         *                         0            )            :                         "             +             zero_mult            .toString            (            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    2    .    4    .         D    i    v    i    s    i    o    n    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     d     =     15.000000     /     3.000000    ;
    if (    d     ==     5.000000    ) {
        {
            myco_print(            "            â            œ            “                         I            n            t            e            g            e            r                         d            i            v            i            s            i            o            n                         (            1            5                         /                         3            )            :                         "             +             d            .toString            (            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         I            n            t            e            g            e            r                         d            i            v            i            s            i            o            n                         (            1            5                         /                         3            )            :                         "             +             d            .toString            (            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     float_div     =     10.000000     /     2.500000    ;
    if (    float_div     ==     4.000000    ) {
        {
            myco_print(            "            â            œ            “                         F            l            o            a            t                         d            i            v            i            s            i            o            n                         (            1            0            .            0                         /                         2            .            5            )            :                         "             +             float_div            .toString            (            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         F            l            o            a            t                         d            i            v            i            s            i            o            n                         (            1            0            .            0                         /                         2            .            5            )            :                         "             +             float_div            .toString            (            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     remainder_div     =     7.000000     /     2.000000    ;
    if (    remainder_div     ==     3.500000    ) {
        {
            myco_print(            "            â            œ            “                         D            i            v            i            s            i            o            n                         w            i            t            h                         r            e            m            a            i            n            d            e            r                         (            7                         /                         2            )            :                         "             +             remainder_div            .toString            (            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         D            i            v            i            s            i            o            n                         w            i            t            h                         r            e            m            a            i            n            d            e            r                         (            7                         /                         2            )            :                         "             +             remainder_div            .toString            (            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    =    =    =         3    .         C    O    M    P    A    R    I    S    O    N         O    P    E    R    A    T    I    O    N    S         =    =    =    "    )    ;
    myco_print(    "    3    .    1    .         E    q    u    a    l    i    t    y    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     comp3     =     5.000000     ==     5.000000    ;
    if (    comp3     ==     1    ) {
        {
            myco_print(            "            â            œ            “                         I            n            t            e            g            e            r                         e            q            u            a            l            i            t            y                         (            5                         =            =                         5            )            :                         "             +             comp3            .toString            (            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         I            n            t            e            g            e            r                         e            q            u            a            l            i            t            y                         (            5                         =            =                         5            )            :                         "             +             comp3            .toString            (            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     comp_neq     =     5.000000     ==     6.000000    ;
    if (    comp_neq     ==     0    ) {
        {
            myco_print(            "            â            œ            “                         I            n            t            e            g            e            r                         i            n            e            q            u            a            l            i            t            y                         (            5                         =            =                         6            )            :                         "             +             comp_neq            .toString            (            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         I            n            t            e            g            e            r                         i            n            e            q            u            a            l            i            t            y                         (            5                         =            =                         6            )            :                         "             +             comp_neq            .toString            (            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     float_eq     =     3.140000     ==     3.140000    ;
    if (    float_eq     ==     1    ) {
        {
            myco_print(            "            â            œ            “                         F            l            o            a            t                         e            q            u            a            l            i            t            y                         (            3            .            1            4                         =            =                         3            .            1            4            )            :                         "             +             float_eq            .toString            (            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         F            l            o            a            t                         e            q            u            a            l            i            t            y                         (            3            .            1            4                         =            =                         3            .            1            4            )            :                         "             +             float_eq            .toString            (            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     str_eq     =     "    h    e    l    l    o    "     ==     "    h    e    l    l    o    "    ;
    if (    str_eq     ==     1    ) {
        {
            myco_print(            "            â            œ            “                         S            t            r            i            n            g                         e            q            u            a            l            i            t            y                         (            '            h            e            l            l            o            '                         =            =                         '            h            e            l            l            o            '            )            :                         "             +             str_eq            .toString            (            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         S            t            r            i            n            g                         e            q            u            a            l            i            t            y                         (            '            h            e            l            l            o            '                         =            =                         '            h            e            l            l            o            '            )            :                         "             +             str_eq            .toString            (            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     bool_eq     =     1     ==     1    ;
    if (    bool_eq     ==     1    ) {
        {
            myco_print(            "            â            œ            “                         B            o            o            l            e            a            n                         e            q            u            a            l            i            t            y                         (            T            r            u            e                         =            =                         T            r            u            e            )            :                         "             +             bool_eq            .toString            (            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         B            o            o            l            e            a            n                         e            q            u            a            l            i            t            y                         (            T            r            u            e                         =            =                         T            r            u            e            )            :                         "             +             bool_eq            .toString            (            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    3    .    2    .         I    n    e    q    u    a    l    i    t    y    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     comp4     =     4.000000     !=     6.000000    ;
    if (    comp4     ==     1    ) {
        {
            myco_print(            "            â            œ            “                         I            n            t            e            g            e            r                         i            n            e            q            u            a            l            i            t            y                         (            4                         !            =                         6            )            :                         "             +             comp4            .toString            (            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         I            n            t            e            g            e            r                         i            n            e            q            u            a            l            i            t            y                         (            4                         !            =                         6            )            :                         "             +             comp4            .toString            (            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     str_neq     =     "    h    e    l    l    o    "     !=     "    w    o    r    l    d    "    ;
    if (    str_neq     ==     1    ) {
        {
            myco_print(            "            â            œ            “                         S            t            r            i            n            g                         i            n            e            q            u            a            l            i            t            y                         (            '            h            e            l            l            o            '                         !            =                         '            w            o            r            l            d            '            )            :                         "             +             str_neq            .toString            (            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         S            t            r            i            n            g                         i            n            e            q            u            a            l            i            t            y                         (            '            h            e            l            l            o            '                         !            =                         '            w            o            r            l            d            '            )            :                         "             +             str_neq            .toString            (            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    3    .    3    .         G    r    e    a    t    e    r         t    h    a    n    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     comp1     =     10.000000     >     5.000000    ;
    if (    comp1     ==     1    ) {
        {
            myco_print(            "            â            œ            “                         G            r            e            a            t            e            r                         t            h            a            n                         (            1            0                         >                         5            )            :                         "             +             comp1            .toString            (            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         G            r            e            a            t            e            r                         t            h            a            n                         (            1            0                         >                         5            )            :                         "             +             comp1            .toString            (            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     comp1_false     =     5.000000     >     10.000000    ;
    if (    comp1_false     ==     0    ) {
        {
            myco_print(            "            â            œ            “                         N            o            t                         g            r            e            a            t            e            r                         t            h            a            n                         (            5                         >                         1            0            )            :                         "             +             comp1_false            .toString            (            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         N            o            t                         g            r            e            a            t            e            r                         t            h            a            n                         (            5                         >                         1            0            )            :                         "             +             comp1_false            .toString            (            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    3    .    4    .         L    e    s    s         t    h    a    n    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     comp2     =     3.000000     <     7.000000    ;
    if (    comp2     ==     1    ) {
        {
            myco_print(            "            â            œ            “                         L            e            s            s                         t            h            a            n                         (            3                         <                         7            )            :                         "             +             comp2            .toString            (            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         L            e            s            s                         t            h            a            n                         (            3                         <                         7            )            :                         "             +             comp2            .toString            (            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     comp2_false     =     7.000000     <     3.000000    ;
    if (    comp2_false     ==     0    ) {
        {
            myco_print(            "            â            œ            “                         N            o            t                         l            e            s            s                         t            h            a            n                         (            7                         <                         3            )            :                         "             +             comp2_false            .toString            (            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         N            o            t                         l            e            s            s                         t            h            a            n                         (            7                         <                         3            )            :                         "             +             comp2_false            .toString            (            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    3    .    5    .         G    r    e    a    t    e    r         t    h    a    n         o    r         e    q    u    a    l    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     gte_true     =     10.000000     >=     5.000000    ;
    if (    gte_true     ==     1    ) {
        {
            myco_print(            "            â            œ            “                         G            r            e            a            t            e            r                         t            h            a            n                         o            r                         e            q            u            a            l                         (            1            0                         >            =                         5            )            :                         "             +             gte_true            .toString            (            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         G            r            e            a            t            e            r                         t            h            a            n                         o            r                         e            q            u            a            l                         (            1            0                         >            =                         5            )            :                         "             +             gte_true            .toString            (            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     gte_equal     =     5.000000     >=     5.000000    ;
    if (    gte_equal     ==     1    ) {
        {
            myco_print(            "            â            œ            “                         E            q            u            a            l                         v            a            l            u            e            s                         (            5                         >            =                         5            )            :                         "             +             gte_equal            .toString            (            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         E            q            u            a            l                         v            a            l            u            e            s                         (            5                         >            =                         5            )            :                         "             +             gte_equal            .toString            (            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    3    .    6    .         L    e    s    s         t    h    a    n         o    r         e    q    u    a    l    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     lte_true     =     3.000000     <=     7.000000    ;
    if (    lte_true     ==     1    ) {
        {
            myco_print(            "            â            œ            “                         L            e            s            s                         t            h            a            n                         o            r                         e            q            u            a            l                         (            3                         <            =                         7            )            :                         "             +             lte_true            .toString            (            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         L            e            s            s                         t            h            a            n                         o            r                         e            q            u            a            l                         (            3                         <            =                         7            )            :                         "             +             lte_true            .toString            (            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     lte_equal     =     5.000000     <=     5.000000    ;
    if (    lte_equal     ==     1    ) {
        {
            myco_print(            "            â            œ            “                         E            q            u            a            l                         v            a            l            u            e            s                         (            5                         <            =                         5            )            :                         "             +             lte_equal            .toString            (            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         E            q            u            a            l                         v            a            l            u            e            s                         (            5                         <            =                         5            )            :                         "             +             lte_equal            .toString            (            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    =    =    =         4    .         L    O    G    I    C    A    L         O    P    E    R    A    T    I    O    N    S         =    =    =    "    )    ;
    myco_print(    "    4    .    1    .         L    o    g    i    c    a    l         A    N    D    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     log1     =     1     &&     1    ;
    if (    log1     ==     1    ) {
        {
            myco_print(            "            â            œ            “                         L            o            g            i            c            a            l                         A            N            D                         (            T            r            u            e                         a            n            d                         T            r            u            e            )            :                         "             +             log1            .toString            (            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         L            o            g            i            c            a            l                         A            N            D                         (            T            r            u            e                         a            n            d                         T            r            u            e            )            :                         "             +             log1            .toString            (            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     log1_false     =     1     &&     0    ;
    if (    log1_false     ==     0    ) {
        {
            myco_print(            "            â            œ            “                         L            o            g            i            c            a            l                         A            N            D                         f            a            l            s            e                         (            T            r            u            e                         a            n            d                         F            a            l            s            e            )            :                         "             +             log1_false            .toString            (            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         L            o            g            i            c            a            l                         A            N            D                         f            a            l            s            e                         (            T            r            u            e                         a            n            d                         F            a            l            s            e            )            :                         "             +             log1_false            .toString            (            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    4    .    2    .         L    o    g    i    c    a    l         O    R    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     log2     =     0     ||     1    ;
    if (    log2     ==     1    ) {
        {
            myco_print(            "            â            œ            “                         L            o            g            i            c            a            l                         O            R                         (            F            a            l            s            e                         o            r                         T            r            u            e            )            :                         "             +             log2            .toString            (            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         L            o            g            i            c            a            l                         O            R                         (            F            a            l            s            e                         o            r                         T            r            u            e            )            :                         "             +             log2            .toString            (            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     log2_false     =     0     ||     0    ;
    if (    log2_false     ==     0    ) {
        {
            myco_print(            "            â            œ            “                         L            o            g            i            c            a            l                         O            R                         f            a            l            s            e                         (            F            a            l            s            e                         o            r                         F            a            l            s            e            )            :                         "             +             log2_false            .toString            (            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         L            o            g            i            c            a            l                         O            R                         f            a            l            s            e                         (            F            a            l            s            e                         o            r                         F            a            l            s            e            )            :                         "             +             log2_false            .toString            (            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    4    .    3    .         L    o    g    i    c    a    l         N    O    T    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     log3     =     !    0    ;
    if (    log3     ==     1    ) {
        {
            myco_print(            "            â            œ            “                         L            o            g            i            c            a            l                         N            O            T                         (            n            o            t                         F            a            l            s            e            )            :                         "             +             log3            .toString            (            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         L            o            g            i            c            a            l                         N            O            T                         (            n            o            t                         F            a            l            s            e            )            :                         "             +             log3            .toString            (            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     log3_false     =     !    1    ;
    if (    log3_false     ==     0    ) {
        {
            myco_print(            "            â            œ            “                         L            o            g            i            c            a            l                         N            O            T                         f            a            l            s            e                         (            n            o            t                         T            r            u            e            )            :                         "             +             log3_false            .toString            (            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         L            o            g            i            c            a            l                         N            O            T                         f            a            l            s            e                         (            n            o            t                         T            r            u            e            )            :                         "             +             log3_false            .toString            (            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    4    .    4    .         C    o    m    p    l    e    x         l    o    g    i    c    a    l         e    x    p    r    e    s    s    i    o    n    s    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     complex_log     =     1     &&     0     ||     1    ;
    if (    complex_log     ==     1    ) {
        {
            myco_print(            "            â            œ            “                         C            o            m            p            l            e            x                         l            o            g            i            c            a            l                         (            (            T            r            u            e                         a            n            d                         F            a            l            s            e            )                         o            r                         T            r            u            e            )            :                         "             +             complex_log            .toString            (            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         C            o            m            p            l            e            x                         l            o            g            i            c            a            l                         (            (            T            r            u            e                         a            n            d                         F            a            l            s            e            )                         o            r                         T            r            u            e            )            :                         "             +             complex_log            .toString            (            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     nested_not     =     !    1     &&     0    ;
    if (    nested_not     ==     1    ) {
        {
            myco_print(            "            â            œ            “                         N            e            s            t            e            d                         N            O            T                         (            n            o            t                         (            T            r            u            e                         a            n            d                         F            a            l            s            e            )            )            :                         "             +             nested_not            .toString            (            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         N            e            s            t            e            d                         N            O            T                         (            n            o            t                         (            T            r            u            e                         a            n            d                         F            a            l            s            e            )            )            :                         "             +             nested_not            .toString            (            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    5    .         A    r    r    a    y    s    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    double*     arr     =     (double[]){    1.000000    ,     2.000000    ,     3.000000    }    ;
    if (    arr    .length    (    )     ==     3.000000    ) {
        {
            myco_print(            "            â            œ            “                         S            i            m            p            l            e                         a            r            r            a            y            :                         "             +             arr            .toString            (            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         S            i            m            p            l            e                         a            r            r            a            y            :                         "             +             arr            .toString            (            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    void**     nested     =     (void*[]){    (void*)    (double[]){    1.000000    ,     2.000000    }    ,     (void*)    (double[]){    3.000000    ,     4.000000    }    }    ;
    if (    nested    .length    (    )     ==     2.000000    ) {
        {
            myco_print(            "            â            œ            “                         N            e            s            t            e            d                         a            r            r            a            y            :                         "             +             nested            .toString            (            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         N            e            s            t            e            d                         a            r            r            a            y            :                         "             +             nested            .toString            (            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    void**     mixed     =     (void*[]){    (void*)    1.000000    ,     (void*)    "    h    e    l    l    o    "    ,     (void*)    1    ,     (void*)    (double[]){    1.000000    ,     2.000000    }    }    ;
    if (    mixed    .length    (    )     ==     4.000000    ) {
        {
            myco_print(            "            â            œ            “                         M            i            x            e            d                         t            y            p            e                         a            r            r            a            y            :                         "             +             mixed            .toString            (            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         M            i            x            e            d                         t            y            p            e                         a            r            r            a            y            :                         "             +             mixed            .toString            (            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char**     empty     =     (double[]){    }    ;
    if (    empty    .length    (    )     ==     0.000000    ) {
        {
            myco_print(            "            â            œ            “                         E            m            p            t            y                         a            r            r            a            y            :                         "             +             empty            .toString            (            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         E            m            p            t            y                         a            r            r            a            y            :                         "             +             empty            .toString            (            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    6    .         S    t    r    i    n    g         O    p    e    r    a    t    i    o    n    s    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     str1     =     "    H    e    l    l    o    "    ;
    char*     str2     =     "    W    o    r    l    d    "    ;
    char*     combined     =     str1     +     "         "     +     str2    ;
    myco_print(    "    â    œ    “         S    t    r    i    n    g         c    o    n    c    a    t    e    n    a    t    i    o    n    :         "     +     combined    )    ;
    tests_passed     =     tests_passed     +     1.000000    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     num_str     =     42.000000    .toString    (    )    ;
    myco_print(    "    â    œ    “         N    u    m    b    e    r         t    o         s    t    r    i    n    g    :         "     +     num_str    )    ;
    tests_passed     =     tests_passed     +     1.000000    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     bool_str     =     1    .toString    (    )    ;
    myco_print(    "    â    œ    “         B    o    o    l    e    a    n         t    o         s    t    r    i    n    g    :         "     +     bool_str    )    ;
    tests_passed     =     tests_passed     +     1.000000    ;
    myco_print(    "    \\    n    =    =    =         7    .         S    T    R    I    N    G         O    P    E    R    A    T    I    O    N    S         =    =    =    "    )    ;
    myco_print(    "    7    .    1    .         S    t    r    i    n    g         c    o    n    c    a    t    e    n    a    t    i    o    n    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     str1_2     =     "    H    e    l    l    o    "    ;
    char*     str2_2     =     "    W    o    r    l    d    "    ;
    char*     combined_2     =     str1_2     +     "         "     +     str2_2    ;
    if (    combined_2     ==     "    H    e    l    l    o         W    o    r    l    d    "    ) {
        {
            myco_print(            "            â            œ            “                         S            t            r            i            n            g                         c            o            n            c            a            t            e            n            a            t            i            o            n            :                         "             +             combined_2            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         S            t            r            i            n            g                         c            o            n            c            a            t            e            n            a            t            i            o            n            :                         "             +             combined_2            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     num_str_2     =     42.000000    .toString    (    )    ;
    if (    num_str_2     ==     "    4    2    "    ) {
        {
            myco_print(            "            â            œ            “                         N            u            m            b            e            r                         t            o                         s            t            r            i            n            g            :                         "             +             num_str_2            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         N            u            m            b            e            r                         t            o                         s            t            r            i            n            g            :                         "             +             num_str_2            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     bool_str_2     =     1    .toString    (    )    ;
    if (    bool_str_2     ==     "    T    r    u    e    "    ) {
        {
            myco_print(            "            â            œ            “                         B            o            o            l            e            a            n                         t            o                         s            t            r            i            n            g            :                         "             +             bool_str_2            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         B            o            o            l            e            a            n                         t            o                         s            t            r            i            n            g            :                         "             +             bool_str_2            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    7    .    2    .         E    s    c    a    p    e         s    e    q    u    e    n    c    e    s    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     escaped     =     "    L    i    n    e         1    \\    n    L    i    n    e         2    \\    t    T    a    b    b    e    d    "    ;
    if (    escaped    .length    (    )     >     0.000000    ) {
        {
            myco_print(            "            â            œ            “                         E            s            c            a            p            e                         s            e            q            u            e            n            c            e            s                         w            o            r            k            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         E            s            c            a            p            e                         s            e            q            u            e            n            c            e            s                         w            o            r            k            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     quoted_str     =     "    H    e         s    a    i    d         \\    \"    H    e    l    l    o    \\    \"    "    ;
    if (    quoted_str     ==     "    H    e         s    a    i    d         \\    \"    H    e    l    l    o    \\    \"    "    ) {
        {
            myco_print(            "            â            œ            “                         Q            u            o            t            e            d                         s            t            r            i            n            g            :                         "             +             quoted_str            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         Q            u            o            t            e            d                         s            t            r            i            n            g            :                         "             +             quoted_str            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     backslash_str     =     "    P    a    t    h    :         C    :    \\    \\    U    s    e    r    s    \\    \\    N    a    m    e    "    ;
    if (    backslash_str     ==     "    P    a    t    h    :         C    :    \\    \\    U    s    e    r    s    \\    \\    N    a    m    e    "    ) {
        {
            myco_print(            "            â            œ            “                         B            a            c            k            s            l            a            s            h                         s            t            r            i            n            g            :                         "             +             backslash_str            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         B            a            c            k            s            l            a            s            h                         s            t            r            i            n            g            :                         "             +             backslash_str            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    8    .         C    o    n    t    r    o    l         F    l    o    w    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    if (    10.000000     >     5.000000    ) {
        {
            myco_print(            "            â            œ            “                         I            f                         s            t            a            t            e            m            e            n            t                         w            o            r            k            s            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         I            f                         s            t            a            t            e            m            e            n            t            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    if (    3.000000     <     2.000000    ) {
        {
            myco_print(            "            â            œ            —                         T            h            i            s                         s            h            o            u            l            d                         n            o            t                         p            r            i            n            t            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    } else {
        {
            myco_print(            "            â            œ            “                         E            l            s            e                         s            t            a            t            e            m            e            n            t                         w            o            r            k            s            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    if (    1.000000     >     2.000000    ) {
        {
            myco_print(            "            â            œ            —                         T            h            i            s                         s            h            o            u            l            d                         n            o            t                         p            r            i            n            t            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    9    .         B    u    i    l    t    -    i    n         L    i    b    r    a    r    i    e    s    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    char* math = "Module";
    if (    "Module"     ==     "    M    o    d    u    l    e    "    ) {
        {
            myco_print(            "            â            œ            “                         M            a            t            h                         l            i            b            r            a            r            y                         i            m            p            o            r            t            e            d            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         M            a            t            h                         l            i            b            r            a            r            y                         i            m            p            o            r            t                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     test_string     =     "    h    e    l    l    o    "    ;
    if (    test_string    .type    (    )     ==     "    S    t    r    i    n    g    "     &&     test_string     != NULL    ) {
        {
            myco_print(            "            â            œ            “                         S            t            r            i            n            g                         t            y            p            e                         a            v            a            i            l            a            b            l            e            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         S            t            r            i            n            g                         t            y            p            e                         n            o            t                         a            v            a            i            l            a            b            l            e            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double*     test_array_1     =     (double[]){    1.000000    ,     2.000000    ,     3.000000    }    ;
    if (    test_array_1    .type    (    )     ==     "    A    r    r    a    y    "     &&     test_array_1     != NULL    ) {
        {
            myco_print(            "            â            œ            “                         A            r            r            a            y                         t            y            p            e                         a            v            a            i            l            a            b            l            e            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         A            r            r            a            y                         t            y            p            e                         n            o            t                         a            v            a            i            l            a            b            l            e            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char* file = "Module";
    if (    "Module"     ==     "    M    o    d    u    l    e    "    ) {
        {
            myco_print(            "            â            œ            “                         F            i            l            e                         l            i            b            r            a            r            y                         i            m            p            o            r            t            e            d            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         F            i            l            e                         l            i            b            r            a            r            y                         i            m            p            o            r            t                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char* dir = "Module";
    if (    "Module"     ==     "    M    o    d    u    l    e    "    ) {
        {
            myco_print(            "            â            œ            “                         D            i            r            e            c            t            o            r            y                         l            i            b            r            a            r            y                         i            m            p            o            r            t            e            d            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         D            i            r            e            c            t            o            r            y                         l            i            b            r            a            r            y                         i            m            p            o            r            t                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    void*     test_map_1     =     NULL    ;
    if (    test_map_1    .type    (    )     ==     "    M    a    p    "    ) {
        {
            myco_print(            "            â            œ            “                         M            a            p            s                         l            i            b            r            a            r            y                         a            v            a            i            l            a            b            l            e            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         M            a            p            s                         l            i            b            r            a            r            y                         n            o            t                         a            v            a            i            l            a            b            l            e            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    void*     test_set1     =     NULL    ;
    if (    test_set1    .type    (    )     ==     "    S    e    t    "    ) {
        {
            myco_print(            "            â            œ            “                         S            e            t            s                         t            y            p            e                         a            v            a            i            l            a            b            l            e            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         S            e            t            s                         t            y            p            e                         n            o            t                         a            v            a            i            l            a            b            l            e            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     test_tree1     =     "TreeObject"    ;
    if (    test_tree1    .type    (    )     ==     "    T    r    e    e    "    ) {
        {
            myco_print(            "            â            œ            “                         T            r            e            e            s                         l            i            b            r            a            r            y                         i            m            p            o            r            t            e            d            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         T            r            e            e            s                         l            i            b            r            a            r            y                         i            m            p            o            r            t                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     graphs_test1     =     "GraphObject"    ;
    if (    graphs_test1    .type    (    )     ==     "    G    r    a    p    h    "    ) {
        {
            myco_print(            "            â            œ            “                         G            r            a            p            h            s                         l            i            b            r            a            r            y                         i            m            p            o            r            t            e            d            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         G            r            a            p            h            s                         l            i            b            r            a            r            y                         i            m            p            o            r            t                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    1    0    .         M    a    t    h         L    i    b    r    a    r    y         F    u    n    c    t    i    o    n    s    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     pi_value     =     NULL    .Pi    ;
    myco_print(    "    â    œ    “         M    a    t    h    .    P    i    :         "     +     pi_value    .toString    (    )    )    ;
    tests_passed     =     tests_passed     +     1.000000    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     e_value     =     NULL    .E    ;
    myco_print(    "    â    œ    “         M    a    t    h    .    E    :         "     +     e_value    .toString    (    )    )    ;
    tests_passed     =     tests_passed     +     1.000000    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     abs_result     =     NULL    .abs    (    -    5.000000    )    ;
    myco_print(    "    â    œ    “         M    a    t    h    .    a    b    s    (    -    5    )    :         "     +     abs_result    .toString    (    )    )    ;
    tests_passed     =     tests_passed     +     1.000000    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     min_result     =     NULL    .min    (    10.000000    ,     5.000000    )    ;
    myco_print(    "    â    œ    “         M    a    t    h    .    m    i    n    (    1    0    ,         5    )    :         "     +     min_result    .toString    (    )    )    ;
    tests_passed     =     tests_passed     +     1.000000    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     max_result     =     NULL    .max    (    10.000000    ,     5.000000    )    ;
    myco_print(    "    â    œ    “         M    a    t    h    .    m    a    x    (    1    0    ,         5    )    :         "     +     max_result    .toString    (    )    )    ;
    tests_passed     =     tests_passed     +     1.000000    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     sqrt_result     =     NULL    .sqrt    (    16.000000    )    ;
    myco_print(    "    â    œ    “         M    a    t    h    .    s    q    r    t    (    1    6    )    :         "     +     sqrt_result    .toString    (    )    )    ;
    tests_passed     =     tests_passed     +     1.000000    ;
    myco_print(    "    \\    n    1    1    .         S    t    r    i    n    g         L    i    b    r    a    r    y         F    u    n    c    t    i    o    n    s    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     test_str     =     "    h    e    l    l    o         w    o    r    l    d    "    ;
    char*     upper_str     =     test_str    .upper    (    )    ;
    myco_print(    "    â    œ    “         S    t    r    i    n    g    .    u    p    p    e    r    (    '    h    e    l    l    o         w    o    r    l    d    '    )    :         "     +     upper_str    )    ;
    tests_passed     =     tests_passed     +     1.000000    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     lower_str_var     =     "    H    E    L    L    O         W    O    R    L    D    "    ;
    char*     lower_str     =     lower_str_var    .lower    (    )    ;
    myco_print(    "    â    œ    “         S    t    r    i    n    g    .    l    o    w    e    r    (    '    H    E    L    L    O         W    O    R    L    D    '    )    :         "     +     lower_str    )    ;
    tests_passed     =     tests_passed     +     1.000000    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     trim_str_var     =     "              h    e    l    l    o              "    ;
    char*     trim_str     =     trim_str_var    .trim    (    )    ;
    myco_print(    "    â    œ    “         S    t    r    i    n    g    .    t    r    i    m    (    '              h    e    l    l    o              '    )    :         '    "     +     trim_str     +     "    '    "    )    ;
    tests_passed     =     tests_passed     +     1.000000    ;
    total_tests     =     total_tests     +     1.000000    ;
    int     len_result     =     "    h    e    l    l    o    "    .length    (    )    ;
    myco_print(    "    â    œ    “         l    e    n    (    '    h    e    l    l    o    '    )    :         "     +     len_result    .toString    (    )    )    ;
    tests_passed     =     tests_passed     +     1.000000    ;
    myco_print(    "    \\    n    1    2    .         I    n    p    u    t         F    u    n    c    t    i    o    n    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    myco_print(    "    â    œ    “         I    n    p    u    t         f    u    n    c    t    i    o    n         a    v    a    i    l    a    b    l    e         (    t    e    s    t    e    d         m    a    n    u    a    l    l    y    )    "    )    ;
    tests_passed     =     tests_passed     +     1.000000    ;
    myco_print(    "    \\    n    1    3    .         A    r    r    a    y         L    i    b    r    a    r    y         F    u    n    c    t    i    o    n    s    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    myco_print(    "    â    œ    “         A    r    r    a    y         l    i    b    r    a    r    y         i    m    p    o    r    t    e    d         s    u    c    c    e    s    s    f    u    l    l    y    "    )    ;
    tests_passed     =     tests_passed     +     1.000000    ;
    myco_print(    "    \\    n    1    3    .    1    .         E    n    h    a    n    c    e    d         A    r    r    a    y         O    p    e    r    a    t    i    o    n    s    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    double*     test_array     =     (double[]){    1.000000    ,     2.000000    ,     3.000000    ,     4.000000    ,     5.000000    }    ;
    char*     joined_result     =     test_array    .join    (    "    ,         "    )    ;
    if (    joined_result    .type    (    )     ==     "    S    t    r    i    n    g    "     &&     joined_result     != NULL    ) {
        {
            myco_print(            "            â            œ            “                         a            r            r            a            y            .            j            o            i            n            (            )                         s            u            c            c            e            s            s            f            u            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         a            r            r            a            y            .            j            o            i            n            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     contains_result     =     test_array    .contains    (    3.000000    )    ;
    if (    contains_result     ==     1    ) {
        {
            myco_print(            "            â            œ            “                         a            r            r            a            y            .            c            o            n            t            a            i            n            s            (            )                         w            i            t            h                         e            x            i            s            t            i            n            g                         v            a            l            u            e            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         a            r            r            a            y            .            c            o            n            t            a            i            n            s            (            )                         w            i            t            h                         e            x            i            s            t            i            n            g                         v            a            l            u            e                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     contains_missing     =     test_array    .contains    (    6.000000    )    ;
    if (    contains_missing     ==     0    ) {
        {
            myco_print(            "            â            œ            “                         a            r            r            a            y            .            c            o            n            t            a            i            n            s            (            )                         w            i            t            h                         m            i            s            s            i            n            g                         v            a            l            u            e            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         a            r            r            a            y            .            c            o            n            t            a            i            n            s            (            )                         w            i            t            h                         m            i            s            s            i            n            g                         v            a            l            u            e                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     index_of_result     =     test_array    .indexOf    (    3.000000    )    ;
    if (    index_of_result     ==     2.000000    ) {
        {
            myco_print(            "            â            œ            “                         a            r            r            a            y            .            i            n            d            e            x            O            f            (            )                         w            i            t            h                         e            x            i            s            t            i            n            g                         v            a            l            u            e            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         a            r            r            a            y            .            i            n            d            e            x            O            f            (            )                         w            i            t            h                         e            x            i            s            t            i            n            g                         v            a            l            u            e                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     index_of_missing     =     test_array    .indexOf    (    6.000000    )    ;
    if (    index_of_missing     ==     -    1.000000    ) {
        {
            myco_print(            "            â            œ            “                         a            r            r            a            y            .            i            n            d            e            x            O            f            (            )                         w            i            t            h                         m            i            s            s            i            n            g                         v            a            l            u            e            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         a            r            r            a            y            .            i            n            d            e            x            O            f            (            )                         w            i            t            h                         m            i            s            s            i            n            g                         v            a            l            u            e                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double*     duplicates_array     =     (double[]){    1.000000    ,     2.000000    ,     2.000000    ,     3.000000    ,     3.000000    ,     3.000000    ,     4.000000    }    ;
    char*     unique_result     =     duplicates_array    .unique    (    )    ;
    if (    unique_result    .type    (    )     ==     "    A    r    r    a    y    "     &&     unique_result     != NULL    ) {
        {
            myco_print(            "            â            œ            “                         a            r            r            a            y            .            u            n            i            q            u            e            (            )                         s            u            c            c            e            s            s            f            u            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         a            r            r            a            y            .            u            n            i            q            u            e            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double*     array1     =     (double[]){    1.000000    ,     2.000000    ,     3.000000    }    ;
    double*     array2     =     (double[]){    4.000000    ,     5.000000    ,     6.000000    }    ;
    char*     concat_result     =     array1    .concat    (    array2    )    ;
    if (    concat_result    .type    (    )     ==     "    A    r    r    a    y    "     &&     concat_result     != NULL    ) {
        {
            myco_print(            "            â            œ            “                         a            r            r            a            y            .            c            o            n            c            a            t            (            )                         s            u            c            c            e            s            s            f            u            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         a            r            r            a            y            .            c            o            n            c            a            t            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     slice_result     =     test_array    .slice    (    1.000000    ,     4.000000    )    ;
    if (    slice_result    .type    (    )     ==     "    A    r    r    a    y    "     &&     slice_result     != NULL    ) {
        {
            myco_print(            "            â            œ            “                         a            r            r            a            y            .            s            l            i            c            e            (            )                         s            u            c            c            e            s            s            f            u            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         a            r            r            a            y            .            s            l            i            c            e            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char**     fill_array     =     (double[]){    }    ;
    // Unsupported statement type: 32
    if (    fill_array    .type    (    )     ==     "    A    r    r    a    y    "    ) {
        {
            myco_print(            "            â            œ            “                         a            r            r            a            y            .            f            i            l            l            (            )                         s            u            c            c            e            s            s            f            u            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         a            r            r            a            y            .            f            i            l            l            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    1    4    .         F    i    l    e         L    i    b    r    a    r    y         F    u    n    c    t    i    o    n    s    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     test_content     =     "    H    e    l    l    o    ,         M    y    c    o         F    i    l    e         S    y    s    t    e    m    !    "    ;
    char*     write_result     =     NULL    .write    (    "    t    e    s    t    _    f    i    l    e    .    t    x    t    "    ,     test_content    )    ;
    if (    write_result     ==     NULL    ) {
        {
            myco_print(            "            â            œ            “                         F            i            l            e                         w            r            i            t            e                         s            u            c            c            e            s            s            f            u            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         F            i            l            e                         w            r            i            t            e                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     exists_result     =     1    ;
    if (    exists_result     ==     1    ) {
        {
            myco_print(            "            â            œ            “                         F            i            l            e                         e            x            i            s            t            s                         c            h            e            c            k                         s            u            c            c            e            s            s            f            u            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         F            i            l            e                         e            x            i            s            t            s                         c            h            e            c            k                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     read_result     =     NULL    .read    (    "    t    e    s    t    _    f    i    l    e    .    t    x    t    "    )    ;
    if (    read_result     != NULL     &&     read_result    .type    (    )     ==     "    S    t    r    i    n    g    "    ) {
        {
            myco_print(            "            â            œ            “                         F            i            l            e                         r            e            a            d                         s            u            c            c            e            s            s            f            u            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         F            i            l            e                         r            e            a            d                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    HttpResponse     delete_result     =     NULL    .delete    (    "    t    e    s    t    _    f    i    l    e    .    t    x    t    "    )    ;
    if (    delete_result     ==     NULL    ) {
        {
            myco_print(            "            â            œ            “                         F            i            l            e                         d            e            l            e            t            e                         s            u            c            c            e            s            s            f            u            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         F            i            l            e                         d            e            l            e            t            e                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    1    5    .         D    i    r    e    c    t    o    r    y         L    i    b    r    a    r    y         F    u    n    c    t    i    o    n    s    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     current_dir     =     "/current/directory"    ;
    if (    current_dir     != NULL     &&     current_dir    .type    (    )     ==     "    S    t    r    i    n    g    "    ) {
        {
            myco_print(            "            â            œ            “                         C            u            r            r            e            n            t                         d            i            r            e            c            t            o            r            y                         s            u            c            c            e            s            s            f            u            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         C            u            r            r            e            n            t                         d            i            r            e            c            t            o            r            y                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     files     =     "[\"file1\", \"file2\"]"    ;
    if (    files     != NULL     &&     files    .type    (    )     ==     "    A    r    r    a    y    "    ) {
        {
            myco_print(            "            â            œ            “                         D            i            r            e            c            t            o            r            y                         l            i            s            t            i            n            g                         s            u            c            c            e            s            s            f            u            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         D            i            r            e            c            t            o            r            y                         l            i            s            t            i            n            g                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     create_result     =     NULL    ;
    if (    create_result     ==     NULL    ) {
        {
            myco_print(            "            â            œ            “                         D            i            r            e            c            t            o            r            y                         c            r            e            a            t            i            o            n                         s            u            c            c            e            s            s            f            u            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
            total_tests             =             total_tests             +             1.000000            ;
            int             exists_result_scope_1             =             1            ;
            if (            exists_result             ==             1            ) {
                {
                    myco_print(                    "                    â                    œ                    “                                         D                    i                    r                    e                    c                    t                    o                    r                    y                                         e                    x                    i                    s                    t                    s                                         c                    h                    e                    c                    k                                         s                    u                    c                    c                    e                    s                    s                    f                    u                    l                    "                    )                    ;
                    tests_passed                     =                     tests_passed                     +                     1.000000                    ;
                }
            } else {
                {
                    myco_print(                    "                    â                    œ                    —                                         D                    i                    r                    e                    c                    t                    o                    r                    y                                         e                    x                    i                    s                    t                    s                                         c                    h                    e                    c                    k                                         f                    a                    i                    l                    e                    d                    "                    )                    ;
                    tests_failed                     =                     tests_failed                    ;
                }
            }
            // Unsupported statement type: 32
        }
    } else {
        {
            myco_print(            "            â            œ            —                         D            i            r            e            c            t            o            r            y                         c            r            e            a            t            i            o            n                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    return 0;
}
