#include "error_system.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <errno.h>

// Default error messages
static const char* error_messages[] = {
    [ERROR_SYNTAX_UNEXPECTED_TOKEN] = "Unexpected token: %s",
    [ERROR_SYNTAX_MISSING_TOKEN] = "Missing token: %s",
    [ERROR_SYNTAX_INVALID_EXPRESSION] = "Invalid expression",
    [ERROR_SYNTAX_UNCLOSED_STRING] = "Unclosed string literal",
    [ERROR_SYNTAX_UNCLOSED_COMMENT] = "Unclosed comment",
    [ERROR_SYNTAX_INVALID_NUMBER] = "Invalid number format",
    [ERROR_SYNTAX_INVALID_IDENTIFIER] = "Invalid identifier: %s",
    
    [ERROR_SEMANTIC_UNDEFINED_VARIABLE] = "Undefined variable: %s",
    [ERROR_SEMANTIC_UNDEFINED_FUNCTION] = "Undefined function: %s",
    [ERROR_SEMANTIC_UNDEFINED_CLASS] = "Undefined class: %s",
    [ERROR_SEMANTIC_REDECLARATION] = "Redeclaration of: %s",
    [ERROR_SEMANTIC_INVALID_OPERATION] = "Invalid operation: %s",
    [ERROR_SEMANTIC_MISSING_RETURN] = "Missing return statement",
    [ERROR_SEMANTIC_UNREACHABLE_CODE] = "Unreachable code",
    
    [ERROR_TYPE_MISMATCH] = "Type mismatch: expected %s, got %s",
    [ERROR_TYPE_INCOMPATIBLE] = "Incompatible types: %s and %s",
    [ERROR_TYPE_MISSING_ANNOTATION] = "Missing type annotation",
    [ERROR_TYPE_INVALID_CAST] = "Invalid type cast: %s to %s",
    [ERROR_TYPE_AMBIGUOUS] = "Ambiguous type: %s",
    [ERROR_TYPE_CIRCULAR_DEPENDENCY] = "Circular dependency detected",
    
    [ERROR_RUNTIME_DIVISION_BY_ZERO] = "Division by zero",
    [ERROR_RUNTIME_INDEX_OUT_OF_BOUNDS] = "Index out of bounds: %d",
    [ERROR_RUNTIME_NULL_POINTER] = "Null pointer dereference",
    [ERROR_RUNTIME_STACK_OVERFLOW] = "Stack overflow",
    [ERROR_RUNTIME_RECURSION_LIMIT] = "Recursion limit exceeded",
    [ERROR_RUNTIME_TIMEOUT] = "Operation timed out",
    [ERROR_RUNTIME_ASSERTION_FAILED] = "Assertion failed: %s",
    
    [ERROR_MEMORY_OUT_OF_MEMORY] = "Out of memory",
    [ERROR_MEMORY_LEAK] = "Memory leak detected",
    [ERROR_MEMORY_DOUBLE_FREE] = "Double free detected",
    [ERROR_MEMORY_CORRUPTION] = "Memory corruption detected",
    [ERROR_MEMORY_ACCESS_VIOLATION] = "Memory access violation",
    
    [ERROR_IO_FILE_NOT_FOUND] = "File not found: %s",
    [ERROR_IO_PERMISSION_DENIED] = "Permission denied: %s",
    [ERROR_IO_DISK_FULL] = "Disk full",
    [ERROR_IO_READ_ERROR] = "Read error: %s",
    [ERROR_IO_WRITE_ERROR] = "Write error: %s",
    [ERROR_IO_NETWORK_ERROR] = "Network error: %s",
    
    [ERROR_SYSTEM_PROCESS_FAILED] = "Process failed: %s",
    [ERROR_SYSTEM_SIGNAL_RECEIVED] = "Signal received: %d",
    [ERROR_SYSTEM_RESOURCE_EXHAUSTED] = "System resource exhausted",
    
    [ERROR_USER_DEFINED] = "User-defined error: %s"
};

