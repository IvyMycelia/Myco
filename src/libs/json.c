#include "libs/json.h"
#include "libs/builtin_libs.h"
#include "core/interpreter.h"
#include "core/ast.h"
#include "../../include/core/standardized_errors.h"
#include "../../include/utils/shared_utilities.h"

// Forward declarations
static JsonValue* json_parse_value(JsonContext* ctx);

// Helper function to skip whitespace
static void json_skip_whitespace(JsonContext* ctx) {
    while (ctx->position < ctx->length && isspace(ctx->input[ctx->position])) {
        ctx->position++;
    }
}

// Helper function to check if we're at end of input
static bool json_is_at_end(JsonContext* ctx) {
    return ctx->position >= ctx->length;
}

// Helper function to get current character
static char json_current_char(JsonContext* ctx) {
    if (json_is_at_end(ctx)) return '\0';
    return ctx->input[ctx->position];
}

// Helper function to advance position
static void json_advance(JsonContext* ctx) {
    if (!json_is_at_end(ctx)) {
        ctx->position++;
    }
}

// Helper function to set error
static void json_set_error(JsonContext* ctx, const char* message) {
    ctx->has_error = true;
    if (ctx->error_message) {
        shared_free_safe(ctx->error_message, "libs", "unknown_function", 40);
    }
    ctx->error_message = (message ? strdup(message) : NULL);
}

// Helper function to create JSON value
static JsonValue* json_create_value(JsonValueType type) {
    JsonValue* value = shared_malloc_safe(sizeof(JsonValue), "libs", "unknown_function", 47);
    if (!value) return NULL;
    
    value->type = type;
    memset(&value->data, 0, sizeof(value->data));
    return value;
}

// Parse JSON null
static JsonValue* json_parse_null(JsonContext* ctx) {
    if (ctx->position + 4 <= ctx->length && 
        strncmp(ctx->input + ctx->position, "null", 4) == 0) {
        ctx->position += 4;
        return json_create_value(JSON_NULL);
    }
    json_set_error(ctx, "Expected 'null'");
    return NULL;
}

// Parse JSON boolean
static JsonValue* json_parse_boolean(JsonContext* ctx) {
    if (ctx->position + 4 <= ctx->length && 
        strncmp(ctx->input + ctx->position, "true", 4) == 0) {
        ctx->position += 4;
        JsonValue* value = json_create_value(JSON_BOOLEAN);
        value->data.boolean_value = true;
        return value;
    }
    if (ctx->position + 5 <= ctx->length && 
        strncmp(ctx->input + ctx->position, "false", 5) == 0) {
        ctx->position += 5;
        JsonValue* value = json_create_value(JSON_BOOLEAN);
        value->data.boolean_value = false;
        return value;
    }
    json_set_error(ctx, "Expected 'true' or 'false'");
    return NULL;
}

// Parse JSON number
static JsonValue* json_parse_number(JsonContext* ctx) {
    size_t start = ctx->position;
    
    // Handle negative sign
    if (json_current_char(ctx) == '-') {
        json_advance(ctx);
    }
    
    // Handle integer part
    if (!isdigit(json_current_char(ctx))) {
        json_set_error(ctx, "Expected digit");
        return NULL;
    }
    
    while (isdigit(json_current_char(ctx))) {
        json_advance(ctx);
    }
    
    // Handle decimal part
    if (json_current_char(ctx) == '.') {
        json_advance(ctx);
        if (!isdigit(json_current_char(ctx))) {
            json_set_error(ctx, "Expected digit after decimal point");
            return NULL;
        }
        while (isdigit(json_current_char(ctx))) {
            json_advance(ctx);
        }
    }
    
    // Handle exponent
    if (json_current_char(ctx) == 'e' || json_current_char(ctx) == 'E') {
        json_advance(ctx);
        if (json_current_char(ctx) == '+' || json_current_char(ctx) == '-') {
            json_advance(ctx);
        }
        if (!isdigit(json_current_char(ctx))) {
            json_set_error(ctx, "Expected digit in exponent");
            return NULL;
        }
        while (isdigit(json_current_char(ctx))) {
            json_advance(ctx);
        }
    }
    
    // Convert to number
    size_t length = ctx->position - start;
    char* number_str = shared_malloc_safe(length + 1, "libs", "unknown_function", 134);
    strncpy(number_str, ctx->input + start, length);
    number_str[length] = '\0';
    
    JsonValue* value = json_create_value(JSON_NUMBER);
    value->data.number_value = atof(number_str);
    shared_free_safe(number_str, "libs", "unknown_function", 140);
    
    return value;
}

