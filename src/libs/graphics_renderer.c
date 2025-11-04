#include "../../include/libs/graphics_renderer.h"
#include "../../include/utils/shared_utilities.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_ttf.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#else
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#endif

typedef struct {
    SDL_GLContext gl_context;
    bool initialized;
    uint32_t clear_color_rgba;
    int width;
    int height;
    
    // Batch rendering state
    GLuint vbo;
    GLuint vao;  // Vertex Array Object for reduced state changes
    GLuint shader_program;
    GLint proj_matrix_location;  // Uniform location for projection matrix
    bool vbo_initialized;
    float* vertex_buffer_mapped;  // Mapped pointer for persistent buffer
    size_t vertex_buffer_size;
    size_t vertex_count;
    size_t vertex_capacity;
    
    // State caching
    GLuint bound_program;
    GLuint bound_vbo;
    bool attribs_enabled;
    
    // Projection matrix (cached, updated on resize)
    float proj_matrix[16];  // 4x4 column-major matrix
    
    // Current rendering state
    uint8_t current_r, current_g, current_b, current_a;
    float current_rf, current_gf, current_bf, current_af;  // Cached float colors
    
    // Text rendering (deferred batching)
    GLuint text_shader_program;
    GLuint text_vao;
    GLuint text_vbo;
    GLuint text_texture;
    GLint text_proj_matrix_location;
    GLint text_texture_location;
    GLint text_color_location;
    float* text_vertex_buffer;
    size_t text_vertex_count;
    size_t text_vertex_capacity;
    
    // Batch state tracking (for efficient batching across multiple drawText calls)
    void* current_text_font;  // Current font being batched
    uint8_t current_text_r, current_text_g, current_text_b, current_text_a;  // Current text color
    bool text_batch_active;  // Whether we have text queued for current font/color
    
    // Multi-batch text support: store completed batches for later rendering
    struct TextBatch {
        void* font;
        uint8_t r, g, b, a;
        float* vertices;
        size_t vertex_count;
    }* text_batches;
    size_t text_batch_count;
    size_t text_batch_capacity;
} GLRenderer;

// Font structure for text rendering
typedef struct {
    TTF_Font* font;
    char* font_path;
    int size;
    GLuint glyph_atlas_texture;  // Texture atlas for ASCII glyphs
    int glyph_widths[128];       // Width of each ASCII glyph
    int glyph_heights[128];       // Height of each ASCII glyph
    float glyph_tex_coords[128][4]; // Texture coordinates for each glyph (x, y, w, h)
    int atlas_width;
    int atlas_height;
    int line_height;
    bool atlas_built;
} GLFont;

static GLRenderer* g_gl_renderer = NULL;

// Font registry
static GLFont** g_fonts = NULL;
static size_t g_font_count = 0;
static size_t g_font_capacity = 0;

// Vertex shader with projection matrix uniform
static const char* vertex_shader_source = 
    "#version 330 core\n"
    "layout (location = 0) in vec2 aPos;\n"
    "layout (location = 1) in vec4 aColor;\n"
    "uniform mat4 uProjMatrix;\n"
    "out vec4 FragColor;\n"
    "void main() {\n"
    "    gl_Position = uProjMatrix * vec4(aPos, 0.0, 1.0);\n"
    "    FragColor = aColor;\n"
    "}\n";

static const char* fragment_shader_source =
    "#version 330 core\n"
    "in vec4 FragColor;\n"
    "out vec4 FragColorOut;\n"
    "void main() {\n"
    "    FragColorOut = FragColor;\n"
    "}\n";

static GLuint compile_shader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar info_log[512];
        glGetShaderInfoLog(shader, 512, NULL, info_log);
        fprintf(stderr, "Shader compilation failed: %s\n", info_log);
        return 0;
    }
    return shader;
}

static GLuint create_shader_program_vf(GLuint vertex_shader, GLuint fragment_shader) {
    if (!vertex_shader || !fragment_shader) return 0;
    
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar info_log[512];
        glGetProgramInfoLog(program, 512, NULL, info_log);
        fprintf(stderr, "Shader linking failed: %s\n", info_log);
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        return 0;
    }
    
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    
    return program;
}

static GLuint create_shader_program(GLint* out_proj_matrix_loc) {
    GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_shader_source);
    GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_shader_source);
    
    if (!vertex_shader || !fragment_shader) return 0;
    
    GLuint program = create_shader_program_vf(vertex_shader, fragment_shader);
    if (!program) return 0;
    
    // Get uniform location for projection matrix
    if (out_proj_matrix_loc) {
        *out_proj_matrix_loc = glGetUniformLocation(program, "uProjMatrix");
    }
    
    return program;
}

// Build orthographic projection matrix (column-major)
static void build_projection_matrix(float* out_matrix, int width, int height) {
    // Orthographic projection: screen coords -> NDC
    // Maps (0,0) to (-1,1) and (width,height) to (1,-1)
    // This is correct for OpenGL where (0,0) is bottom-left in NDC
    // But we want (0,0) at top-left in screen space, so we flip Y
    memset(out_matrix, 0, 16 * sizeof(float));
    out_matrix[0] = 2.0f / width;    // Scale X: maps [0,width] to [-1,1]
    out_matrix[5] = -2.0f / height;  // Scale Y (flip): maps [0,height] to [1,-1]
    out_matrix[10] = 1.0f;            // Z scale
    out_matrix[12] = -1.0f;          // Translate X: maps 0 to -1
    out_matrix[13] = 1.0f;            // Translate Y: maps 0 to 1
    out_matrix[15] = 1.0f;           // W
}

