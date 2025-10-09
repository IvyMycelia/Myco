#include "repl.h"
#include "error_handling.h"
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include "../../include/utils/shared_utilities.h"

// Global REPL state for signal handling
static REPLState* g_repl_state = NULL;

// Forward declarations
static int repl_execute_input(REPLState* state, const char* input);

// Signal handler for graceful exit
static void repl_signal_handler(int sig) {
    (void)sig; // Suppress unused parameter warning
    if (g_repl_state) {
        printf("\n");
        repl_free(g_repl_state);
        g_repl_state = NULL;
    }
    exit(0);
}

/**
 * @brief Create a new REPL state
 * 
 * Initializes all REPL state variables and creates a new interpreter.
 * 
 * @return A pointer to the initialized REPL state, or NULL if allocation failed
 */
REPLState* repl_create(void) {
    REPLState* state = shared_malloc_safe(sizeof(REPLState), "repl", "unknown_function", 34);
    if (!state) {
        return NULL;
    }
    
    // Initialize interpreter
    state->interpreter = interpreter_create();
    if (!state->interpreter) {
        shared_free_safe(state, "repl", "unknown_function", 42);
        return NULL;
    }
    
    // Register built-in libraries
    extern void register_all_builtin_libraries(Interpreter* interpreter);
    register_all_builtin_libraries(state->interpreter);
    
    // Initialize state variables
    state->line_number = 1;
    state->debug_mode = 0;
    state->debug_ast = 0;
    state->debug_lexer = 0;
    state->debug_parser = 0;
    
    // Initialize history
    state->history = NULL;
    state->history_count = 0;
    state->history_capacity = 0;
    state->history_index = 0;
    
    // Initialize input buffer
    state->current_input = NULL;
    state->input_length = 0;
    state->input_capacity = 0;
    
    // Set up signal handlers
    signal(SIGINT, repl_signal_handler);
    signal(SIGTERM, repl_signal_handler);
    g_repl_state = state;
    
    return state;
}

/**
 * @brief Free REPL state and cleanup
 * 
 * @param state The REPL state to free
 */
void repl_free(REPLState* state) {
    if (!state) {
        return;
    }
    
    // Free interpreter
    if (state->interpreter) {
        interpreter_free(state->interpreter);
    }
    
    // Free history
    if (state->history) {
        for (int i = 0; i < state->history_count; i++) {
            shared_free_safe(state->history[i], "repl", "unknown_function", 90);
        }
        shared_free_safe(state->history, "repl", "unknown_function", 92);
    }
    
    // Free input buffer
    if (state->current_input) {
        shared_free_safe(state->current_input, "repl", "unknown_function", 97);
    }
    
    shared_free_safe(state, "repl", "unknown_function", 100);
    g_repl_state = NULL;
}

/**
 * @brief Run the REPL main loop
 * 
 * @param state The REPL state
 * @return 0 on success, non-zero on error
 */
int repl_run(REPLState* state) {
    if (!state) {
        return -1;
    }
    
    char* input;
    
    while (1) {
        // Read input
        input = repl_read_line(state);
        if (!input) {
            break;  // EOF or error
        }
        
        // Skip empty input
        if (strlen(input) == 0) {
            shared_free_safe(input, "repl", "unknown_function", 126);
            continue;
        }
        
        // Check for exit commands
        if (strcmp(input, "exit") == 0 || strcmp(input, "quit") == 0) {
            shared_free_safe(input, "repl", "unknown_function", 132);
            break;
        }
        
        // Process the input
        repl_process_input(state, input);
        
        // Add to history
        repl_add_to_history(state, input);
        
        shared_free_safe(input, "repl", "unknown_function", 142);
    }
    
    return 0;
}

/**
 * @brief Process a single input line
 * 
 * @param state The REPL state
 * @param input The input string
 * @return 0 on success, non-zero on error
 */
