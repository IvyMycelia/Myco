#ifndef MYCO_H
#define MYCO_H

/**
 * @file myco.h
 * @brief Main header file for Myco v2.0 - High-Performance Programming Language
 * 
 * This is the primary header file that includes all the major components of the
 * Myco programming language. It serves as the main entry point for any code that
 * needs to use Myco functionality.
 * 
 * The header is organized into logical modules:
 * - Core: Fundamental language features (lexer, parser, AST, interpreter)
 * - Compilation: Code generation and optimization
 * - Runtime: Memory management and execution support
 * - CLI: Command-line interface and user interaction
 * - Utils: Common utility functions
 * 
 * This header also defines:
 * - Version information and build configuration
 * - Platform and architecture detection
 * - Feature flags and performance targets
 * - Error codes and memory configuration
 * - Global constants and type definitions
 * 
 * When using Myco in your project, typically you only need to include this
 * single header file. All other necessary headers will be included automatically.
 */

// Core language components - fundamental building blocks of the language
#include "core/core.h"

// Compilation components - code generation and optimization systems
#include "compilation/compilation.h"

// Runtime components - memory management and execution support
#include "runtime/runtime.h"

// CLI components - command-line interface and user interaction
#include "cli/cli.h"

// Utility components - common functions used across the project
#include "utils/utils.h"

// Global execution mode flags
extern int g_force_ast_only;      // When 1, forces AST-only execution (disables bytecode VM)
extern int g_bytecode_enabled;    // When 1, enables bytecode VM execution (optional with --bc/--bytecode flag)

/**
 * @brief Version information for Myco v2.0
 * 
 * These constants define the current version of the Myco language.
 * Version numbers follow semantic versioning (MAJOR.MINOR.PATCH):
 * - MAJOR: Incompatible API changes
 * - MINOR: New functionality in a backward-compatible manner
 * - PATCH: Backward-compatible bug fixes
 */
#define MYCO_VERSION_MAJOR 2      // Major version number
#define MYCO_VERSION_MINOR 0      // Minor version number
#define MYCO_VERSION_PATCH 0      // Patch version number
#define MYCO_VERSION_STRING "2.0.0"  // Complete version string

/**
 * @brief Build configuration flags
 * 
 * These flags control how Myco is compiled and behave.
 * They're automatically set based on compiler flags and can be
 * used throughout the codebase for conditional compilation.
 */
#ifdef DEBUG
#define MYCO_DEBUG 1              // Debug mode enabled (more verbose, less optimized)
#else
#define MYCO_DEBUG 0              // Debug mode disabled (release build)
#endif

#ifdef NDEBUG
#define MYCO_RELEASE 1            // Release mode enabled (optimized, minimal debug info)
#else
#define MYCO_RELEASE 0            // Release mode disabled (debug build)
#endif

/**
 * @brief Platform detection macros
 * 
 * These macros automatically detect the operating system Myco is running on.
 * They're used throughout the codebase for platform-specific optimizations
 * and to ensure compatibility across different systems.
 * 
 * Only one platform macro will be set to 1, all others will be 0.
 */
#ifdef _WIN32
#define MYCO_PLATFORM_WINDOWS 1   // Windows operating system (Windows 10, 11, etc.)
#elif defined(__APPLE__)
#define MYCO_PLATFORM_MACOS 1     // macOS operating system (macOS 10.15+, 11+, 12+, etc.)
#elif defined(__linux__)
#define MYCO_PLATFORM_LINUX 1     // Linux operating system (Ubuntu, Debian, CentOS, etc.)
#else
#define MYCO_PLATFORM_UNKNOWN 1   // Unknown or unsupported platform
#endif

/**
 * @brief CPU architecture detection macros
 * 
 * These macros detect the CPU architecture Myco is running on.
 * They're used for architecture-specific optimizations and to ensure
 * the right machine code is generated for the target platform.
 * 
 * Only one architecture macro will be set to 1, all others will be 0.
 */
#ifdef _M_X64
#define MYCO_ARCH_X86_64 1        // x86_64 architecture (Intel/AMD 64-bit)
#elif defined(__x86_64__)
#define MYCO_ARCH_X86_64 1        // x86_64 architecture (Intel/AMD 64-bit)
#elif defined(__aarch64__)
#define MYCO_ARCH_ARM64 1         // ARM64 architecture (Apple Silicon, ARM servers)
#elif defined(__arm__)
#define MYCO_ARCH_ARM 1            // ARM architecture (32-bit ARM devices)
#else
#define MYCO_ARCH_UNKNOWN 1       // Unknown or unsupported architecture
#endif

