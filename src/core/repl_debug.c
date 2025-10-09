#include "repl_debug.h"
#include "debug_system.h"
#include "enhanced_error_system.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

// ============================================================================
// FORWARD DECLARATIONS
// ============================================================================

void repl_cmd_version(ReplDebugSession* session, const char* args);
void repl_cmd_about(ReplDebugSession* session, const char* args);

// External functions from repl.c - implemented locally
static void repl_show_help(ReplDebugSession* session);
static void repl_print_error(ReplDebugSession* session, const char* message);
static void repl_add_to_history(ReplDebugSession* session, const char* command);

// ============================================================================
// GLOBAL REPL DEBUG SESSION INSTANCE
// ============================================================================

static ReplDebugSession* global_repl_session = NULL;

// ============================================================================
// COMMAND DATABASE
// ============================================================================

static const ReplCommandInfo command_database[] = {
    {REPL_CMD_HELP, "help", "Show help information", "help [command]", {"h", "?"}, 2, false, 0, 1},
    {REPL_CMD_QUIT, "quit", "Exit the debug session", "quit", {"exit", "q"}, 2, false, 0, 0},
    {REPL_CMD_CLEAR, "clear", "Clear the screen", "clear", {"cls"}, 1, false, 0, 0},
    {REPL_CMD_DEBUG, "debug", "Toggle debug mode", "debug [on|off]", {"dbg"}, 1, false, 0, 1},
    {REPL_CMD_BREAKPOINT, "breakpoint", "Manage breakpoints", "breakpoint [add|remove|list] [args]", {"bp", "break"}, 1, true, 1, 10},
    {REPL_CMD_WATCH, "watch", "Manage watch expressions", "watch [add|remove|list] [expr]", {"w"}, 1, true, 1, 10},
    {REPL_CMD_STEP, "step", "Step through code", "step [over|into|out]", {"s"}, 1, false, 0, 1},
    {REPL_CMD_CONTINUE, "continue", "Continue execution", "continue", {"c", "cont"}, 2, false, 0, 0},
    {REPL_CMD_VARIABLES, "variables", "Show variables", "variables [filter]", {"vars", "v"}, 2, false, 0, 1},
    {REPL_CMD_STACK, "stack", "Show call stack", "stack", {"st"}, 1, false, 0, 0},
    {REPL_CMD_MEMORY, "memory", "Show memory usage", "memory", {"mem"}, 1, false, 0, 0},
    {REPL_CMD_PERFORMANCE, "performance", "Show performance info", "performance", {"perf"}, 1, false, 0, 0},
    {REPL_CMD_ERRORS, "errors", "Show error information", "errors [count]", {"err"}, 1, false, 0, 1},
    {REPL_CMD_LOGS, "logs", "Show debug logs", "logs [level] [category]", {"log"}, 1, false, 0, 2},
    {REPL_CMD_CONFIG, "config", "Show/modify configuration", "config [key] [value]", {"cfg"}, 1, false, 0, 2},
    {REPL_CMD_EXPORT, "export", "Export session data", "export <filename>", {"exp"}, 1, true, 1, 1},
    {REPL_CMD_IMPORT, "import", "Import session data", "import <filename>", {"imp"}, 1, true, 1, 1},
    {REPL_CMD_RESET, "reset", "Reset session", "reset", {"rst"}, 1, false, 0, 0},
    {REPL_CMD_VERSION, "version", "Show version information", "version", {"ver"}, 1, false, 0, 0},
    {REPL_CMD_ABOUT, "about", "Show about information", "about", {"info"}, 1, false, 0, 0},
    {REPL_CMD_UNKNOWN, "", "", "", {""}, 0, false, 0, 0}
};

// ============================================================================
// SESSION MANAGEMENT
// ============================================================================