// Parse JSON string
static JsonValue* json_parse_string(JsonContext* ctx) {
    if (json_current_char(ctx) != '"') {
        json_set_error(ctx, "Expected '\"'");
        return NULL;
    }
    json_advance(ctx);
    
    size_t start = ctx->position;
    while (!json_is_at_end(ctx) && json_current_char(ctx) != '"') {
        if (json_current_char(ctx) == '\\') {
            json_advance(ctx);
            if (json_is_at_end(ctx)) {
                json_set_error(ctx, "Incomplete escape sequence");
                return NULL;
            }
        }
        json_advance(ctx);
    }
    
    if (json_is_at_end(ctx)) {
        json_set_error(ctx, "Unterminated string");
        return NULL;
    }
    
    size_t length = ctx->position - start;
    char* string_value = shared_malloc_safe(length + 1, "libs", "unknown_function", 171);
    strncpy(string_value, ctx->input + start, length);
    string_value[length] = '\0';
    
    // Handle escape sequences
    char* processed = shared_malloc_safe(length + 1, "libs", "unknown_function", 176);
    size_t processed_len = 0;
    for (size_t i = 0; i < length; i++) {
        if (string_value[i] == '\\' && i + 1 < length) {
            switch (string_value[i + 1]) {
                case '"': processed[processed_len++] = '"'; i++; break;
                case '\\': processed[processed_len++] = '\\'; i++; break;
                case '/': processed[processed_len++] = '/'; i++; break;
                case 'b': processed[processed_len++] = '\b'; i++; break;
                case 'f': processed[processed_len++] = '\f'; i++; break;
                case 'n': processed[processed_len++] = '\n'; i++; break;
                case 'r': processed[processed_len++] = '\r'; i++; break;
                case 't': processed[processed_len++] = '\t'; i++; break;
                default: processed[processed_len++] = string_value[i]; break;
            }
        } else {
            processed[processed_len++] = string_value[i];
        }
    }
    processed[processed_len] = '\0';
    
    JsonValue* value = json_create_value(JSON_STRING);
    value->data.string_value = processed;
    shared_free_safe(string_value, "libs", "unknown_function", 199);
    
    json_advance(ctx); // Skip closing quote
    return value;
}

// Parse JSON array
static JsonValue* json_parse_array(JsonContext* ctx) {
    if (json_current_char(ctx) != '[') {
        json_set_error(ctx, "Expected '['");
        return NULL;
    }
    json_advance(ctx);
    
    JsonValue* array = json_create_value(JSON_ARRAY);
    array->data.array_value.capacity = 4;
    array->data.array_value.elements = shared_malloc_safe(sizeof(JsonValue*) * array->data.array_value.capacity, "libs", "unknown_function", 215);
    array->data.array_value.count = 0;
    
    json_skip_whitespace(ctx);
    
    if (json_current_char(ctx) == ']') {
        json_advance(ctx);
        return array;
    }
    
    while (!json_is_at_end(ctx)) {
        JsonValue* element = json_parse_value(ctx);
        if (ctx->has_error) {
            // free array value structure on error
            shared_free_safe(array->data.array_value.elements, "libs", "unknown_function", 228);
            shared_free_safe(array, "libs", "unknown_function", 228);
            return NULL;
        }
        
        if (array->data.array_value.count >= array->data.array_value.capacity) {
            array->data.array_value.capacity *= 2;
            array->data.array_value.elements = shared_realloc_safe(array->data.array_value.elements, 
                sizeof(JsonValue*) * array->data.array_value.capacity, "libs", "json_parse_array", 236);
        }
        
        array->data.array_value.elements[array->data.array_value.count] = element;
        array->data.array_value.count++;
        
        json_skip_whitespace(ctx);
        if (json_current_char(ctx) == ']') {
            json_advance(ctx);
            break;
        } else if (json_current_char(ctx) == ',') {
            json_advance(ctx);
            json_skip_whitespace(ctx);
        } else {
            json_set_error(ctx, "Expected ',' or ']'");
            json_free(array);
            return NULL;
        }
    }
    
    return array;
}

