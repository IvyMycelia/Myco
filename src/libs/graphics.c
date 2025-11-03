#include "libs/graphics.h"
#include "../../include/core/interpreter.h"
#include "../../include/core/standardized_errors.h"
#include "../../include/utils/shared_utilities.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#define HAS_SDL_TTF
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#ifdef HAS_SDL_TTF
// Font structure for text rendering
typedef struct {
    TTF_Font* font;
    char* font_path;
    int size;
} MycoFont;

// Global font registry (support multiple fonts)
static MycoFont** g_fonts = NULL;
static size_t g_font_count = 0;
static size_t g_font_capacity = 0;

// Text texture cache structure
typedef struct {
    char* text;
    Uint8 r, g, b, a;
    SDL_Texture* texture;
    int w, h;
    Uint32 last_used;  // Frame counter for LRU eviction
} CachedTextTexture;

#define MAX_CACHE_SIZE 200  // Increased cache size to handle more text
static CachedTextTexture* text_cache = NULL;
static size_t cache_size = 0;
static Uint32 cache_frame = 0;
#endif

// Window structure to store SDL window and renderer
typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    int width;
    int height;
    bool is_open;
    Uint8 r, g, b, a;  // Current drawing color
    #ifdef HAS_SDL_TTF
    MycoFont* default_font;  // Default font for text rendering
    #endif
} MycoWindow;

// Global window instance (support one window for now)
static MycoWindow* g_window = NULL;

// Initialize SDL with platform-specific optimizations
static bool init_sdl() {
    static bool sdl_initialized = false;
    if (sdl_initialized) return true;
    
    // Initialize SDL video subsystem
    // SDL_INIT_VIDEO includes windowing, events, and graphics
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
        return false;
    }
    
    // Initialize SDL_ttf for font rendering
    if (TTF_Init() < 0) {
        fprintf(stderr, "SDL_ttf initialization failed: %s\n", TTF_GetError());
        // Don't fail completely - fonts are optional for basic graphics
        // return false;
    }
    
    // Platform-specific initialization hints
    #if MYCO_PLATFORM_WINDOWS
        // Windows: Use high DPI scaling
        SDL_SetHint(SDL_HINT_WINDOWS_DPI_SCALING, "1");
    #elif MYCO_PLATFORM_MACOS
        // macOS: Enable Retina display support (handled by SDL_WINDOW_ALLOW_HIGHDPI)
        SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");
    #elif MYCO_PLATFORM_LINUX
        // Linux: Auto-detect best video driver
        // Can override with SDL_VIDEODRIVER environment variable
    #endif
    
    // Set renderer scaling quality (optional, for better quality)
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    
    sdl_initialized = true;
    return true;
}

// Cleanup SDL
static void cleanup_sdl() {
    #ifdef HAS_SDL_TTF
    // Clean up fonts
    if (g_fonts) {
        for (size_t i = 0; i < g_font_count; i++) {
            if (g_fonts[i]) {
                if (g_fonts[i]->font) {
                    TTF_CloseFont(g_fonts[i]->font);
                }
                shared_free_safe(g_fonts[i]->font_path, "libs", "graphics", 0);
                shared_free_safe(g_fonts[i], "libs", "graphics", 0);
            }
        }
        shared_free_safe(g_fonts, "libs", "graphics", 0);
        g_fonts = NULL;
        g_font_count = 0;
        g_font_capacity = 0;
    }
    #endif
    
    if (g_window) {
        #ifdef HAS_SDL_TTF
        // Don't free default_font - it's part of the registry
        // Just clear the reference
        if (g_window->default_font) {
            g_window->default_font = NULL;
        }
        #endif
        if (g_window->renderer) {
            SDL_DestroyRenderer(g_window->renderer);
        }
        if (g_window->window) {
            SDL_DestroyWindow(g_window->window);
        }
        shared_free_safe(g_window, "libs", "graphics", 0);
        g_window = NULL;
    }
    
    #ifdef HAS_SDL_TTF
    TTF_Quit();
    #endif
    SDL_Quit();
}

// Create a new window
Value builtin_graphics_create_window(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (!init_sdl()) {
        std_error_report(ERROR_INTERNAL_ERROR, "graphics", "unknown_function", "Failed to initialize SDL", line, column);
        return value_create_null();
    }
    
    // Close existing window if any
    if (g_window) {
        builtin_graphics_close_window(interpreter, NULL, 0, line, column);
    }
    
    int width = 800;
    int height = 600;
    const char* title = "Myco Window";
    
    // Parse arguments
    if (arg_count >= 1 && args[0].type == VALUE_NUMBER) {
        width = (int)args[0].data.number_value;
    }
    if (arg_count >= 2 && args[1].type == VALUE_NUMBER) {
        height = (int)args[1].data.number_value;
    }
    if (arg_count >= 3 && args[2].type == VALUE_STRING) {
        title = args[2].data.string_value;
    }
    
    // Create SDL window with cross-platform flags
    // SDL_WINDOW_ALLOW_HIGHDPI enables Retina/High DPI support on all platforms
    // SDL_WINDOW_RESIZABLE allows window resizing (good for testing)
    Uint32 window_flags = SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE;
    
    SDL_Window* window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width,
        height,
        window_flags
    );
    
    if (!window) {
        const char* error_msg = SDL_GetError();
        char full_error[256];
        snprintf(full_error, sizeof(full_error), "Failed to create window: %s", error_msg ? error_msg : "Unknown error");
        std_error_report(ERROR_INTERNAL_ERROR, "graphics", "createWindow", full_error, line, column);
        return value_create_null();
    }
    
    // Create renderer with fallback strategy for maximum compatibility
    // Try hardware-accelerated first, then software fallback
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    if (!renderer) {
        // Fall back to software renderer if hardware fails
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    }
    
    if (!renderer) {
        // Last resort: try basic renderer
        renderer = SDL_CreateRenderer(window, -1, 0);
    }
    
    if (!renderer) {
        const char* error_msg = SDL_GetError();
        char full_error[256];
        snprintf(full_error, sizeof(full_error), "Failed to create renderer: %s", error_msg ? error_msg : "Unknown error");
        SDL_DestroyWindow(window);
        std_error_report(ERROR_INTERNAL_ERROR, "graphics", "createWindow", full_error, line, column);
        return value_create_null();
    }
    
    // Create window object
    g_window = (MycoWindow*)shared_malloc_safe(sizeof(MycoWindow), "libs", "graphics", 0);
    if (!g_window) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        return value_create_null();
    }
    
    g_window->window = window;
    g_window->renderer = renderer;
    g_window->width = width;
    g_window->height = height;
    g_window->is_open = true;
    #ifdef HAS_SDL_TTF
    g_window->default_font = NULL;  // No default font yet
    #endif
    
    // Get actual window size (accounts for high DPI scaling)
    // Logical size is what the user requested, actual size may be scaled
    // For now, we use logical size as the primary size
    // Future: Expose both logical and actual sizes if needed
    g_window->r = 255;
    g_window->g = 255;
    g_window->b = 255;
    g_window->a = 255;
    
    // Pump events immediately after window creation to ensure it appears
    // This is necessary for the window to actually show up on screen
    SDL_PumpEvents();
    
    // Show the window and ensure it's visible
    // Note: SDL_WINDOW_SHOWN flag already makes it visible, but explicitly show it
    SDL_ShowWindow(window);
    SDL_RaiseWindow(window);
    SDL_RestoreWindow(window);  // Ensure window is not minimized
    
    // Enable text input for keyboard capture
    SDL_StartTextInput();
    
    // Ensure window has keyboard focus (helps with receiving keyboard events on macOS/Linux)
    // Note: SDL doesn't have SetWindowInputFocus, so we rely on RaiseWindow and user clicking
    // The window should be raised which helps with focus
    
    // Set initial drawing color to white
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    
    // Initial render to show window content immediately
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    
    // Pump events one more time to ensure window appears
    SDL_PumpEvents();
    
    // Create Myco window object
    Value window_obj = value_create_object(20);
    value_object_set(&window_obj, "__class_name__", value_create_string("Window"));
    value_object_set(&window_obj, "__type__", value_create_string("Window"));
    value_object_set(&window_obj, "type", value_create_string("Window"));
    value_object_set(&window_obj, "width", value_create_number(width));
    value_object_set(&window_obj, "height", value_create_number(height));
    value_object_set(&window_obj, "isOpen", value_create_boolean(true));
    
    // Add methods
    value_object_set(&window_obj, "close", value_create_builtin_function(builtin_window_close));
    value_object_set(&window_obj, "isOpen", value_create_builtin_function(builtin_window_is_open));
    value_object_set(&window_obj, "pollEvents", value_create_builtin_function(builtin_window_poll_events));
    value_object_set(&window_obj, "clear", value_create_builtin_function(builtin_window_clear));
    value_object_set(&window_obj, "present", value_create_builtin_function(builtin_window_present));
    value_object_set(&window_obj, "setColor", value_create_builtin_function(builtin_window_set_color));
    value_object_set(&window_obj, "drawRect", value_create_builtin_function(builtin_window_draw_rect));
    value_object_set(&window_obj, "drawLine", value_create_builtin_function(builtin_window_draw_line));
    value_object_set(&window_obj, "drawCircle", value_create_builtin_function(builtin_window_draw_circle));
    value_object_set(&window_obj, "fillRect", value_create_builtin_function(builtin_window_fill_rect));
    value_object_set(&window_obj, "fillCircle", value_create_builtin_function(builtin_window_fill_circle));
    value_object_set(&window_obj, "getWidth", value_create_builtin_function(builtin_window_get_width));
    value_object_set(&window_obj, "getHeight", value_create_builtin_function(builtin_window_get_height));
    value_object_set(&window_obj, "setTitle", value_create_builtin_function(builtin_window_set_title));
    
    return window_obj;
}