bool myco_renderer_init(MycoRenderer* renderer, struct SDL_Window* sdl_window, int width, int height) {
    if (!sdl_window) return false;
    
    // Create OpenGL context with modern features
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    
    SDL_GLContext gl_context = SDL_GL_CreateContext(sdl_window);
    if (!gl_context) {
        fprintf(stderr, "Failed to create OpenGL context: %s\n", SDL_GetError());
        return false;
    }
    
    // Enable VSync
    SDL_GL_SetSwapInterval(1);
    
    // Initialize GLRenderer
    g_gl_renderer = (GLRenderer*)shared_malloc_safe(sizeof(GLRenderer), "libs", "graphics_renderer", 0);
    if (!g_gl_renderer) {
        SDL_GL_DeleteContext(gl_context);
        return false;
    }
    
    memset(g_gl_renderer, 0, sizeof(GLRenderer));
    g_gl_renderer->gl_context = gl_context;
    g_gl_renderer->width = width;
    g_gl_renderer->height = height;
    g_gl_renderer->clear_color_rgba = 0xFF000000;
    
    // Set up OpenGL state
    glViewport(0, 0, width, height);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    
    // Create shader program
    g_gl_renderer->shader_program = create_shader_program(&g_gl_renderer->proj_matrix_location);
    if (!g_gl_renderer->shader_program) {
        shared_free_safe(g_gl_renderer, "libs", "graphics_renderer", 0);
        SDL_GL_DeleteContext(gl_context);
        return false;
    }
    
    // Build projection matrix
    build_projection_matrix(g_gl_renderer->proj_matrix, width, height);
    
    // Create VAO (Vertex Array Object) for reduced state changes
    glGenVertexArrays(1, &g_gl_renderer->vao);
    glBindVertexArray(g_gl_renderer->vao);
    
    // Create VBO for batched rendering with persistent mapping
    glGenBuffers(1, &g_gl_renderer->vbo);
    g_gl_renderer->vertex_capacity = 4000; // Start with 4K vertices (smaller start)
    g_gl_renderer->vertex_buffer_size = g_gl_renderer->vertex_capacity * 6 * sizeof(float); // x,y + r,g,b,a per vertex
    
    // Create buffer with persistent mapping
    glBindBuffer(GL_ARRAY_BUFFER, g_gl_renderer->vbo);
    // Use GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT if available
    // Fall back to GL_DYNAMIC_DRAW + glMapBufferRange if persistent not supported
    #ifdef GL_MAP_PERSISTENT_BIT
    glBufferStorage(GL_ARRAY_BUFFER, g_gl_renderer->vertex_buffer_size, NULL, 
                    GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);
    g_gl_renderer->vertex_buffer_mapped = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, g_gl_renderer->vertex_buffer_size,
                                                                   GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
    #else
    // Fallback: use dynamic draw and map on demand
    glBufferData(GL_ARRAY_BUFFER, g_gl_renderer->vertex_buffer_size, NULL, GL_DYNAMIC_DRAW);
    g_gl_renderer->vertex_buffer_mapped = NULL; // Will map on flush
    #endif
    
    if (!g_gl_renderer->vertex_buffer_mapped && !g_gl_renderer->vbo_initialized) {
        // Fallback: allocate CPU-side buffer
        g_gl_renderer->vertex_buffer_mapped = (float*)shared_malloc_safe(g_gl_renderer->vertex_buffer_size, "libs", "graphics_renderer", 0);
        if (!g_gl_renderer->vertex_buffer_mapped) {
            glDeleteVertexArrays(1, &g_gl_renderer->vao);
            glDeleteBuffers(1, &g_gl_renderer->vbo);
            glDeleteProgram(g_gl_renderer->shader_program);
            shared_free_safe(g_gl_renderer, "libs", "graphics_renderer", 0);
            SDL_GL_DeleteContext(gl_context);
            return false;
        }
    }
    
    // Set up vertex attributes in VAO (only once)
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
    
    glBindVertexArray(0); // Unbind VAO
    
    // Initialize state caching
    g_gl_renderer->bound_program = 0;
    g_gl_renderer->bound_vbo = 0;
    g_gl_renderer->attribs_enabled = false;
    
    g_gl_renderer->vbo_initialized = true;
    g_gl_renderer->initialized = true;
    
    renderer->device = (void*)gl_context;
    renderer->command_queue = NULL; // Not used for OpenGL
    renderer->layer = NULL; // Not used for OpenGL
    renderer->initialized = true;
    renderer->clear_color_rgba = 0xFF000000;
    renderer->width = width;
    renderer->height = height;
    
    return true;
}

void myco_renderer_shutdown(MycoRenderer* renderer) {
    if (!renderer || !renderer->initialized) return;
    
    if (g_gl_renderer) {
        // Free stored text batches
        if (g_gl_renderer->text_batches) {
            for (size_t i = 0; i < g_gl_renderer->text_batch_count; i++) {
                if (g_gl_renderer->text_batches[i].vertices) {
                    shared_free_safe(g_gl_renderer->text_batches[i].vertices, "libs", "graphics_renderer", 0);
                }
            }
            shared_free_safe(g_gl_renderer->text_batches, "libs", "graphics_renderer", 0);
        }
        
        if (g_gl_renderer->vbo_initialized) {
            // Unmap persistent buffer if it was mapped
            if (g_gl_renderer->vertex_buffer_mapped) {
                // Check if it's a GPU pointer (persistent mapping) or CPU pointer
                // GPU pointers are typically > 0x1000000, CPU pointers are < 0x1000000
                if ((size_t)g_gl_renderer->vertex_buffer_mapped > 0x1000000) {
                    // GPU pointer - unmap it
                    glBindBuffer(GL_ARRAY_BUFFER, g_gl_renderer->vbo);
                    glUnmapBuffer(GL_ARRAY_BUFFER);
                } else {
                    // CPU pointer - free it
                    shared_free_safe(g_gl_renderer->vertex_buffer_mapped, "libs", "graphics_renderer", 0);
                }
            }
            glDeleteBuffers(1, &g_gl_renderer->vbo);
        }
        if (g_gl_renderer->vao) {
            glDeleteVertexArrays(1, &g_gl_renderer->vao);
        }
        if (g_gl_renderer->shader_program) {
            glDeleteProgram(g_gl_renderer->shader_program);
        }
        if (g_gl_renderer->gl_context) {
            SDL_GL_DeleteContext(g_gl_renderer->gl_context);
        }
        shared_free_safe(g_gl_renderer, "libs", "graphics_renderer", 0);
        g_gl_renderer = NULL;
    }
    
    renderer->initialized = false;
}