// Parse JSON object
static JsonValue* json_parse_object(JsonContext* ctx) {
    if (json_current_char(ctx) != '{') {
        json_set_error(ctx, "Expected '{'");
        return NULL;
    }
    json_advance(ctx);
    
    JsonValue* object = json_create_value(JSON_OBJECT);
    object->data.object_value.capacity = 4;
    object->data.object_value.keys = shared_malloc_safe(sizeof(char*) * object->data.object_value.capacity, "libs", "unknown_function", 270);
    object->data.object_value.values = shared_malloc_safe(sizeof(JsonValue*) * object->data.object_value.capacity, "libs", "unknown_function", 271);
    object->data.object_value.count = 0;
    
    json_skip_whitespace(ctx);
    
    if (json_current_char(ctx) == '}') {
        json_advance(ctx);
        return object;
    }
    
    while (!json_is_at_end(ctx)) {
        JsonValue* key_value = json_parse_string(ctx);
        if (ctx->has_error) {
            json_free(object);
            return NULL;
        }
        
        char* key = shared_strdup(key_value->data.string_value);
        json_free(key_value);
        
        json_skip_whitespace(ctx);
        if (json_current_char(ctx) != ':') {
            json_set_error(ctx, "Expected ':'");
            shared_free_safe(key, "libs", "unknown_function", 292);
            json_free(object);
            return NULL;
        }
        json_advance(ctx);
        
        JsonValue* value = json_parse_value(ctx);
        if (ctx->has_error) {
            shared_free_safe(key, "libs", "unknown_function", 300);
            json_free(object);
            return NULL;
        }
        
        if (object->data.object_value.count >= object->data.object_value.capacity) {
            object->data.object_value.capacity *= 2;
            object->data.object_value.keys = shared_realloc_safe(object->data.object_value.keys, 
                sizeof(char*) * object->data.object_value.capacity, "libs", "json_parse_object", 309);
            object->data.object_value.values = shared_realloc_safe(object->data.object_value.values, 
                sizeof(JsonValue*) * object->data.object_value.capacity, "libs", "json_parse_object", 311);
        }
        
        object->data.object_value.keys[object->data.object_value.count] = key;
        object->data.object_value.values[object->data.object_value.count] = value;
        object->data.object_value.count++;
        
        json_skip_whitespace(ctx);
        if (json_current_char(ctx) == '}') {
            json_advance(ctx);
            break;
        } else if (json_current_char(ctx) == ',') {
            json_advance(ctx);
            json_skip_whitespace(ctx);
        } else {
            json_set_error(ctx, "Expected ',' or '}'");
            json_free(object);
            return NULL;
        }
    }
    
    return object;
}

// Main JSON value parser
static JsonValue* json_parse_value(JsonContext* ctx) {
    json_skip_whitespace(ctx);
    
    if (json_is_at_end(ctx)) {
        json_set_error(ctx, "Unexpected end of input");
        return NULL;
    }
    
    char c = json_current_char(ctx);
    
    switch (c) {
        case 'n': return json_parse_null(ctx);
        case 't':
        case 'f': return json_parse_boolean(ctx);
        case '"': return json_parse_string(ctx);
        case '[': return json_parse_array(ctx);
        case '{': return json_parse_object(ctx);
        case '-':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9': return json_parse_number(ctx);
        default:
            json_set_error(ctx, "Unexpected character");
            return NULL;
    }
}

// Public JSON parsing function
JsonValue* json_parse(const char* input) {
    if (!input) return NULL;
    
    JsonContext ctx = {
        .input = input,
        .position = 0,
        .length = strlen(input),
        .error_message = NULL,
        .has_error = false
    };
    
    JsonValue* result = json_parse_value(&ctx);
    
    if (ctx.has_error) {
        if (result) json_free(result);
        if (ctx.error_message) shared_free_safe(ctx.error_message, "libs", "unknown_function", 385);
        return NULL;
    }
    
    json_skip_whitespace(&ctx);
    if (!json_is_at_end(&ctx)) {
        if (result) json_free(result);
        return NULL;
    }
    
    return result;
}