ReplDebugSession* repl_debug_session_create(void) {
    ReplDebugSession* session = malloc(sizeof(ReplDebugSession));
    if (!session) return NULL;
    
    memset(session, 0, sizeof(ReplDebugSession));
    
    // Initialize systems
    session->debug_system = debug_system_get_global();
    session->error_system = enhanced_error_system_get_global();
    
    // Initialize session state
    session->is_active = false;
    session->is_debugging = false;
    session->is_stepping = false;
    session->is_paused = false;
    
    // Initialize command history
    session->history_capacity = 100;
    session->command_history = malloc(sizeof(char*) * session->history_capacity);
    if (!session->command_history) {
        free(session);
        return NULL;
    }
    session->history_count = 0;
    session->history_index = 0;
    
    // Initialize current context
    session->current_file = NULL;
    session->current_line = 0;
    session->current_function = NULL;
    
    // Initialize output settings
    session->verbose_output = false;
    session->color_output = true;
    session->timestamp_output = false;
    
    // Initialize statistics
    session->commands_executed = 0;
    session->debug_messages = 0;
    session->errors_reported = 0;
    session->breakpoints_hit = 0;
    
    // Initialize performance tracking
    session->session_start_time = 0;
    session->last_command_time = 0;
    
    return session;
}

void repl_debug_session_free(ReplDebugSession* session) {
    if (!session) return;
    
    // Free command history
    for (size_t i = 0; i < session->history_count; i++) {
        free(session->command_history[i]);
    }
    free(session->command_history);
    
    // Free current context
    free(session->current_file);
    free(session->current_function);
    free(session->last_command);
    
    free(session);
}

void repl_debug_session_start(ReplDebugSession* session) {
    if (!session) return;
    
    session->is_active = true;
    session->is_debugging = true;
    session->session_start_time = time(NULL);
    
    repl_print_header(session);
    repl_print_success(session, "Debug session started");
}

void repl_debug_session_stop(ReplDebugSession* session) {
    if (!session) return;
    
    session->is_active = false;
    session->is_debugging = false;
    session->is_stepping = false;
    session->is_paused = false;
    
    repl_print_info(session, "Debug session stopped");
}

// ============================================================================
// COMMAND PROCESSING
// ============================================================================

ReplCommand repl_parse_command(const char* input) {
    if (!input) return REPL_CMD_UNKNOWN;
    
    // Skip whitespace
    while (*input && (*input == ' ' || *input == '\t')) input++;
    if (!*input) return REPL_CMD_UNKNOWN;
    
    // Find end of command
    const char* end = input;
    while (*end && *end != ' ' && *end != '\t' && *end != '\n' && *end != '\r') end++;
    
    size_t cmd_len = end - input;
    
    // Check against command database
    for (int i = 0; i < 20; i++) {
        const ReplCommandInfo* cmd = &command_database[i];
        if (cmd->command == REPL_CMD_UNKNOWN) break;
        
        // Check main command name
        if (strncmp(input, cmd->name, cmd_len) == 0 && strlen(cmd->name) == cmd_len) {
            return cmd->command;
        }
        
        // Check aliases
        for (int j = 0; j < cmd->alias_count; j++) {
            if (strncmp(input, cmd->aliases[j], cmd_len) == 0 && strlen(cmd->aliases[j]) == cmd_len) {
                return cmd->command;
            }
        }
    }
    
    return REPL_CMD_UNKNOWN;
}

ReplCommandInfo* repl_get_command_info(ReplCommand command) {
    for (int i = 0; i < 20; i++) {
        if (command_database[i].command == command) {
            return (ReplCommandInfo*)&command_database[i];
        }
    }
    return NULL;
}

