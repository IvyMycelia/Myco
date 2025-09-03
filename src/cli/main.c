#include "myco.h"
#include "core/parser.h"
#include "core/ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h> // For errno

// Global variables
MemoryTracker* g_memory_tracker = NULL;
int g_myco_error_code = MYCO_SUCCESS;
char* g_myco_error_message = NULL;

// Function prototypes
static void print_banner(void);
static void print_usage(const char* program_name);
static void print_version(void);
static int process_file(const char* filename, int interpret, int compile, int debug, int target);
static int process_string(const char* source, int interpret, int compile, int debug, int target);
static void cleanup(void);

int main(int argc, char* argv[]) {
    int interpret = 0;
    int compile = 0;
    int debug = 0;
    int target = TARGET_C;
    char* input_source = NULL;
    char* output_file = NULL;
    
    // Check if we have any arguments
    if (argc < 2) {
        print_usage(argv[0]);
        return MYCO_ERROR_INTERPRETER;
    }
    
    // Check for help and version flags first (these can be anywhere)
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            print_usage(argv[0]);
            return MYCO_SUCCESS;
        } else if (strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-v") == 0) {
            print_version();
            return MYCO_SUCCESS;
        }
    }
    
    // First argument should be the input file or source
    input_source = argv[1];
    
    // Parse remaining command line arguments
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "--interpret") == 0 || strcmp(argv[i], "-i") == 0) {
            interpret = 1;
        } else if (strcmp(argv[i], "--compile") == 0 || strcmp(argv[i], "-c") == 0) {
            compile = 1;
        } else if (strcmp(argv[i], "--debug") == 0) {
            debug = 1;
        } else if (strcmp(argv[i], "--build") == 0) {
            compile = 1;
            if (i + 1 < argc) {
                if (strcmp(argv[i + 1], "c") == 0) {
                    target = TARGET_C;
                } else if (strcmp(argv[i + 1], "x86_64") == 0) {
                    target = TARGET_X86_64;
                } else if (strcmp(argv[i + 1], "arm64") == 0) {
                    target = TARGET_ARM64;
                } else if (strcmp(argv[i + 1], "wasm") == 0) {
                    target = TARGET_WASM;
                } else if (strcmp(argv[i + 1], "bytecode") == 0) {
                    target = TARGET_BYTECODE;
                } else {
                    fprintf(stderr, "Error: Unknown build target '%s'\n", argv[i + 1]);
                    print_usage(argv[0]);
                    return MYCO_ERROR_COMPILER;
                }
                i++; // Skip the target value
            } else {
                fprintf(stderr, "Error: --build requires a target\n");
                print_usage(argv[0]);
                return MYCO_ERROR_COMPILER;
            }
        } else if (strcmp(argv[i], "--target") == 0 || strcmp(argv[i], "-t") == 0) {
            if (i + 1 < argc) {
                if (strcmp(argv[i + 1], "c") == 0) {
                    target = TARGET_C;
                } else if (strcmp(argv[i + 1], "x86_64") == 0) {
                    target = TARGET_X86_64;
                } else if (strcmp(argv[i + 1], "arm64") == 0) {
                    target = TARGET_ARM64;
                } else if (strcmp(argv[i + 1], "wasm") == 0) {
                    target = TARGET_WASM;
                } else if (strcmp(argv[i + 1], "bytecode") == 0) {
                    target = TARGET_BYTECODE;
                } else {
                    fprintf(stderr, "Error: Unknown target '%s'\n", argv[i + 1]);
                    print_usage(argv[0]);
                    return MYCO_ERROR_COMPILER;
                }
                i++; // Skip the target value
            } else {
                fprintf(stderr, "Error: --target requires a value\n");
                print_usage(argv[0]);
                return MYCO_ERROR_COMPILER;
            }
        } else if (strcmp(argv[i], "--output") == 0 || strcmp(argv[i], "-o") == 0) {
            if (i + 1 < argc) {
                output_file = argv[i + 1];
                i++; // Skip the output filename
            } else {
                fprintf(stderr, "Error: --output requires a filename\n");
                print_usage(argv[0]);
                return MYCO_ERROR_COMPILER;
            }
        } else if (strcmp(argv[i], "--") == 0) {
            // End of options, rest are input files
            i++;
            break;
        } else if (argv[i][0] == '-') {
            fprintf(stderr, "Error: Unknown option '%s'\n", argv[i]);
            print_usage(argv[0]);
            return MYCO_ERROR_COMPILER;
        } else {
            // Additional input files (not supported in current version)
            fprintf(stderr, "Error: Multiple input files not supported. Use '%s <input_file> [options]'\n", argv[0]);
            print_usage(argv[0]);
            return MYCO_ERROR_INTERPRETER;
        }
    }
    
    // Set default behavior if no mode specified
    if (!interpret && !compile) {
        interpret = 1; // Default to interpretation
    }
    
    // Initialize Myco (simple initialization)
    g_memory_tracker = memory_tracker_create(MEMORY_STRATEGY_SYSTEM);
    if (!g_memory_tracker) {
        fprintf(stderr, "Error: Failed to initialize memory tracker\n");
        return MYCO_ERROR_MEMORY;
    }
    
    // Process input
    int result = MYCO_SUCCESS;
    if (input_source) {
        // Check if it's a file or a string
        if (strlen(input_source) > 0 && 
            (input_source[0] == '`' && input_source[strlen(input_source) - 1] == '`')) {
            // Treat as source string (remove quotes)
            size_t len = strlen(input_source);
            if (len >= 2) {
                char* source = malloc(len - 1);
                if (source) {
                    strncpy(source, input_source + 1, len - 2);
                    source[len - 2] = '\0';
                    result = process_string(source, interpret, compile, debug, target);
                    free(source);
                } else {
                    result = MYCO_ERROR_MEMORY;
                }
            } else {
                result = MYCO_ERROR_INTERPRETER;
            }
        } else {
            // Treat as filename
            result = process_file(input_source, interpret, compile, debug, target);
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
    printf("Myco v%s - High-Performance Programming Language\n", MYCO_VERSION_STRING);
    printf("==================================================\n");
    printf("Built with %s on %s\n", 
           MYCO_COMPILER_GCC ? "GCC" : 
           MYCO_COMPILER_CLANG ? "Clang" : 
           MYCO_COMPILER_MSVC ? "MSVC" : "Unknown Compiler",
           MYCO_PLATFORM_MACOS ? "macOS" :
           MYCO_PLATFORM_WINDOWS ? "Windows" :
           MYCO_PLATFORM_LINUX ? "Linux" : "Unknown Platform");
    printf("Target: %s\n", 
           MYCO_ARCH_X86_64 ? "x86_64" :
           MYCO_ARCH_ARM64 ? "ARM64" :
           MYCO_ARCH_ARM ? "ARM" : "Unknown Architecture");
    printf("\n");
}

static void print_usage(const char* program_name) {
    printf("Usage: %s <input_file_or_source> [options]\n", program_name);
    printf("\n");
    printf("Options:\n");
    printf("  -h, --help              Show this help message\n");
    printf("  -v, --version           Show version information\n");
    printf("  -i, --interpret         Interpret the input (default)\n");
    printf("  -c, --compile           Compile the input\n");
    printf("  --debug                 Show detailed token information\n");
    printf("  --build <target>        Compile to specific target (implies --compile)\n");
    printf("  -t, --target <target>   Set compilation target\n");
    printf("                          Targets: c, x86_64, arm64, wasm, bytecode\n");
    printf("  -o, --output <file>     Set output filename\n");
    printf("  --                      End of options\n");
    printf("\n");
    printf("Examples:\n");
    printf("  %s program.myco                    # Interpret a file\n", program_name);
    printf("  %s program.myco --build c -o program.c  # Compile to C\n", program_name);
    printf("  %s program.myco --build x86_64 -o program  # Compile to native binary\n", program_name);
    printf("  %s `print(\"Hello, World!\")`      # Interpret a string\n", program_name);
    printf("\n");
}

static void print_version(void) {
    printf("Myco v%s\n", MYCO_VERSION_STRING);
    printf("Copyright (c) 2025 Myco Language Project\n");
    printf("High-performance, memory-safe programming language\n");
}

/**
 * @brief Process a Myco source file
 * 
 * This function reads a Myco source file and processes it according
 * to the specified options. Currently supports interpretation mode.
 * 
 * @param filename The path to the Myco source file
 * @param interpret Whether to interpret the code
 * @param compile Whether to compile the code
 * @param target The compilation target (if compiling)
 * @return MYCO_SUCCESS on success, error code on failure
 */
static int process_file(const char* filename, int interpret, int compile, int debug, int target) {
    if (!filename) {
        fprintf(stderr, "Error: No filename provided\n");
        return MYCO_ERROR_CLI;
    }
    
    if (debug) {
        printf("Processing Myco source file: %s\n", filename);
        printf("==========================\n");
    }
    
    // Open and read the file
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file '%s': %s\n", filename, strerror(errno));
        return MYCO_ERROR_FILE;
    }
    
    // Get file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    if (file_size < 0) {
        fprintf(stderr, "Error: Cannot determine file size\n");
        fclose(file);
        return MYCO_ERROR_FILE;
    }
    
    // Allocate buffer for file content
    char* source = malloc(file_size + 1);
    if (!source) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        fclose(file);
        return MYCO_ERROR_MEMORY;
    }
    
    // Read file content
    size_t bytes_read = fread(source, 1, file_size, file);
    source[bytes_read] = '\0'; // Null-terminate
    fclose(file);
    
    if (bytes_read != (size_t)file_size) {
        fprintf(stderr, "Error: Failed to read entire file\n");
        free(source);
        return MYCO_ERROR_FILE;
    }
    
    if (debug) {
        printf("File size: %ld bytes\n", file_size);
        printf("==========================\n");
        printf("Mode: Interpretation\n");
    }
    
    if (interpret) {
        // Create a lexer and tokenize the source code
        Lexer* lexer = lexer_initialize(source);
        if (!lexer) {
            fprintf(stderr, "Error: Failed to initialize lexer\n");
            free(source);
            return MYCO_ERROR_MEMORY;
        }
        
        if (debug) {
            // Debug mode: show all tokens and detailed information
            printf("==========================\n");
            printf("DEBUG MODE - All Tokens:\n");
            printf("==========================\n");
            // Scan all tokens for debugging
            int token_count = lexer_scan_all(lexer);
            if (token_count < 0) {
                fprintf(stderr, "Error: Failed to scan tokens\n");
                lexer_free(lexer);
                free(source);
                return MYCO_ERROR_LEXER;
            }
            if (lexer_has_errors(lexer)) {
                printf("Warning: Lexical errors detected during tokenization\n");
            }
            lexer_print_tokens(lexer);
            printf("==========================\n");
            printf("Debug mode completed\n");
        } else {
            // Normal mode: parse and execute the program silently
            // Scan all tokens before parsing (silently)
            int token_count = lexer_scan_all(lexer);
            if (token_count < 0) {
                fprintf(stderr, "Error: Failed to scan tokens\n");
                lexer_free(lexer);
                free(source);
                return MYCO_ERROR_LEXER;
            }
            if (lexer_has_errors(lexer)) {
                fprintf(stderr, "Warning: Lexical errors detected during tokenization\n");
            }
            
            // Parse the tokens into an AST
            Parser* parser = parser_initialize(lexer);
            if (parser) {
                ASTNode* ast = parser_parse_program(parser);
                if (ast) {
                    if (parser->error_count > 0) {
                        fprintf(stderr, "Parsing completed with %d error(s). Executing valid parts...\n", parser->error_count);
                    }
                    
                    // Execute AST (even with errors for graceful handling)
                    Interpreter* interp = interpreter_create();
                    if (!interp) {
                        fprintf(stderr, "Interpreter initialization failed\n");
                    } else {
                        interpreter_register_builtins(interp);
                        interpreter_execute_program(interp, ast);
                        interpreter_free(interp);
                    }
                    
                    // Don't free the AST yet - functions might still reference it
                    // ast_free_tree(ast);
                } else {
                    fprintf(stderr, "Parsing failed: %s\n", 
                           parser->error_message ? parser->error_message : "Unknown error");
                }
                parser_free(parser);
            } else {
                fprintf(stderr, "Failed to initialize parser\n");
            }
        }
        
        // Clean up
        lexer_free(lexer);
        
        if (debug) {
            printf("==========================\n");
            printf("File processing completed\n");
        };
        
    } else if (compile) {
        printf("Mode: Compilation to %s\n", 
               target == TARGET_C ? "C" : 
               target == TARGET_X86_64 ? "x86_64" :
               target == TARGET_ARM64 ? "ARM64" :
               target == TARGET_WASM ? "WebAssembly" :
               target == TARGET_BYTECODE ? "Bytecode" : "Unknown");
        
        // TODO: Implement actual compilation
        printf("Compilation not yet implemented\n");
        
    } else {
        printf("Mode: Default (interpretation)\n");
        
        // Create a lexer and tokenize the source code
        Lexer* lexer = lexer_initialize(source);
        if (!lexer) {
            fprintf(stderr, "Error: Failed to initialize lexer\n");
            free(source);
            return MYCO_ERROR_MEMORY;
        }
        
        // Scan all tokens
        printf("DEBUG: Scanning tokens...\n");
        int token_count = lexer_scan_all(lexer);
        printf("DEBUG: Token count: %d\n", token_count);
        printf("DEBUG: Lexer errors: %s\n", lexer_has_errors(lexer) ? "Yes" : "No");
        if (token_count < 0) {
            fprintf(stderr, "Error: Failed to scan tokens\n");
            lexer_free(lexer);
            free(source);
            return MYCO_ERROR_LEXER;
        }
        
        // Check for lexical errors
        if (lexer_has_errors(lexer)) {
            printf("Warning: Lexical errors detected during tokenization\n");
        }
        
        if (debug) {
            // Debug mode: show all tokens
            printf("==========================\n");
            printf("DEBUG MODE - All Tokens:\n");
            printf("==========================\n");
            lexer_print_tokens(lexer);
            printf("==========================\n");
            printf("Debug mode completed\n");
        } else {
            // Normal mode: parse and execute the program silently
            // Parse the tokens into an AST
            Parser* parser = parser_initialize(lexer);
            if (parser) {
                ASTNode* ast = parser_parse_program(parser);
                if (ast) {
                    if (parser->error_count > 0) {
                        fprintf(stderr, "Parsing completed with %d error(s). Executing valid parts...\n", parser->error_count);
                    }
                    // Execute AST (even with errors for graceful handling)
                    Interpreter* interp = interpreter_create();
                    if (!interp) {
                        fprintf(stderr, "Interpreter initialization failed\n");
                    } else {
                        interpreter_register_builtins(interp);
                        interpreter_execute_program(interp, ast);
                        interpreter_free(interp);
                    }
                    
                    // Don't free the AST yet - functions might still reference it
                    // ast_free_tree(ast);
                } else {
                    fprintf(stderr, "Parsing failed: %s\n", 
                           parser->error_message ? parser->error_message : "Unknown error");
                }
                parser_free(parser);
            } else {
                fprintf(stderr, "Failed to initialize parser\n");
            }
        }
        
        // Clean up
        lexer_free(lexer);
        
        if (debug) {
            printf("==========================\n");
            printf("File processing completed\n");
        }
    }
    
    // Clean up
    free(source);
    
    return MYCO_SUCCESS;
}

