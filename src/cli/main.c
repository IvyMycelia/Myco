#include "myco.h"
#include "argument_parser.h"
#include "file_processor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global variables
MemoryTracker* g_memory_tracker = NULL;
int g_myco_error_code = MYCO_SUCCESS;
char* g_myco_error_message = NULL;

// Function prototypes
static void print_banner(void);
static void cleanup(void);

int main(int argc, char* argv[]) {
    ArgumentConfig config;
    int result = parse_arguments(argc, argv, &config);
    
    if (result != MYCO_SUCCESS) {
        cleanup();
        return result;
    }
    
    if (config.help) {
        print_usage(argv[0]);
        cleanup();
        return MYCO_SUCCESS;
    }
    
    if (config.version) {
        print_version();
        cleanup();
        return MYCO_SUCCESS;
    }
    
    if (config.input_source) {
        // Check if it's a file or a string
        if (strlen(config.input_source) > 0 && 
            (config.input_source[0] == '`' && config.input_source[strlen(config.input_source) - 1] == '`')) {
            // Treat as source string (remove quotes)
            size_t len = strlen(config.input_source);
            if (len >= 2) {
                char* source = malloc(len - 1);
                if (source) {
                    strncpy(source, config.input_source + 1, len - 2);
                    source[len - 2] = '\0';
                    result = process_string(source, config.interpret, config.compile, config.build, config.debug, config.target, config.architecture, config.output_file, config.optimization_level, config.jit_enabled, config.jit_mode);
                    free(source);
                } else {
                    result = MYCO_ERROR_MEMORY;
                }
            } else {
                result = MYCO_ERROR_INTERPRETER;
            }
        } else {
            // Treat as filename
            result = process_file(config.input_source, config.interpret, config.compile, config.build, config.debug, config.target, config.architecture, config.output_file, config.optimization_level, config.jit_enabled, config.jit_mode);
        }
    } else {
        // No input specified, show banner and enter REPL mode
        print_banner();
        printf("No input specified. Entering REPL mode...\n");
        printf("Type 'exit' or 'quit' to exit.\n\n");
        
        // TODO: Implement REPL mode
        printf("REPL mode not yet implemented.\n");
        result = MYCO_SUCCESS;
    }
    
    // Cleanup
    cleanup();
    
    return result;
}

static void print_banner(void) {
    printf("Myco Language Interpreter v2.0\n");
    printf("================================\n");
    printf("A functional programming language inspired by fungi\n");
    printf("Built on %s at %s\n", __DATE__, __TIME__);
    printf("================================\n\n");
}

static void cleanup(void) {
    // Clean up global memory tracker if it exists
    if (g_memory_tracker) {
        // TODO: Implement memory tracker cleanup
        g_memory_tracker = NULL;
    }
    
    // Clean up global error message if it exists
    if (g_myco_error_message) {
        free(g_myco_error_message);
        g_myco_error_message = NULL;
    }
}