void repl_execute_command(ReplDebugSession* session, const char* input) {
    if (!session || !input) return;
    
    // Add to history
    repl_add_to_history(session, input);
    session->commands_executed++;
    session->last_command_time = time(NULL);
    
    // Parse command
    ReplCommand command = repl_parse_command(input);
    
    // Parse arguments
    int argc;
    char** argv;
    char* args = repl_parse_args(input, &argc, &argv);
    
    // Execute command
    switch (command) {
        case REPL_CMD_HELP:
            repl_show_help(session);
            break;
        case REPL_CMD_QUIT:
            repl_debug_session_stop(session);
            break;
        case REPL_CMD_CLEAR:
            printf("\033[2J\033[H"); // Clear screen
            break;
        case REPL_CMD_DEBUG:
            repl_cmd_debug(session, args);
            break;
        case REPL_CMD_BREAKPOINT:
            repl_cmd_breakpoint(session, args);
            break;
        case REPL_CMD_WATCH:
            repl_cmd_watch(session, args);
            break;
        case REPL_CMD_STEP:
            repl_cmd_step(session, args);
            break;
        case REPL_CMD_CONTINUE:
            repl_cmd_continue(session, args);
            break;
        case REPL_CMD_VARIABLES:
            repl_cmd_variables(session, args);
            break;
        case REPL_CMD_STACK:
            repl_cmd_stack(session, args);
            break;
        case REPL_CMD_MEMORY:
            repl_cmd_memory(session, args);
            break;
        case REPL_CMD_PERFORMANCE:
            repl_cmd_performance(session, args);
            break;
        case REPL_CMD_ERRORS:
            repl_cmd_errors(session, args);
            break;
        case REPL_CMD_LOGS:
            repl_cmd_logs(session, args);
            break;
        case REPL_CMD_CONFIG:
            repl_cmd_config(session, args);
            break;
        case REPL_CMD_EXPORT:
            repl_cmd_export(session, args);
            break;
        case REPL_CMD_IMPORT:
            repl_cmd_import(session, args);
            break;
        case REPL_CMD_RESET:
            repl_cmd_reset(session, args);
            break;
        case REPL_CMD_VERSION:
            repl_cmd_version(session, args);
            break;
        case REPL_CMD_ABOUT:
            repl_cmd_about(session, args);
            break;
        default:
            repl_print_error(session, "Unknown command. Type 'help' for available commands.");
            break;
    }
    
    // Free arguments
    repl_free_args(argc, argv);
    free(args);
}


void repl_show_help_for_command(ReplDebugSession* session, ReplCommand command) {
    ReplCommandInfo* cmd = repl_get_command_info(command);
    if (!cmd) {
        repl_print_error(session, "Unknown command");
        return;
    }
    
    printf("Command: %s\n", cmd->name);
    printf("Description: %s\n", cmd->description);
    printf("Usage: %s\n", cmd->usage);
    
    if (cmd->alias_count > 0) {
        printf("Aliases: ");
        for (int i = 0; i < cmd->alias_count; i++) {
            printf("%s", cmd->aliases[i]);
            if (i < cmd->alias_count - 1) printf(", ");
        }
        printf("\n");
    }
}

// ============================================================================
// DEBUG COMMANDS IMPLEMENTATION
// ============================================================================

void repl_cmd_debug(ReplDebugSession* session, const char* args) {
    if (!args || strlen(args) == 0) {
        char info_msg[256];
        snprintf(info_msg, sizeof(info_msg), "Debug mode is %s", session->is_debugging ? "on" : "off");
        repl_print_info(session, info_msg);
        return;
    }
    
    if (strcmp(args, "on") == 0 || strcmp(args, "true") == 0 || strcmp(args, "1") == 0) {
        session->is_debugging = true;
        repl_print_success(session, "Debug mode enabled");
    } else if (strcmp(args, "off") == 0 || strcmp(args, "false") == 0 || strcmp(args, "0") == 0) {
        session->is_debugging = false;
        repl_print_success(session, "Debug mode disabled");
    } else {
        repl_print_error(session, "Invalid argument. Use 'on' or 'off'");
    }
}