// Close window
Value builtin_graphics_close_window(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (g_window) {
        // Stop text input
        SDL_StopTextInput();
        
        // Mark as closed first to prevent further operations
        g_window->is_open = false;
        
        if (g_window->renderer) {
            SDL_DestroyRenderer(g_window->renderer);
            g_window->renderer = NULL;
        }
        if (g_window->window) {
            SDL_DestroyWindow(g_window->window);
            g_window->window = NULL;
        }
        shared_free_safe(g_window, "libs", "graphics", 0);
        g_window = NULL;
    }
    // Return a success indicator (not null to avoid errors)
    return value_create_boolean(true);
}

// Check if window is open
Value builtin_graphics_is_open(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (!g_window) {
        return value_create_boolean(false);
    }
    return value_create_boolean(g_window->is_open);
}

// Poll events (returns true if window should close)
Value builtin_graphics_poll_events(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (!g_window || !g_window->is_open) {
        return value_create_boolean(false);
    }
    
    // Pump events to ensure window system events are processed
    // This is critical for the window to appear and stay visible
    SDL_PumpEvents();
    
    SDL_Event event;
    // Process ALL events in the queue to find close events (even if keyboard events are present)
    // This ensures we don't miss close events when keyboard events are also in the queue
    while (SDL_PollEvent(&event)) {
        // Skip keyboard events - push them back for getKey() to handle
        // But continue processing other events to find close events
        if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP || event.type == SDL_TEXTINPUT) {
            SDL_PushEvent(&event);
            continue;  // Skip keyboard events, keep processing other events
        }
        
        // Handle close and window events - these are priority
        if (event.type == SDL_QUIT) {
            // Window close button clicked (cross-platform)
            g_window->is_open = false;
            return value_create_boolean(true);
        }
        
        if (event.type == SDL_WINDOWEVENT) {
            // Handle window close event specifically
            if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
                // Only close if this is actually the close event for our window
                if (event.window.windowID == SDL_GetWindowID(g_window->window)) {
                    g_window->is_open = false;
                    return value_create_boolean(true);
                }
                // Not our window - continue processing other events
                continue;
            }
            // Other window events - just continue (they don't close the window)
            continue;
        }
        
        // Other event types - ignore for now
        // Continue processing to find close events
    }
    
    // No close event found - window should stay open
    return value_create_boolean(false);
}

