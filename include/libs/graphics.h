#ifndef MYCO_GRAPHICS_H
#define MYCO_GRAPHICS_H

#include "../core/interpreter.h"

// Graphics library functions
Value builtin_graphics_create_window(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_graphics_close_window(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_graphics_is_open(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_graphics_poll_events(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_graphics_clear(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_graphics_present(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_graphics_set_color(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_graphics_draw_rect(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_graphics_draw_line(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_graphics_draw_circle(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_graphics_fill_rect(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_graphics_fill_circle(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_graphics_get_width(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_graphics_get_height(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_graphics_set_title(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_graphics_get_key(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_graphics_load_font(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_graphics_draw_text(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_graphics_draw_text_lines(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_graphics_draw_text_lines_from_array(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_graphics_measure_text(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_graphics_set_default_font(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_graphics_clear_text_cache(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
// Draw a dynamic status bar without polluting the text cache.
// Args: filename (string), line (number), col (number), total_lines (number), frame (number), fps (number), cursor_visible (boolean)
Value builtin_graphics_draw_status_bar(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);

// PreparedText API: create once, draw many times
// prepareText(text[, font_id, r, g, b, a]) -> handle (number)
Value builtin_graphics_prepare_text(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
// drawPreparedText(handle, x, y)
Value builtin_graphics_draw_prepared_text(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
// freePreparedText(handle)
Value builtin_graphics_free_prepared_text(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);

// Window object methods
Value builtin_window_close(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_window_is_open(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_window_poll_events(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_window_clear(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_window_present(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_window_set_color(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_window_draw_rect(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_window_draw_line(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_window_draw_circle(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_window_fill_rect(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_window_fill_circle(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_window_get_width(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_window_get_height(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_window_set_title(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);

// Graphics library registration
void graphics_library_register(Interpreter* interpreter);

#endif // MYCO_GRAPHICS_H

