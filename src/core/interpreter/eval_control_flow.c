#include "interpreter/value_operations.h"
#include "../../include/core/interpreter.h"
#include "../../include/core/enhanced_error_system.h"
#include "../../include/utils/shared_utilities.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Forward declaration for global error system
extern EnhancedErrorSystem* global_error_system;

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

// Sync variables from source environment to target environment
static void sync_environment_variables(Environment* source, Environment* target) {
    if (!source || !target) return;
    
    // Iterate through all variables in the source environment
    for (size_t i = 0; i < source->count; i++) {
        if (source->names[i] && source->values[i].type != VALUE_NULL) {
            // Check if variable exists in target environment
            Value existing = environment_get(target, source->names[i]);
            if (existing.type != VALUE_NULL) {
                // Variable exists, update it - free the existing value first
                value_free(&existing);
            }
            // Always define the variable (either update or create)
            environment_define(target, source->names[i], value_clone(&source->values[i]));
        }
    }
}

// ============================================================================
// CONTROL FLOW EVALUATION FUNCTIONS
// ============================================================================

Value eval_if_statement(Interpreter* interpreter, ASTNode* node) {
    Value cond = interpreter_execute(interpreter, node->data.if_statement.condition);
    if (interpreter_has_error(interpreter)) {
        value_free(&cond);
        return value_create_null();
    }
    int truthy = value_is_truthy(&cond);
    value_free(&cond);
    if (truthy && node->data.if_statement.then_block) {
        Value result = interpreter_execute(interpreter, node->data.if_statement.then_block);
        if (interpreter_has_error(interpreter)) {
            return value_create_null();
        }
        return result;
    } else if (!truthy && node->data.if_statement.else_if_chain) {
        // Handle else if chain
        Value result = interpreter_execute(interpreter, node->data.if_statement.else_if_chain);
        if (interpreter_has_error(interpreter)) {
            return value_create_null();
        }
        return result;
    } else if (!truthy && node->data.if_statement.else_block) {
        Value result = interpreter_execute(interpreter, node->data.if_statement.else_block);
        if (interpreter_has_error(interpreter)) {
            return value_create_null();
        }
        return result;
    }
    return value_create_null();
}

Value eval_while_loop(Interpreter* interpreter, ASTNode* node) {
    while (1) {
        Value cond = interpreter_execute(interpreter, node->data.while_loop.condition);
        if (interpreter_has_error(interpreter)) {
            value_free(&cond);
            return value_create_null();
        }
        int truthy = value_is_truthy(&cond);
        value_free(&cond);
        if (!truthy) break;
        if (node->data.while_loop.body) {
            Value body_result = interpreter_execute(interpreter, node->data.while_loop.body);
            if (interpreter_has_error(interpreter)) {
                value_free(&body_result);
                return value_create_null();
            }
            // CRITICAL: Free the body result to prevent memory leak
            value_free(&body_result);
        }
    }
    return value_create_null();
}

