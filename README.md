# Myco

A dynamically-typed interpreted programming language implemented in C with a multi-tier optimization system.

## Language Overview

Myco is a statically-scoped, dynamically-typed language with:
- C-style syntax with `end` block termination
- First-class functions and closures
- Built-in data types: numbers, strings, booleans, arrays, maps, objects
- Standard library modules: file I/O, HTTP, JSON, regex, time, math
- Multi-tier execution: AST interpreter → bytecode → JIT compilation → specialized code

## Core Features

- **AST-based Parser**: Recursive descent parser with comprehensive error reporting
- **Multi-tier VM**: 6-tier execution system with adaptive optimization
- **Register-based Bytecode**: 128-instruction set with specialized operations
- **Trace-based JIT**: Hot path recording and native code generation
- **Type Specialization**: Predictive optimization based on observed types
- **Memory Management**: Reference counting with cycle detection
- **Cross-platform**: x86_64, ARM64 support with platform-specific optimizations

## Architecture

### Parser
- Recursive descent parser with lookahead
- Comprehensive error reporting with line/column information
- Support for expressions, statements, functions, classes, modules

### Interpreter
- AST-based execution with value stack
- Environment-based variable scoping
- Built-in function dispatch system
- Library module loading and method resolution

### Optimization System
- **Tier 0**: AST Interpreter (baseline)
- **Tier 1**: Register Bytecode (4-6x faster than AST)
- **Tier 2**: Trace Recording (hot path detection)
- **Tier 3**: Trace Compilation (15-25x faster than AST)
- **Tier 4**: Predictive Specialization (20-30x faster than AST)
- **Tier 5**: Vectorized Code (30-50x faster for numeric code)

## Quick Start

```bash
# Build Myco
make

# Run a script
./bin/myco script.myco

# Interactive REPL
./bin/myco
```

## Syntax Examples

```myco
# Variables and types
x = 42;
name = "Myco";
flag = true;
arr = [1, 2, 3];
obj = {key: "value"};

# Functions
func factorial(n):
    if n <= 1:
        return 1;
    else:
        return n * factorial(n - 1);
    end
end

# Classes
class Point:
    func init(x, y):
        self.x = x;
        self.y = y;
    end
    
    func distance(other):
        dx = self.x - other.x;
        dy = self.y - other.y;
        return math.sqrt(dx * dx + dy * dy);
    end
end

# Module usage
use http as http;
response = http.get("https://api.example.com/data");
print("Status:", response.status);
```

## Data Types

- **Numbers**: Double-precision floating point (64-bit)
- **Strings**: UTF-8 encoded with reference counting
- **Booleans**: `true` and `false`
- **Arrays**: Dynamic arrays with mixed-type elements
- **Maps**: Hash tables with string keys
- **Objects**: User-defined classes with methods
- **Functions**: First-class functions and closures
- **Null**: Single null value

## Standard Library

- **file**: File I/O operations (read, write, exists, etc.)
- **dir**: Directory operations (list, create, remove, etc.)
- **http**: HTTP client (get, post, put, delete)
- **json**: JSON parsing and serialization
- **regex**: Regular expression matching
- **time**: Time and date operations
- **math**: Mathematical functions and constants
- **string**: String manipulation functions
- **array**: Array operations (push, pop, sort, etc.)

## Performance

Current performance benchmarks show:
- **AST Interpreter**: Baseline performance
- **JIT Optimized**: 29% improvement over AST
- **Target**: 1.5-2x slower than C (beating LuaJIT's 1.5-3x)

## Building from Source

```bash
git clone <repository-url>
cd myco
make
```

## Documentation

- `MYCO_SYNTAX_REFERENCE.md` - Complete language syntax reference
- `MYCO_CODEBASE_RULES.md` - Development guidelines for contributors
- `CHANGELOG.md` - Version history and changes
