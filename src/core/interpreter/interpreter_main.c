#include "interpreter.h"
#include "environment.h"
#include "enhanced_error_system.h"
#include "debug_system.h"
#include "repl_debug.h"
#include "macros.h"
// #include "compile_time.h" // Temporarily disabled due to type system issues
#include "shared_utilities.h"
#include "libs/array.h"
#include "interpreter/value_operations.h"
#include "interpreter/method_handlers.h"
#include "interpreter/eval_engine.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdint.h>

// Forward declarations for pattern matching functions
static int pattern_matches(Interpreter* interpreter, Value* value, ASTNode* pattern);
static int pattern_matches_type(Value* value, const char* type_name);
static int pattern_matches_destructure(Interpreter* interpreter, Value* value, ASTNode* pattern);
static int pattern_matches_guard(Interpreter* interpreter, Value* value, ASTNode* pattern);
static int pattern_matches_or(Interpreter* interpreter, Value* value, ASTNode* pattern);
static int pattern_matches_and(Interpreter* interpreter, Value* value, ASTNode* pattern);
static int pattern_matches_range(Interpreter* interpreter, Value* value, ASTNode* pattern);
static int pattern_matches_regex(Interpreter* interpreter, Value* value, ASTNode* pattern);

// Global error and debug systems
EnhancedErrorSystem* global_error_system = NULL;
static DebugSystem* global_debug_system = NULL;
static ReplDebugSession* global_repl_session = NULL;

// ANSI color codes for terminal output
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#include "libs/math.h"
#include "libs/array.h"
#include "libs/string.h"
#include "libs/sets.h"
#include "libs/maps.h"
#include "libs/trees.h"
#include "libs/graphs.h"
#include "libs/heaps.h"
#include "libs/queues.h"
#include "libs/stacks.h"
#include "libs/server/server.h"
#include "libs/builtin_libs.h"

// Placeholder interpreter implementation
// This will be replaced with the full implementation

Interpreter* interpreter_create(void) {
    Interpreter* interpreter = shared_malloc_safe(sizeof(Interpreter), "interpreter", "unknown_function", 28);
    if (!interpreter) return NULL;
    
    // Initialize global systems if not already done
    if (!global_error_system) {
        global_error_system = enhanced_error_system_create();
    }
    if (!global_debug_system) {
        global_debug_system = debug_system_create();
    }
    if (!global_repl_session) {
        global_repl_session = repl_debug_session_create();
    }
    
    interpreter->global_environment = NULL;
    interpreter->current_environment = NULL;
    interpreter->has_return = 0;
    interpreter->has_error = 0;
    interpreter->error_message = NULL;
    interpreter->error_line = 0;
    interpreter->error_column = 0;
    interpreter->break_depth = 0;
    interpreter->continue_depth = 0;
    interpreter->try_depth = 0;
    interpreter->current_function_return_type = NULL;
    interpreter->self_context = NULL;
    
    // Enhanced error handling initialization
    interpreter->call_stack = NULL;
    interpreter->stack_depth = 0;
    interpreter->max_stack_depth = 1000;  // Reasonable limit
    interpreter->recursion_count = 0;
    interpreter->max_recursion_depth = 100;  // Reasonable limit
    
    // JIT compilation initialization
    interpreter->jit_context = NULL;
    interpreter->jit_enabled = 0;
    interpreter->jit_mode = 0;
    
    // Benchmark timing initialization
    interpreter->benchmark_mode = 0;
    interpreter->execution_time_ns = 0;
    interpreter->start_time_ns = 0;
    
    // Hot spot tracking initialization
    interpreter->hot_spot_tracker = NULL;
    
    // Micro-JIT compiler initialization
    interpreter->micro_jit_context = NULL;
    
    // Macro system initialization
    interpreter->macro_expander = macro_expander_create();
    
    // Compile-time evaluation initialization
    // interpreter->compile_time_evaluator = compile_time_evaluator_create(); // Temporarily disabled
    
    // Setup global env
    interpreter->global_environment = environment_create(NULL);
    interpreter->current_environment = interpreter->global_environment;
    
    // Register all built-in libraries
    register_all_builtin_libraries(interpreter);
    
    return interpreter;
}

