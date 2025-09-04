#include "time.h"
#include "../../include/core/interpreter.h"
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Helper function to create a time value from time_t
static Value create_time_value(time_t timestamp) {
    Value time_val = value_create_object(10);
    value_object_set_member(&time_val, "timestamp", value_create_number((double)timestamp));
    value_object_set_member(&time_val, "type", value_create_string("time"));
    return time_val;
}

// Helper function to get timestamp from time value
static time_t get_timestamp(Value* time_val) {
    if (time_val->type != VALUE_OBJECT) {
        return 0;
    }
    
    Value timestamp_val = value_object_get(time_val, "timestamp");
    if (timestamp_val.type != VALUE_NUMBER) {
        return 0;
    }
    
    return (time_t)timestamp_val.data.number_value;
}

// time.now() - Get current timestamp
Value builtin_time_now(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 0) {
        interpreter_set_error(interpreter, "time.now() takes no arguments", line, column);
        return value_create_null();
    }
    
    time_t now = time(NULL);
    return create_time_value(now);
}

// time.create(year, month, day, hour, minute, second) - Create specific time
Value builtin_time_create(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 6) {
        interpreter_set_error(interpreter, "time.create() takes exactly 6 arguments: year, month, day, hour, minute, second", line, column);
        return value_create_null();
    }
    
    // Validate argument types
    for (size_t i = 0; i < 6; i++) {
        if (args[i].type != VALUE_NUMBER) {
            interpreter_set_error(interpreter, "time.create() arguments must be numbers", line, column);
            return value_create_null();
        }
    }
    
    int year = (int)args[0].data.number_value;
    int month = (int)args[1].data.number_value;
    int day = (int)args[2].data.number_value;
    int hour = (int)args[3].data.number_value;
    int minute = (int)args[4].data.number_value;
    int second = (int)args[5].data.number_value;
    
    // Basic validation
    if (year < 1970 || year > 3000) {
        interpreter_set_error(interpreter, "Year must be between 1970 and 3000", line, column);
        return value_create_null();
    }
    if (month < 1 || month > 12) {
        interpreter_set_error(interpreter, "Month must be between 1 and 12", line, column);
        return value_create_null();
    }
    if (day < 1 || day > 31) {
        interpreter_set_error(interpreter, "Day must be between 1 and 31", line, column);
        return value_create_null();
    }
    if (hour < 0 || hour > 23) {
        interpreter_set_error(interpreter, "Hour must be between 0 and 23", line, column);
        return value_create_null();
    }
    if (minute < 0 || minute > 59) {
        interpreter_set_error(interpreter, "Minute must be between 0 and 59", line, column);
        return value_create_null();
    }
    if (second < 0 || second > 59) {
        interpreter_set_error(interpreter, "Second must be between 0 and 59", line, column);
        return value_create_null();
    }
    
    struct tm time_struct = {0};
    time_struct.tm_year = year - 1900;  // tm_year is years since 1900
    time_struct.tm_mon = month - 1;     // tm_mon is 0-based
    time_struct.tm_mday = day;
    time_struct.tm_hour = hour;
    time_struct.tm_min = minute;
    time_struct.tm_sec = second;
    time_struct.tm_isdst = -1;  // Let system determine DST
    
    time_t timestamp = mktime(&time_struct);
    if (timestamp == -1) {
        interpreter_set_error(interpreter, "Invalid date/time combination", line, column);
        return value_create_null();
    }
    
    return create_time_value(timestamp);
}

// time.format(time_val, format_string) - Format time as string
Value builtin_time_format(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        interpreter_set_error(interpreter, "time.format() takes exactly 2 arguments: time, format", line, column);
        return value_create_null();
    }
    
    if (args[0].type != VALUE_OBJECT) {
        interpreter_set_error(interpreter, "First argument must be a time object", line, column);
        return value_create_null();
    }
    
    if (args[1].type != VALUE_STRING) {
        interpreter_set_error(interpreter, "Second argument must be a format string", line, column);
        return value_create_null();
    }
    
    time_t timestamp = get_timestamp(&args[0]);
    if (timestamp == 0) {
        interpreter_set_error(interpreter, "Invalid time object", line, column);
        return value_create_null();
    }
    
    struct tm* time_struct = localtime(&timestamp);
    if (!time_struct) {
        interpreter_set_error(interpreter, "Failed to convert timestamp", line, column);
        return value_create_null();
    }
    
    char buffer[256];
    size_t result = strftime(buffer, sizeof(buffer), args[1].data.string_value, time_struct);
    
    if (result == 0) {
        interpreter_set_error(interpreter, "Invalid format string or buffer too small", line, column);
        return value_create_null();
    }
    
    return value_create_string(buffer);
}

