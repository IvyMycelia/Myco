#ifndef DEBUG_SYSTEM_H
#define DEBUG_SYSTEM_H

#include "enhanced_error_system.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// ============================================================================
// DEBUG LEVELS
// ============================================================================

typedef enum {
    DEBUG_LEVEL_NONE = 0,
    DEBUG_LEVEL_ERROR = 1,
    DEBUG_LEVEL_WARNING = 2,
    DEBUG_LEVEL_INFO = 3,
    DEBUG_LEVEL_DEBUG = 4,
    DEBUG_LEVEL_TRACE = 5,
    DEBUG_LEVEL_VERBOSE = 6
} DebugLevel;

// ============================================================================
// DEBUG CATEGORIES
// ============================================================================

typedef enum {
    DEBUG_CATEGORY_PARSER = 0,
    DEBUG_CATEGORY_LEXER = 1,
    DEBUG_CATEGORY_INTERPRETER = 2,
    DEBUG_CATEGORY_MEMORY = 3,
    DEBUG_CATEGORY_TYPE_SYSTEM = 4,
    DEBUG_CATEGORY_OPTIMIZER = 5,
    DEBUG_CATEGORY_JIT = 6,
    DEBUG_CATEGORY_GC = 7,
    DEBUG_CATEGORY_IO = 8,
    DEBUG_CATEGORY_NETWORK = 9,
    DEBUG_CATEGORY_USER = 10
} DebugCategory;

// ============================================================================
// BREAKPOINT TYPES
// ============================================================================

typedef enum {
    BREAKPOINT_LINE = 0,
    BREAKPOINT_FUNCTION = 1,
    BREAKPOINT_CONDITION = 2,
    BREAKPOINT_EXCEPTION = 3,
    BREAKPOINT_MEMORY = 4,
    BREAKPOINT_VARIABLE = 5
} BreakpointType;

// ============================================================================
// BREAKPOINT STRUCTURE
// ============================================================================

typedef struct {
    uint32_t id;
    BreakpointType type;
    char* file_name;
    uint32_t line_number;
    char* function_name;
    char* condition;
    char* variable_name;
    bool enabled;
    uint32_t hit_count;
    uint32_t max_hits;
} Breakpoint;

// ============================================================================
// WATCH EXPRESSION
// ============================================================================

typedef struct {
    uint32_t id;
    char* expression;
    char* last_value;
    bool enabled;
    uint32_t hit_count;
} WatchExpression;

// ============================================================================
// DEBUG CONTEXT
// ============================================================================

typedef struct {
    char* current_file;
    uint32_t current_line;
    char* current_function;
    bool is_running;
    bool is_paused;
    bool is_stepping;
    bool step_over;
    bool step_into;
    bool step_out;
    
    // Call stack
    struct {
        char** functions;
        char** files;
        uint32_t* lines;
        size_t depth;
        size_t capacity;
    } call_stack;
    
    // Variables in current scope
    struct {
        char** names;
        char** values;
        char** types;
        size_t count;
        size_t capacity;
    } variables;
} DebugContext;

// ============================================================================
// DEBUG SYSTEM CONFIGURATION
// ============================================================================

typedef struct {
    DebugLevel level;
    bool enable_colors;
    bool enable_timestamps;
    bool enable_file_info;
    bool enable_function_info;
    bool enable_thread_info;
    bool enable_memory_info;
    bool enable_performance_info;
    
    char* log_file;
    char* output_file;
    
    // Category-specific settings
    bool category_enabled[11];
    DebugLevel category_levels[11];
    
    // Performance tracking
    bool track_execution_time;
    bool track_memory_usage;
    bool track_function_calls;
    
    // Debugging features
    bool enable_breakpoints;
    bool enable_watch_expressions;
    bool enable_call_tracing;
    bool enable_variable_inspection;
    bool enable_memory_inspection;
} DebugSystemConfig;

// ============================================================================
// DEBUG SYSTEM
// ============================================================================

typedef struct {
    DebugSystemConfig config;
    EnhancedErrorSystem* error_system;
    
    // Breakpoints
    Breakpoint* breakpoints;
    size_t breakpoint_count;
    size_t breakpoint_capacity;
    uint32_t next_breakpoint_id;
    
    // Watch expressions
    WatchExpression* watch_expressions;
    size_t watch_count;
    size_t watch_capacity;
    uint32_t next_watch_id;
    
    // Debug context
    DebugContext context;
    
    // Statistics
    uint64_t total_debug_messages;
    uint64_t category_counts[11];
    uint64_t level_counts[7];
    
    // Performance tracking
    uint64_t start_time;
    uint64_t last_message_time;
    size_t peak_memory_usage;
    size_t current_memory_usage;
} DebugSystem;

// ============================================================================
// FUNCTION DECLARATIONS
// ============================================================================

// System initialization and cleanup
DebugSystem* debug_system_create(void);
void debug_system_free(DebugSystem* system);
void debug_system_configure(DebugSystem* system, const DebugSystemConfig* config);

