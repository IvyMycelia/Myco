#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include "../../include/core/interpreter.h"
#include "../../include/core/ast.h"

// File library functions

// Read entire file content as string
Value builtin_file_read(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "file.read() requires exactly 1 argument (filename)", line, column);
        return value_create_null();
    }
    
    Value filename_val = args[0];
    if (filename_val.type != VALUE_STRING) {
        interpreter_set_error(interpreter, "file.read() argument must be a string (filename)", line, column);
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
        interpreter_set_error(interpreter, "Cannot determine file size", line, column);
        return value_create_null();
    }
    
    // Allocate buffer and read file
    char* buffer = malloc(file_size + 1);
    if (!buffer) {
        fclose(file);
        interpreter_set_error(interpreter, "Out of memory while reading file", line, column);
        return value_create_null();
    }
    
    size_t bytes_read = fread(buffer, 1, file_size, file);
    buffer[bytes_read] = '\0';
    fclose(file);
    
    if (bytes_read != (size_t)file_size) {
        free(buffer);
        interpreter_set_error(interpreter, "Error reading file content", line, column);
        return value_create_null();
    }
    
    return value_create_string(buffer);
}

// Write string content to file
Value builtin_file_write(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        interpreter_set_error(interpreter, "file.write() requires exactly 2 arguments (filename, content)", line, column);
        return value_create_null();
    }
    
    Value filename_val = args[0];
    Value content_val = args[1];
    
    if (filename_val.type != VALUE_STRING) {
        interpreter_set_error(interpreter, "file.write() first argument must be a string (filename)", line, column);
        return value_create_null();
    }
    
    if (content_val.type != VALUE_STRING) {
        interpreter_set_error(interpreter, "file.write() second argument must be a string (content)", line, column);
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
        interpreter_set_error(interpreter, "Error writing file content", line, column);
        return value_create_null();
    }
    
    return value_create_null(); // Success
}

// Append string content to file
Value builtin_file_append(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        interpreter_set_error(interpreter, "file.append() requires exactly 2 arguments (filename, content)", line, column);
        return value_create_null();
    }
    
    Value filename_val = args[0];
    Value content_val = args[1];
    
    if (filename_val.type != VALUE_STRING) {
        interpreter_set_error(interpreter, "file.append() first argument must be a string (filename)", line, column);
        return value_create_null();
    }
    
    if (content_val.type != VALUE_STRING) {
        interpreter_set_error(interpreter, "file.append() second argument must be a string (content)", line, column);
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
        interpreter_set_error(interpreter, "Error appending to file", line, column);
        return value_create_null();
    }
    
    return value_create_null(); // Success
}

// Check if file exists
Value builtin_file_exists(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "file.exists() requires exactly 1 argument (filename)", line, column);
        return value_create_null();
    }
    
    Value filename_val = args[0];
    if (filename_val.type != VALUE_STRING) {
        interpreter_set_error(interpreter, "file.exists() argument must be a string (filename)", line, column);
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
        interpreter_set_error(interpreter, "file.size() requires exactly 1 argument (filename)", line, column);
        return value_create_null();
    }
    
    Value filename_val = args[0];
    if (filename_val.type != VALUE_STRING) {
        interpreter_set_error(interpreter, "file.size() argument must be a string (filename)", line, column);
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
        interpreter_set_error(interpreter, "file.delete() requires exactly 1 argument (filename)", line, column);
        return value_create_null();
    }
    
    Value filename_val = args[0];
    if (filename_val.type != VALUE_STRING) {
        interpreter_set_error(interpreter, "file.delete() argument must be a string (filename)", line, column);
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
        interpreter_set_error(interpreter, "file.read_lines() requires exactly 1 argument (filename)", line, column);
        return value_create_null();
    }
    
    Value filename_val = args[0];
    if (filename_val.type != VALUE_STRING) {
        interpreter_set_error(interpreter, "file.read_lines() argument must be a string (filename)", line, column);
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
        free(line_buffer);
    }
    fclose(file);
    
    return lines;
}

// Write array of lines to file
Value builtin_file_write_lines(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        interpreter_set_error(interpreter, "file.write_lines() requires exactly 2 arguments (filename, lines)", line, column);
        return value_create_null();
    }
    
    Value filename_val = args[0];
    Value lines_val = args[1];
    
    if (filename_val.type != VALUE_STRING) {
        interpreter_set_error(interpreter, "file.write_lines() first argument must be a string (filename)", line, column);
        return value_create_null();
    }
    
    if (lines_val.type != VALUE_ARRAY) {
        interpreter_set_error(interpreter, "file.write_lines() second argument must be an array of strings", line, column);
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
}