// time.parse(time_string, format_string) - Parse string to time
Value builtin_time_parse(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        interpreter_set_error(interpreter, "time.parse() takes exactly 2 arguments: time_string, format", line, column);
        return value_create_null();
    }
    
    if (args[0].type != VALUE_STRING) {
        interpreter_set_error(interpreter, "First argument must be a time string", line, column);
        return value_create_null();
    }
    
    if (args[1].type != VALUE_STRING) {
        interpreter_set_error(interpreter, "Second argument must be a format string", line, column);
        return value_create_null();
    }
    
    struct tm time_struct = {0};
    char* result = strptime(args[0].data.string_value, args[1].data.string_value, &time_struct);
    
    if (!result) {
        interpreter_set_error(interpreter, "Failed to parse time string with given format", line, column);
        return value_create_null();
    }
    
    time_t timestamp = mktime(&time_struct);
    if (timestamp == -1) {
        interpreter_set_error(interpreter, "Invalid date/time after parsing", line, column);
        return value_create_null();
    }
    
    return create_time_value(timestamp);
}

// time.year(time_val) - Get year from time
Value builtin_time_year(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "time.year() takes exactly 1 argument: time", line, column);
        return value_create_null();
    }
    
    if (args[0].type != VALUE_OBJECT) {
        interpreter_set_error(interpreter, "Argument must be a time object", line, column);
        return value_create_null();
    }
    
    time_t timestamp = get_timestamp(&args[0]);
    if (timestamp == 0) {
        interpreter_set_error(interpreter, "Invalid time object", line, column);
        return value_create_null();
    }
    
    struct tm* time_struct = localtime(&timestamp);
    if (!time_struct) {
        interpreter_set_error(interpreter, "Failed to convert timestamp", line, column);
        return value_create_null();
    }
    
    return value_create_number(time_struct->tm_year + 1900);
}

// time.month(time_val) - Get month from time
Value builtin_time_month(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "time.month() takes exactly 1 argument: time", line, column);
        return value_create_null();
    }
    
    if (args[0].type != VALUE_OBJECT) {
        interpreter_set_error(interpreter, "Argument must be a time object", line, column);
        return value_create_null();
    }
    
    time_t timestamp = get_timestamp(&args[0]);
    if (timestamp == 0) {
        interpreter_set_error(interpreter, "Invalid time object", line, column);
        return value_create_null();
    }
    
    struct tm* time_struct = localtime(&timestamp);
    if (!time_struct) {
        interpreter_set_error(interpreter, "Failed to convert timestamp", line, column);
        return value_create_null();
    }
    
    return value_create_number(time_struct->tm_mon + 1);  // Convert 0-based to 1-based
}

// time.day(time_val) - Get day from time
Value builtin_time_day(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "time.day() takes exactly 1 argument: time", line, column);
        return value_create_null();
    }
    
    if (args[0].type != VALUE_OBJECT) {
        interpreter_set_error(interpreter, "Argument must be a time object", line, column);
        return value_create_null();
    }
    
    time_t timestamp = get_timestamp(&args[0]);
    if (timestamp == 0) {
        interpreter_set_error(interpreter, "Invalid time object", line, column);
        return value_create_null();
    }
    
    struct tm* time_struct = localtime(&timestamp);
    if (!time_struct) {
        interpreter_set_error(interpreter, "Failed to convert timestamp", line, column);
        return value_create_null();
    }
    
    return value_create_number(time_struct->tm_mday);
}

