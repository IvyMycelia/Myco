#ifndef MYCO_BUILTIN_LIBS_H
#define MYCO_BUILTIN_LIBS_H

#include "../core/interpreter.h"

// Math library registration
void math_library_register(Interpreter* interpreter);

// String library registration
void string_library_register(Interpreter* interpreter);

// Array library registration
void array_library_register(Interpreter* interpreter);

// File library registration
void file_library_register(Interpreter* interpreter);

// Directory library registration
void dir_library_register(Interpreter* interpreter);

// Maps library registration
void maps_library_register(Interpreter* interpreter);

// Sets library registration
void sets_library_register(Interpreter* interpreter);

// Trees library registration
void trees_library_register(Interpreter* interpreter);

// Graphs library registration
void graphs_library_register(Interpreter* interpreter);

// Register all built-in libraries
void register_all_builtin_libraries(Interpreter* interpreter);

#endif // MYCO_BUILTIN_LIBS_H
