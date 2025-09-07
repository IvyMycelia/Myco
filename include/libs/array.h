#ifndef ARRAY_H
#define ARRAY_H

#include "../core/interpreter.h"

// Array library function declarations
void array_library_register(Interpreter* interpreter);

// Array method functions
Value builtin_array_push(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_array_pop(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_array_insert(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_array_remove(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_array_reverse(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_array_sort(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_array_filter(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_array_map(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_array_reduce(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_array_find(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_array_join(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_array_contains(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_array_index_of(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_array_unique(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_array_concat(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_array_slice(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_array_fill(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);

#endif // ARRAY_H