void interpreter_free(Interpreter* interpreter) {
    if (interpreter) {
        if (interpreter->error_message) {
            shared_free_safe(interpreter->error_message, "interpreter", "unknown_function", 66);
        }
        
        // Clean up global systems if this is the last interpreter
        // Note: In a real implementation, you'd want reference counting
        // For now, we'll keep the global systems alive
        
        // Clean up call stack
        CallFrame* frame = interpreter->call_stack;
        while (frame) {
            CallFrame* next = frame->next;
            shared_free_safe((void*)frame->function_name, "interpreter", "unknown_function", 73);
            shared_free_safe((void*)frame->file_name, "interpreter", "unknown_function", 74);
            shared_free_safe(frame, "interpreter", "unknown_function", 75);
            frame = next;
        }
        
        // Clean up JIT context
        if (interpreter->jit_context) {
            jit_context_free(interpreter->jit_context);
        }
        
        // Clean up macro expander
        if (interpreter->macro_expander) {
            macro_expander_free(interpreter->macro_expander);
        }
        
        // Clean up compile-time evaluator
        // if (interpreter->compile_time_evaluator) {
        //     compile_time_evaluator_free(interpreter->compile_time_evaluator);
        // }
        
        if (interpreter->global_environment) {
            environment_free(interpreter->global_environment);
        }
        if (interpreter->current_environment && interpreter->current_environment != interpreter->global_environment) {
            environment_free(interpreter->current_environment);
        }
        shared_free_safe(interpreter, "interpreter", "unknown_function", 90);
    }
}

void interpreter_reset(Interpreter* interpreter) {
    if (interpreter) {
        interpreter->has_return = 0;
        interpreter->has_error = 0;
        if (interpreter->error_message) {
            shared_free_safe(interpreter->error_message, "interpreter", "unknown_function", 99);
            interpreter->error_message = NULL;
        }
        interpreter->error_line = 0;
        interpreter->error_column = 0;
    }
}

// eval_node is declared in eval_engine.h

// Helper function to handle super method calls

// Helper function to handle server method calls

// Helper function to handle request method calls

// Helper function to handle response method calls

// Helper function to handle route group method calls

// Helper function to handle server library method calls

// Helper function to handle method calls

Value value_create_error(const char* message, int code) { Value v = {0}; return v; }

// eval_node and interpreter_execute are now defined in eval_core.c
Value interpreter_execute_program(Interpreter* interpreter, ASTNode* node) {
    if (!node) return value_create_null();
    
    // Clear any previous errors before starting execution
    if (interpreter) {
        interpreter_clear_error(interpreter);
    }
    
    if (node->type == AST_NODE_BLOCK) {
        for (size_t i = 0; i < node->data.block.statement_count; i++) {
            // Stop execution if there's an error (like Python)
            eval_node(interpreter, node->data.block.statements[i]);
            if (interpreter_has_error(interpreter)) {
                return value_create_null();
            }
        }
        return value_create_null();
    }
    return eval_node(interpreter, node);
}
Value interpreter_execute_statement(Interpreter* interpreter, ASTNode* node) { Value v = {0}; return v; }
Value interpreter_execute_expression(Interpreter* interpreter, ASTNode* node) { Value v = {0}; return v; }
Value interpreter_execute_binary_op(Interpreter* interpreter, ASTNode* node) { Value v = {0}; return v; }
Value interpreter_execute_unary_op(Interpreter* interpreter, ASTNode* node) { Value v = {0}; return v; }
Value interpreter_execute_assignment(Interpreter* interpreter, ASTNode* node) { Value v = {0}; return v; }
Value interpreter_execute_function_call(Interpreter* interpreter, ASTNode* node) { Value v = {0}; return v; }
Value interpreter_execute_variable_declaration(Interpreter* interpreter, ASTNode* node) { Value v = {0}; return v; }
Value interpreter_execute_if_statement(Interpreter* interpreter, ASTNode* node) { Value v = {0}; return v; }
Value interpreter_execute_while_loop(Interpreter* interpreter, ASTNode* node) { Value v = {0}; return v; }
Value interpreter_execute_for_loop(Interpreter* interpreter, ASTNode* node) { Value v = {0}; return v; }
Value interpreter_execute_try_catch(Interpreter* interpreter, ASTNode* node) { Value v = {0}; return v; }
Value interpreter_execute_switch(Interpreter* interpreter, ASTNode* node) { Value v = {0}; return v; }
Value interpreter_execute_match(Interpreter* interpreter, ASTNode* node) { Value v = {0}; return v; }
Value interpreter_execute_block(Interpreter* interpreter, ASTNode* node) { Value v = {0}; return v; }
Value interpreter_execute_return(Interpreter* interpreter, ASTNode* node) { Value v = {0}; return v; }
Value interpreter_execute_break(Interpreter* interpreter, ASTNode* node) { Value v = {0}; return v; }
Value interpreter_execute_continue(Interpreter* interpreter, ASTNode* node) { Value v = {0}; return v; }
Value interpreter_execute_throw(Interpreter* interpreter, ASTNode* node) { Value v = {0}; return v; }
Value interpreter_execute_function_declaration(Interpreter* interpreter, ASTNode* node) { Value v = {0}; return v; }
Value interpreter_execute_class_declaration(Interpreter* interpreter, ASTNode* node) { Value v = {0}; return v; }
Value interpreter_execute_import(Interpreter* interpreter, ASTNode* node) { Value v = {0}; return v; }
Value interpreter_execute_module(Interpreter* interpreter, ASTNode* node) { Value v = {0}; return v; }
Value interpreter_execute_package(Interpreter* interpreter, ASTNode* node) { Value v = {0}; return v; }

