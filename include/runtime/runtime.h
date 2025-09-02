#ifndef MYCO_RUNTIME_H
#define MYCO_RUNTIME_H

/**
 * @file runtime.h
 * @brief Runtime System Module - supports program execution and memory management
 * 
 * The runtime module provides the infrastructure needed to execute Myco programs
 * after they've been parsed and optionally compiled. This module handles:
 * 
 * - **Memory Management**: Dynamic allocation, garbage collection, and memory safety
 * - **Type System**: Runtime type checking, type information, and dynamic dispatch
 * - **Built-in Functions**: Core language functions like print, input, len, etc.
 * - **Error Handling**: Runtime error detection, reporting, and recovery
 * - **Interop**: Integration with system libraries and foreign function interfaces
 * 
 * The runtime system is designed to be:
 * - **Safe**: Prevents memory leaks, buffer overflows, and type violations
 * - **Fast**: Optimized for performance with minimal overhead
 * - **Portable**: Works across different platforms and architectures
 * - **Extensible**: Easy to add new built-in functions and runtime features
 * 
 * When adding new runtime functionality:
 * 1. **Built-in Functions**: Add to the appropriate stdlib subdirectory
 * 2. **Memory Management**: Extend the memory system in memory.c
 * 3. **Type System**: Enhance type checking and runtime type information
 * 4. **Error Handling**: Add new error types and recovery mechanisms
 * 
 * The runtime works closely with the interpreter (in the core module) to
 * execute programs, but it can also be used by compiled code for runtime
 * services like memory allocation and type checking.
 */

// Runtime components
#include "memory.h"

// TODO: Add runtime headers when implemented
// #include "garbage_collector.h"  // Automatic memory management and cleanup
// #include "type_system.h"        // Runtime type checking and type information
// #include "builtin_functions.h"  // Core language built-in functions

#endif // MYCO_RUNTIME_H
