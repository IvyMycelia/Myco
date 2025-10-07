# Myco Coding Standards

## **Overview**
This document defines the coding standards, style guidelines, and best practices for the Myco programming language project.

## **General Principles**

### **1. Consistency**
- **Follow Established Patterns**: Use existing code patterns as examples
- **Maintain Style**: Keep consistent formatting and naming
- **Document Decisions**: Explain non-standard approaches

### **2. Readability**
- **Clear Names**: Use descriptive variable and function names
- **Logical Structure**: Organize code in logical sections
- **Minimal Complexity**: Keep functions focused and simple

### **3. Maintainability**
- **Modular Design**: Break complex functionality into smaller functions
- **Clear Interfaces**: Define clear function signatures
- **Error Handling**: Use standardized error reporting

## **Code Style**

### **1. Indentation and Formatting**
```c
// ✅ Good: Consistent indentation (4 spaces)
if (condition) {
    if (nested_condition) {
        do_something();
    } else {
        do_alternative();
    }
}

// ❌ Bad: Inconsistent indentation
if (condition) {
  if (nested_condition) {
      do_something();
  }else{
    do_alternative();
  }
}
```

### **2. Function Formatting**
```c
// ✅ Good: Clear function signature
Value builtin_array_push(Interpreter* interpreter, Value* args, size_t arg_count, 
                        int line, int column) {
    // Function body
}

// ❌ Bad: Unclear parameters
Value builtin_array_push(Interpreter*i,Value*a,size_t ac,int l,int c){
    // Function body
}
```

### **3. Variable Declarations**
```c
// ✅ Good: One variable per line, clear initialization
Value array_arg = args[0];
Value element = args[1];
size_t array_length = array_arg.data.array_value.count;

// ❌ Bad: Multiple declarations, unclear initialization
Value array_arg, element;
size_t array_length;
```

## **Naming Conventions**

### **1. Functions**
```c
// ✅ Good: Descriptive, snake_case
Value builtin_array_push(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value handle_super_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name);
void std_error_report(int error_code, const char* component, const char* function, const char* message, int line, int column);

// ❌ Bad: Unclear, inconsistent
Value f(Interpreter* i, Value* a, size_t ac, int l, int c);
Value handleSuperMethodCall(Interpreter* interpreter, ASTNode* call_node, const char* method_name);
```

### **2. Variables**
```c
// ✅ Good: Descriptive names
Value array_arg = args[0];
Value element = args[1];
size_t array_length = array_arg.data.array_value.count;
char* result_string = NULL;

// ❌ Bad: Unclear abbreviations
Value a = args[0];
Value e = args[1];
size_t len = array_arg.data.array_value.count;
char* s = NULL;
```

### **3. Constants and Macros**
```c
// ✅ Good: UPPER_CASE with descriptive names
#define ERROR_ARGUMENT_COUNT 3005
#define MAX_STACK_DEPTH 1000
#define ANSI_COLOR_RED "\x1b[31m"

// ❌ Bad: Unclear or inconsistent
#define ERR_ARG 3005
#define MAX_STACK 1000
#define RED "\x1b[31m"
```

### **4. Types and Structures**
```c
// ✅ Good: PascalCase for types
typedef struct {
    FILE* file;
    char* filename;
    char* mode;
    int is_open;
} FileHandle;

typedef enum {
    ERROR_SEVERITY_INFO,
    ERROR_SEVERITY_WARNING,
    ERROR_SEVERITY_ERROR,
    ERROR_SEVERITY_CRITICAL
} ErrorSeverity;

// ❌ Bad: Inconsistent naming
typedef struct {
    FILE* file;
    char* filename;
    char* mode;
    int is_open;
} file_handle;

typedef enum {
    error_severity_info,
    ERROR_SEVERITY_WARNING,
    Error_Severity_Error,
    ERROR_SEVERITY_CRITICAL
} error_severity;
```

## **Error Handling Standards**

### **1. Standardized Error Reporting**
```c
// ✅ Good: Use standardized error system
if (arg_count != 2) {
    std_error_report(ERROR_ARGUMENT_COUNT, "array", "builtin_array_push", 
                    "push() requires exactly 2 arguments", line, column);
    return value_create_null();
}

// ❌ Bad: Old error handling
if (arg_count != 2) {
    interpreter_set_error(interpreter, "push() requires exactly 2 arguments", line, column);
    return value_create_null();
}
```

