#include "enhanced_error_system.h"
#include "shared_utilities.h"
#include "interpreter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <errno.h>

// ============================================================================
// GLOBAL ERROR SYSTEM INSTANCE
// ============================================================================

static EnhancedErrorSystem* global_error_system = NULL;

// ============================================================================
// ERROR MESSAGE DATABASE
// ============================================================================

typedef struct {
    MycoErrorCode code;
    const char* message;
    const char* suggestion;
    ErrorSeverity severity;
    ErrorCategory category;
} ErrorDatabaseEntry;

static const ErrorDatabaseEntry error_database[] = {
    // Lexical errors (1000-1999)
    {MYCO_ERROR_UNKNOWN_TOKEN, "Unknown token encountered", "Check for typos or unsupported characters", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_LEXICAL},
    {MYCO_ERROR_UNTERMINATED_STRING, "Unterminated string literal", "Add closing quote to string", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_LEXICAL},
    {MYCO_ERROR_UNTERMINATED_COMMENT, "Unterminated comment", "Add comment closing marker", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_LEXICAL},
    {MYCO_ERROR_INVALID_NUMBER, "Invalid number format", "Check number syntax and format", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_LEXICAL},
    {MYCO_ERROR_INVALID_ESCAPE, "Invalid escape sequence", "Use valid escape sequences", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_LEXICAL},
    {MYCO_ERROR_INVALID_CHARACTER, "Invalid character", "Use valid characters for the language", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_LEXICAL},
    {MYCO_ERROR_EOF_IN_STRING, "Unexpected end of file in string", "Close string with matching quote", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_LEXICAL},
    {MYCO_ERROR_EOF_IN_COMMENT, "Unexpected end of file in comment", "Close comment with */", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_LEXICAL},
    
    // Syntax errors (2000-2999)
    {MYCO_ERROR_UNEXPECTED_TOKEN, "Unexpected token", "Check syntax and token placement", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_SYNTAX},
    {MYCO_ERROR_MISSING_SEMICOLON, "Missing semicolon", "Add semicolon at end of statement", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_SYNTAX},
    {MYCO_ERROR_MISSING_PARENTHESIS, "Missing parenthesis", "Add matching parenthesis", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_SYNTAX},
    {MYCO_ERROR_MISSING_BRACE, "Missing brace", "Add matching brace", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_SYNTAX},
    {MYCO_ERROR_MISSING_BRACKET, "Missing bracket", "Add matching bracket", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_SYNTAX},
    {MYCO_ERROR_INVALID_EXPRESSION, "Invalid expression", "Check expression syntax", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_SYNTAX},
    {MYCO_ERROR_INVALID_STATEMENT, "Invalid statement", "Check statement syntax", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_SYNTAX},
    {MYCO_ERROR_INVALID_FUNCTION, "Invalid function definition", "Check function syntax", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_SYNTAX},
    {MYCO_ERROR_INVALID_CLASS, "Invalid class definition", "Check class syntax", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_SYNTAX},
    {MYCO_ERROR_INVALID_IMPORT, "Invalid import statement", "Check import syntax", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_SYNTAX},
    {MYCO_ERROR_INVALID_MATCH, "Invalid match statement", "Check match syntax", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_SYNTAX},
    {MYCO_ERROR_INVALID_LOOP, "Invalid loop statement", "Check loop syntax", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_SYNTAX},
    {MYCO_ERROR_INVALID_CONDITION, "Invalid condition", "Check condition syntax", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_SYNTAX},
    
    // Semantic errors (3000-3999)
    {MYCO_ERROR_UNDEFINED_VARIABLE, "Undefined variable", "Declare variable before use", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_SEMANTIC},
    {MYCO_ERROR_UNDEFINED_FUNCTION, "Undefined function", "Define function before use", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_SEMANTIC},
    {MYCO_ERROR_UNDEFINED_CLASS, "Undefined class", "Define class before use", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_SEMANTIC},
    {MYCO_ERROR_TYPE_MISMATCH, "Type mismatch", "Use compatible types", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_SEMANTIC},
    {MYCO_ERROR_ARGUMENT_COUNT, "Incorrect argument count", "Provide correct number of arguments", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_SEMANTIC},
    {MYCO_ERROR_INVALID_ARGUMENT, "Invalid argument", "Check argument types and values", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_SEMANTIC},
    {MYCO_ERROR_DUPLICATE_DECLARATION, "Duplicate declaration", "Remove duplicate or rename", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_SEMANTIC},
    {MYCO_ERROR_INVALID_OPERATION, "Invalid operation", "Check operation compatibility", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_SEMANTIC},
    {MYCO_ERROR_INVALID_CAST, "Invalid type cast", "Use valid type conversion", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_SEMANTIC},
    {MYCO_ERROR_INVALID_ACCESS, "Invalid access", "Check access permissions", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_SEMANTIC},
    {MYCO_ERROR_SCOPE_VIOLATION, "Scope violation", "Check variable scope", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_SEMANTIC},
    {MYCO_ERROR_CONSTANT_REASSIGNMENT, "Constant reassignment", "Cannot reassign constant", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_SEMANTIC},
    {MYCO_ERROR_READONLY_MODIFICATION, "Readonly modification", "Cannot modify readonly value", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_SEMANTIC},
    
    // Runtime errors (4000-4999)
    {MYCO_ERROR_DIVISION_BY_ZERO, "Division by zero", "Check divisor before division", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_RUNTIME},
    {MYCO_ERROR_ARRAY_BOUNDS, "Array index out of bounds", "Check array bounds", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_RUNTIME},
    {MYCO_ERROR_NULL_POINTER, "Null pointer access", "Check for null before use", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_RUNTIME},
    {MYCO_ERROR_STACK_OVERFLOW, "Stack overflow", "Reduce recursion depth", ERROR_SEVERITY_CRITICAL, ERROR_CATEGORY_RUNTIME},
    {MYCO_ERROR_STACK_UNDERFLOW, "Stack underflow", "Check stack before pop", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_RUNTIME},
    {MYCO_ERROR_INVALID_INDEX, "Invalid index", "Use valid index values", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_RUNTIME},
    {MYCO_ERROR_INVALID_KEY, "Invalid key", "Use valid key values", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_RUNTIME},
    {MYCO_ERROR_INVALID_RUNTIME_OPERATION, "Invalid runtime operation", "Check operation validity", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_RUNTIME},
    {MYCO_ERROR_INVALID_STATE, "Invalid state", "Check object state", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_RUNTIME},
    {MYCO_ERROR_INVALID_CONTEXT, "Invalid context", "Check execution context", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_RUNTIME},
    {MYCO_ERROR_RECURSION_LIMIT, "Recursion limit exceeded", "Use iterative approach", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_RUNTIME},
    {MYCO_ERROR_TIMEOUT, "Operation timeout", "Increase timeout or optimize", ERROR_SEVERITY_WARNING, ERROR_CATEGORY_RUNTIME},
    {MYCO_ERROR_ASSERTION_FAILED, "Assertion failed", "Fix assertion condition", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_RUNTIME},
    
    // Memory errors (5000-5999)
    {MYCO_ERROR_OUT_OF_MEMORY, "Out of memory", "Free memory or increase available", ERROR_SEVERITY_CRITICAL, ERROR_CATEGORY_MEMORY},
    {MYCO_ERROR_MEMORY_CORRUPTION, "Memory corruption", "Check for buffer overflows", ERROR_SEVERITY_CRITICAL, ERROR_CATEGORY_MEMORY},
    {MYCO_ERROR_DOUBLE_FREE, "Double free", "Avoid freeing same memory twice", ERROR_SEVERITY_CRITICAL, ERROR_CATEGORY_MEMORY},
    {MYCO_ERROR_MEMORY_LEAK, "Memory leak", "Ensure proper cleanup", ERROR_SEVERITY_WARNING, ERROR_CATEGORY_MEMORY},
    {MYCO_ERROR_INVALID_POINTER, "Invalid pointer", "Check pointer validity", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_MEMORY},
    {MYCO_ERROR_BUFFER_OVERFLOW, "Buffer overflow", "Check buffer bounds", ERROR_SEVERITY_CRITICAL, ERROR_CATEGORY_MEMORY},
    {MYCO_ERROR_BUFFER_UNDERFLOW, "Buffer underflow", "Check buffer bounds", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_MEMORY},
    {MYCO_ERROR_INVALID_SIZE, "Invalid size", "Use valid size values", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_MEMORY},
    {MYCO_ERROR_INVALID_ALIGNMENT, "Invalid alignment", "Use proper alignment", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_MEMORY},
    {MYCO_ERROR_MEMORY_EXHAUSTED, "Memory exhausted", "Free memory or reduce usage", ERROR_SEVERITY_CRITICAL, ERROR_CATEGORY_MEMORY},
    {MYCO_ERROR_USE_AFTER_FREE, "Use after free", "Avoid using freed memory", ERROR_SEVERITY_CRITICAL, ERROR_CATEGORY_MEMORY},
    {MYCO_ERROR_WILD_POINTER, "Wild pointer", "Initialize pointers properly", ERROR_SEVERITY_CRITICAL, ERROR_CATEGORY_MEMORY},
    
    // I/O errors (6000-6999)
    {MYCO_ERROR_FILE_NOT_FOUND, "File not found", "Check file path", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_IO},
    {MYCO_ERROR_FILE_ACCESS, "File access denied", "Check file permissions", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_IO},
    {MYCO_ERROR_FILE_PERMISSION, "Permission denied", "Check file permissions", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_IO},
    {MYCO_ERROR_FILE_CORRUPTION, "File corruption", "Check file integrity", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_IO},
    {MYCO_ERROR_DISK_FULL, "Disk full", "Free disk space", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_IO},
    {MYCO_ERROR_READ_ERROR, "Read error", "Check file and permissions", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_IO},
    {MYCO_ERROR_WRITE_ERROR, "Write error", "Check disk space and permissions", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_IO},
    {MYCO_ERROR_SEEK_ERROR, "Seek error", "Check file position", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_IO},
    {MYCO_ERROR_FLUSH_ERROR, "Flush error", "Check file state", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_IO},
    
    // Network errors (7000-7999)
    {MYCO_ERROR_NETWORK_ERROR, "Network error", "Check network connection", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_NETWORK},
    {MYCO_ERROR_CONNECTION_FAILED, "Connection failed", "Check network and server", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_NETWORK},
    {MYCO_ERROR_CONNECTION_TIMEOUT, "Connection timeout", "Check network speed", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_NETWORK},
    {MYCO_ERROR_CONNECTION_REFUSED, "Connection refused", "Check server availability", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_NETWORK},
    {MYCO_ERROR_DNS_FAILURE, "DNS failure", "Check DNS configuration", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_NETWORK},
    {MYCO_ERROR_SSL_ERROR, "SSL error", "Check SSL configuration", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_NETWORK},
    {MYCO_ERROR_PROTOCOL_ERROR, "Protocol error", "Check protocol compatibility", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_NETWORK},
    
    // System errors (8000-8999)
    {MYCO_ERROR_SYSTEM_ERROR, "System error", "Check system resources", ERROR_SEVERITY_CRITICAL, ERROR_CATEGORY_SYSTEM},
    {MYCO_ERROR_PROCESS_FAILED, "Process failed", "Check system resources", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_SYSTEM},
    {MYCO_ERROR_SIGNAL_RECEIVED, "Signal received", "Handle signal appropriately", ERROR_SEVERITY_WARNING, ERROR_CATEGORY_SYSTEM},
    {MYCO_ERROR_RESOURCE_EXHAUSTED, "Resource exhausted", "Free resources", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_SYSTEM},
    {MYCO_ERROR_PLATFORM_ERROR, "Platform error", "Check platform compatibility", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_SYSTEM},
    {MYCO_ERROR_EXTERNAL_ERROR, "External error", "Check external dependencies", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_SYSTEM},
    {MYCO_ERROR_CONFIGURATION_ERROR, "Configuration error", "Check configuration", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_SYSTEM},
    {MYCO_ERROR_DEPENDENCY_MISSING, "Missing dependency", "Install dependencies", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_SYSTEM},
    
    // Compilation errors (9000-9999)
    {MYCO_ERROR_COMPILATION_FAILED, "Compilation failed", "Check source code", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_SYSTEM},
    {MYCO_ERROR_OPTIMIZATION_FAILED, "Optimization failed", "Disable optimization", ERROR_SEVERITY_WARNING, ERROR_CATEGORY_SYSTEM},
    {MYCO_ERROR_CODE_GENERATION_FAILED, "Code generation failed", "Check target architecture", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_SYSTEM},
    {MYCO_ERROR_LINKING_FAILED, "Linking failed", "Check libraries", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_SYSTEM},
    {MYCO_ERROR_ASSEMBLY_FAILED, "Assembly failed", "Check assembly code", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_SYSTEM},
    {MYCO_ERROR_TARGET_NOT_SUPPORTED, "Target not supported", "Use supported target", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_SYSTEM},
    {MYCO_ERROR_INVALID_TARGET, "Invalid target", "Use valid target", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_SYSTEM},
    {MYCO_ERROR_COMPILER_BUG, "Compiler bug", "Report to developers", ERROR_SEVERITY_CRITICAL, ERROR_CATEGORY_SYSTEM},
    {MYCO_ERROR_INTERNAL_ERROR, "Internal error", "Report as bug", ERROR_SEVERITY_CRITICAL, ERROR_CATEGORY_SYSTEM},
    {MYCO_ERROR_UNIMPLEMENTED, "Not implemented", "Use alternative approach", ERROR_SEVERITY_WARNING, ERROR_CATEGORY_SYSTEM},
    
    // User-defined errors (10000+)
    {MYCO_ERROR_USER_DEFINED, "User-defined error", "Check error conditions", ERROR_SEVERITY_ERROR, ERROR_CATEGORY_USER},
    
    // Terminator
    {0, NULL, NULL, ERROR_SEVERITY_INFO, ERROR_CATEGORY_USER}
};

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

