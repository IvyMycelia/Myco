#ifndef MAPS_H
#define MAPS_H

#include "../core/interpreter.h"

// Maps library function declarations
void maps_library_register(Interpreter* interpreter);

// Map method functions
Value builtin_map_has(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_map_size(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_map_keys(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_map_delete(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_map_clear(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_map_update(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);

#endif // MAPS_H