// Get keyboard input (returns key information object or Null)
Value builtin_graphics_get_key(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (!g_window || !g_window->is_open) {
        return value_create_null();
    }
    
    // Ensure window has focus (helps with keyboard input on some platforms)
    // Note: SDL doesn't have SetWindowInputFocus, but we can raise the window occasionally
    // to help ensure it has focus (user may need to click on window first)
    if (g_window->window) {
        // OPTIMIZATION: Reduce window raising frequency to save CPU
        static int focus_counter = 0;
        if ((focus_counter++ % 300) == 0) {  // Every ~5 seconds at 60fps (was 60 = ~1 second)
            SDL_RaiseWindow(g_window->window);
        }
    }
    
    SDL_Event event;
    // Don't pump events here - pollEvents() already handles it
    // Multiple SDL_PumpEvents() calls are expensive
    
    // Check for keyboard events - only process ONE keyboard event per call
    // We need to filter through events to find keyboard events
    // but we only want to consume ONE keyboard event per call
    int found_keyboard = 0;
    
    // Process events one at a time until we find a keyboard event or queue is empty
    // CRITICAL: When text input is enabled, both KEYDOWN and TEXTINPUT fire for the same key
    // We process the entire queue in one pass and prioritize TEXTINPUT for printable characters
    // This ensures we consume both events and only return one key object
    while (SDL_PollEvent(&event)) {
        // Check for close events FIRST - these take priority over keyboard events
        if (event.type == SDL_QUIT) {
            // Window close button clicked - mark window as closed
            g_window->is_open = false;
            // Don't push this back - close events should be consumed
            return value_create_null();  // Return null to indicate no key (window closing)
        }
        
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE) {
            // Window close event - mark window as closed
            if (event.window.windowID == SDL_GetWindowID(g_window->window)) {
                g_window->is_open = false;
                // Don't push this back - close events should be consumed
                return value_create_null();  // Return null to indicate no key (window closing)
            }
            // Not our window - push back and continue
            SDL_PushEvent(&event);
            continue;
        }
        
        // Handle keyboard events - we found one!
        // IMPORTANT: SDL_TEXTINPUT and SDL_KEYDOWN can both fire for the same key press
        // TEXTINPUT comes AFTER KEYDOWN in the queue, so we need to look ahead or process both
        
        if (event.type == SDL_TEXTINPUT) {
            // Handle text input (better for international characters)
            // TEXTINPUT handles printable characters - we'll use this and ignore KEYDOWN for the same key
            
            // Store the character from TEXTINPUT
            const char* text_char = event.text.text;
            
            Value key_obj = value_create_object(10);
            value_object_set(&key_obj, "type", value_create_string("CHAR"));
            value_object_set(&key_obj, "key", value_create_string(text_char));
            value_object_set(&key_obj, "shift", value_create_boolean(false));
            value_object_set(&key_obj, "ctrl", value_create_boolean(false));
            value_object_set(&key_obj, "alt", value_create_boolean(false));
            value_object_set(&key_obj, "cmd", value_create_boolean(false));
            
            // Mark that we found a keyboard event
            found_keyboard = 1;
            
            // Continue processing the rest of the queue to consume any KEYDOWN for this same character
            // KEYDOWN may appear before or after TEXTINPUT in the queue, so we need to process all
            // remaining events and skip any printable KEYDOWN events
            SDL_Event next_event;
            while (SDL_PollEvent(&next_event)) {
                // Check for close events
                if (next_event.type == SDL_QUIT) {
                    g_window->is_open = false;
                    value_free(&key_obj);
                    return value_create_null();
                }
                
                if (next_event.type == SDL_WINDOWEVENT && next_event.window.event == SDL_WINDOWEVENT_CLOSE) {
                    if (next_event.window.windowID == SDL_GetWindowID(g_window->window)) {
                        g_window->is_open = false;
                        value_free(&key_obj);
                        return value_create_null();
                    }
                    SDL_PushEvent(&next_event);
                    continue;
                }
                
                // Skip any KEYDOWN events for printable characters (they correspond to this TEXTINPUT)
                if (next_event.type == SDL_KEYDOWN) {
                    SDL_Keycode keycode = next_event.key.keysym.sym;
                    if (keycode >= 32 && keycode <= 126) {
                        // Printable character KEYDOWN - consume it (TEXTINPUT handles it)
                        continue;  // Don't push back, just consume
                    } else {
                        // Special key - push back for next call to getKey()
                        SDL_PushEvent(&next_event);
                    }
                } else if (next_event.type == SDL_TEXTINPUT) {
                    // Another TEXTINPUT - push back for next call
                    SDL_PushEvent(&next_event);
                } else if (next_event.type == SDL_WINDOWEVENT) {
                    // Push window events back
                    SDL_PushEvent(&next_event);
                } else if (next_event.type != SDL_QUIT) {
                    // Push other events back
                    SDL_PushEvent(&next_event);
                }
            }
            
            // Return the key object - we've consumed the corresponding KEYDOWN
            return key_obj;
        } else if (event.type == SDL_KEYDOWN) {
            // Handle the keyboard event
            SDL_Keycode keycode = event.key.keysym.sym;
            
            // CRITICAL: Skip KEYDOWN for printable characters when text input is enabled
            // TEXTINPUT events handle printable characters better, and both fire for the same key
            // If this is a printable character (32-126), skip it - TEXTINPUT will handle it
            // This prevents double detection when both TEXTINPUT and KEYDOWN fire
            if (keycode >= 32 && keycode <= 126) {
                // This is a printable character - TEXTINPUT will handle it, so skip this KEYDOWN
                // Push back for now (though we'll ignore it), or just consume it
                // Actually, just consume it - we don't need it if TEXTINPUT handles it
                continue;  // Skip this event, continue processing
            }
            
            // Create key object with enough capacity for all properties
            Value key_obj = value_create_object(12);  // type, key, shift, ctrl, alt, cmd = 6 properties, add extra for safety
            // Handle special keys only (we've already skipped printable characters)
            SDL_Keymod mod = event.key.keysym.mod;
            
            const char* key_name = NULL;
            char key_type_str[16] = "SPECIAL";  // Use array to allow modification
            const char* key_type = key_type_str;
            
            // Map special keys
            if (keycode == SDLK_RETURN || keycode == SDLK_KP_ENTER) {
                key_name = "ENTER";
            } else if (keycode == SDLK_BACKSPACE) {
                key_name = "BACKSPACE";
            } else if (keycode == SDLK_DELETE) {
                key_name = "DELETE";
            } else if (keycode == SDLK_TAB) {
                key_name = "TAB";
            } else if (keycode == SDLK_ESCAPE) {
                key_name = "ESCAPE";
            } else if (keycode == SDLK_UP) {
                key_name = "UP";
            } else if (keycode == SDLK_DOWN) {
                key_name = "DOWN";
            } else if (keycode == SDLK_LEFT) {
                key_name = "LEFT";
            } else if (keycode == SDLK_RIGHT) {
                key_name = "RIGHT";
            } else if (keycode == SDLK_HOME) {
                key_name = "HOME";
            } else if (keycode == SDLK_END) {
                key_name = "END";
            } else if (keycode == SDLK_PAGEUP) {
                key_name = "PAGEUP";
            } else if (keycode == SDLK_PAGEDOWN) {
                key_name = "PAGEDOWN";
            } else {
                // Unhandled key - skip it
                value_free(&key_obj);
                continue;
            }
            
            if (key_name) {
                // Use value_object_set_member which checks for existing keys and updates them
                // Also handles capacity expansion better
                Value type_val = value_create_string(key_type);
                Value key_val = value_create_string(key_name);
                Value shift_val = value_create_boolean((mod & KMOD_SHIFT) != 0);
                Value ctrl_val = value_create_boolean((mod & (KMOD_LCTRL | KMOD_RCTRL)) != 0);
                Value alt_val = value_create_boolean((mod & (KMOD_LALT | KMOD_RALT)) != 0);
                Value cmd_val = value_create_boolean((mod & (KMOD_LGUI | KMOD_RGUI)) != 0);
                
                // Set all properties on the key object using set_member (handles updates)
                value_object_set_member(&key_obj, "type", type_val);
                value_object_set_member(&key_obj, "key", key_val);
                value_object_set_member(&key_obj, "shift", shift_val);
                value_object_set_member(&key_obj, "ctrl", ctrl_val);
                value_object_set_member(&key_obj, "alt", alt_val);
                value_object_set_member(&key_obj, "cmd", cmd_val);
                
                // Free the values (value_object_set_member clones them)
                value_free(&type_val);
                value_free(&key_val);
                value_free(&shift_val);
                value_free(&ctrl_val);
                value_free(&alt_val);
                value_free(&cmd_val);
                
                // Free the allocated string if it was dynamically allocated (CHAR type)
                if (strcmp(key_type, "CHAR") == 0) {
                    shared_free_safe((void*)key_name, "libs", "graphics", 0);
                }
                
                return key_obj;
            }
        } else {
            // If event is not a keyboard event and not a close event, push it back for pollEvents() to handle
            // Note: close events are handled above and consumed, so we don't push them back
            if (event.type == SDL_WINDOWEVENT) {
                // Push window events back (except close events which are handled above)
                if (event.window.event != SDL_WINDOWEVENT_CLOSE) {
                    SDL_PushEvent(&event);
                }
            } else if (event.type != SDL_QUIT) {
                // Push other events back (mouse, etc.), but not QUIT (already handled above)
                SDL_PushEvent(&event);
            }
            // Close events (SDL_QUIT and SDL_WINDOWEVENT_CLOSE) are consumed and not pushed back
        }
    }
    
    return value_create_null();
}

// Load font (returns font ID or Null)
Value builtin_graphics_load_font(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    #ifndef HAS_SDL_TTF
    std_error_report(ERROR_INTERNAL_ERROR, "graphics", "loadFont", "SDL_ttf not available. Install SDL2_ttf to use fonts.", line, column);
    return value_create_null();
    #else
    if (arg_count < 2) {
        std_error_report(ERROR_ARGUMENT_COUNT, "graphics", "loadFont", "loadFont() requires at least 2 arguments (path, size)", line, column);
        return value_create_null();
    }
    
    if (args[0].type != VALUE_STRING || args[1].type != VALUE_NUMBER) {
        std_error_report(ERROR_INVALID_ARGUMENT, "graphics", "loadFont", "loadFont() requires (string path, number size)", line, column);
        return value_create_null();
    }
    
    const char* font_path = args[0].data.string_value;
    int font_size = (int)args[1].data.number_value;
    
    if (font_size <= 0) {
        std_error_report(ERROR_INVALID_ARGUMENT, "graphics", "loadFont", "Font size must be positive", line, column);
        return value_create_null();
    }
    
    // Load font
    TTF_Font* font = TTF_OpenFont(font_path, font_size);
    if (!font) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Failed to load font '%s': %s", font_path, TTF_GetError());
        std_error_report(ERROR_INTERNAL_ERROR, "graphics", "loadFont", error_msg, line, column);
        return value_create_null();
    }
    
    // Create font structure
    MycoFont* myco_font = (MycoFont*)shared_malloc_safe(sizeof(MycoFont), "libs", "graphics", 0);
    if (!myco_font) {
        TTF_CloseFont(font);
        std_error_report(ERROR_OUT_OF_MEMORY, "graphics", "loadFont", "Out of memory", line, column);
        return value_create_null();
    }
    
    myco_font->font = font;
    myco_font->font_path = shared_strdup(font_path);
    myco_font->size = font_size;
    
    // Register font
    if (g_font_count >= g_font_capacity) {
        size_t new_capacity = g_font_capacity == 0 ? 4 : g_font_capacity * 2;
        MycoFont** new_fonts = (MycoFont**)shared_realloc_safe(g_fonts, new_capacity * sizeof(MycoFont*), "libs", "graphics", 0);
        if (!new_fonts) {
            TTF_CloseFont(font);
            shared_free_safe(myco_font->font_path, "libs", "graphics", 0);
            shared_free_safe(myco_font, "libs", "graphics", 0);
            std_error_report(ERROR_OUT_OF_MEMORY, "graphics", "loadFont", "Out of memory", line, column);
            return value_create_null();
        }
        g_fonts = new_fonts;
        g_font_capacity = new_capacity;
    }
    
    g_fonts[g_font_count] = myco_font;
    size_t font_id = g_font_count;
    g_font_count++;
    
    // Return font ID as number
    return value_create_number((double)font_id);
    #endif
}

