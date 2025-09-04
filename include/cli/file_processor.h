#ifndef FILE_PROCESSOR_H
#define FILE_PROCESSOR_H

#include "../compilation/compiler.h"

// Process a file
int process_file(const char* filename, int interpret, int compile, int debug, int target);

// Process a source string
int process_string(const char* source, int interpret, int compile, int debug, int target);

// Process source code (common implementation)
int process_source(const char* source, int interpret, int compile, int debug, int target);

// Interpret source code
int interpret_source(const char* source, int debug);

// Compile source code
int compile_source(const char* source, int target, int debug);

#endif // FILE_PROCESSOR_H