// JSON stringification functions
static void json_stringify_value(const JsonValue* value, char** output, size_t* capacity, size_t* length) {
    if (!value) return;
    
    switch (value->type) {
        case JSON_NULL:
            *length += 4;
            if (*length >= *capacity) {
                *capacity = *length + 1;
                *output = shared_realloc_safe(*output, *capacity, "libs", "unknown_function", 407);
            }
            strcat(*output, "null");
            break;
            
        case JSON_BOOLEAN:
            *length += value->data.boolean_value ? 4 : 5;
            if (*length >= *capacity) {
                *capacity = *length + 1;
                *output = shared_realloc_safe(*output, *capacity, "libs", "unknown_function", 416);
            }
            strcat(*output, value->data.boolean_value ? "true" : "false");
            break;
            
        case JSON_NUMBER: {
            char number_str[64];
            snprintf(number_str, sizeof(number_str), "%.15g", value->data.number_value);
            *length += strlen(number_str);
            if (*length >= *capacity) {
                *capacity = *length + 1;
                *output = shared_realloc_safe(*output, *capacity, "libs", "unknown_function", 427);
            }
            strcat(*output, number_str);
            break;
        }
        
        case JSON_STRING: {
            *length += strlen(value->data.string_value) + 2; // +2 for quotes
            if (*length >= *capacity) {
                *capacity = *length + 1;
                *output = shared_realloc_safe(*output, *capacity, "libs", "unknown_function", 437);
            }
            strcat(*output, "\"");
            strcat(*output, value->data.string_value);
            strcat(*output, "\"");
            break;
        }
        
        case JSON_ARRAY: {
            *length += 2; // for []
            if (*length >= *capacity) {
                *capacity = *length + 1;
                *output = shared_realloc_safe(*output, *capacity, "libs", "unknown_function", 449);
            }
            strcat(*output, "[");
            
            for (size_t i = 0; i < value->data.array_value.count; i++) {
                if (i > 0) {
                    *length += 1;
                    if (*length >= *capacity) {
                        *capacity = *length + 1;
                        *output = shared_realloc_safe(*output, *capacity, "libs", "unknown_function", 458);
                    }
                    strcat(*output, ",");
                }
                json_stringify_value(value->data.array_value.elements[i], output, capacity, length);
            }
            
            *length += 1;
            if (*length >= *capacity) {
                *capacity = *length + 1;
                *output = shared_realloc_safe(*output, *capacity, "libs", "unknown_function", 468);
            }
            strcat(*output, "]");
            break;
        }
        
        case JSON_OBJECT: {
            *length += 2; // for {}
            if (*length >= *capacity) {
                *capacity = *length + 1;
                *output = shared_realloc_safe(*output, *capacity, "libs", "unknown_function", 478);
            }
            strcat(*output, "{");
            
            for (size_t i = 0; i < value->data.object_value.count; i++) {
                if (i > 0) {
                    *length += 1;
                    if (*length >= *capacity) {
                        *capacity = *length + 1;
                        *output = shared_realloc_safe(*output, *capacity, "libs", "unknown_function", 487);
                    }
                    strcat(*output, ",");
                }
                
                // Key
                *length += strlen(value->data.object_value.keys[i]) + 2;
                if (*length >= *capacity) {
                    *capacity = *length + 1;
                    *output = shared_realloc_safe(*output, *capacity, "libs", "unknown_function", 496);
                }
                strcat(*output, "\"");
                strcat(*output, value->data.object_value.keys[i]);
                strcat(*output, "\":");
                
                // Value
                json_stringify_value(value->data.object_value.values[i], output, capacity, length);
            }
            
            *length += 1;
            if (*length >= *capacity) {
                *capacity = *length + 1;
                *output = shared_realloc_safe(*output, *capacity, "libs", "unknown_function", 509);
            }
            strcat(*output, "}");
            break;
        }
    }
}

char* json_stringify(const JsonValue* value) {
    if (!value) return NULL;
    
    size_t capacity = 256;
    size_t length = 0;
    char* output = shared_malloc_safe(capacity, "libs", "unknown_function", 522);
    output[0] = '\0';
    
    json_stringify_value(value, &output, &capacity, &length);
    
    return output;
}

// JSON validation
bool json_validate(const char* input) {
    JsonValue* parsed = json_parse(input);
    if (parsed) {
        json_free(parsed);
        return true;
    }
    return false;
}

// JSON memory management
void json_free(JsonValue* value) {
    if (!value) return;
    
    switch (value->type) {
        case JSON_STRING:
            if (value->data.string_value) {
                shared_free_safe(value->data.string_value, "libs", "unknown_function", 547);
            }
            break;
            
        case JSON_ARRAY:
            for (size_t i = 0; i < value->data.array_value.count; i++) {
                json_free(value->data.array_value.elements[i]);
            }
            if (value->data.array_value.elements) {
                shared_free_safe(value->data.array_value.elements, "libs", "unknown_function", 556);
            }
            break;
            
        case JSON_OBJECT:
            for (size_t i = 0; i < value->data.object_value.count; i++) {
                shared_free_safe(value->data.object_value.keys[i], "libs", "unknown_function", 562);
                json_free(value->data.object_value.values[i]);
            }
            if (value->data.object_value.keys) {
                shared_free_safe(value->data.object_value.keys, "libs", "unknown_function", 566);
            }
            if (value->data.object_value.values) {
                shared_free_safe(value->data.object_value.values, "libs", "unknown_function", 569);
            }
            break;
            
        default:
            break;
    }
    
    shared_free_safe(value, "libs", "unknown_function", 577);
}