void interpreter_set_return(Interpreter* interpreter, int has_return) {}

// Get error code from message with comprehensive pattern matching
static MycoErrorCode get_error_code(const char* message) {
    if (!message) return MYCO_ERROR_INTERNAL_ERROR;
    
    // Runtime Errors (1000-1999)
    if (strstr(message, "Division by zero")) return MYCO_ERROR_DIVISION_BY_ZERO;
    if (strstr(message, "Undefined variable")) return MYCO_ERROR_UNDEFINED_VARIABLE;
    if (strstr(message, "Array index out of bounds")) return MYCO_ERROR_ARRAY_BOUNDS;
    if (strstr(message, "Cannot index non-array value")) return MYCO_ERROR_INVALID_OPERATION;
    if (strstr(message, "Array index must be a number")) return MYCO_ERROR_TYPE_MISMATCH;
    if (strstr(message, "String index out of bounds")) return MYCO_ERROR_ARRAY_BOUNDS;
    if (strstr(message, "Cannot index non-string value")) return MYCO_ERROR_INVALID_OPERATION;
    if (strstr(message, "String index must be a number")) return MYCO_ERROR_TYPE_MISMATCH;
    if (strstr(message, "Member access") && strstr(message, "non-object")) return MYCO_ERROR_INVALID_OPERATION;
    if (strstr(message, "Cannot call non-function")) return MYCO_ERROR_INVALID_FUNCTION;
    if (strstr(message, "Undefined function")) return MYCO_ERROR_UNDEFINED_FUNCTION;
    if (strstr(message, "requires exactly") || strstr(message, "too many arguments")) return MYCO_ERROR_ARGUMENT_COUNT;
    if (strstr(message, "argument must be")) return MYCO_ERROR_TYPE_MISMATCH;
    if (strstr(message, "Modulo by zero")) return MYCO_ERROR_DIVISION_BY_ZERO;
    if (strstr(message, "Power with invalid base")) return MYCO_ERROR_INVALID_CAST;
    
    // Memory Errors (2000-2999)
    if (strstr(message, "Out of memory") || strstr(message, "malloc failed")) return MYCO_ERROR_OUT_OF_MEMORY;
    if (strstr(message, "Null pointer") || strstr(message, "NULL pointer")) return MYCO_ERROR_NULL_POINTER;
    if (strstr(message, "Double free") || strstr(message, "pointer being freed was not allocated")) return MYCO_ERROR_MEMORY_CORRUPTION;
    if (strstr(message, "Memory corruption") || strstr(message, "corrupted")) return MYCO_ERROR_MEMORY_CORRUPTION;
    
    // Type System Errors (3000-3999)
    if (strstr(message, "Type mismatch") || strstr(message, "type mismatch")) return MYCO_ERROR_TYPE_MISMATCH;
    if (strstr(message, "Invalid cast") || strstr(message, "Cannot cast")) return MYCO_ERROR_INVALID_CAST;
    if (strstr(message, "Unsupported operation")) return MYCO_ERROR_INVALID_OPERATION;
    if (strstr(message, "Invalid return type")) return MYCO_ERROR_TYPE_MISMATCH;
    
    // Class and Object Errors (4000-4999)
    if (strstr(message, "Class not found")) return MYCO_ERROR_INTERNAL_ERROR;
    if (strstr(message, "Method not found")) return MYCO_ERROR_INTERNAL_ERROR;
    if (strstr(message, "Instantiation failed") || strstr(message, "Cannot instantiate")) return MYCO_ERROR_INTERNAL_ERROR;
    if (strstr(message, "Inheritance error") || strstr(message, "Parent class")) return MYCO_ERROR_INTERNAL_ERROR;
    if (strstr(message, "Access violation") || strstr(message, "Access denied")) return MYCO_ERROR_INTERNAL_ERROR;
    
    // Exception System Errors (5000-5999)
    if (strstr(message, "Exception thrown") || strstr(message, "throw")) return MYCO_ERROR_INTERNAL_ERROR;
    if (strstr(message, "Unhandled exception")) return MYCO_ERROR_INTERNAL_ERROR;
    if (strstr(message, "Exception in catch")) return MYCO_ERROR_INTERNAL_ERROR;
    if (strstr(message, "Error in finally")) return MYCO_ERROR_INTERNAL_ERROR;
    
    // I/O and System Errors (6000-6999)
    if (strstr(message, "File not found") || strstr(message, "No such file")) return MYCO_ERROR_FILE_NOT_FOUND;
    if (strstr(message, "Permission denied")) return MYCO_ERROR_FILE_PERMISSION;
    if (strstr(message, "I/O error") || strstr(message, "Input/output error")) return MYCO_ERROR_FILE_NOT_FOUND;
    if (strstr(message, "Network error") || strstr(message, "Connection failed")) return MYCO_ERROR_NETWORK_ERROR;
    
    // Syntax and Parse Errors (7000-7999)
    if (strstr(message, "Syntax error") || strstr(message, "Parse error")) return MYCO_ERROR_UNEXPECTED_TOKEN;
    if (strstr(message, "Unexpected token") || strstr(message, "Unexpected")) return MYCO_ERROR_UNEXPECTED_TOKEN;
    if (strstr(message, "Missing token") || strstr(message, "Expected")) return MYCO_ERROR_UNEXPECTED_TOKEN;
    if (strstr(message, "Invalid expression")) return MYCO_ERROR_INVALID_EXPRESSION;
    
    // System and Environment Errors (8000-8999)
    if (strstr(message, "Stack overflow")) return MYCO_ERROR_STACK_OVERFLOW;
    if (strstr(message, "Recursion limit") || strstr(message, "too deep")) return MYCO_ERROR_STACK_OVERFLOW;
    if (strstr(message, "Timeout") || strstr(message, "timed out")) return MYCO_ERROR_TIMEOUT;
    if (strstr(message, "System error")) return MYCO_ERROR_SYSTEM_ERROR;
    
    // Generic fallback
    return MYCO_ERROR_INTERNAL_ERROR;
}

