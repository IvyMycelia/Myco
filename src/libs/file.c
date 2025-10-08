#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include "../../include/core/interpreter.h"
#include "../../include/core/ast.h"
#include "../../include/core/standardized_errors.h"
#include "../../include/utils/shared_utilities.h"

// File handle structure for stream operations
typedef struct {
    FILE* file;
    char* filename;
    char* mode;
    int is_open;
    long position;
    long size;
} FileHandle;

// File handle management
static FileHandle* file_handles = NULL;
static size_t file_handle_count = 0;
static size_t file_handle_capacity = 0;

// File handle functions
static FileHandle* file_handle_create(FILE* file, const char* filename, const char* mode) {
    FileHandle* handle = shared_malloc_safe(sizeof(FileHandle), "file", "unknown_function", 28);
    if (!handle) return NULL;
    
    handle->file = file;
    handle->filename = filename ? (filename ? strdup(filename) : NULL) : NULL;
    handle->mode = mode ? (mode ? strdup(mode) : NULL) : NULL;
    handle->is_open = 1;
    handle->position = 0;
    
    // Get file size
    if (file) {
        long current_pos = ftell(file);
        fseek(file, 0, SEEK_END);
        handle->size = ftell(file);
        fseek(file, current_pos, SEEK_SET);
        handle->position = current_pos;
    } else {
        handle->size = 0;
    }
    
    return handle;
}

static void file_handle_free(FileHandle* handle) {
    if (!handle) return;
    
    if (handle->file && handle->is_open) {
        fclose(handle->file);
    }
    
    shared_free_safe(handle->filename, "file", "unknown_function", 58);
    shared_free_safe(handle->mode, "file", "unknown_function", 59);
    shared_free_safe(handle, "file", "unknown_function", 60);
}

static FileHandle* file_handle_find(Value handle_value) {
    if (handle_value.type != VALUE_NUMBER) return NULL;
    
    int handle_id = (int)handle_value.data.number_value;
    if (handle_id < 0 || handle_id >= file_handle_count) return NULL;
    
    return &file_handles[handle_id];
}

static int file_handle_register(FileHandle* handle) {
    if (!file_handles) {
        file_handle_capacity = 10;
        file_handles = shared_malloc_safe(sizeof(FileHandle) * file_handle_capacity, "file", "file_handle", 75);
        if (!file_handles) return -1;
    }
    
    if (file_handle_count >= file_handle_capacity) {
        file_handle_capacity *= 2;
        file_handles = shared_realloc_safe(file_handles, sizeof(FileHandle) * file_handle_capacity, "file", "file_handle", 81);
        if (!file_handles) return -1;
    }
    
    file_handles[file_handle_count] = *handle;
    return file_handle_count++;
}

// File library functions

// File handle operations
Value builtin_file_open(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 1 || arg_count > 2) {
        std_error_report(ERROR_INTERNAL_ERROR, "file", "unknown_function", "file.open() requires 1-2 arguments (filename, [mode])", line, column);
        return value_create_null();
    }
    
    Value filename_val = args[0];
    if (filename_val.type != VALUE_STRING) {
        std_error_report(ERROR_INVALID_ARGUMENT, "file", "unknown_function", "file.open() first argument must be a string (filename)", line, column);
        return value_create_null();
    }
    
    const char* filename = filename_val.data.string_value;
    const char* mode = "r";  // Default mode
    
    if (arg_count == 2) {
        Value mode_val = args[1];
        if (mode_val.type != VALUE_STRING) {
            std_error_report(ERROR_INVALID_ARGUMENT, "file", "unknown_function", "file.open() second argument must be a string (mode)", line, column);
            return value_create_null();
        }
        mode = mode_val.data.string_value;
    }
    
    FILE* file = fopen(filename, mode);
    if (!file) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Cannot open file '%s' with mode '%s': %s", filename, mode, strerror(errno));
        interpreter_set_error(interpreter, error_msg, line, column);
        return value_create_null();
    }
    
    FileHandle handle;
    handle.file = file;
    handle.filename = (filename ? strdup(filename) : NULL);
    handle.mode = (mode ? strdup(mode) : NULL);
    handle.is_open = 1;
    handle.position = 0;
    
    // Get file size
    long current_pos = ftell(file);
    fseek(file, 0, SEEK_END);
    handle.size = ftell(file);
    fseek(file, current_pos, SEEK_SET);
    handle.position = current_pos;
    
    int handle_id = file_handle_register(&handle);
    if (handle_id == -1) {
        fclose(file);
        shared_free_safe(handle.filename, "file", "unknown_function", 141);
        shared_free_safe(handle.mode, "file", "unknown_function", 142);
        std_error_report(ERROR_INTERNAL_ERROR, "file", "unknown_function", "Failed to register file handle", line, column);
        return value_create_null();
    }
    
    return value_create_number(handle_id);
}