// Myco library functions
Value builtin_json_parse(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_ARGUMENT_COUNT, "json", "unknown_function", "json.parse() requires exactly 1 argument (json_string)", line, column);
        return value_create_null();
    }
    
    Value json_string = args[0];
    if (json_string.type != VALUE_STRING) {
        std_error_report(ERROR_INVALID_ARGUMENT, "json", "unknown_function", "json.parse() argument must be a string", line, column);
        return value_create_null();
    }
    
    // Use the existing JSON parser infrastructure
    const char* json_str = json_string.data.string_value;
    JsonContext context = {0};
    context.input = json_str;
    context.position = 0;
    context.length = strlen(json_str);
    context.error_message = NULL;
    context.has_error = false;
    
    // Parse the JSON string
    JsonValue* json_result = json_parse_value(&context);
    if (!json_result || context.has_error) {
        if (context.error_message) {
            std_error_report(ERROR_INVALID_ARGUMENT, "json", "unknown_function", context.error_message, line, column);
            shared_free_safe(context.error_message, "libs", "json_parse", 0);
        } else {
            std_error_report(ERROR_INVALID_ARGUMENT, "json", "unknown_function", "Invalid JSON format", line, column);
        }
        return value_create_null();
    }
    
    // Convert JsonValue to Myco Value
    Value result = json_to_myco_value(json_result);
    
    // Clean up the JsonValue
    json_free(json_result);
    
    return result;
}

// Dynamic string builder for JSON
typedef struct {
    char* buffer;
    size_t capacity;
    size_t length;
} JsonStringBuilder;

static JsonStringBuilder* json_string_builder_create(size_t initial_capacity) {
    JsonStringBuilder* builder = shared_malloc_safe(sizeof(JsonStringBuilder), "libs", "json_string_builder_create", 0);
    if (!builder) return NULL;
    
    builder->capacity = initial_capacity > 0 ? initial_capacity : 64;
    builder->buffer = shared_malloc_safe(builder->capacity, "libs", "json_string_builder_create", 0);
    if (!builder->buffer) {
        shared_free_safe(builder, "libs", "json_string_builder_create", 0);
        return NULL;
    }
    builder->length = 0;
    builder->buffer[0] = '\0';
    
    return builder;
}

static void json_string_builder_append(JsonStringBuilder* builder, const char* str) {
    if (!builder || !str) return;
    
    size_t str_len = strlen(str);
    if (builder->length + str_len + 1 > builder->capacity) {
        // Double capacity
        builder->capacity = (builder->capacity * 2) + str_len + 1;
        builder->buffer = shared_realloc_safe(builder->buffer, builder->capacity, "libs", "json_string_builder_append", 0);
    }
    
    strcat(builder->buffer, str);
    builder->length += str_len;
}

static void json_string_builder_append_char(JsonStringBuilder* builder, char c) {
    if (!builder) return;
    
    if (builder->length + 2 > builder->capacity) {
        builder->capacity *= 2;
        builder->buffer = shared_realloc_safe(builder->buffer, builder->capacity, "libs", "json_string_builder_append_char", 0);
    }
    
    builder->buffer[builder->length] = c;
    builder->buffer[builder->length + 1] = '\0';
    builder->length++;
}

static void json_string_builder_free(JsonStringBuilder* builder) {
    if (!builder) return;
    shared_free_safe(builder->buffer, "libs", "json_string_builder_free", 0);
    shared_free_safe(builder, "libs", "json_string_builder_free", 0);
}

// Escape string for JSON
static char* json_escape_string(const char* str) {
    if (!str) return NULL;
    
    size_t len = strlen(str);
    size_t escaped_len = len;
    
    // Count characters that need escaping
    for (size_t i = 0; i < len; i++) {
        if (str[i] == '"' || str[i] == '\\' || str[i] == '\n' || str[i] == '\r' || str[i] == '\t' || str[i] < 32) {
            escaped_len += 1; // Will become \x
        }
    }
    
    char* escaped = shared_malloc_safe(escaped_len + 1, "libs", "json_escape_string", 0);
    if (!escaped) return NULL;
    
    size_t j = 0;
    for (size_t i = 0; i < len; i++) {
        switch (str[i]) {
            case '"':  escaped[j++] = '\\'; escaped[j++] = '"'; break;
            case '\\': escaped[j++] = '\\'; escaped[j++] = '\\'; break;
            case '\n': escaped[j++] = '\\'; escaped[j++] = 'n'; break;
            case '\r': escaped[j++] = '\\'; escaped[j++] = 'r'; break;
            case '\t': escaped[j++] = '\\'; escaped[j++] = 't'; break;
            default:
                if (str[i] < 32) {
                    char hex[5];
                    snprintf(hex, sizeof(hex), "\\u%04x", (unsigned char)str[i]);
                    escaped[j++] = hex[0];
                    escaped[j++] = hex[1];
                    escaped[j++] = hex[2];
                    escaped[j++] = hex[3];
                    escaped[j++] = hex[4];
                } else {
                    escaped[j++] = str[i];
                }
                break;
        }
    }
    escaped[j] = '\0';
    
    return escaped;
}

