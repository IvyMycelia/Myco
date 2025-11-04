#include "../../include/libs/graphics_bgfx.h"
#include "../../include/libs/graphics_backend.h"

#ifdef MYCO_USE_BGFX

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#include <bgfx/c99/bgfx.h>
#include <bgfx/c99/platform.h>

static uint32_t s_clear_rgba = 0xff000000u; // ARGB
static bool s_bgfx_ready = false;

static uint32_t pack_abgr(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    return ((uint32_t)a << 24) | ((uint32_t)b << 16) | ((uint32_t)g << 8) | (uint32_t)r;
}

bool myco_bgfx_init(SDL_Window* sdl_window, int width, int height) {
    if (!sdl_window) return false;

    // Fetch native window handle from SDL
    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version);
    if (!SDL_GetWindowWMInfo(sdl_window, &wmi)) {
        return false;
    }

    bgfx_platform_data_t pd;
    memset(&pd, 0, sizeof(pd));

#if MYCO_PLATFORM_MACOS
    // SDL on macOS: Cocoa window
    pd.nwh = (void*)wmi.info.cocoa.window;
#elif MYCO_PLATFORM_WINDOWS
    pd.nwh = (void*)wmi.info.win.window;
#elif MYCO_PLATFORM_LINUX
    // X11 or Wayland; SDL reports subsystem
    if (wmi.subsystem == SDL_SYSWM_X11) {
        pd.ndt = wmi.info.x11.display;
        pd.nwh = (void*)(uintptr_t)wmi.info.x11.window;
    } else if (wmi.subsystem == SDL_SYSWM_WAYLAND) {
        pd.ndt = wmi.info.wl.display;
        pd.nwh = wmi.info.wl.surface;
    }
#endif

    bgfx_set_platform_data(&pd);

    bgfx_init_t init;
    bgfx_init_ctor(&init);
    init.type = BGFX_RENDERER_TYPE_COUNT; // auto
    init.resolution.width = (uint32_t)width;
    init.resolution.height = (uint32_t)height;
    init.resolution.reset = BGFX_RESET_VSYNC; // can be toggled later

    if (!bgfx_init(&init)) {
        return false;
    }

    bgfx_set_view_clear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, s_clear_rgba, 1.0f, 0);
    bgfx_set_view_rect(0, 0, 0, (uint16_t)width, (uint16_t)height);

    s_bgfx_ready = true;
    return true;
}

void myco_bgfx_shutdown(void) {
    if (!s_bgfx_ready) return;
    bgfx_shutdown();
    s_bgfx_ready = false;
}

void myco_bgfx_set_clear_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    s_clear_rgba = pack_abgr(r, g, b, a);
    if (!s_bgfx_ready) return;
    bgfx_set_view_clear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, s_clear_rgba, 1.0f, 0);
}

void myco_bgfx_clear(void) {
    if (!s_bgfx_ready) return;
    // Nothing to do; clear happens on submit. We just touch the view.
    bgfx_touch(0);
}

void myco_bgfx_present(void) {
    if (!s_bgfx_ready) return;
    bgfx_frame(false);
}

#else

// Stubs when bgfx is not enabled
bool myco_bgfx_init(struct SDL_Window* sdl_window, int width, int height) {
    (void)sdl_window; (void)width; (void)height; return false;
}
void myco_bgfx_shutdown(void) {}
void myco_bgfx_set_clear_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    (void)r; (void)g; (void)b; (void)a;
}
void myco_bgfx_clear(void) {}
void myco_bgfx_present(void) {}

#endif


