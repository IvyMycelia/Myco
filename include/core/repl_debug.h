#ifndef REPL_DEBUG_H
#define REPL_DEBUG_H

#include "debug_system.h"
#include "enhanced_error_system.h"
#include <stdint.h>
#include <stdbool.h>

// ============================================================================
// REPL DEBUG COMMANDS
// ============================================================================

typedef enum {
    REPL_CMD_HELP = 0,
    REPL_CMD_QUIT = 1,
    REPL_CMD_CLEAR = 2,
    REPL_CMD_DEBUG = 3,
    REPL_CMD_BREAKPOINT = 4,
    REPL_CMD_WATCH = 5,
    REPL_CMD_STEP = 6,
    REPL_CMD_CONTINUE = 7,
    REPL_CMD_VARIABLES = 8,
    REPL_CMD_STACK = 9,
    REPL_CMD_MEMORY = 10,
    REPL_CMD_PERFORMANCE = 11,
    REPL_CMD_ERRORS = 12,
    REPL_CMD_LOGS = 13,
    REPL_CMD_CONFIG = 14,
    REPL_CMD_EXPORT = 15,
    REPL_CMD_IMPORT = 16,
    REPL_CMD_RESET = 17,
    REPL_CMD_VERSION = 18,
    REPL_CMD_ABOUT = 19,
    REPL_CMD_UNKNOWN = 20
} ReplCommand;

// ============================================================================
// REPL DEBUG COMMAND STRUCTURE
// ============================================================================

typedef struct {
    ReplCommand command;
    char* name;
    char* description;
    char* usage;
    char* aliases[5];
    int alias_count;
    bool requires_args;
    int min_args;
    int max_args;
} ReplCommandInfo;

// ============================================================================
// REPL DEBUG SESSION
// ============================================================================

typedef struct {
    DebugSystem* debug_system;
    EnhancedErrorSystem* error_system;
    
    // Session state
    bool is_active;
    bool is_debugging;
    bool is_stepping;
    bool is_paused;
    
    // Command history
    char** command_history;
    size_t history_count;
    size_t history_capacity;
    int history_index;
    char* last_command;
    
    // Current context
    char* current_file;
    uint32_t current_line;
    char* current_function;
    
    // Output settings
    bool verbose_output;
    bool color_output;
    bool timestamp_output;
    
    // Statistics
    uint64_t commands_executed;
    uint64_t debug_messages;
    uint64_t errors_reported;
    uint64_t breakpoints_hit;
    
    // Performance tracking
    uint64_t session_start_time;
    uint64_t last_command_time;
} ReplDebugSession;

// ============================================================================
// FUNCTION DECLARATIONS
// ============================================================================

// Session management
ReplDebugSession* repl_debug_session_create(void);
void repl_debug_session_free(ReplDebugSession* session);
void repl_debug_session_start(ReplDebugSession* session);
void repl_debug_session_stop(ReplDebugSession* session);

// Command processing
ReplCommand repl_parse_command(const char* input);
ReplCommandInfo* repl_get_command_info(ReplCommand command);
void repl_execute_command(ReplDebugSession* session, const char* input);
void repl_show_help_for_command(ReplDebugSession* session, ReplCommand command);

// Debug commands
void repl_cmd_debug(ReplDebugSession* session, const char* args);
void repl_cmd_breakpoint(ReplDebugSession* session, const char* args);
void repl_cmd_watch(ReplDebugSession* session, const char* args);
void repl_cmd_step(ReplDebugSession* session, const char* args);
void repl_cmd_continue(ReplDebugSession* session, const char* args);
void repl_cmd_variables(ReplDebugSession* session, const char* args);
void repl_cmd_stack(ReplDebugSession* session, const char* args);
void repl_cmd_memory(ReplDebugSession* session, const char* args);
void repl_cmd_performance(ReplDebugSession* session, const char* args);
void repl_cmd_errors(ReplDebugSession* session, const char* args);
void repl_cmd_logs(ReplDebugSession* session, const char* args);
void repl_cmd_config(ReplDebugSession* session, const char* args);
void repl_cmd_export(ReplDebugSession* session, const char* args);
void repl_cmd_import(ReplDebugSession* session, const char* args);
void repl_cmd_reset(ReplDebugSession* session, const char* args);

// Command utilities
char* repl_parse_args(const char* input, int* argc, char*** argv);
void repl_free_args(int argc, char** argv);
bool repl_parse_bool(const char* str);
int repl_parse_int(const char* str);
uint32_t repl_parse_uint32(const char* str);

// Output formatting
void repl_print_header(ReplDebugSession* session);
void repl_print_prompt(ReplDebugSession* session);
void repl_print_success(ReplDebugSession* session, const char* message);
void repl_print_warning(ReplDebugSession* session, const char* message);
void repl_print_info(ReplDebugSession* session, const char* message);
void repl_print_debug(ReplDebugSession* session, const char* message);

// Interactive features
void repl_interactive_mode(ReplDebugSession* session);
void repl_batch_mode(ReplDebugSession* session, const char* script_file);
void repl_auto_complete(ReplDebugSession* session, const char* input, char* completion);
void repl_command_suggestions(ReplDebugSession* session, const char* input);

// History management
void repl_show_history(ReplDebugSession* session, int count);
void repl_clear_history(ReplDebugSession* session);
char* repl_get_history_item(ReplDebugSession* session, int index);

// Configuration
void repl_set_verbose(ReplDebugSession* session, bool verbose);
void repl_set_colors(ReplDebugSession* session, bool colors);
void repl_set_timestamps(ReplDebugSession* session, bool timestamps);
void repl_show_config(ReplDebugSession* session);

// Statistics and reporting
void repl_show_statistics(ReplDebugSession* session);
void repl_show_session_info(ReplDebugSession* session);
void repl_export_session(ReplDebugSession* session, const char* filename);
void repl_import_session(ReplDebugSession* session, const char* filename);

// Error handling
void repl_handle_error(ReplDebugSession* session, const char* error_message);
void repl_handle_warning(ReplDebugSession* session, const char* warning_message);
void repl_handle_debug_message(ReplDebugSession* session, DebugLevel level, 
                              DebugCategory category, const char* message);

// Utility functions
const char* repl_get_command_name(ReplCommand command);
const char* repl_get_command_description(ReplCommand command);
const char* repl_get_command_usage(ReplCommand command);
bool repl_is_valid_command(const char* command);
ReplCommand repl_find_command_by_name(const char* name);

// Global REPL debug session access
ReplDebugSession* repl_debug_session_get_global(void);
void repl_debug_session_set_global(ReplDebugSession* session);
void repl_debug_session_cleanup_global(void);

// Convenience macros
#define REPL_PRINT_SUCCESS(session, format, ...) \
    repl_print_success(session, format, ##__VA_ARGS__)

#define REPL_PRINT_ERROR(session, format, ...) \
    repl_print_error(session, format, ##__VA_ARGS__)

#define REPL_PRINT_WARNING(session, format, ...) \
    repl_print_warning(session, format, ##__VA_ARGS__)

#define REPL_PRINT_INFO(session, format, ...) \
    repl_print_info(session, format, ##__VA_ARGS__)

#define REPL_PRINT_DEBUG(session, format, ...) \
    repl_print_debug(session, format, ##__VA_ARGS__)

#endif // REPL_DEBUG_H
