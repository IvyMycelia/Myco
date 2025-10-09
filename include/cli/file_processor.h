#ifndef FILE_PROCESSOR_H
#define FILE_PROCESSOR_H

#include "../compilation/compiler.h"

// Process a file
int process_file(const char* filename, int interpret, int compile, int build, int debug, int target, const char* architecture, const char* output_file, int optimization_level, int jit_enabled, int jit_mode);

// Process a source string
int process_string(const char* source, int interpret, int compile, int build, int debug, int target, const char* architecture, const char* output_file, int optimization_level, int jit_enabled, int jit_mode);

// Process source code (common implementation)
int process_source(const char* source, const char* filename, int interpret, int compile, int build, int debug, int target, const char* architecture, const char* output_file, int optimization_level);

// Interpret source code
int interpret_source(const char* source, const char* filename, int debug);

// Compile source code
int compile_source(const char* source, int target, int debug);

// Build executable from source
int build_executable(const char* source, const char* filename, const char* architecture, const char* output_file, int debug, int optimization_level);

#endif // FILE_PROCESSOR_H