void myco_renderer_begin_frame(MycoRenderer* renderer) {
    if (!renderer || !renderer->initialized || !g_gl_renderer) return;
    
    // Set viewport (important for correct rendering)
    glViewport(0, 0, g_gl_renderer->width, g_gl_renderer->height);
    
    // Clear screen with stored clear color at start of frame
    uint32_t rgba = g_gl_renderer->clear_color_rgba;
    float r = ((rgba >> 0) & 0xFF) / 255.0f;
    float g = ((rgba >> 8) & 0xFF) / 255.0f;
    float b = ((rgba >> 16) & 0xFF) / 255.0f;
    float a = ((rgba >> 24) & 0xFF) / 255.0f;
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
    
    g_gl_renderer->vertex_count = 0;
    // Reset text batching state for new frame
    g_gl_renderer->text_vertex_count = 0;
    g_gl_renderer->text_batch_active = false;
    g_gl_renderer->current_text_font = NULL;
    
    // Free any stored text batches from previous frame
    if (g_gl_renderer->text_batches) {
        for (size_t i = 0; i < g_gl_renderer->text_batch_count; i++) {
            if (g_gl_renderer->text_batches[i].vertices) {
                shared_free_safe(g_gl_renderer->text_batches[i].vertices, "libs", "graphics_renderer", 0);
            }
        }
        g_gl_renderer->text_batch_count = 0;
    }
}

static void grow_vertex_buffer(void) {
    if (!g_gl_renderer) return;
    
    size_t old_capacity = g_gl_renderer->vertex_capacity;
    size_t new_capacity = (size_t)(old_capacity * 1.5f); // Grow by 1.5x
    size_t new_size = new_capacity * 6 * sizeof(float);
    
    bool was_gpu_mapped = ((size_t)g_gl_renderer->vertex_buffer_mapped > 0x1000000);
    
    if (was_gpu_mapped) {
        // Unmap old GPU buffer
        glBindBuffer(GL_ARRAY_BUFFER, g_gl_renderer->vbo);
        glUnmapBuffer(GL_ARRAY_BUFFER);
        // Recreate with new size (orphan old buffer)
        glBufferData(GL_ARRAY_BUFFER, new_size, NULL, GL_DYNAMIC_DRAW);
        // Remap
        g_gl_renderer->vertex_buffer_mapped = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, new_size,
                                                                       GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
        // If remapping fails, fall back to CPU buffer
        if (!g_gl_renderer->vertex_buffer_mapped) {
            g_gl_renderer->vertex_buffer_mapped = (float*)shared_malloc_safe(new_size, "libs", "graphics_renderer", 0);
        }
    } else {
        // CPU buffer - free old, allocate new
        if (g_gl_renderer->vertex_buffer_mapped) {
            shared_free_safe(g_gl_renderer->vertex_buffer_mapped, "libs", "graphics_renderer", 0);
        }
        // Orphan old buffer and create new one
        glBindBuffer(GL_ARRAY_BUFFER, g_gl_renderer->vbo);
        glBufferData(GL_ARRAY_BUFFER, new_size, NULL, GL_DYNAMIC_DRAW);
        // Allocate new CPU buffer
        g_gl_renderer->vertex_buffer_mapped = (float*)shared_malloc_safe(new_size, "libs", "graphics_renderer", 0);
    }
    
    if (!g_gl_renderer->vertex_buffer_mapped) {
        // Failed to allocate - keep old buffer, just log warning
        fprintf(stderr, "Warning: Failed to grow vertex buffer, keeping old size\n");
        return;
    }
    
    g_gl_renderer->vertex_capacity = new_capacity;
    g_gl_renderer->vertex_buffer_size = new_size;
}

static void flush_vertices(void) {
    if (!g_gl_renderer) return;
    
    if (g_gl_renderer->vertex_count == 0) {
        // No primitives to draw - this is normal if only text was rendered
        return;
    }
    
    // Always bind program and set uniforms
    glUseProgram(g_gl_renderer->shader_program);
    glUniformMatrix4fv(g_gl_renderer->proj_matrix_location, 1, GL_FALSE, g_gl_renderer->proj_matrix);
    
    // Ensure viewport is set (might have been changed by other code)
    glViewport(0, 0, g_gl_renderer->width, g_gl_renderer->height);
    
    // Ensure blending is enabled (might have been disabled)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Disable depth test (we're doing 2D rendering)
    glDisable(GL_DEPTH_TEST);
    
    // Bind VAO (handles vertex attribs automatically)
    glBindVertexArray(g_gl_renderer->vao);
    
    // Always bind buffer (VAO remembers attributes but not buffer binding)
    glBindBuffer(GL_ARRAY_BUFFER, g_gl_renderer->vbo);
    
    // Upload vertex data to GPU
    // For persistent mapped buffers, we need to ensure data is visible to GPU
    // For CPU-side buffers, we upload with glBufferSubData
    if (g_gl_renderer->vertex_buffer_mapped) {
        // Check if it's a CPU-side buffer (low address) or GPU-mapped buffer
        if ((size_t)g_gl_renderer->vertex_buffer_mapped < 0x1000000) {
            // CPU-side buffer: upload to GPU
            glBufferSubData(GL_ARRAY_BUFFER, 0, g_gl_renderer->vertex_count * 6 * sizeof(float), g_gl_renderer->vertex_buffer_mapped);
        } else {
            // GPU-mapped buffer: ensure data is visible (flush memory barrier)
            // For persistent mapped buffers, we may need a memory barrier
            // But first, let's try uploading anyway to be safe
            glBufferSubData(GL_ARRAY_BUFFER, 0, g_gl_renderer->vertex_count * 6 * sizeof(float), g_gl_renderer->vertex_buffer_mapped);
        }
    }
    
    // Draw - VAO already has all state set up
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)g_gl_renderer->vertex_count);
    
    // Reset vertex count after drawing
    g_gl_renderer->vertex_count = 0;
}

