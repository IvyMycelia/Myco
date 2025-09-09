#include "argument_parser.h"
#include "../myco.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Parse command line arguments
int parse_arguments(int argc, char* argv[], ArgumentConfig* config) {
    if (!config) return MYCO_ERROR_CLI;
    
    // Initialize config
    config->interpret = 0;
    config->compile = 0;
    config->build = 0;
    config->debug = 0;
    config->target = TARGET_C;
    config->optimization_level = OPTIMIZATION_NONE;
    config->jit_enabled = 0;
    config->jit_mode = 0;
    config->input_source = NULL;
    config->output_file = NULL;
    config->architecture = NULL;
    config->help = 0;
    config->version = 0;
    
    // Check if we have any arguments
    if (argc < 2) {
        config->help = 1;
        return MYCO_SUCCESS;
    }
    
    // Check for help and version flags first (these can be anywhere)
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            config->help = 1;
            return MYCO_SUCCESS;
        } else if (strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-v") == 0) {
            config->version = 1;
            return MYCO_SUCCESS;
        }
    }
    
    // First argument should be the input file or source
    config->input_source = argv[1];
    
    // Parse remaining command line arguments
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "--interpret") == 0 || strcmp(argv[i], "-i") == 0) {
            config->interpret = 1;
        } else if (strcmp(argv[i], "--compile") == 0 || strcmp(argv[i], "-c") == 0) {
            config->compile = 1;
        } else if (strcmp(argv[i], "--build") == 0 || strcmp(argv[i], "-b") == 0) {
            config->build = 1;
        } else if (strcmp(argv[i], "--debug") == 0 || strcmp(argv[i], "-d") == 0) {
            config->debug = 1;
        } else if (strcmp(argv[i], "--optimize") == 0 || strcmp(argv[i], "-O") == 0) {
            if (i + 1 < argc) {
                i++;
                if (strcmp(argv[i], "0") == 0 || strcmp(argv[i], "none") == 0) {
                    config->optimization_level = OPTIMIZATION_NONE;
                } else if (strcmp(argv[i], "1") == 0 || strcmp(argv[i], "basic") == 0) {
                    config->optimization_level = OPTIMIZATION_BASIC;
                } else if (strcmp(argv[i], "2") == 0 || strcmp(argv[i], "aggressive") == 0) {
                    config->optimization_level = OPTIMIZATION_AGGRESSIVE;
                } else if (strcmp(argv[i], "3") == 0 || strcmp(argv[i], "maximum") == 0) {
                    config->optimization_level = OPTIMIZATION_SIZE;
                } else {
                    fprintf(stderr, "Error: Unknown optimization level '%s'\n", argv[i]);
                    return MYCO_ERROR_CLI;
                }
            } else {
                fprintf(stderr, "Error: --optimize requires an argument\n");
                return MYCO_ERROR_CLI;
            }
        } else if (strcmp(argv[i], "--jit") == 0 || strcmp(argv[i], "-j") == 0) {
            config->jit_enabled = 1;
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                i++;
                if (strcmp(argv[i], "0") == 0 || strcmp(argv[i], "interpreted") == 0) {
                    config->jit_mode = 0;
                } else if (strcmp(argv[i], "1") == 0 || strcmp(argv[i], "hybrid") == 0) {
                    config->jit_mode = 1;
                } else if (strcmp(argv[i], "2") == 0 || strcmp(argv[i], "compiled") == 0) {
                    config->jit_mode = 2;
                } else {
                    fprintf(stderr, "Error: Unknown JIT mode '%s'\n", argv[i]);
                    return MYCO_ERROR_CLI;
                }
            } else {
                config->jit_mode = 1; // Default to hybrid mode
            }
        } else if (strcmp(argv[i], "--target") == 0 || strcmp(argv[i], "-t") == 0) {
            if (i + 1 < argc) {
                i++;
                if (strcmp(argv[i], "c") == 0) {
                    config->target = TARGET_C;
                } else if (strcmp(argv[i], "x86_64") == 0) {
                    config->target = TARGET_X86_64;
                } else if (strcmp(argv[i], "arm64") == 0) {
                    config->target = TARGET_ARM64;
                } else if (strcmp(argv[i], "wasm") == 0) {
                    config->target = TARGET_WASM;
                } else if (strcmp(argv[i], "bytecode") == 0) {
                    config->target = TARGET_BYTECODE;
                } else {
                    fprintf(stderr, "Error: Unknown target '%s'\n", argv[i]);
                    return MYCO_ERROR_CLI;
                }
            } else {
                fprintf(stderr, "Error: --target requires an argument\n");
                return MYCO_ERROR_CLI;
            }
        } else if (strcmp(argv[i], "--output") == 0 || strcmp(argv[i], "-o") == 0) {
            if (i + 1 < argc) {
                i++;
                config->output_file = argv[i];
            } else {
                fprintf(stderr, "Error: --output requires an argument\n");
                return MYCO_ERROR_CLI;
            }
        } else if (strcmp(argv[i], "--architecture") == 0 || strcmp(argv[i], "-a") == 0) {
            if (i + 1 < argc) {
                i++;
                config->architecture = argv[i];
            } else {
                fprintf(stderr, "Error: --architecture requires an argument\n");
                return MYCO_ERROR_CLI;
            }
        } else {
            fprintf(stderr, "Error: Unknown argument '%s'\n", argv[i]);
            return MYCO_ERROR_CLI;
        }
    }
    
    // Default to interpretation if no mode specified
    if (!config->interpret && !config->compile && !config->build) {
        config->interpret = 1;
    }
    
    return MYCO_SUCCESS;
}

// Print usage information
void print_usage(const char* program_name) {
    printf("Usage: %s <input> [options]\n", program_name);
    printf("\n");
    printf("Input can be:\n");
    printf("  - A file path (e.g., script.myco)\n");
    printf("  - A source string in backticks (e.g., `print(\"Hello, World!\");`)\n");
    printf("\n");
    printf("Options:\n");
    printf("  -h, --help              Show this help message\n");
    printf("  -v, --version           Show version information\n");
    printf("  -i, --interpret         Interpret the input (default)\n");
    printf("  -c, --compile           Compile the input to intermediate code\n");
    printf("  -b, --build             Build executable from input\n");
    printf("    -d, --debug             Enable debug mode\n");
  printf("  -O, --optimize <level>  Set optimization level (0/none, 1/basic, 2/aggressive, 3/maximum)\n");
  printf("  -j, --jit [mode]        Enable JIT compilation (0/interpreted, 1/hybrid, 2/compiled)\n");
  printf("  -t, --target <target>   Set compilation target (c, x86_64, arm64, wasm, bytecode)\n");
  printf("  -a, --architecture <arch> Set target architecture (arm64, x86_64, arm, x86)\n");
  printf("  -o, --output <file>     Set output file\n");
    printf("\n");
    printf("Examples:\n");
    printf("  %s script.myco\n", program_name);
    printf("  %s script.myco --debug\n", program_name);
    printf("  %s script.myco --compile --target c --output script.c\n", program_name);
    printf("  %s script.myco --build --architecture arm64\n", program_name);
    printf("  %s script.myco --build --architecture x86_64 --output myapp\n", program_name);
    printf("  %s `print(\"Hello, World!\");`\n", program_name);
}

// Print version information
void print_version(void) {
    printf("Myco Language Interpreter v2.0\n");
    printf("Built on %s at %s\n", __DATE__, __TIME__);
    printf("Copyright (c) 2024 Myco Language Project\n");
}
