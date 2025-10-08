#include "interpreter.h"
#include "error_system.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>

// Global error system instance
static ErrorSystem* global_error_system = NULL;

// Forward declarations used within this translation unit
ErrorSystem* error_system_get_global(void);

// Comprehensive error code definitions with categories
typedef enum {
    // Lexical errors (1000-1999)
    MYCO_ERROR_UNKNOWN_TOKEN = 1000,
    MYCO_ERROR_UNTERMINATED_STRING = 1001,
    MYCO_ERROR_UNTERMINATED_COMMENT = 1002,
    MYCO_ERROR_INVALID_NUMBER = 1003,
    MYCO_ERROR_INVALID_ESCAPE = 1004,
    
    // Syntax errors (2000-2999)
    MYCO_ERROR_UNEXPECTED_TOKEN = 2000,
    MYCO_ERROR_MISSING_SEMICOLON = 2001,
    MYCO_ERROR_MISSING_PARENTHESIS = 2002,
    MYCO_ERROR_MISSING_BRACE = 2003,
    MYCO_ERROR_MISSING_BRACKET = 2004,
    MYCO_ERROR_INVALID_EXPRESSION = 2005,
    MYCO_ERROR_INVALID_STATEMENT = 2006,
    MYCO_ERROR_INVALID_FUNCTION = 2007,
    MYCO_ERROR_INVALID_CLASS = 2008,
    MYCO_ERROR_INVALID_IMPORT = 2009,
    
    // Semantic errors (3000-3999)
    MYCO_ERROR_UNDEFINED_VARIABLE = 3000,
    MYCO_ERROR_UNDEFINED_FUNCTION = 3001,
    MYCO_ERROR_UNDEFINED_CLASS = 3002,
    MYCO_ERROR_TYPE_MISMATCH = 3003,
    MYCO_ERROR_ARGUMENT_COUNT = 3004,
    MYCO_ERROR_INVALID_ARGUMENT = 3005,
    MYCO_ERROR_DUPLICATE_DECLARATION = 3006,
    MYCO_ERROR_INVALID_OPERATION = 3007,
    MYCO_ERROR_INVALID_CAST = 3008,
    MYCO_ERROR_INVALID_ACCESS = 3009,
    
    // Runtime errors (4000-4999)
    MYCO_ERROR_DIVISION_BY_ZERO = 4000,
    MYCO_ERROR_ARRAY_BOUNDS = 4001,
    MYCO_ERROR_NULL_POINTER = 4002,
    MYCO_ERROR_STACK_OVERFLOW = 4003,
    MYCO_ERROR_STACK_UNDERFLOW = 4004,
    MYCO_ERROR_INVALID_INDEX = 4005,
    MYCO_ERROR_INVALID_KEY = 4006,
    MYCO_ERROR_INVALID_RUNTIME_OPERATION = 4007,
    MYCO_ERROR_INVALID_STATE = 4008,
    MYCO_ERROR_INVALID_CONTEXT = 4009,
    
    // Memory errors (5000-5999)
    MYCO_ERROR_OUT_OF_MEMORY = 5000,
    MYCO_ERROR_MEMORY_CORRUPTION = 5001,
    MYCO_ERROR_DOUBLE_FREE = 5002,
    MYCO_ERROR_MEMORY_LEAK = 5003,
    MYCO_ERROR_INVALID_POINTER = 5004,
    MYCO_ERROR_BUFFER_OVERFLOW = 5005,
    MYCO_ERROR_BUFFER_UNDERFLOW = 5006,
    MYCO_ERROR_INVALID_SIZE = 5007,
    MYCO_ERROR_INVALID_ALIGNMENT = 5008,
    MYCO_ERROR_MEMORY_EXHAUSTED = 5009,
    
    // System errors (6000-6999)
    MYCO_ERROR_FILE_NOT_FOUND = 6000,
    MYCO_ERROR_FILE_ACCESS = 6001,
    MYCO_ERROR_FILE_PERMISSION = 6002,
    MYCO_ERROR_FILE_CORRUPTION = 6003,
    MYCO_ERROR_NETWORK_ERROR = 6004,
    MYCO_ERROR_TIMEOUT = 6005,
    MYCO_ERROR_INTERRUPTED = 6006,
    MYCO_ERROR_SYSTEM_ERROR = 6007,
    MYCO_ERROR_PLATFORM_ERROR = 6008,
    MYCO_ERROR_EXTERNAL_ERROR = 6009,
    
    // Compilation errors (7000-7999)
    MYCO_ERROR_COMPILATION_FAILED = 7000,
    MYCO_ERROR_OPTIMIZATION_FAILED = 7001,
    MYCO_ERROR_CODE_GENERATION_FAILED = 7002,
    MYCO_ERROR_LINKING_FAILED = 7003,
    MYCO_ERROR_ASSEMBLY_FAILED = 7004,
    MYCO_ERROR_TARGET_NOT_SUPPORTED = 7005,
    MYCO_ERROR_INVALID_TARGET = 7006,
    MYCO_ERROR_COMPILER_BUG = 7007,
    MYCO_ERROR_INTERNAL_ERROR = 7008,
    MYCO_ERROR_UNIMPLEMENTED = 7009
} MycoErrorCode;