// Draw text (x, y, text, [font_id])
Value builtin_graphics_draw_text(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    #ifndef HAS_SDL_TTF
    std_error_report(ERROR_INTERNAL_ERROR, "graphics", "drawText", "SDL_ttf not available. Install SDL2_ttf to use text rendering.", line, column);
    return value_create_null();
    #else
    if (!g_window || !g_window->is_open || !g_window->renderer || arg_count < 3) {
        return value_create_null();
    }
    
    if (args[0].type != VALUE_NUMBER || args[1].type != VALUE_NUMBER || args[2].type != VALUE_STRING) {
        return value_create_null();
    }
    
    int x = (int)args[0].data.number_value;
    int y = (int)args[1].data.number_value;
    const char* text = args[2].data.string_value;
    
    // Validate text pointer
    if (!text) {
        text = "";
    }
    
    // Get font (use default if not specified)
    TTF_Font* font = NULL;
    if (arg_count >= 4 && args[3].type == VALUE_NUMBER) {
        size_t font_id = (size_t)args[3].data.number_value;
        if (font_id < g_font_count && g_fonts[font_id]) {
            font = g_fonts[font_id]->font;
        }
    }
    
    // Use window default font if no font specified
    if (!font && g_window->default_font) {
        font = g_window->default_font->font;
    }
    
    if (!font) {
        // No font available - can't render text
        return value_create_null();
    }
    
    // Render text to surface
    // Use the window's current color state (updated by setColor)
    SDL_Color color = {g_window->r, g_window->g, g_window->b, g_window->a};
    
    // Handle empty strings - TTF_RenderText_Blended may fail on empty strings
    // For empty strings, skip rendering (don't render a space as it would show incorrectly)
    const char* text_to_render = text;
    size_t text_len = strlen(text);
    
    if (text_len == 0) {
        // Empty string - don't render anything (cursor will show position)
        return value_create_null();
    }
    
    // Simple text texture cache to avoid recreating textures every frame
    // Cache key: text + color (simplified - doesn't account for font, but good enough)
    
    // Initialize cache if needed
    if (!text_cache) {
        text_cache = (CachedTextTexture*)shared_malloc_safe(MAX_CACHE_SIZE * sizeof(CachedTextTexture), "libs", "graphics", 0);
        if (!text_cache) {
            // Fallback to no caching if malloc fails
            goto render_without_cache;
        }
        // Initialize cache entries
        for (size_t i = 0; i < MAX_CACHE_SIZE; i++) {
            text_cache[i].text = NULL;
            text_cache[i].texture = NULL;
        }
    }
    
    cache_frame++;
    
    // OPTIMIZATION: Skip cache for very short strings (single characters, short numbers)
    // These are often dynamically generated and caching them fills up the cache quickly
    size_t text_len_check = strlen(text_to_render);
    bool should_cache = text_len_check > 2;  // Only cache strings longer than 2 chars (lowered threshold)
    
    // Look for cached texture (only if we should cache it)
    int cache_index = -1;
    if (should_cache) {
        // OPTIMIZATION: Search from the end backwards (LRU items are typically at the end)
        // This improves hit rate for recently used items
        for (int i = (int)cache_size - 1; i >= 0; i--) {
            if (text_cache[i].text && 
                strcmp(text_cache[i].text, text_to_render) == 0 &&
                text_cache[i].r == color.r &&
                text_cache[i].g == color.g &&
                text_cache[i].b == color.b &&
                text_cache[i].a == color.a) {
                // Found cached texture!
                cache_index = i;
                text_cache[i].last_used = cache_frame;
                
                // Render cached texture
                SDL_SetTextureBlendMode(text_cache[i].texture, SDL_BLENDMODE_BLEND);
                SDL_Rect dest_rect = {x, y, text_cache[i].w, text_cache[i].h};
                SDL_RenderCopy(g_window->renderer, text_cache[i].texture, NULL, &dest_rect);
                return value_create_null();
            }
        }
    }
    
    // Not in cache - render and cache it
    render_without_cache:
    SDL_Surface* text_surface = TTF_RenderText_Blended(font, text_to_render, color);
    if (!text_surface) {
        return value_create_null();
    }
    
    // Create texture from surface
    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(g_window->renderer, text_surface);
    int text_w = text_surface->w;
    int text_h = text_surface->h;
    SDL_FreeSurface(text_surface);
    
    if (!text_texture) {
        return value_create_null();
    }
    
    // Render texture
    SDL_SetTextureBlendMode(text_texture, SDL_BLENDMODE_BLEND);
    SDL_Rect dest_rect = {x, y, text_w, text_h};
    SDL_RenderCopy(g_window->renderer, text_texture, NULL, &dest_rect);
    
    // Cache the texture (if cache is not full AND we should cache it)
    if (cache_index == -1 && should_cache && text_cache && cache_size < MAX_CACHE_SIZE) {
        size_t new_index = cache_size;
        text_cache[new_index].text = shared_strdup(text_to_render);
        text_cache[new_index].r = color.r;
        text_cache[new_index].g = color.g;
        text_cache[new_index].b = color.b;
        text_cache[new_index].a = color.a;
        text_cache[new_index].texture = text_texture;  // Don't destroy, keep for cache
        text_cache[new_index].w = text_w;
        text_cache[new_index].h = text_h;
        text_cache[new_index].last_used = cache_frame;
        cache_size++;
        // Note: texture will be destroyed when cache is cleared or item evicted
        return value_create_null();
    }
    
    // Cache full - use LRU eviction: find least recently used entry
    // Only evict if we should cache this text (longer strings)
    if (cache_index == -1 && should_cache && text_cache && cache_size >= MAX_CACHE_SIZE) {
        // Find LRU entry (linear search, but cache is small)
        size_t lru_index = 0;
        Uint32 lru_frame = text_cache[0].last_used;
        for (size_t i = 1; i < cache_size; i++) {
            if (text_cache[i].last_used < lru_frame) {
                lru_frame = text_cache[i].last_used;
                lru_index = i;
            }
        }
        
        // Free old cache entry
        if (text_cache[lru_index].text) {
            shared_free_safe(text_cache[lru_index].text, "libs", "graphics", 0);
        }
        if (text_cache[lru_index].texture) {
            SDL_DestroyTexture(text_cache[lru_index].texture);
        }
        
        // Replace with new entry
        text_cache[lru_index].text = shared_strdup(text_to_render);
        text_cache[lru_index].r = color.r;
        text_cache[lru_index].g = color.g;
        text_cache[lru_index].b = color.b;
        text_cache[lru_index].a = color.a;
        text_cache[lru_index].texture = text_texture;
        text_cache[lru_index].w = text_w;
        text_cache[lru_index].h = text_h;
        text_cache[lru_index].last_used = cache_frame;
        return value_create_null();
    }
    
    // Cache malloc failed - just destroy texture immediately
    SDL_DestroyTexture(text_texture);
    
    return value_create_null();
    #endif
}

