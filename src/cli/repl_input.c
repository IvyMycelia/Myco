#include "repl_input.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include "../../include/utils/shared_utilities.h"

// Custom REPL input implementation to replace libreadline dependency
// This provides a lightweight, dependency-free REPL input system

// Global terminal state
static struct termios g_original_termios;
static bool g_terminal_configured = false;

// Configure terminal for raw input
static int configure_terminal(void) {
    if (g_terminal_configured) return 1;
    
    if (tcgetattr(STDIN_FILENO, &g_original_termios) < 0) {
        return 0;
    }
    
    struct termios raw = g_original_termios;
    raw.c_lflag &= ~(ECHO | ICANON | ISIG);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) < 0) {
        return 0;
    }
    
    g_terminal_configured = true;
    return 1;
}

// Restore terminal to original state
static void restore_terminal(void) {
    if (g_terminal_configured) {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &g_original_termios);
        g_terminal_configured = false;
    }
}

// Get terminal size
static void get_terminal_size(int* width, int* height) {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0) {
        *width = ws.ws_col;
        *height = ws.ws_row;
    } else {
        *width = 80;
        *height = 24;
    }
}

// Create REPL input state
ReplInput* repl_input_create(void) {
    ReplInput* input = shared_malloc_safe(sizeof(ReplInput), "repl_input", "repl_input_create", 0);
    if (!input) return NULL;
    
    input->buffer = shared_malloc_safe(1024, "repl_input", "repl_input_create", 0);
    if (!input->buffer) {
        shared_free_safe(input, "repl_input", "repl_input_create", 0);
        return NULL;
    }
    
    input->capacity = 1024;
    input->length = 0;
    input->cursor_pos = 0;
    input->history = NULL;
    input->history_count = 0;
    input->history_capacity = 0;
    input->history_index = 0;
    
    return input;
}

// Free REPL input state
void repl_input_free(ReplInput* input) {
    if (!input) return;
    
    if (input->buffer) {
        shared_free_safe(input->buffer, "repl_input", "repl_input_free", 0);
    }
    
    if (input->history) {
        for (size_t i = 0; i < input->history_count; i++) {
            if (input->history[i]) {
                shared_free_safe(input->history[i], "repl_input", "repl_input_free", 0);
            }
        }
        shared_free_safe(input->history, "repl_input", "repl_input_free", 0);
    }
    
    shared_free_safe(input, "repl_input", "repl_input_free", 0);
}

// Add string to history
void repl_input_add_history(ReplInput* input, const char* line) {
    if (!input || !line || strlen(line) == 0) return;
    
    // Expand history array if needed
    if (input->history_count >= input->history_capacity) {
        size_t new_capacity = input->history_capacity == 0 ? 16 : input->history_capacity * 2;
        input->history = shared_realloc_safe(input->history, 
            new_capacity * sizeof(char*), "repl_input", "repl_input_add_history", 0);
        if (!input->history) return;
        input->history_capacity = new_capacity;
    }
    
    // Add to history
    input->history[input->history_count] = shared_strdup(line);
    if (input->history[input->history_count]) {
        input->history_count++;
        input->history_index = input->history_count;
    }
}

// Insert character at cursor position
static int insert_char(ReplInput* input, char c) {
    if (input->length >= input->capacity - 1) {
        // Expand buffer
        size_t new_capacity = input->capacity * 2;
        char* new_buffer = shared_realloc_safe(input->buffer, new_capacity, 
            "repl_input", "insert_char", 0);
        if (!new_buffer) return 0;
        input->buffer = new_buffer;
        input->capacity = new_capacity;
    }
    
    // Shift characters to make room
    for (size_t i = input->length; i > input->cursor_pos; i--) {
        input->buffer[i] = input->buffer[i - 1];
    }
    
    input->buffer[input->cursor_pos] = c;
    input->cursor_pos++;
    input->length++;
    input->buffer[input->length] = '\0';
    
    return 1;
}

// Delete character at cursor position
static void delete_char(ReplInput* input) {
    if (input->cursor_pos >= input->length) return;
    
    // Shift characters left
    for (size_t i = input->cursor_pos; i < input->length - 1; i++) {
        input->buffer[i] = input->buffer[i + 1];
    }
    
    input->length--;
    input->buffer[input->length] = '\0';
}

