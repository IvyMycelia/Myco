# Myco Development Guidelines

## **Overview**
This document outlines the development guidelines, coding standards, and best practices for the Myco programming language project.

## **Architecture Principles**

### **1. Modular Design**
- **Component Isolation**: Each component (lexer, parser, interpreter, compiler) should be independently testable
- **Clear Interfaces**: Components communicate through well-defined interfaces
- **Minimal Dependencies**: Reduce interdependencies between components
- **Single Responsibility**: Each module has one clear purpose

### **2. Error Handling Standards**
- **Use Standardized Error System**: Always use `std_error_report()` instead of `interpreter_set_error()`
- **Error Codes**: Use appropriate error codes from `standardized_errors.h`
- **Component Identification**: Always specify the component name in error reports
- **Consistent Format**: Follow the format: `std_error_report(ERROR_CODE, "component", "function", "message", line, column)`

### **3. Memory Management**
- **Use Shared Utilities**: Use `shared_malloc()`, `shared_realloc()`, `shared_free()` for memory operations
- **Safe Operations**: Use `shared_strdup()`, `shared_strndup()` for string operations
- **Validation**: Use `shared_validate_not_null()` before dereferencing pointers
- **Error Reporting**: Memory allocation failures are automatically reported

## **Coding Standards**

### **1. File Organization**
```
src/
├── core/           # Core language components
├── libs/           # Standard library implementations
├── compilation/    # Compiler and code generation
├── cli/           # Command-line interface
├── utils/         # Shared utilities
└── runtime/       # Runtime system
```

### **2. Naming Conventions**
- **Functions**: `snake_case` (e.g., `builtin_array_push`)
- **Variables**: `snake_case` (e.g., `array_arg`)
- **Constants**: `UPPER_CASE` (e.g., `ERROR_INVALID_ARGUMENT`)
- **Types**: `PascalCase` (e.g., `Interpreter`, `ASTNode`)

### **3. Error Handling Patterns**
```c
// ✅ Good: Use standardized error reporting
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

### **4. Memory Management Patterns**
```c
// ✅ Good: Use shared utilities
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

### **5. Validation Patterns**
```c
// ✅ Good: Use validation utilities
if (!shared_validate_not_null(ptr, "component", "function")) {
    std_error_report(ERROR_NULL_POINTER, "component", "function", 
                    "Null pointer validation failed", line, column);
    return value_create_null();
}

// ❌ Bad: Manual validation
if (!ptr) {
    // Inconsistent error reporting
    return NULL;
}
```

## **Development Workflow**

### **1. Before Making Changes**
- **Read Guidelines**: Review this document and related documentation
- **Understand Architecture**: Know how your changes affect other components
- **Plan Testing**: Identify what needs to be tested

### **2. During Development**
- **Use Standards**: Follow error handling and memory management patterns
- **Test Frequently**: Run `make` and `./bin/myco pass.myco` regularly
- **Document Changes**: Update relevant documentation
- **Check Dependencies**: Ensure changes don't break other components

### **3. After Changes**
- **Full Test Suite**: Run the complete test suite
- **Build Verification**: Ensure clean build with no errors
- **Performance Check**: Verify no significant performance regression
- **Documentation Update**: Update any affected documentation

## **Component-Specific Guidelines**

### **Core Components**
- **Interpreter**: Use `std_error_report()` for all error conditions
- **Parser**: Report syntax errors with appropriate error codes
- **Lexer**: Report lexical errors with line/column information
- **Type Checker**: Use semantic error codes for type mismatches

### **Library Components**
- **Array Library**: Use `ERROR_INVALID_ARGUMENT` for type mismatches
- **String Library**: Use `ERROR_TYPE_MISMATCH` for invalid types
- **File Library**: Use `ERROR_FILE_NOT_FOUND` for file operations
- **Math Library**: Use `ERROR_DIVISION_BY_ZERO` for mathematical errors

### **Compiler Components**
- **Code Generation**: Use `ERROR_INTERNAL_ERROR` for generation failures
- **Optimization**: Use `ERROR_NOT_IMPLEMENTED` for unimplemented features
- **Type Inference**: Use `ERROR_TYPE_MISMATCH` for inference failures

## **Testing Standards**

