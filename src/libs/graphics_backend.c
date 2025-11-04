#include "../../include/libs/graphics_backend.h"
#include <stdlib.h>
#include <string.h>

// Storage for global backend selector
MycoGraphicsBackendKind g_myco_graphics_backend = MYCO_GFX_BACKEND_SDL;

static int str_eq_ci(const char* a, const char* b) {
    if (!a || !b) return 0;
    while (*a && *b) {
        char ca = *a, cb = *b;
        if (ca >= 'A' && ca <= 'Z') ca = (char)(ca - 'A' + 'a');
        if (cb >= 'A' && cb <= 'Z') cb = (char)(cb - 'A' + 'a');
        if (ca != cb) return 0;
        ++a; ++b;
    }
    return *a == 0 && *b == 0;
}

void myco_graphics_backend_select_auto(void) {
#ifdef MYCO_USE_BGFX
    g_myco_graphics_backend = MYCO_GFX_BACKEND_BGFX;
#else
    g_myco_graphics_backend = MYCO_GFX_BACKEND_SDL;
#endif
    // Allow override by env var: MYCO_GFX=bgfx|sdl
    const char* env = getenv("MYCO_GFX");
    if (env) {
        if (str_eq_ci(env, "bgfx")) {
            g_myco_graphics_backend = MYCO_GFX_BACKEND_BGFX;
        } else if (str_eq_ci(env, "sdl")) {
            g_myco_graphics_backend = MYCO_GFX_BACKEND_SDL;
        }
    }
}