static const ErrorDatabaseEntry* find_error_entry(MycoErrorCode code) {
    for (const ErrorDatabaseEntry* entry = error_database; entry->code != 0; entry++) {
        if (entry->code == code) {
            return entry;
        }
    }
    return NULL;
}

// ============================================================================
// GLOBAL SYSTEM MANAGEMENT
// ============================================================================

EnhancedErrorSystem* enhanced_error_system_get_global(void) {
    if (!global_error_system) {
        global_error_system = enhanced_error_system_create();
    }
    return global_error_system;
}

void enhanced_error_system_initialize_global(void) {
    if (!global_error_system) {
        global_error_system = enhanced_error_system_create();
    }
}

void enhanced_error_system_cleanup_global(void) {
    if (global_error_system) {
        enhanced_error_system_free(global_error_system);
        global_error_system = NULL;
    }
}

static uint64_t get_current_timestamp(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

static const char* get_severity_color(ErrorSeverity severity) {
    switch (severity) {
        case ERROR_SEVERITY_INFO:     return "\x1b[36m"; // Cyan
        case ERROR_SEVERITY_WARNING:  return "\x1b[33m"; // Yellow
        case ERROR_SEVERITY_ERROR:    return "\x1b[31m"; // Red
        case ERROR_SEVERITY_CRITICAL: return "\x1b[35m"; // Magenta
        case ERROR_SEVERITY_FATAL:    return "\x1b[41m\x1b[37m"; // White on red
        default: return "";
    }
}

static const char* get_severity_icon(ErrorSeverity severity) {
    switch (severity) {
        case ERROR_SEVERITY_INFO:     return "[INFO]";
        case ERROR_SEVERITY_WARNING:  return "[WARNING]";
        case ERROR_SEVERITY_ERROR:    return "[ERROR]";
        case ERROR_SEVERITY_CRITICAL: return "[CRITICAL]";
        case ERROR_SEVERITY_FATAL:    return "[FATAL]";
        default: return "[UNKNOWN]";
    }
}

// ============================================================================
// SYSTEM INITIALIZATION AND CLEANUP
// ============================================================================

EnhancedErrorSystem* enhanced_error_system_create(void) {
    EnhancedErrorSystem* system = malloc(sizeof(EnhancedErrorSystem));
    if (!system) return NULL;
    
    memset(system, 0, sizeof(EnhancedErrorSystem));
    
    // Initialize configuration with defaults
    system->config.colors_enabled = true;
    system->config.verbose_mode = false;
    system->config.stack_trace_enabled = true;
    system->config.timing_enabled = true;
    system->config.context_enabled = true;
    system->config.suggestions_enabled = true;
    system->config.auto_recovery_enabled = false;
    system->config.max_stack_depth = 50;
    system->config.max_error_count = 1000;
    system->config.max_context_length = 1024;
    system->config.log_file = NULL;
    system->config.component_filter = NULL;
    system->config.error_handler = NULL;
    system->config.warning_handler = NULL;
    system->config.critical_handler = NULL;
    
    // Initialize error storage
    system->error_capacity = 100;
    system->errors = malloc(sizeof(EnhancedErrorInfo*) * system->error_capacity);
    if (!system->errors) {
        free(system);
        return NULL;
    }
    
    // Initialize statistics
    system->start_time = get_current_timestamp();
    system->last_error_time = 0;
    system->in_recovery_mode = false;
    system->recovery_attempts = 0;
    
    return system;
}

void enhanced_error_system_free(EnhancedErrorSystem* system) {
    if (!system) return;
    
    // Free all errors
    for (size_t i = 0; i < system->error_count; i++) {
        enhanced_error_free(system->errors[i]);
    }
    free(system->errors);
    
    // Free configuration strings
    free(system->config.log_file);
    free(system->config.component_filter);
    
    free(system);
}

void enhanced_error_system_configure(EnhancedErrorSystem* system, const ErrorSystemConfig* config) {
    if (!system || !config) return;
    
    system->config = *config;
    
    // Copy strings
    free(system->config.log_file);
    system->config.log_file = config->log_file ? strdup(config->log_file) : NULL;
    
    free(system->config.component_filter);
    system->config.component_filter = config->component_filter ? strdup(config->component_filter) : NULL;
}

// ============================================================================
// ERROR CREATION AND MANAGEMENT
// ============================================================================

EnhancedErrorInfo* enhanced_error_create(MycoErrorCode code, ErrorSeverity severity, 
                                        ErrorCategory category, const char* message,
                                        const char* file_name, uint32_t line, uint32_t column) {
    EnhancedErrorInfo* error = malloc(sizeof(EnhancedErrorInfo));
    if (!error) return NULL;
    
    memset(error, 0, sizeof(EnhancedErrorInfo));
    
    error->code = code;
    error->severity = severity;
    error->category = category;
    error->line_number = line;
    error->column_number = column;
    error->timestamp = get_current_timestamp();
    error->execution_time = 0;
    
    // Copy strings
    error->message = message ? strdup(message) : NULL;
    error->file_name = file_name ? strdup(file_name) : NULL;
    error->source_line = NULL;
    error->suggestion = NULL;
    error->context = NULL;
    error->variable_context = NULL;
    error->user_data = NULL;
    
    // Initialize stack trace
    error->stack_trace = NULL;
    error->stack_trace_size = 0;
    error->stack_trace_capacity = 0;
    
    // Initialize related errors
    error->related_errors = NULL;
    error->related_count = 0;
    error->cause = NULL;
    error->next = NULL;
    
    return error;
}

void enhanced_error_free(EnhancedErrorInfo* error) {
    if (!error) return;
    
    // Free strings
    free(error->message);
    free(error->file_name);
    free(error->source_line);
    free(error->suggestion);
    free(error->context);
    free(error->variable_context);
    
    // Free stack trace
    if (error->stack_trace) {
        for (size_t i = 0; i < error->stack_trace_size; i++) {
            free(error->stack_trace[i].function_name);
            free(error->stack_trace[i].file_name);
            free(error->stack_trace[i].source_line);
            free(error->stack_trace[i].context_info);
        }
        free(error->stack_trace);
    }
    
    // Free related errors
    if (error->related_errors) {
        for (size_t i = 0; i < error->related_count; i++) {
            enhanced_error_free(&error->related_errors[i]);
        }
        free(error->related_errors);
    }
    
    // Free error chain
    if (error->cause) {
        enhanced_error_free(error->cause);
    }
    if (error->next) {
        enhanced_error_free(error->next);
    }
    
    free(error);
}

// ============================================================================
// ERROR REPORTING
// ============================================================================

void enhanced_error_report(EnhancedErrorSystem* system, EnhancedErrorInfo* error) {
    if (!system || !error) return;
    
    // Check if we should filter this error
    if (system->config.component_filter && error->file_name) {
        if (strstr(error->file_name, system->config.component_filter) == NULL) {
            return;
        }
    }
    
    // Add to error list
    if (system->error_count >= system->error_capacity) {
        system->error_capacity *= 2;
        system->errors = realloc(system->errors, sizeof(EnhancedErrorInfo*) * system->error_capacity);
        if (!system->errors) return;
    }
    
    system->errors[system->error_count++] = error;
    
    // Update statistics
    system->total_errors++;
    if (error->severity < 5) {
        system->error_counts[error->severity]++;
    }
    if (error->category < 10) {
        system->category_counts[error->category]++;
    }
    system->last_error_time = error->timestamp;
    
    // Call appropriate handler
    switch (error->severity) {
        case ERROR_SEVERITY_WARNING:
            if (system->config.warning_handler) {
                system->config.warning_handler(error);
            }
            break;
        case ERROR_SEVERITY_CRITICAL:
        case ERROR_SEVERITY_FATAL:
            if (system->config.critical_handler) {
                system->config.critical_handler(error);
            }
            break;
        default:
            if (system->config.error_handler) {
                system->config.error_handler(error);
            }
            break;
    }
    
    // Print error
    enhanced_error_print(error);
    
    // Log error if configured
    if (system->config.log_file) {
        FILE* log_file = fopen(system->config.log_file, "a");
        if (log_file) {
            char* formatted = enhanced_error_format(error);
            if (formatted) {
                fprintf(log_file, "%s\n", formatted);
                free(formatted);
            }
            fclose(log_file);
        }
    }
}

void enhanced_error_report_simple(EnhancedErrorSystem* system, MycoErrorCode code, 
                                 const char* message, const char* file_name, 
                                 uint32_t line, uint32_t column) {
    const ErrorDatabaseEntry* entry = find_error_entry(code);
    ErrorSeverity severity = entry ? entry->severity : ERROR_SEVERITY_ERROR;
    ErrorCategory category = entry ? entry->category : ERROR_CATEGORY_USER;
    
    EnhancedErrorInfo* error = enhanced_error_create(code, severity, category, message, file_name, line, column);
    if (error) {
        if (entry && entry->suggestion) {
            enhanced_error_add_suggestion(error, entry->suggestion);
        }
        enhanced_error_report(system, error);
    }
}

// ============================================================================
// ERROR FORMATTING AND DISPLAY
// ============================================================================

void enhanced_error_print(const EnhancedErrorInfo* error) {
    if (!error) return;
    
    const char* severity_color = get_severity_color(error->severity);
    const char* reset_color = "\x1b[0m";
    const char* severity_name = enhanced_error_get_severity_name(error->severity);
    const char* category_name = enhanced_error_get_category_name(error->category);
    const char* icon = get_severity_icon(error->severity);
    
    // Print clean, concise error message (1-2 lines max)
    if (error->file_name) {
        printf("%s%s E%u %s%s%s in %s:%u", 
               severity_color, icon, error->code, severity_color, 
               error->message ? error->message : "Unknown error", reset_color,
               error->file_name, error->line_number);
        if (error->column_number > 0) {
            printf(":%u", error->column_number);
        }
    } else {
        printf("%s%s E%u %s%s%s", 
               severity_color, icon, error->code, severity_color, 
               error->message ? error->message : "Unknown error", reset_color);
    }
    printf("\n");
    
    // Only show hint if available and not too verbose
    if (error->suggestion && strlen(error->suggestion) < 100) {
        printf("  %sHint:%s %s\n", severity_color, reset_color, error->suggestion);
    }
    
    // Print stack trace if available
    if (error->stack_trace_size > 0) {
        enhanced_error_print_stack_trace(error);
    }
    
    // Print timing if available
    if (error->execution_time > 0) {
        printf("  %s⏱️  Time:%s %llu ns\n", "\x1b[2m", reset_color, 
               (unsigned long long)error->execution_time);
    }
}

void enhanced_error_print_compact(const EnhancedErrorInfo* error) {
    if (!error) return;
    
    const char* severity_color = get_severity_color(error->severity);
    const char* reset_color = "\x1b[0m";
    const char* icon = get_severity_icon(error->severity);
    
    printf("%s%s E%d:%s %s", 
           severity_color, icon, error->code, reset_color,
           error->message ? error->message : "Unknown error");
    
    if (error->file_name && error->line_number > 0) {
        printf(" (%s:%u)", error->file_name, error->line_number);
    }
    
    printf("\n");
}

void enhanced_error_print_verbose(const EnhancedErrorInfo* error) {
    if (!error) return;
    
    printf("\n=== ENHANCED ERROR REPORT ===\n");
    printf("Code: %d\n", error->code);
    printf("Severity: %s\n", enhanced_error_get_severity_name(error->severity));
    printf("Category: %s\n", enhanced_error_get_category_name(error->category));
    
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
    
    if (error->context) {
        printf("Context: %s\n", error->context);
    }
    
    if (error->variable_context) {
        printf("Variables: %s\n", error->variable_context);
    }
    
    if (error->suggestion) {
        printf("Suggestion: %s\n", error->suggestion);
    }
    
    if (error->stack_trace_size > 0) {
        enhanced_error_print_stack_trace(error);
    }
    
    if (error->execution_time > 0) {
        printf("Execution Time: %llu ns\n", (unsigned long long)error->execution_time);
    }
    
    printf("Timestamp: %llu\n", (unsigned long long)error->timestamp);
    printf("=============================\n\n");
}

char* enhanced_error_format(const EnhancedErrorInfo* error) {
    if (!error) return NULL;
    
    char* result = malloc(1024);
    if (!result) return NULL;
    
    snprintf(result, 1024, "[%s] E%d: %s (%s:%u:%u)",
             enhanced_error_get_severity_name(error->severity),
             error->code,
             error->message ? error->message : "Unknown error",
             error->file_name ? error->file_name : "unknown",
             error->line_number,
             error->column_number);
    
    return result;
}

char* enhanced_error_format_json(const EnhancedErrorInfo* error) {
    if (!error) return NULL;
    
    char* result = malloc(2048);
    if (!result) return NULL;
    
    snprintf(result, 2048,
             "{\n"
             "  \"code\": %d,\n"
             "  \"severity\": \"%s\",\n"
             "  \"category\": \"%s\",\n"
             "  \"message\": \"%s\",\n"
             "  \"file\": \"%s\",\n"
             "  \"line\": %u,\n"
             "  \"column\": %u,\n"
             "  \"timestamp\": %llu\n"
             "}",
             error->code,
             enhanced_error_get_severity_name(error->severity),
             enhanced_error_get_category_name(error->category),
             error->message ? error->message : "Unknown error",
             error->file_name ? error->file_name : "unknown",
             error->line_number,
             error->column_number,
             (unsigned long long)error->timestamp);
    
    return result;
}

// ============================================================================
// STACK TRACE MANAGEMENT
// ============================================================================

void enhanced_error_add_stack_frame(EnhancedErrorInfo* error, const char* function_name,
                                   const char* file_name, uint32_t line, uint32_t column,
                                   const char* source_line, const char* context_info) {
    if (!error) return;
    
    // Reallocate stack trace array if needed
    if (error->stack_trace_size >= error->stack_trace_capacity) {
        error->stack_trace_capacity = error->stack_trace_capacity ? error->stack_trace_capacity * 2 : 10;
        error->stack_trace = realloc(error->stack_trace, sizeof(StackFrame) * error->stack_trace_capacity);
        if (!error->stack_trace) return;
    }
    
    StackFrame* frame = &error->stack_trace[error->stack_trace_size];
    frame->function_name = function_name ? strdup(function_name) : NULL;
    frame->file_name = file_name ? strdup(file_name) : NULL;
    frame->line_number = line;
    frame->column_number = column;
    frame->source_line = source_line ? strdup(source_line) : NULL;
    frame->context_info = context_info ? strdup(context_info) : NULL;
    
    error->stack_trace_size++;
}

void enhanced_error_print_stack_trace(const EnhancedErrorInfo* error) {
    if (!error || error->stack_trace_size == 0) return;
    
    printf("  %s📚 Stack Trace:%s\n", "\x1b[2m", "\x1b[0m");
    for (size_t i = 0; i < error->stack_trace_size; i++) {
        StackFrame* frame = &error->stack_trace[i];
        printf("    %zu. ", i + 1);
        
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
            printf("        %s\n", frame->source_line);
        }
        
        if (frame->context_info) {
            printf("        %sContext: %s%s\n", "\x1b[2m", frame->context_info, "\x1b[0m");
        }
    }
}