// Draw multiple text lines at once (batch rendering for performance)
// Takes: texts (array of strings), x_positions (array of numbers), y_positions (array of numbers), colors (array of objects with r,g,b,a)
Value builtin_graphics_draw_text_lines(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    #ifndef HAS_SDL_TTF
    std_error_report(ERROR_INTERNAL_ERROR, "graphics", "drawTextLines", "SDL_ttf not available. Install SDL2_ttf to use text rendering.", line, column);
    return value_create_null();
    #else
    // Initialize cache if needed (reuse existing cache from draw_text)
    if (!text_cache) {
        text_cache = (CachedTextTexture*)shared_malloc_safe(MAX_CACHE_SIZE * sizeof(CachedTextTexture), "libs", "graphics", 0);
        if (text_cache) {
            for (size_t i = 0; i < MAX_CACHE_SIZE; i++) {
                text_cache[i].text = NULL;
                text_cache[i].texture = NULL;
            }
        }
    }
    
    // Increment cache frame counter once per batch call
    cache_frame++;
    
    if (!g_window || !g_window->is_open || !g_window->renderer || arg_count < 3) {
        return value_create_null();
    }
    
    // Arguments: texts (array), x_positions (array), y_positions (array), [colors (array)]
    if (args[0].type != VALUE_ARRAY || args[1].type != VALUE_ARRAY || args[2].type != VALUE_ARRAY) {
        return value_create_null();
    }
    
    Value* texts_array = &args[0];
    Value* x_positions_array = &args[1];
    Value* y_positions_array = &args[2];
    Value* colors_array = NULL;
    bool use_custom_colors = false;
    
    if (arg_count >= 4 && args[3].type == VALUE_ARRAY) {
        colors_array = &args[3];
        use_custom_colors = true;
    }
    
    // Get font
    TTF_Font* font = NULL;
    if (g_window->default_font) {
        font = g_window->default_font->font;
    }
    
    if (!font) {
        return value_create_null();
    }
    
    // Get array lengths
    size_t text_count = texts_array->data.array_value.count;
    size_t x_count = x_positions_array->data.array_value.count;
    size_t y_count = y_positions_array->data.array_value.count;
    
    // Use minimum count to avoid overruns
    size_t count = text_count;
    if (x_count < count) count = x_count;
    if (y_count < count) count = y_count;
    
    // OPTIMIZATION: Batch render all text in one pass
    // This reduces function call overhead dramatically
    for (size_t i = 0; i < count; i++) {
        // Get text
        Value* text_elem = (Value*)texts_array->data.array_value.elements[i];
        if (!text_elem || text_elem->type != VALUE_STRING || !text_elem->data.string_value) {
            continue;  // Skip invalid entries
        }
        const char* text = text_elem->data.string_value;
        size_t text_len = strlen(text);
        if (text_len == 0) {
            continue;  // Skip empty strings
        }
        
        // Get x position
        Value* x_elem = (Value*)x_positions_array->data.array_value.elements[i];
        if (!x_elem || x_elem->type != VALUE_NUMBER) {
            continue;
        }
        int x = (int)x_elem->data.number_value;
        
        // Get y position
        Value* y_elem = (Value*)y_positions_array->data.array_value.elements[i];
        if (!y_elem || y_elem->type != VALUE_NUMBER) {
            continue;
        }
        int y = (int)y_elem->data.number_value;
        
        // Get color (use custom color if provided, otherwise window's current color)
        SDL_Color color;
        if (use_custom_colors && colors_array && i < colors_array->data.array_value.count) {
            Value* color_elem = (Value*)colors_array->data.array_value.elements[i];
            if (color_elem && color_elem->type == VALUE_OBJECT) {
                // Extract r, g, b, a from color object
                Value r_val = value_object_get(color_elem, "r");
                Value g_val = value_object_get(color_elem, "g");
                Value b_val = value_object_get(color_elem, "b");
                Value a_val = value_object_get(color_elem, "a");
                
                if (r_val.type == VALUE_NUMBER && g_val.type == VALUE_NUMBER && 
                    b_val.type == VALUE_NUMBER && a_val.type == VALUE_NUMBER) {
                    color.r = (Uint8)r_val.data.number_value;
                    color.g = (Uint8)g_val.data.number_value;
                    color.b = (Uint8)b_val.data.number_value;
                    color.a = (Uint8)a_val.data.number_value;
                } else {
                    // Fallback to window color
                    color.r = g_window->r;
                    color.g = g_window->g;
                    color.b = g_window->b;
                    color.a = g_window->a;
                }
            } else {
                // Fallback to window color
                color.r = g_window->r;
                color.g = g_window->g;
                color.b = g_window->b;
                color.a = g_window->a;
            }
        } else {
            // Use window's current color
            color.r = g_window->r;
            color.g = g_window->g;
            color.b = g_window->b;
            color.a = g_window->a;
        }
        
        // Reuse existing cache from draw_text for performance
        size_t text_len_check = strlen(text);
        bool should_cache = text_len_check > 2;
        
        // Check cache first
        bool found_in_cache = false;
        if (should_cache && text_cache) {
            // Search cache backwards (recent items at end)
            for (int j = (int)cache_size - 1; j >= 0; j--) {
                if (text_cache[j].text && 
                    strcmp(text_cache[j].text, text) == 0 &&
                    text_cache[j].r == color.r &&
                    text_cache[j].g == color.g &&
                    text_cache[j].b == color.b &&
                    text_cache[j].a == color.a) {
                    // Found cached texture!
                    text_cache[j].last_used = cache_frame;
                    
                    // Render cached texture
                    SDL_SetTextureBlendMode(text_cache[j].texture, SDL_BLENDMODE_BLEND);
                    SDL_Rect dest_rect = {x, y, text_cache[j].w, text_cache[j].h};
                    SDL_RenderCopy(g_window->renderer, text_cache[j].texture, NULL, &dest_rect);
                    found_in_cache = true;
                    break;
                }
            }
        }
        
        if (found_in_cache) {
            continue;  // Already rendered from cache
        }
        
        // Not in cache - render it
        SDL_Surface* text_surface = TTF_RenderText_Blended(font, text, color);
        if (!text_surface) {
            continue;
        }
        
        SDL_Texture* text_texture = SDL_CreateTextureFromSurface(g_window->renderer, text_surface);
        int text_w = text_surface->w;
        int text_h = text_surface->h;
        SDL_FreeSurface(text_surface);
        
        if (!text_texture) {
            continue;
        }
        
        // Render texture
        SDL_SetTextureBlendMode(text_texture, SDL_BLENDMODE_BLEND);
        SDL_Rect dest_rect = {x, y, text_w, text_h};
        SDL_RenderCopy(g_window->renderer, text_texture, NULL, &dest_rect);
        
        // Cache the texture (if we should cache it)
        if (should_cache && text_cache) {
            if (cache_size < MAX_CACHE_SIZE) {
                // Add to cache
                size_t new_index = cache_size;
                text_cache[new_index].text = shared_strdup(text);
                text_cache[new_index].r = color.r;
                text_cache[new_index].g = color.g;
                text_cache[new_index].b = color.b;
                text_cache[new_index].a = color.a;
                text_cache[new_index].texture = text_texture;  // Don't destroy, keep for cache
                text_cache[new_index].w = text_w;
                text_cache[new_index].h = text_h;
                text_cache[new_index].last_used = cache_frame;
                cache_size++;
                continue;  // Don't destroy texture, it's cached
            } else {
                // Cache full - use LRU eviction
                size_t lru_index = 0;
                Uint32 lru_frame = text_cache[0].last_used;
                for (size_t j = 1; j < cache_size; j++) {
                    if (text_cache[j].last_used < lru_frame) {
                        lru_frame = text_cache[j].last_used;
                        lru_index = j;
                    }
                }
                
                // Free old cache entry
                if (text_cache[lru_index].text) {
                    shared_free_safe(text_cache[lru_index].text, "libs", "graphics", 0);
                }
                if (text_cache[lru_index].texture) {
                    SDL_DestroyTexture(text_cache[lru_index].texture);
                }
                
                // Replace with new entry
                text_cache[lru_index].text = shared_strdup(text);
                text_cache[lru_index].r = color.r;
                text_cache[lru_index].g = color.g;
                text_cache[lru_index].b = color.b;
                text_cache[lru_index].a = color.a;
                text_cache[lru_index].texture = text_texture;
                text_cache[lru_index].w = text_w;
                text_cache[lru_index].h = text_h;
                text_cache[lru_index].last_used = cache_frame;
                continue;  // Don't destroy texture, it's cached
            }
        }
        
        // Not caching - cleanup immediately
        SDL_DestroyTexture(text_texture);
    }
    
    return value_create_null();
    #endif
}