/**
 * @brief Compiler detection macros
 * 
 * These macros detect which C compiler is being used to build Myco.
 * They're used for compiler-specific optimizations and to ensure
 * compatibility with different compiler implementations.
 * 
 * Only one compiler macro will be set to 1, all others will be 0.
 */
#ifdef _MSC_VER
#define MYCO_COMPILER_MSVC 1      // Microsoft Visual C++ compiler
#elif defined(__GNUC__)
#define MYCO_COMPILER_GCC 1       // GNU Compiler Collection (GCC)
#elif defined(__clang__)
#define MYCO_COMPILER_CLANG 1     // Clang compiler (LLVM-based)
#else
#define MYCO_COMPILER_UNKNOWN 1   // Unknown or unsupported compiler
#endif

/**
 * @brief Feature availability flags
 * 
 * These flags indicate which language features are currently implemented
 * and available in this build of Myco. They're used for conditional
 * compilation and feature detection throughout the codebase.
 * 
 * All feature flags are set to 1 when the feature is available, 0 otherwise.
 * These can be used to provide graceful degradation when features aren't
 * yet implemented.
 */
#define MYCO_FEATURE_LEXER 1                  // Lexical analysis (tokenization)
#define MYCO_FEATURE_PARSER 1                 // Syntax analysis (parsing)
#define MYCO_FEATURE_AST 1                    // Abstract Syntax Tree support
#define MYCO_FEATURE_INTERPRETER 1            // Direct interpretation of AST
#define MYCO_FEATURE_COMPILER 1               // Code generation and compilation
#define MYCO_FEATURE_MEMORY_MANAGEMENT 1      // Memory allocation and management

/**
 * @brief Performance and quality targets
 * 
 * These constants define the goals and targets for Myco's performance
 * and quality characteristics. They're used for optimization decisions
 * and to measure how well the implementation meets its goals.
 * 
 * Performance targets are relative to C (1.0 = 100% of C performance)
 * Quality targets are absolute (1.0 = 100% achievement of the goal)
 */
#define MYCO_TARGET_PERFORMANCE_C_LEVEL 0.90  // Target: 90% of C performance
#define MYCO_TARGET_MEMORY_SAFETY 1.0         // Target: 100% memory safety
#define MYCO_TARGET_CROSS_PLATFORM 1.0        // Target: 100% platform compatibility

/**
 * @brief Error codes for Myco operations
 * 
 * These constants define the return values for Myco functions.
 * Positive values typically indicate success, while negative values
 * indicate various types of errors that occurred during execution.
 * 
 * Error codes are used throughout the codebase to provide detailed
 * information about what went wrong, allowing for better error
 * handling and debugging.
 */
#define MYCO_SUCCESS 0                        // Operation completed successfully
#define MYCO_ERROR_LEXER -1                   // Error occurred during lexical analysis
#define MYCO_ERROR_PARSER -2                  // Error occurred during parsing
#define MYCO_ERROR_SEMANTIC -3                // Semantic error (type mismatch, etc.)
#define MYCO_ERROR_RUNTIME -4                 // Runtime error during execution
#define MYCO_ERROR_MEMORY -5                  // Memory allocation or management error
#define MYCO_ERROR_COMPILER -6                // Error during compilation
#define MYCO_ERROR_INTERPRETER -7             // Error during interpretation
#define MYCO_ERROR_CLI -8                     // Command line interface error
#define MYCO_ERROR_FILE -9                    // File I/O error

/**
 * @brief Memory management configuration
 * 
 * These constants define the default sizes and limits for Myco's
 * memory management system. They control how much memory is allocated
 * initially and how many memory pools/arenas can be created.
 * 
 * These values can be tuned based on the target system and application
 * requirements. Larger values provide better performance but use more memory.
 */
#define MYCO_DEFAULT_POOL_SIZE (1024 * 1024)  // Default memory pool size: 1MB
#define MYCO_DEFAULT_ARENA_SIZE (1024 * 1024) // Default memory arena size: 1MB
#define MYCO_MAX_POOL_COUNT 100               // Maximum number of memory pools
#define MYCO_MAX_ARENA_COUNT 100              // Maximum number of memory arenas