// Recursive JSON stringification
static void json_stringify_myco_value(JsonStringBuilder* builder, Value* value) {
    if (!builder || !value) return;
    
    switch (value->type) {
        case VALUE_NULL:
            json_string_builder_append(builder, "null");
            break;
            
        case VALUE_BOOLEAN:
            json_string_builder_append(builder, value->data.boolean_value ? "true" : "false");
            break;
            
        case VALUE_NUMBER:
            {
                    char num_str[64];
                snprintf(num_str, sizeof(num_str), "%.15g", value->data.number_value);
                json_string_builder_append(builder, num_str);
            }
            break;
            
        case VALUE_STRING:
            {
                char* escaped = json_escape_string(value->data.string_value);
                if (escaped) {
                    json_string_builder_append_char(builder, '"');
                    json_string_builder_append(builder, escaped);
                    json_string_builder_append_char(builder, '"');
                    shared_free_safe(escaped, "libs", "json_stringify_value", 0);
                } else {
                    json_string_builder_append(builder, "\"\"");
                }
            }
            break;
            
        case VALUE_ARRAY:
            json_string_builder_append_char(builder, '[');
            for (size_t i = 0; i < value->data.array_value.count; i++) {
                if (i > 0) json_string_builder_append_char(builder, ',');
                json_stringify_myco_value(builder, (Value*)value->data.array_value.elements[i]);
            }
            json_string_builder_append_char(builder, ']');
            break;
            
        case VALUE_OBJECT:
            json_string_builder_append_char(builder, '{');
            for (size_t i = 0; i < value->data.object_value.count; i++) {
                if (i > 0) json_string_builder_append_char(builder, ',');
                
                // Add key
                char* key = value->data.object_value.keys[i];
                if (key && strlen(key) > 0) {
                    char* escaped_key = json_escape_string(key);
                    if (escaped_key) {
                        json_string_builder_append_char(builder, '"');
                        json_string_builder_append(builder, escaped_key);
                        json_string_builder_append_char(builder, '"');
                        shared_free_safe(escaped_key, "libs", "json_stringify_myco_value", 0);
    } else {
                        json_string_builder_append(builder, "\"\"");
                    }
                } else {
                    // Use index as key
                    char index_key[32];
                    snprintf(index_key, sizeof(index_key), "\"key_%zu\"", i);
                    json_string_builder_append(builder, index_key);
                }
                
                json_string_builder_append_char(builder, ':');
                
                // Add value
                json_stringify_myco_value(builder, (Value*)value->data.object_value.values[i]);
            }
            json_string_builder_append_char(builder, '}');
            break;
            
        case VALUE_HASH_MAP:
            json_string_builder_append_char(builder, '{');
            for (size_t i = 0; i < value->data.hash_map_value.count; i++) {
                if (i > 0) json_string_builder_append_char(builder, ',');
                
                // Add key - hash map keys are Value objects, need to convert to string
                Value* key_value = (Value*)value->data.hash_map_value.keys[i];
                if (key_value && key_value->type == VALUE_STRING) {
                    char* escaped_key = json_escape_string(key_value->data.string_value);
                    if (escaped_key) {
                        json_string_builder_append_char(builder, '"');
                        json_string_builder_append(builder, escaped_key);
                        json_string_builder_append_char(builder, '"');
                        shared_free_safe(escaped_key, "libs", "json_stringify_myco_value", 0);
                } else {
                        json_string_builder_append(builder, "\"\"");
                    }
                } else {
                    // Use index as key if key is not a string
                    char index_key[32];
                    snprintf(index_key, sizeof(index_key), "\"key_%zu\"", i);
                    json_string_builder_append(builder, index_key);
                }
                
                json_string_builder_append_char(builder, ':');
                
                // Add value
                json_stringify_myco_value(builder, (Value*)value->data.hash_map_value.values[i]);
            }
            json_string_builder_append_char(builder, '}');
            break;
            
        default:
            json_string_builder_append(builder, "\"unknown\"");
            break;
    }
}