// Default suggestions
static const char* error_suggestions[] = {
    [ERROR_SYNTAX_UNEXPECTED_TOKEN] = "Check syntax and ensure proper token usage",
    [ERROR_SYNTAX_MISSING_TOKEN] = "Add the missing token or check syntax",
    [ERROR_SYNTAX_INVALID_EXPRESSION] = "Review expression syntax and structure",
    [ERROR_SYNTAX_UNCLOSED_STRING] = "Close the string with a matching quote",
    [ERROR_SYNTAX_UNCLOSED_COMMENT] = "Close the comment with */",
    [ERROR_SYNTAX_INVALID_NUMBER] = "Use a valid number format",
    [ERROR_SYNTAX_INVALID_IDENTIFIER] = "Use a valid identifier name",
    
    [ERROR_SEMANTIC_UNDEFINED_VARIABLE] = "Declare the variable before use",
    [ERROR_SEMANTIC_UNDEFINED_FUNCTION] = "Define the function or check spelling",
    [ERROR_SEMANTIC_UNDEFINED_CLASS] = "Define the class or check spelling",
    [ERROR_SEMANTIC_REDECLARATION] = "Use a different name or remove duplicate declaration",
    [ERROR_SEMANTIC_INVALID_OPERATION] = "Check operation compatibility",
    [ERROR_SEMANTIC_MISSING_RETURN] = "Add a return statement or change return type to void",
    [ERROR_SEMANTIC_UNREACHABLE_CODE] = "Remove unreachable code or fix control flow",
    
    [ERROR_TYPE_MISMATCH] = "Use compatible types or add type conversion",
    [ERROR_TYPE_INCOMPATIBLE] = "Convert types or use compatible operations",
    [ERROR_TYPE_MISSING_ANNOTATION] = "Add explicit type annotation",
    [ERROR_TYPE_INVALID_CAST] = "Use valid type conversion",
    [ERROR_TYPE_AMBIGUOUS] = "Provide explicit type information",
    [ERROR_TYPE_CIRCULAR_DEPENDENCY] = "Restructure dependencies to avoid cycles",
    
    [ERROR_RUNTIME_DIVISION_BY_ZERO] = "Check divisor before division",
    [ERROR_RUNTIME_INDEX_OUT_OF_BOUNDS] = "Validate array bounds before access",
    [ERROR_RUNTIME_NULL_POINTER] = "Check for null before dereferencing",
    [ERROR_RUNTIME_STACK_OVERFLOW] = "Reduce recursion depth or use iterative approach",
    [ERROR_RUNTIME_RECURSION_LIMIT] = "Use iterative approach or increase limit",
    [ERROR_RUNTIME_TIMEOUT] = "Increase timeout or optimize operation",
    [ERROR_RUNTIME_ASSERTION_FAILED] = "Fix the condition or check assumptions",
    
    [ERROR_MEMORY_OUT_OF_MEMORY] = "Free unused memory or increase available memory",
    [ERROR_MEMORY_LEAK] = "Ensure proper memory cleanup",
    [ERROR_MEMORY_DOUBLE_FREE] = "Avoid freeing the same memory twice",
    [ERROR_MEMORY_CORRUPTION] = "Check for buffer overflows or use-after-free",
    [ERROR_MEMORY_ACCESS_VIOLATION] = "Validate memory access bounds",
    
    [ERROR_IO_FILE_NOT_FOUND] = "Check file path and permissions",
    [ERROR_IO_PERMISSION_DENIED] = "Check file permissions or run with appropriate privileges",
    [ERROR_IO_DISK_FULL] = "Free disk space or use different location",
    [ERROR_IO_READ_ERROR] = "Check file integrity and permissions",
    [ERROR_IO_WRITE_ERROR] = "Check disk space and permissions",
    [ERROR_IO_NETWORK_ERROR] = "Check network connection and configuration",
    
    [ERROR_SYSTEM_PROCESS_FAILED] = "Check system resources and permissions",
    [ERROR_SYSTEM_SIGNAL_RECEIVED] = "Handle the signal appropriately",
    [ERROR_SYSTEM_RESOURCE_EXHAUSTED] = "Free resources or increase limits",
    
    [ERROR_USER_DEFINED] = "Check error conditions and handle appropriately"
};