// Get error code from message with comprehensive pattern matching
int get_error_code_from_message(const char* message) {
    if (!message) return MYCO_ERROR_INTERNAL_ERROR;
    
    // Convert to lowercase for case-insensitive matching
    char* lower_message = malloc(strlen(message) + 1);
    if (!lower_message) return MYCO_ERROR_OUT_OF_MEMORY;
    
    for (size_t i = 0; message[i]; i++) {
        lower_message[i] = (message[i] >= 'A' && message[i] <= 'Z') ? 
                          message[i] + 32 : message[i];
    }
    lower_message[strlen(message)] = '\0';
    
    int error_code = MYCO_ERROR_INTERNAL_ERROR;
    
    // Lexical errors
    if (strstr(lower_message, "unknown token") || strstr(lower_message, "unexpected character")) {
        error_code = MYCO_ERROR_UNKNOWN_TOKEN;
    } else if (strstr(lower_message, "unterminated string") || strstr(lower_message, "missing quote")) {
        error_code = MYCO_ERROR_UNTERMINATED_STRING;
    } else if (strstr(lower_message, "unterminated comment") || strstr(lower_message, "missing comment end")) {
        error_code = MYCO_ERROR_UNTERMINATED_COMMENT;
    } else if (strstr(lower_message, "invalid number") || strstr(lower_message, "malformed number")) {
        error_code = MYCO_ERROR_INVALID_NUMBER;
    } else if (strstr(lower_message, "invalid escape") || strstr(lower_message, "unknown escape")) {
        error_code = MYCO_ERROR_INVALID_ESCAPE;
    }
    // Syntax errors
    else if (strstr(lower_message, "unexpected token") || strstr(lower_message, "unexpected symbol")) {
        error_code = MYCO_ERROR_UNEXPECTED_TOKEN;
    } else if (strstr(lower_message, "missing semicolon") || strstr(lower_message, "expected semicolon")) {
        error_code = MYCO_ERROR_MISSING_SEMICOLON;
    } else if (strstr(lower_message, "missing parenthesis") || strstr(lower_message, "unmatched parenthesis")) {
        error_code = MYCO_ERROR_MISSING_PARENTHESIS;
    } else if (strstr(lower_message, "missing brace") || strstr(lower_message, "unmatched brace")) {
        error_code = MYCO_ERROR_MISSING_BRACE;
    } else if (strstr(lower_message, "missing bracket") || strstr(lower_message, "unmatched bracket")) {
        error_code = MYCO_ERROR_MISSING_BRACKET;
    } else if (strstr(lower_message, "invalid expression") || strstr(lower_message, "malformed expression")) {
        error_code = MYCO_ERROR_INVALID_EXPRESSION;
    } else if (strstr(lower_message, "invalid statement") || strstr(lower_message, "malformed statement")) {
        error_code = MYCO_ERROR_INVALID_STATEMENT;
    } else if (strstr(lower_message, "invalid function") || strstr(lower_message, "malformed function")) {
        error_code = MYCO_ERROR_INVALID_FUNCTION;
    } else if (strstr(lower_message, "invalid class") || strstr(lower_message, "malformed class")) {
        error_code = MYCO_ERROR_INVALID_CLASS;
    } else if (strstr(lower_message, "invalid import") || strstr(lower_message, "malformed import")) {
        error_code = MYCO_ERROR_INVALID_IMPORT;
    }
    // Semantic errors
    else if (strstr(lower_message, "undefined variable") || strstr(lower_message, "unknown variable")) {
        error_code = MYCO_ERROR_UNDEFINED_VARIABLE;
    } else if (strstr(lower_message, "undefined function") || strstr(lower_message, "unknown function")) {
        error_code = MYCO_ERROR_UNDEFINED_FUNCTION;
    } else if (strstr(lower_message, "undefined class") || strstr(lower_message, "unknown class")) {
        error_code = MYCO_ERROR_UNDEFINED_CLASS;
    } else if (strstr(lower_message, "type mismatch") || strstr(lower_message, "incompatible type")) {
        error_code = MYCO_ERROR_TYPE_MISMATCH;
    } else if (strstr(lower_message, "argument count") || strstr(lower_message, "wrong number of arguments")) {
        error_code = MYCO_ERROR_ARGUMENT_COUNT;
    } else if (strstr(lower_message, "invalid argument") || strstr(lower_message, "bad argument")) {
        error_code = MYCO_ERROR_INVALID_ARGUMENT;
    } else if (strstr(lower_message, "duplicate declaration") || strstr(lower_message, "already declared")) {
        error_code = MYCO_ERROR_DUPLICATE_DECLARATION;
    } else if (strstr(lower_message, "invalid operation") || strstr(lower_message, "unsupported operation")) {
        error_code = MYCO_ERROR_INVALID_OPERATION;
    } else if (strstr(lower_message, "invalid cast") || strstr(lower_message, "cannot cast")) {
        error_code = MYCO_ERROR_INVALID_CAST;
    } else if (strstr(lower_message, "invalid access") || strstr(lower_message, "access denied")) {
        error_code = MYCO_ERROR_INVALID_ACCESS;
    }
    // Runtime errors
    else if (strstr(lower_message, "division by zero") || strstr(lower_message, "divide by zero")) {
        error_code = MYCO_ERROR_DIVISION_BY_ZERO;
    } else if (strstr(lower_message, "array bounds") || strstr(lower_message, "index out of bounds")) {
        error_code = MYCO_ERROR_ARRAY_BOUNDS;
    } else if (strstr(lower_message, "null pointer") || strstr(lower_message, "null reference")) {
        error_code = MYCO_ERROR_NULL_POINTER;
    } else if (strstr(lower_message, "stack overflow") || strstr(lower_message, "stack too deep")) {
        error_code = MYCO_ERROR_STACK_OVERFLOW;
    } else if (strstr(lower_message, "stack underflow") || strstr(lower_message, "empty stack")) {
        error_code = MYCO_ERROR_STACK_UNDERFLOW;
    } else if (strstr(lower_message, "invalid index") || strstr(lower_message, "bad index")) {
        error_code = MYCO_ERROR_INVALID_INDEX;
    } else if (strstr(lower_message, "invalid key") || strstr(lower_message, "bad key")) {
        error_code = MYCO_ERROR_INVALID_KEY;
    } else if (strstr(lower_message, "invalid state") || strstr(lower_message, "bad state")) {
        error_code = MYCO_ERROR_INVALID_STATE;
    } else if (strstr(lower_message, "invalid context") || strstr(lower_message, "bad context")) {
        error_code = MYCO_ERROR_INVALID_CONTEXT;
    }
    // Memory errors
    else if (strstr(lower_message, "out of memory") || strstr(lower_message, "memory allocation failed")) {
        error_code = MYCO_ERROR_OUT_OF_MEMORY;
    } else if (strstr(lower_message, "memory corruption") || strstr(lower_message, "corrupted memory")) {
        error_code = MYCO_ERROR_MEMORY_CORRUPTION;
    } else if (strstr(lower_message, "double free") || strstr(lower_message, "already freed")) {
        error_code = MYCO_ERROR_DOUBLE_FREE;
    } else if (strstr(lower_message, "memory leak") || strstr(lower_message, "leaked memory")) {
        error_code = MYCO_ERROR_MEMORY_LEAK;
    } else if (strstr(lower_message, "invalid pointer") || strstr(lower_message, "bad pointer")) {
        error_code = MYCO_ERROR_INVALID_POINTER;
    } else if (strstr(lower_message, "buffer overflow") || strstr(lower_message, "buffer overrun")) {
        error_code = MYCO_ERROR_BUFFER_OVERFLOW;
    } else if (strstr(lower_message, "buffer underflow") || strstr(lower_message, "buffer underrun")) {
        error_code = MYCO_ERROR_BUFFER_UNDERFLOW;
    } else if (strstr(lower_message, "invalid size") || strstr(lower_message, "bad size")) {
        error_code = MYCO_ERROR_INVALID_SIZE;
    } else if (strstr(lower_message, "invalid alignment") || strstr(lower_message, "bad alignment")) {
        error_code = MYCO_ERROR_INVALID_ALIGNMENT;
    } else if (strstr(lower_message, "memory exhausted") || strstr(lower_message, "no memory left")) {
        error_code = MYCO_ERROR_MEMORY_EXHAUSTED;
    }
    // System errors
    else if (strstr(lower_message, "file not found") || strstr(lower_message, "no such file")) {
        error_code = MYCO_ERROR_FILE_NOT_FOUND;
    } else if (strstr(lower_message, "file access") || strstr(lower_message, "cannot access file")) {
        error_code = MYCO_ERROR_FILE_ACCESS;
    } else if (strstr(lower_message, "file permission") || strstr(lower_message, "permission denied")) {
        error_code = MYCO_ERROR_FILE_PERMISSION;
    } else if (strstr(lower_message, "file corruption") || strstr(lower_message, "corrupted file")) {
        error_code = MYCO_ERROR_FILE_CORRUPTION;
    } else if (strstr(lower_message, "network error") || strstr(lower_message, "connection failed")) {
        error_code = MYCO_ERROR_NETWORK_ERROR;
    } else if (strstr(lower_message, "timeout") || strstr(lower_message, "timed out")) {
        error_code = MYCO_ERROR_TIMEOUT;
    } else if (strstr(lower_message, "interrupted") || strstr(lower_message, "signal received")) {
        error_code = MYCO_ERROR_INTERRUPTED;
    } else if (strstr(lower_message, "system error") || strstr(lower_message, "os error")) {
        error_code = MYCO_ERROR_SYSTEM_ERROR;
    } else if (strstr(lower_message, "platform error") || strstr(lower_message, "platform specific")) {
        error_code = MYCO_ERROR_PLATFORM_ERROR;
    } else if (strstr(lower_message, "external error") || strstr(lower_message, "third party error")) {
        error_code = MYCO_ERROR_EXTERNAL_ERROR;
    }
    // Compilation errors
    else if (strstr(lower_message, "compilation failed") || strstr(lower_message, "compile error")) {
        error_code = MYCO_ERROR_COMPILATION_FAILED;
    } else if (strstr(lower_message, "optimization failed") || strstr(lower_message, "optimize error")) {
        error_code = MYCO_ERROR_OPTIMIZATION_FAILED;
    } else if (strstr(lower_message, "code generation failed") || strstr(lower_message, "codegen error")) {
        error_code = MYCO_ERROR_CODE_GENERATION_FAILED;
    } else if (strstr(lower_message, "linking failed") || strstr(lower_message, "link error")) {
        error_code = MYCO_ERROR_LINKING_FAILED;
    } else if (strstr(lower_message, "assembly failed") || strstr(lower_message, "assemble error")) {
        error_code = MYCO_ERROR_ASSEMBLY_FAILED;
    } else if (strstr(lower_message, "target not supported") || strstr(lower_message, "unsupported target")) {
        error_code = MYCO_ERROR_TARGET_NOT_SUPPORTED;
    } else if (strstr(lower_message, "invalid target") || strstr(lower_message, "bad target")) {
        error_code = MYCO_ERROR_INVALID_TARGET;
    } else if (strstr(lower_message, "compiler bug") || strstr(lower_message, "internal compiler error")) {
        error_code = MYCO_ERROR_COMPILER_BUG;
    } else if (strstr(lower_message, "internal error") || strstr(lower_message, "internal failure")) {
        error_code = MYCO_ERROR_INTERNAL_ERROR;
    } else if (strstr(lower_message, "unimplemented") || strstr(lower_message, "not implemented")) {
        error_code = MYCO_ERROR_UNIMPLEMENTED;
    }
    
    free(lower_message);
    return error_code;
}

