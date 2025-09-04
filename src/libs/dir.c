#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include "../../include/core/interpreter.h"
#include "../../include/core/ast.h"

// Directory library functions

// List directory contents
Value builtin_dir_list(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "dir.list() requires exactly 1 argument (directory path)", line, column);
        return value_create_null();
    }
    
    Value dirname_val = args[0];
    if (dirname_val.type != VALUE_STRING) {
        interpreter_set_error(interpreter, "dir.list() argument must be a string (directory path)", line, column);
        return value_create_null();
    }
    
    const char* dirname = dirname_val.data.string_value;
    DIR* dir = opendir(dirname);
    if (!dir) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Cannot open directory: %s", dirname);
        interpreter_set_error(interpreter, error_msg, line, column);
        return value_create_null();
    }
    
    // Create array to store directory entries
    Value entries = value_create_array(0);
    struct dirent* entry;
    
    while ((entry = readdir(dir)) != NULL) {
        // Skip . and .. entries
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        Value entry_value = value_create_string(entry->d_name);
        value_array_push(&entries, entry_value);
    }
    
    closedir(dir);
    return entries;
}

// Create directory
Value builtin_dir_create(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "dir.create() requires exactly 1 argument (directory path)", line, column);
        return value_create_null();
    }
    
    Value dirname_val = args[0];
    if (dirname_val.type != VALUE_STRING) {
        interpreter_set_error(interpreter, "dir.create() argument must be a string (directory path)", line, column);
        return value_create_null();
    }
    
    const char* dirname = dirname_val.data.string_value;
    int result = mkdir(dirname, 0755);
    
    if (result != 0) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Cannot create directory: %s", dirname);
        interpreter_set_error(interpreter, error_msg, line, column);
        return value_create_null();
    }
    
    return value_create_null(); // Success
}

// Remove directory
Value builtin_dir_remove(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "dir.remove() requires exactly 1 argument (directory path)", line, column);
        return value_create_null();
    }
    
    Value dirname_val = args[0];
    if (dirname_val.type != VALUE_STRING) {
        interpreter_set_error(interpreter, "dir.remove() argument must be a string (directory path)", line, column);
        return value_create_null();
    }
    
    const char* dirname = dirname_val.data.string_value;
    int result = rmdir(dirname);
    
    if (result != 0) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Cannot remove directory: %s", dirname);
        interpreter_set_error(interpreter, error_msg, line, column);
        return value_create_null();
    }
    
    return value_create_null(); // Success
}

// Check if path is a directory
Value builtin_dir_exists(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "dir.exists() requires exactly 1 argument (directory path)", line, column);
        return value_create_null();
    }
    
    Value dirname_val = args[0];
    if (dirname_val.type != VALUE_STRING) {
        interpreter_set_error(interpreter, "dir.exists() argument must be a string (directory path)", line, column);
        return value_create_null();
    }
    
    const char* dirname = dirname_val.data.string_value;
    struct stat file_stat;
    int result = stat(dirname, &file_stat);
    
    if (result != 0) {
        return value_create_boolean(0);
    }
    
    return value_create_boolean(S_ISDIR(file_stat.st_mode));
}

// Get current working directory
Value builtin_dir_current(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    (void)interpreter; // Suppress unused parameter warning
    (void)args; // Suppress unused parameter warning
    (void)arg_count; // Suppress unused parameter warning
    (void)line; // Suppress unused parameter warning
    (void)column; // Suppress unused parameter warning
    
    char* cwd = getcwd(NULL, 0);
    if (!cwd) {
        return value_create_string("");
    }
    
    Value result = value_create_string(cwd);
    free(cwd);
    return result;
}

// Change directory
Value builtin_dir_change(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "dir.change() requires exactly 1 argument (directory path)", line, column);
        return value_create_null();
    }
    
    Value dirname_val = args[0];
    if (dirname_val.type != VALUE_STRING) {
        interpreter_set_error(interpreter, "dir.change() argument must be a string (directory path)", line, column);
        return value_create_null();
    }
    
    const char* dirname = dirname_val.data.string_value;
    int result = chdir(dirname);
    
    if (result != 0) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Cannot change to directory: %s", dirname);
        interpreter_set_error(interpreter, error_msg, line, column);
        return value_create_null();
    }
    
    return value_create_null(); // Success
}

// Get file/directory information
Value builtin_dir_info(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "dir.info() requires exactly 1 argument (path)", line, column);
        return value_create_null();
    }
    
    Value path_val = args[0];
    if (path_val.type != VALUE_STRING) {
        interpreter_set_error(interpreter, "dir.info() argument must be a string (path)", line, column);
        return value_create_null();
    }
    
    const char* path = path_val.data.string_value;
    struct stat file_stat;
    int result = stat(path, &file_stat);
    
    if (result != 0) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Cannot get info for path: %s", path);
        interpreter_set_error(interpreter, error_msg, line, column);
        return value_create_null();
    }
    
    // Create object with file information
    Value info = value_create_object(6);
    
    // File name
    Value name_val = value_create_string(path);
    value_object_set_member(&info, "name", name_val);
    
    // File size
    Value size_val = value_create_number((double)file_stat.st_size);
    value_object_set_member(&info, "size", size_val);
    
    // Is directory
    Value is_dir_val = value_create_boolean(S_ISDIR(file_stat.st_mode));
    value_object_set_member(&info, "is_directory", is_dir_val);
    
    // Is file
    Value is_file_val = value_create_boolean(S_ISREG(file_stat.st_mode));
    value_object_set_member(&info, "is_file", is_file_val);
    
    // Modification time (simplified)
    Value mtime_val = value_create_number((double)file_stat.st_mtime);
    value_object_set_member(&info, "modified", mtime_val);
    
    // Permissions (simplified)
    Value perms_val = value_create_number((double)(file_stat.st_mode & 0777));
    value_object_set_member(&info, "permissions", perms_val);
    
    return info;
}

// Register directory library functions
void dir_library_register(Interpreter* interpreter) {
    if (!interpreter || !interpreter->global_environment) return;
    
    // Register directory functions with "dir_" prefix for namespace support
    environment_define(interpreter->global_environment, "dir_list", value_create_builtin_function(builtin_dir_list));
    environment_define(interpreter->global_environment, "dir_create", value_create_builtin_function(builtin_dir_create));
    environment_define(interpreter->global_environment, "dir_remove", value_create_builtin_function(builtin_dir_remove));
    environment_define(interpreter->global_environment, "dir_exists", value_create_builtin_function(builtin_dir_exists));
    environment_define(interpreter->global_environment, "dir_current", value_create_builtin_function(builtin_dir_current));
    environment_define(interpreter->global_environment, "dir_change", value_create_builtin_function(builtin_dir_change));
    environment_define(interpreter->global_environment, "dir_info", value_create_builtin_function(builtin_dir_info));
}
