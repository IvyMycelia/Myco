#ifndef SETS_H
#define SETS_H

#include "../core/interpreter.h"

// Sets library function declarations
void sets_library_register(Interpreter* interpreter);

// Set method functions
Value builtin_set_add(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_set_has(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_set_remove(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_set_size(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_set_clear(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_set_to_array(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_set_union(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_set_intersection(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);

#endif // SETS_H