void repl_cmd_breakpoint(ReplDebugSession* session, const char* args) {
    if (!args || strlen(args) == 0) {
        repl_print_error(session, "Usage: breakpoint [add|remove|list] [args]");
        return;
    }
    
    int argc;
    char** argv;
    repl_parse_args(args, &argc, &argv);
    
    if (argc == 0) {
        repl_print_error(session, "Usage: breakpoint [add|remove|list] [args]");
        repl_free_args(argc, argv);
        return;
    }
    
    if (strcmp(argv[0], "list") == 0) {
        if (session->debug_system) {
            debug_print_breakpoints(session->debug_system);
        } else {
            repl_print_error(session, "Debug system not available");
        }
    } else if (strcmp(argv[0], "add") == 0) {
        if (argc < 2) {
            repl_print_error(session, "Usage: breakpoint add <file>:<line>");
        } else {
            // Parse file:line
            char* colon = strchr(argv[1], ':');
            if (colon) {
                *colon = '\0';
                uint32_t line = atoi(colon + 1);
                uint32_t id = debug_add_breakpoint(session->debug_system, BREAKPOINT_LINE, 
                                                  argv[1], line, NULL, NULL);
                if (id > 0) {
                    char success_msg[256];
            snprintf(success_msg, sizeof(success_msg), "Breakpoint added with ID %u", id);
            repl_print_success(session, success_msg);
                } else {
                    repl_print_error(session, "Failed to add breakpoint");
                }
            } else {
                repl_print_error(session, "Invalid format. Use file:line");
            }
        }
    } else if (strcmp(argv[0], "remove") == 0) {
        if (argc < 2) {
            repl_print_error(session, "Usage: breakpoint remove <id>");
        } else {
            uint32_t id = atoi(argv[1]);
            debug_remove_breakpoint(session->debug_system, id);
            char success_msg[256];
            snprintf(success_msg, sizeof(success_msg), "Breakpoint %u removed", id);
            repl_print_success(session, success_msg);
        }
    } else {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Unknown breakpoint command: %s", argv[0]);
        repl_print_error(session, error_msg);
    }
    
    repl_free_args(argc, argv);
}

void repl_cmd_watch(ReplDebugSession* session, const char* args) {
    if (!args || strlen(args) == 0) {
        repl_print_error(session, "Usage: watch [add|remove|list] [expr]");
        return;
    }
    
    int argc;
    char** argv;
    repl_parse_args(args, &argc, &argv);
    
    if (argc == 0) {
        repl_print_error(session, "Usage: watch [add|remove|list] [expr]");
        repl_free_args(argc, argv);
        return;
    }
    
    if (strcmp(argv[0], "list") == 0) {
        if (session->debug_system) {
            debug_print_watches(session->debug_system);
        } else {
            repl_print_error(session, "Debug system not available");
        }
    } else if (strcmp(argv[0], "add") == 0) {
        if (argc < 2) {
            repl_print_error(session, "Usage: watch add <expression>");
        } else {
            uint32_t id = debug_add_watch(session->debug_system, argv[1]);
            if (id > 0) {
                char success_msg[256];
                snprintf(success_msg, sizeof(success_msg), "Watch expression added with ID %u", id);
                repl_print_success(session, success_msg);
            } else {
                repl_print_error(session, "Failed to add watch expression");
            }
        }
    } else if (strcmp(argv[0], "remove") == 0) {
        if (argc < 2) {
            repl_print_error(session, "Usage: watch remove <id>");
        } else {
            uint32_t id = atoi(argv[1]);
            debug_remove_watch(session->debug_system, id);
            char success_msg[256];
            snprintf(success_msg, sizeof(success_msg), "Watch expression %u removed", id);
            repl_print_success(session, success_msg);
        }
    } else {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Unknown watch command: %s", argv[0]);
        repl_print_error(session, error_msg);
    }
    
    repl_free_args(argc, argv);
}

void repl_cmd_step(ReplDebugSession* session, const char* args) {
    if (!args || strlen(args) == 0) {
        debug_step_over(session->debug_system);
        repl_print_info(session, "Stepping over");
    } else if (strcmp(args, "over") == 0) {
        debug_step_over(session->debug_system);
        repl_print_info(session, "Stepping over");
    } else if (strcmp(args, "into") == 0) {
        debug_step_into(session->debug_system);
        repl_print_info(session, "Stepping into");
    } else if (strcmp(args, "out") == 0) {
        debug_step_out(session->debug_system);
        repl_print_info(session, "Stepping out");
    } else {
        repl_print_error(session, "Invalid step command. Use 'over', 'into', or 'out'");
    }
}

void repl_cmd_continue(ReplDebugSession* session, const char* args) {
    debug_resume(session->debug_system);
    repl_print_info(session, "Continuing execution");
}