Value builtin_file_close(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_ARGUMENT_COUNT, "file", "unknown_function", "file.close() requires exactly 1 argument (handle)", line, column);
        return value_create_null();
    }
    
    FileHandle* handle = file_handle_find(args[0]);
    if (!handle) {
        std_error_report(ERROR_INTERNAL_ERROR, "file", "unknown_function", "Invalid file handle", line, column);
        return value_create_null();
    }
    
    if (!handle->is_open) {
        std_error_report(ERROR_INTERNAL_ERROR, "file", "unknown_function", "File handle is already closed", line, column);
        return value_create_null();
    }
    
    int result = fclose(handle->file);
    handle->is_open = 0;
    handle->file = NULL;
    
    if (result != 0) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Error closing file: %s", strerror(errno));
        interpreter_set_error(interpreter, error_msg, line, column);
        return value_create_null();
    }
    
    return value_create_null();
}

Value builtin_file_read_chunk(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 1 || arg_count > 2) {
        std_error_report(ERROR_INTERNAL_ERROR, "file", "unknown_function", "file.read() requires 1-2 arguments (handle, [size])", line, column);
        return value_create_null();
    }
    
    FileHandle* handle = file_handle_find(args[0]);
    if (!handle) {
        std_error_report(ERROR_INTERNAL_ERROR, "file", "unknown_function", "Invalid file handle", line, column);
        return value_create_null();
    }
    
    if (!handle->is_open) {
        std_error_report(ERROR_INTERNAL_ERROR, "file", "unknown_function", "File handle is closed", line, column);
        return value_create_null();
    }
    
    size_t size = 1024;  // Default read size
    if (arg_count == 2) {
        Value size_val = args[1];
        if (size_val.type != VALUE_NUMBER) {
            std_error_report(ERROR_INVALID_ARGUMENT, "file", "unknown_function", "file.read() second argument must be a number (size)", line, column);
            return value_create_null();
        }
        size = (size_t)size_val.data.number_value;
    }
    
    char* buffer = shared_malloc_safe(size + 1, "file", "unknown_function", 208);
    if (!buffer) {
        std_error_report(ERROR_OUT_OF_MEMORY, "file", "unknown_function", "Out of memory while reading file", line, column);
        return value_create_null();
    }
    
    size_t bytes_read = fread(buffer, 1, size, handle->file);
    buffer[bytes_read] = '\0';
    
    handle->position = ftell(handle->file);
    
    Value result = value_create_string(buffer);
    shared_free_safe(buffer, "file", "unknown_function", 220);
    
    return result;
}

Value builtin_file_write_chunk(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        std_error_report(ERROR_ARGUMENT_COUNT, "file", "unknown_function", "file.write() requires exactly 2 arguments (handle, data)", line, column);
        return value_create_null();
    }
    
    FileHandle* handle = file_handle_find(args[0]);
    if (!handle) {
        std_error_report(ERROR_INTERNAL_ERROR, "file", "unknown_function", "Invalid file handle", line, column);
        return value_create_null();
    }
    
    if (!handle->is_open) {
        std_error_report(ERROR_INTERNAL_ERROR, "file", "unknown_function", "File handle is closed", line, column);
        return value_create_null();
    }
    
    Value data_val = args[1];
    if (data_val.type != VALUE_STRING) {
        std_error_report(ERROR_INVALID_ARGUMENT, "file", "unknown_function", "file.write() second argument must be a string (data)", line, column);
        return value_create_null();
    }
    
    const char* data = data_val.data.string_value;
    size_t data_len = strlen(data);
    
    size_t bytes_written = fwrite(data, 1, data_len, handle->file);
    if (bytes_written != data_len) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Error writing to file: %s", strerror(errno));
        interpreter_set_error(interpreter, error_msg, line, column);
        return value_create_null();
    }
    
    handle->position = ftell(handle->file);
    
    return value_create_number(bytes_written);
}

