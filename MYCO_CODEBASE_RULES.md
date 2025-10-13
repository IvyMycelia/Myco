# Myco Lang Codebase Rules and Constraints

This document contains the comprehensive rules and constraints that MUST be followed when editing the Myco Lang codebase. These rules are derived from the codebase structure and language design principles.

## CRITICAL RULES - NEVER BREAK THESE

### 1. File Organization Rules
- **ALL .h files MUST be in the `include/` directory**
- **ALL .c files MUST be in the `src/` directory**
- **Each file should serve at most 2 highly related functional purposes**
- **Files should NOT exceed 2,000 lines of code (LoC)**
- **If a file exceeds 2,000 LoC, it MUST be split into smaller, focused files**
- **File names should be specific, not abstract**
- **Maintain code readability through proper organization**

### 2. Memory Management Rules
- **NEVER** cause segmentation faults anywhere in the codebase
- **ALWAYS** use proper memory allocation and deallocation patterns
- **ALWAYS** check for NULL pointers before dereferencing
- **ALWAYS** free allocated memory in the correct order (child objects first)
- **NEVER** use `free()` on memory that wasn't allocated with `malloc()`, `calloc()`, or `realloc()`
- **ALWAYS** set pointers to NULL after freeing to prevent double-free errors

### 2. Language Syntax Rules (from MYCO_SYNTAX_REFERENCE.md)
- **Comments**: Use `#` for single-line comments, NEVER `//`
- **Statements**: End with semicolon `;` (except in blocks)
- **Blocks**: Use `end` to close blocks, NEVER `{}`
- **Case sensitive**: `let` not `Let`, `func` not `Func`
- **Booleans**: `True` and `False` (capitalized), NEVER `true`/`false`
- **Null**: `Null` (capitalized), NEVER `null`

### 3. Method Invocation Rules For .myco Files
- **ALWAYS** use `var.method()` syntax for built-in types
- **NEVER** use `library.method(var, ...)` or `built_in_method(var, ...)`
- **Examples**:
  - ✅ `arr.push(val)`, `arr.pop()`, `arr.join(sep)`
  - ❌ `array.push(arr, val)`, `push(arr, val)`

### 4. Built-in Function Rules For .myco Files
- **NO** built-in functions `len(val)` and `toString(val)`
- **ALWAYS** use `val.length` and `val.toString()` instead
- **Treat** `len()` and `toString()` calls as user-defined functions

### 5. Type Checking Rules For .myco Files
- **ALWAYS** use instance methods: `x.type` or `x.isInt()`
- **NEVER** use standalone functions: `type(x)` or `isInt(x)`

### 6. Error Message Format Rules For .myco Files
- **Format**: `Error: "<variable>" is Undefined (Line X, Column Y)`
- **Include**: Error codes, line and column information
- **Display**: In red in terminal when available
- **Timing**: Print live rather than at end
- **Style**: Concise and easy to read, avoid messy formatting

### 7. Test Suite Rules For .myco Files
- **NEVER** modify `pass.myco` - it must always pass with 100% success
- **ALWAYS** use `pass.myco` as reference for comprehensive testing
- **EVERY** test must include an `if` statement to check the test condition
- **EACH** test counts as its own entry and must be counted separately

## CODEBASE STRUCTURE RULES

### 8. File Organization Rules
- **Core components**: `src/core/` and `include/core/`
- **Libraries**: `src/libs/` and `include/libs/`
- **Runtime**: `src/runtime/` and `include/runtime/`
- **CLI**: `src/cli/` and `include/cli/`
- **Compilation**: `src/compilation/` and `include/compilation/`
- **Optimization**: `src/optimization/` and `include/optimization/`

### 9. Header File Rules
- **ALWAYS** include proper header guards: `#ifndef NAME_H` / `#define NAME_H` / `#endif`
- **ALWAYS** include necessary dependencies in headers
- **ALWAYS** use forward declarations when possible to reduce compilation time
- **ALWAYS** document all public functions with comprehensive comments

### 10. Source File Rules
- **ALWAYS** include the corresponding header file first
- **ALWAYS** include standard library headers after project headers
- **ALWAYS** use descriptive variable and function names
- **ALWAYS** add comprehensive comments explaining complex logic

## IMPLEMENTATION PATTERNS

### 11. Function Implementation Rules
- **ALWAYS** validate input parameters at function start
- **ALWAYS** handle memory allocation failures gracefully
- **ALWAYS** return appropriate error codes or NULL on failure
- **ALWAYS** free resources before returning from functions
- **ALWAYS** use consistent error handling patterns

### 12. Value System Rules
- **ALWAYS** use the `Value` union type for runtime values
- **ALWAYS** check value types before accessing union members
- **ALWAYS** clone values when storing them in data structures
- **ALWAYS** free values when removing them from data structures

### 13. AST Node Rules
- **ALWAYS** use the provided AST creation functions
- **ALWAYS** set line and column information for error reporting
- **ALWAYS** free AST nodes using `ast_free()` or `ast_free_tree()`
- **ALWAYS** validate AST nodes before processing