// Draw text lines directly from a lines array (optimized for text editors)
// Takes: lines (array of strings), start_x, start_y, line_height, padding_x, padding_y, scroll_y, window_height
// All rendering logic happens in fast C code - no interpreted loops
Value builtin_graphics_draw_text_lines_from_array(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    #ifndef HAS_SDL_TTF
    std_error_report(ERROR_INTERNAL_ERROR, "graphics", "drawTextLinesFromArray", "SDL_ttf not available. Install SDL2_ttf to use text rendering.", line, column);
    return value_create_null();
    #else
    // Initialize cache if needed (reuse existing cache from draw_text)
    if (!text_cache) {
        text_cache = (CachedTextTexture*)shared_malloc_safe(MAX_CACHE_SIZE * sizeof(CachedTextTexture), "libs", "graphics", 0);
        if (text_cache) {
            for (size_t i = 0; i < MAX_CACHE_SIZE; i++) {
                text_cache[i].text = NULL;
                text_cache[i].texture = NULL;
            }
        }
    }
    
    // Increment cache frame counter once per batch call
    cache_frame++;
    
    if (!g_window || !g_window->is_open || !g_window->renderer || arg_count < 8) {
        return value_create_null();
    }
    
    // Arguments: lines (array of strings), start_x, start_y, line_height, padding_x, padding_y, scroll_y, window_height
    if (args[0].type != VALUE_ARRAY) {
        return value_create_null();
    }
    
    Value* lines_array = &args[0];
    size_t lines_count = lines_array->data.array_value.count;
    
    // Get numeric parameters
    if (args[1].type != VALUE_NUMBER || args[2].type != VALUE_NUMBER || args[3].type != VALUE_NUMBER ||
        args[4].type != VALUE_NUMBER || args[5].type != VALUE_NUMBER || args[6].type != VALUE_NUMBER ||
        args[7].type != VALUE_NUMBER) {
        return value_create_null();
    }
    
    int start_x = (int)args[1].data.number_value;
    int start_y = (int)args[2].data.number_value;
    int line_height = (int)args[3].data.number_value;
    int padding_x = (int)args[4].data.number_value;
    int padding_y = (int)args[5].data.number_value;
    int scroll_y = (int)args[6].data.number_value;
    int window_height = (int)args[7].data.number_value;
    
    // Get font
    TTF_Font* font = NULL;
    if (g_window->default_font) {
        font = g_window->default_font->font;
    }
    
    if (!font) {
        return value_create_null();
    }
    
    // OPTIMIZATION: All rendering logic in fast C code
    // Calculate visible lines and render them directly in C - no interpreted loops
    int start_line_idx = scroll_y > 0 ? scroll_y : 0;
    
    // Gray color for line numbers
    SDL_Color gray_color = {150, 150, 150, 255};
    // Black color for text
    SDL_Color black_color = {0, 0, 0, 255};
    
    // Render all visible lines in one pass (entirely in C code)
    for (size_t i = start_line_idx; i < lines_count; i++) {
        // Calculate Y position
        int y_pos = padding_y + (int)(i - start_line_idx) * line_height;
        
        // Check if line is visible
        if (y_pos < padding_y || y_pos >= window_height) {
            continue;  // Skip non-visible lines
        }
        
        // Get line from array (direct C access)
        Value* line_elem = (Value*)lines_array->data.array_value.elements[i];
        if (!line_elem || line_elem->type != VALUE_STRING) {
            continue;
        }
        const char* line_text = line_elem->data.string_value;
        if (!line_text) {
            line_text = "";
        }
        
        // Render line number (convert i+1 to string in C)
        char line_num_buf[32];
        snprintf(line_num_buf, sizeof(line_num_buf), "%zu", i + 1);
        
        // Render line number using cached rendering
        size_t line_num_len = strlen(line_num_buf);
        bool should_cache = line_num_len > 2;
        bool found_in_cache = false;
        
        if (should_cache && text_cache) {
            for (int j = (int)cache_size - 1; j >= 0; j--) {
                if (text_cache[j].text && 
                    strcmp(text_cache[j].text, line_num_buf) == 0 &&
                    text_cache[j].r == gray_color.r &&
                    text_cache[j].g == gray_color.g &&
                    text_cache[j].b == gray_color.b &&
                    text_cache[j].a == gray_color.a) {
                    text_cache[j].last_used = cache_frame;
                    SDL_SetTextureBlendMode(text_cache[j].texture, SDL_BLENDMODE_BLEND);
                    SDL_Rect dest_rect = {start_x, y_pos, text_cache[j].w, text_cache[j].h};
                    SDL_RenderCopy(g_window->renderer, text_cache[j].texture, NULL, &dest_rect);
                    found_in_cache = true;
                    break;
                }
            }
        }
        
        if (!found_in_cache && line_num_len > 0) {
            SDL_Surface* num_surface = TTF_RenderText_Blended(font, line_num_buf, gray_color);
            if (num_surface) {
                SDL_Texture* num_texture = SDL_CreateTextureFromSurface(g_window->renderer, num_surface);
                int num_w = num_surface->w;
                int num_h = num_surface->h;
                SDL_FreeSurface(num_surface);
                
                if (num_texture) {
                    SDL_SetTextureBlendMode(num_texture, SDL_BLENDMODE_BLEND);
                    SDL_Rect dest_rect = {start_x, y_pos, num_w, num_h};
                    SDL_RenderCopy(g_window->renderer, num_texture, NULL, &dest_rect);
                    
                    // Cache it if we should
                    if (should_cache && text_cache && cache_size < MAX_CACHE_SIZE) {
                        text_cache[cache_size].text = shared_strdup(line_num_buf);
                        text_cache[cache_size].r = gray_color.r;
                        text_cache[cache_size].g = gray_color.g;
                        text_cache[cache_size].b = gray_color.b;
                        text_cache[cache_size].a = gray_color.a;
                        text_cache[cache_size].texture = num_texture;
                        text_cache[cache_size].w = num_w;
                        text_cache[cache_size].h = num_h;
                        text_cache[cache_size].last_used = cache_frame;
                        cache_size++;
                    } else if (should_cache && text_cache && cache_size >= MAX_CACHE_SIZE) {
                        // LRU eviction
                        size_t lru_idx = 0;
                        Uint32 lru_frame = text_cache[0].last_used;
                        for (size_t j = 1; j < cache_size; j++) {
                            if (text_cache[j].last_used < lru_frame) {
                                lru_frame = text_cache[j].last_used;
                                lru_idx = j;
                            }
                        }
                        if (text_cache[lru_idx].text) shared_free_safe(text_cache[lru_idx].text, "libs", "graphics", 0);
                        if (text_cache[lru_idx].texture) SDL_DestroyTexture(text_cache[lru_idx].texture);
                        text_cache[lru_idx].text = shared_strdup(line_num_buf);
                        text_cache[lru_idx].r = gray_color.r;
                        text_cache[lru_idx].g = gray_color.g;
                        text_cache[lru_idx].b = gray_color.b;
                        text_cache[lru_idx].a = gray_color.a;
                        text_cache[lru_idx].texture = num_texture;
                        text_cache[lru_idx].w = num_w;
                        text_cache[lru_idx].h = num_h;
                        text_cache[lru_idx].last_used = cache_frame;
                    } else {
                        SDL_DestroyTexture(num_texture);
                    }
                }
            }
        }
        
        // Render line text if it has content
        size_t line_text_len = strlen(line_text);
        if (line_text_len > 0) {
            should_cache = line_text_len > 2;
            found_in_cache = false;
            
            if (should_cache && text_cache) {
                for (int j = (int)cache_size - 1; j >= 0; j--) {
                    if (text_cache[j].text && 
                        strcmp(text_cache[j].text, line_text) == 0 &&
                        text_cache[j].r == black_color.r &&
                        text_cache[j].g == black_color.g &&
                        text_cache[j].b == black_color.b &&
                        text_cache[j].a == black_color.a) {
                        text_cache[j].last_used = cache_frame;
                        SDL_SetTextureBlendMode(text_cache[j].texture, SDL_BLENDMODE_BLEND);
                        SDL_Rect dest_rect = {padding_x, y_pos, text_cache[j].w, text_cache[j].h};
                        SDL_RenderCopy(g_window->renderer, text_cache[j].texture, NULL, &dest_rect);
                        found_in_cache = true;
                        break;
                    }
                }
            }
            
            if (!found_in_cache) {
                SDL_Surface* text_surface = TTF_RenderText_Blended(font, line_text, black_color);
                if (text_surface) {
                    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(g_window->renderer, text_surface);
                    int text_w = text_surface->w;
                    int text_h = text_surface->h;
                    SDL_FreeSurface(text_surface);
                    
                    if (text_texture) {
                        SDL_SetTextureBlendMode(text_texture, SDL_BLENDMODE_BLEND);
                        SDL_Rect dest_rect = {padding_x, y_pos, text_w, text_h};
                        SDL_RenderCopy(g_window->renderer, text_texture, NULL, &dest_rect);
                        
                        // Cache it if we should
                        if (should_cache && text_cache && cache_size < MAX_CACHE_SIZE) {
                            text_cache[cache_size].text = shared_strdup(line_text);
                            text_cache[cache_size].r = black_color.r;
                            text_cache[cache_size].g = black_color.g;
                            text_cache[cache_size].b = black_color.b;
                            text_cache[cache_size].a = black_color.a;
                            text_cache[cache_size].texture = text_texture;
                            text_cache[cache_size].w = text_w;
                            text_cache[cache_size].h = text_h;
                            text_cache[cache_size].last_used = cache_frame;
                            cache_size++;
                        } else if (should_cache && text_cache && cache_size >= MAX_CACHE_SIZE) {
                            // LRU eviction
                            size_t lru_idx = 0;
                            Uint32 lru_frame = text_cache[0].last_used;
                            for (size_t j = 1; j < cache_size; j++) {
                                if (text_cache[j].last_used < lru_frame) {
                                    lru_frame = text_cache[j].last_used;
                                    lru_idx = j;
                                }
                            }
                            if (text_cache[lru_idx].text) shared_free_safe(text_cache[lru_idx].text, "libs", "graphics", 0);
                            if (text_cache[lru_idx].texture) SDL_DestroyTexture(text_cache[lru_idx].texture);
                            text_cache[lru_idx].text = shared_strdup(line_text);
                            text_cache[lru_idx].r = black_color.r;
                            text_cache[lru_idx].g = black_color.g;
                            text_cache[lru_idx].b = black_color.b;
                            text_cache[lru_idx].a = black_color.a;
                            text_cache[lru_idx].texture = text_texture;
                            text_cache[lru_idx].w = text_w;
                            text_cache[lru_idx].h = text_h;
                            text_cache[lru_idx].last_used = cache_frame;
                        } else {
                            SDL_DestroyTexture(text_texture);
                        }
                    }
                }
            }
        }
    }
    
    return value_create_null();
    #endif
}

