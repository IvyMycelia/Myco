#ifndef GRAPHS_H
#define GRAPHS_H

#include "../core/interpreter.h"

// Graphs library function declarations
void graphs_library_register(Interpreter* interpreter);

// Graph method functions
Value builtin_graph_create(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_graph_add_node(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_graph_add_edge(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_graph_size(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_graph_is_empty(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_graph_clear(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);

#endif // GRAPHS_H
