#include "interpreter.h"
#include "error_system.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Global error system instance
static ErrorSystem* global_error_system = NULL;

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
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_WHITE   "\x1b[37m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_COLOR_BOLD    "\x1b[1m"
#define ANSI_COLOR_DIM     "\x1b[2m"

// Enhanced error reporting with colors and formatting
void interpreter_report_error_enhanced(Interpreter* interpreter, const char* message, int line, int column) {
    if (!interpreter || !message) return;
    
    // Get error code and details
    int error_code = get_error_code_from_message(message);
    const char* fungus_name = get_fungus_error_name(error_code);
    const char* solution = get_error_solution(error_code);
    
    // Print error with colors and formatting
    // Column numbers should be 0-based for user display
    printf("%sError (%s)%s at Line %d, Column %d: %s%s%s\n", 
           ANSI_COLOR_RED ANSI_COLOR_BOLD, 
           fungus_name, 
           ANSI_COLOR_RESET,
           line, column - 1, 
           ANSI_COLOR_RED, 
           message, 
           ANSI_COLOR_RESET);
    
    // Print solution in a different color
    printf("%sSolution: %s%s\n", 
           ANSI_COLOR_YELLOW, 
           solution, 
           ANSI_COLOR_RESET);
    
    // Set error in interpreter
    interpreter_set_error(interpreter, message, line, column);
}

// Error system initialization and management
void error_system_initialize(void) {
    if (!global_error_system) {
        global_error_system = error_system_create();
        if (global_error_system) {
            error_enable_debug_mode(global_error_system, true);
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