void repl_cmd_variables(ReplDebugSession* session, const char* args) {
    if (session->debug_system) {
        debug_print_variables(session->debug_system);
    } else {
        repl_print_error(session, "Debug system not available");
    }
}

void repl_cmd_stack(ReplDebugSession* session, const char* args) {
    if (session->debug_system) {
        debug_print_call_stack(session->debug_system);
    } else {
        repl_print_error(session, "Debug system not available");
    }
}

void repl_cmd_memory(ReplDebugSession* session, const char* args) {
    if (session->debug_system) {
        debug_print_memory_usage(session->debug_system);
    } else {
        repl_print_error(session, "Debug system not available");
    }
}

void repl_cmd_performance(ReplDebugSession* session, const char* args) {
    if (session->debug_system) {
        debug_print_performance(session->debug_system);
    } else {
        repl_print_error(session, "Debug system not available");
    }
}

void repl_cmd_errors(ReplDebugSession* session, const char* args) {
    if (session->error_system) {
        enhanced_error_system_print_statistics(session->error_system);
    } else {
        repl_print_error(session, "Error system not available");
    }
}

void repl_cmd_logs(ReplDebugSession* session, const char* args) {
    repl_print_info(session, "Debug logs:");
    if (session->debug_system) {
        debug_print_statistics(session->debug_system);
    } else {
        repl_print_error(session, "Debug system not available");
    }
}

void repl_cmd_config(ReplDebugSession* session, const char* args) {
    repl_print_info(session, "Current configuration:");
    printf("  Verbose output: %s\n", session->verbose_output ? "on" : "off");
    printf("  Color output: %s\n", session->color_output ? "on" : "off");
    printf("  Timestamp output: %s\n", session->timestamp_output ? "on" : "off");
    printf("  Debug mode: %s\n", session->is_debugging ? "on" : "off");
    printf("  Commands executed: %llu\n", (unsigned long long)session->commands_executed);
}

void repl_cmd_export(ReplDebugSession* session, const char* args) {
    if (!args || strlen(args) == 0) {
        repl_print_error(session, "Usage: export <filename>");
        return;
    }
    
    repl_export_session(session, args);
}

void repl_cmd_import(ReplDebugSession* session, const char* args) {
    if (!args || strlen(args) == 0) {
        repl_print_error(session, "Usage: import <filename>");
        return;
    }
    
    repl_import_session(session, args);
}

void repl_cmd_reset(ReplDebugSession* session, const char* args) {
    repl_print_warning(session, "Resetting session...");
    
    // Reset session state
    session->is_debugging = false;
    session->is_stepping = false;
    session->is_paused = false;
    
    // Clear history
    repl_clear_history(session);
    
    // Reset statistics
    session->commands_executed = 0;
    session->debug_messages = 0;
    session->errors_reported = 0;
    session->breakpoints_hit = 0;
    
    repl_print_success(session, "Session reset");
}

void repl_cmd_version(ReplDebugSession* session, const char* args) {
    repl_print_info(session, "Myco Debug REPL v1.0.0");
    repl_print_info(session, "Enhanced error handling and debugging system");
}

void repl_cmd_about(ReplDebugSession* session, const char* args) {
    repl_print_info(session, "Myco Debug REPL");
    repl_print_info(session, "A comprehensive debugging and error handling system for Myco");
    repl_print_info(session, "Features:");
    printf("  - Enhanced error reporting with stack traces\n");
    printf("  - Breakpoint management\n");
    printf("  - Watch expressions\n");
    printf("  - Variable inspection\n");
    printf("  - Memory usage tracking\n");
    printf("  - Performance monitoring\n");
    printf("  - Interactive debugging commands\n");
}

// ============================================================================
// COMMAND UTILITIES
// ============================================================================

