#ifndef ERROR_SYSTEM_H
#define ERROR_SYSTEM_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// Error severity levels
typedef enum {
    ERROR_SEVERITY_INFO = 0,
    ERROR_SEVERITY_WARNING = 1,
    ERROR_SEVERITY_ERROR = 2,
    ERROR_SEVERITY_FATAL = 3
} ErrorSeverity;

// Error categories
typedef enum {
    ERROR_CATEGORY_SYNTAX = 0,
    ERROR_CATEGORY_SEMANTIC = 1,
    ERROR_CATEGORY_TYPE = 2,
    ERROR_CATEGORY_RUNTIME = 3,
    ERROR_CATEGORY_MEMORY = 4,
    ERROR_CATEGORY_IO = 5,
    ERROR_CATEGORY_NETWORK = 6,
    ERROR_CATEGORY_SYSTEM = 7,
    ERROR_CATEGORY_USER = 8
} ErrorCategory;

// Error codes
typedef enum {
    // Syntax errors (1000-1999)
    ERROR_SYNTAX_UNEXPECTED_TOKEN = 1001,
    ERROR_SYNTAX_MISSING_TOKEN = 1002,
    ERROR_SYNTAX_INVALID_EXPRESSION = 1003,
    ERROR_SYNTAX_UNCLOSED_STRING = 1004,
    ERROR_SYNTAX_UNCLOSED_COMMENT = 1005,
    ERROR_SYNTAX_INVALID_NUMBER = 1006,
    ERROR_SYNTAX_INVALID_IDENTIFIER = 1007,
    
    // Semantic errors (2000-2999)
    ERROR_SEMANTIC_UNDEFINED_VARIABLE = 2001,
    ERROR_SEMANTIC_UNDEFINED_FUNCTION = 2002,
    ERROR_SEMANTIC_UNDEFINED_CLASS = 2003,
    ERROR_SEMANTIC_REDECLARATION = 2004,
    ERROR_SEMANTIC_INVALID_OPERATION = 2005,
    ERROR_SEMANTIC_MISSING_RETURN = 2006,
    ERROR_SEMANTIC_UNREACHABLE_CODE = 2007,
    
    // Type errors (3000-3999)
    ERROR_TYPE_MISMATCH = 3001,
    ERROR_TYPE_INCOMPATIBLE = 3002,
    ERROR_TYPE_MISSING_ANNOTATION = 3003,
    ERROR_TYPE_INVALID_CAST = 3004,
    ERROR_TYPE_AMBIGUOUS = 3005,
    ERROR_TYPE_CIRCULAR_DEPENDENCY = 3006,
    
    // Runtime errors (4000-4999)
    ERROR_RUNTIME_DIVISION_BY_ZERO = 4001,
    ERROR_RUNTIME_INDEX_OUT_OF_BOUNDS = 4002,
    ERROR_RUNTIME_NULL_POINTER = 4003,
    ERROR_RUNTIME_STACK_OVERFLOW = 4004,
    ERROR_RUNTIME_RECURSION_LIMIT = 4005,
    ERROR_RUNTIME_TIMEOUT = 4006,
    ERROR_RUNTIME_ASSERTION_FAILED = 4007,
    
    // Memory errors (5000-5999)
    ERROR_MEMORY_OUT_OF_MEMORY = 5001,
    ERROR_MEMORY_LEAK = 5002,
    ERROR_MEMORY_DOUBLE_FREE = 5003,
    ERROR_MEMORY_CORRUPTION = 5004,
    ERROR_MEMORY_ACCESS_VIOLATION = 5005,
    
    // I/O errors (6000-6999)
    ERROR_IO_FILE_NOT_FOUND = 6001,
    ERROR_IO_PERMISSION_DENIED = 6002,
    ERROR_IO_DISK_FULL = 6003,
    ERROR_IO_READ_ERROR = 6004,
    ERROR_IO_WRITE_ERROR = 6005,
    ERROR_IO_NETWORK_ERROR = 6006,
    
    // System errors (7000-7999)
    ERROR_SYSTEM_PROCESS_FAILED = 7001,
    ERROR_SYSTEM_SIGNAL_RECEIVED = 7002,
    ERROR_SYSTEM_RESOURCE_EXHAUSTED = 7003,
    
    // User-defined errors (8000-8999)
    ERROR_USER_DEFINED = 8001
} ErrorCode;

// Stack frame for debugging
typedef struct {
    char* function_name;
    char* file_name;
    uint32_t line_number;
    uint32_t column_number;
    char* source_line;
} StackFrame;

// Error information
typedef struct {
    ErrorCode code;
    ErrorSeverity severity;
    ErrorCategory category;
    char* message;
    char* suggestion;
    char* file_name;
    uint32_t line_number;
    uint32_t column_number;
    char* source_line;
    StackFrame* stack_trace;
    size_t stack_trace_size;
    char* context;
    void* user_data;
} ErrorInfo;