// Error system implementation
ErrorSystem* error_system_create(void) {
    ErrorSystem* system = shared_malloc_safe(sizeof(ErrorSystem), "error_system", "error_system_create", 0);
    if (!system) return NULL;
    
    memset(system, 0, sizeof(ErrorSystem));
    
    system->error_capacity = 100;
    system->errors = shared_malloc_safe(sizeof(ErrorInfo*) * system->error_capacity, "error_system", "error_system_create", 0);
    if (!system->errors) {
        shared_free_safe(system, "error_system", "error_system_create", 0);
        return NULL;
    }
    
    system->exception_context = exception_context_create();
    if (!system->exception_context) {
        shared_free_safe(system->errors, "error_system", "error_system_create", 0);
        shared_free_safe(system, "error_system", "error_system_create", 0);
        return NULL;
    }
    
    system->debug_mode = false;
    system->stack_trace_enabled = true;
    system->max_stack_depth = 50;
    
    return system;
}

void error_system_free(ErrorSystem* system) {
    if (!system) return;
    
    // Free all errors
    for (size_t i = 0; i < system->error_count; i++) {
        error_free(system->errors[i]);
    }
    shared_free_safe(system->errors, "error_system", "error_system_free", 0);
    
    // Free exception context
    if (system->exception_context) {
        exception_context_free(system->exception_context);
    }
    
    // Free handlers
    shared_free_safe(system->handlers, "error_system", "error_system_free", 0);
    
    // Free log file path
    shared_free_safe(system->log_file, "error_system", "error_system_free", 0);
    
    shared_free_safe(system, "error_system", "error_system_free", 0);
}

// Error creation and management
ErrorInfo* error_create(ErrorCode code, ErrorSeverity severity, ErrorCategory category, 
                       const char* message, const char* file_name, uint32_t line, uint32_t column) {
    ErrorInfo* error = shared_malloc_safe(sizeof(ErrorInfo), "error_system", "error_create", 0);
    if (!error) return NULL;
    
    memset(error, 0, sizeof(ErrorInfo));
    
    error->code = code;
    error->severity = severity;
    error->category = category;
    error->line_number = line;
    error->column_number = column;
    
    // Copy strings
    error->message = message ? (message ? strdup(message) : NULL) : NULL;
    error->file_name = file_name ? (file_name ? strdup(file_name) : NULL) : NULL;
    error->suggestion = NULL;
    error->source_line = NULL;
    error->context = NULL;
    error->user_data = NULL;
    
    // Initialize stack trace
    error->stack_trace = NULL;
    error->stack_trace_size = 0;
    
    return error;
}

void error_free(ErrorInfo* error) {
    if (!error) return;
    
    shared_free_safe(error->message, "error_system", "error_free", 0);
    shared_free_safe(error->file_name, "error_system", "error_free", 0);
    shared_free_safe(error->suggestion, "error_system", "error_free", 0);
    shared_free_safe(error->source_line, "error_system", "error_free", 0);
    shared_free_safe(error->context, "error_system", "error_free", 0);
    
    // Free stack trace
    if (error->stack_trace) {
        for (size_t i = 0; i < error->stack_trace_size; i++) {
            shared_free_safe(error->stack_trace[i].function_name, "error_system", "error_free", 0);
            shared_free_safe(error->stack_trace[i].file_name, "error_system", "error_free", 0);
            shared_free_safe(error->stack_trace[i].source_line, "error_system", "error_free", 0);
        }
        shared_free_safe(error->stack_trace, "error_system", "error_free", 0);
    }
    
    shared_free_safe(error, "error_system", "error_free", 0);
}

