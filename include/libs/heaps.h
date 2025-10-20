#ifndef HEAPS_H
#define HEAPS_H

#include "../core/interpreter.h"

// Heaps library function declarations
void heaps_library_register(Interpreter* interpreter);

// Heap method functions
Value builtin_heap_create(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_heap_insert(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_heap_extract(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_heap_peek(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_heap_size(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_heap_isEmpty(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_heap_clear(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);

// Heap method functions (for object method calls)
Value builtin_heap_insert_method(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_heap_extract_method(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_heap_peek_method(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_heap_isEmpty_method(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_heap_clear_method(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);

#endif // HEAPS_H
