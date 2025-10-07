#ifndef STANDARDIZED_ERRORS_H
#define STANDARDIZED_ERRORS_H

#include <stdbool.h>
#include <stdint.h>

// ============================================================================
// STANDARDIZED ERROR CODES
// ============================================================================

// Error categories with ranges
#define ERROR_CATEGORY_LEXICAL    1000
#define ERROR_CATEGORY_SYNTAX     2000
#define ERROR_CATEGORY_SEMANTIC   3000
#define ERROR_CATEGORY_RUNTIME    4000
#define ERROR_CATEGORY_SYSTEM     5000
#define ERROR_CATEGORY_MEMORY     6000

// Lexical errors (1000-1999)
#define ERROR_UNKNOWN_TOKEN          1001
#define ERROR_UNTERMINATED_STRING    1002
#define ERROR_UNTERMINATED_COMMENT   1003
#define ERROR_INVALID_NUMBER         1004
#define ERROR_INVALID_ESCAPE          1005
#define ERROR_INVALID_CHARACTER      1006

// Syntax errors (2000-2999)
#define ERROR_UNEXPECTED_TOKEN       2001
#define ERROR_MISSING_SEMICOLON      2002
#define ERROR_MISSING_PARENTHESIS    2003
#define ERROR_MISSING_BRACE          2004
#define ERROR_MISSING_BRACKET        2005
#define ERROR_INVALID_EXPRESSION     2006
#define ERROR_INVALID_STATEMENT      2007
#define ERROR_INVALID_FUNCTION       2008
#define ERROR_INVALID_CLASS          2009
#define ERROR_INVALID_IMPORT         2010

// Semantic errors (3000-3999)
#define ERROR_UNDEFINED_VARIABLE     3001
#define ERROR_UNDEFINED_FUNCTION     3002
#define ERROR_UNDEFINED_CLASS        3003
#define ERROR_TYPE_MISMATCH          3004
#define ERROR_ARGUMENT_COUNT         3005
#define ERROR_INVALID_ARGUMENT       3006
#define ERROR_DUPLICATE_DECLARATION  3007
#define ERROR_INVALID_OPERATION_SEMANTIC 3008
#define ERROR_INVALID_CAST           3009
#define ERROR_INVALID_ACCESS         3010

// Runtime errors (4000-4999)
#define ERROR_DIVISION_BY_ZERO       4001
#define ERROR_ARRAY_BOUNDS           4002
#define ERROR_NULL_POINTER           4003
#define ERROR_STACK_OVERFLOW         4004
#define ERROR_RECURSION_LIMIT        4005
#define ERROR_INVALID_OPERATION_RUNTIME 4006
#define ERROR_FILE_NOT_FOUND         4007
#define ERROR_PERMISSION_DENIED      4008
#define ERROR_NETWORK_ERROR          4009
#define ERROR_TIMEOUT                4010

// System errors (5000-5999)
#define ERROR_INTERNAL_ERROR         5001
#define ERROR_NOT_IMPLEMENTED        5002
#define ERROR_INVALID_STATE          5003
#define ERROR_CONFIGURATION_ERROR    5004
#define ERROR_DEPENDENCY_MISSING     5005

// Memory errors (6000-6999)
#define ERROR_OUT_OF_MEMORY          6001
#define ERROR_MEMORY_CORRUPTION      6002
#define ERROR_DOUBLE_FREE            6003
#define ERROR_MEMORY_LEAK            6004

// ============================================================================
// ERROR SEVERITY LEVELS
// ============================================================================

typedef enum {
    ERROR_SEVERITY_INFO,
    ERROR_SEVERITY_WARNING,
    ERROR_SEVERITY_ERROR,
    ERROR_SEVERITY_CRITICAL
} ErrorSeverity;

// ============================================================================
// STANDARDIZED ERROR STRUCTURE
// ============================================================================

typedef struct {
    int code;
    ErrorSeverity severity;
    const char* component;
    const char* function;
    const char* message;
    const char* suggestion;
    int line;
    int column;
    const char* file;
    uint64_t timestamp;
} StandardizedError;

// ============================================================================
// ERROR REPORTING FUNCTIONS
// ============================================================================

// Basic error reporting
void std_error_report(int error_code, const char* component, const char* function,
                     const char* message, int line, int column);

// Error reporting with file information
void std_error_report_file(int error_code, const char* component, const char* function,
                          const char* message, const char* file, int line, int column);

// Error reporting with suggestions
void std_error_report_with_suggestion(int error_code, const char* component, const char* function,
                                     const char* message, const char* suggestion, int line, int column);

// Enhanced error reporting with better formatting and context
void std_error_report_enhanced(int error_code, const char* component, const char* function,
                              const char* message, const char* file, int line, int column,
                              const char* source_line, const char* variable_context);

// ============================================================================
// ERROR INFORMATION RETRIEVAL
// ============================================================================

// Get error information
const char* std_error_get_message(int error_code);
const char* std_error_get_suggestion(int error_code);
ErrorSeverity std_error_get_severity(int error_code);
const char* std_error_get_category(int error_code);

// Check if error code is valid
bool std_error_is_valid(int error_code);

// ============================================================================
// ERROR FORMATTING
// ============================================================================

// Format error for display
char* std_error_format(const StandardizedError* error);
char* std_error_format_simple(int error_code, const char* message, int line, int column);

// ============================================================================
// ERROR SYSTEM MANAGEMENT
// ============================================================================

// Initialize the error system
void std_error_system_init(void);

// Cleanup the error system
void std_error_system_cleanup(void);

// Set error reporting options
void std_error_set_colors(bool enable);
void std_error_set_verbose(bool enable);
void std_error_set_component_filter(const char* component);

// ============================================================================
// CONVENIENCE MACROS
// ============================================================================

// Quick error reporting macros
#define STD_ERROR_REPORT(code, msg) \
    std_error_report(code, __FILE__, __func__, msg, __LINE__, 0)

#define STD_ERROR_REPORT_WITH_LINE(code, msg, line) \
    std_error_report(code, __FILE__, __func__, msg, line, 0)

#define STD_ERROR_REPORT_WITH_POS(code, msg, line, col) \
    std_error_report(code, __FILE__, __func__, msg, line, col)

// Validation macros that report errors
#define STD_VALIDATE_NOT_NULL(ptr, code) \
    do { \
        if (!(ptr)) { \
            STD_ERROR_REPORT(code, "Null pointer validation failed"); \
            return; \
        } \
    } while(0)

#define STD_VALIDATE_RANGE(value, min, max, code) \
    do { \
        if ((value) < (min) || (value) > (max)) { \
            char msg[256]; \
            snprintf(msg, sizeof(msg), "Value %d out of range [%d, %d]", (value), (min), (max)); \
            STD_ERROR_REPORT(code, msg); \
            return; \
        } \
    } while(0)

#endif // STANDARDIZED_ERRORS_H