// Measure text dimensions (text, [font_id])
Value builtin_graphics_measure_text(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    #ifndef HAS_SDL_TTF
    std_error_report(ERROR_INTERNAL_ERROR, "graphics", "measureText", "SDL_ttf not available. Install SDL2_ttf to use text measurement.", line, column);
    return value_create_null();
    #else
    if (arg_count < 1 || args[0].type != VALUE_STRING) {
        return value_create_null();
    }
    
    const char* text = args[0].data.string_value;
    
    // Get font (use default if not specified)
    TTF_Font* font = NULL;
    if (arg_count >= 2 && args[1].type == VALUE_NUMBER) {
        size_t font_id = (size_t)args[1].data.number_value;
        if (font_id < g_font_count && g_fonts[font_id]) {
            font = g_fonts[font_id]->font;
        }
    }
    
    // Use window default font if no font specified
    if (!font && g_window && g_window->default_font) {
        font = g_window->default_font->font;
    }
    
    if (!font) {
        // No font available
        return value_create_null();
    }
    
    // Measure text
    int w = 0, h = 0;
    if (TTF_SizeText(font, text, &w, &h) != 0) {
        return value_create_null();
    }
    
    // Return dimensions object
    Value dims = value_create_object(4);
    value_object_set(&dims, "width", value_create_number(w));
    value_object_set(&dims, "height", value_create_number(h));
    
    return dims;
    #endif
}

// Set default font (font_id)
Value builtin_graphics_set_default_font(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    #ifndef HAS_SDL_TTF
    std_error_report(ERROR_INTERNAL_ERROR, "graphics", "setDefaultFont", "SDL_ttf not available. Install SDL2_ttf to use fonts.", line, column);
    return value_create_null();
    #else
    if (!g_window || arg_count < 1 || args[0].type != VALUE_NUMBER) {
        return value_create_null();
    }
    
    size_t font_id = (size_t)args[0].data.number_value;
    if (font_id >= g_font_count || !g_fonts[font_id]) {
        std_error_report(ERROR_INVALID_ARGUMENT, "graphics", "setDefaultFont", "Invalid font ID", line, column);
        return value_create_null();
    }
    
    g_window->default_font = g_fonts[font_id];
    
    return value_create_boolean(true);
    #endif
}

// Clear window with current color
Value builtin_graphics_clear(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (!g_window || !g_window->is_open || !g_window->renderer) {
        return value_create_null();
    }
    
    SDL_SetRenderDrawColor(g_window->renderer, g_window->r, g_window->g, g_window->b, g_window->a);
    SDL_RenderClear(g_window->renderer);
    
    return value_create_null();
}

// Present rendered frame
Value builtin_graphics_present(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (!g_window || !g_window->is_open || !g_window->renderer) {
        return value_create_null();
    }
    
    SDL_RenderPresent(g_window->renderer);
    
    // REMOVED frame pacing delay - if we're already slow (4fps), delaying makes it worse
    // Frame pacing only helps when we're running too fast, not when we're bottlenecked
    return value_create_null();
}

// Set drawing color (r, g, b, a)
Value builtin_graphics_set_color(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (!g_window || !g_window->is_open) {
        return value_create_null();
    }
    
    Uint8 r = 255, g = 255, b = 255, a = 255;
    
    if (arg_count >= 1 && args[0].type == VALUE_NUMBER) {
        r = (Uint8)(args[0].data.number_value);
    }
    if (arg_count >= 2 && args[1].type == VALUE_NUMBER) {
        g = (Uint8)(args[1].data.number_value);
    }
    if (arg_count >= 3 && args[2].type == VALUE_NUMBER) {
        b = (Uint8)(args[2].data.number_value);
    }
    if (arg_count >= 4 && args[3].type == VALUE_NUMBER) {
        a = (Uint8)(args[3].data.number_value);
    }
    
    g_window->r = r;
    g_window->g = g;
    g_window->b = b;
    g_window->a = a;
    
    if (g_window->renderer) {
        SDL_SetRenderDrawColor(g_window->renderer, r, g, b, a);
    }
    
    return value_create_null();
}

// Draw rectangle outline
Value builtin_graphics_draw_rect(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (!g_window || !g_window->is_open || !g_window->renderer || arg_count < 4) {
        return value_create_null();
    }
    
    if (args[0].type != VALUE_NUMBER || args[1].type != VALUE_NUMBER ||
        args[2].type != VALUE_NUMBER || args[3].type != VALUE_NUMBER) {
        return value_create_null();
    }
    
    SDL_Rect rect;
    rect.x = (int)args[0].data.number_value;
    rect.y = (int)args[1].data.number_value;
    rect.w = (int)args[2].data.number_value;
    rect.h = (int)args[3].data.number_value;
    
    SDL_SetRenderDrawColor(g_window->renderer, g_window->r, g_window->g, g_window->b, g_window->a);
    SDL_RenderDrawRect(g_window->renderer, &rect);
    
    return value_create_null();
}

// Draw line
Value builtin_graphics_draw_line(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (!g_window || !g_window->is_open || !g_window->renderer || arg_count < 4) {
        return value_create_null();
    }
    
    if (args[0].type != VALUE_NUMBER || args[1].type != VALUE_NUMBER ||
        args[2].type != VALUE_NUMBER || args[3].type != VALUE_NUMBER) {
        return value_create_null();
    }
    
    int x1 = (int)args[0].data.number_value;
    int y1 = (int)args[1].data.number_value;
    int x2 = (int)args[2].data.number_value;
    int y2 = (int)args[3].data.number_value;
    
    SDL_SetRenderDrawColor(g_window->renderer, g_window->r, g_window->g, g_window->b, g_window->a);
    SDL_RenderDrawLine(g_window->renderer, x1, y1, x2, y2);
    
    return value_create_null();
}

// Draw circle outline (using multiple lines to approximate)
Value builtin_graphics_draw_circle(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (!g_window || !g_window->is_open || !g_window->renderer || arg_count < 3) {
        return value_create_null();
    }
    
    if (args[0].type != VALUE_NUMBER || args[1].type != VALUE_NUMBER ||
        args[2].type != VALUE_NUMBER) {
        return value_create_null();
    }
    
    int cx = (int)args[0].data.number_value;
    int cy = (int)args[1].data.number_value;
    int radius = (int)args[2].data.number_value;
    
    SDL_SetRenderDrawColor(g_window->renderer, g_window->r, g_window->g, g_window->b, g_window->a);
    
    // Draw circle using multiple points
    const int segments = 64;
    for (int i = 0; i < segments; i++) {
        double angle1 = (2.0 * 3.141592653589793 * i) / segments;
        double angle2 = (2.0 * 3.141592653589793 * (i + 1)) / segments;
        
        int x1 = cx + (int)(radius * cos(angle1));
        int y1 = cy + (int)(radius * sin(angle1));
        int x2 = cx + (int)(radius * cos(angle2));
        int y2 = cy + (int)(radius * sin(angle2));
        
        SDL_RenderDrawLine(g_window->renderer, x1, y1, x2, y2);
    }
    
    return value_create_null();
}

// Fill rectangle
Value builtin_graphics_fill_rect(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (!g_window || !g_window->is_open || !g_window->renderer || arg_count < 4) {
        return value_create_null();
    }
    
    if (args[0].type != VALUE_NUMBER || args[1].type != VALUE_NUMBER ||
        args[2].type != VALUE_NUMBER || args[3].type != VALUE_NUMBER) {
        return value_create_null();
    }
    
    SDL_Rect rect;
    rect.x = (int)args[0].data.number_value;
    rect.y = (int)args[1].data.number_value;
    rect.w = (int)args[2].data.number_value;
    rect.h = (int)args[3].data.number_value;
    
    SDL_SetRenderDrawColor(g_window->renderer, g_window->r, g_window->g, g_window->b, g_window->a);
    SDL_RenderFillRect(g_window->renderer, &rect);
    
    return value_create_null();
}