static int process_string(const char* source, int interpret, int compile, int debug, int target) {
    if (!source) {
        fprintf(stderr, "Error: No source code provided\n");
        return MYCO_ERROR_CLI;
    }
    
    if (debug) {
        printf("Processing Myco source code...\n");
        printf("Source: %s\n", source);
        printf("==========================\n");
        printf("Mode: Interpretation\n");
    }
    
    if (interpret) {
        // Create a lexer and tokenize the source code
        Lexer* lexer = lexer_initialize(source);
        if (!lexer) {
            fprintf(stderr, "Error: Failed to initialize lexer\n");
            return MYCO_ERROR_MEMORY;
        }
        
        // Scan all tokens (silently in non-debug mode)
        int token_count = lexer_scan_all(lexer);
        if (debug) {
            printf("DEBUG: Scanning tokens...\n");
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
            if (debug) {
                printf("Warning: Lexical errors detected during tokenization\n");
            } else {
                fprintf(stderr, "Warning: Lexical errors detected during tokenization\n");
            }
        }
        
        if (debug) {
            // Debug mode: show all tokens
            printf("==========================\n");
            printf("DEBUG MODE - All Tokens:\n");
            printf("==========================\n");
            lexer_print_tokens(lexer);
            printf("==========================\n");
            printf("Debug mode completed\n");
        } else {
            // Normal mode: parse and execute the program silently
            // Parse the tokens into an AST
            Parser* parser = parser_initialize(lexer);
            if (parser) {
                ASTNode* ast = parser_parse_program(parser);
                if (ast) {
                    // Execute AST
                    Interpreter* interp = interpreter_create();
                    if (!interp) {
                        fprintf(stderr, "Interpreter initialization failed\n");
                    } else {
                        interpreter_register_builtins(interp);
                        interpreter_execute_program(interp, ast);
                        interpreter_free(interp);
                    }
                    
                    // Don't free the AST yet - functions might still reference it
                    // ast_free_tree(ast);
                } else {
                    fprintf(stderr, "Parsing failed: %s\n", 
                           parser->error_message ? parser->error_message : "Unknown error");
                }
                parser_free(parser);
            } else {
                fprintf(stderr, "Failed to initialize parser\n");
            }
        }
        
        // Clean up
        lexer_free(lexer);
        
        if (debug) {
            printf("==========================\n");
            printf("String processing completed\n");
        }
        
    } else if (compile) {
        if (debug) {
            printf("Mode: Compilation to %s\n", 
                   target == TARGET_C ? "C" : 
                   target == TARGET_X86_64 ? "x86_64" :
                   target == TARGET_ARM64 ? "ARM64" :
                   target == TARGET_WASM ? "WebAssembly" :
                   target == TARGET_BYTECODE ? "Bytecode" : "Unknown");
            
            // TODO: Implement actual compilation
            printf("Compilation not yet implemented\n");
        }
        
    } else {
        if (debug) {
            printf("Mode: Default (interpretation)\n");
        }
        
        // Create a lexer and tokenize the source code
        Lexer* lexer = lexer_initialize(source);
        if (!lexer) {
            fprintf(stderr, "Error: Failed to initialize lexer\n");
            return MYCO_ERROR_MEMORY;
        }
        
        if (debug) {
            // Debug mode: show all tokens
            printf("==========================\n");
            printf("DEBUG MODE - All Tokens:\n");
            printf("==========================\n");
            // Scan all tokens for debugging
            int token_count = lexer_scan_all(lexer);
            if (token_count < 0) {
                fprintf(stderr, "Error: Failed to scan tokens\n");
                lexer_free(lexer);
                return MYCO_ERROR_LEXER;
            }
            if (lexer_has_errors(lexer)) {
                printf("Warning: Lexical errors detected during tokenization\n");
            }
            lexer_print_tokens(lexer);
            printf("==========================\n");
            printf("Debug mode completed\n");
        } else {
            // Normal mode: parse and execute the program silently
            // Parse the tokens into an AST
            Parser* parser = parser_initialize(lexer);
            if (parser) {
                ASTNode* ast = parser_parse_program(parser);
                if (ast) {
                    // Execute AST
                    Interpreter* interp = interpreter_create();
                    if (!interp) {
                        fprintf(stderr, "Interpreter initialization failed\n");
                    } else {
                        interpreter_register_builtins(interp);
                        interpreter_execute_program(interp, ast);
                        interpreter_free(interp);
                    }
                    
                    // Don't free the AST yet - functions might still reference it
                    // ast_free_tree(ast);
                } else {
                    fprintf(stderr, "Parsing failed: %s\n", 
                           parser->error_message ? parser->error_message : "Unknown error");
                }
                parser_free(parser);
            } else {
                fprintf(stderr, "Failed to initialize parser\n");
            }
        }
        
        // Clean up
        lexer_free(lexer);
        
        if (debug) {
            printf("==========================\n");
            printf("String processing completed\n");
        }
    }
    
    return MYCO_SUCCESS;
}

static void cleanup(void) {
    if (g_memory_tracker) {
        memory_tracker_free(g_memory_tracker);
        g_memory_tracker = NULL;
    }
    
    if (g_myco_error_message) {
        free(g_myco_error_message);
        g_myco_error_message = NULL;
    }
}

