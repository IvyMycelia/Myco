#ifndef MYCO_UTILS_H
#define MYCO_UTILS_H

/**
 * @file utils.h
 * @brief Utility Functions Module - common functionality used across the project
 * 
 * The utils module provides shared utility functions that are used by multiple
 * other modules in the Myco project. These utilities handle common tasks that
 * don't belong to any specific language feature but are needed throughout
 * the codebase.
 * 
 * Key utility categories:
 * 
 * - **Error Handling**: Centralized error reporting, error types, and error recovery
 * - **Logging**: Debug output, diagnostic information, and performance profiling
 * - **File Utilities**: File I/O operations, path manipulation, and file validation
 * - **String Utilities**: String manipulation, formatting, and text processing
 * - **Data Structures**: Common data structures like hash tables, linked lists, etc.
 * - **Math Utilities**: Mathematical functions, random number generation, etc.
 * - **Platform Abstraction**: Cross-platform compatibility and system calls
 * 
 * When adding new utility functions:
 * 1. **Identify the category**: Place functions in the appropriate utility file
 * 2. **Keep it generic**: Utilities should be reusable across different modules
 * 3. **Document thoroughly**: Include usage examples and edge case handling
 * 4. **Test thoroughly**: Utilities are used everywhere, so bugs are costly
 * 5. **Consider performance**: Utilities are often called frequently
 * 
 * The utils module follows these design principles:
 * - **Single Responsibility**: Each function does one thing well
 * - **Consistent Interface**: Similar functions follow similar patterns
 * - **Error Handling**: Proper error reporting and recovery
 * - **Memory Safety**: No memory leaks or undefined behavior
 * - **Thread Safety**: Functions can be called from multiple threads
 * 
 * Utilities in this module are designed to be lightweight and efficient,
 * as they're used throughout the codebase and can impact overall performance.
 */

// Utility components
// TODO: Add utility headers when implemented
// #include "error_handling.h"  // Error types, reporting, and recovery mechanisms
// #include "logging.h"         // Debug output, logging levels, and diagnostic tools
// #include "file_utils.h"      // File operations, path handling, and I/O utilities

#endif // MYCO_UTILS_H