int repl_process_input(REPLState* state, const char* input) {
    if (!state || !input) {
        return -1;
    }
    
    // Check for special commands
    if (input[0] == ':') {
        return repl_handle_command(state, input);
    }
    
    // Check if this is a multiline continuation
    if (repl_is_multiline_continuation(input)) {
        // Strip the "..." prefix if present
        const char* content = input;
        if (strncmp(input, "...", 3) == 0) {
            content = input + 3;
            // Skip whitespace after "..."
            while (*content == ' ' || *content == '\t') {
                content++;
            }
        }
        
        // Append to current input
        if (state->current_input) {
            size_t new_len = strlen(state->current_input) + strlen(content) + 2;
            if (new_len > state->input_capacity) {
                state->input_capacity = new_len * 2;
                state->current_input = shared_realloc_safe(state->current_input, state->input_capacity, "repl", "unknown_function", 182);
            }
            strcat(state->current_input, "\n");
            strcat(state->current_input, content);
        } else {
            // Start new multiline input
            state->input_capacity = strlen(content) + 1;
            state->current_input = shared_malloc_safe(state->input_capacity, "repl", "unknown_function", 189);
            strcpy(state->current_input, content);
        }
        
        // Check if input is complete
        if (repl_is_input_complete(state->current_input)) {
            char* complete_input = state->current_input;
            state->current_input = NULL;
            state->input_length = 0;
            state->input_capacity = 0;
            
            
            int result = repl_execute_input(state, complete_input);
            shared_free_safe(complete_input, "repl", "unknown_function", 202);
            return result;
        } else {
            // Show continuation prompt
            printf("... ");
            fflush(stdout);
            return 0;
        }
    }
    
    // Single line input
    return repl_execute_input(state, input);
}

/**
 * @brief Execute Myco code input
 * 
 * @param state The REPL state
 * @param input The complete input string
 * @return 0 on success, non-zero on error
 */
static int repl_execute_input(REPLState* state, const char* input) {
    if (!state || !input) {
        return -1;
    }
    
    // Show debug info if enabled
    if (state->debug_mode) {
        repl_show_debug_info(state, input);
    }

    // Fast path: avoid unnecessary overhead when debugging is off
    
    // Create lexer
    Lexer* lexer = lexer_initialize(input);
    if (!lexer) {
        repl_print_error(state, "Failed to initialize lexer", state->line_number);
        return -1;
    }
    
    // Show lexer debug info
    if (state->debug_lexer) {
        repl_print_debug_lexer(state, lexer);
    }
    
    // Scan tokens
    int token_count = lexer_scan_all(lexer);
    if (token_count < 0) {
        repl_print_error(state, "Failed to scan tokens", state->line_number);
        lexer_free(lexer);
        return -1;
    }
    
    // Check for lexical errors
    if (lexer_has_errors(lexer)) {
        repl_print_error(state, "Lexical errors detected", state->line_number);
        lexer_free(lexer);
        return -1;
    }
    
    // Create parser
    Parser* parser = parser_initialize(lexer);
    if (!parser) {
        repl_print_error(state, "Failed to create parser", state->line_number);
        lexer_free(lexer);
        return -1;
    }
    
    // Show parser debug info
    if (state->debug_parser) {
        repl_print_debug_parser(state, parser);
    }
    
    // Set source for line extraction in error traces
    interpreter_set_source(state->interpreter, input, "<stdin>");
    
    // Parse program while suppressing raw parser prints; REPL will format errors centrally
    FILE* devnull = fopen("/dev/null", "w");
    int saved_out = -1, saved_err = -1;
    if (devnull) {
        fflush(stdout); fflush(stderr);
        saved_out = dup(fileno(stdout));
        saved_err = dup(fileno(stderr));
        if (saved_out >= 0) dup2(fileno(devnull), fileno(stdout));
        if (saved_err >= 0) dup2(fileno(devnull), fileno(stderr));
    }
    ASTNode* program = parser_parse_program(parser);
    if (devnull) {
        fflush(stdout); fflush(stderr);
        if (saved_out >= 0) { dup2(saved_out, fileno(stdout)); close(saved_out); }
        if (saved_err >= 0) { dup2(saved_err, fileno(stderr)); close(saved_err); }
        fclose(devnull);
    }
    
    // Show AST debug info
    if (state->debug_ast && program) {
        repl_print_debug_ast(state, program);
    }
    
    // Check for parse errors
    if (parser->error_count > 0) {
        // Use enhanced error reporter for consistent formatting
        interpreter_report_error_enhanced(state->interpreter,
                                          parser->error_message ? parser->error_message : "Parse error",
                                          parser->error_line,
                                          parser->error_column);
        repl_show_error_suggestion(parser->error_message, state->line_number);
        ast_free_tree(program);
        parser_free(parser);
        lexer_free(lexer);
        return -1;
    }
    
    // Execute program
    if (program) {
        Value result = interpreter_execute_program(state->interpreter, program);
        
        // Check for interpreter errors
        if (interpreter_has_error(state->interpreter)) {
            // Error was already reported by the interpreter, just clean up
            ast_free_tree(program);
            parser_free(parser);
            lexer_free(lexer);
            return -1;
        }
        
        // Show result
        repl_print_result(state, &result);
        
        // Clean up
        ast_free_tree(program);
    }
    
    // Clean up
    parser_free(parser);
    lexer_free(lexer);
    
    state->line_number++;
    return 0;
}

