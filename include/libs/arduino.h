// Arduino library public API for Myco
#ifndef MYCO_LIBS_ARDUINO_H
#define MYCO_LIBS_ARDUINO_H

#include "../core/interpreter.h"

// Registration
void arduino_library_register(Interpreter* interpreter);

// Builtins (exposed as methods on the arduino object)
Value builtin_arduino_pinMode(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_arduino_digitalWrite(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_arduino_digitalRead(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_arduino_analogRead(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_arduino_analogWrite(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_arduino_delay(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_arduino_run(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);

#endif // MYCO_LIBS_ARDUINO_H