Value builtin_file_seek(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        std_error_report(ERROR_ARGUMENT_COUNT, "file", "unknown_function", "file.seek() requires exactly 2 arguments (handle, position)", line, column);
        return value_create_null();
    }
    
    FileHandle* handle = file_handle_find(args[0]);
    if (!handle) {
        std_error_report(ERROR_INTERNAL_ERROR, "file", "unknown_function", "Invalid file handle", line, column);
        return value_create_null();
    }
    
    if (!handle->is_open) {
        std_error_report(ERROR_INTERNAL_ERROR, "file", "unknown_function", "File handle is closed", line, column);
        return value_create_null();
    }
    
    Value pos_val = args[1];
    if (pos_val.type != VALUE_NUMBER) {
        std_error_report(ERROR_INVALID_ARGUMENT, "file", "unknown_function", "file.seek() second argument must be a number (position)", line, column);
        return value_create_null();
    }
    
    long position = (long)pos_val.data.number_value;
    
    if (fseek(handle->file, position, SEEK_SET) != 0) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Error seeking in file: %s", strerror(errno));
        interpreter_set_error(interpreter, error_msg, line, column);
        return value_create_null();
    }
    
    handle->position = position;
    
    return value_create_null();
}

Value builtin_file_tell(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_ARGUMENT_COUNT, "file", "unknown_function", "file.tell() requires exactly 1 argument (handle)", line, column);
        return value_create_null();
    }
    
    FileHandle* handle = file_handle_find(args[0]);
    if (!handle) {
        std_error_report(ERROR_INTERNAL_ERROR, "file", "unknown_function", "Invalid file handle", line, column);
        return value_create_null();
    }
    
    if (!handle->is_open) {
        std_error_report(ERROR_INTERNAL_ERROR, "file", "unknown_function", "File handle is closed", line, column);
        return value_create_null();
    }
    
    long position = ftell(handle->file);
    if (position == -1) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Error getting file position: %s", strerror(errno));
        interpreter_set_error(interpreter, error_msg, line, column);
        return value_create_null();
    }
    
    handle->position = position;
    
    return value_create_number(position);
}

Value builtin_file_eof(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_ARGUMENT_COUNT, "file", "unknown_function", "file.eof() requires exactly 1 argument (handle)", line, column);
        return value_create_null();
    }
    
    FileHandle* handle = file_handle_find(args[0]);
    if (!handle) {
        std_error_report(ERROR_INTERNAL_ERROR, "file", "unknown_function", "Invalid file handle", line, column);
        return value_create_null();
    }
    
    if (!handle->is_open) {
        std_error_report(ERROR_INTERNAL_ERROR, "file", "unknown_function", "File handle is closed", line, column);
        return value_create_null();
    }
    
    return value_create_boolean(feof(handle->file));
}

Value builtin_file_size_handle(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_ARGUMENT_COUNT, "file", "unknown_function", "file.size() requires exactly 1 argument (handle)", line, column);
        return value_create_null();
    }
    
    FileHandle* handle = file_handle_find(args[0]);
    if (!handle) {
        std_error_report(ERROR_INTERNAL_ERROR, "file", "unknown_function", "Invalid file handle", line, column);
        return value_create_null();
    }
    
    if (!handle->is_open) {
        std_error_report(ERROR_INTERNAL_ERROR, "file", "unknown_function", "File handle is closed", line, column);
        return value_create_null();
    }
    
    return value_create_number(handle->size);
}