/**
 * @brief Lexer configuration limits
 * 
 * These constants define the maximum sizes for various lexical elements
 * that the lexer can process. They help prevent memory issues and provide
 * reasonable limits for practical programming needs.
 * 
 * These limits can be adjusted based on system requirements, but be aware
 * that very large values may impact memory usage and performance.
 */
#define MYCO_MAX_TOKEN_LENGTH 1024            // Maximum length of any single token
#define MYCO_MAX_IDENTIFIER_LENGTH 256        // Maximum length of variable/function names
#define MYCO_MAX_STRING_LENGTH 65536          // Maximum length of string literals (64KB)
#define MYCO_MAX_LINE_LENGTH 8192             // Maximum length of a single source line

// Parser configuration
#define MYCO_MAX_EXPRESSION_DEPTH 100
#define MYCO_MAX_STATEMENT_DEPTH 100
#define MYCO_MAX_FUNCTION_PARAMETERS 100
#define MYCO_MAX_CLASS_MEMBERS 1000

// Interpreter configuration
#define MYCO_MAX_CALL_STACK_DEPTH 1000
#define MYCO_MAX_LOOP_ITERATIONS 1000000
#define MYCO_MAX_RECURSION_DEPTH 1000

// Compiler configuration
#define MYCO_MAX_OPTIMIZATION_PASSES 10
#define MYCO_MAX_INLINE_DEPTH 5
#define MYCO_MAX_UNROLL_COUNT 100

// Utility macros
#define MYCO_UNUSED(x) (void)(x)
#define MYCO_ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define MYCO_MIN(a, b) ((a) < (b) ? (a) : (b))
#define MYCO_MAX(a, b) ((a) > (b) ? (a) : (b))
#define MYCO_CLAMP(x, min, max) MYCO_MIN(MYCO_MAX(x, min), max)

// Debug macros
#if MYCO_DEBUG
#define MYCO_ASSERT(condition) \
    do { \
        if (!(condition)) { \
            fprintf(stderr, "Assertion failed: %s, file %s, line %d\n", \
                    #condition, __FILE__, __LINE__); \
            abort(); \
        } \
    } while(0)
#define MYCO_DEBUG_PRINT(fmt, ...) \
    fprintf(stderr, "[DEBUG] " fmt "\n", ##__VA_ARGS__)
#else
#define MYCO_ASSERT(condition) ((void)0)
#define MYCO_DEBUG_PRINT(fmt, ...) ((void)0)
#endif

// Error handling macros
#define MYCO_CHECK_NULL(ptr) \
    do { \
        if (!(ptr)) { \
            fprintf(stderr, "Error: NULL pointer at %s:%d\n", __FILE__, __LINE__); \
            return MYCO_ERROR_MEMORY; \
        } \
    } while(0)

#define MYCO_CHECK_ERROR(result) \
    do { \
        if ((result) != MYCO_SUCCESS) { \
            return (result); \
        } \
    } while(0)

// Memory management macros
#define MYCO_ALLOC(tracker, type) \
    (type*)memory_allocate(tracker, sizeof(type))

#define MYCO_ALLOC_ARRAY(tracker, type, count) \
    (type*)memory_allocate(tracker, sizeof(type) * (count))

#define MYCO_FREE(tracker, ptr) \
    do { \
        if (ptr) { \
            memory_free(tracker, ptr); \
            ptr = NULL; \
        } \
    } while(0)

// String utilities
#define MYCO_STRDUP(str) \
    ((str) ? strdup(str) : NULL)

#define MYCO_STRFREE(str) \
    do { \
        if (str) { \
            free(str); \
            str = NULL; \
        } \
    } while(0)

// Initialization and cleanup
int myco_initialize(void);
void myco_cleanup(void);
int myco_get_version_major(void);
int myco_get_version_minor(void);
int myco_get_version_patch(void);
const char* myco_get_version_string(void);

// Global memory tracker
extern MemoryTracker* g_memory_tracker;

// Global error state
extern int g_myco_error_code;
extern char* g_myco_error_message;

// Error handling
void myco_set_error(int code, const char* message);
void myco_clear_error(void);
int myco_get_error_code(void);
const char* myco_get_error_message(void);

#endif // MYCO_H