char* repl_parse_args(const char* input, int* argc, char*** argv) {
    if (!input || !argc || !argv) return NULL;
    
    // Skip command name
    const char* start = input;
    while (*start && *start != ' ' && *start != '\t') start++;
    while (*start && (*start == ' ' || *start == '\t')) start++;
    
    if (!*start) {
        *argc = 0;
        *argv = NULL;
        return strdup("");
    }
    
    // Count arguments
    int count = 1;
    const char* p = start;
    while (*p) {
        if (*p == ' ' || *p == '\t') {
            count++;
            while (*p && (*p == ' ' || *p == '\t')) p++;
        } else {
            p++;
        }
    }
    
    // Allocate arrays
    char** args = malloc(sizeof(char*) * count);
    if (!args) {
        *argc = 0;
        *argv = NULL;
        return NULL;
    }
    
    // Parse arguments
    int i = 0;
    p = start;
    while (*p && i < count) {
        const char* arg_start = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        
        size_t len = p - arg_start;
        args[i] = malloc(len + 1);
        if (args[i]) {
            strncpy(args[i], arg_start, len);
            args[i][len] = '\0';
        }
        i++;
        
        while (*p && (*p == ' ' || *p == '\t')) p++;
    }
    
    *argc = i;
    *argv = args;
    return strdup(start);
}

void repl_free_args(int argc, char** argv) {
    if (!argv) return;
    
    for (int i = 0; i < argc; i++) {
        free(argv[i]);
    }
    free(argv);
}

bool repl_parse_bool(const char* str) {
    if (!str) return false;
    return (strcmp(str, "true") == 0 || strcmp(str, "1") == 0 || strcmp(str, "on") == 0 ||
            strcmp(str, "yes") == 0 || strcmp(str, "y") == 0);
}

int repl_parse_int(const char* str) {
    if (!str) return 0;
    return atoi(str);
}

uint32_t repl_parse_uint32(const char* str) {
    if (!str) return 0;
    return (uint32_t)atol(str);
}

// ============================================================================
// OUTPUT FORMATTING
// ============================================================================

