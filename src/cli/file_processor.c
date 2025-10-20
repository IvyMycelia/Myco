#include "file_processor.h"
#include "../myco.h"
#include "../core/interpreter.h"
#include "../core/lexer.h"
#include "../core/parser.h"
#include "../core/ast.h"
#include "../compilation/compiler.h"
#include "../libs/builtin_libs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "../../include/utils/shared_utilities.h"

// Process a file
int process_file(const char* filename, int interpret, int compile, int build, int debug, int target, const char* architecture, const char* output_file, int optimization_level, int jit_enabled, int jit_mode) {
    if (!filename) return MYCO_ERROR_CLI;
    
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file '%s': %s\n", filename, strerror(errno));
        return MYCO_ERROR_FILE;
    }
    
    // Get file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    // Allocate buffer for file content
    char* source = shared_malloc_safe(file_size + 1, "file_processor", "unknown_function", 31);
    if (!source) {
        fclose(file);
        return MYCO_ERROR_MEMORY;
    }
    
    // Read file content
    size_t bytes_read = fread(source, 1, file_size, file);
    source[bytes_read] = '\0';
    fclose(file);
    
    if (debug) {
        printf("Processing Myco source file: %s\n", filename);
        printf("File size: %ld bytes\n", file_size);
    }
    
    int result = process_source(source, filename, interpret, compile, build, debug, target, architecture, output_file, optimization_level);
    shared_free_safe(source, "file_processor", "unknown_function", 48);
    return result;
}

// Process a source string
int process_string(const char* source, int interpret, int compile, int build, int debug, int target, const char* architecture, const char* output_file, int optimization_level, int jit_enabled, int jit_mode) {
    if (!source) return MYCO_ERROR_CLI;
    
    if (debug) {
        printf("Processing Myco source string\n");
        printf("Source length: %zu characters\n", strlen(source));
    }
    
    return process_source(source, "string_input", interpret, compile, build, debug, target, architecture, output_file, optimization_level);
}

// Process source code (common implementation)
int process_source(const char* source, const char* filename, int interpret, int compile, int build, int debug, int target, const char* architecture, const char* output_file, int optimization_level) {
    if (!source) return MYCO_ERROR_CLI;
    
    if (debug) {
        printf("Source code:\n");
        printf("---\n");
        printf("%s", source);
        printf("---\n");
    }
    
    if (interpret) {
        return interpret_source(source, filename, debug);
    } else if (compile) {
        return compile_source(source, target, debug);
    } else if (build) {
        return build_executable(source, filename, architecture, output_file, debug, optimization_level);
    }
    
    return MYCO_ERROR_CLI;
}

// Interpret source code
int interpret_source(const char* source, const char* filename, int debug) {
    if (!source) return MYCO_ERROR_CLI;
    
    if (debug) {
        printf("Mode: Interpretation\n");
    }
    
    // Create a lexer and tokenize the source code
    Lexer* lexer = lexer_initialize(source);
    if (!lexer) {
        fprintf(stderr, "Error: Failed to initialize lexer\n");
        return MYCO_ERROR_MEMORY;
    }
    
    // Scan all tokens
    if (debug) {
    }
    int token_count = lexer_scan_all(lexer);
    if (debug) {
    }
    
    if (token_count < 0) {
        fprintf(stderr, "Error: Failed to scan tokens\n");
        lexer_free(lexer);
        return MYCO_ERROR_LEXER;
    }
    
    // Check for lexical errors
    if (lexer_has_errors(lexer)) {
        printf("Warning: Lexical errors detected during tokenization\n");
    }
    
    if (debug) {
        // Debug mode: show all tokens
        for (int i = 0; i < token_count; i++) {
            Token* token = lexer_get_token(lexer, i);
            if (token) {
                printf("  Token %d: Type=%d, Text='%s', Line=%d, Column=%d\n", 
                       i, token->type, token->text ? token->text : "NULL", 
                       token->line, token->column);
            }
        }
    }
    
    // Create parser and parse tokens
    Parser* parser = parser_initialize(lexer);
    if (!parser) {
        fprintf(stderr, "Error: Failed to create parser\n");
        lexer_free(lexer);
        return MYCO_ERROR_MEMORY;
    }
    
    // Parse the program
    if (debug) {
    }
    ASTNode* program = parser_parse_program(parser);
    
    if (parser->error_count > 0) {
        printf("Warning: Parse errors detected\n");
        if (debug) {
            //        parser->error_message, parser->error_line, parser->error_column);
        }
    }
    
    if (!program) {
        fprintf(stderr, "Error: Failed to parse program\n");
        parser_free(parser);
        lexer_free(lexer);
        return MYCO_ERROR_PARSER;
    }
    
    if (debug) {
    }
    
    // Create interpreter and execute program
    Interpreter* interpreter = interpreter_create();
    if (!interpreter) {
        fprintf(stderr, "Error: Failed to create interpreter\n");
        ast_free(program);
        parser_free(parser);
        lexer_free(lexer);
        return MYCO_ERROR_MEMORY;
    }
    
    // Enable test mode for test suite files
    if (strstr(filename, "pass.myco") != NULL || strstr(filename, "test_") != NULL) {
        interpreter_set_test_mode(interpreter, 1);
    }
    
    // Configure JIT if enabled
    // TODO: Re-enable JIT integration once parameter issue is resolved
    // JIT parameters are accepted but not used for now
    
    // Register built-in libraries
    register_all_builtin_libraries(interpreter);
    
    // Set source for line extraction in error traces
    interpreter_set_source(interpreter, source, filename);
    
    if (debug) {
    }
    
    Value result = interpreter_execute_program(interpreter, program);
    
    if (interpreter_has_error(interpreter)) {
        // Errors are now printed live, so we just need to clean up
        interpreter_free(interpreter);
        ast_free(program);
        parser_free(parser);
        lexer_free(lexer);
        return MYCO_ERROR_CLI;
    }
    
    if (debug) {
    }
    
    // Clean up
    interpreter_free(interpreter);
    ast_free(program);
    parser_free(parser);
    lexer_free(lexer);
    
    if (debug) {
        printf("==========================\n");
        printf("File processing completed\n");
    }
    
    return MYCO_SUCCESS;
}