Value builtin_file_flush(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_ARGUMENT_COUNT, "file", "unknown_function", "file.flush() requires exactly 1 argument (handle)", line, column);
        return value_create_null();
    }
    
    FileHandle* handle = file_handle_find(args[0]);
    if (!handle) {
        std_error_report(ERROR_INTERNAL_ERROR, "file", "unknown_function", "Invalid file handle", line, column);
        return value_create_null();
    }
    
    if (!handle->is_open) {
        std_error_report(ERROR_INTERNAL_ERROR, "file", "unknown_function", "File handle is closed", line, column);
        return value_create_null();
    }
    
    if (fflush(handle->file) != 0) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Error flushing file: %s", strerror(errno));
        interpreter_set_error(interpreter, error_msg, line, column);
        return value_create_null();
    }
    
    return value_create_null();
}

// Read entire file content as string
Value builtin_file_read(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_ARGUMENT_COUNT, "file", "unknown_function", "file.read() requires exactly 1 argument (filename)", line, column);
        return value_create_null();
    }
    
    Value filename_val = args[0];
    if (filename_val.type != VALUE_STRING) {
        std_error_report(ERROR_INVALID_ARGUMENT, "file", "unknown_function", "file.read() argument must be a string (filename)", line, column);
        return value_create_null();
    }
    
    const char* filename = filename_val.data.string_value;
    FILE* file = fopen(filename, "r");
    if (!file) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "File not found or cannot be opened: %s", filename);
        interpreter_set_error(interpreter, error_msg, line, column);
        return value_create_null();
    }
    
    // Get file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    if (file_size < 0) {
        fclose(file);
        std_error_report(ERROR_INTERNAL_ERROR, "file", "unknown_function", "Cannot determine file size", line, column);
        return value_create_null();
    }
    
    // Allocate buffer and read file
    char* buffer = shared_malloc_safe(file_size + 1, "file", "unknown_function", 432);
    if (!buffer) {
        fclose(file);
        std_error_report(ERROR_OUT_OF_MEMORY, "file", "unknown_function", "Out of memory while reading file", line, column);
        return value_create_null();
    }
    
    size_t bytes_read = fread(buffer, 1, file_size, file);
    buffer[bytes_read] = '\0';
    fclose(file);
    
    if (bytes_read != (size_t)file_size) {
        shared_free_safe(buffer, "file", "unknown_function", 444);
        std_error_report(ERROR_INTERNAL_ERROR, "file", "unknown_function", "Error reading file content", line, column);
        return value_create_null();
    }
    
    return value_create_string(buffer);
}

// Write string content to file
Value builtin_file_write(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        std_error_report(ERROR_ARGUMENT_COUNT, "file", "unknown_function", "file.write() requires exactly 2 arguments (filename, content)", line, column);
        return value_create_null();
    }
    
    Value filename_val = args[0];
    Value content_val = args[1];
    
    if (filename_val.type != VALUE_STRING) {
        std_error_report(ERROR_INVALID_ARGUMENT, "file", "unknown_function", "file.write() first argument must be a string (filename)", line, column);
        return value_create_null();
    }
    
    if (content_val.type != VALUE_STRING) {
        std_error_report(ERROR_INVALID_ARGUMENT, "file", "builtin_file_write", "file.write() second argument must be a string (content)", line, column);
        return value_create_null();
    }
    
    const char* filename = filename_val.data.string_value;
    const char* content = content_val.data.string_value;
    
    FILE* file = fopen(filename, "w");
    if (!file) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Cannot create or write to file: %s", filename);
        interpreter_set_error(interpreter, error_msg, line, column);
        return value_create_null();
    }
    
    size_t content_len = strlen(content);
    size_t bytes_written = fwrite(content, 1, content_len, file);
    fclose(file);
    
    if (bytes_written != content_len) {
        std_error_report(ERROR_INTERNAL_ERROR, "file", "unknown_function", "Error writing file content", line, column);
        return value_create_null();
    }
    
    return value_create_null(); // Success
}

