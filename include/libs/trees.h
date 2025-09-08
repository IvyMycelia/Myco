#ifndef TREES_H
#define TREES_H

#include "../core/interpreter.h"

// Trees library function declarations
void trees_library_register(Interpreter* interpreter);

// Tree method functions
Value builtin_tree_create(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_tree_insert(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_tree_search(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_tree_size(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_tree_is_empty(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_tree_clear(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);

#endif // TREES_H
