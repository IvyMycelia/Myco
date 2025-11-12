/**
 * @file capability_wrappers.h
 * @brief Safe capability wrapper declarations
 */

#ifndef CAPABILITY_WRAPPERS_H
#define CAPABILITY_WRAPPERS_H

#include "interpreter/interpreter_core.h"

/**
 * @brief Create a read-only file system capability wrapper
 * 
 * This creates a safe wrapper around the file library that only exposes
 * read operations. Write, delete, and other dangerous operations are excluded.
 * 
 * @param interpreter The interpreter instance
 * @return A Value containing the safe file system library object
 */
Value create_readonly_fs_capability(Interpreter* interpreter);

/**
 * @brief Create a sandboxed network capability wrapper
 * 
 * This creates a safe wrapper around network libraries that restricts
 * access to specific domains or IP ranges.
 * 
 * @param interpreter The interpreter instance
 * @param allowed_domains Array of allowed domain names (NULL for no restrictions)
 * @param domain_count Number of allowed domains
 * @return A Value containing the safe network library object
 */
Value create_sandboxed_net_capability(Interpreter* interpreter, const char** allowed_domains, size_t domain_count);

/**
 * @brief Register default safe capability wrappers
 * 
 * This function registers common safe wrappers that can be used by modules.
 * 
 * @param interpreter The interpreter instance
 */
void register_default_capabilities(Interpreter* interpreter);

#endif // CAPABILITY_WRAPPERS_H