// ============================================================================
// CONTEXT AND SUGGESTIONS
// ============================================================================

void enhanced_error_add_context(EnhancedErrorInfo* error, const char* context) {
    if (!error || !context) return;
    
    free(error->context);
    error->context = strdup(context);
}

void enhanced_error_add_suggestion(EnhancedErrorInfo* error, const char* suggestion) {
    if (!error || !suggestion) return;
    
    free(error->suggestion);
    error->suggestion = strdup(suggestion);
}

void enhanced_error_add_variable_context(EnhancedErrorInfo* error, const char* variable_context) {
    if (!error || !variable_context) return;
    
    free(error->variable_context);
    error->variable_context = strdup(variable_context);
}

void enhanced_error_add_source_line(EnhancedErrorInfo* error, const char* source_line) {
    if (!error || !source_line) return;
    
    free(error->source_line);
    error->source_line = strdup(source_line);
}

// ============================================================================
// ERROR INFORMATION RETRIEVAL
// ============================================================================

const char* enhanced_error_get_message(MycoErrorCode code) {
    const ErrorDatabaseEntry* entry = find_error_entry(code);
    return entry ? entry->message : NULL;
}

const char* enhanced_error_get_suggestion(MycoErrorCode code) {
    const ErrorDatabaseEntry* entry = find_error_entry(code);
    return entry ? entry->suggestion : NULL;
}