/**
 * @brief Handle special REPL commands
 * 
 * @param state The REPL state
 * @param command The command string
 * @return 0 on success, non-zero on error
 */
int repl_handle_command(REPLState* state, const char* command) {
    if (!state || !command) {
        return -1;
    }
    
    // Skip the ':'
    const char* cmd = command + 1;
    
    if (strcmp(cmd, "help") == 0) {
        repl_show_help();
    } else if (strcmp(cmd, "clear") == 0) {
        repl_clear_variables(state);
        printf("Variables cleared.\n");
    } else if (strcmp(cmd, "reset") == 0) {
        repl_reset_state(state);
        printf("REPL state reset.\n");
    } else if (strncmp(cmd, "load ", 5) == 0) {
        repl_load_file(state, cmd + 5);
    } else if (strncmp(cmd, "debug ", 6) == 0) {
        repl_set_debug_mode(state, cmd + 6);
    } else if (strncmp(cmd, "colors ", 7) == 0 || strncmp(cmd, "color ", 6) == 0) {
        const char* opt = (cmd[1] == 'c' && cmd[2] == 'o' && cmd[3] == 'l' && cmd[4] == 'o' && cmd[5] == 'r' && cmd[6] == 's' && cmd[7] == ' ')
            ? (cmd + 7)
            : (cmd + 6);
        if (strcmp(opt, "on") == 0) {
            error_colors_enable(1);
            printf("Colors enabled.\n");
        } else if (strcmp(opt, "off") == 0) {
            error_colors_enable(0);
            printf("Colors disabled.\n");
        } else {
            printf("Usage: :colors on|off\n");
        }
    } else if (strncmp(cmd, "stack ", 6) == 0) {
        ErrorSystem* sys = error_system_get_global();
        const char* opt = cmd + 6;
        if (strcmp(opt, "on") == 0) {
            error_enable_stack_trace(sys, true);
            printf("Stack traces enabled.\n");
        } else if (strcmp(opt, "off") == 0) {
            error_enable_stack_trace(sys, false);
            printf("Stack traces disabled.\n");
        } else {
            printf("Usage: :stack on|off\n");
        }
    } else if (strcmp(cmd, "errors") == 0) {
        // Print the last error with full context/stack
        error_print_last();
    } else if (strcmp(cmd, "vars") == 0) {
        repl_show_variables(state);
    } else if (strcmp(cmd, "types") == 0) {
        repl_show_variable_types(state);
    } else if (strcmp(cmd, "memory") == 0) {
        repl_show_memory_usage(state);
    } else if (strcmp(cmd, "trace") == 0) {
        repl_show_call_trace(state);
    } else if (strncmp(cmd, "inspect ", 8) == 0) {
        repl_inspect_variable(state, cmd + 8);
    } else if (strcmp(cmd, "breakpoints") == 0) {
        repl_show_breakpoints(state);
    } else if (strncmp(cmd, "break ", 6) == 0) {
        repl_set_breakpoint(state, cmd + 6);
    } else if (strncmp(cmd, "unbreak ", 8) == 0) {
        repl_remove_breakpoint(state, cmd + 8);
    } else if (strcmp(cmd, "step") == 0) {
        repl_step_execution(state);
    } else if (strcmp(cmd, "continue") == 0) {
        repl_continue_execution(state);
    } else if (strcmp(cmd, "profile") == 0) {
        repl_show_performance_profile(state);
    } else {
        printf("Unknown command: %s\n", cmd);
        printf("Type :help for available commands.\n");
    }
    
    return 0;
}

/**
 * @brief Show help information
 */
