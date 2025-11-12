/**
 * @file capability_wrappers.c
 * @brief Safe capability wrappers for module security
 * 
 * This file provides example implementations of safe wrappers for dangerous libraries.
 * These wrappers can be registered as capabilities to provide restricted access to
 * modules that need limited functionality.
 */

#include "interpreter/interpreter_core.h"
#include "interpreter/value_operations.h"
#include "libs/file.h"
#include <string.h>
#include <stdio.h>
#include "../../include/utils/shared_utilities.h"

// Forward declarations
extern Value builtin_file_read(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
extern Value builtin_file_exists(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
extern Value builtin_file_size(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);

// ============================================================================
// READ-ONLY FILE SYSTEM CAPABILITY WRAPPER
// ============================================================================

/**
 * @brief Safe file read wrapper - only allows reading files
 */
static Value safe_file_read(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    // Delegate to real implementation - this is just a pass-through for now
    // In a real implementation, you'd add path validation, sandboxing, etc.
    return builtin_file_read(interpreter, args, arg_count, line, column);
}

/**
 * @brief Safe file exists wrapper - only checks existence
 */
static Value safe_file_exists(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    return builtin_file_exists(interpreter, args, arg_count, line, column);
}

/**
 * @brief Safe file size wrapper - only gets file size
 */
static Value safe_file_size(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    return builtin_file_size(interpreter, args, arg_count, line, column);
}

/**
 * @brief Create a read-only file system capability wrapper
 * 
 * This creates a safe wrapper around the file library that only exposes
 * read operations. Write, delete, and other dangerous operations are excluded.
 * 
 * @param interpreter The interpreter instance
 * @return A Value containing the safe file system library object
 */
Value create_readonly_fs_capability(Interpreter* interpreter) {
    Value safe_fs = value_create_object(8);
    value_object_set(&safe_fs, "__type__", value_create_string("Library"));
    value_object_set(&safe_fs, "type", value_create_string("Library"));
    value_object_set(&safe_fs, "__library_name__", value_create_string("file"));
    value_object_set(&safe_fs, "__is_safe_wrapper__", value_create_boolean(1));
    
    // Only expose safe read operations
    value_object_set(&safe_fs, "read", value_create_builtin_function(safe_file_read));
    value_object_set(&safe_fs, "exists", value_create_builtin_function(safe_file_exists));
    value_object_set(&safe_fs, "size", value_create_builtin_function(safe_file_size));
    // Note: write, delete, append, etc. are intentionally excluded
    
    return safe_fs;
}

// ============================================================================
// SANDBOXED NETWORK CAPABILITY WRAPPER
// ============================================================================

/**
 * @brief Create a sandboxed network capability wrapper
 * 
 * This creates a safe wrapper around network libraries that restricts
 * access to specific domains or IP ranges. For now, this is a placeholder
 * that demonstrates the pattern.
 * 
 * @param interpreter The interpreter instance
 * @param allowed_domains Array of allowed domain names (NULL for no restrictions)
 * @param domain_count Number of allowed domains
 * @return A Value containing the safe network library object
 */
Value create_sandboxed_net_capability(Interpreter* interpreter, const char** allowed_domains, size_t domain_count) {
    Value safe_net = value_create_object(8);
    value_object_set(&safe_net, "__type__", value_create_string("Library"));
    value_object_set(&safe_net, "type", value_create_string("Library"));
    value_object_set(&safe_net, "__library_name__", value_create_string("http"));
    value_object_set(&safe_net, "__is_safe_wrapper__", value_create_boolean(1));
    
    // In a real implementation, you would:
    // 1. Store allowed_domains in the wrapper object
    // 2. Create wrapper functions that validate URLs before making requests
    // 3. Only expose GET requests (no POST/PUT/DELETE)
    // 4. Add rate limiting
    
    // For now, this is a placeholder that shows the structure
    // The actual HTTP library would need to be wrapped with domain checking
    
    return safe_net;
}

// ============================================================================
// HELPER FUNCTION TO REGISTER DEFAULT CAPABILITIES
// ============================================================================

/**
 * @brief Register default safe capability wrappers
 * 
 * This function registers common safe wrappers that can be used by modules.
 * The host application can call this to set up default capabilities, or
 * create custom wrappers and register them individually.
 * 
 * @param interpreter The interpreter instance
 */
void register_default_capabilities(Interpreter* interpreter) {
    if (!interpreter) return;
    
    // Register read-only file system capability
    Value readonly_fs = create_readonly_fs_capability(interpreter);
    interpreter_register_capability(interpreter, "fs", readonly_fs, 1); // 1 = is_safe
    value_free(&readonly_fs);
    
    // Register sandboxed network capability (no domain restrictions by default)
    Value sandboxed_net = create_sandboxed_net_capability(interpreter, NULL, 0);
    interpreter_register_capability(interpreter, "net", sandboxed_net, 1); // 1 = is_safe
    value_free(&sandboxed_net);
    
    // Add more default capabilities as needed
}