Value eval_for_loop(Interpreter* interpreter, ASTNode* node) {
    // Evaluate the collection/range
    Value collection = interpreter_execute(interpreter, node->data.for_loop.collection);
    
    // Handle range iteration
    if (collection.type == VALUE_RANGE) {
        double start = collection.data.range_value.start;
        double end = collection.data.range_value.end;
        double step = collection.data.range_value.step;
        
        // Create a new environment for the loop scope
        Environment* loop_env = environment_create(interpreter->current_environment);
        Environment* old_env = interpreter->current_environment;
        interpreter->current_environment = loop_env;
        
        // Iterate through the range (exclusive of end) with step
        for (double i = start; i < end; i += step) {
            // Define the iterator variable
            Value iterator_value = value_create_number(i);
            environment_define(loop_env, node->data.for_loop.iterator_name, iterator_value);
            value_free(&iterator_value);
            
            // Execute the loop body
            if (node->data.for_loop.body) {
                Value body_result = interpreter_execute(interpreter, node->data.for_loop.body);
                if (interpreter_has_error(interpreter)) {
                    value_free(&body_result);
                    // Restore environment before returning
                    interpreter->current_environment = old_env;
                    environment_free(loop_env);
                    value_free(&collection);
                    return value_create_null();
                }
                // CRITICAL: Free the body result to prevent memory leak
                value_free(&body_result);
            }
        }
        
        // Sync variables from loop environment back to parent environment
        sync_environment_variables(loop_env, old_env);
        
        // Restore previous environment
        interpreter->current_environment = old_env;
        environment_free(loop_env);
    }
    // Handle array iteration
    else if (collection.type == VALUE_ARRAY) {
        // Create a new environment for the loop scope
        Environment* loop_env = environment_create(interpreter->current_environment);
        Environment* old_env = interpreter->current_environment;
        interpreter->current_environment = loop_env;
        
        // Iterate through the array
        for (size_t i = 0; i < collection.data.array_value.count; i++) {
            Value* element = (Value*)collection.data.array_value.elements[i];
            if (element) {
                // Define the iterator variable
                Value iterator_value = value_clone(element);
                environment_define(loop_env, node->data.for_loop.iterator_name, iterator_value);
                value_free(&iterator_value);
                
                // Execute the loop body
                if (node->data.for_loop.body) {
                    Value body_result = interpreter_execute(interpreter, node->data.for_loop.body);
                    if (interpreter_has_error(interpreter)) {
                        value_free(&body_result);
                        // Restore environment before returning
                        interpreter->current_environment = old_env;
                        environment_free(loop_env);
                        value_free(&collection);
                        return value_create_null();
                    }
                    // CRITICAL: Free the body result to prevent memory leak
                    value_free(&body_result);
                }
            }
        }
        
        // Sync variables from loop environment back to parent environment
        sync_environment_variables(loop_env, old_env);
        
        // Restore previous environment
        interpreter->current_environment = old_env;
        environment_free(loop_env);
    }
    
    value_free(&collection);
    return value_create_null();
}

Value eval_return_statement(Interpreter* interpreter, ASTNode* node) {
    // Minimal handling: evaluate value (if any), store as return_value and flag
    Value rv = value_create_null();
    if (node->data.return_statement.value) {
        rv = interpreter_execute(interpreter, node->data.return_statement.value);
    }
    interpreter->return_value = rv;
    interpreter->has_return = 1;
    return value_create_null();
}

Value eval_throw_statement(Interpreter* interpreter, ASTNode* node) {
    // Evaluate the expression to throw
    Value throw_value = value_create_null();
    if (node->data.throw_statement.value) {
        throw_value = interpreter_execute(interpreter, node->data.throw_statement.value);
    }
    
    // Convert the value to a string for the error message
    Value error_string = value_to_string(&throw_value);
    const char* error_message = error_string.type == VALUE_STRING ? error_string.data.string_value : "Unknown exception";
    
    // Set error with stack trace
    interpreter_throw_exception(interpreter, error_message, node->line, node->column);
    
    // Clean up
    value_free(&throw_value);
    value_free(&error_string);
    
    return value_create_null();
}

