#ifndef MYCO_REPL_H
#define MYCO_REPL_H

#include "../core/interpreter.h"
#include "../core/lexer.h"
#include "../core/parser.h"
#include "../core/ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// REPL state structure
typedef struct {
    Interpreter* interpreter;
    int line_number;
    int debug_mode;
    int debug_ast;
    int debug_lexer;
    int debug_parser;
    char** history;
    int history_count;
    int history_capacity;
    int history_index;
    char* current_input;
    int input_length;
    int input_capacity;
} REPLState;

// REPL functions
REPLState* repl_create(void);
void repl_free(REPLState* state);
int repl_run(REPLState* state);
int repl_process_input(REPLState* state, const char* input);
int repl_handle_command(REPLState* state, const char* command);
void repl_show_help(void);
void repl_show_variables(REPLState* state);
void repl_clear_variables(REPLState* state);
void repl_reset_state(REPLState* state);
void repl_load_file(REPLState* state, const char* filename);
void repl_set_debug_mode(REPLState* state, const char* mode);
void repl_show_debug_info(REPLState* state, const char* input);
void repl_add_to_history(REPLState* state, const char* input);
const char* repl_get_history(REPLState* state, int direction);
void repl_show_error_suggestion(const char* error, int line);

// Input handling functions
char* repl_read_line(REPLState* state);
char* repl_read_multiline(REPLState* state);
int repl_is_multiline_continuation(const char* input);
int repl_is_input_complete(const char* input);

// Output functions
void repl_print_result(REPLState* state, Value* result);
void repl_print_error(REPLState* state, const char* error, int line);
void repl_print_debug_ast(REPLState* state, ASTNode* node);
void repl_print_debug_lexer(REPLState* state, Lexer* lexer);
void repl_print_debug_parser(REPLState* state, Parser* parser);

#endif // MYCO_REPL_H