// Move cursor left
static void move_cursor_left(ReplInput* input) {
    if (input->cursor_pos > 0) {
        input->cursor_pos--;
    }
}

// Move cursor right
static void move_cursor_right(ReplInput* input) {
    if (input->cursor_pos < input->length) {
        input->cursor_pos++;
    }
}

// Move to beginning of line
static void move_cursor_home(ReplInput* input) {
    input->cursor_pos = 0;
}

// Move to end of line
static void move_cursor_end(ReplInput* input) {
    input->cursor_pos = input->length;
}

// Clear line and redraw
static void redraw_line(ReplInput* input, const char* prompt) {
    // Move to beginning of line
    printf("\r");
    
    // Clear line
    int width, height;
    get_terminal_size(&width, &height);
    for (int i = 0; i < width; i++) {
        printf(" ");
    }
    
    // Move back to beginning and redraw
    printf("\r%s%s", prompt, input->buffer);
    
    // Position cursor
    int cursor_col = strlen(prompt) + input->cursor_pos;
    printf("\r\033[%dC", cursor_col);
    fflush(stdout);
}

// Handle special keys
static int handle_special_key(ReplInput* input, const char* prompt) {
    char c;
    if (read(STDIN_FILENO, &c, 1) != 1) return 0;
    
    switch (c) {
        case 'A': // Up arrow
            if (input->history_index > 0) {
                input->history_index--;
                if (input->history[input->history_index]) {
                    strcpy(input->buffer, input->history[input->history_index]);
                    input->length = strlen(input->buffer);
                    input->cursor_pos = input->length;
                    redraw_line(input, prompt);
                }
            }
            break;
            
        case 'B': // Down arrow
            if (input->history_index < input->history_count) {
                input->history_index++;
                if (input->history_index < input->history_count && 
                    input->history[input->history_index]) {
                    strcpy(input->buffer, input->history[input->history_index]);
                    input->length = strlen(input->buffer);
                    input->cursor_pos = input->length;
                } else {
                    input->buffer[0] = '\0';
                    input->length = 0;
                    input->cursor_pos = 0;
                }
                redraw_line(input, prompt);
            }
            break;
            
        case 'C': // Right arrow
            move_cursor_right(input);
            break;
            
        case 'D': // Left arrow
            move_cursor_left(input);
            break;
            
        case 'H': // Home
            move_cursor_home(input);
            break;
            
        case 'F': // End
            move_cursor_end(input);
            break;
    }
    
    return 1;
}

// Read a line of input
char* repl_input_read_line(ReplInput* input, const char* prompt) {
    if (!input || !prompt) return NULL;
    
    // Configure terminal for raw input
    if (!configure_terminal()) {
        // Fallback to simple input
        printf("%s", prompt);
        fflush(stdout);
        
        char buffer[1024];
        if (fgets(buffer, sizeof(buffer), stdin)) {
            size_t len = strlen(buffer);
            if (len > 0 && buffer[len-1] == '\n') {
                buffer[len-1] = '\0';
            }
            return shared_strdup(buffer);
        }
        return NULL;
    }
    
    // Reset input state
    input->buffer[0] = '\0';
    input->length = 0;
    input->cursor_pos = 0;
    input->history_index = input->history_count;
    
    printf("%s", prompt);
    fflush(stdout);
    
    while (1) {
        char c;
        if (read(STDIN_FILENO, &c, 1) != 1) {
            restore_terminal();
            return NULL;
        }
        
        if (c == '\n' || c == '\r') {
            // Enter pressed
            restore_terminal();
            printf("\n");
            
            if (input->length > 0) {
                repl_input_add_history(input, input->buffer);
                return shared_strdup(input->buffer);
            }
            return NULL;
        }
        
        if (c == 27) { // Escape sequence
            if (read(STDIN_FILENO, &c, 1) == 1 && c == '[') {
                handle_special_key(input, prompt);
            }
            continue;
        }
        
        if (c == 127 || c == '\b') { // Backspace
            if (input->cursor_pos > 0) {
                input->cursor_pos--;
                delete_char(input);
                redraw_line(input, prompt);
            }
            continue;
        }
        
        if (c == 3) { // Ctrl+C
            restore_terminal();
            return NULL;
        }
        
        if (c >= 32 && c <= 126) { // Printable character
            if (insert_char(input, c)) {
                redraw_line(input, prompt);
            }
        }
    }
}
