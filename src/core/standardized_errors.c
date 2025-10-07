#include "standardized_errors.h"
#include "shared_utilities.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

// ============================================================================
// GLOBAL ERROR SYSTEM STATE
// ============================================================================

static bool colors_enabled = true;
static bool verbose_enabled = false;
static char* component_filter = NULL;

// ANSI color codes
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

// ============================================================================
// ERROR MESSAGE DATABASE
// ============================================================================

typedef struct {
    int code;
    const char* message;
    const char* suggestion;
    ErrorSeverity severity;
    const char* category;
} ErrorInfo;

static const ErrorInfo error_database[] = {
    // Lexical errors
    {ERROR_UNKNOWN_TOKEN, "Unknown token encountered", "Check for typos or unsupported characters. Common issues: missing quotes, invalid operators, or special characters", ERROR_SEVERITY_ERROR, "Lexical"},
    {ERROR_UNTERMINATED_STRING, "Unterminated string literal", "Add closing quote to string. Make sure to escape quotes inside strings with \\\"", ERROR_SEVERITY_ERROR, "Lexical"},
    {ERROR_UNTERMINATED_COMMENT, "Unterminated comment", "Add comment closing marker (/* */) or use single-line comments (//)", ERROR_SEVERITY_ERROR, "Lexical"},
    {ERROR_INVALID_NUMBER, "Invalid number format", "Check number syntax and format. Use decimal notation (3.14) or scientific notation (1e5)", ERROR_SEVERITY_ERROR, "Lexical"},
    {ERROR_INVALID_ESCAPE, "Invalid escape sequence", "Use valid escape sequences: \\n (newline), \\t (tab), \\r (carriage return), \\\\ (backslash), \\\" (quote)", ERROR_SEVERITY_ERROR, "Lexical"},
    {ERROR_INVALID_CHARACTER, "Invalid character", "Use valid characters for the language. Check for non-ASCII characters or special symbols", ERROR_SEVERITY_ERROR, "Lexical"},
    
    // Syntax errors
    {ERROR_UNEXPECTED_TOKEN, "Unexpected token", "Check syntax and token placement. Common issues: missing operators, incorrect punctuation, or incomplete expressions", ERROR_SEVERITY_ERROR, "Syntax"},
    {ERROR_MISSING_SEMICOLON, "Missing semicolon", "Add semicolon (;) at end of statement. All statements must end with a semicolon", ERROR_SEVERITY_ERROR, "Syntax"},
    {ERROR_MISSING_PARENTHESIS, "Missing parenthesis", "Add matching opening/closing parenthesis. Check function calls, expressions, and control structures", ERROR_SEVERITY_ERROR, "Syntax"},
    {ERROR_MISSING_BRACE, "Missing brace", "Add matching opening/closing brace", ERROR_SEVERITY_ERROR, "Syntax"},
    {ERROR_MISSING_BRACKET, "Missing bracket", "Add matching opening/closing bracket", ERROR_SEVERITY_ERROR, "Syntax"},
    {ERROR_INVALID_EXPRESSION, "Invalid expression", "Check expression syntax and operators", ERROR_SEVERITY_ERROR, "Syntax"},
    {ERROR_INVALID_STATEMENT, "Invalid statement", "Check statement syntax and structure", ERROR_SEVERITY_ERROR, "Syntax"},
    {ERROR_INVALID_FUNCTION, "Invalid function definition", "Check function syntax and parameters", ERROR_SEVERITY_ERROR, "Syntax"},
    {ERROR_INVALID_CLASS, "Invalid class definition", "Check class syntax and structure", ERROR_SEVERITY_ERROR, "Syntax"},
    {ERROR_INVALID_IMPORT, "Invalid import statement", "Check import syntax and path", ERROR_SEVERITY_ERROR, "Syntax"},
    
    // Semantic errors
    {ERROR_UNDEFINED_VARIABLE, "Undefined variable", "Declare variable before use or check spelling", ERROR_SEVERITY_ERROR, "Semantic"},
    {ERROR_UNDEFINED_FUNCTION, "Undefined function", "Define function before use or check spelling", ERROR_SEVERITY_ERROR, "Semantic"},
    {ERROR_UNDEFINED_CLASS, "Undefined class", "Define class before use or check spelling", ERROR_SEVERITY_ERROR, "Semantic"},
    {ERROR_TYPE_MISMATCH, "Type mismatch", "Ensure types are compatible for the operation", ERROR_SEVERITY_ERROR, "Semantic"},
    {ERROR_ARGUMENT_COUNT, "Incorrect argument count", "Provide the correct number of arguments", ERROR_SEVERITY_ERROR, "Semantic"},
    {ERROR_INVALID_ARGUMENT, "Invalid argument", "Check argument types and values", ERROR_SEVERITY_ERROR, "Semantic"},
    {ERROR_DUPLICATE_DECLARATION, "Duplicate declaration", "Remove duplicate or rename identifier", ERROR_SEVERITY_ERROR, "Semantic"},
    {ERROR_INVALID_OPERATION_SEMANTIC, "Invalid operation", "Check if operation is supported for the types", ERROR_SEVERITY_ERROR, "Semantic"},
    {ERROR_INVALID_CAST, "Invalid type cast", "Ensure cast is valid and safe", ERROR_SEVERITY_ERROR, "Semantic"},
    {ERROR_INVALID_ACCESS, "Invalid access", "Check access permissions and scope", ERROR_SEVERITY_ERROR, "Semantic"},
    
    // Runtime errors
    {ERROR_DIVISION_BY_ZERO, "Division by zero", "Check divisor before division", ERROR_SEVERITY_ERROR, "Runtime"},
    {ERROR_ARRAY_BOUNDS, "Array index out of bounds", "Check array bounds before access", ERROR_SEVERITY_ERROR, "Runtime"},
    {ERROR_NULL_POINTER, "Null pointer access", "Check for null before dereferencing", ERROR_SEVERITY_ERROR, "Runtime"},
    {ERROR_STACK_OVERFLOW, "Stack overflow", "Reduce recursion depth or increase stack size", ERROR_SEVERITY_CRITICAL, "Runtime"},
    {ERROR_RECURSION_LIMIT, "Recursion limit exceeded", "Reduce recursion depth or use iteration", ERROR_SEVERITY_ERROR, "Runtime"},
    {ERROR_FILE_NOT_FOUND, "File not found", "Check file path and permissions", ERROR_SEVERITY_ERROR, "Runtime"},
    {ERROR_PERMISSION_DENIED, "Permission denied", "Check file permissions and access rights", ERROR_SEVERITY_ERROR, "Runtime"},
    {ERROR_NETWORK_ERROR, "Network error", "Check network connection and configuration", ERROR_SEVERITY_ERROR, "Runtime"},
    {ERROR_TIMEOUT, "Operation timeout", "Increase timeout or check system performance", ERROR_SEVERITY_WARNING, "Runtime"},
    
    // System errors
    {ERROR_INTERNAL_ERROR, "Internal error", "Report this as a bug", ERROR_SEVERITY_CRITICAL, "System"},
    {ERROR_NOT_IMPLEMENTED, "Feature not implemented", "Use alternative approach or wait for implementation", ERROR_SEVERITY_WARNING, "System"},
    {ERROR_INVALID_STATE, "Invalid state", "Check system state and reset if necessary", ERROR_SEVERITY_ERROR, "System"},
    {ERROR_CONFIGURATION_ERROR, "Configuration error", "Check configuration file and settings", ERROR_SEVERITY_ERROR, "System"},
    {ERROR_DEPENDENCY_MISSING, "Missing dependency", "Install required dependencies", ERROR_SEVERITY_ERROR, "System"},
    
    // Memory errors
    {ERROR_OUT_OF_MEMORY, "Out of memory", "Reduce memory usage or increase available memory", ERROR_SEVERITY_CRITICAL, "Memory"},
    {ERROR_MEMORY_CORRUPTION, "Memory corruption", "Check for buffer overflows or use-after-free", ERROR_SEVERITY_CRITICAL, "Memory"},
    {ERROR_DOUBLE_FREE, "Double free", "Ensure each allocation is freed only once", ERROR_SEVERITY_CRITICAL, "Memory"},
    {ERROR_MEMORY_LEAK, "Memory leak", "Ensure all allocations are properly freed", ERROR_SEVERITY_WARNING, "Memory"},
    
    // Terminator
    {0, NULL, NULL, ERROR_SEVERITY_INFO, NULL}
};

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