static void add_vertex(float x, float y, float r, float g, float b, float a) {
    if (!g_gl_renderer) return;
    
    // Grow buffer if needed
    if (g_gl_renderer->vertex_count >= g_gl_renderer->vertex_capacity) {
        grow_vertex_buffer();
    }
    
    // Write directly to mapped buffer (persistent mapping) or CPU buffer
    size_t idx = g_gl_renderer->vertex_count * 6;
    if (g_gl_renderer->vertex_buffer_mapped) {
        g_gl_renderer->vertex_buffer_mapped[idx + 0] = x;
        g_gl_renderer->vertex_buffer_mapped[idx + 1] = y;
        g_gl_renderer->vertex_buffer_mapped[idx + 2] = r;
        g_gl_renderer->vertex_buffer_mapped[idx + 3] = g;
        g_gl_renderer->vertex_buffer_mapped[idx + 4] = b;
        g_gl_renderer->vertex_buffer_mapped[idx + 5] = a;
    }
    g_gl_renderer->vertex_count++;
}

// Flush accumulated text (batched by font/color)
static void flush_text_batch(void) {
    if (!g_gl_renderer) return;
    if (g_gl_renderer->text_vertex_count == 0) return;
    if (!g_gl_renderer->current_text_font) return;
    
    
    size_t font_id = (size_t)g_gl_renderer->current_text_font;
    if (font_id >= g_font_count || !g_fonts[font_id]) return;
    
    GLFont* gl_font = g_fonts[font_id];
    
    // Set up text shader and state
    if (!g_gl_renderer->text_shader_program) return;
    
    glUseProgram(g_gl_renderer->text_shader_program);
    
    // Ensure viewport is set correctly for text rendering
    glViewport(0, 0, g_gl_renderer->width, g_gl_renderer->height);
    
    // Ensure blending is enabled and configured correctly
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Disable depth test (2D rendering)
    glDisable(GL_DEPTH_TEST);
    
    glUniformMatrix4fv(g_gl_renderer->text_proj_matrix_location, 1, GL_FALSE, g_gl_renderer->proj_matrix);
    glUniform1i(g_gl_renderer->text_texture_location, 0);
    glUniform4f(g_gl_renderer->text_color_location, 
                g_gl_renderer->current_text_r/255.0f, 
                g_gl_renderer->current_text_g/255.0f, 
                g_gl_renderer->current_text_b/255.0f, 
                g_gl_renderer->current_text_a/255.0f);
    
    // Bind atlas texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gl_font->glyph_atlas_texture);
    
    // Upload and draw batched text
    glBindVertexArray(g_gl_renderer->text_vao);
    glBindBuffer(GL_ARRAY_BUFFER, g_gl_renderer->text_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 
                    g_gl_renderer->text_vertex_count * 4 * sizeof(float),
                    g_gl_renderer->text_vertex_buffer);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)g_gl_renderer->text_vertex_count);
    
    // Unbind VAO after drawing
    glBindVertexArray(0);
    
    // Reset batch state AFTER drawing
    g_gl_renderer->text_vertex_count = 0;
    g_gl_renderer->text_batch_active = false;
    // DO NOT clear current_text_font here - it may be needed for the next batch in the same frame
}

void myco_renderer_end_frame(MycoRenderer* renderer) {
    if (!renderer || !renderer->initialized || !g_gl_renderer) return;
    
    // IMPORTANT: Always flush primitives first (they render first, text will be on top)
    // This ensures all primitives (background, grid, snake, food) are drawn together
    // Even if vertex_count is 0, this is a no-op, so it's safe to call
    flush_vertices();
    
    // Flush all stored text batches (renders on top of primitives)
    // These are batches that were stored when color changed mid-frame
    if (g_gl_renderer->text_batches && g_gl_renderer->text_batch_count > 0) {
        for (size_t i = 0; i < g_gl_renderer->text_batch_count; i++) {
            struct TextBatch* batch = &g_gl_renderer->text_batches[i];
            if (!batch->vertices || batch->vertex_count == 0) continue;
            
            // Set up text shader and state
            if (!g_gl_renderer->text_shader_program) continue;
            glUseProgram(g_gl_renderer->text_shader_program);
            
            glViewport(0, 0, g_gl_renderer->width, g_gl_renderer->height);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDisable(GL_DEPTH_TEST);
            
            glUniformMatrix4fv(g_gl_renderer->text_proj_matrix_location, 1, GL_FALSE, g_gl_renderer->proj_matrix);
            glUniform1i(g_gl_renderer->text_texture_location, 0);
            glUniform4f(g_gl_renderer->text_color_location, 
                        batch->r/255.0f, batch->g/255.0f, batch->b/255.0f, batch->a/255.0f);
            
            // Get font and bind atlas
            size_t font_id = (size_t)batch->font;
            if (font_id >= g_font_count || !g_fonts[font_id]) continue;
            GLFont* gl_font = g_fonts[font_id];
            
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, gl_font->glyph_atlas_texture);
            
            // Upload and draw
            glBindVertexArray(g_gl_renderer->text_vao);
            glBindBuffer(GL_ARRAY_BUFFER, g_gl_renderer->text_vbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, 
                            batch->vertex_count * 4 * sizeof(float),
                            batch->vertices);
            glDrawArrays(GL_TRIANGLES, 0, (GLsizei)batch->vertex_count);
            glBindVertexArray(0);
        }
    }
    
    // Flush any remaining accumulated text (current batch)
    // This is the text that was drawn after the last color change
    flush_text_batch();
    
    // IMPORTANT: Restore primitive shader state after text rendering
    // This ensures that if any code checks OpenGL state after end_frame(),
    // it will see the correct shader program bound
    if (g_gl_renderer->shader_program) {
        glUseProgram(g_gl_renderer->shader_program);
    }
    
    // Swap buffers (caller will do SDL_GL_SwapWindow)
}

void myco_renderer_set_clear_color(MycoRenderer* renderer, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    if (!renderer || !renderer->initialized) return;
    renderer->clear_color_rgba = ((uint32_t)a << 24) | ((uint32_t)b << 16) | ((uint32_t)g << 8) | (uint32_t)r;
    if (g_gl_renderer) {
        g_gl_renderer->clear_color_rgba = renderer->clear_color_rgba;
        g_gl_renderer->current_r = r;
        g_gl_renderer->current_g = g;
        g_gl_renderer->current_b = b;
        g_gl_renderer->current_a = a;
        // Cache float colors (avoid division every vertex)
        g_gl_renderer->current_rf = r / 255.0f;
        g_gl_renderer->current_gf = g / 255.0f;
        g_gl_renderer->current_bf = b / 255.0f;
        g_gl_renderer->current_af = a / 255.0f;
    }
}