### **2. Error Code Selection**
```c
// ✅ Good: Appropriate error codes
std_error_report(ERROR_ARGUMENT_COUNT, "array", "builtin_array_push", 
                "push() requires exactly 2 arguments", line, column);

std_error_report(ERROR_INVALID_ARGUMENT, "array", "builtin_array_push", 
                "push() first argument must be an array", line, column);

std_error_report(ERROR_TYPE_MISMATCH, "type_checker", "check_assignment", 
                "Cannot assign String to Int variable", line, column);

// ❌ Bad: Generic or incorrect error codes
std_error_report(ERROR_INTERNAL_ERROR, "array", "builtin_array_push", 
                "push() requires exactly 2 arguments", line, column);
```

### **3. Component Identification**
```c
// ✅ Good: Clear component names
std_error_report(ERROR_ARGUMENT_COUNT, "array", "builtin_array_push", message, line, column);
std_error_report(ERROR_INVALID_ARGUMENT, "string", "builtin_string_upper", message, line, column);
std_error_report(ERROR_FILE_NOT_FOUND, "file", "builtin_file_open", message, line, column);

// ❌ Bad: Unclear or generic component names
std_error_report(ERROR_ARGUMENT_COUNT, "lib", "function", message, line, column);
std_error_report(ERROR_INVALID_ARGUMENT, "unknown", "unknown", message, line, column);
```

## **Memory Management Standards**

### **1. Use Shared Utilities**
```c
// ✅ Good: Use shared utilities with error reporting
char* result = shared_strdup(input_string);
if (!result) {
    std_error_report(ERROR_OUT_OF_MEMORY, "component", "function", 
                    "Failed to duplicate string", line, column);
    return value_create_null();
}

// ❌ Bad: Direct memory operations
char* result = malloc(strlen(input_string) + 1);
if (!result) {
    // No error reporting
    return NULL;
}
```

### **2. Safe Memory Operations**
```c
// ✅ Good: Safe operations with validation
if (!shared_validate_not_null(ptr, "component", "function")) {
    std_error_report(ERROR_NULL_POINTER, "component", "function", 
                    "Null pointer validation failed", line, column);
    return value_create_null();
}

// ❌ Bad: Unsafe operations
if (!ptr) {
    // Inconsistent error reporting
    return NULL;
}
```

### **3. Memory Cleanup**
```c
// ✅ Good: Consistent cleanup
char* temp_string = shared_strdup(input);
if (temp_string) {
    // Use temp_string
    shared_free(temp_string);
}

// ❌ Bad: Inconsistent cleanup
char* temp_string = malloc(strlen(input) + 1);
if (temp_string) {
    // Use temp_string
    free(temp_string);  // Inconsistent with shared utilities
}
```

## **Function Design Standards**

### **1. Function Signatures**
```c
// ✅ Good: Clear, consistent signatures
Value builtin_array_push(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
void std_error_report(int error_code, const char* component, const char* function, const char* message, int line, int column);
bool shared_validate_not_null(const void* ptr, const char* component, const char* function);

// ❌ Bad: Inconsistent or unclear signatures
Value f(Interpreter* i, Value* a, size_t ac, int l, int c);
void std_error_report(int code, const char* comp, const char* func, const char* msg, int line, int col);
bool validate(void* p, const char* c, const char* f);
```

### **2. Parameter Validation**
```c
// ✅ Good: Comprehensive validation
Value builtin_array_push(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    // Validate argument count
    if (arg_count != 2) {
        std_error_report(ERROR_ARGUMENT_COUNT, "array", "builtin_array_push", 
                        "push() requires exactly 2 arguments", line, column);
        return value_create_null();
    }
    
    // Validate argument types
    Value array_arg = args[0];
    if (array_arg.type != VALUE_ARRAY) {
        std_error_report(ERROR_INVALID_ARGUMENT, "array", "builtin_array_push", 
                        "push() first argument must be an array", line, column);
        return value_create_null();
    }
    
    // Function implementation
}

// ❌ Bad: Minimal or no validation
Value builtin_array_push(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    Value array_arg = args[0];
    Value element = args[1];
    // No validation
    // Function implementation
}
```