static const ErrorInfo* find_error_info(int error_code) {
    for (const ErrorInfo* info = error_database; info->code != 0; info++) {
        if (info->code == error_code) {
            return info;
        }
    }
    return NULL;
}

static const char* get_severity_color(ErrorSeverity severity) {
    if (!colors_enabled) return "";
    
    switch (severity) {
        case ERROR_SEVERITY_INFO:     return ANSI_COLOR_CYAN;
        case ERROR_SEVERITY_WARNING:  return ANSI_COLOR_YELLOW;
        case ERROR_SEVERITY_ERROR:   return ANSI_COLOR_RED;
        case ERROR_SEVERITY_CRITICAL: return ANSI_COLOR_MAGENTA;
        default: return "";
    }
}

static const char* get_severity_name(ErrorSeverity severity) {
    switch (severity) {
        case ERROR_SEVERITY_INFO:     return "INFO";
        case ERROR_SEVERITY_WARNING:  return "WARNING";
        case ERROR_SEVERITY_ERROR:   return "ERROR";
        case ERROR_SEVERITY_CRITICAL: return "CRITICAL";
        default: return "UNKNOWN";
    }
}

// ============================================================================
// ERROR REPORTING FUNCTIONS
// ============================================================================

void std_error_report(int error_code, const char* component, const char* function,
                     const char* message, int line, int column) {
    std_error_report_file(error_code, component, function, message, NULL, line, column);
}