// Get fungus-themed error name from code
const char* get_fungus_error_name(int error_code) {
    switch (error_code) {
        // Lexical errors
        case MYCO_ERROR_UNKNOWN_TOKEN: return "SPORE_UNKNOWN";
        case MYCO_ERROR_UNTERMINATED_STRING: return "SPORE_UNFINISHED";
        case MYCO_ERROR_UNTERMINATED_COMMENT: return "SPORE_UNCLOSED";
        case MYCO_ERROR_INVALID_NUMBER: return "SPORE_MALFORMED";
        case MYCO_ERROR_INVALID_ESCAPE: return "SPORE_ESCAPED";
        
        // Syntax errors
        case MYCO_ERROR_UNEXPECTED_TOKEN: return "SPORE_UNEXPECTED";
        case MYCO_ERROR_MISSING_SEMICOLON: return "SPORE_MISSING_DOT";
        case MYCO_ERROR_MISSING_PARENTHESIS: return "SPORE_MISSING_CAP";
        case MYCO_ERROR_MISSING_BRACE: return "SPORE_MISSING_RING";
        case MYCO_ERROR_MISSING_BRACKET: return "SPORE_MISSING_BRACKET";
        case MYCO_ERROR_INVALID_EXPRESSION: return "SPORE_MALFORMED_EXPRESSION";
        case MYCO_ERROR_INVALID_STATEMENT: return "SPORE_MALFORMED_STATEMENT";
        case MYCO_ERROR_INVALID_FUNCTION: return "SPORE_MALFORMED_FUNCTION";
        case MYCO_ERROR_INVALID_CLASS: return "SPORE_MALFORMED_CLASS";
        case MYCO_ERROR_INVALID_IMPORT: return "SPORE_MALFORMED_IMPORT";
        
        // Semantic errors
        case MYCO_ERROR_UNDEFINED_VARIABLE: return "LOST_IN_THE_MYCELIUM";
        case MYCO_ERROR_UNDEFINED_FUNCTION: return "UNKNOWN_FUNGUS";
        case MYCO_ERROR_UNDEFINED_CLASS: return "UNIDENTIFIED_SPORE";
        case MYCO_ERROR_TYPE_MISMATCH: return "SPORE_TYPE_MISMATCH";
        case MYCO_ERROR_ARGUMENT_COUNT: return "SPORE_ARGUMENT_COUNT";
        case MYCO_ERROR_INVALID_ARGUMENT: return "SPORE_INVALID_ARGUMENT";
        case MYCO_ERROR_DUPLICATE_DECLARATION: return "SPORE_DUPLICATE";
        case MYCO_ERROR_INVALID_OPERATION: return "SPORE_INVALID_OPERATION";
        case MYCO_ERROR_INVALID_RUNTIME_OPERATION: return "SPORE_INVALID_RUNTIME_OPERATION";
        case MYCO_ERROR_INVALID_CAST: return "SPORE_INVALID_CAST";
        case MYCO_ERROR_INVALID_ACCESS: return "SPORE_ACCESS_DENIED";
        
        // Runtime errors
        case MYCO_ERROR_DIVISION_BY_ZERO: return "SPORE_SPLIT";
        case MYCO_ERROR_ARRAY_BOUNDS: return "MUSHROOM_TOO_BIG";
        case MYCO_ERROR_NULL_POINTER: return "SPORE_NULL";
        case MYCO_ERROR_STACK_OVERFLOW: return "MUSHROOM_TOWER";
        case MYCO_ERROR_STACK_UNDERFLOW: return "EMPTY_MUSHROOM";
        case MYCO_ERROR_INVALID_INDEX: return "SPORE_INDEX_ERROR";
        case MYCO_ERROR_INVALID_KEY: return "SPORE_KEY_ERROR";
        case MYCO_ERROR_INVALID_STATE: return "SPORE_STATE_ERROR";
        case MYCO_ERROR_INVALID_CONTEXT: return "SPORE_CONTEXT_ERROR";
        
        // Memory errors
        case MYCO_ERROR_OUT_OF_MEMORY: return "SPORE_EXHAUSTED";
        case MYCO_ERROR_MEMORY_CORRUPTION: return "SPORE_CORRUPTED";
        case MYCO_ERROR_DOUBLE_FREE: return "SPORE_DOUBLE_FREED";
        case MYCO_ERROR_MEMORY_LEAK: return "SPORE_LEAKED";
        case MYCO_ERROR_INVALID_POINTER: return "SPORE_INVALID_POINTER";
        case MYCO_ERROR_BUFFER_OVERFLOW: return "SPORE_OVERFLOW";
        case MYCO_ERROR_BUFFER_UNDERFLOW: return "SPORE_UNDERFLOW";
        case MYCO_ERROR_INVALID_SIZE: return "SPORE_INVALID_SIZE";
        case MYCO_ERROR_INVALID_ALIGNMENT: return "SPORE_MISALIGNED";
        case MYCO_ERROR_MEMORY_EXHAUSTED: return "SPORE_DEPLETED";
        
        // System errors
        case MYCO_ERROR_FILE_NOT_FOUND: return "SPORE_NOT_FOUND";
        case MYCO_ERROR_FILE_ACCESS: return "SPORE_ACCESS_DENIED";
        case MYCO_ERROR_FILE_PERMISSION: return "SPORE_PERMISSION_DENIED";
        case MYCO_ERROR_FILE_CORRUPTION: return "SPORE_CORRUPTED_FILE";
        case MYCO_ERROR_NETWORK_ERROR: return "SPORE_NETWORK_ERROR";
        case MYCO_ERROR_TIMEOUT: return "SPORE_TIMEOUT";
        case MYCO_ERROR_INTERRUPTED: return "SPORE_INTERRUPTED";
        case MYCO_ERROR_SYSTEM_ERROR: return "SPORE_SYSTEM_ERROR";
        case MYCO_ERROR_PLATFORM_ERROR: return "SPORE_PLATFORM_ERROR";
        case MYCO_ERROR_EXTERNAL_ERROR: return "SPORE_EXTERNAL_ERROR";
        
        // Compilation errors
        case MYCO_ERROR_COMPILATION_FAILED: return "SPORE_COMPILATION_FAILED";
        case MYCO_ERROR_OPTIMIZATION_FAILED: return "SPORE_OPTIMIZATION_FAILED";
        case MYCO_ERROR_CODE_GENERATION_FAILED: return "SPORE_CODE_GENERATION_FAILED";
        case MYCO_ERROR_LINKING_FAILED: return "SPORE_LINKING_FAILED";
        case MYCO_ERROR_ASSEMBLY_FAILED: return "SPORE_ASSEMBLY_FAILED";
        case MYCO_ERROR_TARGET_NOT_SUPPORTED: return "SPORE_TARGET_NOT_SUPPORTED";
        case MYCO_ERROR_INVALID_TARGET: return "SPORE_INVALID_TARGET";
        case MYCO_ERROR_COMPILER_BUG: return "SPORE_COMPILER_BUG";
        case MYCO_ERROR_INTERNAL_ERROR: return "SPORE_INTERNAL_ERROR";
        case MYCO_ERROR_UNIMPLEMENTED: return "SPORE_UNIMPLEMENTED";
        
        default: return "UNKNOWN_SPORE";
    }
}