void error_add_context(ErrorInfo* error, const char* context) {
    if (!error || !context) return;
    
    shared_free_safe(error->context, "error_system", "error_add_context", 0);
    error->context = (context ? shared_strdup(context) : NULL);
}

void error_add_suggestion(ErrorInfo* error, const char* suggestion) {
    if (!error || !suggestion) return;
    
    shared_free_safe(error->suggestion, "error_system", "error_add_suggestion", 0);
    error->suggestion = (suggestion ? shared_strdup(suggestion) : NULL);
}

void error_add_stack_frame(ErrorInfo* error, const char* function_name, const char* file_name, 
                          uint32_t line, uint32_t column, const char* source_line) {
    if (!error) return;
    
    // Reallocate stack trace array
    error->stack_trace_size++;
    error->stack_trace = shared_realloc_safe(error->stack_trace, sizeof(StackFrame) * error->stack_trace_size, "error_system", "error_add_stack_frame", 0);
    if (!error->stack_trace) {
        error->stack_trace_size--;
        return;
    }
    
    StackFrame* frame = &error->stack_trace[error->stack_trace_size - 1];
    frame->function_name = function_name ? (function_name ? strdup(function_name) : NULL) : NULL;
    frame->file_name = file_name ? (file_name ? strdup(file_name) : NULL) : NULL;
    frame->line_number = line;
    frame->column_number = column;
    frame->source_line = source_line ? (source_line ? strdup(source_line) : NULL) : NULL;
}

// Error reporting
void error_report(ErrorSystem* system, ErrorInfo* error) {
    if (!system || !error) return;
    
    // Add to error list
    if (system->error_count >= system->error_capacity) {
        system->error_capacity *= 2;
        system->errors = shared_realloc_safe(system->errors, sizeof(ErrorInfo*) * system->error_capacity, "error_system", "error_report", 0);
        if (!system->errors) return;
    }
    
    system->errors[system->error_count++] = error;
    
    // Call registered handlers
    for (size_t i = 0; i < system->handler_count; i++) {
        if (system->handlers[i]) {
            system->handlers[i](error);
        }
    }
    
    // Print error if in debug mode
    if (system->debug_mode) {
        error_print(error);
    }
    
    // Log error
    error_log(system, error);
}

void error_print(ErrorInfo* error) {
    if (!error) return;
    
    const char* severity_names[] = {"INFO", "WARNING", "ERROR", "FATAL"};
    const char* category_names[] = {"SYNTAX", "SEMANTIC", "TYPE", "RUNTIME", 
                                   "MEMORY", "IO", "NETWORK", "SYSTEM", "USER"};
    
    printf("\n=== ERROR ===\n");
    printf("Code: %d\n", error->code);
    printf("Severity: %s\n", severity_names[error->severity]);
    printf("Category: %s\n", category_names[error->category]);
    
    if (error->message) {
        printf("Message: %s\n", error->message);
    }
    
    if (error->file_name) {
        printf("File: %s", error->file_name);
        if (error->line_number > 0) {
            printf(":%u", error->line_number);
            if (error->column_number > 0) {
                printf(":%u", error->column_number);
            }
        }
        printf("\n");
    }
    
    if (error->source_line) {
        printf("Source: %s\n", error->source_line);
        if (error->column_number > 0) {
            printf("      ");
            for (uint32_t i = 0; i < error->column_number - 1; i++) {
                printf(" ");
            }
            printf("^\n");
        }
    }
    
    if (error->suggestion) {
        printf("Suggestion: %s\n", error->suggestion);
    }
    
    if (error->context) {
        printf("Context: %s\n", error->context);
    }
    
    if (error->stack_trace_size > 0) {
        error_print_stack_trace(error);
    }
    
    printf("=============\n\n");
}