void repl_show_help(void) {
    printf("Myco REPL Commands:\n");
    printf("  :help              Show this help message\n");
    printf("  :clear             Clear all variables\n");
    printf("  :reset             Reset REPL state\n");
    printf("  :vars              Show all variables\n");
    printf("  :types             Show variable types with details\n");
    printf("  :memory            Show memory usage statistics\n");
    printf("  :trace             Show call trace information\n");
    printf("  :inspect <var>     Inspect a specific variable in detail\n");
    printf("  :load <file>       Load and execute a Myco file\n");
    printf("  :debug <mode>      Set debug mode (--ast, --lexer, --parser, --all, --off)\n");
    printf("  :colors on|off     Enable/disable colored error output\n");
    printf("  :stack on|off      Enable/disable stack traces in errors\n");
    printf("  :errors            Print last error with full context/stack\n");
    printf("  :breakpoints       Show breakpoints (future)\n");
    printf("  :break <loc>       Set breakpoint (future)\n");
    printf("  :unbreak <loc>     Remove breakpoint (future)\n");
    printf("  :step              Step execution (future)\n");
    printf("  :continue          Continue execution (future)\n");
    printf("  :profile           Show performance profile\n");
    printf("  exit, quit         Exit the REPL\n");
    printf("\n");
    printf("Multi-line input:\n");
    printf("  Use '...' continuation for blocks and functions\n");
    printf("  End with semicolon to complete input\n");
    printf("\n");
    printf("Examples:\n");
    printf("  let x = 42;\n");
    printf("  :types             # Show all variable types\n");
    printf("  :inspect x         # Inspect variable x\n");
    printf("  :memory            # Show memory usage\n");
    printf("  func add(a, b):\n");
    printf("  ...   return a + b;\n");
    printf("  ... end\n");
    printf("  print(add(5, 3));\n");
}

/**
 * @brief Show all variables
 * 
 * @param state The REPL state
 */
void repl_show_variables(REPLState* state) {
    if (!state || !state->interpreter) {
        return;
    }
    
    printf("Variables:\n");
    
    // Get the current environment
    Environment* env = state->interpreter->current_environment;
    if (!env) {
        printf("  (No variables defined)\n");
        return;
    }
    
    int has_variables = 0;
    while (env) {
        for (size_t i = 0; i < env->count; i++) {
            if (env->names[i] && env->values[i].type != VALUE_NULL) {
                printf("  %s = ", env->names[i]);
                value_print(&env->values[i]);
                printf(" (%s)\n", value_type_to_string(env->values[i].type));
                has_variables = 1;
            }
        }
        env = env->parent;
    }
    
    if (!has_variables) {
        printf("  (No variables defined)\n");
    }
}

/**
 * @brief Clear all variables
 * 
 * @param state The REPL state
 */
void repl_clear_variables(REPLState* state) {
    if (!state || !state->interpreter) {
        return;
    }
    
    // Clear the current environment by creating a new one
    Environment* new_env = environment_create(NULL);
    if (new_env) {
        environment_free(state->interpreter->current_environment);
        state->interpreter->current_environment = new_env;
        printf("Variables cleared.\n");
    } else {
        printf("Error: Failed to clear variables.\n");
    }
}

/**
 * @brief Reset REPL state
 * 
 * @param state The REPL state
 */
void repl_reset_state(REPLState* state) {
    if (!state) {
        return;
    }
    
    // Reset line number
    state->line_number = 1;
    
    // Reset debug modes
    state->debug_mode = 0;
    state->debug_ast = 0;
    state->debug_lexer = 0;
    state->debug_parser = 0;
    
    // Clear current input
    if (state->current_input) {
        shared_free_safe(state->current_input, "repl", "unknown_function", 506);
        state->current_input = NULL;
        state->input_length = 0;
        state->input_capacity = 0;
    }
    
    // Reset interpreter
    if (state->interpreter) {
        interpreter_reset(state->interpreter);
    }
}

/**
 * @brief Load and execute a file
 * 
 * @param state The REPL state
 * @param filename The file to load
 */
void repl_load_file(REPLState* state, const char* filename) {
    if (!state || !filename) {
        return;
    }
    
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: Cannot open file '%s': %s\n", filename, strerror(errno));
        return;
    }
    
    // Read file content
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* source = shared_malloc_safe(file_size + 1, "repl", "unknown_function", 540);
    if (!source) {
        fclose(file);
        printf("Error: Out of memory\n");
        return;
    }
    
    size_t bytes_read = fread(source, 1, file_size, file);
    source[bytes_read] = '\0';
    fclose(file);
    
    printf("Loading file: %s\n", filename);
    repl_execute_input(state, source);
    
    shared_free_safe(source, "repl", "unknown_function", 554);
}

/**
 * @brief Set debug mode
 * 
 * @param state The REPL state
 * @param mode The debug mode string
 */
