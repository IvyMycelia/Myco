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
    char* source = malloc(file_size + 1);
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
    free(source);
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
        return interpret_source(source, debug);
    } else if (compile) {
        return compile_source(source, target, debug);
    } else if (build) {
        return build_executable(source, filename, architecture, output_file, debug, optimization_level);
    }
    
    return MYCO_ERROR_CLI;
}

// Interpret source code
int interpret_source(const char* source, int debug) {
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
        // printf("DEBUG: Scanning tokens...\n");
    }
    int token_count = lexer_scan_all(lexer);
    if (debug) {
        // printf("DEBUG: Token count: %d\n", token_count);
        // printf("DEBUG: Lexer errors: %s\n", lexer_has_errors(lexer) ? "Yes" : "No");
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
        // printf("DEBUG: All tokens:\n");
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
        // printf("DEBUG: Parsing program...\n");
    }
    ASTNode* program = parser_parse_program(parser);
    
    if (parser->error_count > 0) {
        printf("Warning: Parse errors detected\n");
        if (debug) {
            // printf("DEBUG: Parse errors: %s (Line %d, Column %d)\n", 
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
        // printf("DEBUG: Program parsed successfully\n");
        // printf("DEBUG: AST root type: %d\n", program->type);
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
    
    // Configure JIT if enabled
    // TODO: Re-enable JIT integration once parameter issue is resolved
    // JIT parameters are accepted but not used for now
    
    // Register built-in libraries
    register_all_builtin_libraries(interpreter);
    
    if (debug) {
        // printf("DEBUG: Executing program...\n");
    }
    
    Value result = interpreter_execute(interpreter, program);
    
    if (interpreter_has_error(interpreter)) {
        // Errors are now printed live, so we just need to clean up
        interpreter_free(interpreter);
        ast_free(program);
        parser_free(parser);
        lexer_free(lexer);
        return MYCO_ERROR_CLI;
    }
    
    if (debug) {
        // printf("DEBUG: Program executed successfully\n");
        // printf("DEBUG: Result type: %d\n", result.type);
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
        // printf("DEBUG: Scanning tokens...\n");
    }
    int token_count = lexer_scan_all(lexer);
    if (debug) {
        // printf("DEBUG: Token count: %d\n", token_count);
        // printf("DEBUG: Lexer errors: %s\n", lexer_has_errors(lexer) ? "Yes" : "No");
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
        // printf("DEBUG: Program parsed successfully\n");
        // printf("DEBUG: AST node count: %d\n", 1); // Simplified count
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
            // printf("DEBUG: Generating C code...\n");
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
        // printf("DEBUG: Scanning tokens...\n");
    }
    int token_count = lexer_scan_all(lexer);
    if (debug) {
        // printf("DEBUG: Token count: %d\n", token_count);
        // printf("DEBUG: Lexer errors: %s\n", lexer_has_errors(lexer) ? "Yes" : "No");
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
        // printf("DEBUG: Program parsed successfully\n");
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
    char* c_output_file = malloc(256);
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
        // printf("DEBUG: Generating C code...\n");
    }
    
    // printf("DEBUG: About to call compiler_generate_c\n");
    if (!compiler_generate_c(config, program, c_output_file)) {
        fprintf(stderr, "Error: Failed to generate C code\n");
        free(c_output_file);
        compiler_config_free(config);
        ast_free(program);
        parser_free(parser);
        lexer_free(lexer);
        return MYCO_ERROR_COMPILER;
    }
    // printf("DEBUG: compiler_generate_c completed successfully\n");
    
    if (debug) {
        // printf("DEBUG: C code generated successfully\n");
    }
    
    // Compile C code to executable
    char* final_output;
    if (output_file) {
        final_output = strdup(output_file);
    } else {
        // Use the input filename without extension
        char* base_name = strdup(filename);
        char* dot = strrchr(base_name, '.');
        if (dot) {
            *dot = '\0';  // Remove the extension
        }
        final_output = malloc(strlen(base_name) + 1);
        strcpy(final_output, base_name);
        free(base_name);
    }
    if (!final_output) {
        fprintf(stderr, "Error: Failed to allocate final output filename\n");
        free(c_output_file);
        compiler_config_free(config);
        ast_free(program);
        parser_free(parser);
        lexer_free(lexer);
        return MYCO_ERROR_MEMORY;
    }
    
    // Compile C code to binary using the compiler
    if (debug) {
        // printf("DEBUG: Compiling C code to binary...\n");
    }
    
    if (!compiler_compile_to_binary(config, c_output_file, final_output)) {
        fprintf(stderr, "Error: Failed to compile C code to binary\n");
        free(c_output_file);
        free(final_output);
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
    //         printf("DEBUG: Warning: Could not remove temporary file %s\n", c_output_file);
    //     }
    // }
    
    // Clean up
    free(c_output_file);
    free(final_output);
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
        // printf("DEBUG: Generating C code...\n");
    }
    
    int result = compiler_generate_c(config, program, c_output_file);
    if (!result) {
        fprintf(stderr, "Error: Failed to generate C code\n");
        free(c_output_file);
        compiler_config_free(config);
        ast_free(program);
        parser_free(parser);
        lexer_free(lexer);
        return MYCO_ERROR_COMPILER;
    }
    
    if (debug) {
        // printf("DEBUG: C code generated successfully: %s\n", c_output_file);
    }
    
    // Compile C code to executable
    char* executable_name = NULL;
    if (output_file) {
        executable_name = strdup(output_file);
    } else {
        executable_name = strdup("output");
    }
    
    if (!executable_name) {
        fprintf(stderr, "Error: Failed to allocate executable name\n");
        free(c_output_file);
        compiler_config_free(config);
        ast_free(program);
        parser_free(parser);
        lexer_free(lexer);
        return MYCO_ERROR_MEMORY;
    }
    
    // Build the executable
    if (debug) {
        // printf("DEBUG: Compiling C code to executable...\n");
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
        // printf("DEBUG: Compilation command: %s\n", compile_command);
    }
    
    // Execute compilation
    int compile_result = system(compile_command);
    if (compile_result != 0) {
        fprintf(stderr, "Error: Failed to compile C code to executable\n");
        free(executable_name);
        free(c_output_file);
        compiler_config_free(config);
        ast_free(program);
        parser_free(parser);
        lexer_free(lexer);
        return MYCO_ERROR_COMPILER;
    }
    
    if (debug) {
        // printf("DEBUG: Executable built successfully: %s\n", executable_name);
    }
    
    // Keep temporary C file for debugging if debug mode is on
    if (debug) {
        // printf("DEBUG: Keeping temporary C file for inspection: %s\n", c_output_file);
    } else {
        // Clean up temporary C file
        // Temporarily comment out file removal for debugging
        // if (remove(c_output_file) != 0) {
        //     printf("Warning: Could not remove temporary file %s\n", c_output_file);
        // }
    }
    
    // Clean up
    free(executable_name);
    free(c_output_file);
    compiler_config_free(config);
    ast_free(program);
    parser_free(parser);
    lexer_free(lexer);
    
    printf("Build successful! Executable: %s\n", executable_name);
    return MYCO_SUCCESS;
}
