# Myco Architecture Guide

This document provides an in-depth look at Myco's internal architecture, helping developers understand how the language is structured and how to contribute effectively.

## Table of Contents

1. [Overview](#overview)
2. [Core Components](#core-components)
3. [Compilation Pipeline](#compilation-pipeline)
4. [Runtime System](#runtime-system)
5. [Memory Management](#memory-management)
6. [Optimization System](#optimization-system)
7. [Standard Library](#standard-library)
8. [Build System](#build-system)
9. [Testing Framework](#testing-framework)
10. [Development Workflow](#development-workflow)

## Overview

Myco is built with a modular, layered architecture that separates concerns and enables independent development of different components.

```
┌─────────────────────────────────────────────────────────────┐
│                    Myco Language                            │
├─────────────────────────────────────────────────────────────┤
│  Standard Library  │  Built-in Functions  │  Core Types     │
├─────────────────────────────────────────────────────────────┤
│  Bytecode VM      │  JIT Compiler        │  Memory Manager  │
├─────────────────────────────────────────────────────────────┤
│  Type Checker     │  Parser              │  Lexer           │
├─────────────────────────────────────────────────────────────┤
│  Error System     │  Debug System        │  Profiler        │
├─────────────────────────────────────────────────────────────┤
│  File I/O         │  Network I/O         │  Platform APIs   │
└─────────────────────────────────────────────────────────────┘
```

## Core Components

### 1. Lexer (`src/core/lexer.c`)

**Purpose**: Converts source code into tokens

**Key Functions**:
- `lexer_init()` - Initialize lexer state
- `lexer_next_token()` - Get next token from input
- `lexer_peek_token()` - Look ahead at next token
- `lexer_cleanup()` - Free lexer resources

**Token Types**:
```c
typedef enum {
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_KEYWORD,
    TOKEN_OPERATOR,
    TOKEN_PUNCTUATION,
    TOKEN_EOF
} TokenType;
```

### 2. Parser (`src/core/parser.c`)

**Purpose**: Converts tokens into Abstract Syntax Tree (AST)

**Key Functions**:
- `parse_program()` - Parse entire program
- `parse_statement()` - Parse individual statements
- `parse_expression()` - Parse expressions
- `parse_function()` - Parse function definitions
- `parse_class()` - Parse class definitions

**AST Node Types**:
```c
typedef enum {
    NODE_PROGRAM,
    NODE_FUNCTION,
    NODE_CLASS,
    NODE_VARIABLE,
    NODE_ASSIGNMENT,
    NODE_IF,
    NODE_WHILE,
    NODE_FOR,
    NODE_CALL,
    NODE_BINARY_OP,
    NODE_UNARY_OP,
    NODE_LITERAL
} NodeType;
```

### 3. Type Checker (`src/core/type_checker.c`)

**Purpose**: Performs static type checking and inference

**Key Functions**:
- `type_check_program()` - Type check entire program
- `type_check_expression()` - Type check expressions
- `type_infer()` - Infer types from context
- `type_unify()` - Unify types in expressions

**Type System**:
```c
typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_BOOLEAN,
    TYPE_NULL,
    TYPE_ARRAY,
    TYPE_MAP,
    TYPE_SET,
    TYPE_UNION,
    TYPE_OPTIONAL,
    TYPE_FUNCTION
} TypeKind;
```

### 4. Bytecode Compiler (`src/compilation/bytecode_compiler.c`)

**Purpose**: Compiles AST to bytecode instructions

**Key Functions**:
- `bytecode_compile_program()` - Compile entire program
- `bytecode_compile_node()` - Compile AST nodes
- `bytecode_compile_expression()` - Compile expressions
- `bytecode_compile_function()` - Compile functions

**Bytecode Instructions**:
```c
typedef enum {
    BC_LOAD_CONST,      // Load constant
    BC_LOAD_VAR,        // Load variable
    BC_STORE_VAR,       // Store variable
    BC_ADD,             // Addition
    BC_SUB,             // Subtraction
    BC_MUL,             // Multiplication
    BC_DIV,             // Division
    BC_CALL,            // Function call
    BC_RETURN,          // Return from function
    BC_JUMP,            // Unconditional jump
    BC_JUMP_IF_FALSE,   // Conditional jump
    BC_POP,             // Pop from stack
    BC_NOP              // No operation
} BytecodeInstruction;
```

### 5. Virtual Machine (`src/core/bytecode_vm.c`)

**Purpose**: Executes bytecode instructions

**Key Functions**:
- `bytecode_execute()` - Execute bytecode program
- `vm_push()` - Push value to stack
- `vm_pop()` - Pop value from stack
- `vm_call_function()` - Call function
- `vm_handle_instruction()` - Handle individual instructions

**VM State**:
```c
typedef struct {
    BytecodeProgram* program;
    Value* stack;
    int stack_size;
    int stack_capacity;
    Value* locals;
    int local_count;
    Environment* environment;
    int pc;  // Program counter
} VMState;
```

## Compilation Pipeline

### 1. Source Code → Tokens

```
Source: "let x = 42;"
Tokens: [KEYWORD(let), IDENTIFIER(x), OPERATOR(=), NUMBER(42), PUNCTUATION(;)]
```

### 2. Tokens → AST

```
AST: AssignmentNode {
    variable: "x",
    value: LiteralNode { type: INT, value: 42 }
}
```

### 3. AST → Type-Checked AST

```
Type-checked AST: AssignmentNode {
    variable: "x" (type: Int),
    value: LiteralNode { type: Int, value: 42 }
}
```

### 4. AST → Bytecode

```
Bytecode: [
    BC_LOAD_CONST(0),  // Load constant 42
    BC_STORE_VAR(0)    // Store to variable x
]
```

### 5. Bytecode → Execution

```
VM executes bytecode instructions and produces result
```

## Runtime System

### Value System

Myco uses a unified value system where all data is represented as `Value` objects:

```c
typedef struct {
    ValueType type;
    union {
        int int_value;
        double float_value;
        char* string_value;
        bool boolean_value;
        Array* array_value;
        Map* map_value;
        Set* set_value;
        Function* function_value;
    } data;
} Value;
```

### Memory Management

**Garbage Collection**: Automatic memory management using mark-and-sweep algorithm

**Memory Pools**: Efficient allocation using pre-allocated memory pools

**Value Pooling**: Reuse of common values to reduce allocation overhead

### Error Handling

**Error Types**:
- Syntax errors (lexer/parser)
- Type errors (type checker)
- Runtime errors (VM)
- Memory errors (memory manager)

**Error Propagation**: Errors bubble up through the call stack with context

## Optimization System

### 1. Compiler Optimizations

**Constant Folding**: Evaluate constant expressions at compile time
```c
// Source: 2 + 3 * 4
// Optimized: 14
```

**Dead Code Elimination**: Remove unreachable code
```c
// Source: if (false) { print("unreachable"); }
// Optimized: (removed)
```

**Loop Unrolling**: Unroll small loops for better performance
```c
// Source: for i in range(1, 4): print(i); end
// Optimized: print(1); print(2); print(3);
```

### 2. Runtime Optimizations

**JIT Compilation**: Compile hot code paths to native code

**Branch Prediction**: Use CPU branch prediction hints

**Cache Optimization**: Optimize memory access patterns

**Value Pooling**: Reuse common values

### 3. Memory Optimizations

**String Interning**: Share identical strings in memory

**Array Pooling**: Reuse array objects

**Garbage Collection Tuning**: Optimize GC for different workloads

## Standard Library

### Built-in Types

**Primitive Types**: Int, Float, String, Boolean, Null

**Collection Types**: Array, Map, Set, Tree, Graph, Heap, Queue, Stack

**Optional Types**: String?, Int?, etc.

**Union Types**: String | Int, etc.

### Built-in Functions

**Math Functions**: sqrt, pow, sin, cos, etc.

**String Functions**: upper, lower, trim, split, etc.

**I/O Functions**: print, input, file operations

**Collection Functions**: map, filter, reduce, etc.

### Library Organization

```
src/libs/
├── math.c          # Mathematical functions
├── string.c        # String operations
├── array.c         # Array operations
├── map.c           # Map operations
├── set.c           # Set operations
├── tree.c          # Tree operations
├── graph.c         # Graph operations
├── heap.c          # Heap operations
├── queue.c         # Queue operations
├── stack.c         # Stack operations
├── file.c          # File I/O
├── http.c          # HTTP client
├── json.c          # JSON processing
└── regex.c         # Regular expressions
```

## Build System

### Makefile Structure

```makefile
# Main targets
all: myco
myco: $(OBJECTS)
	$(CC) $(LDFLAGS) -o bin/myco $(OBJECTS)

# Object files
build/core/lexer.o: src/core/lexer.c
	$(CC) $(CFLAGS) -c $< -o $@

# Dependencies
-include $(OBJECTS:.o=.d)
```

### Compilation Flags

```makefile
CFLAGS = -Wall -Wextra -O2 -g -std=c99
LDFLAGS = -lm -lpthread
DEBUG_FLAGS = -DDEBUG -g3 -O0
RELEASE_FLAGS = -DNDEBUG -O3 -flto
```

### Platform Support

**macOS**: Clang with Xcode tools
**Linux**: GCC with standard libraries
**Windows**: MinGW-w64 or Visual Studio

## Testing Framework

### Test Structure

**Unit Tests**: Individual function testing
**Integration Tests**: Component interaction testing
**End-to-End Tests**: Full program execution testing
**Performance Tests**: Benchmarking and profiling

### Test Execution

```bash
# Run all tests
myco pass.myco

# Run specific test categories
myco tests/arithmetic.myco
myco tests/strings.myco

# Run with coverage
myco --coverage pass.myco
```

### Test Organization

```
tests/
├── arithmetic.myco    # Math operations
├── strings.myco       # String operations
├── collections.myco   # Collection operations
├── classes.myco       # Object-oriented features
├── functions.myco     # Function features
├── control_flow.myco  # Control structures
└── error_handling.myco # Error handling
```

## Development Workflow

### 1. Code Changes

1. **Create feature branch**
2. **Make changes** with tests
3. **Run test suite**
4. **Update documentation**
5. **Submit pull request**

### 2. Testing

1. **Unit tests** for new functions
2. **Integration tests** for new features
3. **Regression tests** to ensure no breakage
4. **Performance tests** for optimizations

### 3. Documentation

1. **Code comments** for complex logic
2. **API documentation** for new functions
3. **User documentation** for new features
4. **Architecture documentation** for major changes

### 4. Review Process

1. **Automated checks** (build, test, lint)
2. **Code review** by maintainers
3. **Performance review** for optimizations
4. **Documentation review** for user-facing changes

## Performance Considerations

### 1. Compilation Performance

**Lexer**: O(n) where n is source length
**Parser**: O(n) for most constructs
**Type Checker**: O(n) for most programs
**Bytecode Compiler**: O(n) for most programs

### 2. Runtime Performance

**VM Execution**: Optimized instruction dispatch
**Memory Allocation**: Pooled allocation for common types
**Garbage Collection**: Incremental collection to minimize pauses
**JIT Compilation**: Hot code path optimization

### 3. Memory Usage

**Value Objects**: 16 bytes per value (on 64-bit)
**String Interning**: Shared storage for identical strings
**Array Pooling**: Reuse of array objects
**Garbage Collection**: Automatic cleanup of unused objects

## Debugging and Profiling

### Debug Tools

**GDB Integration**: Debug with GDB
**Valgrind Support**: Memory leak detection
**Address Sanitizer**: Memory error detection
**Thread Sanitizer**: Race condition detection

### Profiling Tools

**Built-in Profiler**: Function call timing
**Memory Profiler**: Memory usage analysis
**CPU Profiler**: Hot spot identification
**Cache Profiler**: Cache performance analysis

### Debug Output

```bash
# Debug mode
myco --debug program.myco

# Verbose output
myco --verbose program.myco

# Profile mode
myco --profile program.myco
```

---

This architecture guide provides a comprehensive overview of Myco's internal structure. For more specific implementation details, see the source code and individual component documentation.
