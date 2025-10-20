#include "myco.h"
#include "argument_parser.h"
#include "file_processor.h"
#include "repl.h"
#include "version.h"
#include "arduino_emitter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global variables
MemoryTracker* g_memory_tracker = NULL;
int g_myco_error_code = MYCO_SUCCESS;
char* g_myco_error_message = NULL;
// Controls whether interpreter forces AST-only execution (disables bytecode VM)
int g_force_ast_only = 0;
// Controls whether bytecode VM execution is enabled (optional with --bc/--bytecode flag)
int g_bytecode_enabled = 0;

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

    // Apply global execution toggles
    g_force_ast_only = config.ast_only ? 1 : 0;
    g_bytecode_enabled = config.bytecode_enabled ? 1 : 0;
    
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
    
    if (config.run_tests) {
        // Force AST-only mode for reliable testing BEFORE any processing
        g_force_ast_only = 1;
        
        // Run the built-in test suite (pass.myco)
        printf("MYCO COMPREHENSIVE TEST SUITE\n");
        printf("=====================================\n");
        printf("Running all tests...\n\n");
        
        // Run the test file
        result = process_file("pass.myco", 1, 0, 0, 0, 0, NULL, NULL, 0, 0, 0);
        
        // Print final status
        printf("\n");
        printf("=====================================\n");
        if (result == MYCO_SUCCESS) {
            printf("✓ ALL TESTS COMPLETED SUCCESSFULLY!\n");
        } else {
            printf("X SOME TESTS FAILED (Exit code: %d)\n", result);
        }
        printf("=====================================\n");
        
        cleanup();
        return result;
    }
    
    if (config.emit_arduino && config.input_source) {
        const char* out = config.output_file ? config.output_file : "out.ino";
        int erc = emit_arduino_sketch_from_file(config.input_source, out);
        if (erc != 0) {
            fprintf(stderr, "Error: Failed to emit Arduino sketch (code %d)\n", erc);
            result = MYCO_ERROR_CLI;
        } else {
            printf("Emitted Arduino sketch: %s\n", out);
            result = MYCO_SUCCESS;
        }
    } else if (config.input_source) {
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
        // No input specified, enter REPL mode
        REPLState* repl_state = repl_create();
        if (repl_state) {
            result = repl_run(repl_state);
            repl_free(repl_state);
        } else {
            fprintf(stderr, "Error: Failed to initialize REPL\n");
            result = MYCO_ERROR_MEMORY;
        }
    }
    
    // Cleanup
    cleanup();
    
    return result;
}

static void print_banner(void) {
    myco_print_version_info();
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
