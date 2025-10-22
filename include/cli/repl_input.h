#ifndef REPL_INPUT_H
#define REPL_INPUT_H

#include <stddef.h>
#include <stdbool.h>

// Custom REPL input to replace libreadline dependency
// This provides a lightweight, dependency-free REPL input system

// REPL input state structure
typedef struct {
    char* buffer;
    size_t capacity;
    size_t length;
    size_t cursor_pos;
    char** history;
    size_t history_count;
    size_t history_capacity;
    size_t history_index;
} ReplInput;

// REPL input management
ReplInput* repl_input_create(void);
void repl_input_free(ReplInput* input);

// History management
void repl_input_add_history(ReplInput* input, const char* line);

// Input reading
char* repl_input_read_line(ReplInput* input, const char* prompt);

#endif // REPL_INPUT_H