Value builtin_json_stringify(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_ARGUMENT_COUNT, "json", "unknown_function", "json.stringify() requires exactly 1 argument (value)", line, column);
        return value_create_null();
    }
    
    Value myco_value = args[0];
    
    // Create dynamic string builder
    JsonStringBuilder* builder = json_string_builder_create(256);
    if (!builder) {
        std_error_report(ERROR_INTERNAL_ERROR, "json", "unknown_function", "Failed to create JSON string builder", line, column);
        return value_create_null();
    }
    
    // Convert Myco value to JSON string
    json_stringify_myco_value(builder, &myco_value);
    
    // Create result string (copy the buffer content)
    char* json_string = shared_strdup(builder->buffer);
    if (!json_string) {
        shared_free_safe(builder, "libs", "builtin_json_stringify", 0);
        return value_create_null();
    }
    Value result = value_create_string(json_string);
    shared_free_safe(json_string, "libs", "builtin_json_stringify", 0);
    
    // Clean up builder structure and buffer
    shared_free_safe(builder->buffer, "libs", "builtin_json_stringify", 0);
    shared_free_safe(builder, "libs", "builtin_json_stringify", 0);
    
    return result;
}

Value builtin_json_validate(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_ARGUMENT_COUNT, "json", "unknown_function", "json.validate() requires exactly 1 argument (json_string)", line, column);
        return value_create_null();
    }
    
    Value json_string = args[0];
    if (json_string.type != VALUE_STRING) {
        std_error_report(ERROR_INVALID_ARGUMENT, "json", "unknown_function", "json.validate() argument must be a string", line, column);
        return value_create_null();
    }
    
    // Simple validation - check for basic JSON structure
    const char* str = json_string.data.string_value;
    bool is_valid = false;
    
    if (str && strlen(str) > 0) {
        // Check if it starts and ends with proper brackets/braces
        char first = str[0];
        char last = str[strlen(str) - 1];
        if ((first == '{' && last == '}') || (first == '[' && last == ']')) {
            is_valid = true;
        }
    }
    
    return value_create_boolean(is_valid);
}

// Convert Myco Value to JSON Value
JsonValue* json_from_myco_value(const Value* value) {
    if (!value) return NULL;
    
    JsonValue* json_value = shared_malloc_safe(sizeof(JsonValue), "libs", "unknown_function", 747);
    if (!json_value) return NULL;
    
    // Initialize to null by default
    json_value->type = JSON_NULL;
    
    switch (value->type) {
        case VALUE_NULL:
            json_value->type = JSON_NULL;
            break;
            
        case VALUE_BOOLEAN:
            json_value->type = JSON_BOOLEAN;
            json_value->data.boolean_value = value->data.boolean_value;
            break;
            
        case VALUE_NUMBER:
            json_value->type = JSON_NUMBER;
            json_value->data.number_value = value->data.number_value;
            break;
            
        case VALUE_STRING:
            json_value->type = JSON_STRING;
            json_value->data.string_value = (value->data.string_value ? strdup(value->data.string_value) : NULL);
            break;
            
        case VALUE_ARRAY: {
            json_value->type = JSON_ARRAY;
            json_value->data.array_value.count = value->data.array_value.count;
            json_value->data.array_value.capacity = value->data.array_value.capacity;
            json_value->data.array_value.elements = shared_malloc_safe(sizeof(JsonValue*) * value->data.array_value.capacity, "libs", "unknown_function", 777);
            
            for (size_t i = 0; i < value->data.array_value.count; i++) {
                Value* element = (Value*)value->data.array_value.elements[i];
                json_value->data.array_value.elements[i] = json_from_myco_value(element);
            }
            break;
        }
        
        case VALUE_OBJECT: {
            json_value->type = JSON_OBJECT;
            json_value->data.object_value.count = value->data.object_value.count;
            json_value->data.object_value.capacity = value->data.object_value.capacity;
            json_value->data.object_value.keys = shared_malloc_safe(sizeof(char*) * value->data.object_value.capacity, "libs", "unknown_function", 790);
            json_value->data.object_value.values = shared_malloc_safe(sizeof(JsonValue*) * value->data.object_value.capacity, "libs", "unknown_function", 791);
            
            for (size_t i = 0; i < value->data.object_value.count; i++) {
                json_value->data.object_value.keys[i] = (value->data.object_value.keys[i] ? strdup(value->data.object_value.keys[i]) : NULL);
                Value* val = (Value*)value->data.object_value.values[i];
                json_value->data.object_value.values[i] = json_from_myco_value(val);
            }
            break;
        }
        
        default:
            shared_free_safe(json_value, "libs", "unknown_function", 802);
            return NULL;
    }
    
    return json_value;
}

