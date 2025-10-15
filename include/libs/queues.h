#ifndef QUEUES_H
#define QUEUES_H

#include "../core/interpreter.h"

// Queues library function declarations
void queues_library_register(Interpreter* interpreter);

// Queue method functions
Value builtin_queue_create(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_queue_enqueue(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_queue_dequeue(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_queue_front(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_queue_back(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_queue_size(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_queue_isEmpty(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_queue_clear(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);

// Queue method functions (for object method calls)
Value builtin_queue_isEmpty_method(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_queue_enqueue_method(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_queue_dequeue_method(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_queue_front_method(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_queue_clear_method(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);

#endif // QUEUES_H
