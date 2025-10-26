#ifndef STRING_H
#define STRING_H

#include "../core/interpreter.h"

// String library function declarations
void string_library_register(Interpreter* interpreter);

// String method functions
Value builtin_string_upper(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_string_lower(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_string_trim(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_string_split(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_string_join(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_string_contains(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_string_starts_with(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_string_ends_with(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_string_replace(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_string_repeat(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_string_charAt(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_string_substring(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_string_charCodeAt(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);

#endif // STRING_H