void repl_set_debug_mode(REPLState* state, const char* mode) {
    if (!state || !mode) {
        return;
    }
    
    if (strcmp(mode, "--off") == 0) {
        state->debug_mode = 0;
        state->debug_ast = 0;
        state->debug_lexer = 0;
        state->debug_parser = 0;
        printf("Debug mode disabled.\n");
    } else if (strcmp(mode, "--all") == 0) {
        state->debug_mode = 1;
        state->debug_ast = 1;
        state->debug_lexer = 1;
        state->debug_parser = 1;
        printf("Debug mode enabled (all).\n");
    } else if (strcmp(mode, "--ast") == 0) {
        state->debug_mode = 1;
        state->debug_ast = 1;
        printf("Debug mode enabled (AST).\n");
    } else if (strcmp(mode, "--lexer") == 0) {
        state->debug_mode = 1;
        state->debug_lexer = 1;
        printf("Debug mode enabled (lexer).\n");
    } else if (strcmp(mode, "--parser") == 0) {
        state->debug_mode = 1;
        state->debug_parser = 1;
        printf("Debug mode enabled (parser).\n");
    } else {
        printf("Unknown debug mode: %s\n", mode);
        printf("Available modes: --ast, --lexer, --parser, --all, --off\n");
    }
}

/**
 * @brief Show debug information
 * 
 * @param state The REPL state
 * @param input The input string
 */
void repl_show_debug_info(REPLState* state, const char* input) {
    if (!state || !input) {
        return;
    }
    
    // printf("DEBUG: Processing input (line %d)\n", state->line_number);
    // printf("DEBUG: Input: %s\n", input);
}

/**
 * @brief Add input to history
 * 
 * @param state The REPL state
 * @param input The input string
 */
void repl_add_to_history(REPLState* state, const char* input) {
    if (!state || !input) {
        return;
    }
    
    // Expand history if needed
    if (state->history_count >= state->history_capacity) {
        state->history_capacity = state->history_capacity == 0 ? 100 : state->history_capacity * 2;
        state->history = shared_realloc_safe(state->history, state->history_capacity * sizeof(char*), "repl", "unknown_function", 627);
    }
    
    // Add to history
    state->history[state->history_count] = (input ? strdup(input) : NULL);
    state->history_count++;
    state->history_index = state->history_count;
}

/**
 * @brief Read a line of input
 * 
 * @param state The REPL state
 * @return The input string, or NULL on EOF/error
 */
char* repl_read_line(REPLState* state) {
    if (!state) {
        return NULL;
    }
    
    char* input = readline("myco> ");
    if (input && strlen(input) > 0) {
        add_history(input);
        return input;
    }
    
    // Fallback to standard input if readline fails
    if (!input) {
        printf("myco> ");
        fflush(stdout);
        
        char buffer[1024];
        if (fgets(buffer, sizeof(buffer), stdin)) {
            // Remove newline
            size_t len = strlen(buffer);
            if (len > 0 && buffer[len-1] == '\n') {
                buffer[len-1] = '\0';
            }
            
            if (strlen(buffer) > 0) {
                input = shared_malloc_safe(strlen(buffer) + 1, "repl", "unknown_function", 667);
                strcpy(input, buffer);
                add_history(input);
            }
        }
    }
    
    return input;
}

/**
 * @brief Check if input is a multiline continuation
 * 
 * @param input The input string
 * @return 1 if continuation, 0 otherwise
 */
int repl_is_multiline_continuation(const char* input) {
    if (!input) {
        return 0;
    }
    
    // Check for common multiline patterns
    const char* trimmed = input;
    while (*trimmed == ' ' || *trimmed == '\t') {
        trimmed++;
    }
    
    // Check if line starts with '...' (continuation prompt)
    if (strncmp(trimmed, "...", 3) == 0) {
        return 1;
    }
    
    // Check for incomplete statements
    // This is a simplified check - in practice, you'd want more sophisticated parsing
    size_t len = strlen(trimmed);
    if (len > 0) {
        char last_char = trimmed[len - 1];
        // If line doesn't end with semicolon and isn't a complete block, it's likely incomplete
        if (last_char != ';' && last_char != '}' && last_char != 'e') {
            // Check for common incomplete patterns
            if (strstr(trimmed, "func ") || strstr(trimmed, "if ") || 
                strstr(trimmed, "while ") || strstr(trimmed, "for ")) {
                return 1;
            }
        }
    }
    
    return 0;
}

/**
 * @brief Check if input is complete
 * 
 * @param input The input string
 * @return 1 if complete, 0 otherwise
 */