### **3. Return Values**
```c
// ✅ Good: Consistent return patterns
Value builtin_array_push(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    // Validation with early returns
    if (arg_count != 2) {
        std_error_report(ERROR_ARGUMENT_COUNT, "array", "builtin_array_push", 
                        "push() requires exactly 2 arguments", line, column);
        return value_create_null();
    }
    
    // Implementation
    Value result = value_create_array(new_size);
    // ... populate result ...
    
    return result;
}

// ❌ Bad: Inconsistent return patterns
Value builtin_array_push(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        std_error_report(ERROR_ARGUMENT_COUNT, "array", "builtin_array_push", 
                        "push() requires exactly 2 arguments", line, column);
        return value_create_null();
    }
    
    // Implementation
    Value result = value_create_array(new_size);
    // ... populate result ...
    
    // Sometimes return result, sometimes return value_create_null()
    if (some_condition) {
        return result;
    } else {
        return value_create_null();  // Inconsistent
    }
}
```

## **Comment Standards**

### **1. Function Comments**
```c
// ✅ Good: Clear function documentation
/**
 * Pushes an element onto the end of an array.
 * 
 * @param interpreter The interpreter instance
 * @param args Array containing the array and element to push
 * @param arg_count Number of arguments (must be 2)
 * @param line Line number for error reporting
 * @param column Column number for error reporting
 * @return New array with the element added, or NULL on error
 */
Value builtin_array_push(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);

// ❌ Bad: No documentation or unclear comments
Value builtin_array_push(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column); // pushes element
```

### **2. Inline Comments**
```c
// ✅ Good: Explain complex logic
// Create a new array with all existing elements plus the new element
Value result = value_create_array(array_arg.data.array_value.count + 1);

// Copy all existing elements
for (size_t i = 0; i < array_arg.data.array_value.count; i++) {
    Value* existing_element = (Value*)array_arg.data.array_value.elements[i];
    if (existing_element) {
        Value cloned_element = value_clone(existing_element);
        value_array_push(&result, cloned_element);
        // Don't free the cloned element - it's now owned by the array
    }
}

// ❌ Bad: Obvious or missing comments
Value result = value_create_array(array_arg.data.array_value.count + 1); // create array
for (size_t i = 0; i < array_arg.data.array_value.count; i++) {
    // copy elements
}
```

### **3. TODO Comments**
```c
// ✅ Good: Clear TODO with context
// TODO: Implement proper array concatenation instead of no-op assignment
// This is a temporary fix to allow compilation
fill_array = fill_array;  // No-op to satisfy compiler

// ❌ Bad: Unclear TODO
// TODO: fix this
fill_array = fill_array;
```

## **Header File Standards**

### **1. Include Guards**
```c
// ✅ Good: Consistent include guards
#ifndef STANDARDIZED_ERRORS_H
#define STANDARDIZED_ERRORS_H
// ... header content ...
#endif // STANDARDIZED_ERRORS_H

// ❌ Bad: Inconsistent or missing guards
#ifndef ERROR_H
#define ERROR_H
// ... header content ...
#endif
```

### **2. Include Organization**
```c
// ✅ Good: Organized includes
#include "interpreter.h"
#include "environment.h"
#include "standardized_errors.h"
#include "shared_utilities.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// ❌ Bad: Random order
#include <stdio.h>
#include "interpreter.h"
#include <stdlib.h>
#include "shared_utilities.h"
#include <string.h>
#include "environment.h"
```

### **3. Function Declarations**
```c
// ✅ Good: Clear declarations with documentation
/**
 * Reports an error using the standardized error system.
 * 
 * @param error_code The error code from standardized_errors.h
 * @param component The component name (e.g., "array", "string")
 * @param function The function name where the error occurred
 * @param message The error message
 * @param line Line number where the error occurred
 * @param column Column number where the error occurred
 */
void std_error_report(int error_code, const char* component, const char* function,
                     const char* message, int line, int column);

// ❌ Bad: Unclear declarations
void std_error_report(int code, const char* comp, const char* func, const char* msg, int line, int col);
```

## **Testing Standards**

