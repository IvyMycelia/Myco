#ifndef MYCO_COMPILATION_H
#define MYCO_COMPILATION_H

/**
 * @file compilation.h
 * @brief Compilation Module - converts Myco code to other formats and optimizes code
 * 
 * The compilation module is responsible for transforming Myco source code into
 * other formats that can be executed by different systems. This includes:
 * 
 * - **C Code Generation**: Converting Myco to C for compilation with standard C compilers
 * - **Native Binary Generation**: Direct compilation to machine code for x86_64, ARM64, etc.
 * - **WebAssembly**: Compiling to WASM for web and embedded applications
 * - **Bytecode**: Generating intermediate bytecode for the Myco virtual machine
 * - **Optimization**: Applying various optimization passes to improve performance
 * 
 * The compilation process follows these stages:
 * 1. **Frontend**: Lexical analysis, parsing, and AST construction (handled by core module)
 * 2. **Optimization**: AST transformations to improve performance and reduce code size
 * 3. **Code Generation**: Converting the optimized AST to the target format
 * 4. **Linking**: Combining generated code with runtime libraries and dependencies
 * 
 * When adding new compilation targets or optimizations:
 * 1. Create the appropriate header and implementation files in the relevant subdirectory
 * 2. Update this compilation.h file to include your new components
 * 3. Integrate with the main compiler in compiler.c
 * 4. Add any necessary configuration options to the compiler interface
 */

// Compilation components
#include "compiler.h"

// TODO: Add codegen headers when implemented
// #include "codegen/c_codegen.h"      // C code generation backend
// #include "codegen/x86_64_codegen.h" // x86_64 native code generation
// #include "codegen/wasm_codegen.h"   // WebAssembly code generation
// #include "codegen/bytecode_codegen.h" // Myco bytecode generation

// TODO: Add optimization headers when implemented
// #include "optimization/constant_folding.h"           // Constant expression evaluation
// #include "optimization/dead_code_elimination.h"      // Remove unreachable code
// #include "optimization/inlining.h"                   // Function inlining optimization

#endif // MYCO_COMPILATION_H
