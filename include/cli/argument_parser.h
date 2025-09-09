#ifndef ARGUMENT_PARSER_H
#define ARGUMENT_PARSER_H

#include "../compilation/compiler.h"

// Argument configuration structure
typedef struct {
    int interpret;
    int compile;
    int build;
    int debug;
    int target;
    int optimization_level;
    char* input_source;
    char* output_file;
    char* architecture;
    int help;
    int version;
} ArgumentConfig;

// Parse command line arguments
int parse_arguments(int argc, char* argv[], ArgumentConfig* config);

// Print usage information
void print_usage(const char* program_name);

// Print version information
void print_version(void);

#endif // ARGUMENT_PARSER_H
