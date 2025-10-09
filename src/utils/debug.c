#include "../../include/utils/debug.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

// Global debug flags
int g_parser_debug = 0;
int g_interpreter_debug = 0;

// Debug logging functions
void debug_parser(const char* format, ...) {
    if (!g_parser_debug) return;
    
    printf("[PARSER] ");
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");
    fflush(stdout);
}

void debug_interpreter(const char* format, ...) {
    if (!g_interpreter_debug) return;
    
    printf("[INTERPRETER] ");
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");
    fflush(stdout);
}

void debug_lexer(const char* format, ...) {
    if (!g_parser_debug) return;
    
    printf("[LEXER] ");
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");
    fflush(stdout);
}

void debug_ast(const char* format, ...) {
    if (!g_parser_debug) return;
    
    printf("[AST] ");
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");
    fflush(stdout);
}

// Set debug flags
void set_parser_debug(int enabled) {
    g_parser_debug = enabled;
}

void set_interpreter_debug(int enabled) {
    g_interpreter_debug = enabled;
}

// Debug utilities
void debug_print_ast_node(void* node, int depth) {
    if (!g_parser_debug || !node) return;
    
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }
    printf("AST Node: %p\n", node);
}

void debug_print_token(void* token) {
    if (!g_parser_debug || !token) return;
    
    printf("Token: %p\n", token);
}

void debug_print_value(void* value) {
    if (!g_interpreter_debug || !value) return;
    
    printf("Value: %p\n", value);
}

