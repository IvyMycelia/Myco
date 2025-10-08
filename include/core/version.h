#ifndef VERSION_H
#define VERSION_H

// Myco Language Version Information
// Using Semantic Versioning (SemVer): MAJOR.MINOR.PATCH

#define MYCO_VERSION_MAJOR 2
#define MYCO_VERSION_MINOR 0
#define MYCO_VERSION_PATCH 0

// Version string macros
#define MYCO_VERSION_STRING "2.0.0"
#define MYCO_VERSION_FULL "v2.0.0"

// Build information
#define MYCO_BUILD_DATE __DATE__
#define MYCO_BUILD_TIME __TIME__

// Version comparison macros
#define MYCO_VERSION_CHECK(major, minor, patch) \
    (MYCO_VERSION_MAJOR > (major) || \
     (MYCO_VERSION_MAJOR == (major) && MYCO_VERSION_MINOR > (minor)) || \
     (MYCO_VERSION_MAJOR == (major) && MYCO_VERSION_MINOR == (minor) && MYCO_VERSION_PATCH >= (patch)))

// Version string generation
#define MYCO_VERSION_STRING_FORMAT "%d.%d.%d"
#define MYCO_VERSION_STRING_ARGS MYCO_VERSION_MAJOR, MYCO_VERSION_MINOR, MYCO_VERSION_PATCH

// Release information
#define MYCO_RELEASE_NAME "Async Await"
#define MYCO_RELEASE_CODENAME "Fungus"

// Feature flags for version-specific capabilities
#define MYCO_FEATURE_ASYNC_AWAIT 1
#define MYCO_FEATURE_GENERICS 1
#define MYCO_FEATURE_ENHANCED_REPL 1
#define MYCO_FEATURE_PYTHON_ERRORS 1
#define MYCO_FEATURE_MEMORY_TRACKING 1

// API version for compatibility checking
#define MYCO_API_VERSION 1

// Version information structure
typedef struct {
    int major;
    int minor;
    int patch;
    const char* release_name;
    const char* codename;
    const char* build_date;
    const char* build_time;
} MycoVersionInfo;

// Function declarations
const char* myco_get_version_string(void);
const char* myco_get_version_full(void);
const char* myco_get_release_name(void);
const char* myco_get_codename(void);
MycoVersionInfo* myco_get_version_info(void);
int myco_version_compare(int major, int minor, int patch);
void myco_print_version_info(void);

#endif // VERSION_H