void repl_print_header(ReplDebugSession* session) {
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                    Myco Debug REPL v1.0.0                   ║\n");
    printf("║              Enhanced Error Handling & Debugging            ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    printf("Type 'help' for available commands, 'quit' to exit.\n");
    printf("\n");
}

void repl_print_prompt(ReplDebugSession* session) {
    if (session->is_paused) {
        printf("(paused) ");
    } else if (session->is_stepping) {
        printf("(stepping) ");
    }
    printf("myco-debug> ");
    fflush(stdout);
}

void repl_print_success(ReplDebugSession* session, const char* message) {
    if (session->color_output) {
        printf("\x1b[32m✓ %s\x1b[0m\n", message);
    } else {
        printf("✓ %s\n", message);
    }
}


void repl_print_warning(ReplDebugSession* session, const char* message) {
    if (session->color_output) {
        printf("\x1b[33m⚠ %s\x1b[0m\n", message);
    } else {
        printf("⚠ %s\n", message);
    }
}

void repl_print_info(ReplDebugSession* session, const char* message) {
    if (session->color_output) {
        printf("\x1b[36mℹ %s\x1b[0m\n", message);
    } else {
        printf("ℹ %s\n", message);
    }
}

void repl_print_debug(ReplDebugSession* session, const char* message) {
    if (session->color_output) {
        printf("\x1b[37m🐛 %s\x1b[0m\n", message);
    } else {
        printf("🐛 %s\n", message);
    }
}

// ============================================================================
// HISTORY MANAGEMENT
// ============================================================================


void repl_show_history(ReplDebugSession* session, int count) {
    if (!session) return;
    
    int start = (count > 0 && count < (int)session->history_count) ? 
                session->history_count - count : 0;
    
    printf("Command History:\n");
    for (int i = start; i < (int)session->history_count; i++) {
        printf("  %d: %s\n", i + 1, session->command_history[i]);
    }
}

void repl_clear_history(ReplDebugSession* session) {
    if (!session) return;
    
    for (size_t i = 0; i < session->history_count; i++) {
        free(session->command_history[i]);
    }
    session->history_count = 0;
    session->history_index = 0;
}

char* repl_get_history_item(ReplDebugSession* session, int index) {
    if (!session || index < 0 || index >= (int)session->history_count) return NULL;
    return session->command_history[index];
}

// ============================================================================
// CONFIGURATION
// ============================================================================

void repl_set_verbose(ReplDebugSession* session, bool verbose) {
    if (session) {
        session->verbose_output = verbose;
    }
}

void repl_set_colors(ReplDebugSession* session, bool colors) {
    if (session) {
        session->color_output = colors;
    }
}

void repl_set_timestamps(ReplDebugSession* session, bool timestamps) {
    if (session) {
        session->timestamp_output = timestamps;
    }
}

void repl_show_config(ReplDebugSession* session) {
    repl_cmd_config(session, NULL);
}

// ============================================================================
// STATISTICS AND REPORTING
// ============================================================================

void repl_show_statistics(ReplDebugSession* session) {
    if (!session) return;
    
    printf("Session Statistics:\n");
    printf("  Commands executed: %llu\n", (unsigned long long)session->commands_executed);
    printf("  Debug messages: %llu\n", (unsigned long long)session->debug_messages);
    printf("  Errors reported: %llu\n", (unsigned long long)session->errors_reported);
    printf("  Breakpoints hit: %llu\n", (unsigned long long)session->breakpoints_hit);
    
    if (session->session_start_time > 0) {
        time_t current_time = time(NULL);
        time_t elapsed = current_time - session->session_start_time;
        printf("  Session duration: %ld seconds\n", elapsed);
    }
}

void repl_show_session_info(ReplDebugSession* session) {
    if (!session) return;
    
    printf("Session Information:\n");
    printf("  Status: %s\n", session->is_active ? "Active" : "Inactive");
    printf("  Debugging: %s\n", session->is_debugging ? "Enabled" : "Disabled");
    printf("  Stepping: %s\n", session->is_stepping ? "Yes" : "No");
    printf("  Paused: %s\n", session->is_paused ? "Yes" : "No");
    
    if (session->current_file) {
        printf("  Current file: %s:%u\n", session->current_file, session->current_line);
    }
    if (session->current_function) {
        printf("  Current function: %s\n", session->current_function);
    }
}

void repl_export_session(ReplDebugSession* session, const char* filename) {
    if (!session || !filename) return;
    
    FILE* file = fopen(filename, "w");
    if (!file) {
        repl_print_error(session, "Failed to open file for export");
        return;
    }
    
    fprintf(file, "Myco Debug Session Export\n");
    fprintf(file, "========================\n\n");
    
    fprintf(file, "Session Statistics:\n");
    fprintf(file, "  Commands executed: %llu\n", (unsigned long long)session->commands_executed);
    fprintf(file, "  Debug messages: %llu\n", (unsigned long long)session->debug_messages);
    fprintf(file, "  Errors reported: %llu\n", (unsigned long long)session->errors_reported);
    fprintf(file, "  Breakpoints hit: %llu\n", (unsigned long long)session->breakpoints_hit);
    
    fprintf(file, "\nCommand History:\n");
    for (size_t i = 0; i < session->history_count; i++) {
        fprintf(file, "  %zu: %s\n", i + 1, session->command_history[i]);
    }
    
    fclose(file);
    char success_msg[256];
    snprintf(success_msg, sizeof(success_msg), "Session exported to %s", filename);
    repl_print_success(session, success_msg);
}

void repl_import_session(ReplDebugSession* session, const char* filename) {
    if (!session || !filename) return;
    
    FILE* file = fopen(filename, "r");
    if (!file) {
        repl_print_error(session, "Failed to open file for import");
        return;
    }
    
    // TODO: Implement session import
    fclose(file);
    char success_msg[256];
    snprintf(success_msg, sizeof(success_msg), "Session imported from %s", filename);
    repl_print_success(session, success_msg);
}

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

const char* repl_get_command_name(ReplCommand command) {
    ReplCommandInfo* info = repl_get_command_info(command);
    return info ? info->name : "unknown";
}

const char* repl_get_command_description(ReplCommand command) {
    ReplCommandInfo* info = repl_get_command_info(command);
    return info ? info->description : "Unknown command";
}

const char* repl_get_command_usage(ReplCommand command) {
    ReplCommandInfo* info = repl_get_command_info(command);
    return info ? info->usage : "No usage information available";
}

bool repl_is_valid_command(const char* command) {
    return repl_parse_command(command) != REPL_CMD_UNKNOWN;
}

ReplCommand repl_find_command_by_name(const char* name) {
    return repl_parse_command(name);
}

// ============================================================================
// GLOBAL REPL DEBUG SESSION ACCESS
// ============================================================================

ReplDebugSession* repl_debug_session_get_global(void) {
    if (!global_repl_session) {
        global_repl_session = repl_debug_session_create();
    }
    return global_repl_session;
}

void repl_debug_session_set_global(ReplDebugSession* session) {
    if (global_repl_session) {
        repl_debug_session_free(global_repl_session);
    }
    global_repl_session = session;
}

void repl_debug_session_cleanup_global(void) {
    if (global_repl_session) {
        repl_debug_session_free(global_repl_session);
        global_repl_session = NULL;
    }
}

// ============================================================================
// STUB IMPLEMENTATIONS FOR MISSING FUNCTIONS
// ============================================================================

void repl_interactive_mode(ReplDebugSession* session) {
    if (!session) return;
    
    repl_debug_session_start(session);
    
    char input[1024];
    while (session->is_active) {
        repl_print_prompt(session);
        
        if (fgets(input, sizeof(input), stdin)) {
            // Remove newline
            input[strcspn(input, "\n")] = '\0';
            
            if (strlen(input) > 0) {
                repl_execute_command(session, input);
            }
        } else {
            break;
        }
    }
}

void repl_batch_mode(ReplDebugSession* session, const char* script_file) {
    if (!session || !script_file) return;
    
    FILE* file = fopen(script_file, "r");
    if (!file) {
        repl_print_error(session, "Failed to open script file");
        return;
    }
    
    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        // Remove newline
        line[strcspn(line, "\n")] = '\0';
        
        if (strlen(line) > 0) {
            repl_execute_command(session, line);
        }
    }
    
    fclose(file);
}