### **1. Test Function Names**
```c
// ✅ Good: Clear test function names
void test_array_push_success(void);
void test_array_push_invalid_arguments(void);
void test_array_push_memory_allocation(void);

// ❌ Bad: Unclear test names
void test1(void);
void test_array(void);
void test_push(void);
```

### **2. Test Organization**
```c
// ✅ Good: Organized test structure
void test_array_push_success(void) {
    // Setup
    Interpreter* interpreter = interpreter_create();
    Value args[2];
    args[0] = value_create_array(0);
    args[1] = value_create_string("test");
    
    // Execute
    Value result = builtin_array_push(interpreter, args, 2, 1, 1);
    
    // Verify
    assert(result.type == VALUE_ARRAY);
    assert(result.data.array_value.count == 1);
    
    // Cleanup
    value_free(&result);
    interpreter_free(interpreter);
}

// ❌ Bad: Unclear test structure
void test_array_push_success(void) {
    // test code
}
```

## **Performance Standards**

### **1. Efficient Algorithms**
```c
// ✅ Good: Efficient string operations
char* result = shared_strdup(input_string);
if (result) {
    // Process result
    shared_free(result);
}

// ❌ Bad: Inefficient operations
char* result = malloc(strlen(input_string) + 1);
strcpy(result, input_string);
// Process result
free(result);
```

### **2. Memory Efficiency**
```c
// ✅ Good: Reuse objects when possible
static char* temp_buffer = NULL;
static size_t temp_size = 0;

if (needed_size > temp_size) {
    temp_buffer = shared_realloc(temp_buffer, needed_size);
    temp_size = needed_size;
}

// ❌ Bad: Allocate new objects frequently
char* temp_buffer = malloc(needed_size);
// Use temp_buffer
free(temp_buffer);
```

## **Common Anti-Patterns to Avoid**

### **1. Error Handling Anti-Patterns**
```c
// ❌ Bad: Silent failures
if (!ptr) {
    return NULL;  // No error reporting
}

// ❌ Bad: Inconsistent error handling
if (condition1) {
    interpreter_set_error(interpreter, "Error 1", line, column);
} else if (condition2) {
    std_error_report(ERROR_INTERNAL_ERROR, "component", "function", "Error 2", line, column);
}

// ❌ Bad: Generic error messages
std_error_report(ERROR_INTERNAL_ERROR, "component", "function", "Error occurred", line, column);
```

### **2. Memory Management Anti-Patterns**
```c
// ❌ Bad: Memory leaks
char* result = malloc(size);
// Use result
// Forgot to free(result)

// ❌ Bad: Double free
char* result = malloc(size);
free(result);
free(result);  // Double free

// ❌ Bad: Use after free
char* result = malloc(size);
free(result);
strcpy(result, "test");  // Use after free
```

### **3. Code Organization Anti-Patterns**
```c
// ❌ Bad: God functions
Value process_everything(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    // 500 lines of code doing everything
}

// ❌ Bad: Global variables
static Interpreter* global_interpreter = NULL;
static Value* global_args = NULL;

// ❌ Bad: Tight coupling
// Component A directly accesses Component B's internal data
```

## **Code Review Checklist**

### **1. Style and Formatting**
- [ ] Consistent indentation (4 spaces)
- [ ] Clear function signatures
- [ ] Descriptive variable names
- [ ] Proper include organization

### **2. Error Handling**
- [ ] Uses `std_error_report()` instead of `interpreter_set_error()`
- [ ] Appropriate error codes
- [ ] Clear component identification
- [ ] Consistent error patterns

### **3. Memory Management**
- [ ] Uses shared utilities (`shared_malloc`, `shared_free`)
- [ ] Proper validation (`shared_validate_not_null`)
- [ ] No memory leaks
- [ ] No use after free

### **4. Function Design**
- [ ] Clear function signatures
- [ ] Comprehensive parameter validation
- [ ] Consistent return patterns
- [ ] Appropriate function length

### **5. Documentation**
- [ ] Function documentation
- [ ] Complex logic comments
- [ ] Clear TODO comments
- [ ] Updated related documentation

---

**Remember**: These standards ensure consistent, maintainable, and reliable code. Follow them to contribute effectively to the Myco project.