int repl_is_input_complete(const char* input) {
    if (!input) {
        return 0;
    }
    
    // Check for complete blocks (ends with 'end')
    const char* trimmed = input;
    while (*trimmed == ' ' || *trimmed == '\t' || *trimmed == '\n') {
        trimmed++;
    }
    
    // Find the last non-whitespace character
    const char* last_non_ws = trimmed;
    for (const char* p = trimmed; *p; p++) {
        if (*p != ' ' && *p != '\t' && *p != '\n' && *p != '\r') {
            last_non_ws = p;
        }
    }
    
    // Check if it ends with 'end'
    if (last_non_ws >= trimmed + 2) {
        const char* end_start = last_non_ws - 2;
        if (strncmp(end_start, "end", 3) == 0) {
            return 1;
        }
    }
    
    // Check if it's a simple statement ending with semicolon
    // but only if it doesn't contain block keywords
    if (strstr(input, "func ") == NULL && strstr(input, "if ") == NULL && 
        strstr(input, "while ") == NULL && strstr(input, "for ") == NULL &&
        strstr(input, "class ") == NULL && strstr(input, "try ") == NULL) {
        size_t len = strlen(input);
        if (len > 0 && input[len - 1] == ';') {
            return 1;
        }
    }
    
    return 0;
}

/**
 * @brief Print evaluation result
 * 
 * @param state The REPL state
 * @param result The result value
 */
void repl_print_result(REPLState* state, Value* result) {
    if (!state || !result) {
        return;
    }
    
    // Don't print null results
    if (result->type == VALUE_NULL) {
        return;
    }
    
    // Print the result
    value_print(result);
    printf("\n");
}

/**
 * @brief Print error message
 * 
 * @param state The REPL state
 * @param error The error message
 * @param line The line number
 */
void repl_print_error(REPLState* state, const char* error, int line) {
    if (!state || !error) {
        return;
    }
    
    printf("\033[31mREPL Error at line %d: %s\033[0m\n", line, error);
}

/**
 * @brief Show error suggestions
 * 
 * @param error The error message
 * @param line The line number
 */
void repl_show_error_suggestion(const char* error, int line) {
    if (!error) {
        return;
    }
    
    // Simple error suggestions
    if (strstr(error, "Expected Identifier")) {
        printf("Suggestion: Check for typos in variable names or missing declarations\n");
    } else if (strstr(error, "Expected a primary expression")) {
        printf("Suggestion: Check for missing operands or incorrect syntax\n");
    } else if (strstr(error, "Expected a statement")) {
        printf("Suggestion: Check for missing semicolons or incorrect statement syntax\n");
    }
}

/**
 * @brief Print debug AST information
 * 
 * @param state The REPL state
 * @param node The AST node
 */
void repl_print_debug_ast(REPLState* state, ASTNode* node) {
    if (!state || !node) {
        return;
    }
    
    // printf("DEBUG AST:\n");
    ast_print_tree(node);
    printf("\n");
}

/**
 * @brief Print debug lexer information
 * 
 * @param state The REPL state
 * @param lexer The lexer
 */
void repl_print_debug_lexer(REPLState* state, Lexer* lexer) {
    if (!state || !lexer) {
        return;
    }
    
    // printf("DEBUG LEXER: %d tokens\n", lexer->token_count);
    for (int i = 0; i < lexer->token_count; i++) {
        Token* token = lexer_get_token(lexer, i);
        if (token) {
            printf("  Token %d: Type=%d, Text='%s', Line=%d, Column=%d\n", 
                   i, token->type, token->text ? token->text : "NULL", 
                   token->line, token->column);
        }
    }
    printf("\n");
}

/**
 * @brief Print debug parser information
 * 
 * @param state The REPL state
 * @param parser The parser
 */
void repl_print_debug_parser(REPLState* state, Parser* parser) {
    if (!state || !parser) {
        return;
    }
    
    // printf("DEBUG PARSER: Error count=%d\n", parser->error_count);
    if (parser->error_message) {
        printf("  Error: %s\n", parser->error_message);
    }
    printf("\n");
}

// Enhanced debugging functions

/**
 * @brief Show variable types with detailed information
 */