void std_error_report_file(int error_code, const char* component, const char* function,
                          const char* message, const char* file, int line, int column) {
    // Check component filter
    if (component_filter && component && strcmp(component_filter, component) != 0) {
        return;
    }
    
    const ErrorInfo* info = find_error_info(error_code);
    ErrorSeverity severity = info ? info->severity : ERROR_SEVERITY_ERROR;
    const char* suggestion = info ? info->suggestion : NULL;
    
    // Get color codes
    const char* severity_color = get_severity_color(severity);
    const char* reset_color = colors_enabled ? ANSI_COLOR_RESET : "";
    
    // Print error header
    fprintf(stderr, "%s[%s]%s ", severity_color, get_severity_name(severity), reset_color);
    
    // Print error code and component
    fprintf(stderr, "E%d [%s::%s]", error_code, component ? component : "unknown", function ? function : "unknown");
    
    // Print location information
    if (file) {
        fprintf(stderr, " in %s", file);
    }
    if (line > 0) {
        fprintf(stderr, ":%d", line);
        if (column > 0) {
            fprintf(stderr, ":%d", column);
        }
    }
    fprintf(stderr, ": ");
    
    // Print error message
    if (message) {
        fprintf(stderr, "%s", message);
    } else if (info && info->message) {
        fprintf(stderr, "%s", info->message);
    } else {
        fprintf(stderr, "Unknown error");
    }
    
    fprintf(stderr, "\n");
    
    // Print suggestion if available
    if (suggestion && verbose_enabled) {
        fprintf(stderr, "  Hint: %s\n", suggestion);
    }
}