// Append string content to file
Value builtin_file_append(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        std_error_report(ERROR_ARGUMENT_COUNT, "file", "unknown_function", "file.append() requires exactly 2 arguments (filename, content)", line, column);
        return value_create_null();
    }
    
    Value filename_val = args[0];
    Value content_val = args[1];
    
    if (filename_val.type != VALUE_STRING) {
        std_error_report(ERROR_INVALID_ARGUMENT, "file", "unknown_function", "file.append() first argument must be a string (filename)", line, column);
        return value_create_null();
    }
    
    if (content_val.type != VALUE_STRING) {
        std_error_report(ERROR_INVALID_ARGUMENT, "file", "unknown_function", "file.append() second argument must be a string (content)", line, column);
        return value_create_null();
    }
    
    const char* filename = filename_val.data.string_value;
    const char* content = content_val.data.string_value;
    
    FILE* file = fopen(filename, "a");
    if (!file) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Cannot open file for appending: %s", filename);
        interpreter_set_error(interpreter, error_msg, line, column);
        return value_create_null();
    }
    
    size_t content_len = strlen(content);
    size_t bytes_written = fwrite(content, 1, content_len, file);
    fclose(file);
    
    if (bytes_written != content_len) {
        std_error_report(ERROR_INTERNAL_ERROR, "file", "unknown_function", "Error appending to file", line, column);
        return value_create_null();
    }
    
    return value_create_null(); // Success
}

// Check if file exists
Value builtin_file_exists(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_ARGUMENT_COUNT, "file", "unknown_function", "file.exists() requires exactly 1 argument (filename)", line, column);
        return value_create_null();
    }
    
    Value filename_val = args[0];
    if (filename_val.type != VALUE_STRING) {
        std_error_report(ERROR_INVALID_ARGUMENT, "file", "unknown_function", "file.exists() argument must be a string (filename)", line, column);
        return value_create_null();
    }
    
    const char* filename = filename_val.data.string_value;
    struct stat file_stat;
    int result = stat(filename, &file_stat);
    
    return value_create_boolean(result == 0);
}

// Get file size in bytes
Value builtin_file_size(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_ARGUMENT_COUNT, "file", "unknown_function", "file.size() requires exactly 1 argument (filename)", line, column);
        return value_create_null();
    }
    
    Value filename_val = args[0];
    if (filename_val.type != VALUE_STRING) {
        std_error_report(ERROR_INVALID_ARGUMENT, "file", "unknown_function", "file.size() argument must be a string (filename)", line, column);
        return value_create_null();
    }
    
    const char* filename = filename_val.data.string_value;
    struct stat file_stat;
    int result = stat(filename, &file_stat);
    
    if (result != 0) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "File not found or cannot access: %s", filename);
        interpreter_set_error(interpreter, error_msg, line, column);
        return value_create_null();
    }
    
    return value_create_number((double)file_stat.st_size);
}

// Delete file
Value builtin_file_delete(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_ARGUMENT_COUNT, "file", "unknown_function", "file.delete() requires exactly 1 argument (filename)", line, column);
        return value_create_null();
    }
    
    Value filename_val = args[0];
    if (filename_val.type != VALUE_STRING) {
        std_error_report(ERROR_INVALID_ARGUMENT, "file", "unknown_function", "file.delete() argument must be a string (filename)", line, column);
        return value_create_null();
    }
    
    const char* filename = filename_val.data.string_value;
    int result = remove(filename);
    
    if (result != 0) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Cannot delete file: %s", filename);
        interpreter_set_error(interpreter, error_msg, line, column);
        return value_create_null();
    }
    
    return value_create_null(); // Success
}

// Read file as array of lines
Value builtin_file_read_lines(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_ARGUMENT_COUNT, "file", "unknown_function", "file.read_lines() requires exactly 1 argument (filename)", line, column);
        return value_create_null();
    }
    
    Value filename_val = args[0];
    if (filename_val.type != VALUE_STRING) {
        std_error_report(ERROR_INVALID_ARGUMENT, "file", "unknown_function", "file.read_lines() argument must be a string (filename)", line, column);
        return value_create_null();
    }
    
    const char* filename = filename_val.data.string_value;
    FILE* file = fopen(filename, "r");
    if (!file) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "File not found or cannot be opened: %s", filename);
        interpreter_set_error(interpreter, error_msg, line, column);
        return value_create_null();
    }
    
    // Create array to store lines
    Value lines = value_create_array(0);
    char* line_buffer = NULL;
    size_t line_length = 0;
    
    while (getline(&line_buffer, &line_length, file) != -1) {
        // Remove newline character if present
        size_t len = strlen(line_buffer);
        if (len > 0 && line_buffer[len - 1] == '\n') {
            line_buffer[len - 1] = '\0';
        }
        
        Value line_value = value_create_string(line_buffer);
        value_array_push(&lines, line_value);
    }
    
    if (line_buffer) {
        shared_free_safe(line_buffer, "file", "unknown_function", 650);
    }
    fclose(file);
    
    return lines;
}