void repl_show_variable_types(REPLState* state) {
    if (!state || !state->interpreter) {
        printf("No interpreter available.\n");
        return;
    }
    
    Environment* env = state->interpreter->global_environment;
    if (!env || env->count == 0) {
        printf("No variables defined.\n");
        return;
    }
    
    printf("Variable Types:\n");
    printf("┌─────────────┬─────────────┬─────────────────────────────────────┐\n");
    printf("│ Name        │ Type        │ Value                               │\n");
    printf("├─────────────┼─────────────┼─────────────────────────────────────┤\n");
    
    for (size_t i = 0; i < env->count; i++) {
        const char* name = env->names[i];
        Value* value = &env->values[i];
        
        // Get type name
        const char* type_name = "Unknown";
        switch (value->type) {
            case VALUE_NUMBER:
                type_name = "Number";
                break;
            case VALUE_STRING:
                type_name = "String";
                break;
            case VALUE_BOOLEAN:
                type_name = "Boolean";
                break;
            case VALUE_ARRAY:
                type_name = "Array";
                break;
            case VALUE_OBJECT:
                type_name = "Object";
                break;
            case VALUE_FUNCTION:
                type_name = "Function";
                break;
            case VALUE_NULL:
                type_name = "Null";
                break;
            default:
                type_name = "Unknown";
                break;
        }
        
        // Get value preview
        char value_preview[32] = {0};
        switch (value->type) {
            case VALUE_NUMBER:
                snprintf(value_preview, sizeof(value_preview), "%.2f", value->data.number_value);
                break;
            case VALUE_STRING:
                snprintf(value_preview, sizeof(value_preview), "\"%s\"", 
                        value->data.string_value ? value->data.string_value : "NULL");
                break;
            case VALUE_BOOLEAN:
                snprintf(value_preview, sizeof(value_preview), "%s", 
                        value->data.boolean_value ? "true" : "false");
                break;
            case VALUE_ARRAY:
                snprintf(value_preview, sizeof(value_preview), "[%zu items]", 
                        value->data.array_value.count);
                break;
            case VALUE_OBJECT:
                snprintf(value_preview, sizeof(value_preview), "{object}");
                break;
            case VALUE_FUNCTION:
                snprintf(value_preview, sizeof(value_preview), "{function}");
                break;
            case VALUE_NULL:
                snprintf(value_preview, sizeof(value_preview), "null");
                break;
            default:
                snprintf(value_preview, sizeof(value_preview), "unknown");
                break;
        }
        
        printf("│ %-11s │ %-11s │ %-35s │\n", name, type_name, value_preview);
    }
    
    printf("└─────────────┴─────────────┴─────────────────────────────────────┘\n");
}

/**
 * @brief Show memory usage statistics
 */
void repl_show_memory_usage(REPLState* state) {
    if (!state || !state->interpreter) {
        printf("No interpreter available.\n");
        return;
    }
    
    printf("Memory Usage:\n");
    printf("┌─────────────────────┬─────────────────────┐\n");
    printf("│ Metric              │ Value               │\n");
    printf("├─────────────────────┼─────────────────────┤\n");
    
    // Get basic memory stats
    size_t total_variables = 0;
    size_t total_strings = 0;
    size_t total_arrays = 0;
    
    Environment* env = state->interpreter->global_environment;
    if (env) {
        total_variables = env->count;
        for (size_t i = 0; i < env->count; i++) {
            Value* value = &env->values[i];
            if (value->type == VALUE_STRING && value->data.string_value) {
                total_strings++;
            } else if (value->type == VALUE_ARRAY) {
                total_arrays++;
            }
        }
    }
    
    printf("│ Variables           │ %-19zu │\n", total_variables);
    printf("│ String Objects      │ %-19zu │\n", total_strings);
    printf("│ Array Objects       │ %-19zu │\n", total_arrays);
    printf("│ Call Stack Depth    │ %-19d │\n", state->interpreter->stack_depth);
    printf("│ Max Stack Depth     │ %-19d │\n", state->interpreter->max_stack_depth);
    printf("└─────────────────────┴─────────────────────┘\n");
}

/**
 * @brief Show call trace information
 */
void repl_show_call_trace(REPLState* state) {
    if (!state || !state->interpreter) {
        printf("No interpreter available.\n");
        return;
    }
    
    printf("Call Trace:\n");
    if (state->interpreter->call_stack) {
        CallFrame* frame = state->interpreter->call_stack;
        int frame_num = 0;
        while (frame) {
            printf("  #%d %s in %s at line %d\n", 
                   frame_num,
                   frame->function_name ? frame->function_name : "unknown",
                   frame->file_name ? frame->file_name : "unknown",
                   frame->line);
            frame = frame->next;
            frame_num++;
        }
    } else {
        printf("  No active call stack.\n");
    }
}

/**
 * @brief Inspect a specific variable in detail
 */