// Get error solution from code
const char* get_error_solution(int error_code) {
    switch (error_code) {
        // Lexical errors
        case MYCO_ERROR_UNKNOWN_TOKEN: return "Check for typos or unsupported characters";
        case MYCO_ERROR_UNTERMINATED_STRING: return "Add closing quote to string literal";
        case MYCO_ERROR_UNTERMINATED_COMMENT: return "Add closing comment marker";
        case MYCO_ERROR_INVALID_NUMBER: return "Check number format and digits";
        case MYCO_ERROR_INVALID_ESCAPE: return "Use valid escape sequences (\\n, \\t, \\\", \\\\, etc.)";
        
        // Syntax errors
        case MYCO_ERROR_UNEXPECTED_TOKEN: return "Check syntax and token placement";
        case MYCO_ERROR_MISSING_SEMICOLON: return "Add semicolon at end of statement";
        case MYCO_ERROR_MISSING_PARENTHESIS: return "Add matching parenthesis";
        case MYCO_ERROR_MISSING_BRACE: return "Add matching brace";
        case MYCO_ERROR_MISSING_BRACKET: return "Add matching bracket";
        case MYCO_ERROR_INVALID_EXPRESSION: return "Check expression syntax and operators";
        case MYCO_ERROR_INVALID_STATEMENT: return "Check statement syntax and structure";
        case MYCO_ERROR_INVALID_FUNCTION: return "Check function declaration syntax";
        case MYCO_ERROR_INVALID_CLASS: return "Check class declaration syntax";
        case MYCO_ERROR_INVALID_IMPORT: return "Check import statement syntax";
        
        // Semantic errors
        case MYCO_ERROR_UNDEFINED_VARIABLE: return "Declare variable before use or check spelling";
        case MYCO_ERROR_UNDEFINED_FUNCTION: return "Define function before use or check spelling";
        case MYCO_ERROR_UNDEFINED_CLASS: return "Define class before use or check spelling";
        case MYCO_ERROR_TYPE_MISMATCH: return "Check types and use appropriate conversions";
        case MYCO_ERROR_ARGUMENT_COUNT: return "Provide correct number of arguments";
        case MYCO_ERROR_INVALID_ARGUMENT: return "Check argument types and values";
        case MYCO_ERROR_DUPLICATE_DECLARATION: return "Remove duplicate declaration or use different name";
        case MYCO_ERROR_INVALID_OPERATION: return "Use supported operations for the data type";
        case MYCO_ERROR_INVALID_RUNTIME_OPERATION: return "Use supported runtime operations";
        case MYCO_ERROR_INVALID_CAST: return "Use valid type conversions";
        case MYCO_ERROR_INVALID_ACCESS: return "Check access permissions and scope";
        
        // Runtime errors
        case MYCO_ERROR_DIVISION_BY_ZERO: return "Check divisor is not zero before division";
        case MYCO_ERROR_ARRAY_BOUNDS: return "Check array index is within valid range";
        case MYCO_ERROR_NULL_POINTER: return "Check for null values before dereferencing";
        case MYCO_ERROR_STACK_OVERFLOW: return "Reduce recursion depth or increase stack size";
        case MYCO_ERROR_STACK_UNDERFLOW: return "Check stack is not empty before popping";
        case MYCO_ERROR_INVALID_INDEX: return "Use valid index values";
        case MYCO_ERROR_INVALID_KEY: return "Use valid key values";
        case MYCO_ERROR_INVALID_STATE: return "Check object state before operation";
        case MYCO_ERROR_INVALID_CONTEXT: return "Check execution context";
        
        // Memory errors
        case MYCO_ERROR_OUT_OF_MEMORY: return "Free unused memory or increase available memory";
        case MYCO_ERROR_MEMORY_CORRUPTION: return "Check for buffer overflows and invalid writes";
        case MYCO_ERROR_DOUBLE_FREE: return "Avoid freeing the same memory twice";
        case MYCO_ERROR_MEMORY_LEAK: return "Free allocated memory when no longer needed";
        case MYCO_ERROR_INVALID_POINTER: return "Check pointer validity before use";
        case MYCO_ERROR_BUFFER_OVERFLOW: return "Check buffer bounds before writing";
        case MYCO_ERROR_BUFFER_UNDERFLOW: return "Check buffer bounds before reading";
        case MYCO_ERROR_INVALID_SIZE: return "Use valid size values";
        case MYCO_ERROR_INVALID_ALIGNMENT: return "Use properly aligned memory";
        case MYCO_ERROR_MEMORY_EXHAUSTED: return "Free memory or reduce memory usage";
        
        // System errors
        case MYCO_ERROR_FILE_NOT_FOUND: return "Check file path and existence";
        case MYCO_ERROR_FILE_ACCESS: return "Check file permissions and accessibility";
        case MYCO_ERROR_FILE_PERMISSION: return "Check file permissions and user rights";
        case MYCO_ERROR_FILE_CORRUPTION: return "Check file integrity and format";
        case MYCO_ERROR_NETWORK_ERROR: return "Check network connection and configuration";
        case MYCO_ERROR_TIMEOUT: return "Increase timeout or check system performance";
        case MYCO_ERROR_INTERRUPTED: return "Handle interruption gracefully";
        case MYCO_ERROR_SYSTEM_ERROR: return "Check system resources and configuration";
        case MYCO_ERROR_PLATFORM_ERROR: return "Check platform compatibility";
        case MYCO_ERROR_EXTERNAL_ERROR: return "Check external dependencies and services";
        
        // Compilation errors
        case MYCO_ERROR_COMPILATION_FAILED: return "Check source code syntax and dependencies";
        case MYCO_ERROR_OPTIMIZATION_FAILED: return "Disable optimization or check code structure";
        case MYCO_ERROR_CODE_GENERATION_FAILED: return "Check target architecture and code generation";
        case MYCO_ERROR_LINKING_FAILED: return "Check libraries and linking configuration";
        case MYCO_ERROR_ASSEMBLY_FAILED: return "Check assembly code and target architecture";
        case MYCO_ERROR_TARGET_NOT_SUPPORTED: return "Use supported target architecture";
        case MYCO_ERROR_INVALID_TARGET: return "Use valid target architecture";
        case MYCO_ERROR_COMPILER_BUG: return "Report bug to compiler developers";
        case MYCO_ERROR_INTERNAL_ERROR: return "Check system resources and report issue";
        case MYCO_ERROR_UNIMPLEMENTED: return "Feature not yet implemented";
        
        default: return "Unknown error - check documentation";
    }
}