// Debug message logging
void debug_log(DebugSystem* system, DebugLevel level, DebugCategory category,
               const char* file, uint32_t line, const char* function,
               const char* format, ...);
void debug_log_simple(DebugSystem* system, DebugLevel level, const char* message);
void debug_log_with_context(DebugSystem* system, DebugLevel level, DebugCategory category,
                           const char* file, uint32_t line, const char* function,
                           const char* message, const char* context);

// Debug level and category management
void debug_set_level(DebugSystem* system, DebugLevel level);
void debug_set_category_enabled(DebugSystem* system, DebugCategory category, bool enabled);
void debug_set_category_level(DebugSystem* system, DebugCategory category, DebugLevel level);
bool debug_is_enabled(DebugSystem* system, DebugLevel level, DebugCategory category);

// Breakpoint management
uint32_t debug_add_breakpoint(DebugSystem* system, BreakpointType type, const char* file,
                             uint32_t line, const char* function, const char* condition);
void debug_remove_breakpoint(DebugSystem* system, uint32_t id);
void debug_enable_breakpoint(DebugSystem* system, uint32_t id, bool enabled);
Breakpoint* debug_find_breakpoint(DebugSystem* system, uint32_t id);
bool debug_check_breakpoint(DebugSystem* system, const char* file, uint32_t line,
                           const char* function);

// Watch expression management
uint32_t debug_add_watch(DebugSystem* system, const char* expression);
void debug_remove_watch(DebugSystem* system, uint32_t id);
void debug_enable_watch(DebugSystem* system, uint32_t id, bool enabled);
WatchExpression* debug_find_watch(DebugSystem* system, uint32_t id);
void debug_evaluate_watches(DebugSystem* system);

// Debug context management
void debug_set_context(DebugSystem* system, const char* file, uint32_t line, const char* function);
void debug_push_stack_frame(DebugSystem* system, const char* function, const char* file, uint32_t line);
void debug_pop_stack_frame(DebugSystem* system);
void debug_add_variable(DebugSystem* system, const char* name, const char* value, const char* type);
void debug_clear_variables(DebugSystem* system);

// Debugging control
void debug_pause(DebugSystem* system);
void debug_resume(DebugSystem* system);
void debug_step_over(DebugSystem* system);
void debug_step_into(DebugSystem* system);
void debug_step_out(DebugSystem* system);
bool debug_is_paused(DebugSystem* system);

// Debug information retrieval
void debug_print_call_stack(DebugSystem* system);
void debug_print_variables(DebugSystem* system);
void debug_print_breakpoints(DebugSystem* system);
void debug_print_watches(DebugSystem* system);
void debug_print_statistics(DebugSystem* system);

// Memory debugging
void debug_track_memory(DebugSystem* system, size_t size);
void debug_untrack_memory(DebugSystem* system, size_t size);
void debug_print_memory_usage(DebugSystem* system);
void debug_detect_memory_leaks(DebugSystem* system);

// Performance debugging
void debug_start_timer(DebugSystem* system, const char* name);
void debug_end_timer(DebugSystem* system, const char* name);
void debug_print_performance(DebugSystem* system);

// Debug output formatting
void debug_print_header(DebugSystem* system, DebugLevel level, DebugCategory category,
                       const char* file, uint32_t line, const char* function);
void debug_print_footer(DebugSystem* system);
char* debug_format_message(DebugSystem* system, DebugLevel level, DebugCategory category,
                          const char* file, uint32_t line, const char* function,
                          const char* message);

// Utility functions
const char* debug_get_level_name(DebugLevel level);
const char* debug_get_category_name(DebugCategory category);
const char* debug_get_level_color(DebugLevel level);
const char* debug_get_category_color(DebugCategory category);

// Global debug system access
DebugSystem* debug_system_get_global(void);
void debug_system_set_global(DebugSystem* system);
void debug_system_cleanup_global(void);

// Convenience macros
#define DEBUG_LOG(level, category, format, ...) \
    debug_log(debug_system_get_global(), level, category, __FILE__, __LINE__, __func__, format, ##__VA_ARGS__)

#define DEBUG_ERROR(category, format, ...) \
    DEBUG_LOG(DEBUG_LEVEL_ERROR, category, format, ##__VA_ARGS__)

#define DEBUG_WARNING(category, format, ...) \
    DEBUG_LOG(DEBUG_LEVEL_WARNING, category, format, ##__VA_ARGS__)

#define DEBUG_INFO(category, format, ...) \
    DEBUG_LOG(DEBUG_LEVEL_INFO, category, format, ##__VA_ARGS__)

#define DEBUG_DEBUG(category, format, ...) \
    DEBUG_LOG(DEBUG_LEVEL_DEBUG, category, format, ##__VA_ARGS__)

#define DEBUG_TRACE(category, format, ...) \
    DEBUG_LOG(DEBUG_LEVEL_TRACE, category, format, ##__VA_ARGS__)

#define DEBUG_VERBOSE(category, format, ...) \
    DEBUG_LOG(DEBUG_LEVEL_VERBOSE, category, format, ##__VA_ARGS__)

#endif // DEBUG_SYSTEM_H