// time.hour(time_val) - Get hour from time
Value builtin_time_hour(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "time.hour() takes exactly 1 argument: time", line, column);
        return value_create_null();
    }
    
    if (args[0].type != VALUE_OBJECT) {
        interpreter_set_error(interpreter, "Argument must be a time object", line, column);
        return value_create_null();
    }
    
    time_t timestamp = get_timestamp(&args[0]);
    if (timestamp == 0) {
        interpreter_set_error(interpreter, "Invalid time object", line, column);
        return value_create_null();
    }
    
    struct tm* time_struct = localtime(&timestamp);
    if (!time_struct) {
        interpreter_set_error(interpreter, "Failed to convert timestamp", line, column);
        return value_create_null();
    }
    
    return value_create_number(time_struct->tm_hour);
}

// time.minute(time_val) - Get minute from time
Value builtin_time_minute(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "time.minute() takes exactly 1 argument: time", line, column);
        return value_create_null();
    }
    
    if (args[0].type != VALUE_OBJECT) {
        interpreter_set_error(interpreter, "Argument must be a time object", line, column);
        return value_create_null();
    }
    
    time_t timestamp = get_timestamp(&args[0]);
    if (timestamp == 0) {
        interpreter_set_error(interpreter, "Invalid time object", line, column);
        return value_create_null();
    }
    
    struct tm* time_struct = localtime(&timestamp);
    if (!time_struct) {
        interpreter_set_error(interpreter, "Failed to convert timestamp", line, column);
        return value_create_null();
    }
    
    return value_create_number(time_struct->tm_min);
}

// time.second(time_val) - Get second from time
Value builtin_time_second(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "time.second() takes exactly 1 argument: time", line, column);
        return value_create_null();
    }
    
    if (args[0].type != VALUE_OBJECT) {
        interpreter_set_error(interpreter, "Argument must be a time object", line, column);
        return value_create_null();
    }
    
    time_t timestamp = get_timestamp(&args[0]);
    if (timestamp == 0) {
        interpreter_set_error(interpreter, "Invalid time object", line, column);
        return value_create_null();
    }
    
    struct tm* time_struct = localtime(&timestamp);
    if (!time_struct) {
        interpreter_set_error(interpreter, "Failed to convert timestamp", line, column);
        return value_create_null();
    }
    
    return value_create_number(time_struct->tm_sec);
}

// time.add(time_val, seconds) - Add seconds to time
Value builtin_time_add(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        interpreter_set_error(interpreter, "time.add() takes exactly 2 arguments: time, seconds", line, column);
        return value_create_null();
    }
    
    if (args[0].type != VALUE_OBJECT) {
        interpreter_set_error(interpreter, "First argument must be a time object", line, column);
        return value_create_null();
    }
    
    if (args[1].type != VALUE_NUMBER) {
        interpreter_set_error(interpreter, "Second argument must be a number", line, column);
        return value_create_null();
    }
    
    time_t timestamp = get_timestamp(&args[0]);
    if (timestamp == 0) {
        interpreter_set_error(interpreter, "Invalid time object", line, column);
        return value_create_null();
    }
    
    long seconds = (long)args[1].data.number_value;
    time_t new_timestamp = timestamp + seconds;
    
    return create_time_value(new_timestamp);
}

// time.subtract(time_val, seconds) - Subtract seconds from time
Value builtin_time_subtract(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        interpreter_set_error(interpreter, "time.subtract() takes exactly 2 arguments: time, seconds", line, column);
        return value_create_null();
    }
    
    if (args[0].type != VALUE_OBJECT) {
        interpreter_set_error(interpreter, "First argument must be a time object", line, column);
        return value_create_null();
    }
    
    if (args[1].type != VALUE_NUMBER) {
        interpreter_set_error(interpreter, "Second argument must be a number", line, column);
        return value_create_null();
    }
    
    time_t timestamp = get_timestamp(&args[0]);
    if (timestamp == 0) {
        interpreter_set_error(interpreter, "Invalid time object", line, column);
        return value_create_null();
    }
    
    long seconds = (long)args[1].data.number_value;
    time_t new_timestamp = timestamp - seconds;
    
    return create_time_value(new_timestamp);
}

