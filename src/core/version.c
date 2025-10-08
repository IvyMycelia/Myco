#include "version.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global version info instance
static MycoVersionInfo version_info = {
    .major = MYCO_VERSION_MAJOR,
    .minor = MYCO_VERSION_MINOR,
    .patch = MYCO_VERSION_PATCH,
    .release_name = MYCO_RELEASE_NAME,
    .codename = NULL,
    .build_date = MYCO_BUILD_DATE,
    .build_time = MYCO_BUILD_TIME
};

/**
 * @brief Get the version string (e.g., "2.0.0")
 */
const char* myco_get_version_string(void) {
    return MYCO_VERSION_STRING;
}

/**
 * @brief Get the full version string (e.g., "v2.0.0")
 */
const char* myco_get_version_full(void) {
    return MYCO_VERSION_FULL;
}

/**
 * @brief Get the release name
 */
const char* myco_get_release_name(void) {
    return MYCO_RELEASE_NAME;
}

/**
 * @brief Get the release codename
 */
const char* myco_get_codename(void) {
    return MYCO_RELEASE_CODENAME;
}

/**
 * @brief Get the complete version information structure
 */
MycoVersionInfo* myco_get_version_info(void) {
    return &version_info;
}

/**
 * @brief Compare current version with given version
 * @param major Major version number
 * @param minor Minor version number  
 * @param patch Patch version number
 * @return 1 if current version is greater, 0 if equal, -1 if less
 */
int myco_version_compare(int major, int minor, int patch) {
    if (MYCO_VERSION_MAJOR > major) return 1;
    if (MYCO_VERSION_MAJOR < major) return -1;
    
    if (MYCO_VERSION_MINOR > minor) return 1;
    if (MYCO_VERSION_MINOR < minor) return -1;
    
    if (MYCO_VERSION_PATCH > patch) return 1;
    if (MYCO_VERSION_PATCH < patch) return -1;
    
    return 0;
}

/**
 * @brief Print comprehensive version information
 */
void myco_print_version_info(void) {
    printf("Myco Language Interpreter %s\n", myco_get_version_full());
    printf("Release: %s\n", myco_get_release_name());
    printf("Built on %s at %s\n", version_info.build_date, version_info.build_time);
    printf("API Version: %d\n", MYCO_API_VERSION);
    
    // Print feature flags
    printf("\nFeatures:\n");
    if (MYCO_FEATURE_ASYNC_AWAIT) printf("  ✓ Async/Await Programming\n");
    if (MYCO_FEATURE_GENERICS) printf("  ✓ Generic Type System\n");
    if (MYCO_FEATURE_ENHANCED_REPL) printf("  ✓ Enhanced REPL with Debugging\n");
    if (MYCO_FEATURE_ENHANCED_ERRORS) printf("  ✓ Enhanced Error Handling\n");
    if (MYCO_FEATURE_MEMORY_TRACKING) printf("  ✓ Memory Management Tracking\n");
    
    printf("\nCopyright (c) 2025 Myco Language Project\n");
}

/**
 * @brief Check if a feature is available in the current version
 */
int myco_has_feature(const char* feature_name) {
    if (strcmp(feature_name, "async_await") == 0) return MYCO_FEATURE_ASYNC_AWAIT;
    if (strcmp(feature_name, "generics") == 0) return MYCO_FEATURE_GENERICS;
    if (strcmp(feature_name, "enhanced_repl") == 0) return MYCO_FEATURE_ENHANCED_REPL;
    if (strcmp(feature_name, "enhanced_errors") == 0) return MYCO_FEATURE_ENHANCED_ERRORS;
    if (strcmp(feature_name, "memory_tracking") == 0) return MYCO_FEATURE_MEMORY_TRACKING;
    return 0;
}

/**
 * @brief Get version compatibility information
 */
const char* myco_get_compatibility_info(void) {
    static char compatibility[256];
    snprintf(compatibility, sizeof(compatibility), 
             "Compatible with Myco %s+ (API v%d)", 
             MYCO_VERSION_STRING, MYCO_API_VERSION);
    return compatibility;
}
