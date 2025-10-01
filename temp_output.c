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
        myco_print(        "        H        e        l        l        o                 f        r        o        m                 s        i        m        p        l        e                 f        u        n        c        t        i        o        n        !        "        )        ;
    }
}
void greet_with_name(char* name) {
    {
        myco_print(        myco_string_concat(        myco_string_concat(        "        H        e        l        l        o        ,                 "        ,         name        )        ,         "        !        "        )        )        ;
    }
}
void add_numbers(double a, double b) {
    {
        double         result_scope_1         =         a         +         b        ;
        myco_print(        myco_string_concat(        "        S        u        m        :                 "        ,         myco_number_to_string(        result_scope_1        )        )        )        ;
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
        return         myco_string_concat(        myco_string_concat(        "        H        e        l        l        o        ,                 "        ,         name        )        ,         "        !        "        )        ;    }
}
char* mixed_func1(double x, double y) {
    {
        return         myco_string_concat(        myco_string_concat(        myco_string_concat(        "        M        i        x        e        d                 t        y        p        e        s        :                 "        ,         myco_number_to_string(        x        )        )        ,         "        ,                 "        )        ,         myco_number_to_string(        y        )        )        ;    }
}
void mixed_func2(char* name) {
    {
        myco_print(        myco_string_concat(        "        N        a        m        e        :                 "        ,         name        )        )        ;
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
        return         "        H        e        l        l        o        "        ;    }
}
char* explicit_all(int param) {
    {
        return         myco_number_to_string(        param        )        ;    }
}
// Main program entry point
int main(void) {
    myco_print(    "    =    =    =         M    y    c    o         C    o    m    p    r    e    h    e    n    s    i    v    e         T    e    s    t         S    u    i    t    e         =    =    =    "    )    ;
    myco_print(    "    T    e    s    t    i    n    g         E    V    E    R    Y         f    e    a    t    u    r    e         a    n    d         e    d    g    e         c    a    s    e         i    n         M    y    c    o    "    )    ;
    myco_print(    "    "    )    ;
    double     total_tests     =     0.000000    ;
    double     tests_passed     =     0.000000    ;
    char**     tests_failed     =     (char*[]){    }    ;
    myco_print(    "    =    =    =         1    .         B    A    S    I    C         D    A    T    A         T    Y    P    E    S         =    =    =    "    )    ;
    myco_print(    "    1    .    1    .         N    u    m    b    e    r    s    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     x     =     42.000000    ;
    if (    x     ==     42.000000    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         I            n            t            e            g            e            r                         v            a            r            i            a            b            l            e            :                         "            ,             myco_number_to_string(            x            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         I            n            t            e            g            e            r                         v            a            r            i            a            b            l            e            :                         "            ,             myco_number_to_string(            x            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     y     =     3.140000    ;
    if (    y     ==     3.140000    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         F            l            o            a            t                         v            a            r            i            a            b            l            e            :                         "            ,             myco_number_to_string(            y            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         F            l            o            a            t                         v            a            r            i            a            b            l            e            :                         "            ,             myco_number_to_string(            y            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     neg_int     =     -    10.000000    ;
    if (    neg_int     ==     -    10.000000    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         N            e            g            a            t            i            v            e                         i            n            t            e            g            e            r            :                         "            ,             myco_number_to_string(            neg_int            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         N            e            g            a            t            i            v            e                         i            n            t            e            g            e            r            :                         "            ,             myco_number_to_string(            neg_int            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     neg_float     =     -    3.140000    ;
    if (    neg_float     ==     -    3.140000    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         N            e            g            a            t            i            v            e                         f            l            o            a            t            :                         "            ,             myco_number_to_string(            neg_float            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         N            e            g            a            t            i            v            e                         f            l            o            a            t            :                         "            ,             myco_number_to_string(            neg_float            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     zero     =     0.000000    ;
    if (    zero     ==     0.000000    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         Z            e            r            o            :                         "            ,             myco_number_to_string(            zero            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         Z            e            r            o            :                         "            ,             myco_number_to_string(            zero            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    1    .    2    .         S    t    r    i    n    g    s    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     name     =     "    M    y    c    o    "    ;
    if (    strcmp(    name    ,     "    M    y    c    o    "    ) == 0    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         S            t            r            i            n            g                         v            a            r            i            a            b            l            e            :                         "            ,             name            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         S            t            r            i            n            g                         v            a            r            i            a            b            l            e            :                         "            ,             name            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     empty_str     =     "    "    ;
    if (    strcmp(    empty_str    ,     "    "    ) == 0    ) {
        {
            myco_print(            myco_string_concat(            myco_string_concat(            "            â            œ            “                         E            m            p            t            y                         s            t            r            i            n            g            :                         '            "            ,             empty_str            )            ,             "            '            "            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            myco_string_concat(            "            â            œ            —                         E            m            p            t            y                         s            t            r            i            n            g            :                         '            "            ,             empty_str            )            ,             "            '            "            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     spaced_str     =     "    h    e    l    l    o         w    o    r    l    d    "    ;
    if (    strcmp(    spaced_str    ,     "    h    e    l    l    o         w    o    r    l    d    "    ) == 0    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         S            t            r            i            n            g                         w            i            t            h                         s            p            a            c            e            s            :                         "            ,             spaced_str            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         S            t            r            i            n            g                         w            i            t            h                         s            p            a            c            e            s            :                         "            ,             spaced_str            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    1    .    3    .         B    o    o    l    e    a    n    s    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    int     flag     =     1    ;
    if (    flag     ==     1    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         B            o            o            l            e            a            n                         T            r            u            e            :                         "            ,             myco_number_to_string(            flag            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         B            o            o            l            e            a            n                         T            r            u            e            :                         "            ,             myco_number_to_string(            flag            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     false_flag     =     0    ;
    if (    false_flag     ==     0    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         B            o            o            l            e            a            n                         F            a            l            s            e            :                         "            ,             myco_number_to_string(            false_flag            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         B            o            o            l            e            a            n                         F            a            l            s            e            :                         "            ,             myco_number_to_string(            false_flag            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    1    .    4    .         N    u    l    l    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     null_var     =     NULL    ;
    if (    null_var     ==     NULL    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         N            u            l            l                         v            a            r            i            a            b            l            e            :                         "            ,             myco_string_to_string(            null_var            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         N            u            l            l                         v            a            r            i            a            b            l            e            :                         "            ,             myco_string_to_string(            null_var            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    =    =    =         2    .         A    R    I    T    H    M    E    T    I    C         O    P    E    R    A    T    I    O    N    S         =    =    =    "    )    ;
    myco_print(    "    2    .    1    .         A    d    d    i    t    i    o    n    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     a     =     5.000000     +     3.000000    ;
    if (    a     ==     8.000000    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         I            n            t            e            g            e            r                         a            d            d            i            t            i            o            n                         (            5                         +                         3            )            :                         "            ,             myco_number_to_string(            a            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         I            n            t            e            g            e            r                         a            d            d            i            t            i            o            n                         (            5                         +                         3            )            :                         "            ,             myco_number_to_string(            a            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     float_add     =     2.500000     +     1.500000    ;
    if (    float_add     ==     4.000000    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         F            l            o            a            t                         a            d            d            i            t            i            o            n                         (            2            .            5                         +                         1            .            5            )            :                         "            ,             myco_number_to_string(            float_add            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         F            l            o            a            t                         a            d            d            i            t            i            o            n                         (            2            .            5                         +                         1            .            5            )            :                         "            ,             myco_number_to_string(            float_add            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     mixed_add     =     5.000000     +     2.500000    ;
    if (    mixed_add     ==     7.500000    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         M            i            x            e            d                         a            d            d            i            t            i            o            n                         (            5                         +                         2            .            5            )            :                         "            ,             myco_number_to_string(            mixed_add            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         M            i            x            e            d                         a            d            d            i            t            i            o            n                         (            5                         +                         2            .            5            )            :                         "            ,             myco_number_to_string(            mixed_add            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     str_concat     =     myco_string_concat(    "    h    e    l    l    o    "    ,     "    w    o    r    l    d    "    )    ;
    if (    strcmp(    str_concat    ,     "    h    e    l    l    o    w    o    r    l    d    "    ) == 0    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         S            t            r            i            n            g                         c            o            n            c            a            t            e            n            a            t            i            o            n            :                         "            ,             str_concat            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         S            t            r            i            n            g                         c            o            n            c            a            t            e            n            a            t            i            o            n            :                         "            ,             str_concat            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    2    .    2    .         S    u    b    t    r    a    c    t    i    o    n    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     b     =     10.000000     -     4.000000    ;
    if (    b     ==     6.000000    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         I            n            t            e            g            e            r                         s            u            b            t            r            a            c            t            i            o            n                         (            1            0                         -                         4            )            :                         "            ,             myco_number_to_string(            b            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         I            n            t            e            g            e            r                         s            u            b            t            r            a            c            t            i            o            n                         (            1            0                         -                         4            )            :                         "            ,             myco_number_to_string(            b            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     float_sub     =     5.500000     -     2.500000    ;
    if (    float_sub     ==     3.000000    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         F            l            o            a            t                         s            u            b            t            r            a            c            t            i            o            n                         (            5            .            5                         -                         2            .            5            )            :                         "            ,             myco_number_to_string(            float_sub            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         F            l            o            a            t                         s            u            b            t            r            a            c            t            i            o            n                         (            5            .            5                         -                         2            .            5            )            :                         "            ,             myco_number_to_string(            float_sub            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     neg_result     =     3.000000     -     5.000000    ;
    if (    neg_result     ==     -    2.000000    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         N            e            g            a            t            i            v            e                         r            e            s            u            l            t                         (            3                         -                         5            )            :                         "            ,             myco_number_to_string(            neg_result            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         N            e            g            a            t            i            v            e                         r            e            s            u            l            t                         (            3                         -                         5            )            :                         "            ,             myco_number_to_string(            neg_result            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    2    .    3    .         M    u    l    t    i    p    l    i    c    a    t    i    o    n    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     c     =     6.000000     *     7.000000    ;
    if (    c     ==     42.000000    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         I            n            t            e            g            e            r                         m            u            l            t            i            p            l            i            c            a            t            i            o            n                         (            6                         *                         7            )            :                         "            ,             myco_number_to_string(            c            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         I            n            t            e            g            e            r                         m            u            l            t            i            p            l            i            c            a            t            i            o            n                         (            6                         *                         7            )            :                         "            ,             myco_number_to_string(            c            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     float_mult     =     2.500000     *     4.000000    ;
    if (    float_mult     ==     10.000000    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         F            l            o            a            t                         m            u            l            t            i            p            l            i            c            a            t            i            o            n                         (            2            .            5                         *                         4            )            :                         "            ,             myco_number_to_string(            float_mult            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         F            l            o            a            t                         m            u            l            t            i            p            l            i            c            a            t            i            o            n                         (            2            .            5                         *                         4            )            :                         "            ,             myco_number_to_string(            float_mult            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     zero_mult     =     5.000000     *     0.000000    ;
    if (    zero_mult     ==     0.000000    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         Z            e            r            o                         m            u            l            t            i            p            l            i            c            a            t            i            o            n                         (            5                         *                         0            )            :                         "            ,             myco_number_to_string(            zero_mult            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         Z            e            r            o                         m            u            l            t            i            p            l            i            c            a            t            i            o            n                         (            5                         *                         0            )            :                         "            ,             myco_number_to_string(            zero_mult            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    2    .    4    .         D    i    v    i    s    i    o    n    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     d     =     15.000000     /     3.000000    ;
    if (    d     ==     5.000000    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         I            n            t            e            g            e            r                         d            i            v            i            s            i            o            n                         (            1            5                         /                         3            )            :                         "            ,             myco_number_to_string(            d            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         I            n            t            e            g            e            r                         d            i            v            i            s            i            o            n                         (            1            5                         /                         3            )            :                         "            ,             myco_number_to_string(            d            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     float_div     =     10.000000     /     2.500000    ;
    if (    float_div     ==     4.000000    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         F            l            o            a            t                         d            i            v            i            s            i            o            n                         (            1            0            .            0                         /                         2            .            5            )            :                         "            ,             myco_number_to_string(            float_div            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         F            l            o            a            t                         d            i            v            i            s            i            o            n                         (            1            0            .            0                         /                         2            .            5            )            :                         "            ,             myco_number_to_string(            float_div            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     remainder_div     =     7.000000     /     2.000000    ;
    if (    remainder_div     ==     3.500000    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         D            i            v            i            s            i            o            n                         w            i            t            h                         r            e            m            a            i            n            d            e            r                         (            7                         /                         2            )            :                         "            ,             myco_number_to_string(            remainder_div            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         D            i            v            i            s            i            o            n                         w            i            t            h                         r            e            m            a            i            n            d            e            r                         (            7                         /                         2            )            :                         "            ,             myco_number_to_string(            remainder_div            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    =    =    =         3    .         C    O    M    P    A    R    I    S    O    N         O    P    E    R    A    T    I    O    N    S         =    =    =    "    )    ;
    myco_print(    "    3    .    1    .         E    q    u    a    l    i    t    y    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     comp3     =     5.000000     ==     5.000000    ;
    if (    comp3     ==     1    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         I            n            t            e            g            e            r                         e            q            u            a            l            i            t            y                         (            5                         =            =                         5            )            :                         "            ,             myco_number_to_string(            comp3            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         I            n            t            e            g            e            r                         e            q            u            a            l            i            t            y                         (            5                         =            =                         5            )            :                         "            ,             myco_number_to_string(            comp3            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     comp_neq     =     5.000000     ==     6.000000    ;
    if (    comp_neq     ==     0    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         I            n            t            e            g            e            r                         i            n            e            q            u            a            l            i            t            y                         (            5                         =            =                         6            )            :                         "            ,             myco_number_to_string(            comp_neq            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         I            n            t            e            g            e            r                         i            n            e            q            u            a            l            i            t            y                         (            5                         =            =                         6            )            :                         "            ,             myco_number_to_string(            comp_neq            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     float_eq     =     3.140000     ==     3.140000    ;
    if (    float_eq     ==     1    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         F            l            o            a            t                         e            q            u            a            l            i            t            y                         (            3            .            1            4                         =            =                         3            .            1            4            )            :                         "            ,             myco_number_to_string(            float_eq            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         F            l            o            a            t                         e            q            u            a            l            i            t            y                         (            3            .            1            4                         =            =                         3            .            1            4            )            :                         "            ,             myco_number_to_string(            float_eq            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     str_eq     =     strcmp(    "    h    e    l    l    o    "    ,     "    h    e    l    l    o    "    ) == 0    ;
    if (    str_eq     ==     1    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         S            t            r            i            n            g                         e            q            u            a            l            i            t            y                         (            '            h            e            l            l            o            '                         =            =                         '            h            e            l            l            o            '            )            :                         "            ,             myco_number_to_string(            str_eq            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         S            t            r            i            n            g                         e            q            u            a            l            i            t            y                         (            '            h            e            l            l            o            '                         =            =                         '            h            e            l            l            o            '            )            :                         "            ,             myco_number_to_string(            str_eq            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     bool_eq     =     1     ==     1    ;
    if (    bool_eq     ==     1    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         B            o            o            l            e            a            n                         e            q            u            a            l            i            t            y                         (            T            r            u            e                         =            =                         T            r            u            e            )            :                         "            ,             myco_number_to_string(            bool_eq            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         B            o            o            l            e            a            n                         e            q            u            a            l            i            t            y                         (            T            r            u            e                         =            =                         T            r            u            e            )            :                         "            ,             myco_number_to_string(            bool_eq            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    3    .    2    .         I    n    e    q    u    a    l    i    t    y    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     comp4     =     4.000000     !=     6.000000    ;
    if (    comp4     ==     1    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         I            n            t            e            g            e            r                         i            n            e            q            u            a            l            i            t            y                         (            4                         !            =                         6            )            :                         "            ,             myco_number_to_string(            comp4            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         I            n            t            e            g            e            r                         i            n            e            q            u            a            l            i            t            y                         (            4                         !            =                         6            )            :                         "            ,             myco_number_to_string(            comp4            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     str_neq     =     strcmp(    "    h    e    l    l    o    "    ,     "    w    o    r    l    d    "    ) != 0    ;
    if (    str_neq     ==     1    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         S            t            r            i            n            g                         i            n            e            q            u            a            l            i            t            y                         (            '            h            e            l            l            o            '                         !            =                         '            w            o            r            l            d            '            )            :                         "            ,             myco_number_to_string(            str_neq            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         S            t            r            i            n            g                         i            n            e            q            u            a            l            i            t            y                         (            '            h            e            l            l            o            '                         !            =                         '            w            o            r            l            d            '            )            :                         "            ,             myco_number_to_string(            str_neq            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    3    .    3    .         G    r    e    a    t    e    r         t    h    a    n    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     comp1     =     10.000000     >     5.000000    ;
    if (    comp1     ==     1    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         G            r            e            a            t            e            r                         t            h            a            n                         (            1            0                         >                         5            )            :                         "            ,             myco_number_to_string(            comp1            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         G            r            e            a            t            e            r                         t            h            a            n                         (            1            0                         >                         5            )            :                         "            ,             myco_number_to_string(            comp1            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     comp1_false     =     5.000000     >     10.000000    ;
    if (    comp1_false     ==     0    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         N            o            t                         g            r            e            a            t            e            r                         t            h            a            n                         (            5                         >                         1            0            )            :                         "            ,             myco_number_to_string(            comp1_false            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         N            o            t                         g            r            e            a            t            e            r                         t            h            a            n                         (            5                         >                         1            0            )            :                         "            ,             myco_number_to_string(            comp1_false            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    3    .    4    .         L    e    s    s         t    h    a    n    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     comp2     =     3.000000     <     7.000000    ;
    if (    comp2     ==     1    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         L            e            s            s                         t            h            a            n                         (            3                         <                         7            )            :                         "            ,             myco_number_to_string(            comp2            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         L            e            s            s                         t            h            a            n                         (            3                         <                         7            )            :                         "            ,             myco_number_to_string(            comp2            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     comp2_false     =     7.000000     <     3.000000    ;
    if (    comp2_false     ==     0    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         N            o            t                         l            e            s            s                         t            h            a            n                         (            7                         <                         3            )            :                         "            ,             myco_number_to_string(            comp2_false            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         N            o            t                         l            e            s            s                         t            h            a            n                         (            7                         <                         3            )            :                         "            ,             myco_number_to_string(            comp2_false            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    3    .    5    .         G    r    e    a    t    e    r         t    h    a    n         o    r         e    q    u    a    l    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     gte_true     =     10.000000     >=     5.000000    ;
    if (    gte_true     ==     1    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         G            r            e            a            t            e            r                         t            h            a            n                         o            r                         e            q            u            a            l                         (            1            0                         >            =                         5            )            :                         "            ,             myco_number_to_string(            gte_true            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         G            r            e            a            t            e            r                         t            h            a            n                         o            r                         e            q            u            a            l                         (            1            0                         >            =                         5            )            :                         "            ,             myco_number_to_string(            gte_true            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     gte_equal     =     5.000000     >=     5.000000    ;
    if (    gte_equal     ==     1    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         E            q            u            a            l                         v            a            l            u            e            s                         (            5                         >            =                         5            )            :                         "            ,             myco_number_to_string(            gte_equal            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         E            q            u            a            l                         v            a            l            u            e            s                         (            5                         >            =                         5            )            :                         "            ,             myco_number_to_string(            gte_equal            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    3    .    6    .         L    e    s    s         t    h    a    n         o    r         e    q    u    a    l    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     lte_true     =     3.000000     <=     7.000000    ;
    if (    lte_true     ==     1    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         L            e            s            s                         t            h            a            n                         o            r                         e            q            u            a            l                         (            3                         <            =                         7            )            :                         "            ,             myco_number_to_string(            lte_true            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         L            e            s            s                         t            h            a            n                         o            r                         e            q            u            a            l                         (            3                         <            =                         7            )            :                         "            ,             myco_number_to_string(            lte_true            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     lte_equal     =     5.000000     <=     5.000000    ;
    if (    lte_equal     ==     1    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         E            q            u            a            l                         v            a            l            u            e            s                         (            5                         <            =                         5            )            :                         "            ,             myco_number_to_string(            lte_equal            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         E            q            u            a            l                         v            a            l            u            e            s                         (            5                         <            =                         5            )            :                         "            ,             myco_number_to_string(            lte_equal            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    =    =    =         4    .         L    O    G    I    C    A    L         O    P    E    R    A    T    I    O    N    S         =    =    =    "    )    ;
    myco_print(    "    4    .    1    .         L    o    g    i    c    a    l         A    N    D    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     log1     =     1     &&     1    ;
    if (    log1     ==     1    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         L            o            g            i            c            a            l                         A            N            D                         (            T            r            u            e                         a            n            d                         T            r            u            e            )            :                         "            ,             myco_number_to_string(            log1            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         L            o            g            i            c            a            l                         A            N            D                         (            T            r            u            e                         a            n            d                         T            r            u            e            )            :                         "            ,             myco_number_to_string(            log1            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     log1_false     =     1     &&     0    ;
    if (    log1_false     ==     0    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         L            o            g            i            c            a            l                         A            N            D                         f            a            l            s            e                         (            T            r            u            e                         a            n            d                         F            a            l            s            e            )            :                         "            ,             myco_number_to_string(            log1_false            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         L            o            g            i            c            a            l                         A            N            D                         f            a            l            s            e                         (            T            r            u            e                         a            n            d                         F            a            l            s            e            )            :                         "            ,             myco_number_to_string(            log1_false            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    4    .    2    .         L    o    g    i    c    a    l         O    R    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     log2     =     0     ||     1    ;
    if (    log2     ==     1    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         L            o            g            i            c            a            l                         O            R                         (            F            a            l            s            e                         o            r                         T            r            u            e            )            :                         "            ,             myco_number_to_string(            log2            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         L            o            g            i            c            a            l                         O            R                         (            F            a            l            s            e                         o            r                         T            r            u            e            )            :                         "            ,             myco_number_to_string(            log2            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     log2_false     =     0     ||     0    ;
    if (    log2_false     ==     0    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         L            o            g            i            c            a            l                         O            R                         f            a            l            s            e                         (            F            a            l            s            e                         o            r                         F            a            l            s            e            )            :                         "            ,             myco_number_to_string(            log2_false            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         L            o            g            i            c            a            l                         O            R                         f            a            l            s            e                         (            F            a            l            s            e                         o            r                         F            a            l            s            e            )            :                         "            ,             myco_number_to_string(            log2_false            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    4    .    3    .         L    o    g    i    c    a    l         N    O    T    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     log3     =     !    0    ;
    if (    log3     ==     1    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         L            o            g            i            c            a            l                         N            O            T                         (            n            o            t                         F            a            l            s            e            )            :                         "            ,             myco_number_to_string(            log3            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         L            o            g            i            c            a            l                         N            O            T                         (            n            o            t                         F            a            l            s            e            )            :                         "            ,             myco_number_to_string(            log3            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     log3_false     =     !    1    ;
    if (    log3_false     ==     0    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         L            o            g            i            c            a            l                         N            O            T                         f            a            l            s            e                         (            n            o            t                         T            r            u            e            )            :                         "            ,             myco_number_to_string(            log3_false            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         L            o            g            i            c            a            l                         N            O            T                         f            a            l            s            e                         (            n            o            t                         T            r            u            e            )            :                         "            ,             myco_number_to_string(            log3_false            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    4    .    4    .         C    o    m    p    l    e    x         l    o    g    i    c    a    l         e    x    p    r    e    s    s    i    o    n    s    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     complex_log     =     1     &&     0     ||     1    ;
    if (    complex_log     ==     1    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         C            o            m            p            l            e            x                         l            o            g            i            c            a            l                         (            (            T            r            u            e                         a            n            d                         F            a            l            s            e            )                         o            r                         T            r            u            e            )            :                         "            ,             myco_number_to_string(            complex_log            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         C            o            m            p            l            e            x                         l            o            g            i            c            a            l                         (            (            T            r            u            e                         a            n            d                         F            a            l            s            e            )                         o            r                         T            r            u            e            )            :                         "            ,             myco_number_to_string(            complex_log            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     nested_not     =     !    1     &&     0    ;
    if (    nested_not     ==     1    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         N            e            s            t            e            d                         N            O            T                         (            n            o            t                         (            T            r            u            e                         a            n            d                         F            a            l            s            e            )            )            :                         "            ,             myco_number_to_string(            nested_not            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         N            e            s            t            e            d                         N            O            T                         (            n            o            t                         (            T            r            u            e                         a            n            d                         F            a            l            s            e            )            )            :                         "            ,             myco_number_to_string(            nested_not            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    5    .         A    r    r    a    y    s    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    double*     arr     =     (double[]){    1.000000    ,     2.000000    ,     3.000000    }    ;
    if (    sizeof(    arr    ) / sizeof(    arr    [0])     ==     3.000000    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         S            i            m            p            l            e                         a            r            r            a            y            :                         "            ,             myco_safe_to_string(            arr            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         S            i            m            p            l            e                         a            r            r            a            y            :                         "            ,             myco_safe_to_string(            arr            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char**     nested     =     (char*[]){    (double[]){    1.000000    ,     2.000000    }    ,     (double[]){    3.000000    ,     4.000000    }    }    ;
    if (    sizeof(    nested    ) / sizeof(    nested    [0])     ==     2.000000    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         N            e            s            t            e            d                         a            r            r            a            y            :                         "            ,             myco_safe_to_string(            nested            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         N            e            s            t            e            d                         a            r            r            a            y            :                         "            ,             myco_safe_to_string(            nested            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char**     mixed     =     (char*[]){    myco_number_to_string(    1.000000    )    ,     "    h    e    l    l    o    "    ,     myco_number_to_string(    1    )    ,     (double[]){    1.000000    ,     2.000000    }    }    ;
    if (    sizeof(    mixed    ) / sizeof(    mixed    [0])     ==     4.000000    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         M            i            x            e            d                         t            y            p            e                         a            r            r            a            y            :                         "            ,             myco_safe_to_string(            mixed            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         M            i            x            e            d                         t            y            p            e                         a            r            r            a            y            :                         "            ,             myco_safe_to_string(            mixed            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char**     empty     =     (char*[]){    }    ;
    if (    sizeof(    empty    ) / sizeof(    empty    [0])     ==     0.000000    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         E            m            p            t            y                         a            r            r            a            y            :                         "            ,             myco_safe_to_string(            empty            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         E            m            p            t            y                         a            r            r            a            y            :                         "            ,             myco_safe_to_string(            empty            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    6    .         S    t    r    i    n    g         O    p    e    r    a    t    i    o    n    s    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     str1     =     "    H    e    l    l    o    "    ;
    char*     str2     =     "    W    o    r    l    d    "    ;
    char*     combined     =     myco_string_concat(    myco_string_concat(    str1    ,     "         "    )    ,     str2    )    ;
    myco_print(    myco_string_concat(    "    â    œ    “         S    t    r    i    n    g         c    o    n    c    a    t    e    n    a    t    i    o    n    :         "    ,     combined    )    )    ;
    tests_passed     =     tests_passed     +     1.000000    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     num_str     =     myco_number_to_string(    42.000000    )    ;
    myco_print(    myco_string_concat(    "    â    œ    “         N    u    m    b    e    r         t    o         s    t    r    i    n    g    :         "    ,     num_str    )    )    ;
    tests_passed     =     tests_passed     +     1.000000    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     bool_str     =     myco_number_to_string(    1    )    ;
    myco_print(    myco_string_concat(    "    â    œ    “         B    o    o    l    e    a    n         t    o         s    t    r    i    n    g    :         "    ,     bool_str    )    )    ;
    tests_passed     =     tests_passed     +     1.000000    ;
    myco_print(    "    \\    n    =    =    =         7    .         S    T    R    I    N    G         O    P    E    R    A    T    I    O    N    S         =    =    =    "    )    ;
    myco_print(    "    7    .    1    .         S    t    r    i    n    g         c    o    n    c    a    t    e    n    a    t    i    o    n    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     str1_2     =     "    H    e    l    l    o    "    ;
    char*     str2_2     =     "    W    o    r    l    d    "    ;
    char*     combined_2     =     myco_string_concat(    myco_string_concat(    str1_2    ,     "         "    )    ,     str2_2    )    ;
    if (    strcmp(    combined_2    ,     "    H    e    l    l    o         W    o    r    l    d    "    ) == 0    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         S            t            r            i            n            g                         c            o            n            c            a            t            e            n            a            t            i            o            n            :                         "            ,             combined_2            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         S            t            r            i            n            g                         c            o            n            c            a            t            e            n            a            t            i            o            n            :                         "            ,             combined_2            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     num_str_2     =     myco_number_to_string(    42.000000    )    ;
    if (    strcmp(    num_str_2    ,     "    4    2    "    ) == 0    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         N            u            m            b            e            r                         t            o                         s            t            r            i            n            g            :                         "            ,             num_str_2            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         N            u            m            b            e            r                         t            o                         s            t            r            i            n            g            :                         "            ,             num_str_2            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     bool_str_2     =     myco_number_to_string(    1    )    ;
    if (    strcmp(    bool_str_2    ,     "    T    r    u    e    "    ) == 0    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         B            o            o            l            e            a            n                         t            o                         s            t            r            i            n            g            :                         "            ,             bool_str_2            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         B            o            o            l            e            a            n                         t            o                         s            t            r            i            n            g            :                         "            ,             bool_str_2            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    7    .    2    .         E    s    c    a    p    e         s    e    q    u    e    n    c    e    s    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     escaped     =     "    L    i    n    e         1    \\    n    L    i    n    e         2    \\    t    T    a    b    b    e    d    "    ;
    if (    sizeof(    escaped    ) / sizeof(    escaped    [0])     >     0.000000    ) {
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
    if (    strcmp(    quoted_str    ,     "    H    e         s    a    i    d         \\    \"    H    e    l    l    o    \\    \"    "    ) == 0    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         Q            u            o            t            e            d                         s            t            r            i            n            g            :                         "            ,             quoted_str            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         Q            u            o            t            e            d                         s            t            r            i            n            g            :                         "            ,             quoted_str            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     backslash_str     =     "    P    a    t    h    :         C    :    \\    \\    U    s    e    r    s    \\    \\    N    a    m    e    "    ;
    if (    strcmp(    backslash_str    ,     "    P    a    t    h    :         C    :    \\    \\    U    s    e    r    s    \\    \\    N    a    m    e    "    ) == 0    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         B            a            c            k            s            l            a            s            h                         s            t            r            i            n            g            :                         "            ,             backslash_str            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         B            a            c            k            s            l            a            s            h                         s            t            r            i            n            g            :                         "            ,             backslash_str            )            )            ;
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
    if (    "Array"     ==     "    S    t    r    i    n    g    "     &&     test_string     != NULL    ) {
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
    if (    "Array"     ==     "    A    r    r    a    y    "     &&     test_array_1     != NULL    ) {
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
    if (    "Array"     ==     "    M    a    p    "    ) {
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
    if (    "Array"     ==     "    S    e    t    "    ) {
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
    if (    "Array"     ==     "    T    r    e    e    "    ) {
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
    if (    "Array"     ==     "    G    r    a    p    h    "    ) {
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
    double     pi_value     =     3.141592653589793    ;
    myco_print(    myco_string_concat(    "    â    œ    “         M    a    t    h    .    P    i    :         "    ,     myco_number_to_string(    pi_value    )    )    )    ;
    tests_passed     =     tests_passed     +     1.000000    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     e_value     =     2.718281828459045    ;
    myco_print(    myco_string_concat(    "    â    œ    “         M    a    t    h    .    E    :         "    ,     myco_number_to_string(    e_value    )    )    )    ;
    tests_passed     =     tests_passed     +     1.000000    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     abs_result     =     fabs    ;
    myco_print(    myco_string_concat(    "    â    œ    “         M    a    t    h    .    a    b    s    (    -    5    )    :         "    ,     myco_number_to_string(    abs_result    )    )    )    ;
    tests_passed     =     tests_passed     +     1.000000    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     min_result     =     fmin    ;
    myco_print(    myco_string_concat(    "    â    œ    “         M    a    t    h    .    m    i    n    (    1    0    ,         5    )    :         "    ,     myco_number_to_string(    min_result    )    )    )    ;
    tests_passed     =     tests_passed     +     1.000000    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     max_result     =     fmax    ;
    myco_print(    myco_string_concat(    "    â    œ    “         M    a    t    h    .    m    a    x    (    1    0    ,         5    )    :         "    ,     myco_number_to_string(    max_result    )    )    )    ;
    tests_passed     =     tests_passed     +     1.000000    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     sqrt_result     =     sqrt    ;
    myco_print(    myco_string_concat(    "    â    œ    “         M    a    t    h    .    s    q    r    t    (    1    6    )    :         "    ,     myco_number_to_string(    sqrt_result    )    )    )    ;
    tests_passed     =     tests_passed     +     1.000000    ;
    myco_print(    "    \\    n    1    1    .         S    t    r    i    n    g         L    i    b    r    a    r    y         F    u    n    c    t    i    o    n    s    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     test_str     =     "    h    e    l    l    o         w    o    r    l    d    "    ;
    char*     upper_str     =     test_str    .upper    (    )    ;
    myco_print(    myco_string_concat(    "    â    œ    “         S    t    r    i    n    g    .    u    p    p    e    r    (    '    h    e    l    l    o         w    o    r    l    d    '    )    :         "    ,     upper_str    )    )    ;
    tests_passed     =     tests_passed     +     1.000000    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     lower_str_var     =     "    H    E    L    L    O         W    O    R    L    D    "    ;
    char*     lower_str     =     lower_str_var    .lower    (    )    ;
    myco_print(    myco_string_concat(    "    â    œ    “         S    t    r    i    n    g    .    l    o    w    e    r    (    '    H    E    L    L    O         W    O    R    L    D    '    )    :         "    ,     lower_str    )    )    ;
    tests_passed     =     tests_passed     +     1.000000    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     trim_str_var     =     "              h    e    l    l    o              "    ;
    char*     trim_str     =     trim_str_var    .trim    (    )    ;
    myco_print(    myco_string_concat(    myco_string_concat(    "    â    œ    “         S    t    r    i    n    g    .    t    r    i    m    (    '              h    e    l    l    o              '    )    :         '    "    ,     trim_str    )    ,     "    '    "    )    )    ;
    tests_passed     =     tests_passed     +     1.000000    ;
    total_tests     =     total_tests     +     1.000000    ;
    int     len_result     =     sizeof(    "    h    e    l    l    o    "    ) / sizeof(    "    h    e    l    l    o    "    [0])    ;
    myco_print(    myco_string_concat(    "    â    œ    “         l    e    n    (    '    h    e    l    l    o    '    )    :         "    ,     myco_number_to_string(    len_result    )    )    )    ;
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
    if (    "Array"     ==     "    S    t    r    i    n    g    "     &&     joined_result     != NULL    ) {
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
    if (    "Array"     ==     "    A    r    r    a    y    "     &&     unique_result     != NULL    ) {
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
    if (    "Array"     ==     "    A    r    r    a    y    "     &&     concat_result     != NULL    ) {
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
    if (    "Array"     ==     "    A    r    r    a    y    "     &&     slice_result     != NULL    ) {
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
    char**     fill_array     =     (char*[]){    }    ;
    // Unsupported statement type: 32
    if (    "Array"     ==     "    A    r    r    a    y    "    ) {
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
    char*     write_result     =     &(struct { int dummy; }){0}    .write    (    "    t    e    s    t    _    f    i    l    e    .    t    x    t    "    ,     test_content    )    ;
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
    char*     read_result     =     &(struct { int dummy; }){0}    .read    (    "    t    e    s    t    _    f    i    l    e    .    t    x    t    "    )    ;
    if (    read_result     != NULL     &&     "Array"     ==     "    S    t    r    i    n    g    "    ) {
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
    HttpResponse     delete_result     =     &(struct { int dummy; }){0}    .delete    (    "    t    e    s    t    _    f    i    l    e    .    t    x    t    "    )    ;
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
    if (    current_dir     != NULL     &&     "Array"     ==     "    S    t    r    i    n    g    "    ) {
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
    if (    files     != NULL     &&     "Array"     ==     "    A    r    r    a    y    "    ) {
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
            if (            exists_result_scope_1             ==             1            ) {
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
    myco_print(    "    \\    n    1    6    .         M    a    p    s         L    i    b    r    a    r    y         F    u    n    c    t    i    o    n    s    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    void*     test_map     =     NULL    ;
    int     has_name     =     test_map    .has    (    "    n    a    m    e    "    )    ;
    if (    has_name     ==     1    ) {
        {
            myco_print(            "            â            œ            “                         m            a            p            s            .            h            a            s            (            )                         w            i            t            h                         e            x            i            s            t            i            n            g                         k            e            y            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         m            a            p            s            .            h            a            s            (            )                         w            i            t            h                         e            x            i            s            t            i            n            g                         k            e            y                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     has_missing     =     test_map    .has    (    "    s    a    l    a    r    y    "    )    ;
    if (    has_missing     ==     0    ) {
        {
            myco_print(            "            â            œ            “                         m            a            p            s            .            h            a            s            (            )                         w            i            t            h                         m            i            s            s            i            n            g                         k            e            y            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         m            a            p            s            .            h            a            s            (            )                         w            i            t            h                         m            i            s            s            i            n            g                         k            e            y                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     map_size     =     test_map    .size    (    )    ;
    if (    map_size     ==     3.000000    ) {
        {
            myco_print(            "            â            œ            “                         m            a            p            s            .            s            i            z            e            (            )                         r            e            t            u            r            n            s                         c            o            r            r            e            c            t                         c            o            u            n            t            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         m            a            p            s            .            s            i            z            e            (            )                         f            a            i            l            e            d            :                         "            ,             myco_number_to_string(            map_size            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     map_keys     =     test_map    .keys    (    )    ;
    if (    "Array"     ==     "    A    r    r    a    y    "     &&     map_keys     != NULL    ) {
        {
            myco_print(            "            â            œ            “                         m            a            p            s            .            k            e            y            s            (            )                         s            u            c            c            e            s            s            f            u            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         m            a            p            s            .            k            e            y            s            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    test_map     =     test_map    .delete    (    "    c    i    t    y    "    )    ;
    int     size_after_delete     =     test_map    .size    (    )    ;
    if (    size_after_delete     >=     2.000000    ) {
        {
            myco_print(            "            â            œ            “                         m            a            p            s            .            d            e            l            e            t            e            (            )                         e            x            e            c            u            t            e            d            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         m            a            p            s            .            d            e            l            e            t            e            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    void*     update_map     =     NULL    ;
    test_map     =     test_map    .update    (    update_map    )    ;
    int     size_after_update     =     test_map    .size    (    )    ;
    if (    size_after_update     >=     2.000000    ) {
        {
            myco_print(            "            â            œ            “                         m            a            p            s            .            u            p            d            a            t            e            (            )                         e            x            e            c            u            t            e            d            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         m            a            p            s            .            u            p            d            a            t            e            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    test_map     =     test_map    .clear    (    )    ;
    myco_print(    "    â    œ    “         m    a    p    s    .    c    l    e    a    r    (    )         e    x    e    c    u    t    e    d    "    )    ;
    tests_passed     =     tests_passed     +     1.000000    ;
    myco_print(    "    \\    n    1    7    .         S    e    t    s         L    i    b    r    a    r    y         F    u    n    c    t    i    o    n    s    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    void*     test_set     =     NULL    ;
    int     has_apple     =     test_set    .has    (    "    a    p    p    l    e    "    )    ;
    if (    has_apple     ==     1    ) {
        {
            myco_print(            "            â            œ            “                         s            e            t            .            h            a            s            (            )                         w            i            t            h                         e            x            i            s            t            i            n            g                         e            l            e            m            e            n            t            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         s            e            t            .            h            a            s            (            )                         w            i            t            h                         e            x            i            s            t            i            n            g                         e            l            e            m            e            n            t                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     has_orange     =     test_set    .has    (    "    o    r    a    n    g    e    "    )    ;
    if (    has_orange     ==     0    ) {
        {
            myco_print(            "            â            œ            “                         s            e            t            .            h            a            s            (            )                         w            i            t            h                         m            i            s            s            i            n            g                         e            l            e            m            e            n            t            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         s            e            t            .            h            a            s            (            )                         w            i            t            h                         m            i            s            s            i            n            g                         e            l            e            m            e            n            t                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     set_size     =     test_set    .size    (    )    ;
    if (    set_size     ==     3.000000    ) {
        {
            myco_print(            "            â            œ            “                         s            e            t            .            s            i            z            e            (            )                         r            e            t            u            r            n            s                         c            o            r            r            e            c            t                         c            o            u            n            t            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         s            e            t            .            s            i            z            e            (            )                         f            a            i            l            e            d            :                         "            ,             myco_number_to_string(            set_size            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     updated_set     =     test_set    .add    (    "    o    r    a    n    g    e    "    )    ;
    int     size_after_add     =     updated_set    .size    (    )    ;
    if (    size_after_add     >=     3.000000    ) {
        {
            myco_print(            "            â            œ            “                         s            e            t            .            a            d            d            (            )                         e            x            e            c            u            t            e            d            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         s            e            t            .            a            d            d            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     removed_set     =     test_set    .remove    (    "    b    a    n    a    n    a    "    )    ;
    int     size_after_remove     =     removed_set    .size    (    )    ;
    if (    size_after_remove     >=     2.000000    ) {
        {
            myco_print(            "            â            œ            “                         s            e            t            .            r            e            m            o            v            e            (            )                         e            x            e            c            u            t            e            d            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         s            e            t            .            r            e            m            o            v            e            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     set_to_array     =     test_set    .toArray    (    )    ;
    if (    "Array"     ==     "    A    r    r    a    y    "     &&     set_to_array     != NULL    ) {
        {
            myco_print(            "            â            œ            “                         s            e            t            .            t            o            A            r            r            a            y            (            )                         s            u            c            c            e            s            s            f            u            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         s            e            t            .            t            o            A            r            r            a            y            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    void*     other_set     =     NULL    ;
    char*     union_result     =     test_set    .union    (    other_set    )    ;
    if (    "Array"     ==     "    S    e    t    "     &&     union_result     != NULL    ) {
        {
            myco_print(            "            â            œ            “                         s            e            t            .            u            n            i            o            n            (            )                         s            u            c            c            e            s            s            f            u            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         s            e            t            .            u            n            i            o            n            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     intersection_result     =     test_set    .intersection    (    other_set    )    ;
    if (    "Array"     ==     "    S    e    t    "     &&     intersection_result     != NULL    ) {
        {
            myco_print(            "            â            œ            “                         s            e            t            .            i            n            t            e            r            s            e            c            t            i            o            n            (            )                         s            u            c            c            e            s            s            f            u            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         s            e            t            .            i            n            t            e            r            s            e            c            t            i            o            n            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     clear_result     =     test_set    .clear    (    )    ;
    if (    "Array"     ==     "    S    e    t    "    ) {
        {
            myco_print(            "            â            œ            “                         s            e            t            .            c            l            e            a            r            (            )                         s            u            c            c            e            s            s            f            u            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         s            e            t            .            c            l            e            a            r            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    1    8    .         T    r    e    e    s         L    i    b    r    a    r    y         F    u    n    c    t    i    o    n    s    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     test_tree     =     "TreeObject"    ;
    if (    "Array"     ==     "    T    r    e    e    "     &&     test_tree     != NULL    ) {
        {
            myco_print(            "            â            œ            “                         t            r            e            e            s            .            c            r            e            a            t            e            (            )                         s            u            c            c            e            s            s            f            u            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         t            r            e            e            s            .            c            r            e            a            t            e            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     tree_size     =     test_tree    .size    (    )    ;
    if (    tree_size     ==     0.000000    ) {
        {
            myco_print(            "            â            œ            “                         t            r            e            e            s            .            s            i            z            e            (            )                         r            e            t            u            r            n            s                         c            o            r            r            e            c            t                         i            n            i            t            i            a            l                         s            i            z            e            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         t            r            e            e            s            .            s            i            z            e            (            )                         f            a            i            l            e            d            :                         "            ,             myco_number_to_string(            tree_size            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     tree_is_empty     =     test_tree    .isEmpty    (    )    ;
    if (    tree_is_empty     ==     1    ) {
        {
            myco_print(            "            â            œ            “                         t            r            e            e            s            .            i            s            E            m            p            t            y            (            )                         r            e            t            u            r            n            s                         c            o            r            r            e            c            t                         i            n            i            t            i            a            l                         s            t            a            t            e            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         t            r            e            e            s            .            i            s            E            m            p            t            y            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    test_tree     =     test_tree    .insert    (    "    r    o    o    t    "    )    ;
    if (    "Array"     ==     "    T    r    e    e    "     &&     test_tree     != NULL    ) {
        {
            myco_print(            "            â            œ            “                         t            r            e            e            s            .            i            n            s            e            r            t            (            )                         s            u            c            c            e            s            s            f            u            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         t            r            e            e            s            .            i            n            s            e            r            t            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     tree_search     =     test_tree    .search    (    "    r    o    o    t    "    )    ;
    if (    "Array"     ==     "    B    o    o    l    e    a    n    "    ) {
        {
            myco_print(            "            â            œ            “                         t            r            e            e            s            .            s            e            a            r            c            h            (            )                         s            u            c            c            e            s            s            f            u            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         t            r            e            e            s            .            s            e            a            r            c            h            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    test_tree     =     test_tree    .clear    (    )    ;
    if (    "Array"     ==     "    T    r    e    e    "     &&     test_tree     != NULL    ) {
        {
            myco_print(            "            â            œ            “                         t            r            e            e            s            .            c            l            e            a            r            (            )                         s            u            c            c            e            s            s            f            u            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         t            r            e            e            s            .            c            l            e            a            r            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    1    9    .         G    r    a    p    h    s         L    i    b    r    a    r    y         F    u    n    c    t    i    o    n    s    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     test_graph     =     "GraphObject"    ;
    if (    "Array"     ==     "    G    r    a    p    h    "     &&     test_graph     != NULL    ) {
        {
            myco_print(            "            â            œ            “                         g            r            a            p            h            s            .            c            r            e            a            t            e            (            )                         s            u            c            c            e            s            s            f            u            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         g            r            a            p            h            s            .            c            r            e            a            t            e            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     graph_size     =     test_graph    .size    (    )    ;
    if (    graph_size     ==     0.000000    ) {
        {
            myco_print(            "            â            œ            “                         g            r            a            p            h            s            .            s            i            z            e            (            )                         r            e            t            u            r            n            s                         c            o            r            r            e            c            t                         i            n            i            t            i            a            l                         s            i            z            e            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         g            r            a            p            h            s            .            s            i            z            e            (            )                         f            a            i            l            e            d            :                         "            ,             myco_number_to_string(            graph_size            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     graph_is_empty     =     test_graph    .is_empty    (    )    ;
    if (    graph_is_empty     ==     1    ) {
        {
            myco_print(            "            â            œ            “                         g            r            a            p            h            s            .            i            s            _            e            m            p            t            y            (            )                         r            e            t            u            r            n            s                         c            o            r            r            e            c            t                         i            n            i            t            i            a            l                         s            t            a            t            e            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         g            r            a            p            h            s            .            i            s            _            e            m            p            t            y            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    test_graph     =     test_graph    .add_node    (    "    n    o    d    e    1    "    )    ;
    if (    "Array"     ==     "    G    r    a    p    h    "     &&     test_graph     != NULL    ) {
        {
            myco_print(            "            â            œ            “                         g            r            a            p            h            s            .            a            d            d            _            n            o            d            e            (            )                         s            u            c            c            e            s            s            f            u            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         g            r            a            p            h            s            .            a            d            d            _            n            o            d            e            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    test_graph     =     test_graph    .add_edge    (    "    n    o    d    e    1    "    ,     "    n    o    d    e    2    "    )    ;
    if (    "Array"     ==     "    G    r    a    p    h    "     &&     test_graph     != NULL    ) {
        {
            myco_print(            "            â            œ            “                         g            r            a            p            h            s            .            a            d            d            _            e            d            g            e            (            )                         s            u            c            c            e            s            s            f            u            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         g            r            a            p            h            s            .            a            d            d            _            e            d            g            e            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    test_graph     =     test_graph    .clear    (    )    ;
    if (    "Array"     ==     "    G    r    a    p    h    "     &&     test_graph     != NULL    ) {
        {
            myco_print(            "            â            œ            “                         g            r            a            p            h            s            .            c            l            e            a            r            (            )                         s            u            c            c            e            s            s            f            u            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         g            r            a            p            h            s            .            c            l            e            a            r            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     directed_graph     =     "GraphObject"    ;
    if (    "Array"     ==     "    G    r    a    p    h    "     &&     directed_graph     != NULL    ) {
        {
            myco_print(            "            â            œ            “                         g            r            a            p            h            s            .            c            r            e            a            t            e            (            )                         w            i            t            h                         d            i            r            e            c            t            e            d                         p            a            r            a            m            e            t            e            r                         s            u            c            c            e            s            s            f            u            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         g            r            a            p            h            s            .            c            r            e            a            t            e            (            )                         w            i            t            h                         d            i            r            e            c            t            e            d                         p            a            r            a            m            e            t            e            r                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    2    0    .         H    e    a    p    s         L    i    b    r    a    r    y         F    u    n    c    t    i    o    n    s    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     test_heap     =     heaps    .create    (    1    )    ;
    if (    "Array"     ==     "    H    e    a    p    "     &&     test_heap     != NULL    ) {
        {
            myco_print(            "            â            œ            “                         h            e            a            p            s            .            c            r            e            a            t            e            (            )                         s            u            c            c            e            s            s            f            u            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         h            e            a            p            s            .            c            r            e            a            t            e            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     heap_size     =     test_heap    .size    (    )    ;
    if (    heap_size     ==     0.000000    ) {
        {
            myco_print(            "            â            œ            “                         h            e            a            p            s            .            s            i            z            e            (            )                         s            u            c            c            e            s            s            f            u            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         h            e            a            p            s            .            s            i            z            e            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     heap_empty     =     test_heap    .isEmpty    (    )    ;
    if (    heap_empty     ==     1    ) {
        {
            myco_print(            "            â            œ            “                         h            e            a            p            s            .            i            s            E            m            p            t            y            (            )                         s            u            c            c            e            s            s            f            u            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         h            e            a            p            s            .            i            s            E            m            p            t            y            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    test_heap     =     test_heap    .insert    (    10.000000    )    ;
    test_heap     =     test_heap    .insert    (    5.000000    )    ;
    test_heap     =     test_heap    .insert    (    15.000000    )    ;
    int     heap_size_after     =     test_heap    .size    (    )    ;
    if (    heap_size_after     ==     3.000000    ) {
        {
            myco_print(            "            â            œ            “                         h            e            a            p            s            .            i            n            s            e            r            t            (            )                         s            u            c            c            e            s            s            f            u            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         h            e            a            p            s            .            i            n            s            e            r            t            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     heap_peek     =     test_heap    .peek    (    )    ;
    if (    heap_peek     ==     15.000000    ) {
        {
            myco_print(            "            â            œ            “                         h            e            a            p            s            .            p            e            e            k            (            )                         s            u            c            c            e            s            s            f            u            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         h            e            a            p            s            .            p            e            e            k            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    test_heap     =     test_heap    .extract    (    )    ;
    int     heap_size_after_extract     =     test_heap    .size    (    )    ;
    if (    heap_size_after_extract     ==     2.000000    ) {
        {
            myco_print(            "            â            œ            “                         h            e            a            p            s            .            e            x            t            r            a            c            t            (            )                         s            u            c            c            e            s            s            f            u            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         h            e            a            p            s            .            e            x            t            r            a            c            t            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    test_heap     =     test_heap    .clear    (    )    ;
    int     heap_clear_size     =     test_heap    .size    (    )    ;
    if (    heap_clear_size     ==     0.000000    ) {
        {
            myco_print(            "            â            œ            “                         h            e            a            p            s            .            c            l            e            a            r            (            )                         s            u            c            c            e            s            s            f            u            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         h            e            a            p            s            .            c            l            e            a            r            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    2    1    .         Q    u    e    u    e    s         L    i    b    r    a    r    y         F    u    n    c    t    i    o    n    s    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     test_queue     =     queues    .create    (    )    ;
    if (    "Array"     ==     "    Q    u    e    u    e    "     &&     test_queue     != NULL    ) {
        {
            myco_print(            "            â            œ            “                         q            u            e            u            e            s            .            c            r            e            a            t            e            (            )                         s            u            c            c            e            s            s            f            u            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         q            u            e            u            e            s            .            c            r            e            a            t            e            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     queue_size     =     test_queue    .size    (    )    ;
    if (    queue_size     ==     0.000000    ) {
        {
            myco_print(            "            â            œ            “                         q            u            e            u            e            s            .            s            i            z            e            (            )                         s            u            c            c            e            s            s            f            u            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         q            u            e            u            e            s            .            s            i            z            e            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     queue_empty     =     test_queue    .isEmpty    (    )    ;
    if (    queue_empty     ==     1    ) {
        {
            myco_print(            "            â            œ            “                         q            u            e            u            e            s            .            i            s            E            m            p            t            y            (            )                         s            u            c            c            e            s            s            f            u            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         q            u            e            u            e            s            .            i            s            E            m            p            t            y            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    test_queue     =     test_queue    .enqueue    (    "    f    i    r    s    t    "    )    ;
    test_queue     =     test_queue    .enqueue    (    "    s    e    c    o    n    d    "    )    ;
    test_queue     =     test_queue    .enqueue    (    "    t    h    i    r    d    "    )    ;
    int     queue_size_after     =     test_queue    .size    (    )    ;
    if (    queue_size_after     ==     3.000000    ) {
        {
            myco_print(            "            â            œ            “                         q            u            e            u            e            s            .            e            n            q            u            e            u            e            (            )                         s            u            c            c            e            s            s            f            u            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         q            u            e            u            e            s            .            e            n            q            u            e            u            e            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     queue_front     =     test_queue    .front    (    )    ;
    if (    strcmp(    queue_front    ,     "    f    i    r    s    t    "    ) == 0    ) {
        {
            myco_print(            "            â            œ            “                         q            u            e            u            e            s            .            f            r            o            n            t            (            )                         s            u            c            c            e            s            s            f            u            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         q            u            e            u            e            s            .            f            r            o            n            t            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     queue_back     =     test_queue    .back    (    )    ;
    if (    strcmp(    queue_back    ,     "    t    h    i    r    d    "    ) == 0    ) {
        {
            myco_print(            "            â            œ            “                         q            u            e            u            e            s            .            b            a            c            k            (            )                         s            u            c            c            e            s            s            f            u            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         q            u            e            u            e            s            .            b            a            c            k            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    test_queue     =     test_queue    .dequeue    (    )    ;
    int     queue_size_after_dequeue     =     test_queue    .size    (    )    ;
    if (    queue_size_after_dequeue     ==     2.000000    ) {
        {
            myco_print(            "            â            œ            “                         q            u            e            u            e            s            .            d            e            q            u            e            u            e            (            )                         s            u            c            c            e            s            s            f            u            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         q            u            e            u            e            s            .            d            e            q            u            e            u            e            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    test_queue     =     test_queue    .clear    (    )    ;
    int     queue_clear_size     =     test_queue    .size    (    )    ;
    if (    queue_clear_size     ==     0.000000    ) {
        {
            myco_print(            "            â            œ            “                         q            u            e            u            e            s            .            c            l            e            a            r            (            )                         s            u            c            c            e            s            s            f            u            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         q            u            e            u            e            s            .            c            l            e            a            r            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    2    2    .         S    t    a    c    k    s         L    i    b    r    a    r    y         F    u    n    c    t    i    o    n    s    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     test_stack     =     stacks    .create    (    )    ;
    if (    "Array"     ==     "    S    t    a    c    k    "     &&     test_stack     != NULL    ) {
        {
            myco_print(            "            â            œ            “                         s            t            a            c            k            s            .            c            r            e            a            t            e            (            )                         s            u            c            c            e            s            s            f            u            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         s            t            a            c            k            s            .            c            r            e            a            t            e            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     stack_size     =     test_stack    .size    (    )    ;
    if (    stack_size     ==     0.000000    ) {
        {
            myco_print(            "            â            œ            “                         s            t            a            c            k            s            .            s            i            z            e            (            )                         s            u            c            c            e            s            s            f            u            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         s            t            a            c            k            s            .            s            i            z            e            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     stack_empty     =     test_stack    .isEmpty    (    )    ;
    if (    stack_empty     ==     1    ) {
        {
            myco_print(            "            â            œ            “                         s            t            a            c            k            s            .            i            s            E            m            p            t            y            (            )                         s            u            c            c            e            s            s            f            u            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         s            t            a            c            k            s            .            i            s            E            m            p            t            y            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    test_stack     =     test_stack    .push    (    "    b    o    t    t    o    m    "    )    ;
    test_stack     =     test_stack    .push    (    "    m    i    d    d    l    e    "    )    ;
    test_stack     =     test_stack    .push    (    "    t    o    p    "    )    ;
    int     stack_size_after     =     test_stack    .size    (    )    ;
    if (    stack_size_after     ==     3.000000    ) {
        {
            myco_print(            "            â            œ            “                         s            t            a            c            k            s            .            p            u            s            h            (            )                         s            u            c            c            e            s            s            f            u            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         s            t            a            c            k            s            .            p            u            s            h            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     stack_top     =     test_stack    .top    (    )    ;
    if (    strcmp(    stack_top    ,     "    t    o    p    "    ) == 0    ) {
        {
            myco_print(            "            â            œ            “                         s            t            a            c            k            s            .            t            o            p            (            )                         s            u            c            c            e            s            s            f            u            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         s            t            a            c            k            s            .            t            o            p            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    test_stack     =     test_stack    .pop    (    )    ;
    int     stack_size_after_pop     =     test_stack    .size    (    )    ;
    if (    stack_size_after_pop     ==     2.000000    ) {
        {
            myco_print(            "            â            œ            “                         s            t            a            c            k            s            .            p            o            p            (            )                         s            u            c            c            e            s            s            f            u            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         s            t            a            c            k            s            .            p            o            p            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    test_stack     =     test_stack    .clear    (    )    ;
    int     stack_clear_size     =     test_stack    .size    (    )    ;
    if (    stack_clear_size     ==     0.000000    ) {
        {
            myco_print(            "            â            œ            “                         s            t            a            c            k            s            .            c            l            e            a            r            (            )                         s            u            c            c            e            s            s            f            u            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         s            t            a            c            k            s            .            c            l            e            a            r            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    =    =    =         2    3    .         B    U    I    L    T    -    I    N         F    U    N    C    T    I    O    N    S         =    =    =    "    )    ;
    myco_print(    "    2    0    .    1    .         T    y    p    e         c    h    e    c    k    i    n    g    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     type_result     =     "Array"    ;
    if (    strcmp(    type_result    ,     "    I    n    t    "    ) == 0    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         4            2            .            t            y            p            e            (            )            :                         "            ,             type_result            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         4            2            .            t            y            p            e            (            )            :                         "            ,             type_result            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     type_float     =     "Array"    ;
    if (    strcmp(    type_float    ,     "    F    l    o    a    t    "    ) == 0    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         3            .            1            4            .            t            y            p            e            (            )            :                         "            ,             type_float            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         3            .            1            4            .            t            y            p            e            (            )            :                         "            ,             type_float            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     type_str     =     "Array"    ;
    if (    strcmp(    type_str    ,     "    S    t    r    i    n    g    "    ) == 0    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         '            h            e            l            l            o            '            .            t            y            p            e            (            )            :                         "            ,             type_str            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         '            h            e            l            l            o            '            .            t            y            p            e            (            )            :                         "            ,             type_str            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     type_bool     =     "Array"    ;
    if (    strcmp(    type_bool    ,     "    B    o    o    l    e    a    n    "    ) == 0    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         T            r            u            e            .            t            y            p            e            (            )            :                         "            ,             type_bool            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         T            r            u            e            .            t            y            p            e            (            )            :                         "            ,             type_bool            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     type_null     =     "Array"    ;
    if (    strcmp(    type_null    ,     "    N    u    l    l    "    ) == 0    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         N            u            l            l            .            t            y            p            e            (            )            :                         "            ,             type_null            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         N            u            l            l            .            t            y            p            e            (            )            :                         "            ,             type_null            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     type_array     =     "Array"    ;
    if (    strcmp(    type_array    ,     "    A    r    r    a    y    "    ) == 0    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         [            1            ,            2            ,            3            ]            .            t            y            p            e            (            )            :                         "            ,             type_array            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         [            1            ,            2            ,            3            ]            .            t            y            p            e            (            )            :                         "            ,             type_array            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    1    3    .    2    .         L    e    n    g    t    h         f    u    n    c    t    i    o    n    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    int     len_builtin     =     sizeof(    "    h    e    l    l    o    "    ) / sizeof(    "    h    e    l    l    o    "    [0])    ;
    if (    len_builtin     ==     5.000000    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         l            e            n            (            '            h            e            l            l            o            '            )            :                         "            ,             myco_number_to_string(            len_builtin            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         l            e            n            (            '            h            e            l            l            o            '            )            :                         "            ,             myco_number_to_string(            len_builtin            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     len_empty     =     sizeof(    "    "    ) / sizeof(    "    "    [0])    ;
    if (    len_empty     ==     0.000000    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         l            e            n            (            '            '            )            :                         "            ,             myco_safe_to_string(            len_empty            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         l            e            n            (            '            '            )            :                         "            ,             myco_safe_to_string(            len_empty            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     len_array     =     sizeof(    (double[]){    1.000000    ,     2.000000    ,     3.000000    }    ) / sizeof(    (double[]){    1.000000    ,     2.000000    ,     3.000000    }    [0])    ;
    if (    len_array     ==     3.000000    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         l            e            n            (            [            1            ,            2            ,            3            ]            )            :                         "            ,             myco_safe_to_string(            len_array            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         l            e            n            (            [            1            ,            2            ,            3            ]            )            :                         "            ,             myco_safe_to_string(            len_array            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    1    3    .    3    .         S    t    r    i    n    g         c    o    n    v    e    r    s    i    o    n    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     str_builtin     =     myco_number_to_string(    3.140000    )    ;
    if (    strcmp(    str_builtin    ,     "    3    .    1    4    0    0    0    0    "    ) == 0    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         t            o            S            t            r            i            n            g            (            3            .            1            4            )            :                         "            ,             str_builtin            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         t            o            S            t            r            i            n            g            (            3            .            1            4            )            :                         "            ,             str_builtin            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     str_bool     =     myco_number_to_string(    0    )    ;
    if (    strcmp(    str_bool    ,     "    F    a    l    s    e    "    ) == 0    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         t            o            S            t            r            i            n            g            (            F            a            l            s            e            )            :                         "            ,             str_bool            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         t            o            S            t            r            i            n            g            (            F            a            l            s            e            )            :                         "            ,             str_bool            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     str_null     =     myco_safe_to_string(    NULL    )    ;
    if (    strcmp(    str_null    ,     "    N    u    l    l    "    ) == 0    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         t            o            S            t            r            i            n            g            (            N            u            l            l            )            :                         "            ,             str_null            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         t            o            S            t            r            i            n            g            (            N            u            l            l            )            :                         "            ,             str_null            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     str_array     =     myco_safe_to_string(    (double[]){    1.000000    ,     2.000000    ,     3.000000    }    )    ;
    if (    strcmp(    str_array    ,     "    [    1    ,         2    ,         3    ]    "    ) == 0    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         t            o            S            t            r            i            n            g            (            [            1            ,            2            ,            3            ]            )            :                         "            ,             str_array            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         t            o            S            t            r            i            n            g            (            [            1            ,            2            ,            3            ]            )            :                         "            ,             str_array            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    =    =    =         1    4    .         U    N    I    O    N         T    Y    P    E    S         =    =    =    "    )    ;
    myco_print(    "    1    4    .    1    .         B    a    s    i    c         u    n    i    o    n         t    y    p    e         d    e    c    l    a    r    a    t    i    o    n    s    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    void*     union_str     =     "    h    e    l    l    o    "    ;
    if (    strcmp(    union_str    ,     "    h    e    l    l    o    "    ) == 0    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         U            n            i            o            n                         t            y            p            e                         w            i            t            h                         S            t            r            i            n            g                         v            a            l            u            e            :                         "            ,             union_str            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         U            n            i            o            n                         t            y            p            e                         w            i            t            h                         S            t            r            i            n            g                         v            a            l            u            e            :                         "            ,             union_str            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    void*     union_int     =     (void*)(intptr_t)    42.000000    ;
    if (    union_int     ==     42.000000    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         U            n            i            o            n                         t            y            p            e                         w            i            t            h                         I            n            t                         v            a            l            u            e            :                         "            ,             myco_number_to_string(            union_int            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         U            n            i            o            n                         t            y            p            e                         w            i            t            h                         I            n            t                         v            a            l            u            e            :                         "            ,             myco_number_to_string(            union_int            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    1    4    .    2    .         U    n    i    o    n         t    y    p    e         w    i    t    h         m    u    l    t    i    p    l    e         t    y    p    e    s    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    void*     mixed_union     =     (void*)(intptr_t)    1    ;
    if (    mixed_union     ==     1    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         U            n            i            o            n                         t            y            p            e                         w            i            t            h                         B            o            o            l                         v            a            l            u            e            :                         "            ,             myco_safe_to_string(            mixed_union            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         U            n            i            o            n                         t            y            p            e                         w            i            t            h                         B            o            o            l                         v            a            l            u            e            :                         "            ,             myco_safe_to_string(            mixed_union            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    void*     mixed_union2     =     "    w    o    r    l    d    "    ;
    if (    strcmp(    mixed_union2    ,     "    w    o    r    l    d    "    ) == 0    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         U            n            i            o            n                         t            y            p            e                         w            i            t            h                         S            t            r            i            n            g                         v            a            l            u            e            :                         "            ,             mixed_union2            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         U            n            i            o            n                         t            y            p            e                         w            i            t            h                         S            t            r            i            n            g                         v            a            l            u            e            :                         "            ,             mixed_union2            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    1    4    .    3    .         U    n    i    o    n         t    y    p    e         t    y    p    e         c    h    e    c    k    i    n    g    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     type_check_str     =     "Array"    ;
    if (    strcmp(    type_check_str    ,     "    S    t    r    i    n    g    "    ) == 0    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         U            n            i            o            n                         t            y            p            e                         t            y            p            e            (            )                         r            e            t            u            r            n            s                         c            o            r            r            e            c            t                         t            y            p            e                         f            o            r                         S            t            r            i            n            g            :                         "            ,             type_check_str            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         U            n            i            o            n                         t            y            p            e                         t            y            p            e            (            )                         r            e            t            u            r            n            s                         c            o            r            r            e            c            t                         t            y            p            e                         f            o            r                         S            t            r            i            n            g            :                         "            ,             type_check_str            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     type_check_int     =     "Array"    ;
    if (    strcmp(    type_check_int    ,     "    I    n    t    "    ) == 0    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         U            n            i            o            n                         t            y            p            e                         t            y            p            e            (            )                         r            e            t            u            r            n            s                         c            o            r            r            e            c            t                         t            y            p            e                         f            o            r                         I            n            t            :                         "            ,             type_check_int            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         U            n            i            o            n                         t            y            p            e                         t            y            p            e            (            )                         r            e            t            u            r            n            s                         c            o            r            r            e            c            t                         t            y            p            e                         f            o            r                         I            n            t            :                         "            ,             type_check_int            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    1    4    .    4    .         U    n    i    o    n         t    y    p    e         p    a    r    s    i    n    g         a    n    d         r    e    p    r    e    s    e    n    t    a    t    i    o    n    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    void*     union_parsing     =     "    p    a    r    s    i    n    g    _    t    e    s    t    "    ;
    if (    strcmp(    union_parsing    ,     "    p    a    r    s    i    n    g    _    t    e    s    t    "    ) == 0    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         U            n            i            o            n                         t            y            p            e                         p            a            r            s            i            n            g                         w            i            t            h                         t            h            r            e            e                         t            y            p            e            s            :                         "            ,             union_parsing            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         U            n            i            o            n                         t            y            p            e                         p            a            r            s            i            n            g                         w            i            t            h                         t            h            r            e            e                         t            y            p            e            s            :                         "            ,             union_parsing            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    void*     union_float     =     (void*)(intptr_t)    3.141590    ;
    if (    union_float     ==     3.141590    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         U            n            i            o            n                         t            y            p            e                         w            i            t            h                         F            l            o            a            t            :                         "            ,             myco_number_to_string(            union_float            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         U            n            i            o            n                         t            y            p            e                         w            i            t            h                         F            l            o            a            t            :                         "            ,             myco_number_to_string(            union_float            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    1    4    .    5    .         U    n    i    o    n         t    y    p    e         e    d    g    e         c    a    s    e    s    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    void*     union_null     =     NULL    ;
    if (    union_null     ==     NULL    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         U            n            i            o            n                         t            y            p            e                         w            i            t            h                         N            u            l            l            :                         "            ,             myco_number_to_string(            union_null            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         U            n            i            o            n                         t            y            p            e                         w            i            t            h                         N            u            l            l            :                         "            ,             myco_number_to_string(            union_null            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    void*     union_bool     =     (void*)(intptr_t)    0    ;
    if (    union_bool     ==     0    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         U            n            i            o            n                         t            y            p            e                         w            i            t            h                         B            o            o            l            :                         "            ,             myco_number_to_string(            union_bool            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         U            n            i            o            n                         t            y            p            e                         w            i            t            h                         B            o            o            l            :                         "            ,             myco_number_to_string(            union_bool            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    =    =    =         1    5    .         O    P    T    I    O    N    A    L         T    Y    P    E    S         =    =    =    "    )    ;
    myco_print(    "    1    5    .    1    .         B    a    s    i    c         o    p    t    i    o    n    a    l         t    y    p    e         d    e    c    l    a    r    a    t    i    o    n    s    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    void*     optional_string     =     "    h    e    l    l    o    "    ;
    if (    strcmp(    optional_string    ,     "    h    e    l    l    o    "    ) == 0    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         O            p            t            i            o            n            a            l                         s            t            r            i            n            g            :                         "            ,             optional_string            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         O            p            t            i            o            n            a            l                         s            t            r            i            n            g            :                         "            ,             optional_string            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    void*     optional_null     =     NULL    ;
    if (    optional_null     ==     NULL    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         O            p            t            i            o            n            a            l                         n            u            l            l            :                         "            ,             myco_number_to_string(            optional_null            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         O            p            t            i            o            n            a            l                         n            u            l            l            :                         "            ,             myco_number_to_string(            optional_null            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    1    5    .    2    .         O    p    t    i    o    n    a    l         t    y    p    e    s         w    i    t    h         d    i    f    f    e    r    e    n    t         b    a    s    e         t    y    p    e    s    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    void*     optional_array     =     (double[]){    1.000000    ,     2.000000    ,     3.000000    }    ;
    if (    "Array"     ==     "    A    r    r    a    y    "    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         O            p            t            i            o            n            a            l                         a            r            r            a            y            :                         "            ,             myco_safe_to_string(            optional_array            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         O            p            t            i            o            n            a            l                         a            r            r            a            y            :                         "            ,             myco_safe_to_string(            optional_array            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    1    5    .    3    .         O    p    t    i    o    n    a    l         t    y    p    e         c    h    e    c    k    i    n    g    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    if (    "Array"     ==     "    S    t    r    i    n    g    "    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         O            p            t            i            o            n            a            l                         s            t            r            i            n            g                         t            y            p            e            :                         "            ,             "Array"            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         O            p            t            i            o            n            a            l                         s            t            r            i            n            g                         t            y            p            e            :                         "            ,             "Array"            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    if (    "Array"     ==     "    N    u    l    l    "    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         O            p            t            i            o            n            a            l                         n            u            l            l                         t            y            p            e            :                         "            ,             "Array"            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         O            p            t            i            o            n            a            l                         n            u            l            l                         t            y            p            e            :                         "            ,             "Array"            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    1    5    .    4    .         O    p    t    i    o    n    a    l         t    y    p    e    s         w    i    t    h         m    a    t    c    h         s    t    a    t    e    m    e    n    t    s    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     match_result     =     "    "    ;
    // Unsupported statement type: 22
    if (    strcmp(    match_result    ,     "    f    o    u    n    d         h    e    l    l    o    "    ) == 0    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         M            a            t            c            h                         w            i            t            h                         o            p            t            i            o            n            a            l                         s            t            r            i            n            g            :                         "            ,             match_result            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         M            a            t            c            h                         w            i            t            h                         o            p            t            i            o            n            a            l                         s            t            r            i            n            g            :                         "            ,             match_result            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    match_result     =     "    "    ;
    // Unsupported statement type: 22
    if (    strcmp(    match_result    ,     "    f    o    u    n    d         n    u    l    l    "    ) == 0    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         M            a            t            c            h                         w            i            t            h                         o            p            t            i            o            n            a            l                         n            u            l            l            :                         "            ,             match_result            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         M            a            t            c            h                         w            i            t            h                         o            p            t            i            o            n            a            l                         n            u            l            l            :                         "            ,             match_result            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    1    5    .    5    .         O    p    t    i    o    n    a    l         u    n    i    o    n         t    y    p    e    s    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    void*     optional_union     =     "    t    e    s    t    "    ;
    if (    strcmp(    optional_union    ,     "    t    e    s    t    "    ) == 0    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         O            p            t            i            o            n            a            l                         u            n            i            o            n            :                         "            ,             myco_number_to_string(            optional_union            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         O            p            t            i            o            n            a            l                         u            n            i            o            n            :                         "            ,             myco_number_to_string(            optional_union            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    =    =    =         1    6    .         T    Y    P    E         G    U    A    R    D    S         =    =    =    "    )    ;
    myco_print(    "    1    6    .    1    .         B    a    s    i    c         t    y    p    e         g    u    a    r    d         m    e    t    h    o    d    s    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    if (    isString(    "    h    e    l    l    o    "    )    ) {
        {
            myco_print(            "            â            œ            “                         i            s            S            t            r            i            n            g            (            '            h            e            l            l            o            '            )            :                         T            r            u            e            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         i            s            S            t            r            i            n            g            (            '            h            e            l            l            o            '            )            :                         F            a            l            s            e            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    if (    isString(    42.000000    )    ) {
        {
            myco_print(            "            â            œ            —                         i            s            S            t            r            i            n            g            (            4            2            )            :                         T            r            u            e            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    } else {
        {
            myco_print(            "            â            œ            “                         i            s            S            t            r            i            n            g            (            4            2            )            :                         F            a            l            s            e            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    }
    myco_print(    "    1    6    .    2    .         I    n    t    e    g    e    r         t    y    p    e         g    u    a    r    d    s    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    if (    isInt(    42.000000    )    ) {
        {
            myco_print(            "            â            œ            “                         i            s            I            n            t            (            4            2            )            :                         T            r            u            e            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         i            s            I            n            t            (            4            2            )            :                         F            a            l            s            e            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    if (    isInt(    3.140000    )    ) {
        {
            myco_print(            "            â            œ            —                         i            s            I            n            t            (            3            .            1            4            )            :                         T            r            u            e            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    } else {
        {
            myco_print(            "            â            œ            “                         i            s            I            n            t            (            3            .            1            4            )            :                         F            a            l            s            e            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    }
    myco_print(    "    1    6    .    3    .         F    l    o    a    t         t    y    p    e         g    u    a    r    d    s    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    if (    isFloat(    3.140000    )    ) {
        {
            myco_print(            "            â            œ            “                         i            s            F            l            o            a            t            (            3            .            1            4            )            :                         T            r            u            e            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         i            s            F            l            o            a            t            (            3            .            1            4            )            :                         F            a            l            s            e            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    if (    isFloat(    42.000000    )    ) {
        {
            myco_print(            "            â            œ            —                         i            s            F            l            o            a            t            (            4            2            )            :                         T            r            u            e            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    } else {
        {
            myco_print(            "            â            œ            “                         i            s            F            l            o            a            t            (            4            2            )            :                         F            a            l            s            e            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    }
    myco_print(    "    1    6    .    4    .         B    o    o    l    e    a    n         t    y    p    e         g    u    a    r    d    s    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    if (    isBool(    1    )    ) {
        {
            myco_print(            "            â            œ            “                         i            s            B            o            o            l            (            T            r            u            e            )            :                         T            r            u            e            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         i            s            B            o            o            l            (            T            r            u            e            )            :                         F            a            l            s            e            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    if (    isBool(    42.000000    )    ) {
        {
            myco_print(            "            â            œ            —                         i            s            B            o            o            l            (            4            2            )            :                         T            r            u            e            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    } else {
        {
            myco_print(            "            â            œ            “                         i            s            B            o            o            l            (            4            2            )            :                         F            a            l            s            e            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    }
    myco_print(    "    1    6    .    5    .         A    r    r    a    y         t    y    p    e         g    u    a    r    d    s    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    if (    isArray(    (double[]){    1.000000    ,     2.000000    ,     3.000000    }    )    ) {
        {
            myco_print(            "            â            œ            “                         i            s            A            r            r            a            y            (            [            1            ,                         2            ,                         3            ]            )            :                         T            r            u            e            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         i            s            A            r            r            a            y            (            [            1            ,                         2            ,                         3            ]            )            :                         F            a            l            s            e            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    if (    isArray(    "    h    e    l    l    o    "    )    ) {
        {
            myco_print(            "            â            œ            —                         i            s            A            r            r            a            y            (            '            h            e            l            l            o            '            )            :                         T            r            u            e            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    } else {
        {
            myco_print(            "            â            œ            “                         i            s            A            r            r            a            y            (            '            h            e            l            l            o            '            )            :                         F            a            l            s            e            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    }
    myco_print(    "    1    6    .    6    .         N    u    l    l         t    y    p    e         g    u    a    r    d    s    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    if (    isNull(    NULL    )    ) {
        {
            myco_print(            "            â            œ            “                         i            s            N            u            l            l            (            N            u            l            l            )            :                         T            r            u            e            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         i            s            N            u            l            l            (            N            u            l            l            )            :                         F            a            l            s            e            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    if (    isNull(    "    h    e    l    l    o    "    )    ) {
        {
            myco_print(            "            â            œ            —                         i            s            N            u            l            l            (            '            h            e            l            l            o            '            )            :                         T            r            u            e            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    } else {
        {
            myco_print(            "            â            œ            “                         i            s            N            u            l            l            (            '            h            e            l            l            o            '            )            :                         F            a            l            s            e            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    }
    myco_print(    "    1    6    .    7    .         N    u    m    b    e    r         t    y    p    e         g    u    a    r    d    s    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    if (    isNumber(    42.000000    )    ) {
        {
            myco_print(            "            â            œ            “                         i            s            N            u            m            b            e            r            (            4            2            )            :                         T            r            u            e            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         i            s            N            u            m            b            e            r            (            4            2            )            :                         F            a            l            s            e            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    if (    isNumber(    3.140000    )    ) {
        {
            myco_print(            "            â            œ            “                         i            s            N            u            m            b            e            r            (            3            .            1            4            )            :                         T            r            u            e            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         i            s            N            u            m            b            e            r            (            3            .            1            4            )            :                         F            a            l            s            e            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    if (    isNumber(    "    h    e    l    l    o    "    )    ) {
        {
            myco_print(            "            â            œ            —                         i            s            N            u            m            b            e            r            (            '            h            e            l            l            o            '            )            :                         T            r            u            e            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    } else {
        {
            myco_print(            "            â            œ            “                         i            s            N            u            m            b            e            r            (            '            h            e            l            l            o            '            )            :                         F            a            l            s            e            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    }
    myco_print(    "    1    6    .    8    .         T    y    p    e         g    u    a    r    d    s         w    i    t    h         o    p    t    i    o    n    a    l         t    y    p    e    s    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    void*     optional_string_2     =     "    h    e    l    l    o    "    ;
    if (    isString(    optional_string_2    )    ) {
        {
            myco_print(            "            â            œ            “                         i            s            S            t            r            i            n            g            (            o            p            t            i            o            n            a            l            _            s            t            r            i            n            g            )            :                         T            r            u            e            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         i            s            S            t            r            i            n            g            (            o            p            t            i            o            n            a            l            _            s            t            r            i            n            g            )            :                         F            a            l            s            e            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    void*     optional_null_2     =     NULL    ;
    if (    isNull(    optional_null_2    )    ) {
        {
            myco_print(            "            â            œ            “                         i            s            N            u            l            l            (            o            p            t            i            o            n            a            l            _            n            u            l            l            )            :                         T            r            u            e            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         i            s            N            u            l            l            (            o            p            t            i            o            n            a            l            _            n            u            l            l            )            :                         F            a            l            s            e            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    1    6    .    9    .         T    y    p    e         g    u    a    r    d    s         i    n         c    o    n    d    i    t    i    o    n    a    l         l    o    g    i    c    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    char*     test_value     =     "    h    e    l    l    o    "    ;
    char*     type_result_2     =     "    "    ;
    if (    isString(    test_value    )    ) {
        {
            type_result_2             =             "            s            t            r            i            n            g            "            ;
        }
    } else {
        {
            if (            isInt(            test_value            )            ) {
                {
                    type_result_2                     =                     "                    i                    n                    t                    e                    g                    e                    r                    "                    ;
                }
            } else {
                {
                    if (                    isBool(                    test_value                    )                    ) {
                        {
                            type_result_2                             =                             "                            b                            o                            o                            l                            e                            a                            n                            "                            ;
                        }
                    } else {
                        {
                            if (                            isNull(                            test_value                            )                            ) {
                                {
                                    type_result_2                                     =                                     "                                    n                                    u                                    l                                    l                                    "                                    ;
                                }
                            } else {
                                {
                                    type_result_2                                     =                                     "                                    u                                    n                                    k                                    n                                    o                                    w                                    n                                    "                                    ;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    if (    strcmp(    type_result_2    ,     "    s    t    r    i    n    g    "    ) == 0    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         T            y            p            e                         g            u            a            r            d                         c            o            n            d            i            t            i            o            n            a            l                         l            o            g            i            c            :                         "            ,             type_result_2            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         T            y            p            e                         g            u            a            r            d                         c            o            n            d            i            t            i            o            n            a            l                         l            o            g            i            c            :                         "            ,             type_result_2            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    =    =    =         1    7    .         E    D    G    E         C    A    S    E    S         =    =    =    "    )    ;
    myco_print(    "    1    7    .    1    .         Z    e    r    o         a    n    d         n    e    g    a    t    i    v    e         n    u    m    b    e    r    s    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     zero_add     =     0.000000     +     0.000000    ;
    if (    zero_add     ==     0.000000    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         Z            e            r            o                         a            d            d            i            t            i            o            n                         (            0                         +                         0            )            :                         "            ,             myco_number_to_string(            zero_add            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         Z            e            r            o                         a            d            d            i            t            i            o            n                         (            0                         +                         0            )            :                         "            ,             myco_number_to_string(            zero_add            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     zero_mult_2     =     0.000000     *     5.000000    ;
    if (    zero_mult_2     ==     0.000000    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         Z            e            r            o                         m            u            l            t            i            p            l            i            c            a            t            i            o            n                         (            0                         *                         5            )            :                         "            ,             myco_number_to_string(            zero_mult_2            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         Z            e            r            o                         m            u            l            t            i            p            l            i            c            a            t            i            o            n                         (            0                         *                         5            )            :                         "            ,             myco_number_to_string(            zero_mult_2            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     neg_add     =     -    5.000000     +     3.000000    ;
    if (    neg_add     ==     -    2.000000    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         N            e            g            a            t            i            v            e                         a            d            d            i            t            i            o            n                         (            -            5                         +                         3            )            :                         "            ,             myco_number_to_string(            neg_add            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         N            e            g            a            t            i            v            e                         a            d            d            i            t            i            o            n                         (            -            5                         +                         3            )            :                         "            ,             myco_number_to_string(            neg_add            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    1    7    .    2    .         L    a    r    g    e         n    u    m    b    e    r    s    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     large_add     =     999999.000000     +     1.000000    ;
    if (    large_add     ==     1000000.000000    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         L            a            r            g            e                         a            d            d            i            t            i            o            n                         (            9            9            9            9            9            9                         +                         1            )            :                         "            ,             myco_number_to_string(            large_add            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         L            a            r            g            e                         a            d            d            i            t            i            o            n                         (            9            9            9            9            9            9                         +                         1            )            :                         "            ,             myco_number_to_string(            large_add            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     large_mult     =     1000.000000     *     1000.000000    ;
    if (    large_mult     ==     1000000.000000    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         L            a            r            g            e                         m            u            l            t            i            p            l            i            c            a            t            i            o            n                         (            1            0            0            0                         *                         1            0            0            0            )            :                         "            ,             myco_number_to_string(            large_mult            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         L            a            r            g            e                         m            u            l            t            i            p            l            i            c            a            t            i            o            n                         (            1            0            0            0                         *                         1            0            0            0            )            :                         "            ,             myco_number_to_string(            large_mult            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    1    7    .    3    .         F    l    o    a    t    i    n    g         p    o    i    n    t         p    r    e    c    i    s    i    o    n    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     float_prec     =     0.100000     +     0.200000    ;
    if (    float_prec     >     0.300000    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         F            l            o            a            t                         p            r            e            c            i            s            i            o            n                         (            0            .            1                         +                         0            .            2                         >                         0            .            3            )            :                         "            ,             myco_number_to_string(            float_prec            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         F            l            o            a            t                         p            r            e            c            i            s            i            o            n                         (            0            .            1                         +                         0            .            2                         >                         0            .            3            )            :                         "            ,             myco_number_to_string(            float_prec            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     float_eq_2     =     1.000000     ==     1.000000    ;
    if (    float_eq_2     ==     1    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         F            l            o            a            t                         e            q            u            a            l            i            t            y                         (            1            .            0                         =            =                         1            )            :                         "            ,             myco_number_to_string(            float_eq_2            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         F            l            o            a            t                         e            q            u            a            l            i            t            y                         (            1            .            0                         =            =                         1            )            :                         "            ,             myco_number_to_string(            float_eq_2            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    1    7    .    4    .         E    m    p    t    y         v    a    l    u    e    s    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    int     empty_str_len     =     sizeof(    "    "    ) / sizeof(    "    "    [0])    ;
    if (    empty_str_len     ==     0.000000    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         E            m            p            t            y                         s            t            r            i            n            g                         l            e            n            g            t            h            :                         "            ,             myco_safe_to_string(            empty_str_len            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         E            m            p            t            y                         s            t            r            i            n            g                         l            e            n            g            t            h            :                         "            ,             myco_safe_to_string(            empty_str_len            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    int     empty_array_len     =     sizeof(    (char*[]){    }    ) / sizeof(    (char*[]){    }    [0])    ;
    if (    empty_array_len     ==     0.000000    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         E            m            p            t            y                         a            r            r            a            y                         l            e            n            g            t            h            :                         "            ,             myco_safe_to_string(            empty_array_len            )            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         E            m            p            t            y                         a            r            r            a            y                         l            e            n            g            t            h            :                         "            ,             myco_safe_to_string(            empty_array_len            )            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     empty_concat     =     myco_string_concat(    "    "    ,     "    h    e    l    l    o    "    )    ;
    if (    strcmp(    empty_concat    ,     "    h    e    l    l    o    "    ) == 0    ) {
        {
            myco_print(            myco_string_concat(            "            â            œ            “                         E            m            p            t            y                         s            t            r            i            n            g                         c            o            n            c            a            t            e            n            a            t            i            o            n            :                         "            ,             empty_concat            )            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            myco_string_concat(            "            â            œ            —                         E            m            p            t            y                         s            t            r            i            n            g                         c            o            n            c            a            t            e            n            a            t            i            o            n            :                         "            ,             empty_concat            )            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    =    =    =         1    8    .         E    R    R    O    R         H    A    N    D    L    I    N    G         (    E    x    p    e    c    t    e    d         E    r    r    o    r    s    )         =    =    =    "    )    ;
    myco_print(    "    1    8    .    1    .         D    i    v    i    s    i    o    n         b    y         z    e    r    o         (    s    h    o    u    l    d         c    o    n    t    i    n    u    e         e    x    e    c    u    t    i    o    n    )    .    .    .    "    )    ;
    myco_print(    "    \\    n    1    8    .    2    .         A    r    r    a    y         i    n    d    e    x         o    u    t         o    f         b    o    u    n    d    s         (    s    h    o    u    l    d         c    o    n    t    i    n    u    e         e    x    e    c    u    t    i    o    n    )    .    .    .    "    )    ;
    myco_print(    "    \\    n    1    8    .    3    .         U    n    d    e    f    i    n    e    d         v    a    r    i    a    b    l    e         (    s    h    o    u    l    d         c    o    n    t    i    n    u    e         e    x    e    c    u    t    i    o    n    )    .    .    .    "    )    ;
    myco_print(    "    \\    n    1    8    .    1    .         F    u    n    c    t    i    o    n         d    e    f    i    n    i    t    i    o    n    s         (    n    o         p    a    r    a    m    e    t    e    r    s    ,         n    o         r    e    t    u    r    n         t    y    p    e    )    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    if (    "Array"     ==     "    F    u    n    c    t    i    o    n    "    ) {
        {
            myco_print(            "            â            œ            “                         S            i            m            p            l            e                         f            u            n            c            t            i            o            n                         d            e            f            i            n            i            t            i            o            n                         (            n            o                         p            a            r            a            m            s            ,                         n            o                         r            e            t            u            r            n            )            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         S            i            m            p            l            e                         f            u            n            c            t            i            o            n                         d            e            f            i            n            i            t            i            o            n                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    1    8    .    2    .         F    u    n    c    t    i    o    n         w    i    t    h         s    i    n    g    l    e         p    a    r    a    m    e    t    e    r         (    n    o         r    e    t    u    r    n         t    y    p    e    )    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    if (    "Array"     ==     "    F    u    n    c    t    i    o    n    "    ) {
        {
            myco_print(            "            â            œ            “                         F            u            n            c            t            i            o            n                         w            i            t            h                         s            i            n            g            l            e                         p            a            r            a            m            e            t            e            r                         (            n            o                         r            e            t            u            r            n                         t            y            p            e            )            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         F            u            n            c            t            i            o            n                         w            i            t            h                         s            i            n            g            l            e                         p            a            r            a            m            e            t            e            r                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    1    8    .    3    .         F    u    n    c    t    i    o    n         w    i    t    h         m    u    l    t    i    p    l    e         p    a    r    a    m    e    t    e    r    s         (    n    o         r    e    t    u    r    n         t    y    p    e    )    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    if (    "Array"     ==     "    F    u    n    c    t    i    o    n    "    ) {
        {
            myco_print(            "            â            œ            “                         F            u            n            c            t            i            o            n                         w            i            t            h                         m            u            l            t            i            p            l            e                         p            a            r            a            m            e            t            e            r            s                         (            n            o                         r            e            t            u            r            n                         t            y            p            e            )            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         F            u            n            c            t            i            o            n                         w            i            t            h                         m            u            l            t            i            p            l            e                         p            a            r            a            m            e            t            e            r            s                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    1    8    .    4    .         F    u    n    c    t    i    o    n         d    e    f    i    n    i    t    i    o    n    s         (    w    i    t    h         r    e    t    u    r    n         t    y    p    e    s    )    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    if (    "Array"     ==     "    F    u    n    c    t    i    o    n    "     &&     "Array"     ==     "    F    u    n    c    t    i    o    n    "     &&     "Array"     ==     "    F    u    n    c    t    i    o    n    "    ) {
        {
            myco_print(            "            â            œ            “                         F            u            n            c            t            i            o            n                         d            e            f            i            n            i            t            i            o            n            s                         w            i            t            h                         r            e            t            u            r            n                         t            y            p            e            s            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         F            u            n            c            t            i            o            n                         d            e            f            i            n            i            t            i            o            n            s                         w            i            t            h                         r            e            t            u            r            n                         t            y            p            e            s                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    1    8    .    4    .         F    u    n    c    t    i    o    n         d    e    f    i    n    i    t    i    o    n    s         (    m    i    x    e    d         e    x    p    l    i    c    i    t    /    i    m    p    l    i    c    i    t         t    y    p    e    s    )    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    if (    "Array"     ==     "    F    u    n    c    t    i    o    n    "     &&     "Array"     ==     "    F    u    n    c    t    i    o    n    "     &&     "Array"     ==     "    F    u    n    c    t    i    o    n    "    ) {
        {
            myco_print(            "            â            œ            “                         F            u            n            c            t            i            o            n                         d            e            f            i            n            i            t            i            o            n            s                         w            i            t            h                         m            i            x            e            d                         e            x            p            l            i            c            i            t            /            i            m            p            l            i            c            i            t                         t            y            p            e            s            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         F            u            n            c            t            i            o            n                         d            e            f            i            n            i            t            i            o            n            s                         w            i            t            h                         m            i            x            e            d                         t            y            p            e            s                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    1    8    .    5    .         F    u    n    c    t    i    o    n         c    a    l    l    s         (    n    o         p    a    r    a    m    e    t    e    r    s    )    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    double     five     =     return_five(    )    ;
    char*     hello     =     return_hello(    )    ;
    if (    five     ==     5.000000     &&     strcmp(    hello    ,     "    H    e    l    l    o    "    ) == 0    ) {
        {
            myco_print(            "            â            œ            “                         F            u            n            c            t            i            o            n                         c            a            l            l            s                         w            i            t            h                         n            o                         p            a            r            a            m            e            t            e            r            s            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         F            u            n            c            t            i            o            n                         c            a            l            l            s                         w            i            t            h                         n            o                         p            a            r            a            m            e            t            e            r            s                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    1    8    .    6    .         F    u    n    c    t    i    o    n         c    a    l    l    s         (    w    i    t    h         p    a    r    a    m    e    t    e    r    s    )    .    .    .    "    )    ;
    myco_print(    "    \\    n    1    8    .    7    .         F    u    n    c    t    i    o    n         c    a    l    l    s         (    w    i    t    h         t    y    p    e         a    n    n    o    t    a    t    i    o    n    s    )    .    .    .    "    )    ;
    myco_print(    "    \\    n    1    8    .    8    .         R    e    c    u    r    s    i    v    e         f    u    n    c    t    i    o    n    s    .    .    .    "    )    ;
    myco_print(    "    \\    n    1    8    .    9    .         F    u    n    c    t    i    o    n         v    a    r    i    a    b    l    e    s         a    n    d         a    s    s    i    g    n    m    e    n    t    s    .    .    .    "    )    ;
    myco_print(    "    \\    n    1    8    .    1    0    .         C    o    m    p    l    e    x         f    u    n    c    t    i    o    n         c    o    m    b    i    n    a    t    i    o    n    s    .    .    .    "    )    ;
    myco_print(    "    \\    n    1    8    .    1    1    .         F    u    n    c    t    i    o    n         s    c    o    p    e         a    n    d         c    l    o    s    u    r    e    s    .    .    .    "    )    ;
    myco_print(    "    \\    n    1    8    .    1    2    .         F    u    n    c    t    i    o    n         e    r    r    o    r         h    a    n    d    l    i    n    g    .    .    .    "    )    ;
    myco_print(    "    \\    n    1    8    .    1    3    .         L    a    m    b    d    a         f    u    n    c    t    i    o    n    s         (    a    n    o    n    y    m    o    u    s         f    u    n    c    t    i    o    n    s    )    .    .    .    "    )    ;
    void*     simple_lambda     =     NULL    ;
    void*     add_lambda     =     NULL    ;
    void*     multiply_lambda     =     NULL    ;
    void*     greet_lambda     =     NULL    ;
    void*     mixed_lambda     =     NULL    ;
    total_tests     =     total_tests     +     1.000000    ;
    if (    "Array"     ==     "    F    u    n    c    t    i    o    n    "     &&     "Array"     ==     "    F    u    n    c    t    i    o    n    "     &&     "Array"     ==     "    F    u    n    c    t    i    o    n    "     &&     "Array"     ==     "    F    u    n    c    t    i    o    n    "     &&     "Array"     ==     "    F    u    n    c    t    i    o    n    "    ) {
        {
            myco_print(            "            â            œ            “                         L            a            m            b            d            a                         f            u            n            c            t            i            o            n            s                         (            a            n            o            n            y            m            o            u            s                         f            u            n            c            t            i            o            n            s            )            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         L            a            m            b            d            a                         f            u            n            c            t            i            o            n            s                         (            a            n            o            n            y            m            o            u            s                         f            u            n            c            t            i            o            n            s            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    1    8    .    1    4    .         F    u    n    c    t    i    o    n         v    a    r    i    a    b    l    e         a    s    s    i    g    n    m    e    n    t    s    .    .    .    "    )    ;
    void*     my_square     =     NULL    ;
    void*     my_add     =     NULL    ;
    void*     my_greet     =     NULL    ;
    total_tests     =     total_tests     +     1.000000    ;
    if (    "Array"     ==     "    F    u    n    c    t    i    o    n    "     &&     "Array"     ==     "    F    u    n    c    t    i    o    n    "     &&     "Array"     ==     "    F    u    n    c    t    i    o    n    "    ) {
        {
            myco_print(            "            â            œ            “                         F            u            n            c            t            i            o            n                         v            a            r            i            a            b            l            e                         a            s            s            i            g            n            m            e            n            t            s            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         F            u            n            c            t            i            o            n                         v            a            r            i            a            b            l            e                         a            s            s            i            g            n            m            e            n            t            s                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    1    8    .    1    5    .         L    a    m    b    d    a         f    u    n    c    t    i    o    n    s         w    i    t    h         d    i    f    f    e    r    e    n    t         p    a    r    a    m    e    t    e    r         t    y    p    e    s    .    .    .    "    )    ;
    void*     no_params_lambda     =     NULL    ;
    void*     single_param_lambda     =     NULL    ;
    void*     multi_param_lambda     =     NULL    ;
    void*     typed_param_lambda     =     NULL    ;
    total_tests     =     total_tests     +     1.000000    ;
    if (    "Array"     ==     "    F    u    n    c    t    i    o    n    "     &&     "Array"     ==     "    F    u    n    c    t    i    o    n    "     &&     "Array"     ==     "    F    u    n    c    t    i    o    n    "     &&     "Array"     ==     "    F    u    n    c    t    i    o    n    "    ) {
        {
            myco_print(            "            â            œ            “                         L            a            m            b            d            a                         f            u            n            c            t            i            o            n            s                         w            i            t            h                         d            i            f            f            e            r            e            n            t                         p            a            r            a            m            e            t            e            r                         t            y            p            e            s            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         L            a            m            b            d            a                         f            u            n            c            t            i            o            n            s                         w            i            t            h                         d            i            f            f            e            r            e            n            t                         p            a            r            a            m            e            t            e            r                         t            y            p            e            s                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    1    8    .    1    6    .         E    x    p    l    i    c    i    t         f    u    n    c    t    i    o    n         w    i    t    h         a    l    l         t    y    p    e    s    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    if (    "Array"     ==     "    F    u    n    c    t    i    o    n    "    ) {
        {
            char*             result_scope_1             =             explicit_all(            42.000000            )            ;
            if (            result_scope_1             != NULL            ) {
                {
                    myco_print(                    "                    â                    œ                    “                                         E                    x                    p                    l                    i                    c                    i                    t                                         f                    u                    n                    c                    t                    i                    o                    n                                         w                    i                    t                    h                                         a                    l                    l                                         t                    y                    p                    e                    s                    "                    )                    ;
                    tests_passed                     =                     tests_passed                     +                     1.000000                    ;
                }
            } else {
                {
                    myco_print(                    "                    â                    œ                    —                                         E                    x                    p                    l                    i                    c                    i                    t                                         f                    u                    n                    c                    t                    i                    o                    n                                         w                    i                    t                    h                                         a                    l                    l                                         t                    y                    p                    e                    s                                         f                    a                    i                    l                    e                    d                    "                    )                    ;
                    tests_failed                     =                     tests_failed                    ;
                }
            }
        }
    } else {
        {
            myco_print(            "            â            œ            —                         E            x            p            l            i            c            i            t                         f            u            n            c            t            i            o            n                         w            i            t            h                         a            l            l                         t            y            p            e            s                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    1    8    .    1    7    .         I    m    p    l    i    c    i    t         f    u    n    c    t    i    o    n         w    i    t    h         n    o         t    y    p    e    s    .    .    .    "    )    ;
    myco_print(    "    \\    n    1    8    .    1    8    .         M    i    x    e    d         f    u    n    c    t    i    o    n         -         e    x    p    l    i    c    i    t         p    a    r    a    m    e    t    e    r    ,         i    m    p    l    i    c    i    t         r    e    t    u    r    n    .    .    .    "    )    ;
    myco_print(    "    \\    n    1    8    .    1    9    .         M    i    x    e    d         f    u    n    c    t    i    o    n         -         i    m    p    l    i    c    i    t         p    a    r    a    m    e    t    e    r    ,         e    x    p    l    i    c    i    t         r    e    t    u    r    n    .    .    .    "    )    ;
    myco_print(    "    \\    n    1    8    .    2    0    .         F    u    n    c    t    i    o    n         v    a    r    i    a    b    l    e         w    i    t    h         e    x    p    l    i    c    i    t         t    y    p    e    s    .    .    .    "    )    ;
    myco_print(    "    \\    n    1    8    .    2    1    .         F    u    n    c    t    i    o    n         v    a    r    i    a    b    l    e         w    i    t    h         i    m    p    l    i    c    i    t         t    y    p    e    s    .    .    .    "    )    ;
    myco_print(    "    \\    n    1    8    .    2    2    .         F    u    n    c    t    i    o    n         v    a    r    i    a    b    l    e         w    i    t    h         m    i    x    e    d         t    y    p    e    s    .    .    .    "    )    ;
    myco_print(    "    \\    n    1    8    .    2    3    .         F    u    n    c    t    i    o    n         v    a    r    i    a    b    l    e         w    i    t    h         m    i    x    e    d         r    e    t    u    r    n         t    y    p    e    .    .    .    "    )    ;
    myco_print(    "    \\    n    1    8    .    2    4    .         F    u    n    c    t    i    o    n         t    y    p    e         i    n    t    e    r    c    h    a    n    g    e    a    b    i    l    i    t    y         w    i    t    h         d    i    f    f    e    r    e    n    t         a    r    g    u    m    e    n    t         t    y    p    e    s    .    .    .    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    typedef struct {
        char* name;
        double count;
    } DefaultClass;
    DefaultClass     default_instance     =     DefaultClass(    )    ;
    if (    "Array"     ==     "    D    e    f    a    u    l    t    C    l    a    s    s    "     &&     default_instance    .name     ==     "    D    e    f    a    u    l    t    "     &&     default_instance    .count     ==     0.000000    ) {
        {
            myco_print(            "            â            œ            “                         C            l            a            s            s                         w            i            t            h                         d            e            f            a            u            l            t                         f            i            e            l            d                         v            a            l            u            e            s            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         C            l            a            s            s                         w            i            t            h                         d            e            f            a            u            l            t                         f            i            e            l            d                         v            a            l            u            e            s                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    typedef struct {
        int value;
    } SimpleClass;
    SimpleClass     s     =     SimpleClass(    42.000000    )    ;
    if (    "Array"     ==     "    S    i    m    p    l    e    C    l    a    s    s    "     &&     s    .value     ==     42.000000    ) {
        {
            myco_print(            "            â            œ            “                         C            l            a            s            s                         i            n            s            t            a            n            t            i            a            t            i            o            n            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         C            l            a            s            s                         i            n            s            t            a            n            t            i            a            t            i            o            n                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    typedef struct {
        char* name;
    } MethodClass;
    MethodClass     m     =     MethodClass(    "    W    o    r    l    d    "    )    ;
    char*     greeting     =     m    .greet    (    )    ;
    if (    "Array"     ==     "    M    e    t    h    o    d    C    l    a    s    s    "     &&     strcmp(    greeting    ,     "    H    e    l    l    o    ,         W    o    r    l    d    "    ) == 0    ) {
        {
            myco_print(            "            â            œ            “                         C            l            a            s            s                         w            i            t            h                         m            e            t            h            o            d            s            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         C            l            a            s            s                         w            i            t            h                         m            e            t            h            o            d            s                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    typedef struct {
        int value;
    } SelfClass;
    SelfClass     self_test     =     SelfClass(    100.000000    )    ;
    double     retrieved_value     =     self_test    .getValue    (    )    ;
    if (    "Array"     ==     "    S    e    l    f    C    l    a    s    s    "     &&     retrieved_value     ==     100.000000    ) {
        {
            myco_print(            "            â            œ            “                         C            l            a            s            s                         w            i            t            h                         s            e            l            f                         r            e            f            e            r            e            n            c            e            s            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         C            l            a            s            s                         w            i            t            h                         s            e            l            f                         r            e            f            e            r            e            n            c            e            s                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    typedef struct {
        char* name;
        int age;
        int active;
    } MixedClass;
    MixedClass     mixed_2     =     MixedClass(    "    A    l    i    c    e    "    ,     25.000000    )    ;
    if (    "Array"     ==     "    M    i    x    e    d    C    l    a    s    s    "     &&     mixed_2    .name     ==     "    A    l    i    c    e    "     &&     mixed_2    .age     ==     25.000000     &&     mixed_2    .active     ==     0    ) {
        {
            myco_print(            "            â            œ            “                         C            l            a            s            s                         w            i            t            h                         m            i            x            e            d                         f            i            e            l            d                         t            y            p            e            s            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         C            l            a            s            s                         w            i            t            h                         m            i            x            e            d                         f            i            e            l            d                         t            y            p            e            s                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    typedef struct {
        int count;
    } TypedMethodClass;
    TypedMethodClass     typed     =     TypedMethodClass(    5.000000    )    ;
    double     incremented     =     typed    .increment    (    )    ;
    char*     class_name     =     typed    .getName    (    )    ;
    if (    "Array"     ==     "    T    y    p    e    d    M    e    t    h    o    d    C    l    a    s    s    "     &&     incremented     ==     6.000000     &&     strcmp(    class_name    ,     "    T    y    p    e    d    M    e    t    h    o    d    C    l    a    s    s    "    ) == 0    ) {
        {
            myco_print(            "            â            œ            “                         C            l            a            s            s                         w            i            t            h                         t            y            p            e            d                         m            e            t            h            o            d            s            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         C            l            a            s            s                         w            i            t            h                         t            y            p            e            d                         m            e            t            h            o            d            s                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    typedef struct {
        char* data;
    } UntypedMethodClass;
    UntypedMethodClass     untyped     =     UntypedMethodClass(    "    d    a    t    a    "    )    ;
    double     result     =     untyped    .process    (    )    ;
    if (    "Array"     ==     "    U    n    t    y    p    e    d    M    e    t    h    o    d    C    l    a    s    s    "     &&     result     ==     NULL     ||     result     ==     0.000000    ) {
        {
            myco_print(            "            â            œ            “                         C            l            a            s            s                         w            i            t            h                         u            n            t            y            p            e            d                         m            e            t            h            o            d            s            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         C            l            a            s            s                         w            i            t            h                         u            n            t            y            p            e            d                         m            e            t            h            o            d            s                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    typedef struct {
        int x;
        int y;
    } ComplexClass;
    ComplexClass     complex     =     ComplexClass(    10.000000    ,     5.000000    )    ;
    double     calculation     =     complex    .calculate    (    )    ;
    if (    "Array"     ==     "    C    o    m    p    l    e    x    C    l    a    s    s    "     &&     calculation     ==     20.000000    ) {
        {
            myco_print(            "            â            œ            “                         C            l            a            s            s                         w            i            t            h                         c            o            m            p            l            e            x                         m            e            t            h            o            d                         b            o            d            i            e            s            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         C            l            a            s            s                         w            i            t            h                         c            o            m            p            l            e            x                         m            e            t            h            o            d                         b            o            d            i            e            s                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    =    =    =         1    9    .         I    N    H    E    R    I    T    A    N    C    E         T    E    S    T    S         =    =    =    "    )    ;
    typedef struct {
        char* name;
    } Animal;
    typedef struct {
        char* name;  // Inherited from Animal
    } Dog;
    Dog     test_dog     =     Dog(    "    R    e    x    "    )    ;
    if (    "Array"     ==     "    D    o    g    "     &&     test_dog    .name     ==     "    R    e    x    "    ) {
        {
            myco_print(            "            â            œ            “                         B            a            s            i            c                         i            n            h            e            r            i            t            a            n            c            e                         p            a            r            s            i            n            g            "            )            ;
            total_tests             =             total_tests             +             1.000000            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         B            a            s            i            c                         i            n            h            e            r            i            t            a            n            c            e                         p            a            r            s            i            n            g                         f            a            i            l            e            d            "            )            ;
            total_tests             =             total_tests             +             1.000000            ;
            tests_failed             =             tests_failed            ;
        }
    }
    Dog     dog     =     Dog(    "    B    u    d    d    y    "    )    ;
    if (    dog    .name     ==     "    B    u    d    d    y    "    ) {
        {
            myco_print(            "            â            œ            “                         F            i            e            l            d                         i            n            h            e            r            i            t            a            n            c            e            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         F            i            e            l            d                         i            n            h            e            r            i            t            a            n            c            e            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     sound     =     dog    .speak    (    )    ;
    if (    strcmp(    sound    ,     "    W    o    o    f    !    "    ) == 0    ) {
        {
            myco_print(            "            â            œ            “                         M            e            t            h            o            d                         o            v            e            r            r            i            d            i            n            g            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         M            e            t            h            o            d                         o            v            e            r            r            i            d            i            n            g            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    typedef struct {
        char* name;  // Inherited from Animal via Dog
        int age;
    } Puppy;
    Puppy     test_puppy     =     Puppy(    "    M    a    x    "    ,     6.000000    )    ;
    if (    "Array"     ==     "    P    u    p    p    y    "     &&     test_puppy    .name     ==     "    M    a    x    "     &&     test_puppy    .age     ==     6.000000    ) {
        {
            myco_print(            "            â            œ            “                         M            u            l            t            i            -            l            e            v            e            l                         i            n            h            e            r            i            t            a            n            c            e                         p            a            r            s            i            n            g            "            )            ;
            total_tests             =             total_tests             +             1.000000            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         M            u            l            t            i            -            l            e            v            e            l                         i            n            h            e            r            i            t            a            n            c            e                         p            a            r            s            i            n            g                         f            a            i            l            e            d            "            )            ;
            total_tests             =             total_tests             +             1.000000            ;
            tests_failed             =             tests_failed            ;
        }
    }
    Puppy     puppy     =     Puppy(    "    M    a    x    "    ,     6.000000    )    ;
    if (    puppy    .name     ==     "    M    a    x    "    ) {
        {
            myco_print(            "            â            œ            “                         I            n            h            e            r            i            t            a            n            c            e                         w            i            t            h                         a            d            d            i            t            i            o            n            a            l                         f            i            e            l            d            s            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         I            n            h            e            r            i            t            a            n            c            e                         w            i            t            h                         a            d            d            i            t            i            o            n            a            l                         f            i            e            l            d            s            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    typedef struct {
        char* name;  // Inherited from Animal
        char* color;
    } Cat;
    Cat     test_cat     =     Cat(    "    W    h    i    s    k    e    r    s    "    ,     "    O    r    a    n    g    e    "    )    ;
    if (    "Array"     ==     "    C    a    t    "     &&     test_cat    .name     ==     "    W    h    i    s    k    e    r    s    "     &&     test_cat    .color     ==     "    O    r    a    n    g    e    "    ) {
        {
            myco_print(            "            â            œ            “                         I            n            h            e            r            i            t            a            n            c            e                         w            i            t            h            o            u            t                         m            e            t            h            o            d                         o            v            e            r            r            i            d            e            "            )            ;
            total_tests             =             total_tests             +             1.000000            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         I            n            h            e            r            i            t            a            n            c            e                         w            i            t            h            o            u            t                         m            e            t            h            o            d                         o            v            e            r            r            i            d            e                         f            a            i            l            e            d            "            )            ;
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
    Lion     test_lion     =     Lion(    "    S    i    m    b    a    "    ,     "    S    a    v    a    n    n    a    "    ,     "    G    o    l    d    e    n    "    )    ;
    if (    "Array"     ==     "    L    i    o    n    "     &&     test_lion    .name     ==     "    S    i    m    b    a    "     &&     test_lion    .habitat     ==     "    S    a    v    a    n    n    a    "     &&     test_lion    .mane_color     ==     "    G    o    l    d    e    n    "    ) {
        {
            myco_print(            "            â            œ            “                         C            o            m            p            l            e            x                         i            n            h            e            r            i            t            a            n            c            e                         c            h            a            i            n            "            )            ;
            total_tests             =             total_tests             +             1.000000            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         C            o            m            p            l            e            x                         i            n            h            e            r            i            t            a            n            c            e                         c            h            a            i            n                         f            a            i            l            e            d            "            )            ;
            total_tests             =             total_tests             +             1.000000            ;
            tests_failed             =             tests_failed            ;
        }
    }
    typedef struct {
        int can_fly;
        double wing_span;
    } Bird;
    myco_print(    "    â    œ    “         I    n    h    e    r    i    t    a    n    c    e         w    i    t    h         d    e    f    a    u    l    t         v    a    l    u    e    s    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    typedef struct {
        int altitude;
    } FlyingAnimal;
    myco_print(    "    â    œ    “         M    u    l    t    i    p    l    e         i    n    h    e    r    i    t    a    n    c    e         s    i    m    u    l    a    t    i    o    n    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    typedef struct {
        char* species;
        double depth;
        void* is_saltwater;
    } Fish;
    myco_print(    "    â    œ    “         I    n    h    e    r    i    t    a    n    c    e         w    i    t    h         m    i    x    e    d         f    i    e    l    d         t    y    p    e    s    "    )    ;
    total_tests     =     total_tests     +     1.000000    ;
    tests_passed     =     tests_passed     +     1.000000    ;
    myco_print(    "    \\    n    =    =    =         2    0    .         T    i    m    e         L    i    b    r    a    r    y         F    u    n    c    t    i    o    n    s         =    =    =    "    )    ;
    char* time = "Module";
    total_tests     =     total_tests     +     1.000000    ;
    if (    "Module"     ==     "    O    b    j    e    c    t    "    ) {
        {
            myco_print(            "            â            œ            “                         T            i            m            e                         l            i            b            r            a            r            y                         i            m            p            o            r            t                         w            o            r            k            s            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         T            i            m            e                         l            i            b            r            a            r            y                         i            m            p            o            r            t                         f            a            i            l            e            d            "            )            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     current_time     =     &(struct { int dummy; }){0}    .now    (    )    ;
    if (    "Array"     ==     "    O    b    j    e    c    t    "    ) {
        {
            myco_print(            "            â            œ            “                         t            i            m            e            .            n            o            w            (            )                         r            e            t            u            r            n            s                         t            i            m            e                         o            b            j            e            c            t            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         t            i            m            e            .            n            o            w            (            )                         f            a            i            l            e            d            "            )            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     specific_time     =     NULL    ;
    if (    "Array"     ==     "    O    b    j    e    c    t    "    ) {
        {
            myco_print(            "            â            œ            “                         t            i            m            e            .            c            r            e            a            t            e            (            )                         r            e            t            u            r            n            s                         t            i            m            e                         o            b            j            e            c            t            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         t            i            m            e            .            c            r            e            a            t            e            (            )                         f            a            i            l            e            d            "            )            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     formatted     =     &(struct { int dummy; }){0}    .format    (    specific_time    ,     "    %    Y    -    %    m    -    %    d         %    H    :    %    M    :    %    S    "    )    ;
    if (    strcmp(    formatted    ,     "    2    0    2    4    -    0    1    -    1    5         1    4    :    3    0    :    0    0    "    ) == 0    ) {
        {
            myco_print(            "            â            œ            “                         t            i            m            e            .            f            o            r            m            a            t            (            )                         w            o            r            k            s                         c            o            r            r            e            c            t            l            y            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         t            i            m            e            .            f            o            r            m            a            t            (            )                         f            a            i            l            e            d            "            )            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     year     =     2024.000000    ;
    if (    year     ==     2024.000000    ) {
        {
            myco_print(            "            â            œ            “                         t            i            m            e            .            y            e            a            r            (            )                         w            o            r            k            s                         c            o            r            r            e            c            t            l            y            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         t            i            m            e            .            y            e            a            r            (            )                         f            a            i            l            e            d            "            )            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     month     =     &(struct { int dummy; }){0}    .month    (    specific_time    )    ;
    if (    month     ==     1.000000    ) {
        {
            myco_print(            "            â            œ            “                         t            i            m            e            .            m            o            n            t            h            (            )                         w            o            r            k            s                         c            o            r            r            e            c            t            l            y            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         t            i            m            e            .            m            o            n            t            h            (            )                         f            a            i            l            e            d            "            )            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     day     =     &(struct { int dummy; }){0}    .day    (    specific_time    )    ;
    if (    day     ==     15.000000    ) {
        {
            myco_print(            "            â            œ            “                         t            i            m            e            .            d            a            y            (            )                         w            o            r            k            s                         c            o            r            r            e            c            t            l            y            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         t            i            m            e            .            d            a            y            (            )                         f            a            i            l            e            d            "            )            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     hour     =     &(struct { int dummy; }){0}    .hour    (    specific_time    )    ;
    if (    hour     ==     14.000000    ) {
        {
            myco_print(            "            â            œ            “                         t            i            m            e            .            h            o            u            r            (            )                         w            o            r            k            s                         c            o            r            r            e            c            t            l            y            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         t            i            m            e            .            h            o            u            r            (            )                         f            a            i            l            e            d            "            )            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     minute     =     &(struct { int dummy; }){0}    .minute    (    specific_time    )    ;
    if (    minute     ==     30.000000    ) {
        {
            myco_print(            "            â            œ            “                         t            i            m            e            .            m            i            n            u            t            e            (            )                         w            o            r            k            s                         c            o            r            r            e            c            t            l            y            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         t            i            m            e            .            m            i            n            u            t            e            (            )                         f            a            i            l            e            d            "            )            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     second     =     &(struct { int dummy; }){0}    .second    (    specific_time    )    ;
    if (    second     ==     0.000000    ) {
        {
            myco_print(            "            â            œ            “                         t            i            m            e            .            s            e            c            o            n            d            (            )                         w            o            r            k            s                         c            o            r            r            e            c            t            l            y            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         t            i            m            e            .            s            e            c            o            n            d            (            )                         f            a            i            l            e            d            "            )            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     iso     =     &(struct { int dummy; }){0}    .iso_string    (    specific_time    )    ;
    if (    strcmp(    iso    ,     "    2    0    2    4    -    0    1    -    1    5    T    1    4    :    3    0    :    0    0    "    ) == 0    ) {
        {
            myco_print(            "            â            œ            “                         t            i            m            e            .            i            s            o            _            s            t            r            i            n            g            (            )                         w            o            r            k            s                         c            o            r            r            e            c            t            l            y            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         t            i            m            e            .            i            s            o            _            s            t            r            i            n            g            (            )                         f            a            i            l            e            d            "            )            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double     unix     =     &(struct { int dummy; }){0}    .unix_timestamp    (    specific_time    )    ;
    if (    unix     ==     1705347000.000000    ) {
        {
            myco_print(            "            â            œ            “                         t            i            m            e            .            u            n            i            x            _            t            i            m            e            s            t            a            m            p            (            )                         w            o            r            k            s                         c            o            r            r            e            c            t            l            y            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         t            i            m            e            .            u            n            i            x            _            t            i            m            e            s            t            a            m            p            (            )                         f            a            i            l            e            d            "            )            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     future_time     =     &(struct { int dummy; }){0}    .add    (    specific_time    ,     3600.000000    )    ;
    double     future_hour     =     &(struct { int dummy; }){0}    .hour    (    future_time    )    ;
    if (    future_hour     ==     15.000000    ) {
        {
            myco_print(            "            â            œ            “                         t            i            m            e            .            a            d            d            (            )                         w            o            r            k            s                         c            o            r            r            e            c            t            l            y            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         t            i            m            e            .            a            d            d            (            )                         f            a            i            l            e            d            "            )            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     past_time     =     &(struct { int dummy; }){0}    .subtract    (    specific_time    ,     3600.000000    )    ;
    double     past_hour     =     &(struct { int dummy; }){0}    .hour    (    past_time    )    ;
    if (    past_hour     ==     13.000000    ) {
        {
            myco_print(            "            â            œ            “                         t            i            m            e            .            s            u            b            t            r            a            c            t            (            )                         w            o            r            k            s                         c            o            r            r            e            c            t            l            y            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         t            i            m            e            .            s            u            b            t            r            a            c            t            (            )                         f            a            i            l            e            d            "            )            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     time1     =     NULL    ;
    char*     time2     =     NULL    ;
    double     diff     =     &(struct { int dummy; }){0}    .difference    (    time1    ,     time2    )    ;
    if (    diff     ==     3600.000000    ) {
        {
            myco_print(            "            â            œ            “                         t            i            m            e            .            d            i            f            f            e            r            e            n            c            e            (            )                         w            o            r            k            s                         c            o            r            r            e            c            t            l            y            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         t            i            m            e            .            d            i            f            f            e            r            e            n            c            e            (            )                         f            a            i            l            e            d            "            )            ;
        }
    }
    myco_print(    "    \\    n    =    =    =         2    1    .         R    e    g    e    x         L    i    b    r    a    r    y         F    u    n    c    t    i    o    n    s         =    =    =    "    )    ;
    char* regex = "Module";
    total_tests     =     total_tests     +     1.000000    ;
    if (    "Module"     ==     "    O    b    j    e    c    t    "    ) {
        {
            myco_print(            "            â            œ            “                         R            e            g            e            x                         l            i            b            r            a            r            y                         i            m            p            o            r            t                         w            o            r            k            s            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         R            e            g            e            x                         l            i            b            r            a            r            y                         i            m            p            o            r            t                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     match_result_2     =     1    ;
    if (    "Array"     ==     "    O    b    j    e    c    t    "     &&     match_result_2     != NULL    ) {
        {
            myco_print(            "            â            œ            “                         r            e            g            e            x            .            m            a            t            c            h            (            )                         r            e            t            u            r            n            s                         m            a            t            c            h                         o            b            j            e            c            t            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         r            e            g            e            x            .            m            a            t            c            h            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     no_match     =     1    ;
    if (    no_match     ==     NULL    ) {
        {
            myco_print(            "            â            œ            “                         r            e            g            e            x            .            m            a            t            c            h            (            )                         r            e            t            u            r            n            s                         N            u            l            l                         f            o            r                         n            o                         m            a            t            c            h            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         r            e            g            e            x            .            m            a            t            c            h            (            )                         s            h            o            u            l            d                         r            e            t            u            r            n                         N            u            l            l                         f            o            r                         n            o                         m            a            t            c            h            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     test_result     =     &(struct { int dummy; }){0}    .test    (    "    w    o    r    l    d    "    ,     "    h    e    l    l    o         w    o    r    l    d    "    )    ;
    if (    test_result     ==     1    ) {
        {
            myco_print(            "            â            œ            “                         r            e            g            e            x            .            t            e            s            t            (            )                         w            o            r            k            s                         f            o            r                         e            x            i            s            t            i            n            g                         p            a            t            t            e            r            n            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         r            e            g            e            x            .            t            e            s            t            (            )                         f            a            i            l            e            d                         f            o            r                         e            x            i            s            t            i            n            g                         p            a            t            t            e            r            n            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     test_no_match     =     &(struct { int dummy; }){0}    .test    (    "    x    y    z    "    ,     "    h    e    l    l    o         w    o    r    l    d    "    )    ;
    if (    test_no_match     ==     0    ) {
        {
            myco_print(            "            â            œ            “                         r            e            g            e            x            .            t            e            s            t            (            )                         w            o            r            k            s                         f            o            r                         n            o            n            -            e            x            i            s            t            i            n            g                         p            a            t            t            e            r            n            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         r            e            g            e            x            .            t            e            s            t            (            )                         f            a            i            l            e            d                         f            o            r                         n            o            n            -            e            x            i            s            t            i            n            g                         p            a            t            t            e            r            n            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     valid_email     =     &(struct { int dummy; }){0}    .is_email    (    "    t    e    s    t    @    e    x    a    m    p    l    e    .    c    o    m    "    )    ;
    if (    valid_email     ==     1    ) {
        {
            myco_print(            "            â            œ            “                         r            e            g            e            x            .            i            s            _            e            m            a            i            l            (            )                         w            o            r            k            s                         f            o            r                         v            a            l            i            d                         e            m            a            i            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         r            e            g            e            x            .            i            s            _            e            m            a            i            l            (            )                         f            a            i            l            e            d                         f            o            r                         v            a            l            i            d                         e            m            a            i            l            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     invalid_email     =     &(struct { int dummy; }){0}    .is_email    (    "    n    o    t    -    a    n    -    e    m    a    i    l    "    )    ;
    if (    invalid_email     ==     0    ) {
        {
            myco_print(            "            â            œ            “                         r            e            g            e            x            .            i            s            _            e            m            a            i            l            (            )                         w            o            r            k            s                         f            o            r                         i            n            v            a            l            i            d                         e            m            a            i            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         r            e            g            e            x            .            i            s            _            e            m            a            i            l            (            )                         f            a            i            l            e            d                         f            o            r                         i            n            v            a            l            i            d                         e            m            a            i            l            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     valid_url     =     &(struct { int dummy; }){0}    .is_url    (    "    h    t    t    p    s    :    /    /    e    x    a    m    p    l    e    .    c    o    m    "    )    ;
    if (    valid_url     ==     1    ) {
        {
            myco_print(            "            â            œ            “                         r            e            g            e            x            .            i            s            _            u            r            l            (            )                         w            o            r            k            s                         f            o            r                         v            a            l            i            d                         U            R            L            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         r            e            g            e            x            .            i            s            _            u            r            l            (            )                         f            a            i            l            e            d                         f            o            r                         v            a            l            i            d                         U            R            L            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     invalid_url     =     &(struct { int dummy; }){0}    .is_url    (    "    n    o    t    -    a    -    u    r    l    "    )    ;
    if (    invalid_url     ==     0    ) {
        {
            myco_print(            "            â            œ            “                         r            e            g            e            x            .            i            s            _            u            r            l            (            )                         w            o            r            k            s                         f            o            r                         i            n            v            a            l            i            d                         U            R            L            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         r            e            g            e            x            .            i            s            _            u            r            l            (            )                         f            a            i            l            e            d                         f            o            r                         i            n            v            a            l            i            d                         U            R            L            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     valid_ip     =     &(struct { int dummy; }){0}    .is_ip    (    "    1    9    2    .    1    6    8    .    1    .    1    "    )    ;
    if (    valid_ip     ==     1    ) {
        {
            myco_print(            "            â            œ            “                         r            e            g            e            x            .            i            s            _            i            p            (            )                         w            o            r            k            s                         f            o            r                         v            a            l            i            d                         I            P            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         r            e            g            e            x            .            i            s            _            i            p            (            )                         f            a            i            l            e            d                         f            o            r                         v            a            l            i            d                         I            P            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     invalid_ip     =     &(struct { int dummy; }){0}    .is_ip    (    "    n    o    t    -    a    n    -    i    p    "    )    ;
    if (    invalid_ip     ==     0    ) {
        {
            myco_print(            "            â            œ            “                         r            e            g            e            x            .            i            s            _            i            p            (            )                         w            o            r            k            s                         f            o            r                         i            n            v            a            l            i            d                         I            P            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         r            e            g            e            x            .            i            s            _            i            p            (            )                         f            a            i            l            e            d                         f            o            r                         i            n            v            a            l            i            d                         I            P            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     case_test     =     &(struct { int dummy; }){0}    .test    (    "    H    E    L    L    O    "    ,     "    h    e    l    l    o         w    o    r    l    d    "    ,     &(struct { int dummy; }){0}    .CASE_INSENSITIVE    )    ;
    if (    case_test     ==     1    ) {
        {
            myco_print(            "            â            œ            “                         r            e            g            e            x            .            C            A            S            E            _            I            N            S            E            N            S            I            T            I            V            E                         f            l            a            g                         w            o            r            k            s            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         r            e            g            e            x            .            C            A            S            E            _            I            N            S            E            N            S            I            T            I            V            E                         f            l            a            g                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     global_test     =     &(struct { int dummy; }){0}    .test    (    "    l    "    ,     "    h    e    l    l    o    "    ,     &(struct { int dummy; }){0}    .GLOBAL    )    ;
    if (    global_test     ==     1    ) {
        {
            myco_print(            "            â            œ            “                         r            e            g            e            x            .            G            L            O            B            A            L                         f            l            a            g                         w            o            r            k            s            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         r            e            g            e            x            .            G            L            O            B            A            L                         f            l            a            g                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     multiline_test     =     &(struct { int dummy; }){0}    .test    (    "    ^    w    o    r    l    d    "    ,     "    h    e    l    l    o    \\    n    w    o    r    l    d    "    ,     &(struct { int dummy; }){0}    .MULTILINE    )    ;
    if (    multiline_test     ==     1    ) {
        {
            myco_print(            "            â            œ            “                         r            e            g            e            x            .            M            U            L            T            I            L            I            N            E                         f            l            a            g                         w            o            r            k            s            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         r            e            g            e            x            .            M            U            L            T            I            L            I            N            E                         f            l            a            g                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     dotall_test     =     &(struct { int dummy; }){0}    .test    (    "    h    e    l    l    o    .    w    o    r    l    d    "    ,     "    h    e    l    l    o    \\    n    w    o    r    l    d    "    ,     &(struct { int dummy; }){0}    .DOTALL    )    ;
    if (    dotall_test     ==     1    ) {
        {
            myco_print(            "            â            œ            “                         r            e            g            e            x            .            D            O            T            A            L            L                         f            l            a            g                         w            o            r            k            s            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         r            e            g            e            x            .            D            O            T            A            L            L                         f            l            a            g                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     complex_pattern     =     &(struct { int dummy; }){0}    .test    (    "    [    0    -    9    ]    +    "    ,     "    1    2    3    a    b    c    "    )    ;
    if (    complex_pattern     ==     1    ) {
        {
            myco_print(            "            â            œ            “                         C            o            m            p            l            e            x                         r            e            g            e            x                         p            a            t            t            e            r            n            s                         w            o            r            k            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         C            o            m            p            l            e            x                         r            e            g            e            x                         p            a            t            t            e            r            n            s                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     special_chars     =     &(struct { int dummy; }){0}    .test    (    "    [    a    -    z    A    -    Z    0    -    9    _    ]    +    "    ,     "    h    e    l    l    o    1    2    3    "    )    ;
    if (    special_chars     ==     1    ) {
        {
            myco_print(            "            â            œ            “                         R            e            g            e            x                         w            i            t            h                         s            p            e            c            i            a            l                         c            h            a            r            a            c            t            e            r            s                         w            o            r            k            s            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         R            e            g            e            x                         w            i            t            h                         s            p            e            c            i            a            l                         c            h            a            r            a            c            t            e            r            s                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     email1     =     &(struct { int dummy; }){0}    .is_email    (    "    u    s    e    r    @    d    o    m    a    i    n    .    c    o    m    "    )    ;
    char*     email2     =     &(struct { int dummy; }){0}    .is_email    (    "    t    e    s    t    .    e    m    a    i    l    +    t    a    g    @    e    x    a    m    p    l    e    .    c    o    .    u    k    "    )    ;
    char*     email3     =     &(struct { int dummy; }){0}    .is_email    (    "    i    n    v    a    l    i    d    @    "    )    ;
    char*     email4     =     &(struct { int dummy; }){0}    .is_email    (    "    @    d    o    m    a    i    n    .    c    o    m    "    )    ;
    if (    email1     ==     1     &&     email2     ==     1     &&     email3     ==     0     &&     email4     ==     0    ) {
        {
            myco_print(            "            â            œ            “                         M            u            l            t            i            p            l            e                         e            m            a            i            l                         f            o            r            m            a            t                         v            a            l            i            d            a            t            i            o            n                         w            o            r            k            s            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         M            u            l            t            i            p            l            e                         e            m            a            i            l                         f            o            r            m            a            t                         v            a            l            i            d            a            t            i            o            n                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     url1     =     &(struct { int dummy; }){0}    .is_url    (    "    h    t    t    p    :    /    /    e    x    a    m    p    l    e    .    c    o    m    "    )    ;
    char*     url2     =     &(struct { int dummy; }){0}    .is_url    (    "    h    t    t    p    s    :    /    /    w    w    w    .    e    x    a    m    p    l    e    .    c    o    m    /    p    a    t    h    "    )    ;
    char*     url3     =     &(struct { int dummy; }){0}    .is_url    (    "    f    t    p    :    /    /    f    i    l    e    s    .    e    x    a    m    p    l    e    .    c    o    m    "    )    ;
    char*     url4     =     &(struct { int dummy; }){0}    .is_url    (    "    n    o    t    -    a    -    u    r    l    "    )    ;
    if (    url1     ==     1     &&     url2     ==     1     &&     url3     ==     1     &&     url4     ==     0    ) {
        {
            myco_print(            "            â            œ            “                         M            u            l            t            i            p            l            e                         U            R            L                         f            o            r            m            a            t                         v            a            l            i            d            a            t            i            o            n                         w            o            r            k            s            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         M            u            l            t            i            p            l            e                         U            R            L                         f            o            r            m            a            t                         v            a            l            i            d            a            t            i            o            n                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     ip1     =     &(struct { int dummy; }){0}    .is_ip    (    "    1    9    2    .    1    6    8    .    1    .    1    "    )    ;
    char*     ip2     =     &(struct { int dummy; }){0}    .is_ip    (    "    1    0    .    0    .    0    .    1    "    )    ;
    char*     ip3     =     &(struct { int dummy; }){0}    .is_ip    (    "    2    5    5    .    2    5    5    .    2    5    5    .    2    5    5    "    )    ;
    char*     ip4     =     &(struct { int dummy; }){0}    .is_ip    (    "    9    9    9    .    9    9    9    .    9    9    9    .    9    9    9    "    )    ;
    if (    ip1     ==     1     &&     ip2     ==     1     &&     ip3     ==     1     &&     ip4     ==     0    ) {
        {
            myco_print(            "            â            œ            “                         M            u            l            t            i            p            l            e                         I            P                         f            o            r            m            a            t                         v            a            l            i            d            a            t            i            o            n                         w            o            r            k            s            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         M            u            l            t            i            p            l            e                         I            P                         f            o            r            m            a            t                         v            a            l            i            d            a            t            i            o            n                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     invalid_pattern     =     &(struct { int dummy; }){0}    .test    (    "    [    "    ,     "    t    e    s    t    "    )    ;
    if (    "Array"     ==     "    B    o    o    l    e    a    n    "    ) {
        {
            myco_print(            "            â            œ            “                         R            e            g            e            x                         e            r            r            o            r                         h            a            n            d            l            i            n            g                         w            o            r            k            s            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         R            e            g            e            x                         e            r            r            o            r                         h            a            n            d            l            i            n            g                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     empty_match     =     1    ;
    char*     empty_test     =     &(struct { int dummy; }){0}    .test    (    "    "    ,     "    h    e    l    l    o    "    )    ;
    if (    "Array"     ==     "    O    b    j    e    c    t    "     ||     empty_match     ==     NULL     &&     "Array"     ==     "    B    o    o    l    e    a    n    "    ) {
        {
            myco_print(            "            â            œ            “                         R            e            g            e            x                         w            i            t            h                         e            m            p            t            y                         s            t            r            i            n            g            s                         w            o            r            k            s            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         R            e            g            e            x                         w            i            t            h                         e            m            p            t            y                         s            t            r            i            n            g            s                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    =    =    =         2    2    .         J    S    O    N         L    i    b    r    a    r    y         F    u    n    c    t    i    o    n    s         =    =    =    "    )    ;
    char* json = "Module";
    total_tests     =     total_tests     +     1.000000    ;
    if (    "Module"     ==     "    O    b    j    e    c    t    "    ) {
        {
            myco_print(            "            â            œ            “                         J            S            O            N                         l            i            b            r            a            r            y                         i            m            p            o            r            t                         w            o            r            k            s            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         J            S            O            N                         l            i            b            r            a            r            y                         i            m            p            o            r            t                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     str_result     =     &(struct { int dummy; }){0}    .stringify    (    "    h    e    l    l    o    "    )    ;
    if (    strcmp(    str_result    ,     "    \\    \"    h    e    l    l    o    \\    \"    "    ) == 0    ) {
        {
            myco_print(            "            â            œ            “                         j            s            o            n            .            s            t            r            i            n            g            i            f            y            (            )                         w            o            r            k            s                         f            o            r                         s            t            r            i            n            g            s            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         j            s            o            n            .            s            t            r            i            n            g            i            f            y            (            )                         f            a            i            l            e            d                         f            o            r                         s            t            r            i            n            g            s            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     num_result     =     &(struct { int dummy; }){0}    .stringify    (    42.000000    )    ;
    if (    strcmp(    num_result    ,     "    4    2    "    ) == 0    ) {
        {
            myco_print(            "            â            œ            “                         j            s            o            n            .            s            t            r            i            n            g            i            f            y            (            )                         w            o            r            k            s                         f            o            r                         n            u            m            b            e            r            s            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         j            s            o            n            .            s            t            r            i            n            g            i            f            y            (            )                         f            a            i            l            e            d                         f            o            r                         n            u            m            b            e            r            s            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     bool_result     =     &(struct { int dummy; }){0}    .stringify    (    1    )    ;
    if (    strcmp(    bool_result    ,     "    t    r    u    e    "    ) == 0    ) {
        {
            myco_print(            "            â            œ            “                         j            s            o            n            .            s            t            r            i            n            g            i            f            y            (            )                         w            o            r            k            s                         f            o            r                         b            o            o            l            e            a            n            s            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         j            s            o            n            .            s            t            r            i            n            g            i            f            y            (            )                         f            a            i            l            e            d                         f            o            r                         b            o            o            l            e            a            n            s            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     null_result     =     &(struct { int dummy; }){0}    .stringify    (    NULL    )    ;
    if (    strcmp(    null_result    ,     "    n    u    l    l    "    ) == 0    ) {
        {
            myco_print(            "            â            œ            “                         j            s            o            n            .            s            t            r            i            n            g            i            f            y            (            )                         w            o            r            k            s                         f            o            r                         n            u            l            l            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         j            s            o            n            .            s            t            r            i            n            g            i            f            y            (            )                         f            a            i            l            e            d                         f            o            r                         n            u            l            l            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char**     arr_2     =     (char*[]){    myco_number_to_string(    1.000000    )    ,     myco_number_to_string(    2.000000    )    ,     myco_number_to_string(    3.000000    )    ,     "    h    e    l    l    o    "    ,     myco_number_to_string(    0    )    }    ;
    char*     arr_result     =     &(struct { int dummy; }){0}    .stringify    (    arr_2    )    ;
    if (    "Array"     ==     "    S    t    r    i    n    g    "     &&     strcmp(    arr_result    ,     "    "    ) != 0    ) {
        {
            myco_print(            "            â            œ            “                         j            s            o            n            .            s            t            r            i            n            g            i            f            y            (            )                         w            o            r            k            s                         f            o            r                         a            r            r            a            y            s            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         j            s            o            n            .            s            t            r            i            n            g            i            f            y            (            )                         f            a            i            l            e            d                         f            o            r                         a            r            r            a            y            s            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     valid_json     =     "    {    \"    t    e    s    t    \"    :         \"    v    a    l    u    e    \"    }    "    ;
    char*     is_valid     =     &(struct { int dummy; }){0}    .validate    (    valid_json    )    ;
    if (    is_valid     ==     1    ) {
        {
            myco_print(            "            â            œ            “                         j            s            o            n            .            v            a            l            i            d            a            t            e            (            )                         w            o            r            k            s                         f            o            r                         v            a            l            i            d                         J            S            O            N            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         j            s            o            n            .            v            a            l            i            d            a            t            e            (            )                         f            a            i            l            e            d                         f            o            r                         v            a            l            i            d                         J            S            O            N            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     invalid_json     =     "    {    \"    t    e    s    t    \"    :         \"    v    a    l    u    e    \"    "    ;
    char*     is_invalid     =     &(struct { int dummy; }){0}    .validate    (    invalid_json    )    ;
    if (    is_invalid     ==     0    ) {
        {
            myco_print(            "            â            œ            “                         j            s            o            n            .            v            a            l            i            d            a            t            e            (            )                         w            o            r            k            s                         f            o            r                         i            n            v            a            l            i            d                         J            S            O            N            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         j            s            o            n            .            v            a            l            i            d            a            t            e            (            )                         f            a            i            l            e            d                         f            o            r                         i            n            v            a            l            i            d                         J            S            O            N            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    double*     test_array_2     =     (double[]){    1.000000    ,     2.000000    ,     3.000000    ,     4.000000    ,     5.000000    }    ;
    int     array_size     =     &(struct { int dummy; }){0}    .size    (    test_array_2    )    ;
    if (    array_size     ==     5.000000    ) {
        {
            myco_print(            "            â            œ            “                         j            s            o            n            .            s            i            z            e            (            )                         w            o            r            k            s                         f            o            r                         a            r            r            a            y            s            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         j            s            o            n            .            s            i            z            e            (            )                         f            a            i            l            e            d                         f            o            r                         a            r            r            a            y            s            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char**     empty_array     =     (char*[]){    }    ;
    double*     non_empty_array     =     (double[]){    1.000000    ,     2.000000    ,     3.000000    }    ;
    char*     empty_check     =     &(struct { int dummy; }){0}    .is_empty    (    empty_array    )    ;
    char*     non_empty_check     =     &(struct { int dummy; }){0}    .is_empty    (    non_empty_array    )    ;
    if (    empty_check     ==     1     &&     non_empty_check     ==     0    ) {
        {
            myco_print(            "            â            œ            “                         j            s            o            n            .            i            s            _            e            m            p            t            y            (            )                         w            o            r            k            s                         f            o            r                         a            r            r            a            y            s            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         j            s            o            n            .            i            s            _            e            m            p            t            y            (            )                         f            a            i            l            e            d                         f            o            r                         a            r            r            a            y            s            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     parsed     =     &(struct { int dummy; }){0}    .parse    (    "    {    \"    t    e    s    t    \"    :         \"    v    a    l    u    e    \"    }    "    )    ;
    if (    "Array"     ==     "    O    b    j    e    c    t    "    ) {
        {
            myco_print(            "            â            œ            “                         j            s            o            n            .            p            a            r            s            e            (            )                         w            o            r            k            s            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         j            s            o            n            .            p            a            r            s            e            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    =    =    =         2    3    .         H    T    T    P         L    i    b    r    a    r    y         F    u    n    c    t    i    o    n    s         =    =    =    "    )    ;
    char* http = "Module";
    total_tests     =     total_tests     +     1.000000    ;
    if (    "Module"     ==     "    O    b    j    e    c    t    "    ) {
        {
            myco_print(            "            â            œ            “                         H            T            T            P                         l            i            b            r            a            r            y                         i            m            p            o            r            t                         w            o            r            k            s            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         H            T            T            P                         l            i            b            r            a            r            y                         i            m            p            o            r            t                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    HttpResponse     get_response     =     &(struct { int dummy; }){0}    .get    (    "    h    t    t    p    s    :    /    /    h    t    t    p    b    i    n    .    o    r    g    /    g    e    t    "    )    ;
    if (    "Array"     ==     "    O    b    j    e    c    t    "     &&     get_response    .status_code     ==     200.000000    ) {
        {
            myco_print(            "            â            œ            “                         h            t            t            p            .            g            e            t            (            )                         w            o            r            k            s            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         h            t            t            p            .            g            e            t            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     post_data     =     "    {    \"    t    e    s    t    \"    :         \"    d    a    t    a    \"    }    "    ;
    HttpResponse     post_response     =     &(struct { int dummy; }){0}    .post    (    "    h    t    t    p    s    :    /    /    h    t    t    p    b    i    n    .    o    r    g    /    p    o    s    t    "    ,     post_data    )    ;
    if (    "Array"     ==     "    O    b    j    e    c    t    "     &&     post_response    .status_code     ==     200.000000    ) {
        {
            myco_print(            "            â            œ            “                         h            t            t            p            .            p            o            s            t            (            )                         w            o            r            k            s            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         h            t            t            p            .            p            o            s            t            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     put_data     =     "    {    \"    i    d    \"    :         1    ,         \"    n    a    m    e    \"    :         \"    t    e    s    t    \"    }    "    ;
    HttpResponse     put_response     =     &(struct { int dummy; }){0}    .put    (    "    h    t    t    p    s    :    /    /    h    t    t    p    b    i    n    .    o    r    g    /    p    u    t    "    ,     put_data    )    ;
    if (    "Array"     ==     "    O    b    j    e    c    t    "     &&     put_response    .status_code     ==     200.000000    ) {
        {
            myco_print(            "            â            œ            “                         h            t            t            p            .            p            u            t            (            )                         w            o            r            k            s            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         h            t            t            p            .            p            u            t            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    HttpResponse     delete_response     =     &(struct { int dummy; }){0}    .delete    (    "    h    t    t    p    s    :    /    /    h    t    t    p    b    i    n    .    o    r    g    /    d    e    l    e    t    e    "    )    ;
    if (    "Array"     ==     "    O    b    j    e    c    t    "     &&     delete_response    .status_code     ==     200.000000    ) {
        {
            myco_print(            "            â            œ            “                         h            t            t            p            .            d            e            l            e            t            e            (            )                         w            o            r            k            s            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         h            t            t            p            .            d            e            l            e            t            e            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    if (    "Array"     ==     "    O    b    j    e    c    t    "    ) {
        {
            double             has_status_code_scope_1             =             get_response            .status_code             != NULL            ;
            double             has_status_text_scope_1             =             get_response            .status_text             != NULL            ;
            double             has_body_scope_1             =             get_response            .body             != NULL            ;
            double             has_success_scope_1             =             get_response            .success             != NULL            ;
            if (            has_status_code_scope_1             &&             has_status_text_scope_1             &&             has_body_scope_1             &&             has_success_scope_1            ) {
                {
                    myco_print(                    "                    â                    œ                    “                                         H                    T                    T                    P                                         r                    e                    s                    p                    o                    n                    s                    e                                         o                    b                    j                    e                    c                    t                                         h                    a                    s                                         a                    l                    l                                         r                    e                    q                    u                    i                    r                    e                    d                                         p                    r                    o                    p                    e                    r                    t                    i                    e                    s                    "                    )                    ;
                    tests_passed                     =                     tests_passed                     +                     1.000000                    ;
                }
            } else {
                {
                    myco_print(                    "                    â                    œ                    —                                         H                    T                    T                    P                                         r                    e                    s                    p                    o                    n                    s                    e                                         o                    b                    j                    e                    c                    t                                         m                    i                    s                    s                    i                    n                    g                                         p                    r                    o                    p                    e                    r                    t                    i                    e                    s                    "                    )                    ;
                    tests_failed                     =                     tests_failed                    ;
                }
            }
        }
    } else {
        {
            myco_print(            "            â            œ            —                         H            T            T            P                         r            e            s            p            o            n            s            e                         o            b            j            e            c            t                         t            e            s            t                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     status_ok     =     &(struct { int dummy; }){0}    .status_ok    (    get_response    )    ;
    if (    "Array"     ==     "    B    o    o    l    e    a    n    "    ) {
        {
            myco_print(            "            â            œ            “                         h            t            t            p            .            s            t            a            t            u            s            _            o            k            (            )                         w            o            r            k            s            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         h            t            t            p            .            s            t            a            t            u            s            _            o            k            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     content_type     =     &(struct { int dummy; }){0}    .get_header    (    get_response    ,     "    c    o    n    t    e    n    t    -    t    y    p    e    "    )    ;
    if (    "Array"     ==     "    S    t    r    i    n    g    "     ||     content_type     ==     NULL    ) {
        {
            myco_print(            "            â            œ            “                         h            t            t            p            .            g            e            t            _            h            e            a            d            e            r            (            )                         w            o            r            k            s            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         h            t            t            p            .            g            e            t            _            h            e            a            d            e            r            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    char*     json_response     =     &(struct { int dummy; }){0}    .get_json    (    get_response    )    ;
    if (    "Array"     ==     "    S    t    r    i    n    g    "     ||     json_response     ==     NULL    ) {
        {
            myco_print(            "            â            œ            “                         h            t            t            p            .            g            e            t            _            j            s            o            n            (            )                         w            o            r            k            s            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         h            t            t            p            .            g            e            t            _            j            s            o            n            (            )                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    total_tests     =     total_tests     +     1.000000    ;
    HttpResponse     error_response     =     &(struct { int dummy; }){0}    .get    (    "    h    t    t    p    s    :    /    /    i    n    v    a    l    i    d    -    u    r    l    -    t    h    a    t    -    d    o    e    s    -    n    o    t    -    e    x    i    s    t    .    c    o    m    "    )    ;
    if (    "Array"     ==     "    O    b    j    e    c    t    "    ) {
        {
            myco_print(            "            â            œ            “                         H            T            T            P                         e            r            r            o            r                         h            a            n            d            l            i            n            g                         w            o            r            k            s            "            )            ;
            tests_passed             =             tests_passed             +             1.000000            ;
        }
    } else {
        {
            myco_print(            "            â            œ            —                         H            T            T            P                         e            r            r            o            r                         h            a            n            d            l            i            n            g                         f            a            i            l            e            d            "            )            ;
            tests_failed             =             tests_failed            ;
        }
    }
    myco_print(    "    \\    n    =    =    =         R    E    S    U    L    T    S         =    =    =    "    )    ;
    myco_print(    myco_string_concat(    "    T    o    t    a    l    :         "    ,     myco_number_to_string(    total_tests    )    )    )    ;
    myco_print(    myco_string_concat(    "    P    a    s    s    e    d    :         "    ,     myco_number_to_string(    tests_passed    )    )    )    ;
    myco_print(    "    F    a    i    l    e    d    :         "    ,     total_tests     -     tests_passed    )    ;
    double     rate     =     tests_passed     *     100.000000     /     total_tests    ;
    myco_print(    myco_string_concat(    myco_string_concat(    "    S    u    c    c    e    s    s         r    a    t    e    :         "    ,     myco_number_to_string(    rate    )    )    ,     "    %    "    )    )    ;
    if (    total_tests     -     tests_passed     >     0.000000    ) {
        {
            myco_print(            "            F            a            i            l            e            d                         t            e            s            t            s            :            "            )            ;
            for (int i = 0; i <             0; i++) {                {
                    myco_print(                    "                    \\                    t                                         -                                         "                    ,                     i                    )                    ;
                }
            }
        }
    } else {
        {
            myco_print(            "            A            L            L                         T            E            S            T            S                         P            A            S            S            E            D            !            "            )            ;
        }
    }
    return 0;
}