// ANSI color codes for terminal output
static int g_error_colors_enabled = 1;

#define ANSI_COLOR_RED     (g_error_colors_enabled ? "\x1b[31m" : "")
#define ANSI_COLOR_GREEN   (g_error_colors_enabled ? "\x1b[32m" : "")
#define ANSI_COLOR_YELLOW  (g_error_colors_enabled ? "\x1b[33m" : "")
#define ANSI_COLOR_BLUE    (g_error_colors_enabled ? "\x1b[34m" : "")
#define ANSI_COLOR_MAGENTA (g_error_colors_enabled ? "\x1b[35m" : "")
#define ANSI_COLOR_CYAN    (g_error_colors_enabled ? "\x1b[36m" : "")
#define ANSI_COLOR_WHITE   (g_error_colors_enabled ? "\x1b[37m" : "")
#define ANSI_COLOR_RESET   (g_error_colors_enabled ? "\x1b[0m" : "")
#define ANSI_COLOR_BOLD    (g_error_colors_enabled ? "\x1b[1m" : "")
#define ANSI_COLOR_DIM     (g_error_colors_enabled ? "\x1b[2m" : "")

// Enhanced error reporting with colors and formatting
void interpreter_report_error_enhanced(Interpreter* interpreter, const char* message, int line, int column) {
    if (!interpreter || !message) return;
    
    // Get error code and details (no themed names, keep it simple)
    int error_code = get_error_code_from_message(message);
    const char* solution = get_error_solution(error_code);
    
    // Python-like: if stack traces are enabled and we're not inside a try-block,
    // print a traceback before the concise error summary.
    ErrorSystem* system = error_system_get_global();
    int in_try_block = 0;
    int stack_enabled = 1;
    if (system) {
        stack_enabled = system->stack_trace_enabled ? 1 : 0;
        if (system->exception_context && system->exception_context->in_try_block) {
            in_try_block = 1;
        }
    }
    
    if (stack_enabled && !in_try_block && interpreter->call_stack) {
        // Collect frames to print most-recent-last
        // First count frames
        size_t count = 0;
        CallFrame* it = interpreter->call_stack;
        while (it) { count++; it = it->next; }
        
        const char* header = "Traceback (most recent call last):\n";
        printf("%s", header);
        
        // Copy pointers to an array
        CallFrame** frames = (CallFrame**)malloc(sizeof(CallFrame*) * count);
        size_t idx = 0;
        it = interpreter->call_stack;
        while (it && idx < count) { frames[idx++] = it; it = it->next; }
        
        // Print from oldest to newest (Python style)
        for (size_t i = count; i > 0; i--) {
            CallFrame* f = frames[i - 1];
            const char* file_name = f->file_name ? f->file_name : "<unknown>";
            const char* func_name = f->function_name ? f->function_name : "<unknown>";
            unsigned line_no = (unsigned)(f->line > 0 ? f->line : 0);
            printf("  File \"%s\", line %u, in %s\n", file_name, line_no, func_name);
        }
        free(frames);
    }
    
    // Column numbers should be 0-based for user display
    int display_column = column > 0 ? (column - 1) : 0;
    // Print a single concise line. Do not print a second plain error summary.
    printf("%s%sError:%s %s (Line %d, Column %d) [E%d]\n",
           ANSI_COLOR_RED, ANSI_COLOR_BOLD, ANSI_COLOR_RESET,
           message,
           line, display_column,
           error_code);
    // Keep the primary line concise
    // Print hint in a dimmer color to avoid clutter while still helpful
    if (solution && *solution) {
        printf("%sHint:%s %s\n", ANSI_COLOR_YELLOW, ANSI_COLOR_RESET, solution);
    }
    
    // Set error in interpreter
    interpreter_set_error(interpreter, message, line, column);
}