// Exception handling context
typedef struct {
    ErrorInfo* current_error;
    bool in_try_block;
    bool in_catch_block;
    bool in_finally_block;
    size_t try_depth;
    char* catch_variable;
    void* exception_handler;
} ExceptionContext;

// Error handler function type
typedef void (*ErrorHandler)(const ErrorInfo* error);

// Error system context
typedef struct {
    ErrorInfo** errors;
    size_t error_count;
    size_t error_capacity;
    ExceptionContext* exception_context;
    ErrorHandler* handlers;
    size_t handler_count;
    bool debug_mode;
    bool stack_trace_enabled;
    size_t max_stack_depth;
    char* log_file;
} ErrorSystem;

// Error system functions
ErrorSystem* error_system_create(void);
void error_system_free(ErrorSystem* system);

// Error creation and management
ErrorInfo* error_create(ErrorCode code, ErrorSeverity severity, ErrorCategory category, 
                       const char* message, const char* file_name, uint32_t line, uint32_t column);
void error_free(ErrorInfo* error);
void error_add_context(ErrorInfo* error, const char* context);
void error_add_suggestion(ErrorInfo* error, const char* suggestion);
void error_add_stack_frame(ErrorInfo* error, const char* function_name, const char* file_name, 
                          uint32_t line, uint32_t column, const char* source_line);

// Error reporting
void error_report(ErrorSystem* system, ErrorInfo* error);
void error_print(ErrorInfo* error);
void error_print_stack_trace(ErrorInfo* error);
void error_log(ErrorSystem* system, ErrorInfo* error);

// Exception handling
ExceptionContext* exception_context_create(void);
void exception_context_free(ExceptionContext* context);
void exception_throw(ErrorSystem* system, ErrorInfo* error);
ErrorInfo* exception_catch(ErrorSystem* system);
void exception_clear(ErrorSystem* system);
bool exception_has_error(ErrorSystem* system);

// Try/catch/finally support
void exception_enter_try(ExceptionContext* context);
void exception_exit_try(ExceptionContext* context);
void exception_enter_catch(ExceptionContext* context, const char* variable_name);
void exception_exit_catch(ExceptionContext* context);
void exception_enter_finally(ExceptionContext* context);
void exception_exit_finally(ExceptionContext* context);

// Error handlers
void error_register_handler(ErrorSystem* system, ErrorHandler handler);
void error_unregister_handler(ErrorSystem* system, ErrorHandler handler);
void error_set_default_handler(ErrorSystem* system, ErrorHandler handler);

// Utility functions
const char* error_get_message(ErrorCode code);
const char* error_get_suggestion(ErrorCode code);
ErrorSeverity error_get_severity(ErrorCode code);
ErrorCategory error_get_category(ErrorCode code);
bool error_is_recoverable(ErrorCode code);
bool error_should_abort(ErrorCode code);

// Debugging support
void error_enable_debug_mode(ErrorSystem* system, bool enable);
void error_enable_stack_trace(ErrorSystem* system, bool enable);
void error_set_max_stack_depth(ErrorSystem* system, size_t max_depth);
void error_set_log_file(ErrorSystem* system, const char* log_file);

// Assertion system
void error_assert(ErrorSystem* system, bool condition, const char* message, 
                 const char* file_name, uint32_t line);
void error_assert_equals(ErrorSystem* system, const void* expected, const void* actual, 
                        const char* message, const char* file_name, uint32_t line);
void error_assert_not_null(ErrorSystem* system, const void* ptr, const char* message, 
                          const char* file_name, uint32_t line);

// Macros for easy error handling
#define ERROR_CREATE(code, message) \
    error_create(code, error_get_severity(code), error_get_category(code), \
                 message, __FILE__, __LINE__, 0)

#define ERROR_THROW(system, code, message) \
    do { \
        ErrorInfo* _error = ERROR_CREATE(code, message); \
        exception_throw(system, _error); \
    } while(0)

#define ERROR_ASSERT(system, condition, message) \
    error_assert(system, condition, message, __FILE__, __LINE__)

#define ERROR_ASSERT_EQUALS(system, expected, actual, message) \
    error_assert_equals(system, expected, actual, message, __FILE__, __LINE__)

#define ERROR_ASSERT_NOT_NULL(system, ptr, message) \
    error_assert_not_null(system, ptr, message, __FILE__, __LINE__)

// Try/catch/finally macros
#define TRY(system) \
    do { \
        exception_enter_try((system)->exception_context); \
        if (!exception_has_error(system)) {

#define CATCH(system, var) \
        } \
        if (exception_has_error(system)) { \
            exception_enter_catch((system)->exception_context, #var); \
            ErrorInfo* var = exception_catch(system);

#define FINALLY(system) \
        } \
        exception_enter_finally((system)->exception_context); \

#define END_TRY(system) \
        exception_exit_finally((system)->exception_context); \
        exception_exit_try((system)->exception_context); \
    } while(0)

#endif // ERROR_SYSTEM_H