// This function is now defined in error_handling.c

void interpreter_set_error(Interpreter* interpreter, const char* message, int line, int column) {
    if (!interpreter) return;
    
    // Initialize global error system if not already done
    if (!global_error_system) {
        global_error_system = enhanced_error_system_create();
    }
    
    // Set basic error state
    interpreter->has_error = 1;
    interpreter->error_line = line;
    interpreter->error_column = column;
    
    // Free existing error message
    if (interpreter->error_message) {
        shared_free_safe(interpreter->error_message, "interpreter", "unknown_function", 5304);
    }
    
    // Create new error message
    if (message) {
        interpreter->error_message = (message ? strdup(message) : NULL);
    } else {
        interpreter->error_message = strdup("Unknown runtime error");
    }
    
    // Use enhanced error reporting
    if (global_error_system) {
        // Determine error code from message
        MycoErrorCode error_code = MYCO_ERROR_INTERNAL_ERROR;
        if (strstr(message, "Division by zero")) error_code = MYCO_ERROR_DIVISION_BY_ZERO;
        else if (strstr(message, "Undefined variable")) error_code = MYCO_ERROR_UNDEFINED_VARIABLE;
        else if (strstr(message, "Array index out of bounds")) error_code = MYCO_ERROR_ARRAY_BOUNDS;
        else if (strstr(message, "Out of memory")) error_code = MYCO_ERROR_OUT_OF_MEMORY;
        else if (strstr(message, "Type mismatch")) error_code = MYCO_ERROR_TYPE_MISMATCH;
        else if (strstr(message, "Null pointer")) error_code = MYCO_ERROR_NULL_POINTER;
        else if (strstr(message, "Stack overflow")) error_code = MYCO_ERROR_STACK_OVERFLOW;
        else if (strstr(message, "File not found")) error_code = MYCO_ERROR_FILE_NOT_FOUND;
        else if (strstr(message, "Permission denied")) error_code = MYCO_ERROR_FILE_PERMISSION;
        else if (strstr(message, "Network error")) error_code = MYCO_ERROR_NETWORK_ERROR;
        else if (strstr(message, "Timeout")) error_code = MYCO_ERROR_TIMEOUT;
        else if (strstr(message, "Syntax error")) error_code = MYCO_ERROR_UNEXPECTED_TOKEN;
        else if (strstr(message, "Parse error")) error_code = MYCO_ERROR_INVALID_EXPRESSION;
        else if (strstr(message, "Compilation failed")) error_code = MYCO_ERROR_COMPILATION_FAILED;
        else if (strstr(message, "Not implemented")) error_code = MYCO_ERROR_UNIMPLEMENTED;
        
        // Create enhanced error
        EnhancedErrorInfo* error = enhanced_error_create(
            error_code,
            enhanced_error_get_severity(error_code),
            enhanced_error_get_category(error_code),
            message,
            interpreter->current_filename,
            line,
            column
        );
        
        if (error) {
            // Add context information
            if (interpreter->call_stack) {
                CallFrame* frame = interpreter->call_stack;
                while (frame) {
                    enhanced_error_add_stack_frame(error, 
                        frame->function_name,
                        frame->file_name,
                        frame->line,
                        frame->column,
                        NULL, // source_line
                        NULL  // context_info
                    );
                    frame = frame->next;
                }
            }
            
            // Add suggestion
            const char* suggestion = enhanced_error_get_suggestion(error_code);
            if (suggestion) {
                enhanced_error_add_suggestion(error, suggestion);
            }
            
            // Report the error only if not inside a try block
            if (interpreter->try_depth == 0) {
                enhanced_error_report(global_error_system, error);
            }
        }
    } else {
        // Fallback to simple error display
        printf("Error: %s at line %d, column %d\n", message, line, column);
    }
}