void error_print_stack_trace(ErrorInfo* error) {
    if (!error || error->stack_trace_size == 0) return;
    
    printf("\nStack Trace:\n");
    for (size_t i = 0; i < error->stack_trace_size; i++) {
        StackFrame* frame = &error->stack_trace[i];
        printf("  %zu. ", i + 1);
        
        if (frame->function_name) {
            printf("%s", frame->function_name);
        } else {
            printf("<unknown>");
        }
        
        if (frame->file_name) {
            printf(" at %s", frame->file_name);
            if (frame->line_number > 0) {
                printf(":%u", frame->line_number);
                if (frame->column_number > 0) {
                    printf(":%u", frame->column_number);
                }
            }
        }
        
        printf("\n");
        
        if (frame->source_line) {
            printf("      %s\n", frame->source_line);
        }
    }
}

void error_log(ErrorSystem* system, ErrorInfo* error) {
    if (!system || !error) return;
    
    FILE* log_file = stderr;
    if (system->log_file) {
        log_file = fopen(system->log_file, "a");
        if (!log_file) {
            log_file = stderr;
        }
    }
    
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);
    
    fprintf(log_file, "[%s] ERROR %d: %s\n", timestamp, error->code, 
            error->message ? error->message : "Unknown error");
    
    if (error->file_name) {
        fprintf(log_file, "  File: %s", error->file_name);
        if (error->line_number > 0) {
            fprintf(log_file, ":%u", error->line_number);
            if (error->column_number > 0) {
                fprintf(log_file, ":%u", error->column_number);
            }
        }
        fprintf(log_file, "\n");
    }
    
    if (error->stack_trace_size > 0) {
        fprintf(log_file, "  Stack trace:\n");
        for (size_t i = 0; i < error->stack_trace_size; i++) {
            StackFrame* frame = &error->stack_trace[i];
            fprintf(log_file, "    %zu. %s", i + 1, 
                    frame->function_name ? frame->function_name : "<unknown>");
            if (frame->file_name) {
                fprintf(log_file, " at %s", frame->file_name);
                if (frame->line_number > 0) {
                    fprintf(log_file, ":%u", frame->line_number);
                }
            }
            fprintf(log_file, "\n");
        }
    }
    
    if (system->log_file && log_file != stderr) {
        fclose(log_file);
    }
}

// Exception handling
ExceptionContext* exception_context_create(void) {
    ExceptionContext* context = shared_malloc_safe(sizeof(ExceptionContext), "error_system", "exception_context_create", 0);
    if (!context) return NULL;
    
    memset(context, 0, sizeof(ExceptionContext));
    context->try_depth = 0;
    return context;
}

void exception_context_free(ExceptionContext* context) {
    if (!context) return;
    
    if (context->current_error) {
        error_free(context->current_error);
    }
    
    shared_free_safe(context->catch_variable, "error_system", "exception_context_free", 0);
    shared_free_safe(context, "error_system", "exception_context_free", 0);
}

void exception_throw(ErrorSystem* system, ErrorInfo* error) {
    if (!system || !error) return;
    
    if (system->exception_context) {
        if (system->exception_context->current_error) {
            error_free(system->exception_context->current_error);
        }
        system->exception_context->current_error = error;
    }
    
    error_report(system, error);
}

ErrorInfo* exception_catch(ErrorSystem* system) {
    if (!system || !system->exception_context) return NULL;
    
    ErrorInfo* error = system->exception_context->current_error;
    system->exception_context->current_error = NULL;
    return error;
}

void exception_clear(ErrorSystem* system) {
    if (!system || !system->exception_context) return;
    
    if (system->exception_context->current_error) {
        error_free(system->exception_context->current_error);
        system->exception_context->current_error = NULL;
    }
}

bool exception_has_error(ErrorSystem* system) {
    return system && system->exception_context && system->exception_context->current_error;
}

// Try/catch/finally support
void exception_enter_try(ExceptionContext* context) {
    if (!context) return;
    context->in_try_block = true;
    context->try_depth++;
}

void exception_exit_try(ExceptionContext* context) {
    if (!context) return;
    context->in_try_block = false;
    if (context->try_depth > 0) {
        context->try_depth--;
    }
}