// time.difference(time1, time2) - Get difference in seconds between two times
Value builtin_time_difference(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        interpreter_set_error(interpreter, "time.difference() takes exactly 2 arguments: time1, time2", line, column);
        return value_create_null();
    }
    
    if (args[0].type != VALUE_OBJECT || args[1].type != VALUE_OBJECT) {
        interpreter_set_error(interpreter, "Both arguments must be time objects", line, column);
        return value_create_null();
    }
    
    time_t timestamp1 = get_timestamp(&args[0]);
    time_t timestamp2 = get_timestamp(&args[1]);
    
    if (timestamp1 == 0 || timestamp2 == 0) {
        interpreter_set_error(interpreter, "Invalid time objects", line, column);
        return value_create_null();
    }
    
    long difference = (long)(timestamp1 - timestamp2);
    return value_create_number(difference);
}

// time.iso_string(time_val) - Get ISO 8601 string representation
Value builtin_time_iso_string(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "time.iso_string() takes exactly 1 argument: time", line, column);
        return value_create_null();
    }
    
    if (args[0].type != VALUE_OBJECT) {
        interpreter_set_error(interpreter, "Argument must be a time object", line, column);
        return value_create_null();
    }
    
    time_t timestamp = get_timestamp(&args[0]);
    if (timestamp == 0) {
        interpreter_set_error(interpreter, "Invalid time object", line, column);
        return value_create_null();
    }
    
    struct tm* time_struct = localtime(&timestamp);
    if (!time_struct) {
        interpreter_set_error(interpreter, "Failed to convert timestamp", line, column);
        return value_create_null();
    }
    
    char buffer[32];
    strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S", time_struct);
    
    return value_create_string(buffer);
}

// time.unix_timestamp(time_val) - Get Unix timestamp
Value builtin_time_unix_timestamp(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "time.unix_timestamp() takes exactly 1 argument: time", line, column);
        return value_create_null();
    }
    
    if (args[0].type != VALUE_OBJECT) {
        interpreter_set_error(interpreter, "Argument must be a time object", line, column);
        return value_create_null();
    }
    
    time_t timestamp = get_timestamp(&args[0]);
    if (timestamp == 0) {
        interpreter_set_error(interpreter, "Invalid time object", line, column);
        return value_create_null();
    }
    
    return value_create_number((double)timestamp);
}

// Register all time library functions
void time_library_register(Interpreter* interpreter) {
    if (!interpreter || !interpreter->global_environment) return;
    
    // Create time object with methods
    Value time_obj = value_create_object(16);
    
    // Add methods to time object
    value_object_set_member(&time_obj, "now", value_create_builtin_function(builtin_time_now));
    value_object_set_member(&time_obj, "create", value_create_builtin_function(builtin_time_create));
    value_object_set_member(&time_obj, "format", value_create_builtin_function(builtin_time_format));
    value_object_set_member(&time_obj, "parse", value_create_builtin_function(builtin_time_parse));
    value_object_set_member(&time_obj, "year", value_create_builtin_function(builtin_time_year));
    value_object_set_member(&time_obj, "month", value_create_builtin_function(builtin_time_month));
    value_object_set_member(&time_obj, "day", value_create_builtin_function(builtin_time_day));
    value_object_set_member(&time_obj, "hour", value_create_builtin_function(builtin_time_hour));
    value_object_set_member(&time_obj, "minute", value_create_builtin_function(builtin_time_minute));
    value_object_set_member(&time_obj, "second", value_create_builtin_function(builtin_time_second));
    value_object_set_member(&time_obj, "add", value_create_builtin_function(builtin_time_add));
    value_object_set_member(&time_obj, "subtract", value_create_builtin_function(builtin_time_subtract));
    value_object_set_member(&time_obj, "difference", value_create_builtin_function(builtin_time_difference));
    value_object_set_member(&time_obj, "iso_string", value_create_builtin_function(builtin_time_iso_string));
    value_object_set_member(&time_obj, "unix_timestamp", value_create_builtin_function(builtin_time_unix_timestamp));
    
    // Register time object in global environment
    environment_define(interpreter->global_environment, "time", time_obj);
}