void repl_inspect_variable(REPLState* state, const char* var_name) {
    if (!state || !state->interpreter || !var_name) {
        printf("Invalid parameters.\n");
        return;
    }
    
    Environment* env = state->interpreter->global_environment;
    if (!env) {
        printf("No environment available.\n");
        return;
    }
    
    // Find the variable
    Value* value = NULL;
    for (size_t i = 0; i < env->count; i++) {
        if (strcmp(env->names[i], var_name) == 0) {
            value = &env->values[i];
            break;
        }
    }
    
    if (!value) {
        printf("Variable '%s' not found.\n", var_name);
        return;
    }
    
    printf("Variable Inspection: %s\n", var_name);
    printf("┌─────────────────────┬─────────────────────────────────────────┐\n");
    printf("│ Property            │ Value                                   │\n");
    printf("├─────────────────────┼─────────────────────────────────────────┤\n");
    
    // Type information
    const char* type_name = "Unknown";
    switch (value->type) {
        case VALUE_NUMBER: type_name = "Number"; break;
        case VALUE_STRING: type_name = "String"; break;
        case VALUE_BOOLEAN: type_name = "Boolean"; break;
        case VALUE_ARRAY: type_name = "Array"; break;
        case VALUE_OBJECT: type_name = "Object"; break;
        case VALUE_FUNCTION: type_name = "Function"; break;
        case VALUE_NULL: type_name = "Null"; break;
        default: type_name = "Unknown"; break;
    }
    
    printf("│ Type                │ %-39s │\n", type_name);
    
    // Value information
    char value_str[64] = {0};
    switch (value->type) {
        case VALUE_NUMBER:
            snprintf(value_str, sizeof(value_str), "%.15g", value->data.number_value);
            break;
        case VALUE_STRING:
            snprintf(value_str, sizeof(value_str), "\"%s\"", 
                    value->data.string_value ? value->data.string_value : "NULL");
            break;
        case VALUE_BOOLEAN:
            snprintf(value_str, sizeof(value_str), "%s", 
                    value->data.boolean_value ? "true" : "false");
            break;
        case VALUE_ARRAY:
            snprintf(value_str, sizeof(value_str), "[%zu items]", 
                    value->data.array_value.count);
            break;
        case VALUE_OBJECT:
            snprintf(value_str, sizeof(value_str), "{object}");
            break;
        case VALUE_FUNCTION:
            snprintf(value_str, sizeof(value_str), "{function}");
            break;
        case VALUE_NULL:
            snprintf(value_str, sizeof(value_str), "null");
            break;
        default:
            snprintf(value_str, sizeof(value_str), "unknown");
            break;
    }
    
    printf("│ Value               │ %-39s │\n", value_str);
    printf("└─────────────────────┴─────────────────────────────────────────┘\n");
}

/**
 * @brief Show breakpoints (placeholder for future implementation)
 */
void repl_show_breakpoints(REPLState* state) {
    (void)state; // Suppress unused parameter warning
    printf("Breakpoints: (Not yet implemented)\n");
    printf("  No breakpoints set.\n");
}

/**
 * @brief Set a breakpoint (placeholder for future implementation)
 */
void repl_set_breakpoint(REPLState* state, const char* location) {
    (void)state; // Suppress unused parameter warning
    printf("Breakpoint at '%s': (Not yet implemented)\n", location);
}

/**
 * @brief Remove a breakpoint (placeholder for future implementation)
 */
void repl_remove_breakpoint(REPLState* state, const char* location) {
    (void)state; // Suppress unused parameter warning
    printf("Remove breakpoint at '%s': (Not yet implemented)\n", location);
}

/**
 * @brief Step execution (placeholder for future implementation)
 */
void repl_step_execution(REPLState* state) {
    (void)state; // Suppress unused parameter warning
    printf("Step execution: (Not yet implemented)\n");
}

/**
 * @brief Continue execution (placeholder for future implementation)
 */
void repl_continue_execution(REPLState* state) {
    (void)state; // Suppress unused parameter warning
    printf("Continue execution: (Not yet implemented)\n");
}

/**
 * @brief Show performance profile
 */
void repl_show_performance_profile(REPLState* state) {
    if (!state || !state->interpreter) {
        printf("No interpreter available.\n");
        return;
    }
    
    printf("Performance Profile:\n");
    printf("┌─────────────────────┬─────────────────────┐\n");
    printf("│ Metric              │ Value               │\n");
    printf("├─────────────────────┼─────────────────────┤\n");
    printf("│ Execution Time      │ %-19s │\n", "N/A");
    printf("│ Memory Allocations  │ %-19s │\n", "N/A");
    printf("│ Function Calls      │ %-19s │\n", "N/A");
    printf("│ Parse Time          │ %-19s │\n", "N/A");
    printf("│ Lex Time            │ %-19s │\n", "N/A");
    printf("└─────────────────────┴─────────────────────┘\n");
    printf("Note: Performance profiling not yet implemented.\n");
}