void interpreter_clear_error(Interpreter* interpreter) {
    if (!interpreter) return;
    
    interpreter->has_error = 0;
    interpreter->error_line = 0;
    interpreter->error_column = 0;
    
    if (interpreter->error_message) {
        shared_free_safe(interpreter->error_message, "interpreter", "unknown_function", 5326);
        interpreter->error_message = NULL;
    }
}

int interpreter_has_error(Interpreter* interpreter) {
    return interpreter ? interpreter->has_error : 0;
}

int interpreter_has_return(Interpreter* interpreter) {
    return interpreter ? interpreter->has_return : 0;
}

void value_print(Value* value) { Value s = value_to_string(value); if (s.type == VALUE_STRING && s.data.string_value) { printf("%s", s.data.string_value); } value_free(&s); }
void value_print_debug(Value* value) { value_print(value); }
// Enhanced error handling with stack traces
void interpreter_push_call_frame(Interpreter* interpreter, const char* function_name, const char* file_name, int line, int column) {
    if (!interpreter) return;
    
    // Check stack depth limit
    if (interpreter->stack_depth >= interpreter->max_stack_depth) {
        shared_error_report("interpreter", "unknown_function", "Stack overflow: maximum call depth exceeded", line, column);
        return;
    }
    
    CallFrame* frame = shared_malloc_safe(sizeof(CallFrame), "interpreter", "unknown_function", 5572);
    if (!frame) {
        shared_error_report("interpreter", "unknown_function", "Out of memory: cannot create call frame", line, column);
        return;
    }
    
    frame->function_name = function_name ? strdup(function_name) : strdup("<unknown>");
    frame->file_name = file_name ? strdup(file_name) : strdup("<unknown>");
    frame->line = line;
    frame->column = column;
    
    // Extract source line if available
    frame->source_line = NULL;
    if (interpreter->current_source && line > 0) {
        const char* source = interpreter->current_source;
        int current_line = 1;
        const char* line_start = source;
        
        // Find the line
        while (*source && current_line < line) {
            if (*source == '\n') {
                current_line++;
                line_start = source + 1;
            }
            source++;
        }
        
        if (current_line == line) {
            // Find end of line
            const char* line_end = source;
            while (*line_end && *line_end != '\n') {
                line_end++;
            }
            
            // Allocate and copy the line
            size_t line_len = line_end - line_start;
            frame->source_line = shared_malloc_safe(line_len + 1, "interpreter", "unknown_function", 5608);
            if (frame->source_line) {
                strncpy((char*)frame->source_line, line_start, line_len);
                ((char*)frame->source_line)[line_len] = '\0';
            }
        }
    }
    
    frame->next = interpreter->call_stack;
    
    interpreter->call_stack = frame;
    interpreter->stack_depth++;
}

void interpreter_pop_call_frame(Interpreter* interpreter) {
    if (!interpreter || !interpreter->call_stack) return;
    
    CallFrame* frame = interpreter->call_stack;
    interpreter->call_stack = frame->next;
    interpreter->stack_depth--;
    
    shared_free_safe((void*)frame->function_name, "interpreter", "unknown_function", 5629);
    shared_free_safe((void*)frame->file_name, "interpreter", "unknown_function", 5630);
    if (frame->source_line) {
        shared_free_safe((void*)frame->source_line, "interpreter", "unknown_function", 5632);
    }
    shared_free_safe(frame, "interpreter", "unknown_function", 5634);
}