void myco_renderer_clear(MycoRenderer* renderer) {
    if (!renderer || !renderer->initialized || !g_gl_renderer) return;
    
    // DO NOT flush primitives here - clear() is called mid-frame to set background color
    // Primitives will be flushed at end_frame() after all drawing is done
    
    // Just clear the screen with current clear color
    uint32_t rgba = g_gl_renderer->clear_color_rgba;
    float r = ((rgba >> 0) & 0xFF) / 255.0f;
    float g = ((rgba >> 8) & 0xFF) / 255.0f;
    float b = ((rgba >> 16) & 0xFF) / 255.0f;
    float a = ((rgba >> 24) & 0xFF) / 255.0f;
    
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // DO NOT reset vertex_count here - allow primitives to accumulate across the frame
    // vertex_count is reset in begin_frame() at the start of each frame
}

void myco_renderer_draw_rect_filled(MycoRenderer* renderer, int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    if (!renderer || !renderer->initialized || !g_gl_renderer) return;
    
    // Check for invalid dimensions (might indicate game not calculating properly)
    if (w <= 0 || h <= 0) {
        return;
    }
    
    // Use cached float colors (converted once per set_color call)
    float rf = r / 255.0f, gf = g / 255.0f, bf = b / 255.0f, af = a / 255.0f;
    
    // Use screen coordinates directly - projection matrix handles NDC conversion in shader
    float x1 = (float)x;
    float y1 = (float)y;
    float x2 = (float)(x + w);
    float y2 = (float)(y + h);
    
    // Draw as two triangles
    add_vertex(x1, y1, rf, gf, bf, af);
    add_vertex(x2, y1, rf, gf, bf, af);
    add_vertex(x1, y2, rf, gf, bf, af);
    
    add_vertex(x1, y2, rf, gf, bf, af);
    add_vertex(x2, y1, rf, gf, bf, af);
    add_vertex(x2, y2, rf, gf, bf, af);
}

void myco_renderer_draw_rect_outline(MycoRenderer* renderer, int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    // Draw as 4 lines
    myco_renderer_draw_line(renderer, x, y, x + w, y, r, g, b, a);
    myco_renderer_draw_line(renderer, x + w, y, x + w, y + h, r, g, b, a);
    myco_renderer_draw_line(renderer, x + w, y + h, x, y + h, r, g, b, a);
    myco_renderer_draw_line(renderer, x, y + h, x, y, r, g, b, a);
}

void myco_renderer_draw_line(MycoRenderer* renderer, int x1, int y1, int x2, int y2, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    if (!renderer || !renderer->initialized || !g_gl_renderer) return;
    
    float rf = r / 255.0f, gf = g / 255.0f, bf = b / 255.0f, af = a / 255.0f;
    
    // Use screen coordinates directly - projection matrix handles conversion
    float fx1 = (float)x1;
    float fy1 = (float)y1;
    float fx2 = (float)x2;
    float fy2 = (float)y2;
    
    // Draw line as two triangles (thick line approximation - 1 pixel)
    // Calculate perpendicular offset for thickness
    float dx = fx2 - fx1;
    float dy = fy2 - fy1;
    float len = sqrtf(dx * dx + dy * dy);
    if (len < 0.001f) return; // Degenerate line
    
    float perp_x = -dy / len * 0.5f; // Half-thickness in screen space
    float perp_y = dx / len * 0.5f;
    
    add_vertex(fx1 + perp_x, fy1 + perp_y, rf, gf, bf, af);
    add_vertex(fx2 + perp_x, fy2 + perp_y, rf, gf, bf, af);
    add_vertex(fx1 - perp_x, fy1 - perp_y, rf, gf, bf, af);
    
    add_vertex(fx1 - perp_x, fy1 - perp_y, rf, gf, bf, af);
    add_vertex(fx2 + perp_x, fy2 + perp_y, rf, gf, bf, af);
    add_vertex(fx2 - perp_x, fy2 - perp_y, rf, gf, bf, af);
}

void myco_renderer_draw_circle_filled(MycoRenderer* renderer, int cx, int cy, int radius, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    if (!renderer || !renderer->initialized || !g_gl_renderer || radius <= 0) {
        return;
    }
    
    float rf = r / 255.0f, gf = g / 255.0f, bf = b / 255.0f, af = a / 255.0f;
    
    // Use screen coordinates directly
    float center_x = (float)cx;
    float center_y = (float)cy;
    float radius_f = (float)radius;
    
    // Adaptive segments based on radius (smaller circles need fewer segments)
    int segments = radius > 50 ? 32 : (radius > 20 ? 16 : 8);
    
    for (int i = 0; i < segments; i++) {
        float angle1 = (2.0f * M_PI * i) / segments;
        float angle2 = (2.0f * M_PI * (i + 1)) / segments;
        
        float x1 = center_x + radius_f * cosf(angle1);
        float y1 = center_y + radius_f * sinf(angle1);
        float x2 = center_x + radius_f * cosf(angle2);
        float y2 = center_y + radius_f * sinf(angle2);
        
        add_vertex(center_x, center_y, rf, gf, bf, af);
        add_vertex(x1, y1, rf, gf, bf, af);
        add_vertex(x2, y2, rf, gf, bf, af);
    }
}

void myco_renderer_draw_circle_outline(MycoRenderer* renderer, int cx, int cy, int radius, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    if (!renderer || !renderer->initialized || !g_gl_renderer || radius <= 0) return;
    
    const int segments = 64;
    for (int i = 0; i < segments; i++) {
        float angle1 = (2.0f * M_PI * i) / segments;
        float angle2 = (2.0f * M_PI * (i + 1)) / segments;
        
        int x1 = cx + (int)(radius * cosf(angle1));
        int y1 = cy + (int)(radius * sinf(angle1));
        int x2 = cx + (int)(radius * cosf(angle2));
        int y2 = cy + (int)(radius * sinf(angle2));
        
        myco_renderer_draw_line(renderer, x1, y1, x2, y2, r, g, b, a);
    }
}