// Compile source code
int compile_source(const char* source, int target, int debug) {
    if (!source) return MYCO_ERROR_CLI;
    
    if (debug) {
        printf("Mode: Compilation to %s\n", 
               target == TARGET_C ? "C" : 
               target == TARGET_X86_64 ? "x86_64" :
               target == TARGET_ARM64 ? "ARM64" :
               target == TARGET_WASM ? "WebAssembly" :
               target == TARGET_BYTECODE ? "Bytecode" : "Unknown");
    }
    
    // Create a lexer and tokenize the source code
    Lexer* lexer = lexer_initialize(source);
    if (!lexer) {
        fprintf(stderr, "Error: Failed to initialize lexer\n");
        return MYCO_ERROR_MEMORY;
    }
    
    // Scan all tokens
    if (debug) {
    }
    int token_count = lexer_scan_all(lexer);
    if (debug) {
    }
    
    if (token_count < 0) {
        fprintf(stderr, "Error: Failed to scan tokens\n");
        lexer_free(lexer);
        return MYCO_ERROR_LEXER;
    }
    
    // Check for lexical errors
    if (lexer_has_errors(lexer)) {
        fprintf(stderr, "Warning: Lexical errors detected during tokenization\n");
    }
    
    // Create a parser and parse the tokens
    Parser* parser = parser_initialize(lexer);
    if (!parser) {
        fprintf(stderr, "Error: Failed to initialize parser\n");
        lexer_free(lexer);
        return MYCO_ERROR_MEMORY;
    }
    
    // Parse the program
    ASTNode* program = parser_parse_program(parser);
    
    // Check for parser errors
    if (parser->error_message) {
        fprintf(stderr, "Error: Parser error: %s\n", parser->error_message);
        if (program) ast_free(program);
        parser_free(parser);
        lexer_free(lexer);
        return MYCO_ERROR_PARSER;
    }
    
    if (!program) {
        fprintf(stderr, "Error: Failed to parse program\n");
        parser_free(parser);
        lexer_free(lexer);
        return MYCO_ERROR_PARSER;
    }
    
    if (debug) {
    }
    
    // Create compiler configuration
    CompilerConfig* config = compiler_config_create();
    if (!config) {
        fprintf(stderr, "Error: Failed to create compiler configuration\n");
        ast_free(program);
        parser_free(parser);
        lexer_free(lexer);
        return MYCO_ERROR_MEMORY;
    }
    
    // Set target architecture
    compiler_config_set_target(config, (TargetArchitecture)target);
    
    // Set optimization level (temporarily disabled for testing)
    compiler_config_set_optimization(config, OPTIMIZATION_BASIC);
    
    // Set output file
    const char* output_file = "output.c";
    compiler_config_set_output(config, output_file);
    
    // Generate C code
    if (target == TARGET_C) {
        if (debug) {
        }
        
        if (!compiler_generate_c(config, program, output_file)) {
            fprintf(stderr, "Error: Failed to generate C code\n");
            compiler_config_free(config);
            ast_free(program);
            parser_free(parser);
            lexer_free(lexer);
            return MYCO_ERROR_COMPILER;
        }
        
        printf("Successfully compiled to C: %s\n", output_file);
    } else {
        fprintf(stderr, "Error: Target architecture %d not yet supported\n", target);
        compiler_config_free(config);
        ast_free(program);
        parser_free(parser);
        lexer_free(lexer);
        return MYCO_ERROR_COMPILER;
    }
    
    // Clean up
    compiler_config_free(config);
    ast_free(program);
    parser_free(parser);
    lexer_free(lexer);
    
    if (debug) {
        printf("==========================\n");
        printf("Compilation completed\n");
    }
    
    return MYCO_SUCCESS;
}