void interpreter_print_stack_trace(Interpreter* interpreter) {
    if (!interpreter || !interpreter->call_stack) return;
    
    fprintf(stderr, "\nStack trace:\n");
    CallFrame* frame = interpreter->call_stack;
    int depth = 0;
    
    while (frame) {
        fprintf(stderr, "  %d. %s at %s:%d:%d\n", 
                depth, frame->function_name, frame->file_name, frame->line, frame->column);
        
        // Show source line if available
        if (frame->source_line) {
            fprintf(stderr, "     %s\n", frame->source_line);
        }
        
        frame = frame->next;
        depth++;
    }
    fprintf(stderr, "\n");
}

void interpreter_set_error_with_stack(Interpreter* interpreter, const char* message, int line, int column) {
    if (!interpreter) return;
    
    // Set the error
    interpreter_set_error(interpreter, message, line, column);
    
    // Print stack trace if available
    if (interpreter->call_stack) {
        interpreter_print_stack_trace(interpreter);
    }
}

// Set current source text and filename for line extraction
void interpreter_set_source(Interpreter* interpreter, const char* source, const char* filename) {
    if (!interpreter) return;
    interpreter->current_source = source;
    interpreter->current_filename = filename;
}

// Exception handling
void interpreter_throw_exception(Interpreter* interpreter, const char* message, int line, int column) {
    if (!interpreter) return;
    
    // Set error with stack trace
    interpreter_set_error_with_stack(interpreter, message, line, column);
    
    // Mark as exception (could be used for different handling)
    // For now, we'll use the same error mechanism
}


// JIT compilation functions
void interpreter_enable_jit(Interpreter* interpreter, int enable) {
    if (!interpreter) return;
    
    interpreter->jit_enabled = enable;
    
    if (enable && !interpreter->jit_context) {
        interpreter->jit_context = jit_context_create(JIT_TARGET_AUTO, JIT_MODE_HYBRID);
        if (interpreter->jit_context) {
            jit_set_optimization_level(interpreter->jit_context, 2);
        }
    } else if (!enable && interpreter->jit_context) {
        jit_context_free(interpreter->jit_context);
        interpreter->jit_context = NULL;
    }
}

void interpreter_set_jit_mode(Interpreter* interpreter, int mode) {
    if (!interpreter) return;
    
    interpreter->jit_mode = mode;
    
    if (interpreter->jit_context) {
        JitCompilationMode jit_mode = JIT_MODE_INTERPRETED;
        switch (mode) {
            case 0: jit_mode = JIT_MODE_INTERPRETED; break;
            case 1: jit_mode = JIT_MODE_HYBRID; break;
            case 2: jit_mode = JIT_MODE_COMPILED; break;
            default: jit_mode = JIT_MODE_HYBRID; break;
        }
        
        // Recreate context with new mode
        jit_context_free(interpreter->jit_context);
        interpreter->jit_context = jit_context_create(JIT_TARGET_AUTO, jit_mode);
        if (interpreter->jit_context) {
            jit_set_optimization_level(interpreter->jit_context, 2);
        }
    }
}

JitContext* interpreter_get_jit_context(Interpreter* interpreter) {
    return interpreter ? interpreter->jit_context : NULL;
}

Value* interpreter_get_self_context(Interpreter* interpreter) {
    return interpreter ? interpreter->self_context : NULL;
}

void interpreter_set_self_context(Interpreter* interpreter, Value* self) {
    if (interpreter) {
        interpreter->self_context = self;
    }
}

int interpreter_compile_function(Interpreter* interpreter, const char* function_name) {
    if (!interpreter || !function_name || !interpreter->jit_enabled || !interpreter->jit_context) {
        return 0;
    }
    
    // Look up function in global environment
    Value func_value = environment_get(interpreter->global_environment, function_name);
    if (func_value.type != VALUE_FUNCTION) {
        return 0;
    }
    
    // Create AST node for function (simplified)
    // In a real implementation, we'd need to store the original AST
    // For now, we'll return success but not actually compile
    return 1;
}

