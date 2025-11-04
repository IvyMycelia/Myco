#ifndef MYCO_GRAPHICS_RENDERER_H
#define MYCO_GRAPHICS_RENDERER_H

#include <stdbool.h>
#include <stdint.h>

struct SDL_Window;

// High-performance in-house graphics renderer
// Uses native GPU APIs for maximum performance

typedef struct {
    void* device;          // Metal MTLDevice* (void* to avoid exposing Metal headers)
    void* command_queue;  // Metal MTLCommandQueue*
    void* layer;          // Metal CAMetalLayer*
    bool initialized;
    uint32_t clear_color_rgba;
    int width;
    int height;
} MycoRenderer;

// Initialize renderer with SDL window (extracts native handle)
bool myco_renderer_init(MycoRenderer* renderer, struct SDL_Window* sdl_window, int width, int height);

// Shutdown and cleanup
void myco_renderer_shutdown(MycoRenderer* renderer);

// Frame lifecycle
void myco_renderer_begin_frame(MycoRenderer* renderer);
void myco_renderer_end_frame(MycoRenderer* renderer);

// Clear screen with color
void myco_renderer_set_clear_color(MycoRenderer* renderer, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void myco_renderer_clear(MycoRenderer* renderer);

// Drawing commands (batched)
void myco_renderer_draw_rect_outline(MycoRenderer* renderer, int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void myco_renderer_draw_rect_filled(MycoRenderer* renderer, int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void myco_renderer_draw_line(MycoRenderer* renderer, int x1, int y1, int x2, int y2, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void myco_renderer_draw_circle_outline(MycoRenderer* renderer, int cx, int cy, int radius, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void myco_renderer_draw_circle_filled(MycoRenderer* renderer, int cx, int cy, int radius, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

// Text rendering (efficient atlas-based)
void* myco_renderer_load_font(MycoRenderer* renderer, const char* font_path, int size);
void myco_renderer_unload_font(MycoRenderer* renderer, void* font_handle);
void myco_renderer_draw_text(MycoRenderer* renderer, void* font_handle, const char* text, int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void myco_renderer_measure_text(MycoRenderer* renderer, void* font_handle, const char* text, int* out_width, int* out_height);

// Update window size (for resize events)
void myco_renderer_resize(MycoRenderer* renderer, int width, int height);

#endif // MYCO_GRAPHICS_RENDERER_H
