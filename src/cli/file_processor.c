#include "file_processor.h"
#include "../myco.h"
#include "../core/interpreter.h"
#include "../core/lexer.h"
#include "../core/parser.h"
#include "../core/ast.h"
#include "../libs/builtin_libs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// Process a file
int process_file(const char* filename, int interpret, int compile, int debug, int target) {
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
    
    int result = process_source(source, interpret, compile, debug, target);
    free(source);
    return result;
}

// Process a source string
int process_string(const char* source, int interpret, int compile, int debug, int target) {
    if (!source) return MYCO_ERROR_CLI;
    
    if (debug) {
        printf("Processing Myco source string\n");
        printf("Source length: %zu characters\n", strlen(source));
    }
    
    return process_source(source, interpret, compile, debug, target);
}

// Process source code (common implementation)
int process_source(const char* source, int interpret, int compile, int debug, int target) {
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
    }
    
    return MYCO_ERROR_CLI;
}

// Interpret source code
int interpret_source(const char* source, int debug) {
    if (!source) return MYCO_ERROR_CLI;
    
    printf("Mode: Interpretation\n");
    
    // Create a lexer and tokenize the source code
    Lexer* lexer = lexer_initialize(source);
    if (!lexer) {
        fprintf(stderr, "Error: Failed to initialize lexer\n");
        return MYCO_ERROR_MEMORY;
    }
    
    // Scan all tokens
    if (debug) {
        printf("DEBUG: Scanning tokens...\n");
    }
    int token_count = lexer_scan_all(lexer);
    if (debug) {
        printf("DEBUG: Token count: %d\n", token_count);
        printf("DEBUG: Lexer errors: %s\n", lexer_has_errors(lexer) ? "Yes" : "No");
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
        printf("DEBUG: All tokens:\n");
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
        printf("DEBUG: Parsing program...\n");
    }
    ASTNode* program = parser_parse_program(parser);
    
    if (parser->error_count > 0) {
        printf("Warning: Parse errors detected\n");
        if (debug) {
            printf("DEBUG: Parse errors: %s (Line %d, Column %d)\n", 
                   parser->error_message, parser->error_line, parser->error_column);
        }
    }
    
    if (!program) {
        fprintf(stderr, "Error: Failed to parse program\n");
        parser_free(parser);
        lexer_free(lexer);
        return MYCO_ERROR_PARSER;
    }
    
    if (debug) {
        printf("DEBUG: Program parsed successfully\n");
        printf("DEBUG: AST root type: %d\n", program->type);
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
    
    // Register built-in libraries
    register_all_builtin_libraries(interpreter);
    
    if (debug) {
        printf("DEBUG: Executing program...\n");
    }
    
    Value result = interpreter_execute(interpreter, program);
    
    if (interpreter_has_error(interpreter)) {
        printf("Error: %s (Line %d, Column %d)\n", 
               interpreter->error_message, 
               interpreter->error_line, 
               interpreter->error_column);
        interpreter_free(interpreter);
        ast_free(program);
        parser_free(parser);
        lexer_free(lexer);
        return MYCO_ERROR_CLI;
    }
    
    if (debug) {
        printf("DEBUG: Program executed successfully\n");
        printf("DEBUG: Result type: %d\n", result.type);
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
    
    printf("Mode: Compilation to %s\n", 
           target == TARGET_C ? "C" : 
           target == TARGET_X86_64 ? "x86_64" :
           target == TARGET_ARM64 ? "ARM64" :
           target == TARGET_WASM ? "WebAssembly" :
           target == TARGET_BYTECODE ? "Bytecode" : "Unknown");
    
    // TODO: Implement actual compilation
    printf("Compilation not yet implemented\n");
    
    return MYCO_SUCCESS;
}
