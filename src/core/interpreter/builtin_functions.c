#include "interpreter/value_operations.h"
#include "../../include/core/interpreter.h"
#include "../../include/utils/shared_utilities.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// ============================================================================
// BUILT-IN FUNCTIONS
// ============================================================================

Value builtin_print(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    (void)interpreter; // Suppress unused parameter warning
    (void)line; // Suppress unused parameter warning
    (void)column; // Suppress unused parameter warning
    
    if (arg_count == 0) {
        printf("\n");
        fflush(stdout);
        return value_create_null();
    }
    
    // Check if first argument is a format string (contains %)
    Value first_arg = args[0];
    if (first_arg.type == VALUE_STRING && first_arg.data.string_value) {
        const char* format_str = first_arg.data.string_value;
        if (strstr(format_str, "%") != NULL) {
            // Format string detected - use printf-style formatting
            if (arg_count == 1) {
                // Just the format string, no arguments
                printf("%s\n", format_str);
            } else {
                // Format string with arguments
                // For now, support basic %s, %d, %f formatting
                // This is a simplified implementation
                const char* current = format_str;
                size_t arg_index = 1;
                
                while (*current && arg_index < arg_count) {
                    if (*current == '%' && *(current + 1) != '\0') {
                        current++; // Skip the %
                        switch (*current) {
                            case 's': {
                                // String formatting
                                if (arg_index < arg_count) {
                                    Value s = value_to_string(&args[arg_index]);
                                    if (s.type == VALUE_STRING && s.data.string_value) {
                                        printf("%s", s.data.string_value);
                                    }
                                    value_free(&s);
                                    arg_index++;
                                }
                                break;
                            }
                            case 'd': {
                                // Integer formatting
                                if (arg_index < arg_count) {
                                    Value v = args[arg_index];
                                    if (v.type == VALUE_NUMBER) {
                                        printf("%.0f", v.data.number_value);
                                    } else {
                                        printf("0");
                                    }
                                    arg_index++;
                                }
                                break;
                            }
                            case 'f': {
                                // Float formatting
                                if (arg_index < arg_count) {
                                    Value v = args[arg_index];
                                    if (v.type == VALUE_NUMBER) {
                                        printf("%f", v.data.number_value);
                                    } else {
                                        printf("0.0");
                                    }
                                    arg_index++;
                                }
                                break;
                            }
                            default:
                                // Unknown format specifier, print as-is
                                printf("%c", *current);
                                break;
                        }
                    } else {
                        printf("%c", *current);
                    }
                    current++;
                }
                printf("\n");
            }
        } else {
            // No format string, treat as regular print
            for (size_t i = 0; i < arg_count; i++) {
                Value s = value_to_string(&args[i]);
                if (s.type == VALUE_STRING && s.data.string_value) {
                    printf("%s", s.data.string_value);
                }
                value_free(&s);
                if (i + 1 < arg_count) printf(" ");
            }
            printf("\n");
        }
    } else {
        // First argument is not a string, treat as regular print
        for (size_t i = 0; i < arg_count; i++) {
            Value s = value_to_string(&args[i]);
            if (s.type == VALUE_STRING && s.data.string_value) {
                printf("%s", s.data.string_value);
            }
            value_free(&s);
            if (i + 1 < arg_count) printf(" ");
        }
        printf("\n");
    }
    
    fflush(stdout);
    return value_create_null();
}

Value builtin_input(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    (void)interpreter; // Suppress unused parameter warning
    (void)line; // Suppress unused parameter warning
    (void)column; // Suppress unused parameter warning
    
    // Print prompt if provided
    if (arg_count > 0) {
        Value prompt = args[0];
        if (prompt.type == VALUE_STRING) {
            printf("%s", prompt.data.string_value);
            fflush(stdout);
        }
    }
    
    // Read input from stdin
    char* buffer = NULL;
    size_t buffer_size = 0;
    ssize_t bytes_read = getline(&buffer, &buffer_size, stdin);
    
    if (bytes_read == -1) {
        // Error reading input or EOF
        if (buffer) {
            shared_free_safe(buffer, "interpreter", "unknown_function", 5470);
        }
        return value_create_string("");
    }
    
    // Remove newline character if present
    if (bytes_read > 0 && buffer[bytes_read - 1] == '\n') {
        buffer[bytes_read - 1] = '\0';
    }
    
    Value result = value_create_string(buffer);
    shared_free_safe(buffer, "interpreter", "unknown_function", 5481);
    return result;
}

Value builtin_len(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    (void)interpreter; // Suppress unused parameter warning
    (void)line; // Suppress unused parameter warning
    (void)column; // Suppress unused parameter warning
    
    if (arg_count < 1) {
        return value_create_number(0);
    }
    
    Value* arg = &args[0];
    switch (arg->type) {
        case VALUE_STRING:
            if (arg->data.string_value) {
                return value_create_number((double)strlen(arg->data.string_value));
            } else {
                return value_create_number(0);
            }
        case VALUE_ARRAY:
            return value_create_number((double)arg->data.array_value.count);
        case VALUE_OBJECT:
            return value_create_number((double)arg->data.object_value.count);
        default:
            return value_create_number(0);
    }
}

Value builtin_assert(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 2) {
        fprintf(stderr, "Assertion failed: assert() requires at least 2 arguments (condition, message)\n");
        return value_create_null();
    }
    
    // First argument should be the condition (boolean)
    Value condition = args[0];
    if (!value_is_truthy(&condition)) {
        // Second argument should be the error message
        Value message = args[1];
        if (message.type == VALUE_STRING && message.data.string_value) {
            fprintf(stderr, "Assertion failed: %s\n", message.data.string_value);
        } else {
            fprintf(stderr, "Assertion failed: condition is false\n");
        }
        // In a real implementation, this might exit the program
        // For now, just print the error and continue
    }
    
    return value_create_null();
}

// Placeholder functions for future implementation
Value builtin_int(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) { 
    (void)interpreter; (void)args; (void)arg_count; (void)line; (void)column;
    Value v = {0}; 
    return v; 
}

Value builtin_float(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) { 
    (void)interpreter; (void)args; (void)arg_count; (void)line; (void)column;
    Value v = {0}; 
    return v; 
}

Value builtin_bool(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) { 
    (void)interpreter; (void)args; (void)arg_count; (void)line; (void)column;
    Value v = {0}; 
    return v; 
}

// ============================================================================
// BUILT-IN REGISTRATION
// ============================================================================

void interpreter_register_builtins(Interpreter* interpreter) {
    if (!interpreter || !interpreter->global_environment) return;
    // Register builtin function symbols in the global environment so they are visible and can be shadowed later.
    // Calls are still dispatched by name in eval_node, but having these in the environment
    // helps introspection and keeps a single registration point for builtins.
    Value v = value_create_string("<builtin>");
    environment_define(interpreter->global_environment, "print", v);
    environment_define(interpreter->global_environment, "uprint", v);
    environment_define(interpreter->global_environment, "isString", v);
    environment_define(interpreter->global_environment, "isInt", v);
    environment_define(interpreter->global_environment, "isFloat", v);
    environment_define(interpreter->global_environment, "isBool", v);
    environment_define(interpreter->global_environment, "isArray", v);
    environment_define(interpreter->global_environment, "isNull", v);
    environment_define(interpreter->global_environment, "isNumber", v);
    environment_define(interpreter->global_environment, "assert", v);
    value_free(&v);
    
    // Register built-in libraries
    extern void register_all_builtin_libraries(Interpreter* interpreter);
    register_all_builtin_libraries(interpreter);
}