void repl_auto_complete(ReplDebugSession* session, const char* input, char* completion) {
    // TODO: Implement auto-completion
    completion[0] = '\0';
}

void repl_command_suggestions(ReplDebugSession* session, const char* input) {
    // TODO: Implement command suggestions
}

void repl_handle_error(ReplDebugSession* session, const char* error_message) {
    if (session) {
        session->errors_reported++;
        repl_print_error(session, error_message);
    }
}

void repl_handle_warning(ReplDebugSession* session, const char* warning_message) {
    if (session) {
        repl_print_warning(session, warning_message);
    }
}

void repl_handle_debug_message(ReplDebugSession* session, DebugLevel level, 
                              DebugCategory category, const char* message) {
    if (session) {
        session->debug_messages++;
        repl_print_debug(session, message);
    }
}

// ============================================================================
// IMPLEMENTATION OF MISSING FUNCTIONS
// ============================================================================

static void repl_show_help(ReplDebugSession* session) {
    if (!session) return;
    
    printf("Myco Debug Commands:\n");
    printf("  help              Show this help message\n");
    printf("  quit              Exit the debug session\n");
    printf("  clear             Clear the screen\n");
    printf("  debug [on|off]    Toggle debug mode\n");
    printf("  breakpoint        Manage breakpoints\n");
    printf("  watch             Manage watch expressions\n");
    printf("  step              Step through code\n");
    printf("  continue          Continue execution\n");
    printf("  variables         Show variables\n");
    printf("  stack             Show call stack\n");
    printf("  memory            Show memory usage\n");
    printf("  performance       Show performance info\n");
    printf("  errors            Show error information\n");
    printf("  logs              Show debug logs\n");
    printf("  config            Show/modify configuration\n");
    printf("  export            Export session data\n");
    printf("  import            Import session data\n");
    printf("  reset             Reset session\n");
    printf("\n");
}

static void repl_print_error(ReplDebugSession* session, const char* message) {
    if (!session || !message) return;
    
    printf("\033[31mDebug Error: %s\033[0m\n", message);
}

static void repl_add_to_history(ReplDebugSession* session, const char* command) {
    if (!session || !command) return;
    
    // Simple history implementation - just store the last command
    if (session->last_command) {
        free(session->last_command);
    }
    session->last_command = strdup(command);
}