### 14. Environment Rules
- **ALWAYS** create new environments for function scopes
- **ALWAYS** maintain proper parent-child relationships
- **ALWAYS** free environments when exiting scopes
- **ALWAYS** check for variable existence before accessing

## LIBRARY IMPLEMENTATION RULES

### 15. Built-in Library Rules
- **ALWAYS** implement each library in its own file
- **ALWAYS** include comprehensive tests in `pass.myco`
- **ALWAYS** use consistent function signatures: `(Interpreter*, Value*, size_t, int, int)`
- **ALWAYS** validate argument counts and types
- **ALWAYS** return appropriate error values on failure

### 16. String Library Rules
- **ALWAYS** handle UTF-8 encoding properly
- **ALWAYS** allocate sufficient memory for string operations
- **ALWAYS** null-terminate strings
- **ALWAYS** free temporary strings after use

### 17. Array Library Rules
- **ALWAYS** implement dynamic resizing
- **ALWAYS** handle bounds checking
- **ALWAYS** clone elements when adding to arrays
- **ALWAYS** free elements when removing from arrays

### 18. Math Library Rules
- **ALWAYS** handle division by zero
- **ALWAYS** handle overflow/underflow conditions
- **ALWAYS** use appropriate precision for floating-point operations

## COMPILATION AND BUILD RULES

### 19. Makefile Rules
- **ALWAYS** maintain cross-platform compatibility (Unix, Mac, Windows, Linux)
- **ALWAYS** use appropriate compiler flags for each platform
- **ALWAYS** include proper include paths
- **ALWAYS** link required libraries

### 20. Compiler Flags Rules
- **ALWAYS** use `-std=c99` for C standard compliance
- **ALWAYS** use `-Wall -Wextra -pedantic` for warnings
- **ALWAYS** use `-O2` for release builds
- **ALWAYS** use `-g -O0` for debug builds

## ERROR HANDLING RULES

### 21. Error Reporting Rules
- **ALWAYS** provide clear, actionable error messages
- **ALWAYS** include line and column information
- **ALWAYS** suggest common fixes when possible
- **ALWAYS** use consistent error message formatting

### 22. Exception Handling Rules
- **ALWAYS** implement proper try-catch-finally support
- **ALWAYS** maintain call stack for stack traces
- **ALWAYS** provide meaningful exception information
- **ALWAYS** clean up resources in finally blocks

## PERFORMANCE RULES

### 23. Memory Performance Rules
- **ALWAYS** use memory pools for frequent allocations
- **ALWAYS** implement proper garbage collection
- **ALWAYS** avoid memory leaks
- **ALWAYS** optimize for memory usage

### 24. Execution Performance Rules
- **ALWAYS** implement JIT compilation where beneficial
- **ALWAYS** optimize hot code paths
- **ALWAYS** use appropriate data structures
- **ALWAYS** minimize function call overhead

## DOCUMENTATION RULES

### 25. Code Documentation Rules
- **ALWAYS** use descriptive comments everywhere possible
- **ALWAYS** make comments simple, detailed, and professional
- **ALWAYS** avoid AI-like or overly verbose comments
- **ALWAYS** document complex algorithms and data structures

### 26. API Documentation Rules
- **ALWAYS** document all public functions
- **ALWAYS** include parameter descriptions
- **ALWAYS** include return value descriptions
- **ALWAYS** include usage examples

## USER PREFERENCE RULES

### 27. User Interface Rules
- **NEVER** use emojis in code or output
- **ALWAYS** use backticks for string inputs in CLI
- **ALWAYS** display "Processing Myco source file: <filename>" only in debug mode
- **ALWAYS** list what is still needed at end of responses

### 28. Feature Priority Rules
- **ALWAYS** list features first, package manager last
- **ALWAYS** commit when everything is 100% working
- **ALWAYS** maintain modular, organized codebase structure

### 29. Syntax Preference Rules
- **NEVER** use angle-bracket generic syntax like `<T>`
- **ALWAYS** use `use file as file` with simple `read()` and `write()`
- **ALWAYS** import most I/O from libraries

## TESTING RULES

### 30. Test Implementation Rules
- **ALWAYS** add comprehensive tests for new features
- **ALWAYS** ensure all tests pass before committing
- **ALWAYS** test edge cases and error conditions
- **ALWAYS** maintain test coverage for critical paths

### 31. Test Organization Rules
- **ALWAYS** organize tests by feature area
- **ALWAYS** use descriptive test names
- **ALWAYS** include setup and teardown when needed
- **ALWAYS** verify test results programmatically

## VERSION CONTROL RULES

### 32. Git Rules
- **ALWAYS** commit when everything is 100% working
- **ALWAYS** write clear, descriptive commit messages
- **ALWAYS** test thoroughly before committing
- **ALWAYS** maintain a clean, working codebase

## FINAL REMINDERS

- **NEVER** break existing functionality when adding new features
- **ALWAYS** maintain backward compatibility
- **ALWAYS** follow the established patterns in the codebase
- **ALWAYS** test thoroughly before submitting changes
- **ALWAYS** ask for clarification when requirements are unclear
- **ALWAYS** prioritize code quality and maintainability