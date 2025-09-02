# Myco v2.0 - Modular Structure Guide

**Version**: v2.0  
**Last Updated**: August 2025  
**Status**: Complete and Current

---

## **Overview**

Myco v2.0 has been reorganized into a highly modular and maintainable structure. This organization makes it easy to:

- **Find specific functionality** quickly
- **Add new features** without affecting existing code
- **Understand the codebase** architecture
- **Work on components** in isolation
- **Scale the project** as it grows

---

## **Directory Structure**

```directory
myco-v2/
├── src/                          # Source code
│   ├── core/                     # Core language features
│   │   ├── lexer.c              # Lexical analysis
│   │   ├── parser.c             # Syntax analysis
│   │   ├── ast.c                # Abstract Syntax Tree
│   │   └── interpreter.c        # Language interpreter
│   ├── compilation/              # Compilation system
│   │   ├── compiler.c           # Main compiler
│   │   ├── codegen/             # Code generation (future)
│   │   └── optimization/        # Optimizations (future)
│   ├── runtime/                  # Runtime system
│   │   └── memory.c             # Memory management
│   ├── stdlib/                   # Standard library (future)
│   ├── cli/                      # Command line interface
│   │   └── main.c               # Main entry point
│   └── utils/                    # Utility functions (future)
├── include/                       # Header files
│   ├── core/                     # Core headers
│   │   ├── lexer.h
│   │   ├── parser.h
│   │   ├── ast.h
│   │   └── interpreter.h
│   ├── compilation/              # Compilation headers
│   │   ├── compiler.h
│   │   ├── codegen/             # Code generation headers (future)
│   │   └── optimization/        # Optimization headers (future)
│   ├── runtime/                  # Runtime headers
│   │   └── memory.h
│   ├── stdlib/                   # Standard library headers (future)
│   ├── cli/                      # CLI headers (future)
│   ├── utils/                    # Utility headers (future)
│   └── myco.h                   # Main header (includes all)
├── tests/                        # Test files
├── examples/                      # Example programs
├── docs/                         # Documentation
├── tools/                        # Build and development tools
├── bin/                          # Compiled executables
└── build/                        # Build artifacts
```

---

## **Module Responsibilities**

### **Core Module** (`src/core/`, `include/core/`)

**Purpose**: Fundamental language features that form the foundation of Myco.

**Components**:

- **Lexer**: Tokenizes source code into meaningful units
- **Parser**: Converts tokens into an Abstract Syntax Tree
- **AST**: Represents the program structure
- **Interpreter**: Executes the AST directly

**When to modify**: Adding new language syntax, changing parsing rules, modifying AST structure.

**Example additions**:

- New token types in `lexer.c`
- New AST node types in `ast.c`
- New parsing rules in `parser.c`
- New execution logic in `interpreter.c`

---

### **Compilation Module** (`src/compilation/`, `include/compilation/`)

**Purpose**: Converting Myco code to other formats and optimizing code.

**Components**:

- **Compiler**: Main compilation orchestration
- **Code Generation**: Target-specific code generation (C, x86_64, WASM, etc.)
- **Optimization**: Code optimization passes

**When to modify**: Adding new compilation targets, implementing optimizations, changing code generation.

**Example additions**:

- New target architecture in `codegen/`
- New optimization pass in `optimization/`
- Enhanced compiler configuration

---

### **Runtime Module** (`src/runtime/`, `include/runtime/`)

**Purpose**: Runtime system that supports program execution.

**Components**:

- **Memory Management**: Memory allocation, garbage collection
- **Type System**: Runtime type checking and management
- **Built-in Functions**: Core language functions

**When to modify**: Changing memory management strategy, adding new built-in functions, modifying type system.

**Example additions**:

- New memory allocation strategy
- New built-in function in `builtin_functions.c`
- Enhanced type checking in `type_system.c`

---

### **Standard Library** (`src/stdlib/`, `include/stdlib/`)

**Purpose**: Pre-built functionality that comes with Myco.

**Components**:

- **I/O Functions**: File and console operations
- **Math Functions**: Mathematical operations
- **Collections**: Data structure implementations
- **String Functions**: String manipulation utilities

**When to modify**: Adding new standard library functions, enhancing existing functionality.

**Example additions**:

- New file I/O function in `io.c`
- New mathematical function in `math.c`
- New data structure in `collections.c`

---

### **CLI Module** (`src/cli/`, `include/cli/`)

**Purpose**: Command-line interface and user interaction.

**Components**:

- **Main Entry Point**: Program startup and argument parsing
- **Argument Parser**: Command-line argument handling
- **REPL**: Interactive read-eval-print loop