// Text rendering shaders
static const char* text_vertex_shader_source = 
    "#version 330 core\n"
    "layout (location = 0) in vec2 aPos;\n"
    "layout (location = 1) in vec2 aTexCoord;\n"
    "uniform mat4 uProjMatrix;\n"
    "out vec2 TexCoord;\n"
    "void main() {\n"
    "    gl_Position = uProjMatrix * vec4(aPos, 0.0, 1.0);\n"
    "    TexCoord = aTexCoord;\n"
    "}\n";

static const char* text_fragment_shader_source =
    "#version 330 core\n"
    "in vec2 TexCoord;\n"
    "uniform sampler2D uTexture;\n"
    "uniform vec4 uColor;\n"
    "out vec4 FragColor;\n"
    "void main() {\n"
    "    vec4 texColor = texture(uTexture, TexCoord);\n"
    "    // Use uniform color for RGB, texture alpha for transparency\n"
    "    // Texture is white glyphs, so we only need alpha channel\n"
    "    FragColor = vec4(uColor.rgb, texColor.a * uColor.a);\n"
    "}\n";

// Build glyph atlas for a font (ASCII 32-126)
static bool build_glyph_atlas(GLFont* gl_font) {
    if (!gl_font || !gl_font->font) return false;
    if (gl_font->atlas_built) return true;
    
    TTF_Font* font = gl_font->font;
    int max_glyph_w = 0, max_glyph_h = 0;
    int min_y = 0, max_y = 0;
    
    // Measure all ASCII glyphs
    for (int c = 32; c < 127; c++) {
        int w, h;
        if (TTF_SizeText(font, (char[]){c, 0}, &w, &h) == 0) {
            if (w > max_glyph_w) max_glyph_w = w;
            if (h > max_glyph_h) max_glyph_h = h;
        }
        int miny, maxy, adv;
        if (TTF_GlyphMetrics(font, c, NULL, NULL, &miny, &maxy, &adv) == 0) {
            if (miny < min_y) min_y = miny;
            if (maxy > max_y) max_y = maxy;
        }
    }
    
    gl_font->line_height = max_y - min_y;
    if (gl_font->line_height <= 0) gl_font->line_height = max_glyph_h;
    
    // Create atlas texture (16x8 grid)
    int cols = 16, rows = 8;
    int cell_w = max_glyph_w + 2, cell_h = max_glyph_h + 2;
    int atlas_w = cols * cell_w, atlas_h = rows * cell_h;
    
    SDL_Surface* atlas_surface = SDL_CreateRGBSurface(0, atlas_w, atlas_h, 32,
                                                       0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
    if (!atlas_surface) return false;
    
    SDL_FillRect(atlas_surface, NULL, SDL_MapRGBA(atlas_surface->format, 0, 0, 0, 0));
    
    SDL_Color white = {255, 255, 255, 255};
    for (int c = 32; c < 127; c++) {
        SDL_Surface* glyph_surface = TTF_RenderGlyph_Blended(font, c, white);
        if (!glyph_surface) continue;
        
        int col = (c - 32) % cols, row = (c - 32) / cols;
        int x = col * cell_w + 1, y = row * cell_h + 1;
        
        SDL_Rect dest_rect = {x, y, glyph_surface->w, glyph_surface->h};
        SDL_BlitSurface(glyph_surface, NULL, atlas_surface, &dest_rect);
        
        gl_font->glyph_widths[c] = glyph_surface->w;
        gl_font->glyph_heights[c] = glyph_surface->h;
        gl_font->glyph_tex_coords[c][0] = (float)x / atlas_w;
        gl_font->glyph_tex_coords[c][1] = (float)y / atlas_h;
        gl_font->glyph_tex_coords[c][2] = (float)glyph_surface->w / atlas_w;
        gl_font->glyph_tex_coords[c][3] = (float)glyph_surface->h / atlas_h;
        
        SDL_FreeSurface(glyph_surface);
    }
    
    glGenTextures(1, &gl_font->glyph_atlas_texture);
    glBindTexture(GL_TEXTURE_2D, gl_font->glyph_atlas_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, atlas_w, atlas_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, atlas_surface->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    SDL_FreeSurface(atlas_surface);
    gl_font->atlas_width = atlas_w;
    gl_font->atlas_height = atlas_h;
    gl_font->atlas_built = true;
    return true;
}

// Load font and create atlas
void* myco_renderer_load_font(MycoRenderer* renderer, const char* font_path, int size) {
    (void)renderer;
    if (!font_path || size <= 0) return NULL;
    
    TTF_Font* font = TTF_OpenFont(font_path, size);
    if (!font) return NULL;
    
    GLFont* gl_font = (GLFont*)shared_malloc_safe(sizeof(GLFont), "libs", "graphics_renderer", 0);
    if (!gl_font) {
        TTF_CloseFont(font);
        return NULL;
    }
    
    memset(gl_font, 0, sizeof(GLFont));
    gl_font->font = font;
    gl_font->font_path = shared_strdup(font_path);
    gl_font->size = size;
    
    if (!build_glyph_atlas(gl_font)) {
        shared_free_safe(gl_font->font_path, "libs", "graphics_renderer", 0);
        TTF_CloseFont(font);
        shared_free_safe(gl_font, "libs", "graphics_renderer", 0);
        return NULL;
    }
    
    // Register font
    if (g_font_count >= g_font_capacity) {
        size_t new_cap = g_font_capacity == 0 ? 4 : g_font_capacity * 2;
        GLFont** new_fonts = (GLFont**)shared_realloc_safe(g_fonts, new_cap * sizeof(GLFont*), "libs", "graphics_renderer", 0);
        if (!new_fonts) {
            glDeleteTextures(1, &gl_font->glyph_atlas_texture);
            shared_free_safe(gl_font->font_path, "libs", "graphics_renderer", 0);
            TTF_CloseFont(font);
            shared_free_safe(gl_font, "libs", "graphics_renderer", 0);
            return NULL;
        }
        g_fonts = new_fonts;
        g_font_capacity = new_cap;
    }
    
    g_fonts[g_font_count] = gl_font;
    return (void*)(g_font_count++);
}

void myco_renderer_unload_font(MycoRenderer* renderer, void* font_handle) {
    (void)renderer;
    if (!font_handle) return;
    
    size_t font_id = (size_t)font_handle;
    if (font_id >= g_font_count || !g_fonts[font_id]) return;
    
    GLFont* gl_font = g_fonts[font_id];
    if (gl_font->glyph_atlas_texture) {
        glDeleteTextures(1, &gl_font->glyph_atlas_texture);
    }
    if (gl_font->font) {
        TTF_CloseFont(gl_font->font);
    }
    shared_free_safe(gl_font->font_path, "libs", "graphics_renderer", 0);
    shared_free_safe(gl_font, "libs", "graphics_renderer", 0);
    g_fonts[font_id] = NULL;
}

void myco_renderer_draw_text(MycoRenderer* renderer, void* font_handle, const char* text, int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    if (!renderer || !renderer->initialized || !g_gl_renderer || !font_handle || !text) return;
    
    size_t font_id = (size_t)font_handle;
    if (font_id >= g_font_count || !g_fonts[font_id] || !g_fonts[font_id]->atlas_built) {
        return;
    }
    
    
    GLFont* gl_font = g_fonts[font_id];
    
    // Create text shader on first use
    if (!g_gl_renderer->text_shader_program) {
        GLuint vs = compile_shader(GL_VERTEX_SHADER, text_vertex_shader_source);
        GLuint fs = compile_shader(GL_FRAGMENT_SHADER, text_fragment_shader_source);
        if (!vs || !fs) return;
        
        g_gl_renderer->text_shader_program = create_shader_program_vf(vs, fs);
        if (g_gl_renderer->text_shader_program) {
            g_gl_renderer->text_proj_matrix_location = glGetUniformLocation(g_gl_renderer->text_shader_program, "uProjMatrix");
            g_gl_renderer->text_texture_location = glGetUniformLocation(g_gl_renderer->text_shader_program, "uTexture");
            g_gl_renderer->text_color_location = glGetUniformLocation(g_gl_renderer->text_shader_program, "uColor");
        }
    }
    
    if (!g_gl_renderer->text_shader_program) return;
    
    // Initialize text rendering buffers on first use
    if (!g_gl_renderer->text_vao) {
        glGenVertexArrays(1, &g_gl_renderer->text_vao);
        glGenBuffers(1, &g_gl_renderer->text_vbo);
        g_gl_renderer->text_vertex_capacity = 4000; // 4000 quads = 24000 vertices (larger for better batching)
        g_gl_renderer->text_vertex_buffer = (float*)shared_malloc_safe(
            g_gl_renderer->text_vertex_capacity * 6 * 4 * sizeof(float), // x,y,tex_x,tex_y per vertex
            "libs", "graphics_renderer", 0);
        if (!g_gl_renderer->text_vertex_buffer) return;
        
        glBindVertexArray(g_gl_renderer->text_vao);
        glBindBuffer(GL_ARRAY_BUFFER, g_gl_renderer->text_vbo);
        glBufferData(GL_ARRAY_BUFFER, 
                     g_gl_renderer->text_vertex_capacity * 6 * 4 * sizeof(float),
                     NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glBindVertexArray(0);
    }
    
    // Check if font or color changed - need to flush current text batch and start new one
    bool font_changed = (g_gl_renderer->current_text_font != font_handle);
    bool color_changed = (g_gl_renderer->current_text_r != r || 
                         g_gl_renderer->current_text_g != g || 
                         g_gl_renderer->current_text_b != b || 
                         g_gl_renderer->current_text_a != a);
    
    // If we have an active text batch with different font/color, store it for later rendering
    // IMPORTANT: We DO NOT flush primitives or text here - everything accumulates until end_frame()
    // This ensures correct render order: all primitives first, then all text on top
    if (g_gl_renderer->text_batch_active && (font_changed || color_changed) && g_gl_renderer->text_vertex_count > 0) {
        // Store the current batch for later rendering at end_frame()
        if (!g_gl_renderer->text_batches) {
            g_gl_renderer->text_batch_capacity = 8;
            g_gl_renderer->text_batches = (struct TextBatch*)shared_malloc_safe(
                sizeof(struct TextBatch) * g_gl_renderer->text_batch_capacity,
                "libs", "graphics_renderer", 0);
            if (!g_gl_renderer->text_batches) return;
        }
        
        if (g_gl_renderer->text_batch_count >= g_gl_renderer->text_batch_capacity) {
            size_t new_capacity = g_gl_renderer->text_batch_capacity * 2;
            struct TextBatch* new_batches = (struct TextBatch*)shared_malloc_safe(
                sizeof(struct TextBatch) * new_capacity,
                "libs", "graphics_renderer", 0);
            if (!new_batches) return;
            memcpy(new_batches, g_gl_renderer->text_batches, 
                   sizeof(struct TextBatch) * g_gl_renderer->text_batch_count);
            shared_free_safe(g_gl_renderer->text_batches, "libs", "graphics_renderer", 0);
            g_gl_renderer->text_batches = new_batches;
            g_gl_renderer->text_batch_capacity = new_capacity;
        }
        
        // Copy current batch data
        struct TextBatch* batch = &g_gl_renderer->text_batches[g_gl_renderer->text_batch_count++];
        batch->font = g_gl_renderer->current_text_font;
        batch->r = g_gl_renderer->current_text_r;
        batch->g = g_gl_renderer->current_text_g;
        batch->b = g_gl_renderer->current_text_b;
        batch->a = g_gl_renderer->current_text_a;
        batch->vertex_count = g_gl_renderer->text_vertex_count;
        
        // Allocate and copy vertex data
        size_t vertex_size = g_gl_renderer->text_vertex_count * 4 * sizeof(float);
        batch->vertices = (float*)shared_malloc_safe(vertex_size, "libs", "graphics_renderer", 0);
        if (batch->vertices) {
            memcpy(batch->vertices, g_gl_renderer->text_vertex_buffer, vertex_size);
        }
        
        // Reset current batch for new color
        g_gl_renderer->text_vertex_count = 0;
    }
    
    // Start new batch if needed
    // Note: When starting the first text batch (text_batch_active is false), we DON'T flush primitives
    // because we want all primitives to accumulate until end_frame() for correct batching
    if (!g_gl_renderer->text_batch_active || font_changed || color_changed) {
        g_gl_renderer->text_vertex_count = 0;
        g_gl_renderer->current_text_font = font_handle;
        g_gl_renderer->current_text_r = r;
        g_gl_renderer->current_text_g = g;
        g_gl_renderer->current_text_b = b;
        g_gl_renderer->current_text_a = a;
        g_gl_renderer->text_batch_active = true;
    }
    
    // Add text glyphs to batch (deferred - no immediate draw)
    float fx = (float)x;
    float fy = (float)y;
    
    for (const char* p = text; *p; p++) {
        unsigned char c = (unsigned char)*p;
        if (c < 32 || c >= 127) continue;
        
        int glyph_w = gl_font->glyph_widths[c];
        int glyph_h = gl_font->glyph_heights[c];
        if (glyph_w <= 0 || glyph_h <= 0) continue;
        
        float* tex_coords = gl_font->glyph_tex_coords[c];
        float x1 = fx, y1 = fy;
        float x2 = fx + glyph_w, y2 = fy + glyph_h;
        
        // Check capacity - flush and continue if full
        if (g_gl_renderer->text_vertex_count + 6 > g_gl_renderer->text_vertex_capacity) {
            flush_text_batch();
            // Restart batch with same font/color
            g_gl_renderer->current_text_font = font_handle;
            g_gl_renderer->current_text_r = r;
            g_gl_renderer->current_text_g = g;
            g_gl_renderer->current_text_b = b;
            g_gl_renderer->current_text_a = a;
            g_gl_renderer->text_vertex_count = 0;
            g_gl_renderer->text_batch_active = true;
        }
        
        size_t idx = g_gl_renderer->text_vertex_count * 4;
        // Triangle 1
        g_gl_renderer->text_vertex_buffer[idx + 0] = x1; g_gl_renderer->text_vertex_buffer[idx + 1] = y1;
        g_gl_renderer->text_vertex_buffer[idx + 2] = tex_coords[0]; g_gl_renderer->text_vertex_buffer[idx + 3] = tex_coords[1];
        idx += 4;
        g_gl_renderer->text_vertex_buffer[idx + 0] = x2; g_gl_renderer->text_vertex_buffer[idx + 1] = y1;
        g_gl_renderer->text_vertex_buffer[idx + 2] = tex_coords[0] + tex_coords[2]; g_gl_renderer->text_vertex_buffer[idx + 3] = tex_coords[1];
        idx += 4;
        g_gl_renderer->text_vertex_buffer[idx + 0] = x1; g_gl_renderer->text_vertex_buffer[idx + 1] = y2;
        g_gl_renderer->text_vertex_buffer[idx + 2] = tex_coords[0]; g_gl_renderer->text_vertex_buffer[idx + 3] = tex_coords[1] + tex_coords[3];
        idx += 4;
        // Triangle 2
        g_gl_renderer->text_vertex_buffer[idx + 0] = x1; g_gl_renderer->text_vertex_buffer[idx + 1] = y2;
        g_gl_renderer->text_vertex_buffer[idx + 2] = tex_coords[0]; g_gl_renderer->text_vertex_buffer[idx + 3] = tex_coords[1] + tex_coords[3];
        idx += 4;
        g_gl_renderer->text_vertex_buffer[idx + 0] = x2; g_gl_renderer->text_vertex_buffer[idx + 1] = y1;
        g_gl_renderer->text_vertex_buffer[idx + 2] = tex_coords[0] + tex_coords[2]; g_gl_renderer->text_vertex_buffer[idx + 3] = tex_coords[1];
        idx += 4;
        g_gl_renderer->text_vertex_buffer[idx + 0] = x2; g_gl_renderer->text_vertex_buffer[idx + 1] = y2;
        g_gl_renderer->text_vertex_buffer[idx + 2] = tex_coords[0] + tex_coords[2]; g_gl_renderer->text_vertex_buffer[idx + 3] = tex_coords[1] + tex_coords[3];
        
        g_gl_renderer->text_vertex_count += 6;
        fx += glyph_w;
    }
    
    // Note: Don't flush here - let it accumulate across multiple drawText calls
    // Flush happens automatically in end_frame() or when font/color changes
}

void myco_renderer_measure_text(MycoRenderer* renderer, void* font_handle, const char* text, int* out_width, int* out_height) {
    if (!renderer || !font_handle || !text) {
        if (out_width) *out_width = 0;
        if (out_height) *out_height = 0;
        return;
    }
    
    size_t font_id = (size_t)font_handle;
    if (font_id >= g_font_count || !g_fonts[font_id]) {
        if (out_width) *out_width = 0;
        if (out_height) *out_height = 0;
        return;
    }
    
    GLFont* gl_font = g_fonts[font_id];
    int w = 0, h = gl_font->line_height;
    
    for (const char* p = text; *p; p++) {
        unsigned char c = (unsigned char)*p;
        if (c >= 32 && c < 127) {
            w += gl_font->glyph_widths[c];
        }
    }
    
    if (out_width) *out_width = w;
    if (out_height) *out_height = h;
}

void myco_renderer_resize(MycoRenderer* renderer, int width, int height) {
    if (!renderer || !renderer->initialized || !g_gl_renderer) return;
    renderer->width = width;
    renderer->height = height;
    g_gl_renderer->width = width;
    g_gl_renderer->height = height;
    glViewport(0, 0, width, height);
    
    // Rebuild projection matrix for new size
    build_projection_matrix(g_gl_renderer->proj_matrix, width, height);
    
    // Update uniform if program is bound (will update on next flush)
    if (g_gl_renderer->bound_program == g_gl_renderer->shader_program) {
        glUseProgram(g_gl_renderer->shader_program);
        glUniformMatrix4fv(g_gl_renderer->proj_matrix_location, 1, GL_FALSE, g_gl_renderer->proj_matrix);
    }
}