ErrorSeverity enhanced_error_get_severity(MycoErrorCode code) {
    const ErrorDatabaseEntry* entry = find_error_entry(code);
    return entry ? entry->severity : ERROR_SEVERITY_ERROR;
}

ErrorCategory enhanced_error_get_category(MycoErrorCode code) {
    const ErrorDatabaseEntry* entry = find_error_entry(code);
    return entry ? entry->category : ERROR_CATEGORY_USER;
}

const char* enhanced_error_get_category_name(ErrorCategory category) {
    switch (category) {
        case ERROR_CATEGORY_LEXICAL:  return "Lexical";
        case ERROR_CATEGORY_SYNTAX:   return "Syntax";
        case ERROR_CATEGORY_SEMANTIC: return "Semantic";
        case ERROR_CATEGORY_TYPE:     return "Type";
        case ERROR_CATEGORY_RUNTIME:  return "Runtime";
        case ERROR_CATEGORY_MEMORY:   return "Memory";
        case ERROR_CATEGORY_IO:       return "I/O";
        case ERROR_CATEGORY_NETWORK:  return "Network";
        case ERROR_CATEGORY_SYSTEM:   return "System";
        case ERROR_CATEGORY_USER:     return "User";
        default: return "Unknown";
    }
}

const char* enhanced_error_get_severity_name(ErrorSeverity severity) {
    switch (severity) {
        case ERROR_SEVERITY_INFO:     return "INFO";
        case ERROR_SEVERITY_WARNING:  return "WARNING";
        case ERROR_SEVERITY_ERROR:    return "ERROR";
        case ERROR_SEVERITY_CRITICAL: return "CRITICAL";
        case ERROR_SEVERITY_FATAL:    return "FATAL";
        default: return "UNKNOWN";
    }
}

