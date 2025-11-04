#ifndef MYCO_GRAPHICS_BGFX_H
#define MYCO_GRAPHICS_BGFX_H

#include <stdbool.h>
#include <stdint.h>

struct SDL_Window; // forward decl

// Initialize bgfx with an SDL window. Returns true on success.
bool myco_bgfx_init(struct SDL_Window* sdl_window, int width, int height);

// Shutdown bgfx and free resources.
void myco_bgfx_shutdown(void);

// Per-frame operations
void myco_bgfx_set_clear_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void myco_bgfx_clear(void);
void myco_bgfx_present(void);

#endif // MYCO_GRAPHICS_BGFX_H


