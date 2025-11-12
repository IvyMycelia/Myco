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
// eval_engine.h removed - AST execution no longer used
#include "optimization/hot_spot_tracker.h"
#include "optimization/bytecode_engine.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>

// Pattern matching is handled by bytecode_vm.c (pattern_matches_value)

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
#include <signal.h>
#include <execinfo.h>

// Placeholder interpreter implementation
// This will be replaced with the full implementation

// Signal handler for debugging segfaults (disabled for performance)
// void segfault_handler(int sig) {
//     
//     // Print backtrace
//     void *array[10];
//     size_t size = backtrace(array, 10);
//     char **strings = backtrace_symbols(array, size);
//     
//     for (size_t i = 0; i < size; i++) {
//     }
//     
//     free(strings);
//     exit(1);
// }

Interpreter* interpreter_create(void) {
    
    Interpreter* interpreter = shared_malloc_safe(sizeof(Interpreter), "interpreter", "unknown_function", 28);
    if (!interpreter) {
        return NULL;
    }
    
    
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
    interpreter->bytecode_program_cache = NULL;
    
    // Module cache initialization (Phase 4)
    interpreter->module_cache = NULL;
    interpreter->module_cache_count = 0;
    interpreter->module_cache_capacity = 0;
    interpreter->import_chain = NULL;
    
    // Async/await support initialization
    interpreter->task_queue = NULL;
    interpreter->task_queue_size = 0;
    interpreter->task_queue_capacity = 0;
    interpreter->async_enabled = 1;  // Enable async by default
    
    // Concurrency support initialization
    interpreter->worker_threads = NULL;
    interpreter->worker_thread_count = 0;
    interpreter->shutdown_workers = 0;
    // Mutexes and condition variable will be initialized when concurrency is enabled
    
    // Promise registry initialization
    interpreter->promise_registry = NULL;
    interpreter->promise_registry_size = 0;
    interpreter->promise_registry_capacity = 0;
    interpreter->next_promise_id = 1;  // Start at 1 (0 means no ID)
    
    // Capability-based security initialization
    interpreter->capability_registry = NULL;
    interpreter->capability_registry_count = 0;
    interpreter->capability_registry_capacity = 0;
    interpreter->module_security_contexts = NULL;
    interpreter->module_security_context_count = 0;
    interpreter->module_security_context_capacity = 0;
    interpreter->current_loading_module = NULL;
    interpreter->module_security_enabled = 1;  // Enable security by default
    
    // Test mode - disabled by default
    interpreter->test_mode = 0;
    
    // Enhanced error handling initialization
    interpreter->call_stack = NULL;
    interpreter->stack_depth = 0;
    interpreter->max_stack_depth = 1000;  // Reasonable limit
    interpreter->recursion_count = 0;
    interpreter->max_recursion_depth = 100;  // Reasonable limit
    
    // JIT compilation initialization (disabled for now due to conflicts)
    interpreter->micro_jit_context = NULL;
    interpreter->hot_spot_tracker = hot_spot_tracker_create();
    if (!interpreter->hot_spot_tracker) {
        // If hot spot tracker creation fails, disable it
        interpreter->hot_spot_tracker = NULL;
    }
    interpreter->jit_enabled = 0;
    interpreter->jit_mode = 0;
    
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
        
        // Clean up module cache (Phase 4)
        if (interpreter->module_cache) {
            for (size_t i = 0; i < interpreter->module_cache_count; i++) {
                ModuleCacheEntry* entry = &interpreter->module_cache[i];
                if (entry->file_path) {
                    shared_free_safe(entry->file_path, "interpreter", "interpreter_free", 0);
                }
                if (entry->file_hash) {
                    shared_free_safe(entry->file_hash, "interpreter", "interpreter_free", 0);
                }
                // Note: module_env and module_value are freed when interpreter is freed
                // They may be referenced elsewhere, so we don't free them here
            }
            shared_free_safe(interpreter->module_cache, "interpreter", "interpreter_free", 0);
        }
        
        // Clean up import chain (Phase 4)
        ImportChain* chain = interpreter->import_chain;
        while (chain) {
            ImportChain* next = chain->next;
            if (chain->module_path) {
                shared_free_safe(chain->module_path, "interpreter", "interpreter_free", 0);
            }
            shared_free_safe(chain, "interpreter", "interpreter_free", 0);
            chain = next;
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
        
        // Clean up hot spot tracker
        if (interpreter->hot_spot_tracker) {
            hot_spot_tracker_free(interpreter->hot_spot_tracker);
        }
        
        // Clean up macro expander
        if (interpreter->macro_expander) {
            macro_expander_free(interpreter->macro_expander);
        }
        
        // Clean up compile-time evaluator
        // if (interpreter->compile_time_evaluator) {
        //     compile_time_evaluator_free(interpreter->compile_time_evaluator);
        // }
        
        // Shutdown async concurrency system
        // Note: async_shutdown_concurrency is static in bytecode_vm.c, so we clean up manually
        if (interpreter->worker_thread_count > 0 && interpreter->worker_threads) {
            // Signal shutdown
            pthread_mutex_lock(&interpreter->task_queue_mutex);
            interpreter->shutdown_workers = 1;
            pthread_cond_broadcast(&interpreter->task_available);
            pthread_mutex_unlock(&interpreter->task_queue_mutex);
            
            // Wait for all threads to finish
            for (size_t i = 0; i < interpreter->worker_thread_count; i++) {
                pthread_join(interpreter->worker_threads[i], NULL);
            }
            
            // Cleanup
            shared_free_safe(interpreter->worker_threads, "interpreter", "interpreter_free", 0);
            interpreter->worker_threads = NULL;
            interpreter->worker_thread_count = 0;
            pthread_mutex_destroy(&interpreter->task_queue_mutex);
            pthread_mutex_destroy(&interpreter->promise_registry_mutex);
            pthread_cond_destroy(&interpreter->task_available);
        }
        
        // Clean up async task queue
        if (interpreter->task_queue) {
            // Free any remaining tasks
            for (size_t i = 0; i < interpreter->task_queue_size; i++) {
                AsyncTask* task = interpreter->task_queue[i];
                if (task) {
                    if (task->args) {
                        for (size_t j = 0; j < task->arg_count; j++) {
                            value_free(&task->args[j]);
                        }
                        shared_free_safe(task->args, "interpreter", "interpreter_free", 0);
                    }
                    value_free(&task->promise_copy);
                    value_free(&task->result);
                    shared_free_safe(task, "interpreter", "interpreter_free", 0);
                }
            }
            shared_free_safe(interpreter->task_queue, "interpreter", "interpreter_free", 0);
        }
        
        // Clean up promise registry
        if (interpreter->promise_registry) {
            for (size_t i = 0; i < interpreter->promise_registry_size; i++) {
                value_free(&interpreter->promise_registry[i]);
            }
            shared_free_safe(interpreter->promise_registry, "interpreter", "interpreter_free", 0);
        }
        
        // Clean up capability registry
        if (interpreter->capability_registry) {
            for (size_t i = 0; i < interpreter->capability_registry_count; i++) {
                shared_free_safe(interpreter->capability_registry[i].name, "interpreter", "interpreter_free", 0);
                value_free(&interpreter->capability_registry[i].implementation);
            }
            shared_free_safe(interpreter->capability_registry, "interpreter", "interpreter_free", 0);
        }
        
        // Clean up module security contexts
        if (interpreter->module_security_contexts) {
            for (size_t i = 0; i < interpreter->module_security_context_count; i++) {
                shared_free_safe(interpreter->module_security_contexts[i].module_path, "interpreter", "interpreter_free", 0);
                if (interpreter->module_security_contexts[i].allowed_capabilities) {
                    for (size_t j = 0; j < interpreter->module_security_contexts[i].capability_count; j++) {
                        shared_free_safe(interpreter->module_security_contexts[i].allowed_capabilities[j], "interpreter", "interpreter_free", 0);
                    }
                    shared_free_safe(interpreter->module_security_contexts[i].allowed_capabilities, "interpreter", "interpreter_free", 0);
                }
            }
            shared_free_safe(interpreter->module_security_contexts, "interpreter", "interpreter_free", 0);
        }
        
        shared_free_safe(interpreter->current_loading_module, "interpreter", "interpreter_free", 0);
        
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

// Helper function to handle super method calls

// Helper function to handle server method calls

// Helper function to handle request method calls

// Helper function to handle response method calls

// Helper function to handle route group method calls

// Helper function to handle server library method calls

// Helper function to handle method calls

Value value_create_error(const char* message, int code) { Value v = {0}; return v; }

// AST execution removed - bytecode is the only execution path
// Helper function to compile AST node to bytecode and execute it
static Value interpreter_execute_ast_node(Interpreter* interpreter, ASTNode* node) {
    if (!interpreter || !node) {
        return value_create_null();
    }
    
    // Compile AST node to bytecode sub-program
    BytecodeProgram* temp_program = bytecode_compile_ast(node, interpreter);
    if (!temp_program) {
        if (interpreter) {
            interpreter_set_error(interpreter, "Failed to compile AST node to bytecode", 0, 0);
        }
        return value_create_null();
    }
    
    // Execute the bytecode
    Value result = interpreter_execute_bytecode(interpreter, temp_program);
    
    // Free the temporary program
    bytecode_program_free(temp_program);
    
    return result;
}

Value interpreter_execute_program(Interpreter* interpreter, ASTNode* node) {
    
    if (!node) {
        return value_create_null();
    }
    
    
    // Clear any previous errors before starting execution
    if (interpreter) {
        interpreter_clear_error(interpreter);
    }
    
    // Bytecode is the ONLY execution path (like LuaJIT)
    // If bytecode compilation or execution fails, the program fails
    BytecodeProgram* bytecode = bytecode_compile_ast(node, interpreter);
    if (!bytecode) {
        // Bytecode compilation failed - report error and fail
        if (interpreter) {
            interpreter_set_error(interpreter, "Bytecode compilation failed", 0, 0);
        }
        return value_create_null();
    }
    
    Value result = interpreter_execute_bytecode(interpreter, bytecode);
    
    // Store bytecode program in interpreter cache for function calls
    // CRITICAL: Don't free the old cached program if we're executing a module
    // (modules execute during main program execution, so we need to preserve the main program)
    // We detect this by checking if the cache already exists and is different from the new program
    // If so, we're likely executing a module, so preserve the main program
    BytecodeProgram* old_cache = interpreter->bytecode_program_cache;
    if (old_cache && old_cache != bytecode) {
        // We're replacing the cache with a different program
        // This happens when executing modules - don't free the old one yet
        // The old program (main program) is still being executed and needs its bytecode
        // We'll let the caller handle cleanup, or use reference counting
        // For now, just don't free it - it will be freed when the main program finishes
    }
    interpreter->bytecode_program_cache = bytecode; // Keep program alive for function calls
    
    // Errors are reported but execution continues
    return result;
}
// Legacy AST execution functions - replaced with bytecode compilation
Value interpreter_execute(Interpreter* interpreter, ASTNode* node) {
    return interpreter_execute_ast_node(interpreter, node);
}

Value interpreter_execute_statement(Interpreter* interpreter, ASTNode* node) { 
    return interpreter_execute_ast_node(interpreter, node); 
}
Value interpreter_execute_expression(Interpreter* interpreter, ASTNode* node) { 
    return interpreter_execute_ast_node(interpreter, node); 
}
Value interpreter_execute_binary_op(Interpreter* interpreter, ASTNode* node) { 
    return interpreter_execute_ast_node(interpreter, node); 
}
Value interpreter_execute_unary_op(Interpreter* interpreter, ASTNode* node) { 
    return interpreter_execute_ast_node(interpreter, node); 
}
Value interpreter_execute_assignment(Interpreter* interpreter, ASTNode* node) { 
    return interpreter_execute_ast_node(interpreter, node); 
}
Value interpreter_execute_function_call(Interpreter* interpreter, ASTNode* node) { 
    return interpreter_execute_ast_node(interpreter, node); 
}
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
        interpreter->error_message = shared_strdup(message);
    } else {
        interpreter->error_message = shared_strdup("Unknown runtime error");
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
            interpreter->current_file,
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
        printf("Error: %s at line %d, column %d\n", message ? message : "Unknown error", line, column);
    }
    
    
    // Errors are reported but execution continues
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

void interpreter_set_test_mode(Interpreter* interpreter, int test_mode) {
    if (!interpreter) return;
    interpreter->test_mode = test_mode;
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
    
    frame->function_name = function_name ? shared_strdup(function_name) : shared_strdup("<unknown>");
    frame->file_name = file_name ? shared_strdup(file_name) : shared_strdup("<unknown>");
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
    interpreter->current_file = filename;
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

// Pattern matching is handled by bytecode_vm.c (pattern_matches_value)
// All AST-based pattern matching code has been removed

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
// CAPABILITY-BASED SECURITY FUNCTIONS
// ============================================================================

// Register a capability (host provides safe implementation)
void interpreter_register_capability(Interpreter* interpreter, const char* capability_name, Value implementation, int is_safe) {
    if (!interpreter || !capability_name) return;
    
    // Check if capability already exists
    for (size_t i = 0; i < interpreter->capability_registry_count; i++) {
        if (strcmp(interpreter->capability_registry[i].name, capability_name) == 0) {
            // Update existing capability
            value_free(&interpreter->capability_registry[i].implementation);
            interpreter->capability_registry[i].implementation = value_clone(&implementation);
            interpreter->capability_registry[i].is_safe = is_safe;
            return;
        }
    }
    
    // Expand registry if needed
    if (interpreter->capability_registry_count >= interpreter->capability_registry_capacity) {
        size_t new_capacity = interpreter->capability_registry_capacity == 0 ? 8 : interpreter->capability_registry_capacity * 2;
        CapabilityEntry* new_registry = shared_realloc_safe(interpreter->capability_registry,
                                                           new_capacity * sizeof(CapabilityEntry),
                                                           "interpreter", "interpreter_register_capability", 0);
        if (!new_registry) return;
        interpreter->capability_registry = new_registry;
        interpreter->capability_registry_capacity = new_capacity;
    }
    
    // Add new capability
    CapabilityEntry* entry = &interpreter->capability_registry[interpreter->capability_registry_count++];
    entry->name = shared_strdup(capability_name);
    entry->implementation = value_clone(&implementation);
    entry->is_safe = is_safe;
}

// Find or create module security context
static ModuleSecurityContext* find_or_create_module_security_context(Interpreter* interpreter, const char* module_path) {
    if (!interpreter || !module_path) return NULL;
    
    // Find existing context
    for (size_t i = 0; i < interpreter->module_security_context_count; i++) {
        if (strcmp(interpreter->module_security_contexts[i].module_path, module_path) == 0) {
            return &interpreter->module_security_contexts[i];
        }
    }
    
    // Expand contexts array if needed
    if (interpreter->module_security_context_count >= interpreter->module_security_context_capacity) {
        size_t new_capacity = interpreter->module_security_context_capacity == 0 ? 8 : interpreter->module_security_context_capacity * 2;
        ModuleSecurityContext* new_contexts = shared_realloc_safe(interpreter->module_security_contexts,
                                                                   new_capacity * sizeof(ModuleSecurityContext),
                                                                   "interpreter", "find_or_create_module_security_context", 0);
        if (!new_contexts) return NULL;
        interpreter->module_security_contexts = new_contexts;
        interpreter->module_security_context_capacity = new_capacity;
    }
    
    // Create new context
    ModuleSecurityContext* context = &interpreter->module_security_contexts[interpreter->module_security_context_count++];
    context->module_path = shared_strdup(module_path);
    context->allowed_capabilities = NULL;
    context->capability_count = 0;
    context->is_trusted = 0;
    
    return context;
}

// Grant a capability to a module
void interpreter_grant_capability_to_module(Interpreter* interpreter, const char* module_path, const char* capability_name) {
    if (!interpreter || !module_path || !capability_name) return;
    
    ModuleSecurityContext* context = find_or_create_module_security_context(interpreter, module_path);
    if (!context) return;
    
    // Check if capability already granted
    for (size_t i = 0; i < context->capability_count; i++) {
        if (strcmp(context->allowed_capabilities[i], capability_name) == 0) {
            return; // Already granted
        }
    }
    
    // Expand capabilities array
    char** new_capabilities = shared_realloc_safe(context->allowed_capabilities,
                                                 (context->capability_count + 1) * sizeof(char*),
                                                 "interpreter", "interpreter_grant_capability_to_module", 0);
    if (!new_capabilities) return;
    context->allowed_capabilities = new_capabilities;
    context->allowed_capabilities[context->capability_count++] = shared_strdup(capability_name);
}

// Revoke a capability from a module
void interpreter_revoke_capability_from_module(Interpreter* interpreter, const char* module_path, const char* capability_name) {
    if (!interpreter || !module_path || !capability_name) return;
    
    ModuleSecurityContext* context = find_or_create_module_security_context(interpreter, module_path);
    if (!context) return;
    
    // Find and remove capability
    for (size_t i = 0; i < context->capability_count; i++) {
        if (strcmp(context->allowed_capabilities[i], capability_name) == 0) {
            shared_free_safe(context->allowed_capabilities[i], "interpreter", "interpreter_revoke_capability_from_module", 0);
            // Shift remaining capabilities
            for (size_t j = i; j < context->capability_count - 1; j++) {
                context->allowed_capabilities[j] = context->allowed_capabilities[j + 1];
            }
            context->capability_count--;
            return;
        }
    }
}

// Check if a module has a capability
int interpreter_module_has_capability(Interpreter* interpreter, const char* module_path, const char* capability_name) {
    if (!interpreter || !module_path || !capability_name) return 0;
    
    // If security is disabled, allow everything
    if (!interpreter->module_security_enabled) return 1;
    
    // Find module security context
    for (size_t i = 0; i < interpreter->module_security_context_count; i++) {
        if (strcmp(interpreter->module_security_contexts[i].module_path, module_path) == 0) {
            ModuleSecurityContext* context = &interpreter->module_security_contexts[i];
            
            // Trusted modules bypass capability checks
            if (context->is_trusted) return 1;
            
            // Check if capability is granted
            for (size_t j = 0; j < context->capability_count; j++) {
                if (strcmp(context->allowed_capabilities[j], capability_name) == 0) {
                    return 1;
                }
            }
            return 0;
        }
    }
    
    // Module not found - deny by default
    return 0;
}

// Set module as trusted (bypasses all capability checks)
void interpreter_set_module_trusted(Interpreter* interpreter, const char* module_path, int trusted) {
    if (!interpreter || !module_path) return;
    
    ModuleSecurityContext* context = find_or_create_module_security_context(interpreter, module_path);
    if (!context) return;
    
    context->is_trusted = trusted ? 1 : 0;
}

// Get capability implementation for current module
Value interpreter_get_capability(Interpreter* interpreter, const char* capability_name) {
    if (!interpreter || !capability_name) return value_create_null();
    
    // Find capability in registry
    for (size_t i = 0; i < interpreter->capability_registry_count; i++) {
        if (strcmp(interpreter->capability_registry[i].name, capability_name) == 0) {
            // Check if current module has access
            const char* current_module = interpreter->current_loading_module;
            if (current_module && !interpreter_module_has_capability(interpreter, current_module, capability_name)) {
                return value_create_null(); // Module doesn't have this capability
            }
            return value_clone(&interpreter->capability_registry[i].implementation);
        }
    }
    
    return value_create_null(); // Capability not found
}

// Enable/disable module security globally
void interpreter_set_module_security_enabled(Interpreter* interpreter, int enabled) {
    if (!interpreter) return;
    interpreter->module_security_enabled = enabled ? 1 : 0;
}