void exception_enter_catch(ExceptionContext* context, const char* variable_name) {
    if (!context) return;
    context->in_catch_block = true;
    shared_free_safe(context->catch_variable, "error_system", "exception_context_set_catch_variable", 0);
    context->catch_variable = variable_name ? (variable_name ? strdup(variable_name) : NULL) : NULL;
}

void exception_exit_catch(ExceptionContext* context) {
    if (!context) return;
    context->in_catch_block = false;
    shared_free_safe(context->catch_variable, "error_system", "exception_context_set_catch_variable", 0);
    context->catch_variable = NULL;
}

void exception_enter_finally(ExceptionContext* context) {
    if (!context) return;
    context->in_finally_block = true;
}

void exception_exit_finally(ExceptionContext* context) {
    if (!context) return;
    context->in_finally_block = false;
}

// Error handlers
void error_register_handler(ErrorSystem* system, ErrorHandler handler) {
    if (!system || !handler) return;
    
    system->handler_count++;
    system->handlers = shared_realloc_safe(system->handlers, sizeof(ErrorHandler) * system->handler_count, "error_system", "error_register_handler", 0);
    if (!system->handlers) {
        system->handler_count--;
        return;
    }
    
    system->handlers[system->handler_count - 1] = handler;
}

void error_unregister_handler(ErrorSystem* system, ErrorHandler handler) {
    if (!system || !handler) return;
    
    for (size_t i = 0; i < system->handler_count; i++) {
        if (system->handlers[i] == handler) {
            // Move remaining handlers
            for (size_t j = i; j < system->handler_count - 1; j++) {
                system->handlers[j] = system->handlers[j + 1];
            }
            system->handler_count--;
            break;
        }
    }
}

// Utility functions
const char* error_get_message(ErrorCode code) {
    if (code >= 0 && code < sizeof(error_messages) / sizeof(error_messages[0])) {
        return error_messages[code];
    }
    return "Unknown error";
}

const char* error_get_suggestion(ErrorCode code) {
    if (code >= 0 && code < sizeof(error_suggestions) / sizeof(error_suggestions[0])) {
        return error_suggestions[code];
    }
    return "No suggestion available";
}

ErrorSeverity error_get_severity(ErrorCode code) {
    if (code >= ERROR_SYNTAX_UNEXPECTED_TOKEN && code <= ERROR_SYNTAX_INVALID_IDENTIFIER) {
        return ERROR_SEVERITY_ERROR;
    }
    if (code >= ERROR_SEMANTIC_UNDEFINED_VARIABLE && code <= ERROR_SEMANTIC_UNREACHABLE_CODE) {
        return ERROR_SEVERITY_ERROR;
    }
    if (code >= ERROR_TYPE_MISMATCH && code <= ERROR_TYPE_CIRCULAR_DEPENDENCY) {
        return ERROR_SEVERITY_ERROR;
    }
    if (code >= ERROR_RUNTIME_DIVISION_BY_ZERO && code <= ERROR_RUNTIME_ASSERTION_FAILED) {
        return ERROR_SEVERITY_ERROR;
    }
    if (code >= ERROR_MEMORY_OUT_OF_MEMORY && code <= ERROR_MEMORY_ACCESS_VIOLATION) {
        return ERROR_SEVERITY_FATAL;
    }
    if (code >= ERROR_IO_FILE_NOT_FOUND && code <= ERROR_IO_NETWORK_ERROR) {
        return ERROR_SEVERITY_ERROR;
    }
    if (code >= ERROR_SYSTEM_PROCESS_FAILED && code <= ERROR_SYSTEM_RESOURCE_EXHAUSTED) {
        return ERROR_SEVERITY_FATAL;
    }
    return ERROR_SEVERITY_ERROR;
}

