#ifndef STACKS_H
#define STACKS_H

#include "../core/interpreter.h"

// Stacks library function declarations
void stacks_library_register(Interpreter* interpreter);

// Stack method functions
Value builtin_stack_create(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_stack_push(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_stack_pop(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_stack_top(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_stack_size(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_stack_isEmpty(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_stack_clear(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);

// Stack method functions (for object method calls)
Value builtin_stack_isEmpty_method(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_stack_push_method(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_stack_pop_method(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_stack_top_method(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_stack_clear_method(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);

#endif // STACKS_H