Value interpreter_execute_compiled_function(Interpreter* interpreter, const char* function_name, Value* args, size_t arg_count) {
    if (!interpreter || !function_name || !interpreter->jit_enabled || !interpreter->jit_context) {
        // Fall back to interpreted execution
        return interpreter_execute_function_call(interpreter, NULL);
    }
    
    // Try to execute compiled function
    void** native_args = shared_malloc_safe(sizeof(void*) * arg_count, "interpreter", "unknown_function", 2992);
    if (!native_args) {
        return interpreter_execute_function_call(interpreter, NULL);
    }
    
    // Convert Value* to void** for native execution
    for (size_t i = 0; i < arg_count; i++) {
        native_args[i] = &args[i];
    }
    
    void* result = jit_execute_function(interpreter->jit_context, function_name, native_args);
    shared_free_safe(native_args, "interpreter", "unknown_function", 5769);
    
    if (result) {
        // Convert native result back to Value
        return *(Value*)result;
    } else {
        // Fall back to interpreted execution
        return interpreter_execute_function_call(interpreter, NULL);
    }
}

/**
 * @brief Check if a value matches a pattern (enhanced pattern matching)
 * 
 * @param interpreter The interpreter context
 * @param value The value to match against
 * @param pattern The pattern AST node
 * @return 1 if pattern matches, 0 otherwise
 */
static int pattern_matches(Interpreter* interpreter, Value* value, ASTNode* pattern) {
    if (!interpreter || !value || !pattern) {
        return 0;
    }
    
    switch (pattern->type) {
        case AST_NODE_PATTERN_TYPE:
            return pattern_matches_type(value, pattern->data.pattern_type.type_name);
            
        case AST_NODE_PATTERN_WILDCARD:
            return 1; // Wildcard matches anything
            
        case AST_NODE_PATTERN_DESTRUCTURE:
            return pattern_matches_destructure(interpreter, value, pattern);
            
        case AST_NODE_PATTERN_GUARD:
            return pattern_matches_guard(interpreter, value, pattern);
            
        case AST_NODE_PATTERN_OR:
            return pattern_matches_or(interpreter, value, pattern);
            
        case AST_NODE_PATTERN_AND:
            return pattern_matches_and(interpreter, value, pattern);
            
        case AST_NODE_PATTERN_NOT:
            return !pattern_matches(interpreter, value, pattern->data.pattern_not.pattern);
            
        case AST_NODE_PATTERN_RANGE:
            return pattern_matches_range(interpreter, value, pattern);
            
        case AST_NODE_PATTERN_REGEX:
            return pattern_matches_regex(interpreter, value, pattern);
            
        default:
            // Fall back to simple equality for basic patterns
            Value pattern_value = eval_node(interpreter, pattern);
            int matches = value_equals(value, &pattern_value);
            value_free(&pattern_value);
            return matches;
    }
}

/**
 * @brief Check if value matches a type pattern
 */
static int pattern_matches_type(Value* value, const char* type_name) {
    if (!value || !type_name) return 0;
    
    // Handle common type names
    if (strcmp(type_name, "Int") == 0) {
        return value->type == VALUE_NUMBER && value->data.number_value == (int)value->data.number_value;
    }
    if (strcmp(type_name, "Float") == 0) {
        return value->type == VALUE_NUMBER;
    }
    if (strcmp(type_name, "String") == 0) {
        return value->type == VALUE_STRING;
    }
    if (strcmp(type_name, "Bool") == 0) {
        return value->type == VALUE_BOOLEAN;
    }
    if (strcmp(type_name, "Array") == 0) {
        return value->type == VALUE_ARRAY;
    }
    if (strcmp(type_name, "Object") == 0) {
        return value->type == VALUE_OBJECT;
    }
    if (strcmp(type_name, "Null") == 0) {
        return value->type == VALUE_NULL;
    }
    
    return 0;
}

/**
 * @brief Check if value matches a destructuring pattern
 */
static int pattern_matches_destructure(Interpreter* interpreter, Value* value, ASTNode* pattern) {
    if (!value || !pattern) return 0;
    
    if (pattern->data.pattern_destructure.is_array) {
        // Array destructuring: [a, b, c]
        if (value->type != VALUE_ARRAY) return 0;
        
        // For now, simplified array destructuring - just check if it's an array
        // TODO: Implement proper array element matching
        return 1;
    } else {
        // Object destructuring: {name: n, age: a}
        if (value->type != VALUE_OBJECT) return 0;
        
        // For now, just check if it's an object (simplified)
        return 1;
    }
}

/**
 * @brief Check if value matches a guard pattern
 */
static int pattern_matches_guard(Interpreter* interpreter, Value* value, ASTNode* pattern) {
    if (!value || !pattern) return 0;
    
    // First check if the base pattern matches
    if (!pattern_matches(interpreter, value, pattern->data.pattern_guard.pattern)) {
        return 0;
    }
    
    // Then check the guard condition
    Value condition_result = eval_node(interpreter, pattern->data.pattern_guard.condition);
    int matches = condition_result.type == VALUE_BOOLEAN && condition_result.data.boolean_value;
    value_free(&condition_result);
    return matches;
}