void std_error_report_with_suggestion(int error_code, const char* component, const char* function,
                                     const char* message, const char* suggestion, int line, int column) {
    // Check component filter
    if (component_filter && component && strcmp(component_filter, component) != 0) {
        return;
    }
    
    const ErrorInfo* info = find_error_info(error_code);
    ErrorSeverity severity = info ? info->severity : ERROR_SEVERITY_ERROR;
    
    // Get color codes
    const char* severity_color = get_severity_color(severity);
    const char* reset_color = colors_enabled ? ANSI_COLOR_RESET : "";
    
    // Print error header
    fprintf(stderr, "%s[%s]%s ", severity_color, get_severity_name(severity), reset_color);
    
    // Print error code and component
    fprintf(stderr, "E%d [%s::%s]", error_code, component ? component : "unknown", function ? function : "unknown");
    
    // Print location information
    if (line > 0) {
        fprintf(stderr, ":%d", line);
        if (column > 0) {
            fprintf(stderr, ":%d", column);
        }
    }
    fprintf(stderr, ": ");
    
    // Print error message
    if (message) {
        fprintf(stderr, "%s", message);
    } else if (info && info->message) {
        fprintf(stderr, "%s", info->message);
    } else {
        fprintf(stderr, "Unknown error");
    }
    
    fprintf(stderr, "\n");
    
    // Print suggestion
    if (suggestion) {
        fprintf(stderr, "  Hint: %s\n", suggestion);
    }
}

// Enhanced error reporting with better formatting and context
void std_error_report_enhanced(int error_code, const char* component, const char* function,
                              const char* message, const char* file, int line, int column,
                              const char* source_line, const char* variable_context) {
    // Check component filter
    if (component_filter && component && strcmp(component_filter, component) != 0) {
        return;
    }
    
    const ErrorInfo* info = find_error_info(error_code);
    ErrorSeverity severity = info ? info->severity : ERROR_SEVERITY_ERROR;
    const char* suggestion = info ? info->suggestion : NULL;
    
    // Get enhanced color codes
    const char* severity_color = get_severity_color(severity);
    const char* reset_color = colors_enabled ? ANSI_COLOR_RESET : "";
    const char* dim_color = colors_enabled ? "\x1b[2m" : "";
    const char* bold_color = colors_enabled ? "\x1b[1m" : "";
    const char* underline_color = colors_enabled ? "\x1b[4m" : "";
    
    // Print enhanced error header
    fprintf(stderr, "%s[%s]%s ", severity_color, get_severity_name(severity), reset_color);
    
    // Print error code and component with enhanced formatting
    fprintf(stderr, "%sE%d%s [%s%s%s::%s%s%s]", 
            dim_color, error_code, reset_color,
            bold_color, component ? component : "unknown", reset_color,
            dim_color, function ? function : "unknown", reset_color);
    
    // Print enhanced location information
    if (file) {
        fprintf(stderr, " %sin%s %s%s%s", dim_color, reset_color, bold_color, file, reset_color);
    }
    if (line > 0) {
        fprintf(stderr, "%s:%s%d", dim_color, reset_color, line);
        if (column > 0) {
            fprintf(stderr, "%s:%s%d", dim_color, reset_color, column);
        }
    }
    fprintf(stderr, "%s: ", reset_color);
    
    // Print error message with enhanced formatting
    if (message) {
        fprintf(stderr, "%s", message);
    } else if (info && info->message) {
        fprintf(stderr, "%s", info->message);
    } else {
        fprintf(stderr, "Unknown error");
    }
    
    fprintf(stderr, "\n");
    
    // Print source code snippet if available
    if (source_line && line > 0) {
        fprintf(stderr, "  %s┌─ Source:%s\n", dim_color, reset_color);
        fprintf(stderr, "  %s│%s %s", dim_color, reset_color, source_line);
        if (column > 0) {
            fprintf(stderr, "\n  %s│%s %*s%s^%s", dim_color, reset_color, column - 1, "", underline_color, reset_color);
        }
        fprintf(stderr, "\n  %s└─%s\n", dim_color, reset_color);
    }
    
    // Print variable context if available
    if (variable_context) {
        fprintf(stderr, "  %s🔍 Context:%s %s\n", dim_color, reset_color, variable_context);
    }
    
    // Print enhanced suggestion with better formatting
    if (suggestion) {
        fprintf(stderr, "  %s💡 Hint:%s %s\n", severity_color, reset_color, suggestion);
    }
    
    // Print error category if available
    if (info && info->category) {
        fprintf(stderr, "  %s📁 Category:%s %s\n", dim_color, reset_color, info->category);
    }
    
    // Print related error suggestions
    if (info && info->code >= 2000 && info->code < 3000) {
        fprintf(stderr, "  %s🔗 Related:%s Check syntax documentation for similar constructs\n", dim_color, reset_color);
    } else if (info && info->code >= 3000 && info->code < 4000) {
        fprintf(stderr, "  %s🔗 Related:%s Check variable declarations and type annotations\n", dim_color, reset_color);
    }
}