// Error system initialization and management
void error_system_initialize(void) {
    if (!global_error_system) {
        global_error_system = error_system_create();
        if (global_error_system) {
            // Keep debug printing off by default to avoid duplicate error lines in REPL
            error_enable_debug_mode(global_error_system, false);
            error_enable_stack_trace(global_error_system, true);
        }
    }
}

void error_system_cleanup(void) {
    if (global_error_system) {
        error_system_free(global_error_system);
        global_error_system = NULL;
    }
}

ErrorSystem* error_system_get_global(void) {
    if (!global_error_system) {
        error_system_initialize();
    }
    return global_error_system;
}

// Enhanced error reporting with new error system
void interpreter_report_error_enhanced_v2(Interpreter* interpreter, int error_code, const char* message, 
                                         const char* file_name, int line, int column) {
    if (!interpreter || !message) return;
    
    ErrorSystem* system = error_system_get_global();
    if (!system) return;
    
    // Map old error codes to new error codes
    ErrorCode new_code;
    ErrorSeverity severity;
    ErrorCategory category;
    
    if (error_code >= 1000 && error_code <= 1999) {
        new_code = ERROR_SYNTAX_UNEXPECTED_TOKEN;
        severity = ERROR_SEVERITY_ERROR;
        category = ERROR_CATEGORY_SYNTAX;
    } else if (error_code >= 2000 && error_code <= 2999) {
        new_code = ERROR_SYNTAX_UNEXPECTED_TOKEN;
        severity = ERROR_SEVERITY_ERROR;
        category = ERROR_CATEGORY_SYNTAX;
    } else if (error_code >= 3000 && error_code <= 3999) {
        new_code = ERROR_SEMANTIC_UNDEFINED_VARIABLE;
        severity = ERROR_SEVERITY_ERROR;
        category = ERROR_CATEGORY_SEMANTIC;
    } else if (error_code >= 4000 && error_code <= 4999) {
        new_code = ERROR_RUNTIME_DIVISION_BY_ZERO;
        severity = ERROR_SEVERITY_ERROR;
        category = ERROR_CATEGORY_RUNTIME;
    } else {
        new_code = ERROR_USER_DEFINED;
        severity = ERROR_SEVERITY_ERROR;
        category = ERROR_CATEGORY_USER;
    }
    
    // Create error with new system
    ErrorInfo* error = error_create(new_code, severity, category, message, file_name, line, column);
    if (error) {
        // Add suggestion
        const char* suggestion = get_error_solution(error_code);
        if (suggestion) {
            error_add_suggestion(error, suggestion);
        }
        
        // Add context
        char context[256];
        const char* function_name = "main";
        if (interpreter->call_stack && interpreter->call_stack->function_name) {
            function_name = interpreter->call_stack->function_name;
        }
        snprintf(context, sizeof(context), "Interpreter error in function: %s", function_name);
        error_add_context(error, context);
        
        // Report error
        error_report(system, error);
    }
    
    // Also set error in interpreter for backward compatibility
    interpreter_set_error(interpreter, message, line, column);
}

void error_colors_enable(int enable) {
    g_error_colors_enabled = enable ? 1 : 0;
}

// Auto-disable colors when not writing to a TTY
static int error_colors_should_enable(void) {
    #if defined(__unix__) || defined(__linux__) || defined(__APPLE__)
    extern int isatty(int);
    extern int fileno(FILE *);
    if (!isatty(fileno(stderr)) && !isatty(fileno(stdout))) {
        return 0;
    }
    #endif
    return 1;
}

// Print last error with full details (stack/context) if available
void error_print_last(void) {
    ErrorSystem* system = error_system_get_global();
    if (!system || system->error_count == 0) return;
    // Respect auto color policy unless explicitly disabled
    if (!error_colors_should_enable()) {
        g_error_colors_enabled = 0;
    }
    ErrorInfo* last = system->errors[system->error_count - 1];
    error_print(last);
}

// Try/catch/finally support for interpreter
void interpreter_enter_try(Interpreter* interpreter) {
    ErrorSystem* system = error_system_get_global();
    if (system && system->exception_context) {
        exception_enter_try(system->exception_context);
    }
}

void interpreter_exit_try(Interpreter* interpreter) {
    ErrorSystem* system = error_system_get_global();
    if (system && system->exception_context) {
        exception_exit_try(system->exception_context);
    }
}

void interpreter_enter_catch(Interpreter* interpreter, const char* variable_name) {
    ErrorSystem* system = error_system_get_global();
    if (system && system->exception_context) {
        exception_enter_catch(system->exception_context, variable_name);
    }
}

void interpreter_exit_catch(Interpreter* interpreter) {
    ErrorSystem* system = error_system_get_global();
    if (system && system->exception_context) {
        exception_exit_catch(system->exception_context);
    }
}

void interpreter_enter_finally(Interpreter* interpreter) {
    ErrorSystem* system = error_system_get_global();
    if (system && system->exception_context) {
        exception_enter_finally(system->exception_context);
    }
}

void interpreter_exit_finally(Interpreter* interpreter) {
    ErrorSystem* system = error_system_get_global();
    if (system && system->exception_context) {
        exception_exit_finally(system->exception_context);
    }
}