// ============================================================================
// ERROR SYSTEM UTILITIES
// ============================================================================

void enhanced_error_system_reset(EnhancedErrorSystem* system) {
    if (!system) return;
    
    // Clear all errors
    for (size_t i = 0; i < system->error_count; i++) {
        enhanced_error_free(system->errors[i]);
    }
    system->error_count = 0;
    
    // Reset statistics
    system->total_errors = 0;
    memset(system->error_counts, 0, sizeof(system->error_counts));
    memset(system->category_counts, 0, sizeof(system->category_counts));
    
    // Reset recovery state
    system->in_recovery_mode = false;
    system->recovery_attempts = 0;
}

void enhanced_error_system_clear_errors(EnhancedErrorSystem* system) {
    enhanced_error_system_reset(system);
}

size_t enhanced_error_system_get_error_count(EnhancedErrorSystem* system) {
    return system ? system->error_count : 0;
}

EnhancedErrorInfo* enhanced_error_system_get_last_error(EnhancedErrorSystem* system) {
    if (!system || system->error_count == 0) return NULL;
    return system->errors[system->error_count - 1];
}

EnhancedErrorInfo* enhanced_error_system_get_error(EnhancedErrorSystem* system, size_t index) {
    if (!system || index >= system->error_count) return NULL;
    return system->errors[index];
}

// ============================================================================
// ERROR FILTERING AND SEARCHING
// ============================================================================

EnhancedErrorInfo** enhanced_error_system_filter_by_severity(EnhancedErrorSystem* system, 
                                                           ErrorSeverity severity, size_t* count) {
    if (!system || !count) return NULL;
    
    *count = 0;
    EnhancedErrorInfo** result = malloc(sizeof(EnhancedErrorInfo*) * system->error_count);
    if (!result) return NULL;
    
    for (size_t i = 0; i < system->error_count; i++) {
        if (system->errors[i]->severity == severity) {
            result[(*count)++] = system->errors[i];
        }
    }
    
    return result;
}

EnhancedErrorInfo** enhanced_error_system_filter_by_category(EnhancedErrorSystem* system, 
                                                           ErrorCategory category, size_t* count) {
    if (!system || !count) return NULL;
    
    *count = 0;
    EnhancedErrorInfo** result = malloc(sizeof(EnhancedErrorInfo*) * system->error_count);
    if (!result) return NULL;
    
    for (size_t i = 0; i < system->error_count; i++) {
        if (system->errors[i]->category == category) {
            result[(*count)++] = system->errors[i];
        }
    }
    
    return result;
}

EnhancedErrorInfo** enhanced_error_system_filter_by_file(EnhancedErrorSystem* system, 
                                                        const char* file_name, size_t* count) {
    if (!system || !file_name || !count) return NULL;
    
    *count = 0;
    EnhancedErrorInfo** result = malloc(sizeof(EnhancedErrorInfo*) * system->error_count);
    if (!result) return NULL;
    
    for (size_t i = 0; i < system->error_count; i++) {
        if (system->errors[i]->file_name && strcmp(system->errors[i]->file_name, file_name) == 0) {
            result[(*count)++] = system->errors[i];
        }
    }
    
    return result;
}

// ============================================================================
// DEBUGGING AND ANALYSIS
// ============================================================================

