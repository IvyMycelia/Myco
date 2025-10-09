#include "debug_system.h"
#include "enhanced_error_system.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>

// ============================================================================
// GLOBAL DEBUG SYSTEM INSTANCE
// ============================================================================

static DebugSystem* global_debug_system = NULL;

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

static uint64_t get_current_timestamp(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

static const char* get_level_color(DebugLevel level) {
    switch (level) {
        case DEBUG_LEVEL_NONE:     return "";
        case DEBUG_LEVEL_ERROR:    return "\x1b[31m"; // Red
        case DEBUG_LEVEL_WARNING:  return "\x1b[33m"; // Yellow
        case DEBUG_LEVEL_INFO:     return "\x1b[36m"; // Cyan
        case DEBUG_LEVEL_DEBUG:    return "\x1b[32m"; // Green
        case DEBUG_LEVEL_TRACE:    return "\x1b[35m"; // Magenta
        case DEBUG_LEVEL_VERBOSE:  return "\x1b[37m"; // White
        default: return "";
    }
}

static const char* get_category_color(DebugCategory category) {
    switch (category) {
        case DEBUG_CATEGORY_PARSER:      return "\x1b[34m"; // Blue
        case DEBUG_CATEGORY_LEXER:       return "\x1b[36m"; // Cyan
        case DEBUG_CATEGORY_INTERPRETER: return "\x1b[32m"; // Green
        case DEBUG_CATEGORY_MEMORY:      return "\x1b[35m"; // Magenta
        case DEBUG_CATEGORY_TYPE_SYSTEM: return "\x1b[33m"; // Yellow
        case DEBUG_CATEGORY_OPTIMIZER:   return "\x1b[37m"; // White
        case DEBUG_CATEGORY_JIT:         return "\x1b[31m"; // Red
        case DEBUG_CATEGORY_GC:          return "\x1b[90m"; // Dark gray
        case DEBUG_CATEGORY_IO:          return "\x1b[94m"; // Light blue
        case DEBUG_CATEGORY_NETWORK:     return "\x1b[95m"; // Light magenta
        case DEBUG_CATEGORY_USER:        return "\x1b[93m"; // Light yellow
        default: return "";
    }
}

// ============================================================================
// SYSTEM INITIALIZATION AND CLEANUP
// ============================================================================

DebugSystem* debug_system_create(void) {
    DebugSystem* system = malloc(sizeof(DebugSystem));
    if (!system) return NULL;
    
    memset(system, 0, sizeof(DebugSystem));
    
    // Initialize configuration with defaults
    system->config.level = DEBUG_LEVEL_INFO;
    system->config.enable_colors = true;
    system->config.enable_timestamps = true;
    system->config.enable_file_info = true;
    system->config.enable_function_info = true;
    system->config.enable_thread_info = false;
    system->config.enable_memory_info = false;
    system->config.enable_performance_info = false;
    system->config.log_file = NULL;
    system->config.output_file = NULL;
    
    // Enable all categories by default
    for (int i = 0; i < 11; i++) {
        system->config.category_enabled[i] = true;
        system->config.category_levels[i] = DEBUG_LEVEL_INFO;
    }
    
    system->config.track_execution_time = false;
    system->config.track_memory_usage = false;
    system->config.track_function_calls = false;
    system->config.enable_breakpoints = true;
    system->config.enable_watch_expressions = true;
    system->config.enable_call_tracing = true;
    system->config.enable_variable_inspection = true;
    system->config.enable_memory_inspection = true;
    
    // Initialize error system
    system->error_system = enhanced_error_system_get_global();
    
    // Initialize breakpoints
    system->breakpoint_capacity = 10;
    system->breakpoints = malloc(sizeof(Breakpoint) * system->breakpoint_capacity);
    if (!system->breakpoints) {
        free(system);
        return NULL;
    }
    system->breakpoint_count = 0;
    system->next_breakpoint_id = 1;
    
    // Initialize watch expressions
    system->watch_capacity = 10;
    system->watch_expressions = malloc(sizeof(WatchExpression) * system->watch_capacity);
    if (!system->watch_expressions) {
        free(system->breakpoints);
        free(system);
        return NULL;
    }
    system->watch_count = 0;
    system->next_watch_id = 1;
    
    // Initialize debug context
    system->context.current_file = NULL;
    system->context.current_line = 0;
    system->context.current_function = NULL;
    system->context.is_running = false;
    system->context.is_paused = false;
    system->context.is_stepping = false;
    system->context.step_over = false;
    system->context.step_into = false;
    system->context.step_out = false;
    
    system->context.call_stack.functions = NULL;
    system->context.call_stack.files = NULL;
    system->context.call_stack.lines = NULL;
    system->context.call_stack.depth = 0;
    system->context.call_stack.capacity = 0;
    
    system->context.variables.names = NULL;
    system->context.variables.values = NULL;
    system->context.variables.types = NULL;
    system->context.variables.count = 0;
    system->context.variables.capacity = 0;
    
    // Initialize statistics
    system->total_debug_messages = 0;
    memset(system->category_counts, 0, sizeof(system->category_counts));
    memset(system->level_counts, 0, sizeof(system->level_counts));
    
    // Initialize performance tracking
    system->start_time = get_current_timestamp();
    system->last_message_time = 0;
    system->peak_memory_usage = 0;
    system->current_memory_usage = 0;
    
    return system;
}

void debug_system_free(DebugSystem* system) {
    if (!system) return;
    
    // Free breakpoints
    for (size_t i = 0; i < system->breakpoint_count; i++) {
        free(system->breakpoints[i].file_name);
        free(system->breakpoints[i].function_name);
        free(system->breakpoints[i].condition);
        free(system->breakpoints[i].variable_name);
    }
    free(system->breakpoints);
    
    // Free watch expressions
    for (size_t i = 0; i < system->watch_count; i++) {
        free(system->watch_expressions[i].expression);
        free(system->watch_expressions[i].last_value);
    }
    free(system->watch_expressions);
    
    // Free debug context
    free(system->context.current_file);
    free(system->context.current_function);
    
    // Free call stack
    for (size_t i = 0; i < system->context.call_stack.depth; i++) {
        free(system->context.call_stack.functions[i]);
        free(system->context.call_stack.files[i]);
    }
    free(system->context.call_stack.functions);
    free(system->context.call_stack.files);
    free(system->context.call_stack.lines);
    
    // Free variables
    for (size_t i = 0; i < system->context.variables.count; i++) {
        free(system->context.variables.names[i]);
        free(system->context.variables.values[i]);
        free(system->context.variables.types[i]);
    }
    free(system->context.variables.names);
    free(system->context.variables.values);
    free(system->context.variables.types);
    
    // Free configuration
    free(system->config.log_file);
    free(system->config.output_file);
    
    free(system);
}

void debug_system_configure(DebugSystem* system, const DebugSystemConfig* config) {
    if (!system || !config) return;
    
    system->config = *config;
    
    // Copy strings
    free(system->config.log_file);
    system->config.log_file = config->log_file ? strdup(config->log_file) : NULL;
    
    free(system->config.output_file);
    system->config.output_file = config->output_file ? strdup(config->output_file) : NULL;
}

// ============================================================================
// DEBUG MESSAGE LOGGING
// ============================================================================

void debug_log(DebugSystem* system, DebugLevel level, DebugCategory category,
               const char* file, uint32_t line, const char* function,
               const char* format, ...) {
    if (!system || !debug_is_enabled(system, level, category)) return;
    
    // Check if this category is enabled
    if (!system->config.category_enabled[category]) return;
    if (level > system->config.category_levels[category]) return;
    
    // Format the message
    char message[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);
    
    // Update statistics
    system->total_debug_messages++;
    system->level_counts[level]++;
    system->category_counts[category]++;
    system->last_message_time = get_current_timestamp();
    
    // Print the debug message
    if (system->config.enable_colors) {
        const char* level_color = get_level_color(level);
        const char* category_color = get_category_color(category);
        const char* reset_color = "\x1b[0m";
        
        printf("%s[%s]%s %s[%s]%s ", 
               level_color, debug_get_level_name(level), reset_color,
               category_color, debug_get_category_name(category), reset_color);
    } else {
        printf("[%s] [%s] ", 
               debug_get_level_name(level), 
               debug_get_category_name(category));
    }
    
    if (system->config.enable_timestamps) {
        uint64_t current_time = get_current_timestamp();
        uint64_t elapsed = current_time - system->start_time;
        printf("[%llu] ", (unsigned long long)elapsed);
    }
    
    if (system->config.enable_file_info && file) {
        printf("%s:", file);
        if (line > 0) {
            printf("%u", line);
        }
        printf(" ");
    }
    
    if (system->config.enable_function_info && function) {
        printf("%s() ", function);
    }
    
    printf("%s\n", message);
    
    // Log to file if configured
    if (system->config.log_file) {
        FILE* log_file = fopen(system->config.log_file, "a");
        if (log_file) {
            fprintf(log_file, "[%s] [%s] %s:%u %s() %s\n",
                    debug_get_level_name(level),
                    debug_get_category_name(category),
                    file ? file : "unknown",
                    line,
                    function ? function : "unknown",
                    message);
            fclose(log_file);
        }
    }
}

void debug_log_simple(DebugSystem* system, DebugLevel level, const char* message) {
    debug_log(system, level, DEBUG_CATEGORY_USER, NULL, 0, NULL, "%s", message);
}

void debug_log_with_context(DebugSystem* system, DebugLevel level, DebugCategory category,
                           const char* file, uint32_t line, const char* function,
                           const char* message, const char* context) {
    if (!system || !debug_is_enabled(system, level, category)) return;
    
    char full_message[2048];
    if (context) {
        snprintf(full_message, sizeof(full_message), "%s (Context: %s)", message, context);
    } else {
        strncpy(full_message, message, sizeof(full_message) - 1);
        full_message[sizeof(full_message) - 1] = '\0';
    }
    
    debug_log(system, level, category, file, line, function, "%s", full_message);
}

// ============================================================================
// DEBUG LEVEL AND CATEGORY MANAGEMENT
// ============================================================================

void debug_set_level(DebugSystem* system, DebugLevel level) {
    if (system) {
        system->config.level = level;
    }
}

void debug_set_category_enabled(DebugSystem* system, DebugCategory category, bool enabled) {
    if (system && category < 11) {
        system->config.category_enabled[category] = enabled;
    }
}

void debug_set_category_level(DebugSystem* system, DebugCategory category, DebugLevel level) {
    if (system && category < 11) {
        system->config.category_levels[category] = level;
    }
}

bool debug_is_enabled(DebugSystem* system, DebugLevel level, DebugCategory category) {
    if (!system) return false;
    if (level > system->config.level) return false;
    if (category >= 11) return false;
    if (!system->config.category_enabled[category]) return false;
    if (level > system->config.category_levels[category]) return false;
    return true;
}

// ============================================================================
// BREAKPOINT MANAGEMENT
// ============================================================================

uint32_t debug_add_breakpoint(DebugSystem* system, BreakpointType type, const char* file,
                             uint32_t line, const char* function, const char* condition) {
    if (!system) return 0;
    
    // Reallocate if needed
    if (system->breakpoint_count >= system->breakpoint_capacity) {
        system->breakpoint_capacity *= 2;
        system->breakpoints = realloc(system->breakpoints, sizeof(Breakpoint) * system->breakpoint_capacity);
        if (!system->breakpoints) return 0;
    }
    
    Breakpoint* bp = &system->breakpoints[system->breakpoint_count];
    bp->id = system->next_breakpoint_id++;
    bp->type = type;
    bp->file_name = file ? strdup(file) : NULL;
    bp->line_number = line;
    bp->function_name = function ? strdup(function) : NULL;
    bp->condition = condition ? strdup(condition) : NULL;
    bp->variable_name = NULL;
    bp->enabled = true;
    bp->hit_count = 0;
    bp->max_hits = 0; // 0 means unlimited
    
    system->breakpoint_count++;
    
    return bp->id;
}

void debug_remove_breakpoint(DebugSystem* system, uint32_t id) {
    if (!system) return;
    
    for (size_t i = 0; i < system->breakpoint_count; i++) {
        if (system->breakpoints[i].id == id) {
            // Free memory
            free(system->breakpoints[i].file_name);
            free(system->breakpoints[i].function_name);
            free(system->breakpoints[i].condition);
            free(system->breakpoints[i].variable_name);
            
            // Move remaining breakpoints
            for (size_t j = i; j < system->breakpoint_count - 1; j++) {
                system->breakpoints[j] = system->breakpoints[j + 1];
            }
            system->breakpoint_count--;
            break;
        }
    }
}

void debug_enable_breakpoint(DebugSystem* system, uint32_t id, bool enabled) {
    if (!system) return;
    
    for (size_t i = 0; i < system->breakpoint_count; i++) {
        if (system->breakpoints[i].id == id) {
            system->breakpoints[i].enabled = enabled;
            break;
        }
    }
}

Breakpoint* debug_find_breakpoint(DebugSystem* system, uint32_t id) {
    if (!system) return NULL;
    
    for (size_t i = 0; i < system->breakpoint_count; i++) {
        if (system->breakpoints[i].id == id) {
            return &system->breakpoints[i];
        }
    }
    return NULL;
}

bool debug_check_breakpoint(DebugSystem* system, const char* file, uint32_t line,
                           const char* function) {
    if (!system) return false;
    
    for (size_t i = 0; i < system->breakpoint_count; i++) {
        Breakpoint* bp = &system->breakpoints[i];
        if (!bp->enabled) continue;
        
        bool matches = false;
        switch (bp->type) {
            case BREAKPOINT_LINE:
                matches = (bp->file_name && file && strcmp(bp->file_name, file) == 0 && 
                          bp->line_number == line);
                break;
            case BREAKPOINT_FUNCTION:
                matches = (bp->function_name && function && 
                          strcmp(bp->function_name, function) == 0);
                break;
            case BREAKPOINT_CONDITION:
                // TODO: Evaluate condition
                matches = false;
                break;
            default:
                break;
        }
        
        if (matches) {
            bp->hit_count++;
            if (bp->max_hits > 0 && bp->hit_count >= bp->max_hits) {
                bp->enabled = false;
            }
            return true;
        }
    }
    return false;
}

// ============================================================================
// WATCH EXPRESSION MANAGEMENT
// ============================================================================

uint32_t debug_add_watch(DebugSystem* system, const char* expression) {
    if (!system || !expression) return 0;
    
    // Reallocate if needed
    if (system->watch_count >= system->watch_capacity) {
        system->watch_capacity *= 2;
        system->watch_expressions = realloc(system->watch_expressions, 
                                          sizeof(WatchExpression) * system->watch_capacity);
        if (!system->watch_expressions) return 0;
    }
    
    WatchExpression* watch = &system->watch_expressions[system->watch_count];
    watch->id = system->next_watch_id++;
    watch->expression = strdup(expression);
    watch->last_value = NULL;
    watch->enabled = true;
    watch->hit_count = 0;
    
    system->watch_count++;
    
    return watch->id;
}

void debug_remove_watch(DebugSystem* system, uint32_t id) {
    if (!system) return;
    
    for (size_t i = 0; i < system->watch_count; i++) {
        if (system->watch_expressions[i].id == id) {
            free(system->watch_expressions[i].expression);
            free(system->watch_expressions[i].last_value);
            
            // Move remaining watches
            for (size_t j = i; j < system->watch_count - 1; j++) {
                system->watch_expressions[j] = system->watch_expressions[j + 1];
            }
            system->watch_count--;
            break;
        }
    }
}

void debug_enable_watch(DebugSystem* system, uint32_t id, bool enabled) {
    if (!system) return;
    
    for (size_t i = 0; i < system->watch_count; i++) {
        if (system->watch_expressions[i].id == id) {
            system->watch_expressions[i].enabled = enabled;
            break;
        }
    }
}

WatchExpression* debug_find_watch(DebugSystem* system, uint32_t id) {
    if (!system) return NULL;
    
    for (size_t i = 0; i < system->watch_count; i++) {
        if (system->watch_expressions[i].id == id) {
            return &system->watch_expressions[i];
        }
    }
    return NULL;
}

void debug_evaluate_watches(DebugSystem* system) {
    if (!system) return;
    
    for (size_t i = 0; i < system->watch_count; i++) {
        WatchExpression* watch = &system->watch_expressions[i];
        if (!watch->enabled) continue;
        
        // TODO: Evaluate expression and update last_value
        // This would require integration with the interpreter
        watch->hit_count++;
    }
}

// ============================================================================
// DEBUG CONTEXT MANAGEMENT
// ============================================================================

void debug_set_context(DebugSystem* system, const char* file, uint32_t line, const char* function) {
    if (!system) return;
    
    free(system->context.current_file);
    free(system->context.current_function);
    
    system->context.current_file = file ? strdup(file) : NULL;
    system->context.current_line = line;
    system->context.current_function = function ? strdup(function) : NULL;
}

void debug_push_stack_frame(DebugSystem* system, const char* function, const char* file, uint32_t line) {
    if (!system) return;
    
    // Reallocate if needed
    if (system->context.call_stack.depth >= system->context.call_stack.capacity) {
        system->context.call_stack.capacity = system->context.call_stack.capacity ? 
            system->context.call_stack.capacity * 2 : 10;
        system->context.call_stack.functions = realloc(system->context.call_stack.functions,
            sizeof(char*) * system->context.call_stack.capacity);
        system->context.call_stack.files = realloc(system->context.call_stack.files,
            sizeof(char*) * system->context.call_stack.capacity);
        system->context.call_stack.lines = realloc(system->context.call_stack.lines,
            sizeof(uint32_t) * system->context.call_stack.capacity);
    }
    
    size_t index = system->context.call_stack.depth++;
    system->context.call_stack.functions[index] = function ? strdup(function) : NULL;
    system->context.call_stack.files[index] = file ? strdup(file) : NULL;
    system->context.call_stack.lines[index] = line;
}

void debug_pop_stack_frame(DebugSystem* system) {
    if (!system || system->context.call_stack.depth == 0) return;
    
    size_t index = --system->context.call_stack.depth;
    free(system->context.call_stack.functions[index]);
    free(system->context.call_stack.files[index]);
}

void debug_add_variable(DebugSystem* system, const char* name, const char* value, const char* type) {
    if (!system || !name) return;
    
    // Reallocate if needed
    if (system->context.variables.count >= system->context.variables.capacity) {
        system->context.variables.capacity = system->context.variables.capacity ? 
            system->context.variables.capacity * 2 : 10;
        system->context.variables.names = realloc(system->context.variables.names,
            sizeof(char*) * system->context.variables.capacity);
        system->context.variables.values = realloc(system->context.variables.values,
            sizeof(char*) * system->context.variables.capacity);
        system->context.variables.types = realloc(system->context.variables.types,
            sizeof(char*) * system->context.variables.capacity);
    }
    
    size_t index = system->context.variables.count++;
    system->context.variables.names[index] = strdup(name);
    system->context.variables.values[index] = value ? strdup(value) : NULL;
    system->context.variables.types[index] = type ? strdup(type) : NULL;
}

void debug_clear_variables(DebugSystem* system) {
    if (!system) return;
    
    for (size_t i = 0; i < system->context.variables.count; i++) {
        free(system->context.variables.names[i]);
        free(system->context.variables.values[i]);
        free(system->context.variables.types[i]);
    }
    system->context.variables.count = 0;
}

// ============================================================================
// DEBUGGING CONTROL
// ============================================================================

void debug_pause(DebugSystem* system) {
    if (system) {
        system->context.is_paused = true;
        system->context.is_running = false;
    }
}

void debug_resume(DebugSystem* system) {
    if (system) {
        system->context.is_paused = false;
        system->context.is_running = true;
    }
}

void debug_step_over(DebugSystem* system) {
    if (system) {
        system->context.is_stepping = true;
        system->context.step_over = true;
        system->context.step_into = false;
        system->context.step_out = false;
    }
}

void debug_step_into(DebugSystem* system) {
    if (system) {
        system->context.is_stepping = true;
        system->context.step_over = false;
        system->context.step_into = true;
        system->context.step_out = false;
    }
}

void debug_step_out(DebugSystem* system) {
    if (system) {
        system->context.is_stepping = true;
        system->context.step_over = false;
        system->context.step_into = false;
        system->context.step_out = true;
    }
}

bool debug_is_paused(DebugSystem* system) {
    return system ? system->context.is_paused : false;
}

// ============================================================================
// DEBUG INFORMATION RETRIEVAL
// ============================================================================

void debug_print_call_stack(DebugSystem* system) {
    if (!system) return;
    
    printf("Call Stack:\n");
    for (size_t i = 0; i < system->context.call_stack.depth; i++) {
        printf("  %zu. %s", i + 1, 
               system->context.call_stack.functions[i] ? 
               system->context.call_stack.functions[i] : "<unknown>");
        
        if (system->context.call_stack.files[i]) {
            printf(" at %s:%u", 
                   system->context.call_stack.files[i], 
                   system->context.call_stack.lines[i]);
        }
        printf("\n");
    }
}

void debug_print_variables(DebugSystem* system) {
    if (!system) return;
    
    printf("Variables:\n");
    for (size_t i = 0; i < system->context.variables.count; i++) {
        printf("  %s: %s (%s)\n",
               system->context.variables.names[i],
               system->context.variables.values[i] ? 
               system->context.variables.values[i] : "null",
               system->context.variables.types[i] ? 
               system->context.variables.types[i] : "unknown");
    }
}

void debug_print_breakpoints(DebugSystem* system) {
    if (!system) return;
    
    printf("Breakpoints:\n");
    for (size_t i = 0; i < system->breakpoint_count; i++) {
        Breakpoint* bp = &system->breakpoints[i];
        printf("  %u. %s %s:%u %s (hits: %u)\n",
               bp->id,
               bp->enabled ? "enabled" : "disabled",
               bp->file_name ? bp->file_name : "any",
               bp->line_number,
               bp->function_name ? bp->function_name : "any",
               bp->hit_count);
    }
}

void debug_print_watches(DebugSystem* system) {
    if (!system) return;
    
    printf("Watch Expressions:\n");
    for (size_t i = 0; i < system->watch_count; i++) {
        WatchExpression* watch = &system->watch_expressions[i];
        printf("  %u. %s = %s (hits: %u)\n",
               watch->id,
               watch->expression,
               watch->last_value ? watch->last_value : "not evaluated",
               watch->hit_count);
    }
}

void debug_print_statistics(DebugSystem* system) {
    if (!system) return;
    
    printf("Debug Statistics:\n");
    printf("  Total messages: %llu\n", (unsigned long long)system->total_debug_messages);
    printf("  Peak memory usage: %zu bytes\n", system->peak_memory_usage);
    printf("  Current memory usage: %zu bytes\n", system->current_memory_usage);
    
    printf("  By level:\n");
    for (int i = 0; i < 7; i++) {
        if (system->level_counts[i] > 0) {
            printf("    %s: %llu\n", debug_get_level_name(i), 
                   (unsigned long long)system->level_counts[i]);
        }
    }
    
    printf("  By category:\n");
    for (int i = 0; i < 11; i++) {
        if (system->category_counts[i] > 0) {
            printf("    %s: %llu\n", debug_get_category_name(i), 
                   (unsigned long long)system->category_counts[i]);
        }
    }
}

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

const char* debug_get_level_name(DebugLevel level) {
    switch (level) {
        case DEBUG_LEVEL_NONE:     return "NONE";
        case DEBUG_LEVEL_ERROR:    return "ERROR";
        case DEBUG_LEVEL_WARNING:  return "WARNING";
        case DEBUG_LEVEL_INFO:     return "INFO";
        case DEBUG_LEVEL_DEBUG:    return "DEBUG";
        case DEBUG_LEVEL_TRACE:    return "TRACE";
        case DEBUG_LEVEL_VERBOSE:  return "VERBOSE";
        default: return "UNKNOWN";
    }
}

const char* debug_get_category_name(DebugCategory category) {
    switch (category) {
        case DEBUG_CATEGORY_PARSER:      return "PARSER";
        case DEBUG_CATEGORY_LEXER:       return "LEXER";
        case DEBUG_CATEGORY_INTERPRETER: return "INTERPRETER";
        case DEBUG_CATEGORY_MEMORY:      return "MEMORY";
        case DEBUG_CATEGORY_TYPE_SYSTEM: return "TYPE_SYSTEM";
        case DEBUG_CATEGORY_OPTIMIZER:   return "OPTIMIZER";
        case DEBUG_CATEGORY_JIT:         return "JIT";
        case DEBUG_CATEGORY_GC:          return "GC";
        case DEBUG_CATEGORY_IO:          return "IO";
        case DEBUG_CATEGORY_NETWORK:     return "NETWORK";
        case DEBUG_CATEGORY_USER:        return "USER";
        default: return "UNKNOWN";
    }
}

const char* debug_get_level_color(DebugLevel level) {
    return get_level_color(level);
}

const char* debug_get_category_color(DebugCategory category) {
    return get_category_color(category);
}

// ============================================================================
// GLOBAL DEBUG SYSTEM ACCESS
// ============================================================================

DebugSystem* debug_system_get_global(void) {
    if (!global_debug_system) {
        global_debug_system = debug_system_create();
    }
    return global_debug_system;
}

void debug_system_set_global(DebugSystem* system) {
    if (global_debug_system) {
        debug_system_free(global_debug_system);
    }
    global_debug_system = system;
}

void debug_system_cleanup_global(void) {
    if (global_debug_system) {
        debug_system_free(global_debug_system);
        global_debug_system = NULL;
    }
}

// ============================================================================
// STUB IMPLEMENTATIONS FOR MISSING FUNCTIONS
// ============================================================================

void debug_memory_track(DebugSystem* system, size_t size) {
    if (system) {
        system->current_memory_usage += size;
        if (system->current_memory_usage > system->peak_memory_usage) {
            system->peak_memory_usage = system->current_memory_usage;
        }
    }
}

void debug_memory_untrack(DebugSystem* system, size_t size) {
    if (system && system->current_memory_usage >= size) {
        system->current_memory_usage -= size;
    }
}

void debug_print_memory_usage(DebugSystem* system) {
    if (system) {
        printf("Memory Usage: %zu bytes (peak: %zu bytes)\n",
               system->current_memory_usage, system->peak_memory_usage);
    }
}

void debug_detect_memory_leaks(DebugSystem* system) {
    if (system) {
        printf("Memory leak detection not implemented yet\n");
    }
}

void debug_start_timer(DebugSystem* system, const char* name) {
    // TODO: Implement timer functionality
}

void debug_end_timer(DebugSystem* system, const char* name) {
    // TODO: Implement timer functionality
}

void debug_print_performance(DebugSystem* system) {
    if (system) {
        printf("Performance tracking not implemented yet\n");
    }
}

void debug_print_header(DebugSystem* system, DebugLevel level, DebugCategory category,
                       const char* file, uint32_t line, const char* function) {
    // This is handled in debug_log
}

void debug_print_footer(DebugSystem* system) {
    // This is handled in debug_log
}

char* debug_format_message(DebugSystem* system, DebugLevel level, DebugCategory category,
                          const char* file, uint32_t line, const char* function,
                          const char* message) {
    // TODO: Implement message formatting
    return strdup(message);
}