// ============================================================================
// ERROR INFORMATION RETRIEVAL
// ============================================================================

const char* std_error_get_message(int error_code) {
    const ErrorInfo* info = find_error_info(error_code);
    return info ? info->message : NULL;
}

const char* std_error_get_suggestion(int error_code) {
    const ErrorInfo* info = find_error_info(error_code);
    return info ? info->suggestion : NULL;
}

ErrorSeverity std_error_get_severity(int error_code) {
    const ErrorInfo* info = find_error_info(error_code);
    return info ? info->severity : ERROR_SEVERITY_ERROR;
}

const char* std_error_get_category(int error_code) {
    const ErrorInfo* info = find_error_info(error_code);
    return info ? info->category : "Unknown";
}

bool std_error_is_valid(int error_code) {
    return find_error_info(error_code) != NULL;
}

// ============================================================================
// ERROR FORMATTING
// ============================================================================

char* std_error_format(const StandardizedError* error) {
    if (!error) return NULL;
    
    const char* severity_color = get_severity_color(error->severity);
    const char* reset_color = colors_enabled ? ANSI_COLOR_RESET : "";
    
    return shared_strprintf("%s[%s]%s E%d [%s::%s]%s:%d:%d: %s",
                           severity_color, get_severity_name(error->severity), reset_color,
                           error->code, error->component, error->function,
                           error->file, error->line, error->column, error->message);
}

char* std_error_format_simple(int error_code, const char* message, int line, int column) {
    const ErrorInfo* info = find_error_info(error_code);
    ErrorSeverity severity = info ? info->severity : ERROR_SEVERITY_ERROR;
    const char* severity_color = get_severity_color(severity);
    const char* reset_color = colors_enabled ? ANSI_COLOR_RESET : "";
    
    if (line > 0 && column > 0) {
        return shared_strprintf("%s[%s]%s E%d: %s (Line %d, Column %d)",
                               severity_color, get_severity_name(severity), reset_color,
                               error_code, message, line, column);
    } else if (line > 0) {
        return shared_strprintf("%s[%s]%s E%d: %s (Line %d)",
                               severity_color, get_severity_name(severity), reset_color,
                               error_code, message, line);
    } else {
        return shared_strprintf("%s[%s]%s E%d: %s",
                               severity_color, get_severity_name(severity), reset_color,
                               error_code, message);
    }
}

// ============================================================================
// ERROR SYSTEM MANAGEMENT
// ============================================================================

void std_error_system_init(void) {
    colors_enabled = true;
    verbose_enabled = false;
    component_filter = NULL;
}

void std_error_system_cleanup(void) {
    shared_free(component_filter);
    component_filter = NULL;
}

void std_error_set_colors(bool enable) {
    colors_enabled = enable;
}

void std_error_set_verbose(bool enable) {
    verbose_enabled = enable;
}

void std_error_set_component_filter(const char* component) {
    shared_free(component_filter);
    component_filter = component ? shared_strdup(component) : NULL;
}