void enhanced_error_system_print_statistics(EnhancedErrorSystem* system) {
    if (!system) return;
    
    printf("\n=== ERROR SYSTEM STATISTICS ===\n");
    printf("Total Errors: %llu\n", (unsigned long long)system->total_errors);
    printf("Current Errors: %zu\n", system->error_count);
    
    printf("\nBy Severity:\n");
    printf("  INFO:     %llu\n", (unsigned long long)system->error_counts[ERROR_SEVERITY_INFO]);
    printf("  WARNING:  %llu\n", (unsigned long long)system->error_counts[ERROR_SEVERITY_WARNING]);
    printf("  ERROR:    %llu\n", (unsigned long long)system->error_counts[ERROR_SEVERITY_ERROR]);
    printf("  CRITICAL: %llu\n", (unsigned long long)system->error_counts[ERROR_SEVERITY_CRITICAL]);
    printf("  FATAL:    %llu\n", (unsigned long long)system->error_counts[ERROR_SEVERITY_FATAL]);
    
    printf("\nBy Category:\n");
    printf("  Lexical:  %llu\n", (unsigned long long)system->category_counts[ERROR_CATEGORY_LEXICAL]);
    printf("  Syntax:   %llu\n", (unsigned long long)system->category_counts[ERROR_CATEGORY_SYNTAX]);
    printf("  Semantic: %llu\n", (unsigned long long)system->category_counts[ERROR_CATEGORY_SEMANTIC]);
    printf("  Type:     %llu\n", (unsigned long long)system->category_counts[ERROR_CATEGORY_TYPE]);
    printf("  Runtime:  %llu\n", (unsigned long long)system->category_counts[ERROR_CATEGORY_RUNTIME]);
    printf("  Memory:   %llu\n", (unsigned long long)system->category_counts[ERROR_CATEGORY_MEMORY]);
    printf("  I/O:      %llu\n", (unsigned long long)system->category_counts[ERROR_CATEGORY_IO]);
    printf("  Network:  %llu\n", (unsigned long long)system->category_counts[ERROR_CATEGORY_NETWORK]);
    printf("  System:   %llu\n", (unsigned long long)system->category_counts[ERROR_CATEGORY_SYSTEM]);
    printf("  User:     %llu\n", (unsigned long long)system->category_counts[ERROR_CATEGORY_USER]);
    
    if (system->last_error_time > 0) {
        uint64_t current_time = get_current_timestamp();
        uint64_t time_since_last = current_time - system->last_error_time;
        printf("\nTime since last error: %llu ns\n", (unsigned long long)time_since_last);
    }
    
    printf("===============================\n\n");
}

void enhanced_error_system_print_summary(EnhancedErrorSystem* system) {
    if (!system) return;
    
    printf("Error Summary: %zu errors, %llu total\n", 
           system->error_count, (unsigned long long)system->total_errors);
    
    if (system->error_count > 0) {
        EnhancedErrorInfo* last_error = enhanced_error_system_get_last_error(system);
        if (last_error) {
            printf("Last error: E%d - %s\n", last_error->code, 
                   last_error->message ? last_error->message : "Unknown");
        }
    }
}

void enhanced_error_system_export_log(EnhancedErrorSystem* system, const char* filename) {
    if (!system || !filename) return;
    
    FILE* file = fopen(filename, "w");
    if (!file) return;
    
    fprintf(file, "Myco Error Log\n");
    fprintf(file, "==============\n\n");
    
    for (size_t i = 0; i < system->error_count; i++) {
        EnhancedErrorInfo* error = system->errors[i];
        fprintf(file, "Error %zu:\n", i + 1);
        fprintf(file, "  Code: %d\n", error->code);
        fprintf(file, "  Severity: %s\n", enhanced_error_get_severity_name(error->severity));
        fprintf(file, "  Category: %s\n", enhanced_error_get_category_name(error->category));
        fprintf(file, "  Message: %s\n", error->message ? error->message : "Unknown");
        if (error->file_name) {
            fprintf(file, "  File: %s:%u:%u\n", error->file_name, error->line_number, error->column_number);
        }
        if (error->suggestion) {
            fprintf(file, "  Suggestion: %s\n", error->suggestion);
        }
        fprintf(file, "  Timestamp: %llu\n", (unsigned long long)error->timestamp);
        fprintf(file, "\n");
    }
    
    fclose(file);
}

// ============================================================================
// SAFE OPERATIONS WITH ERROR HANDLING
// ============================================================================

void* enhanced_safe_malloc(size_t size, EnhancedErrorSystem* system, const char* context) {
    void* ptr = malloc(size);
    if (!ptr && system) {
        enhanced_error_report_simple(system, MYCO_ERROR_OUT_OF_MEMORY, 
                                   "Memory allocation failed", __FILE__, __LINE__, 0);
    }
    return ptr;
}

char* enhanced_safe_strdup(const char* str, EnhancedErrorSystem* system, const char* context) {
    if (!str) {
        if (system) {
            enhanced_error_report_simple(system, MYCO_ERROR_NULL_POINTER, 
                                       "Null string pointer", __FILE__, __LINE__, 0);
        }
        return NULL;
    }
    
    char* result = strdup(str);
    if (!result && system) {
        enhanced_error_report_simple(system, MYCO_ERROR_OUT_OF_MEMORY, 
                                   "String duplication failed", __FILE__, __LINE__, 0);
    }
    return result;
}

FILE* enhanced_safe_fopen(const char* filename, const char* mode, EnhancedErrorSystem* system) {
    if (!filename) {
        if (system) {
            enhanced_error_report_simple(system, MYCO_ERROR_NULL_POINTER, 
                                       "Null filename", __FILE__, __LINE__, 0);
        }
        return NULL;
    }
    
    FILE* file = fopen(filename, mode);
    if (!file && system) {
        MycoErrorCode error_code = MYCO_ERROR_FILE_NOT_FOUND;
        const char* error_msg = "File not found";
        
        if (errno == EACCES) {
            error_code = MYCO_ERROR_FILE_PERMISSION;
            error_msg = "Permission denied";
        } else if (errno == ENOSPC) {
            error_code = MYCO_ERROR_DISK_FULL;
            error_msg = "No space left on device";
        }
        
        enhanced_error_report_simple(system, error_code, error_msg, __FILE__, __LINE__, 0);
    }
    return file;
}

int enhanced_safe_divide(int a, int b, EnhancedErrorSystem* system, const char* context) {
    if (b == 0) {
        if (system) {
            enhanced_error_report_simple(system, MYCO_ERROR_DIVISION_BY_ZERO, 
                                       "Division by zero", __FILE__, __LINE__, 0);
        }
        return 0;
    }
    return a / b;
}

// ============================================================================
// ERROR RECOVERY MECHANISMS IMPLEMENTATION
// ============================================================================

bool enhanced_error_add_recovery_rule(EnhancedErrorSystem* system, const ErrorRecoveryRule* rule) {
    if (!system || !rule) return false;
    
    // Check if recovery rules array needs to be initialized
    if (!system->recovery_rules) {
        system->recovery_rules = malloc(sizeof(ErrorRecoveryRule) * 100);
        if (!system->recovery_rules) return false;
        system->recovery_rules_capacity = 100;
        system->recovery_rules_count = 0;
    }
    
    // Check if we need to expand the array
    if (system->recovery_rules_count >= system->recovery_rules_capacity) {
        ErrorRecoveryRule* new_rules = realloc(system->recovery_rules, 
                                              sizeof(ErrorRecoveryRule) * system->recovery_rules_capacity * 2);
        if (!new_rules) return false;
        system->recovery_rules = new_rules;
        system->recovery_rules_capacity *= 2;
    }
    
    // Add the rule
    system->recovery_rules[system->recovery_rules_count] = *rule;
    system->recovery_rules_count++;
    
    return true;
}

bool enhanced_error_remove_recovery_rule(EnhancedErrorSystem* system, MycoErrorCode error_code) {
    if (!system || !system->recovery_rules) return false;
    
    for (size_t i = 0; i < system->recovery_rules_count; i++) {
        if (system->recovery_rules[i].error_code == error_code) {
            // Free any allocated strings
            free(system->recovery_rules[i].fallback_message);
            
            // Move remaining rules up
            for (size_t j = i; j < system->recovery_rules_count - 1; j++) {
                system->recovery_rules[j] = system->recovery_rules[j + 1];
            }
            system->recovery_rules_count--;
            return true;
        }
    }
    
    return false;
}