// Build executable from source
int build_executable(const char* source, const char* filename, const char* architecture, const char* output_file, int debug, int optimization_level) {
    if (!source) return MYCO_ERROR_CLI;
    
    if (debug) {
        printf("Mode: Building executable\n");
        if (architecture) {
            printf("Target architecture: %s\n", architecture);
        }
        if (output_file) {
            printf("Output file: %s\n", output_file);
        }
    }
    
    // Create a lexer and tokenize the source code
    Lexer* lexer = lexer_initialize(source);
    if (!lexer) {
        fprintf(stderr, "Error: Failed to initialize lexer\n");
        return MYCO_ERROR_MEMORY;
    }
    
    // Scan all tokens
    if (debug) {
    }
    int token_count = lexer_scan_all(lexer);
    if (debug) {
    }
    
    if (token_count < 0) {
        fprintf(stderr, "Error: Failed to scan tokens\n");
        lexer_free(lexer);
        return MYCO_ERROR_LEXER;
    }
    
    // Check for lexical errors
    if (lexer_has_errors(lexer)) {
        fprintf(stderr, "Warning: Lexical errors detected during tokenization\n");
    }
    
    // Create a parser and parse the tokens
    Parser* parser = parser_initialize(lexer);
    if (!parser) {
        fprintf(stderr, "Error: Failed to initialize parser\n");
        lexer_free(lexer);
        return MYCO_ERROR_MEMORY;
    }
    
    // Parse the program
    ASTNode* program = parser_parse_program(parser);
    
    // Check for parser errors
    if (parser->error_message) {
        fprintf(stderr, "Error: Parser error: %s\n", parser->error_message);
        if (program) ast_free(program);
        parser_free(parser);
        lexer_free(lexer);
        return MYCO_ERROR_PARSER;
    }
    
    if (!program) {
        fprintf(stderr, "Error: Failed to parse program\n");
        parser_free(parser);
        lexer_free(lexer);
        return MYCO_ERROR_PARSER;
    }
    
    if (debug) {
    }
    
    // Create compiler configuration
    CompilerConfig* config = compiler_config_create();
    if (!config) {
        fprintf(stderr, "Error: Failed to create compiler configuration\n");
        ast_free(program);
        parser_free(parser);
        lexer_free(lexer);
        return MYCO_ERROR_MEMORY;
    }
    
    // Set target to C for compilation
    compiler_config_set_target(config, TARGET_C);
    
    // Set optimization level
    compiler_config_set_optimization(config, optimization_level);
    
    // Generate temporary C filename
    char* c_output_file = shared_malloc_safe(256, "file_processor", "unknown_function", 445);
    if (c_output_file) {
        snprintf(c_output_file, 256, "temp_output.c");
    }
    
    if (!c_output_file) {
        fprintf(stderr, "Error: Failed to allocate output filename\n");
        compiler_config_free(config);
        ast_free(program);
        parser_free(parser);
        lexer_free(lexer);
        return MYCO_ERROR_MEMORY;
    }
    
    // Set output file
    compiler_config_set_output(config, c_output_file);
    
    // Generate C code
    if (debug) {
    }
    
    if (!compiler_generate_c(config, program, c_output_file)) {
        fprintf(stderr, "Error: Failed to generate C code\n");
        shared_free_safe(c_output_file, "file_processor", "unknown_function", 470);
        compiler_config_free(config);
        ast_free(program);
        parser_free(parser);
        lexer_free(lexer);
        return MYCO_ERROR_COMPILER;
    }
    
    if (debug) {
    }
    
    // Compile C code to executable
    char* final_output;
    if (output_file) {
        final_output = (output_file ? strdup(output_file) : NULL);
    } else {
        // Use the input filename without extension
        char* base_name = (filename ? strdup(filename) : NULL);
        char* dot = strrchr(base_name, '.');
        if (dot) {
            *dot = '\0';  // Remove the extension
        }
        final_output = shared_malloc_safe(strlen(base_name) + 1, "file_processor", "unknown_function", 494);
        strcpy(final_output, base_name);
        shared_free_safe(base_name, "file_processor", "unknown_function", 496);
    }
    if (!final_output) {
        fprintf(stderr, "Error: Failed to allocate final output filename\n");
        shared_free_safe(c_output_file, "file_processor", "unknown_function", 500);
        compiler_config_free(config);
        ast_free(program);
        parser_free(parser);
        lexer_free(lexer);
        return MYCO_ERROR_MEMORY;
    }
    
    // Compile C code to binary using the compiler
    if (debug) {
    }
    
    if (!compiler_compile_to_binary(config, c_output_file, final_output)) {
        fprintf(stderr, "Error: Failed to compile C code to binary\n");
        shared_free_safe(c_output_file, "file_processor", "unknown_function", 515);
        shared_free_safe(final_output, "file_processor", "unknown_function", 516);
        compiler_config_free(config);
        ast_free(program);
        parser_free(parser);
        lexer_free(lexer);
        return MYCO_ERROR_COMPILER;
    }
    
    printf("Successfully built executable: %s\n", final_output);
    
    // Clean up temporary files
    // Temporarily comment out file removal for debugging
    // if (remove(c_output_file) != 0) {
    //     if (debug) {
    //     }
    // }
    
    // Clean up
    shared_free_safe(c_output_file, "file_processor", "unknown_function", 535);
    shared_free_safe(final_output, "file_processor", "unknown_function", 536);
    compiler_config_free(config);
    ast_free(program);
    parser_free(parser);
    lexer_free(lexer);
    
    if (debug) {
        printf("==========================\n");
        printf("Build completed\n");
    }
    
    return MYCO_SUCCESS;
    
    // Generate C code
    if (debug) {
    }
    
    int result = compiler_generate_c(config, program, c_output_file);
    if (!result) {
        fprintf(stderr, "Error: Failed to generate C code\n");
        shared_free_safe(c_output_file, "file_processor", "unknown_function", 557);
        compiler_config_free(config);
        ast_free(program);
        parser_free(parser);
        lexer_free(lexer);
        return MYCO_ERROR_COMPILER;
    }
    
    if (debug) {
    }
    
    // Compile C code to executable
    char* executable_name = NULL;
    if (output_file) {
        executable_name = (output_file ? strdup(output_file) : NULL);
    } else {
        executable_name = ("output" ? strdup("output") : NULL);
    }
    
    if (!executable_name) {
        fprintf(stderr, "Error: Failed to allocate executable name\n");
        shared_free_safe(c_output_file, "file_processor", "unknown_function", 579);
        compiler_config_free(config);
        ast_free(program);
        parser_free(parser);
        lexer_free(lexer);
        return MYCO_ERROR_MEMORY;
    }
    
    // Build the executable
    if (debug) {
    }
    
    char compile_command[1024];
    const char* arch_flag = "";
    
    // Set architecture-specific flags
    if (architecture) {
        if (strcmp(architecture, "arm64") == 0) {
            arch_flag = "-arch arm64";
        } else if (strcmp(architecture, "x86_64") == 0) {
            arch_flag = "-arch x86_64";
        } else if (strcmp(architecture, "arm") == 0) {
            arch_flag = "-arch armv7";
        } else if (strcmp(architecture, "x86") == 0) {
            arch_flag = "-arch i386";
        }
    }
    
    // Build compilation command
    snprintf(compile_command, sizeof(compile_command), 
             "gcc -std=c99 -Wall -Wextra -pedantic -O2 %s -Iinclude -Iinclude/core -Iinclude/compilation -Iinclude/runtime -Iinclude/cli -Iinclude/utils %s -o %s -lm",
             arch_flag, c_output_file, executable_name);
    
    if (debug) {
    }
    
    // Execute compilation
    int compile_result = system(compile_command);
    if (compile_result != 0) {
        fprintf(stderr, "Error: Failed to compile C code to executable\n");
        shared_free_safe(executable_name, "file_processor", "unknown_function", 621);
        shared_free_safe(c_output_file, "file_processor", "unknown_function", 622);
        compiler_config_free(config);
        ast_free(program);
        parser_free(parser);
        lexer_free(lexer);
        return MYCO_ERROR_COMPILER;
    }
    
    if (debug) {
    }
    
    // Keep temporary C file for debugging if debug mode is on
    if (debug) {
    } else {
        // Clean up temporary C file
        // Temporarily comment out file removal for debugging
        // if (remove(c_output_file) != 0) {
        //     printf("Warning: Could not remove temporary file %s\n", c_output_file);
        // }
    }
    
    // Clean up
    shared_free_safe(executable_name, "file_processor", "unknown_function", 646);
    shared_free_safe(c_output_file, "file_processor", "unknown_function", 647);
    compiler_config_free(config);
    ast_free(program);
    parser_free(parser);
    lexer_free(lexer);
    
    printf("Build successful! Executable: %s\n", executable_name);
    return MYCO_SUCCESS;
}