// Convert JSON Value to Myco Value
Value json_to_myco_value(const JsonValue* json_value) {
    if (!json_value) return value_create_null();
    
    switch (json_value->type) {
        case JSON_NULL:
            return value_create_null();
            
        case JSON_BOOLEAN:
            return value_create_boolean(json_value->data.boolean_value);
            
        case JSON_NUMBER:
            return value_create_number(json_value->data.number_value);
            
        case JSON_STRING:
            return value_create_string(json_value->data.string_value);
            
        case JSON_ARRAY: {
            Value array = value_create_array(json_value->data.array_value.count);
            for (size_t i = 0; i < json_value->data.array_value.count; i++) {
                Value element = json_to_myco_value(json_value->data.array_value.elements[i]);
                value_array_push(&array, element);
                // Don't free element - value_array_push takes ownership
            }
            return array;
        }
        
        case JSON_OBJECT: {
            Value object = value_create_object(json_value->data.object_value.capacity);
            for (size_t i = 0; i < json_value->data.object_value.count; i++) {
                Value val = json_to_myco_value(json_value->data.object_value.values[i]);
                value_object_set_member(&object, json_value->data.object_value.keys[i], val);
                // Don't free val here - value_object_set_member takes ownership
            }
            return object;
        }
        
        default:
            return value_create_null();
    }
}

// Placeholder implementations for other functions
Value builtin_json_pretty(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    // For now, just return the stringified version
    return builtin_json_stringify(interpreter, args, arg_count, line, column);
}

Value builtin_json_merge(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        std_error_report(ERROR_ARGUMENT_COUNT, "json", "unknown_function", "json.merge() requires exactly 2 arguments (obj1, obj2)", line, column);
        return value_create_null();
    }
    
    // For now, return the first object
    return args[0];
}

Value builtin_json_extract(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        std_error_report(ERROR_ARGUMENT_COUNT, "json", "unknown_function", "json.extract() requires exactly 2 arguments (value, path)", line, column);
        return value_create_null();
    }
    
    // For now, return the value
    return args[0];
}

Value builtin_json_get(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        std_error_report(ERROR_ARGUMENT_COUNT, "json", "unknown_function", "json.get() requires exactly 2 arguments (object, key)", line, column);
        return value_create_null();
    }
    
    // For now, return null
    return value_create_null();
}

Value builtin_json_set(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 3) {
        std_error_report(ERROR_ARGUMENT_COUNT, "json", "unknown_function", "json.set() requires exactly 3 arguments (object, key, value)", line, column);
        return value_create_null();
    }
    
    // For now, return the object
    return args[0];
}

Value builtin_json_size(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_ARGUMENT_COUNT, "json", "unknown_function", "json.size() requires exactly 1 argument (value)", line, column);
        return value_create_null();
    }
    
    Value value = args[0];
    if (value.type == VALUE_ARRAY) {
        return value_create_number(value.data.array_value.count);
    } else if (value.type == VALUE_OBJECT) {
        return value_create_number(value.data.object_value.count);
    }
    
    return value_create_number(0);
}

Value builtin_json_is_empty(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_ARGUMENT_COUNT, "json", "unknown_function", "json.is_empty() requires exactly 1 argument (value)", line, column);
        return value_create_null();
    }
    
    Value value = args[0];
    if (value.type == VALUE_ARRAY) {
        return value_create_boolean(value.data.array_value.count == 0);
    } else if (value.type == VALUE_OBJECT) {
        return value_create_boolean(value.data.object_value.count == 0);
    }
    
    return value_create_boolean(true);
}

// Register JSON library with interpreter
void json_library_register(Interpreter* interpreter) {
    if (!interpreter || !interpreter->global_environment) return;

    // Create JSON library object
    Value json_lib = value_create_object(16);
    // Mark as Library for .type reporting
    value_object_set(&json_lib, "__type__", value_create_string("Library"));
    value_object_set(&json_lib, "type", value_create_string("Library"));

    // Register core functions
    value_object_set(&json_lib, "parse", value_create_builtin_function(builtin_json_parse));
    value_object_set(&json_lib, "stringify", value_create_builtin_function(builtin_json_stringify));
    value_object_set(&json_lib, "validate", value_create_builtin_function(builtin_json_validate));
    value_object_set(&json_lib, "pretty", value_create_builtin_function(builtin_json_pretty));
    value_object_set(&json_lib, "merge", value_create_builtin_function(builtin_json_merge));
    value_object_set(&json_lib, "extract", value_create_builtin_function(builtin_json_extract));
    value_object_set(&json_lib, "get", value_create_builtin_function(builtin_json_get));
    value_object_set(&json_lib, "set", value_create_builtin_function(builtin_json_set));
    value_object_set(&json_lib, "size", value_create_builtin_function(builtin_json_size));
    value_object_set(&json_lib, "isEmpty", value_create_builtin_function(builtin_json_is_empty));

    // Register the library in global environment
    environment_define(interpreter->global_environment, "json", json_lib);
}