/**
 * @brief Check if value matches an OR pattern
 */
static int pattern_matches_or(Interpreter* interpreter, Value* value, ASTNode* pattern) {
    if (!value || !pattern) return 0;
    
    return pattern_matches(interpreter, value, pattern->data.pattern_or.left) ||
           pattern_matches(interpreter, value, pattern->data.pattern_or.right);
}

/**
 * @brief Check if value matches an AND pattern
 */
static int pattern_matches_and(Interpreter* interpreter, Value* value, ASTNode* pattern) {
    if (!value || !pattern) return 0;
    
    return pattern_matches(interpreter, value, pattern->data.pattern_and.left) &&
           pattern_matches(interpreter, value, pattern->data.pattern_and.right);
}

/**
 * @brief Check if value matches a range pattern
 */
static int pattern_matches_range(Interpreter* interpreter, Value* value, ASTNode* pattern) {
    if (!value || !pattern || value->type != VALUE_NUMBER) return 0;
    
    Value start_val = eval_node(interpreter, pattern->data.pattern_range.start);
    Value end_val = eval_node(interpreter, pattern->data.pattern_range.end);
    
    if (start_val.type != VALUE_NUMBER || end_val.type != VALUE_NUMBER) {
        value_free(&start_val);
        value_free(&end_val);
        return 0;
    }
    
    double val = value->data.number_value;
    double start = start_val.data.number_value;
    double end = end_val.data.number_value;
    
    value_free(&start_val);
    value_free(&end_val);
    
    if (pattern->data.pattern_range.inclusive) {
        return val >= start && val <= end;
    } else {
        return val >= start && val < end;
    }
}

/**
 * @brief Check if value matches a regex pattern
 */
static int pattern_matches_regex(Interpreter* interpreter, Value* value, ASTNode* pattern) {
    if (!value || !pattern || value->type != VALUE_STRING) return 0;
    
    // For now, just check if it's a string (simplified regex matching)
    // TODO: Implement actual regex matching
    return 1;
}

// ============================================================================
// GLOBAL SYSTEM MANAGEMENT
// ============================================================================

/**
 * @brief Initialize global error and debug systems
 */
void interpreter_initialize_global_systems(void) {
    if (!global_error_system) {
        global_error_system = enhanced_error_system_create();
    }
    if (!global_debug_system) {
        global_debug_system = debug_system_create();
    }
    if (!global_repl_session) {
        global_repl_session = repl_debug_session_create();
    }
}

/**
 * @brief Cleanup global error and debug systems
 */
void interpreter_cleanup_global_systems(void) {
    if (global_repl_session) {
        repl_debug_session_free(global_repl_session);
        global_repl_session = NULL;
    }
    if (global_debug_system) {
        debug_system_free(global_debug_system);
        global_debug_system = NULL;
    }
    if (global_error_system) {
        enhanced_error_system_free(global_error_system);
        global_error_system = NULL;
    }
}

/**
 * @brief Get global error system
 */
EnhancedErrorSystem* interpreter_get_global_error_system(void) {
    if (!global_error_system) {
        interpreter_initialize_global_systems();
    }
    return global_error_system;
}

/**
 * @brief Get global debug system
 */
DebugSystem* interpreter_get_global_debug_system(void) {
    if (!global_debug_system) {
        interpreter_initialize_global_systems();
    }
    return global_debug_system;
}

/**
 * @brief Get global REPL debug session
 */
ReplDebugSession* interpreter_get_global_repl_session(void) {
    if (!global_repl_session) {
        interpreter_initialize_global_systems();
    }
    return global_repl_session;
}

// ============================================================================
// BENCHMARK TIMING FUNCTIONS
// ============================================================================

void interpreter_set_benchmark_mode(Interpreter* interpreter, int enabled) {
    if (interpreter) {
        interpreter->benchmark_mode = enabled;
    }
}

uint64_t interpreter_get_execution_time_ns(Interpreter* interpreter) {
    return interpreter ? interpreter->execution_time_ns : 0;
}

void interpreter_start_timing(Interpreter* interpreter) {
    if (interpreter && interpreter->benchmark_mode) {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        interpreter->start_time_ns = (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
    }
}

void interpreter_stop_timing(Interpreter* interpreter) {
    if (interpreter && interpreter->benchmark_mode) {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        uint64_t end_time_ns = (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
        interpreter->execution_time_ns = end_time_ns - interpreter->start_time_ns;
    }
}