int interpreter_has_exception(Interpreter* interpreter) {
    ErrorSystem* system = error_system_get_global();
    return system ? (exception_has_error(system) ? 1 : 0) : 0;
}

ErrorInfo* interpreter_catch_exception(Interpreter* interpreter) {
    ErrorSystem* system = error_system_get_global();
    return system ? exception_catch(system) : NULL;
}

void interpreter_clear_exception(Interpreter* interpreter) {
    ErrorSystem* system = error_system_get_global();
    if (system) {
        exception_clear(system);
    }
}

// Enhanced error reporting with comprehensive error handling
void interpreter_report_error_comprehensive(Interpreter* interpreter, int error_code, const char* message, 
                                          const char* file_name, int line, int column, const char* context) {
    if (!interpreter) return;
    
    // Set error state
    interpreter->has_error = 1;
    if (interpreter->error_message) {
        free(interpreter->error_message);
    }
    interpreter->error_message = strdup(message);
    interpreter->error_line = line;
    interpreter->error_column = column;
    
    // Enhanced error reporting with comprehensive information
    printf("\033[1;31m[ERROR %d]\033[0m %s\n", error_code, message);
    printf("\033[1;33mLocation\033[0m: %s:%d:%d\n", file_name ? file_name : "<unknown>", line, column);
    
    // Add context if provided
    if (context) {
        printf("\033[1;35mContext\033[0m: %s\n", context);
    }
    
    // Add specific suggestions based on error code
    switch (error_code) {
        case MYCO_ERROR_UNDEFINED_VARIABLE:
            printf("\033[1;36mSuggestion\033[0m: Declare the variable before use or check spelling\n");
            break;
        case MYCO_ERROR_UNDEFINED_FUNCTION:
            printf("\033[1;36mSuggestion\033[0m: Define the function or check spelling\n");
            break;
        case MYCO_ERROR_TYPE_MISMATCH:
            printf("\033[1;36mSuggestion\033[0m: Use compatible types or add type conversion\n");
            break;
        case MYCO_ERROR_DIVISION_BY_ZERO:
            printf("\033[1;36mSuggestion\033[0m: Check divisor before division\n");
            break;
        case MYCO_ERROR_ARRAY_BOUNDS:
            printf("\033[1;36mSuggestion\033[0m: Validate array bounds before access\n");
            break;
        case MYCO_ERROR_OUT_OF_MEMORY:
            printf("\033[1;36mSuggestion\033[0m: Free unused memory or increase available memory\n");
            break;
        case MYCO_ERROR_FILE_NOT_FOUND:
            printf("\033[1;36mSuggestion\033[0m: Check file path and permissions\n");
            break;
        default:
            printf("\033[1;36mSuggestion\033[0m: Review the error and check your code\n");
            break;
    }
    
    // Add stack trace if available
    if (interpreter->call_stack) {
        printf("\033[1;37mStack Trace\033[0m:\n");
        CallFrame* frame = interpreter->call_stack;
        int depth = 0;
        while (frame && depth < 10) {
        printf("  %d. %s in %s:%d\n", depth + 1, 
               frame->function_name ? frame->function_name : "<unknown>",
               frame->file_name ? frame->file_name : "<unknown>",
               0);
            frame = frame->next;
            depth++;
        }
    }
}

// Safe division with error handling
double safe_divide(double a, double b, Interpreter* interpreter, int line, int column) {
    if (b == 0.0) {
        interpreter_report_error_comprehensive(interpreter, MYCO_ERROR_DIVISION_BY_ZERO, 
                                            "Division by zero", interpreter->current_filename, line, column, NULL);
        return 0.0;
    }
    return a / b;
}

// Safe array access with bounds checking
Value safe_array_access(Value* array, int index, Interpreter* interpreter, int line, int column) {
    if (array->type != VALUE_ARRAY) {
        interpreter_report_error_comprehensive(interpreter, MYCO_ERROR_INVALID_OPERATION, 
                                            "Not an array", interpreter->current_filename, line, column, NULL);
        return value_create_null();
    }
    
    if (index < 0 || index >= (int)array->data.array_value.count) {
        char context[256];
        snprintf(context, sizeof(context), "Index: %d, Array size: %zu", index, array->data.array_value.count);
        interpreter_report_error_comprehensive(interpreter, MYCO_ERROR_ARRAY_BOUNDS, 
                                            "Array index out of bounds", interpreter->current_filename, line, column, 
                                            context);
        return value_create_null();
    }
    
    return *((Value*)array->data.array_value.elements[index]);
}

// Safe memory allocation with error handling
void* safe_malloc(size_t size, Interpreter* interpreter, const char* context, int line, int column) {
    void* ptr = malloc(size);
    if (!ptr) {
        interpreter_report_error_comprehensive(interpreter, MYCO_ERROR_OUT_OF_MEMORY, 
                                            "Memory allocation failed", interpreter->current_filename, line, column, 
                                            context);
        return NULL;
    }
    return ptr;
}

// Safe string operations with bounds checking
char* safe_strdup(const char* str, Interpreter* interpreter, const char* context, int line, int column) {
    if (!str) {
        interpreter_report_error_comprehensive(interpreter, MYCO_ERROR_NULL_POINTER, 
                                            "Null string pointer", interpreter->current_filename, line, column, 
                                            context);
        return NULL;
    }
    
    char* result = strdup(str);
    if (!result) {
        interpreter_report_error_comprehensive(interpreter, MYCO_ERROR_OUT_OF_MEMORY, 
                                            "String duplication failed", interpreter->current_filename, line, column, 
                                            context);
        return NULL;
    }
    return result;
}

// Safe file operations with error handling
FILE* safe_fopen(const char* filename, const char* mode, Interpreter* interpreter, int line, int column) {
    if (!filename) {
        interpreter_report_error_comprehensive(interpreter, MYCO_ERROR_NULL_POINTER, 
                                            "Null filename", interpreter->current_filename, line, column, 
                                            "File operation");
        return NULL;
    }
    
    FILE* file = fopen(filename, mode);
    if (!file) {
        int error_code = MYCO_ERROR_FILE_NOT_FOUND;
        const char* error_msg = "File not found";
        
        if (errno == EACCES) {
            error_code = MYCO_ERROR_FILE_PERMISSION;
            error_msg = "Permission denied";
        } else if (errno == ENOSPC) {
            error_code = MYCO_ERROR_SYSTEM_ERROR;
            error_msg = "No space left on device";
        }
        
        char context[256];
        snprintf(context, sizeof(context), "File: %s", filename);
        interpreter_report_error_comprehensive(interpreter, error_code, error_msg, 
                                            interpreter->current_filename, line, column, 
                                            context);
        return NULL;
    }
    return file;
}