// Fill circle (using filled rectangle approximation)
Value builtin_graphics_fill_circle(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (!g_window || !g_window->is_open || !g_window->renderer || arg_count < 3) {
        return value_create_null();
    }
    
    if (args[0].type != VALUE_NUMBER || args[1].type != VALUE_NUMBER ||
        args[2].type != VALUE_NUMBER) {
        return value_create_null();
    }
    
    int cx = (int)args[0].data.number_value;
    int cy = (int)args[1].data.number_value;
    int radius = (int)args[2].data.number_value;
    
    SDL_SetRenderDrawColor(g_window->renderer, g_window->r, g_window->g, g_window->b, g_window->a);
    
    // Fill circle by drawing filled rectangles for each row
    for (int y = -radius; y <= radius; y++) {
        int x_extent = (int)sqrt(radius * radius - y * y);
        SDL_Rect rect;
        rect.x = cx - x_extent;
        rect.y = cy + y;
        rect.w = 2 * x_extent;
        rect.h = 1;
        SDL_RenderFillRect(g_window->renderer, &rect);
    }
    
    return value_create_null();
}

// Get window width
Value builtin_graphics_get_width(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (!g_window) {
        return value_create_number(0);
    }
    return value_create_number(g_window->width);
}

// Get window height
Value builtin_graphics_get_height(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (!g_window) {
        return value_create_number(0);
    }
    return value_create_number(g_window->height);
}

// Set window title
Value builtin_graphics_set_title(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (!g_window || !g_window->window || arg_count < 1 || args[0].type != VALUE_STRING) {
        return value_create_null();
    }
    
    SDL_SetWindowTitle(g_window->window, args[0].data.string_value);
    return value_create_null();
}

// Window object methods (these take window object as first argument)
Value builtin_window_close(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    return builtin_graphics_close_window(interpreter, args, arg_count, line, column);
}

Value builtin_window_is_open(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    return builtin_graphics_is_open(interpreter, args, arg_count, line, column);
}

Value builtin_window_poll_events(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    return builtin_graphics_poll_events(interpreter, args, arg_count, line, column);
}

Value builtin_window_clear(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    return builtin_graphics_clear(interpreter, args, arg_count, line, column);
}

Value builtin_window_present(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    return builtin_graphics_present(interpreter, args, arg_count, line, column);
}

Value builtin_window_set_color(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    // Skip first argument (window object) for window methods
    if (arg_count > 0 && args[0].type == VALUE_OBJECT) {
        Value* new_args = args + 1;
        size_t new_count = arg_count > 0 ? arg_count - 1 : 0;
        return builtin_graphics_set_color(interpreter, new_args, new_count, line, column);
    }
    return builtin_graphics_set_color(interpreter, args, arg_count, line, column);
}

Value builtin_window_draw_rect(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count > 0 && args[0].type == VALUE_OBJECT) {
        Value* new_args = args + 1;
        size_t new_count = arg_count > 0 ? arg_count - 1 : 0;
        return builtin_graphics_draw_rect(interpreter, new_args, new_count, line, column);
    }
    return builtin_graphics_draw_rect(interpreter, args, arg_count, line, column);
}

Value builtin_window_draw_line(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count > 0 && args[0].type == VALUE_OBJECT) {
        Value* new_args = args + 1;
        size_t new_count = arg_count > 0 ? arg_count - 1 : 0;
        return builtin_graphics_draw_line(interpreter, new_args, new_count, line, column);
    }
    return builtin_graphics_draw_line(interpreter, args, arg_count, line, column);
}

Value builtin_window_draw_circle(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count > 0 && args[0].type == VALUE_OBJECT) {
        Value* new_args = args + 1;
        size_t new_count = arg_count > 0 ? arg_count - 1 : 0;
        return builtin_graphics_draw_circle(interpreter, new_args, new_count, line, column);
    }
    return builtin_graphics_draw_circle(interpreter, args, arg_count, line, column);
}

Value builtin_window_fill_rect(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count > 0 && args[0].type == VALUE_OBJECT) {
        Value* new_args = args + 1;
        size_t new_count = arg_count > 0 ? arg_count - 1 : 0;
        return builtin_graphics_fill_rect(interpreter, new_args, new_count, line, column);
    }
    return builtin_graphics_fill_rect(interpreter, args, arg_count, line, column);
}

Value builtin_window_fill_circle(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count > 0 && args[0].type == VALUE_OBJECT) {
        Value* new_args = args + 1;
        size_t new_count = arg_count > 0 ? arg_count - 1 : 0;
        return builtin_graphics_fill_circle(interpreter, new_args, new_count, line, column);
    }
    return builtin_graphics_fill_circle(interpreter, args, arg_count, line, column);
}

Value builtin_window_get_width(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    return builtin_graphics_get_width(interpreter, args, arg_count, line, column);
}

Value builtin_window_get_height(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    return builtin_graphics_get_height(interpreter, args, arg_count, line, column);
}

Value builtin_window_set_title(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count > 0 && args[0].type == VALUE_OBJECT) {
        Value* new_args = args + 1;
        size_t new_count = arg_count > 0 ? arg_count - 1 : 0;
        return builtin_graphics_set_title(interpreter, new_args, new_count, line, column);
    }
    return builtin_graphics_set_title(interpreter, args, arg_count, line, column);
}

// Register graphics library
void graphics_library_register(Interpreter* interpreter) {
    if (!interpreter || !interpreter->global_environment) return;
    
    // Create graphics library object (enough capacity for all properties)
    Value graphics_lib = value_create_object(25); // Increased capacity for new functions
    // Mark as Library for .type reporting (matching math library pattern)
    value_object_set(&graphics_lib, "__type__", value_create_string("Library"));
    value_object_set(&graphics_lib, "__library_name__", value_create_string("graphics"));
    value_object_set(&graphics_lib, "type", value_create_string("Library"));
    
    // Add functions
    value_object_set(&graphics_lib, "createWindow", value_create_builtin_function(builtin_graphics_create_window));
    value_object_set(&graphics_lib, "closeWindow", value_create_builtin_function(builtin_graphics_close_window));
    value_object_set(&graphics_lib, "isOpen", value_create_builtin_function(builtin_graphics_is_open));
    value_object_set(&graphics_lib, "pollEvents", value_create_builtin_function(builtin_graphics_poll_events));
    value_object_set(&graphics_lib, "getKey", value_create_builtin_function(builtin_graphics_get_key));
    value_object_set(&graphics_lib, "clear", value_create_builtin_function(builtin_graphics_clear));
    value_object_set(&graphics_lib, "present", value_create_builtin_function(builtin_graphics_present));
    value_object_set(&graphics_lib, "setColor", value_create_builtin_function(builtin_graphics_set_color));
    value_object_set(&graphics_lib, "drawRect", value_create_builtin_function(builtin_graphics_draw_rect));
    value_object_set(&graphics_lib, "drawLine", value_create_builtin_function(builtin_graphics_draw_line));
    value_object_set(&graphics_lib, "drawCircle", value_create_builtin_function(builtin_graphics_draw_circle));
    value_object_set(&graphics_lib, "fillRect", value_create_builtin_function(builtin_graphics_fill_rect));
    value_object_set(&graphics_lib, "fillCircle", value_create_builtin_function(builtin_graphics_fill_circle));
    value_object_set(&graphics_lib, "getWidth", value_create_builtin_function(builtin_graphics_get_width));
    value_object_set(&graphics_lib, "getHeight", value_create_builtin_function(builtin_graphics_get_height));
    value_object_set(&graphics_lib, "setTitle", value_create_builtin_function(builtin_graphics_set_title));
    value_object_set(&graphics_lib, "loadFont", value_create_builtin_function(builtin_graphics_load_font));
    value_object_set(&graphics_lib, "drawText", value_create_builtin_function(builtin_graphics_draw_text));
    value_object_set(&graphics_lib, "drawTextLines", value_create_builtin_function(builtin_graphics_draw_text_lines));
    value_object_set(&graphics_lib, "drawTextLinesFromArray", value_create_builtin_function(builtin_graphics_draw_text_lines_from_array));
    value_object_set(&graphics_lib, "measureText", value_create_builtin_function(builtin_graphics_measure_text));
    value_object_set(&graphics_lib, "setDefaultFont", value_create_builtin_function(builtin_graphics_set_default_font));
    
    // Register the library in global environment
    environment_define(interpreter->global_environment, "graphics", graphics_lib);
}