Value eval_try_catch(Interpreter* interpreter, ASTNode* node) {
    // Simple try/catch implementation
    // Execute try block
    if (node->data.try_catch.try_block) {
        // Set try depth for error handling
        interpreter->try_depth++;
        
        Value result = interpreter_execute(interpreter, node->data.try_catch.try_block);
        
        // If no error occurred, restore try depth and return result
        if (!interpreter->has_error) {
            interpreter->try_depth--;
            return result;
        }
        
        // Error occurred, execute catch block if it exists
        if (node->data.try_catch.catch_block) {
            // Clear error state BEFORE executing catch block
            // This allows interpreter_execute() to execute the catch block statements
            interpreter->has_error = 0;
            
            // Create catch environment that can access outer variables
            Environment* catch_env = environment_create(interpreter->current_environment);
            Environment* old_env = interpreter->current_environment;
            interpreter->current_environment = catch_env;
            
            // Bind error variable if specified
            if (node->data.try_catch.catch_variable) {
                Value error_value = value_create_string(interpreter->error_message);
                environment_define(catch_env, node->data.try_catch.catch_variable, error_value);
                value_free(&error_value);
            }
            
            // Execute catch block
            Value catch_result = interpreter_execute(interpreter, node->data.try_catch.catch_block);
            
            // Restore environment
            interpreter->current_environment = old_env;
            environment_free(catch_env);
            
            // Decrement try depth
            interpreter->try_depth--;
            
            return catch_result;
        }
        
        // No catch block, report error and propagate
        if (global_error_system) {
            // Determine error code from message
            MycoErrorCode error_code = MYCO_ERROR_UNDEFINED_VARIABLE;
            if (interpreter->error_message) {
                if (strstr(interpreter->error_message, "Division by zero")) error_code = MYCO_ERROR_DIVISION_BY_ZERO;
                else if (strstr(interpreter->error_message, "Undefined variable")) error_code = MYCO_ERROR_UNDEFINED_VARIABLE;
                else if (strstr(interpreter->error_message, "Array index out of bounds")) error_code = MYCO_ERROR_ARRAY_BOUNDS;
                else if (strstr(interpreter->error_message, "Out of memory")) error_code = MYCO_ERROR_OUT_OF_MEMORY;
                else if (strstr(interpreter->error_message, "Type mismatch")) error_code = MYCO_ERROR_TYPE_MISMATCH;
                else if (strstr(interpreter->error_message, "Null pointer")) error_code = MYCO_ERROR_NULL_POINTER;
                else if (strstr(interpreter->error_message, "Stack overflow")) error_code = MYCO_ERROR_STACK_OVERFLOW;
                else if (strstr(interpreter->error_message, "File not found")) error_code = MYCO_ERROR_FILE_NOT_FOUND;
                else if (strstr(interpreter->error_message, "Permission denied")) error_code = MYCO_ERROR_FILE_PERMISSION;
                else if (strstr(interpreter->error_message, "Network error")) error_code = MYCO_ERROR_NETWORK_ERROR;
                else if (strstr(interpreter->error_message, "Timeout")) error_code = MYCO_ERROR_TIMEOUT;
                else if (strstr(interpreter->error_message, "Syntax error")) error_code = MYCO_ERROR_UNEXPECTED_TOKEN;
                else if (strstr(interpreter->error_message, "Parse error")) error_code = MYCO_ERROR_INVALID_EXPRESSION;
                else if (strstr(interpreter->error_message, "Compilation failed")) error_code = MYCO_ERROR_COMPILATION_FAILED;
                else if (strstr(interpreter->error_message, "Not implemented")) error_code = MYCO_ERROR_UNIMPLEMENTED;
            }
            
            // Create enhanced error
            EnhancedErrorInfo* error = enhanced_error_create(
                error_code,
                enhanced_error_get_severity(error_code),
                enhanced_error_get_category(error_code),
                interpreter->error_message,
                interpreter->current_file,
                interpreter->error_line,
                interpreter->error_column
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
                
                // Report the error
                enhanced_error_report(global_error_system, error);
            }
        }
        
        interpreter->try_depth--;
        return result;
    }
    return value_create_null();
}

Value eval_block(Interpreter* interpreter, ASTNode* node) {
    // Execute all statements in the block
    for (size_t i = 0; i < node->data.block.statement_count; i++) {
        Value stmt_result = interpreter_execute(interpreter, node->data.block.statements[i]);
        if (interpreter_has_error(interpreter)) {
            value_free(&stmt_result);
            return value_create_null();
        }
        // CRITICAL: Free the statement result to prevent memory leak
        value_free(&stmt_result);
        // If a return was set, stop executing and preserve it
        if (interpreter->has_return) {
            return value_create_null(); // Block returns null, but return value is preserved in interpreter
        }
    }
    return value_create_null();
}
