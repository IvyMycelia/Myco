#ifndef MYCO_JSON_H
#define MYCO_JSON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "../core/interpreter.h"

// JSON value types
typedef enum {
    JSON_NULL,
    JSON_BOOLEAN,
    JSON_NUMBER,
    JSON_STRING,
    JSON_ARRAY,
    JSON_OBJECT
} JsonValueType;

// JSON value structure
typedef struct JsonValue {
    JsonValueType type;
    union {
        bool boolean_value;
        double number_value;
        char* string_value;
        struct {
            struct JsonValue** elements;
            size_t count;
            size_t capacity;
        } array_value;
        struct {
            char** keys;
            struct JsonValue** values;
            size_t count;
            size_t capacity;
        } object_value;
    } data;
} JsonValue;

// JSON parsing context
typedef struct {
    const char* input;
    size_t position;
    size_t length;
    char* error_message;
    bool has_error;
} JsonContext;

// JSON parsing functions
JsonValue* json_parse(const char* input);
JsonValue* json_parse_file(const char* filename);
char* json_stringify(const JsonValue* value);
char* json_pretty(const JsonValue* value, int indent);
bool json_validate(const char* input);
JsonValue* json_merge(const JsonValue* obj1, const JsonValue* obj2);
JsonValue* json_extract(const JsonValue* value, const char* path);

// JSON utility functions
void json_free(JsonValue* value);
JsonValue* json_clone(const JsonValue* value);
bool json_equals(const JsonValue* a, const JsonValue* b);
size_t json_size(const JsonValue* value);
bool json_is_empty(const JsonValue* value);
JsonValue* json_get(const JsonValue* value, const char* key);
void json_set(JsonValue* value, const char* key, JsonValue* item);
JsonValue* json_array_get(const JsonValue* array, size_t index);
void json_array_set(JsonValue* array, size_t index, JsonValue* item);
void json_array_append(JsonValue* array, JsonValue* item);

// JSON type checking
bool json_is_null(const JsonValue* value);
bool json_is_boolean(const JsonValue* value);
bool json_is_number(const JsonValue* value);
bool json_is_string(const JsonValue* value);
bool json_is_array(const JsonValue* value);
bool json_is_object(const JsonValue* value);

// JSON conversion functions
JsonValue* json_from_myco_value(const Value* value);
Value json_to_myco_value(const JsonValue* json_value);

// Myco library functions
Value builtin_json_parse(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_json_stringify(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_json_validate(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_json_pretty(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_json_merge(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_json_extract(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_json_get(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_json_set(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_json_size(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_json_is_empty(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);

// Library registration function
void json_library_register(Interpreter* interpreter);

#endif // MYCO_JSON_H