// Write array of lines to file
Value builtin_file_write_lines(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        std_error_report(ERROR_ARGUMENT_COUNT, "file", "unknown_function", "file.write_lines() requires exactly 2 arguments (filename, lines)", line, column);
        return value_create_null();
    }
    
    Value filename_val = args[0];
    Value lines_val = args[1];
    
    if (filename_val.type != VALUE_STRING) {
        std_error_report(ERROR_INVALID_ARGUMENT, "file", "unknown_function", "file.write_lines() first argument must be a string (filename)", line, column);
        return value_create_null();
    }
    
    if (lines_val.type != VALUE_ARRAY) {
        std_error_report(ERROR_INVALID_ARGUMENT, "file", "unknown_function", "file.write_lines() second argument must be an array of strings", line, column);
        return value_create_null();
    }
    
    const char* filename = filename_val.data.string_value;
    FILE* file = fopen(filename, "w");
    if (!file) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Cannot create or write to file: %s", filename);
        interpreter_set_error(interpreter, error_msg, line, column);
        return value_create_null();
    }
    
    // Write each line
    for (size_t i = 0; i < lines_val.data.array_value.count; i++) {
        Value* line_val = (Value*)lines_val.data.array_value.elements[i];
        if (line_val->type == VALUE_STRING) {
            fprintf(file, "%s\n", line_val->data.string_value);
        } else {
            // Convert non-string values to string
            Value str_val = value_to_string(line_val);
            if (str_val.type == VALUE_STRING) {
                fprintf(file, "%s\n", str_val.data.string_value);
                value_free(&str_val);
            }
        }
    }
    
    fclose(file);
    return value_create_null(); // Success
}

// Register file library functions
void file_library_register(Interpreter* interpreter) {
    if (!interpreter || !interpreter->global_environment) return;
    
    // Register file functions with "file_" prefix for namespace support
    environment_define(interpreter->global_environment, "file_read", value_create_builtin_function(builtin_file_read));
    environment_define(interpreter->global_environment, "file_write", value_create_builtin_function(builtin_file_write));
    environment_define(interpreter->global_environment, "file_append", value_create_builtin_function(builtin_file_append));
    environment_define(interpreter->global_environment, "file_exists", value_create_builtin_function(builtin_file_exists));
    environment_define(interpreter->global_environment, "file_size", value_create_builtin_function(builtin_file_size));
    environment_define(interpreter->global_environment, "file_delete", value_create_builtin_function(builtin_file_delete));
    environment_define(interpreter->global_environment, "file_read_lines", value_create_builtin_function(builtin_file_read_lines));
    environment_define(interpreter->global_environment, "file_write_lines", value_create_builtin_function(builtin_file_write_lines));
    
    // File handle operations
    environment_define(interpreter->global_environment, "file_open", value_create_builtin_function(builtin_file_open));
    environment_define(interpreter->global_environment, "file_close", value_create_builtin_function(builtin_file_close));
    environment_define(interpreter->global_environment, "file_read_chunk", value_create_builtin_function(builtin_file_read_chunk));
    environment_define(interpreter->global_environment, "file_write_chunk", value_create_builtin_function(builtin_file_write_chunk));
    environment_define(interpreter->global_environment, "file_seek", value_create_builtin_function(builtin_file_seek));
    environment_define(interpreter->global_environment, "file_tell", value_create_builtin_function(builtin_file_tell));
    environment_define(interpreter->global_environment, "file_eof", value_create_builtin_function(builtin_file_eof));
    environment_define(interpreter->global_environment, "file_size_handle", value_create_builtin_function(builtin_file_size_handle));
    environment_define(interpreter->global_environment, "file_flush", value_create_builtin_function(builtin_file_flush));
}