**When to modify**: Adding new CLI options, changing user interface, enhancing REPL functionality.

**Example additions**:

- New command-line flag in `argument_parser.c`
- Enhanced REPL features in `repl.c`
- New CLI command in `main.c`

---

### **Utils Module** (`src/utils/`, `include/utils/`)

**Purpose**: Common utility functions used across the project.

**Components**:

- **Error Handling**: Error reporting and management
- **Logging**: Debug and diagnostic output
- **File Utilities**: File system operations

**When to modify**: Adding new utility functions, changing error handling, enhancing logging.

**Example additions**:

- New error type in `error_handling.c`
- Enhanced logging in `logging.c`
- New file operation in `file_utils.c`

---

## **How to Add New Features**

### **1. Identify the Right Module**

Ask yourself: "What is the primary purpose of this feature?"

- **Language feature** → `core/`
- **Compilation target** → `compilation/`
- **Runtime functionality** → `runtime/`
- **Standard library** → `stdlib/`
- **User interface** → `cli/`
- **Utility function** → `utils/`

### **2. Create or Modify Files**

- **Header file**: Define the interface in `include/[module]/`
- **Implementation**: Implement the functionality in `src/[module]/`
- **Integration**: Update the module header (e.g., `core.h`) to include your new header

### **3. Update Build System**

The Makefile automatically finds all `.c` files in subdirectories, so no changes needed unless you add new dependencies.

### **4. Test Your Changes**

- Build the project: `make clean && make`
- Run tests: `make test`
- Test functionality: `./bin/myco [your-feature]`

---

## **Common Development Tasks**

### **Adding a New Language Feature**

1. **Lexer**: Add token type in `include/core/lexer.h`
2. **Parser**: Add parsing rule in `src/core/parser.c`
3. **AST**: Add node type in `include/core/ast.h`
4. **Interpreter**: Add execution logic in `src/core/interpreter.c`

### **Adding a New Compilation Target**

1. **Header**: Create `include/compilation/codegen/[target].h`
2. **Implementation**: Create `src/compilation/codegen/[target].c`
3. **Integration**: Update `include/compilation/compilation.h`
4. **Compiler**: Add target support in `src/compilation/compiler.c`

### **Adding a New Built-in Function**

1. **Header**: Add function declaration in `include/runtime/builtin_functions.h`
2. **Implementation**: Add function in `src/runtime/builtin_functions.c`
3. **Registration**: Register in `interpreter_register_builtins()`

### **Adding a New CLI Option**

1. **Header**: Add option definition in `include/cli/argument_parser.h`
2. **Implementation**: Add parsing logic in `src/cli/argument_parser.c`
3. **Integration**: Update `src/cli/main.c` to handle the option

---

## **Build System**

### **Automatic Discovery**

The Makefile automatically:

- Finds all `.c` files in subdirectories
- Creates appropriate build directories
- Links all object files together

### **Include Paths**

All subdirectories are automatically included:

```makefile
-Iinclude -Iinclude/core -Iinclude/compilation -Iinclude/runtime -Iinclude/cli -Iinclude/utils
```

### **No Manual Configuration Needed**

Just add your files to the appropriate directories and rebuild!

---

## **Best Practices**

### **1. Keep Modules Focused**

Each module should have a single, clear responsibility. Don't mix concerns.

### **2. Use Clear Naming**

- Files: `descriptive_name.c` (e.g., `string_utils.c`)
- Functions: `module_action_object()` (e.g., `lexer_scan_identifier()`)

### **3. Minimize Dependencies**

- Core modules should not depend on higher-level modules
- Use forward declarations when possible
- Keep includes minimal and focused

### **4. Document Your Changes**

- Update this guide when adding new modules
- Add comments explaining complex logic
- Update relevant documentation

### **5. Test Incrementally**

- Test your changes as you build them
- Ensure the project still builds after each change
- Run existing functionality to verify nothing broke

---

## **Benefits of This Structure**

1. **Easy Navigation**: You'll always know where to find specific functionality
2. **Clean Separation**: Changes in one module don't affect others
3. **Scalable**: Easy to add new features without cluttering existing code
4. **Maintainable**: Clear organization makes debugging and enhancement easier
5. **Team-Friendly**: Multiple developers can work on different modules simultaneously
6. **Learning-Friendly**: Clear structure helps you understand the codebase

---

## **Next Steps**

Now that you have this organized structure:

1. **Explore the modules** to understand what's already implemented
2. **Identify areas** where you want to add functionality
3. **Follow the patterns** established in existing code
4. **Add new features** in the appropriate modules
5. **Keep this guide updated** as you expand the codebase

The modular structure will make your development experience much more enjoyable and productive!