ErrorCategory error_get_category(ErrorCode code) {
    if (code >= ERROR_SYNTAX_UNEXPECTED_TOKEN && code <= ERROR_SYNTAX_INVALID_IDENTIFIER) {
        return ERROR_CATEGORY_SYNTAX;
    }
    if (code >= ERROR_SEMANTIC_UNDEFINED_VARIABLE && code <= ERROR_SEMANTIC_UNREACHABLE_CODE) {
        return ERROR_CATEGORY_SEMANTIC;
    }
    if (code >= ERROR_TYPE_MISMATCH && code <= ERROR_TYPE_CIRCULAR_DEPENDENCY) {
        return ERROR_CATEGORY_TYPE;
    }
    if (code >= ERROR_RUNTIME_DIVISION_BY_ZERO && code <= ERROR_RUNTIME_ASSERTION_FAILED) {
        return ERROR_CATEGORY_RUNTIME;
    }
    if (code >= ERROR_MEMORY_OUT_OF_MEMORY && code <= ERROR_MEMORY_ACCESS_VIOLATION) {
        return ERROR_CATEGORY_MEMORY;
    }
    if (code >= ERROR_IO_FILE_NOT_FOUND && code <= ERROR_IO_NETWORK_ERROR) {
        return ERROR_CATEGORY_IO;
    }
    if (code >= ERROR_SYSTEM_PROCESS_FAILED && code <= ERROR_SYSTEM_RESOURCE_EXHAUSTED) {
        return ERROR_CATEGORY_SYSTEM;
    }
    return ERROR_CATEGORY_USER;
}

bool error_is_recoverable(ErrorCode code) {
    return code < ERROR_MEMORY_OUT_OF_MEMORY;
}

bool error_should_abort(ErrorCode code) {
    return code >= ERROR_MEMORY_OUT_OF_MEMORY || code >= ERROR_SYSTEM_PROCESS_FAILED;
}

// Debugging support
void error_enable_debug_mode(ErrorSystem* system, bool enable) {
    if (system) {
        system->debug_mode = enable;
    }
}

void error_enable_stack_trace(ErrorSystem* system, bool enable) {
    if (system) {
        system->stack_trace_enabled = enable;
    }
}

void error_set_max_stack_depth(ErrorSystem* system, size_t max_depth) {
    if (system) {
        system->max_stack_depth = max_depth;
    }
}

void error_set_log_file(ErrorSystem* system, const char* log_file) {
    if (!system) return;
    
    shared_free_safe(system->log_file, "error_system", "error_system_set_log_file", 0);
    system->log_file = log_file ? (log_file ? shared_strdup(log_file) : NULL) : NULL;
}

// Assertion system
void error_assert(ErrorSystem* system, bool condition, const char* message, 
                 const char* file_name, uint32_t line) {
    if (!condition) {
        ErrorInfo* error = error_create(ERROR_RUNTIME_ASSERTION_FAILED, ERROR_SEVERITY_ERROR, 
                                       ERROR_CATEGORY_RUNTIME, message, file_name, line, 0);
        if (error) {
            exception_throw(system, error);
        }
    }
}

void error_assert_equals(ErrorSystem* system, const void* expected, const void* actual, 
                        const char* message, const char* file_name, uint32_t line) {
    if (expected != actual) {
        char* full_message = shared_malloc_safe(256, "error_system", "error_assert", 0);
        if (full_message) {
            snprintf(full_message, 256, "%s: expected %p, got %p", message, expected, actual);
            ErrorInfo* error = error_create(ERROR_RUNTIME_ASSERTION_FAILED, ERROR_SEVERITY_ERROR, 
                                           ERROR_CATEGORY_RUNTIME, full_message, file_name, line, 0);
            if (error) {
                exception_throw(system, error);
            }
            shared_free_safe(full_message, "error_system", "error_assert_equals", 0);
        }
    }
}

void error_assert_not_null(ErrorSystem* system, const void* ptr, const char* message, 
                          const char* file_name, uint32_t line) {
    if (!ptr) {
        ErrorInfo* error = error_create(ERROR_RUNTIME_NULL_POINTER, ERROR_SEVERITY_ERROR, 
                                       ERROR_CATEGORY_RUNTIME, message, file_name, line, 0);
        if (error) {
            exception_throw(system, error);
        }
    }
}