const ErrorRecoveryRule* enhanced_error_get_recovery_rule(EnhancedErrorSystem* system, MycoErrorCode error_code) {
    if (!system || !system->recovery_rules) return NULL;
    
    for (size_t i = 0; i < system->recovery_rules_count; i++) {
        if (system->recovery_rules[i].error_code == error_code) {
            return &system->recovery_rules[i];
        }
    }
    
    return NULL;
}

bool enhanced_error_attempt_recovery(EnhancedErrorSystem* system, const EnhancedErrorInfo* error, void* context) {
    if (!system || !error) return false;
    
    const ErrorRecoveryRule* rule = enhanced_error_get_recovery_rule(system, error->code);
    if (!rule) return false;
    
    switch (rule->strategy) {
        case RECOVERY_STRATEGY_RETRY:
            // Implement retry logic
            if (rule->recovery_function) {
                return rule->recovery_function(context, rule->recovery_data);
            }
            break;
            
        case RECOVERY_STRATEGY_FALLBACK:
            // Implement fallback logic
            if (rule->fallback_message) {
                printf("Fallback: %s\n", rule->fallback_message);
                return true;
            }
            break;
            
        case RECOVERY_STRATEGY_IGNORE:
            // Ignore the error
            return true;
            
        case RECOVERY_STRATEGY_ABORT:
            // Abort execution
            return false;
            
        case RECOVERY_STRATEGY_RECOVER:
            // Attempt recovery
            if (rule->recovery_function) {
                return rule->recovery_function(context, rule->recovery_data);
            }
            break;
            
        case RECOVERY_STRATEGY_CONTINUE:
            // Continue execution
            return true;
            
        default:
            return false;
    }
    
    return false;
}

// ============================================================================
// ENHANCED ERROR CONTEXT IMPLEMENTATION
// ============================================================================

EnhancedErrorContext* enhanced_error_context_create(void) {
    EnhancedErrorContext* context = malloc(sizeof(EnhancedErrorContext));
    if (!context) return NULL;
    
    memset(context, 0, sizeof(EnhancedErrorContext));
    
    // Initialize arrays
    context->variables = malloc(sizeof(VariableContext) * 50);
    context->functions = malloc(sizeof(FunctionContext) * 50);
    context->files = malloc(sizeof(FileContext) * 10);
    
    if (!context->variables || !context->functions || !context->files) {
        enhanced_error_context_free(context);
        return NULL;
    }
    
    context->variable_count = 0;
    context->function_count = 0;
    context->file_count = 0;
    
    return context;
}

void enhanced_error_context_free(EnhancedErrorContext* context) {
    if (!context) return;
    
    // Free variables
    for (size_t i = 0; i < context->variable_count; i++) {
        free(context->variables[i].variable_name);
        free(context->variables[i].variable_type);
        free(context->variables[i].variable_value);
        free(context->variables[i].scope_info);
        free(context->variables[i].memory_address);
        free(context->variables[i].last_assignment);
    }
    free(context->variables);
    
    // Free functions
    for (size_t i = 0; i < context->function_count; i++) {
        free(context->functions[i].function_name);
        free(context->functions[i].function_signature);
        free(context->functions[i].parameter_types);
        free(context->functions[i].return_type);
        free(context->functions[i].call_stack);
    }
    free(context->functions);
    
    // Free files
    for (size_t i = 0; i < context->file_count; i++) {
        free(context->files[i].file_path);
        free(context->files[i].file_content);
        free(context->files[i].file_encoding);
        free(context->files[i].file_permissions);
    }
    free(context->files);
    
    // Free info strings
    free(context->system_info);
    free(context->memory_info);
    free(context->performance_info);
    
    free(context);
}

bool enhanced_error_context_add_variable(EnhancedErrorContext* context, const char* var_name, 
                                        const char* var_type, const char* var_value, const char* scope_info) {
    if (!context || !var_name) return false;
    
    if (context->variable_count >= 50) return false; // Simple limit
    
    VariableContext* var = &context->variables[context->variable_count];
    var->variable_name = strdup(var_name);
    var->variable_type = var_type ? strdup(var_type) : NULL;
    var->variable_value = var_value ? strdup(var_value) : NULL;
    var->scope_info = scope_info ? strdup(scope_info) : NULL;
    var->memory_address = NULL; // Could be filled in later
    var->last_assignment = NULL; // Could be filled in later
    
    context->variable_count++;
    return true;
}

bool enhanced_error_context_add_function(EnhancedErrorContext* context, const char* func_name,
                                        const char* func_signature, const char* param_types,
                                        const char* return_type, const char* call_stack) {
    if (!context || !func_name) return false;
    
    if (context->function_count >= 50) return false; // Simple limit
    
    FunctionContext* func = &context->functions[context->function_count];
    func->function_name = strdup(func_name);
    func->function_signature = func_signature ? strdup(func_signature) : NULL;
    func->parameter_types = param_types ? strdup(param_types) : NULL;
    func->return_type = return_type ? strdup(return_type) : NULL;
    func->call_stack = call_stack ? strdup(call_stack) : NULL;
    func->recursion_depth = 0; // Could be calculated
    
    context->function_count++;
    return true;
}

bool enhanced_error_context_add_file(EnhancedErrorContext* context, const char* file_path,
                                    const char* file_content, const char* file_encoding) {
    if (!context || !file_path) return false;
    
    if (context->file_count >= 10) return false; // Simple limit
    
    FileContext* file = &context->files[context->file_count];
    file->file_path = strdup(file_path);
    file->file_content = file_content ? strdup(file_content) : NULL;
    file->file_encoding = file_encoding ? strdup(file_encoding) : NULL;
    file->file_size = file_content ? strlen(file_content) : 0;
    file->file_modified = time(NULL);
    file->file_permissions = NULL; // Could be filled in later
    
    context->file_count++;
    return true;
}

bool enhanced_error_context_set_system_info(EnhancedErrorContext* context, const char* system_info) {
    if (!context) return false;
    
    free(context->system_info);
    context->system_info = system_info ? strdup(system_info) : NULL;
    return true;
}

bool enhanced_error_context_set_memory_info(EnhancedErrorContext* context, const char* memory_info) {
    if (!context) return false;
    
    free(context->memory_info);
    context->memory_info = memory_info ? strdup(memory_info) : NULL;
    return true;
}

bool enhanced_error_context_set_performance_info(EnhancedErrorContext* context, const char* performance_info) {
    if (!context) return false;
    
    free(context->performance_info);
    context->performance_info = performance_info ? strdup(performance_info) : NULL;
    return true;
}

// ============================================================================
// ENHANCED ERROR REPORTING WITH CONTEXT IMPLEMENTATION
// ============================================================================