// Safe object property access with error handling
Value safe_object_access(Value* object, const char* property, Interpreter* interpreter, int line, int column) {
    if (!object) {
        interpreter_report_error_comprehensive(interpreter, MYCO_ERROR_NULL_POINTER, 
                                            "Null object", interpreter->current_filename, line, column, 
                                            "Property access");
        return value_create_null();
    }
    
    if (object->type != VALUE_OBJECT) {
        interpreter_report_error_comprehensive(interpreter, MYCO_ERROR_INVALID_OPERATION, 
                                            "Not an object", interpreter->current_filename, line, column, 
                                            "Property access");
        return value_create_null();
    }
    
    if (!property) {
        interpreter_report_error_comprehensive(interpreter, MYCO_ERROR_NULL_POINTER, 
                                            "Null property name", interpreter->current_filename, line, column, 
                                            "Property access");
        return value_create_null();
    }
    
    // Search for property
    for (size_t i = 0; i < object->data.object_value.count; i++) {
        if (object->data.object_value.keys[i] && 
            strcmp(object->data.object_value.keys[i], property) == 0) {
            return *((Value*)object->data.object_value.values[i]);
        }
    }
    
    char context[256];
    snprintf(context, sizeof(context), "Property: %s", property);
    interpreter_report_error_comprehensive(interpreter, MYCO_ERROR_INVALID_KEY, 
                                        "Property not found", interpreter->current_filename, line, column, 
                                        context);
    return value_create_null();
}

// Enhanced error handling for arithmetic operations
Value safe_arithmetic_operation(Value* left, Value* right, char operation, Interpreter* interpreter, int line, int column) {
    if (!left || !right) {
        interpreter_report_error_comprehensive(interpreter, MYCO_ERROR_NULL_POINTER, 
                                            "Null operand in arithmetic operation", interpreter->current_filename, line, column, 
                                            "Arithmetic operation");
        return value_create_null();
    }
    
    // Check if both operands are numbers
    if (left->type != VALUE_NUMBER || right->type != VALUE_NUMBER) {
        char context[256];
        snprintf(context, sizeof(context), "Left: %d, Right: %d", left->type, right->type);
        interpreter_report_error_comprehensive(interpreter, MYCO_ERROR_TYPE_MISMATCH, 
                                            "Non-numeric operands in arithmetic operation", interpreter->current_filename, line, column, 
                                            context);
        return value_create_null();
    }
    
    double a = left->data.number_value;
    double b = right->data.number_value;
    double result = 0.0;
    
    switch (operation) {
        case '+':
            result = a + b;
            break;
        case '-':
            result = a - b;
            break;
        case '*':
            result = a * b;
            break;
        case '/':
            if (b == 0.0) {
                interpreter_report_error_comprehensive(interpreter, MYCO_ERROR_DIVISION_BY_ZERO, 
                                                    "Division by zero", interpreter->current_filename, line, column, 
                                                    "Arithmetic operation");
                return value_create_null();
            }
            result = a / b;
            break;
        case '%':
            if (b == 0.0) {
                interpreter_report_error_comprehensive(interpreter, MYCO_ERROR_DIVISION_BY_ZERO, 
                                                    "Modulo by zero", interpreter->current_filename, line, column, 
                                                    "Arithmetic operation");
                return value_create_null();
            }
            result = fmod(a, b);
            break;
        default:
            interpreter_report_error_comprehensive(interpreter, MYCO_ERROR_INVALID_OPERATION, 
                                                "Invalid arithmetic operation", interpreter->current_filename, line, column, 
                                                "Arithmetic operation");
            return value_create_null();
    }
    
    return value_create_number(result);
}

// Enhanced error handling for comparison operations
Value safe_comparison_operation(Value* left, Value* right, char operation, Interpreter* interpreter, int line, int column) {
    if (!left || !right) {
        interpreter_report_error_comprehensive(interpreter, MYCO_ERROR_NULL_POINTER, 
                                            "Null operand in comparison operation", interpreter->current_filename, line, column, 
                                            "Comparison operation");
        return value_create_null();
    }
    
    bool result = false;
    
    // Handle different type combinations
    if (left->type == right->type) {
        switch (left->type) {
            case VALUE_NUMBER:
                switch (operation) {
                    case '>': result = left->data.number_value > right->data.number_value; break;
                    case '<': result = left->data.number_value < right->data.number_value; break;
                    case '=': result = left->data.number_value == right->data.number_value; break;
                    case '!': result = left->data.number_value != right->data.number_value; break;
                    case 'g': result = left->data.number_value >= right->data.number_value; break;
                    case 'l': result = left->data.number_value <= right->data.number_value; break;
                    default:
                        interpreter_report_error_comprehensive(interpreter, MYCO_ERROR_INVALID_OPERATION, 
                                                            "Invalid comparison operation", interpreter->current_filename, line, column, 
                                                            "Comparison operation");
                        return value_create_null();
                }
                break;
            case VALUE_STRING:
                switch (operation) {
                    case '=': result = strcmp(left->data.string_value, right->data.string_value) == 0; break;
                    case '!': result = strcmp(left->data.string_value, right->data.string_value) != 0; break;
                    default:
                        interpreter_report_error_comprehensive(interpreter, MYCO_ERROR_INVALID_OPERATION, 
                                                            "Invalid string comparison operation", interpreter->current_filename, line, column, 
                                                            "Comparison operation");
                        return value_create_null();
                }
                break;
            case VALUE_BOOLEAN:
                switch (operation) {
                    case '=': result = left->data.boolean_value == right->data.boolean_value; break;
                    case '!': result = left->data.boolean_value != right->data.boolean_value; break;
                    default:
                        interpreter_report_error_comprehensive(interpreter, MYCO_ERROR_INVALID_OPERATION, 
                                                            "Invalid boolean comparison operation", interpreter->current_filename, line, column, 
                                                            "Comparison operation");
                        return value_create_null();
                }
                break;
            default:
                interpreter_report_error_comprehensive(interpreter, MYCO_ERROR_TYPE_MISMATCH, 
                                                    "Unsupported type for comparison", interpreter->current_filename, line, column, 
                                                    "Comparison operation");
                return value_create_null();
        }
    } else {
        interpreter_report_error_comprehensive(interpreter, MYCO_ERROR_TYPE_MISMATCH, 
                                            "Type mismatch in comparison operation", interpreter->current_filename, line, column, 
                                            "Comparison operation");
        return value_create_null();
    }
    
    return value_create_boolean(result);
}
