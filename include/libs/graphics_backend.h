#ifndef MYCO_GRAPHICS_BACKEND_H
#define MYCO_GRAPHICS_BACKEND_H

// Lightweight backend switch to prepare for bgfx integration.
// Default remains SDL; setting MYCO_USE_BGFX enables bgfx path (when implemented).

typedef enum MycoGraphicsBackendKind {
    MYCO_GFX_BACKEND_SDL = 0,
    MYCO_GFX_BACKEND_BGFX = 1
} MycoGraphicsBackendKind;

// Global selector (read-only for most code). Implementation provides storage.
extern MycoGraphicsBackendKind g_myco_graphics_backend;

// Set backend at startup based on compile flags or env var; safe to call multiple times.
void myco_graphics_backend_select_auto(void);

#endif // MYCO_GRAPHICS_BACKEND_H