bool enhanced_error_report_with_context(EnhancedErrorSystem* system, MycoErrorCode error_code,
                                       const char* message, const char* file_name,
                                       uint32_t line, uint32_t column, const EnhancedErrorContext* context) {
    if (!system) return false;
    
    // Create error with basic info
    EnhancedErrorInfo* error = enhanced_error_create(error_code, ERROR_SEVERITY_ERROR, 
                                                    ERROR_CATEGORY_RUNTIME, message, file_name, line, column);
    if (!error) return false;
    
    // Add context information to error
    if (context) {
        // Add variable context
        if (context->variable_count > 0) {
            char* var_info = malloc(1024);
            if (var_info) {
                snprintf(var_info, 1024, "Variables in scope: %zu", context->variable_count);
                enhanced_error_add_context(error, var_info);
                free(var_info);
            }
        }
        
        // Add function context
        if (context->function_count > 0) {
            char* func_info = malloc(1024);
            if (func_info) {
                snprintf(func_info, 1024, "Functions in call stack: %zu", context->function_count);
                enhanced_error_add_context(error, func_info);
                free(func_info);
            }
        }
        
        // Add system info
        if (context->system_info) {
            enhanced_error_add_context(error, context->system_info);
        }
        
        // Add memory info
        if (context->memory_info) {
            enhanced_error_add_context(error, context->memory_info);
        }
    }
    
    // Report the error
    enhanced_error_report(system, error);
    return true;
}

char* enhanced_error_get_detailed_analysis(const EnhancedErrorInfo* error, const EnhancedErrorContext* context) {
    if (!error) return NULL;
    
    char* analysis = malloc(2048);
    if (!analysis) return NULL;
    
    snprintf(analysis, 2048, 
             "Error Analysis:\n"
             "Code: %d\n"
             "Severity: %s\n"
             "Category: %s\n"
             "Message: %s\n"
             "Location: %s:%u:%u\n",
             error->code,
             enhanced_error_get_severity_name(error->severity),
             enhanced_error_get_category_name(error->category),
             error->message ? error->message : "Unknown",
             error->file_name ? error->file_name : "unknown",
             error->line_number,
             error->column_number);
    
    if (context) {
        char* context_info = malloc(1024);
        if (context_info) {
            snprintf(context_info, 1024,
                     "Context Information:\n"
                     "Variables: %zu\n"
                     "Functions: %zu\n"
                     "Files: %zu\n",
                     context->variable_count,
                     context->function_count,
                     context->file_count);
            
            // Append context info to analysis
            char* new_analysis = malloc(strlen(analysis) + strlen(context_info) + 1);
            if (new_analysis) {
                strcpy(new_analysis, analysis);
                strcat(new_analysis, context_info);
                free(analysis);
                analysis = new_analysis;
            }
            free(context_info);
        }
    }
    
    return analysis;
}

char* enhanced_error_get_contextual_suggestions(const EnhancedErrorInfo* error, const EnhancedErrorContext* context) {
    if (!error) return NULL;
    
    char* suggestions = malloc(1024);
    if (!suggestions) return NULL;
    
    // Basic suggestions based on error code
    switch (error->code) {
        case MYCO_ERROR_UNDEFINED_VARIABLE:
            snprintf(suggestions, 1024, "Suggestions:\n"
                     "1. Check if the variable is declared before use\n"
                     "2. Verify the variable name spelling\n"
                     "3. Check variable scope\n"
                     "4. Consider if the variable should be imported\n");
            break;
            
        case MYCO_ERROR_TYPE_MISMATCH:
            snprintf(suggestions, 1024, "Suggestions:\n"
                     "1. Check variable types match the expected type\n"
                     "2. Use explicit type conversion if needed\n"
                     "3. Verify function parameter types\n"
                     "4. Check return type compatibility\n");
            break;
            
        case MYCO_ERROR_DIVISION_BY_ZERO:
            snprintf(suggestions, 1024, "Suggestions:\n"
                     "1. Add a check for zero before division\n"
                     "2. Use conditional logic to handle zero case\n"
                     "3. Consider using a default value\n"
                     "4. Validate input parameters\n");
            break;
            
        default:
            snprintf(suggestions, 1024, "Suggestions:\n"
                     "1. Check the error message for details\n"
                     "2. Verify input parameters\n"
                     "3. Check for null pointers\n"
                     "4. Review the code logic\n");
            break;
    }
    
    // Add context-specific suggestions
    if (context && context->variable_count > 0) {
        char* context_suggestions = malloc(512);
        if (context_suggestions) {
            snprintf(context_suggestions, 512,
                     "\nContext-specific suggestions:\n"
                     "Consider checking the %zu variables in scope\n"
                     "Review the %zu functions in the call stack\n",
                     context->variable_count,
                     context->function_count);
            
            // Append context suggestions
            char* new_suggestions = malloc(strlen(suggestions) + strlen(context_suggestions) + 1);
            if (new_suggestions) {
                strcpy(new_suggestions, suggestions);
                strcat(new_suggestions, context_suggestions);
                free(suggestions);
                suggestions = new_suggestions;
            }
            free(context_suggestions);
        }
    }
    
    return suggestions;
}

// ============================================================================
// COMPATIBILITY FUNCTIONS FOR OLD ERROR SYSTEM
// ============================================================================

// Global state for compatibility
static int error_colors_enabled = 1;
static int stack_traces_enabled = 1;

void error_colors_enable(int enable) {
    error_colors_enabled = enable;
}

void error_enable_stack_trace(EnhancedErrorSystem* system, bool enable) {
    stack_traces_enabled = enable;
    // Note: EnhancedErrorSystem doesn't have a stack_trace_enabled field
    // This is handled by the global state for compatibility
}

void error_print_last(void) {
    EnhancedErrorSystem* system = enhanced_error_system_get_global();
    if (system) {
        EnhancedErrorInfo* last_error = enhanced_error_system_get_last_error(system);
        if (last_error) {
            if (error_colors_enabled) {
                printf("\033[31m"); // Red color
            }
            printf("Error: %s\n", last_error->message);
            if (last_error->suggestion) {
                printf("Suggestion: %s\n", last_error->suggestion);
            }
            if (error_colors_enabled) {
                printf("\033[0m"); // Reset color
            }
        } else {
            printf("No errors recorded.\n");
        }
    }
}

// Legacy function name for compatibility
ErrorSystem* error_system_get_global(void) {
    // Return the enhanced error system cast as the old type for compatibility
    return (ErrorSystem*)enhanced_error_system_get_global();
}

/**
 * @brief Enhanced error reporting for interpreter (compatibility function)
 * @param interpreter The interpreter instance
 * @param message The error message
 * @param line The line number
 * @param column The column number
 */
void interpreter_report_error_enhanced(struct Interpreter* interpreter, const char* message, int line, int column) {
    if (!interpreter || !message) return;
    
    EnhancedErrorSystem* system = enhanced_error_system_get_global();
    if (!system) return;
    
    // Map message to error code
    MycoErrorCode error_code = MYCO_ERROR_INVALID_OPERATION;
    if (strstr(message, "undefined variable") || strstr(message, "undefined function")) {
        error_code = MYCO_ERROR_UNDEFINED_VARIABLE;
    } else if (strstr(message, "type mismatch")) {
        error_code = MYCO_ERROR_TYPE_MISMATCH;
    } else if (strstr(message, "division by zero")) {
        error_code = MYCO_ERROR_DIVISION_BY_ZERO;
    } else if (strstr(message, "out of memory")) {
        error_code = MYCO_ERROR_OUT_OF_MEMORY;
    } else if (strstr(message, "array bounds")) {
        error_code = MYCO_ERROR_ARRAY_BOUNDS;
    }
    
    // Report the error
    enhanced_error_report_simple(system, error_code, message, 
                               "unknown",  // TODO: Get filename from interpreter context
                               line, column);
}