### **1. Test Coverage**
- **Unit Tests**: Test individual functions and components
- **Integration Tests**: Test component interactions
- **Regression Tests**: Ensure existing functionality works
- **Performance Tests**: Verify performance requirements

### **2. Test Execution**
```bash
# Build and test
make clean && make
./bin/myco pass.myco

# Expected output: 100% success rate
```

### **3. Test Development**
- **Add New Tests**: Add tests for new functionality
- **Update Existing Tests**: Modify tests when behavior changes
- **Document Test Cases**: Explain what each test validates

## **Performance Guidelines**

### **1. Memory Efficiency**
- **Use Shared Utilities**: Use optimized shared functions
- **Avoid Memory Leaks**: Use `shared_free()` consistently
- **Minimize Allocations**: Reuse objects when possible

### **2. Execution Speed**
- **Profile Hot Paths**: Identify and optimize critical code paths
- **Use Efficient Algorithms**: Choose appropriate data structures
- **Avoid Redundant Operations**: Cache results when possible

### **3. Compilation Speed**
- **Minimize Dependencies**: Reduce include complexity
- **Use Forward Declarations**: When possible, avoid full includes
- **Optimize Build Process**: Use appropriate compiler flags

## **Debugging Guidelines**

### **1. Debug Output**
```c
// ✅ Good: Use component-specific debugging
shared_debug_print("parser", "parse_statement", "Starting to parse statement");
shared_debug_printf("parser", "parse_statement", "Parsed %d tokens", token_count);

// ❌ Bad: Direct printf
printf("Debug: Starting to parse statement\n");
```

### **2. Error Investigation**
- **Use Standardized Errors**: Use error codes and suggestions
- **Check Component Logs**: Use component-specific debug settings
- **Profile Performance**: Use timing utilities for performance issues

### **3. Development Tools**
```bash
# Enable component debugging
shared_config_set_component_debug("parser", true);

# Enable verbose output
shared_config_set_verbose(true);

# Enable performance timing
shared_time_measure_start("operation_name");
// ... operation code ...
shared_time_measure_end("operation_name");
```

## **Documentation Standards**

### **1. Code Documentation**
- **Function Comments**: Explain purpose, parameters, and return values
- **Complex Logic**: Document non-obvious algorithms
- **Error Conditions**: Document when functions can fail

### **2. API Documentation**
- **Public Interfaces**: Document all public functions
- **Usage Examples**: Provide clear usage examples
- **Error Handling**: Document error conditions and codes

### **3. Architecture Documentation**
- **Component Overview**: Explain component responsibilities
- **Data Flow**: Document how data flows between components
- **Dependencies**: Document component dependencies

## **Common Pitfalls to Avoid**

### **1. Error Handling**
- ❌ **Don't**: Use `interpreter_set_error()` (deprecated)
- ❌ **Don't**: Use `fprintf(stderr, ...)` for errors
- ❌ **Don't**: Ignore error conditions
- ✅ **Do**: Use `std_error_report()` with appropriate error codes

### **2. Memory Management**
- ❌ **Don't**: Use `malloc()`/`free()` directly
- ❌ **Don't**: Forget to free allocated memory
- ❌ **Don't**: Use freed memory
- ✅ **Do**: Use shared utilities with automatic error reporting

### **3. Component Design**
- ❌ **Don't**: Create tight coupling between components
- ❌ **Don't**: Use global variables for component state
- ❌ **Don't**: Ignore component boundaries
- ✅ **Do**: Use clear interfaces and minimal dependencies

## **Getting Help**

### **1. Documentation**
- **This Guide**: Development guidelines and standards
- **API Reference**: Function and interface documentation
- **Architecture Docs**: System design and component overview

### **2. Debugging Tools**
- **Error System**: Use standardized error reporting
- **Debug Utilities**: Use component-specific debugging
- **Performance Tools**: Use timing and profiling utilities

### **3. Testing**
- **Test Suite**: Run `./bin/myco pass.myco` for comprehensive testing
- **Unit Tests**: Test individual components
- **Integration Tests**: Test component interactions

---

**Remember**: The goal is to create a maintainable, efficient, and reliable programming language. Follow these guidelines to ensure consistent, high-quality development.

