#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include <stdarg.h>

// Global debug flags
extern int g_parser_debug;
extern int g_interpreter_debug;

// Debug logging functions
void debug_parser(const char* format, ...);
void debug_interpreter(const char* format, ...);
void debug_lexer(const char* format, ...);
void debug_ast(const char* format, ...);

// Set debug flags
void set_parser_debug(int enabled);
void set_interpreter_debug(int enabled);

// Debug utilities
void debug_print_ast_node(void* node, int depth);
void debug_print_token(void* token);
void debug_print_value(void* value);

#endif // DEBUG_H

