#include "../../include/libs/regex.h"
#include "../../include/libs/builtin_libs.h"
#include "../../include/core/interpreter.h"
#include "../../include/core/ast.h"
#include <stdio.h>

// Helper function to convert Myco regex flags to POSIX regex flags
int convert_flags(int myco_flags) {
    int posix_flags = REG_EXTENDED;
    
    if (myco_flags & REGEX_FLAG_CASE_INSENSITIVE) {
        posix_flags |= REG_ICASE;
    }
    
    if (myco_flags & REGEX_FLAG_MULTILINE) {
        posix_flags |= REG_NEWLINE;
    }
    
    return posix_flags;
}

// Create a new regex match result
RegexMatch* regex_create_match() {
    RegexMatch* match = malloc(sizeof(RegexMatch));
    if (!match) return NULL;
    
    match->match = NULL;
    match->start = -1;
    match->end = -1;
    match->groups = NULL;
    match->group_count = 0;
    match->success = false;
    
    return match;
}

// Test if a regex pattern is valid
bool regex_is_valid_pattern(const char* pattern) {
    if (!pattern) return false;
    
    regex_t regex;
    int result = regcomp(&regex, pattern, REG_EXTENDED);
    regfree(&regex);
    
    return result == 0;
}

// Escape special regex characters in text
char* regex_escape(const char* text) {
    if (!text) return NULL;
    
    size_t len = strlen(text);
    char* escaped = malloc(len * 2 + 1); // Worst case: every char needs escaping
    if (!escaped) return NULL;
    
    size_t j = 0;
    for (size_t i = 0; i < len; i++) {
        char c = text[i];
        if (c == '.' || c == '*' || c == '+' || c == '?' || c == '^' || 
            c == '$' || c == '|' || c == '\\' || c == '(' || c == ')' || 
            c == '[' || c == ']' || c == '{' || c == '}') {
            escaped[j++] = '\\';
        }
        escaped[j++] = c;
    }
    escaped[j] = '\0';
    
    return escaped;
}

// Match a pattern against text (first match only)
RegexMatch* regex_match(const char* pattern, const char* text, int flags) {
    if (!pattern) return NULL;
    if (!text) text = ""; // Handle empty strings gracefully
    
    RegexMatch* match = regex_create_match();
    if (!match) return NULL;
    
    regex_t regex;
    int posix_flags = convert_flags(flags);
    
    if (regcomp(&regex, pattern, posix_flags) != 0) {
        free(match);
        return NULL;
    }
    
    regmatch_t pmatch[10]; // Support up to 9 groups
    int result = regexec(&regex, text, 10, pmatch, 0);
    
    if (result == 0) {
        match->success = true;
        match->start = pmatch[0].rm_so;
        match->end = pmatch[0].rm_eo;
        
        // Extract the main match
        int match_len = pmatch[0].rm_eo - pmatch[0].rm_so;
        match->match = malloc(match_len + 1);
        if (match->match) {
            strncpy(match->match, text + pmatch[0].rm_so, match_len);
            match->match[match_len] = '\0';
        }
        
        // Extract captured groups
        int group_count = 0;
        for (int i = 1; i < 10; i++) {
            if (pmatch[i].rm_so != -1) {
                group_count++;
            }
        }
        
        if (group_count > 0) {
            match->groups = malloc(group_count * sizeof(char*));
            match->group_count = group_count;
            
            for (int i = 0; i < group_count; i++) {
                if (pmatch[i + 1].rm_so != -1) {
                    int group_len = pmatch[i + 1].rm_eo - pmatch[i + 1].rm_so;
                    match->groups[i] = malloc(group_len + 1);
                    if (match->groups[i]) {
                        strncpy(match->groups[i], text + pmatch[i + 1].rm_so, group_len);
                        match->groups[i][group_len] = '\0';
                    }
                } else {
                    match->groups[i] = NULL;
                }
            }
        }
    }
    
    regfree(&regex);
    return match;
}

// Find all matches of a pattern in text
RegexMatch** regex_find_all(const char* pattern, const char* text, int flags, int* count) {
    if (!pattern || !text || !count) return NULL;
    
    *count = 0;
    RegexMatch** matches = malloc(100 * sizeof(RegexMatch*)); // Max 100 matches
    if (!matches) return NULL;
    
    regex_t regex;
    int posix_flags = convert_flags(flags);
    
    if (regcomp(&regex, pattern, posix_flags) != 0) {
        free(matches);
        return NULL;
    }
    
    const char* search_text = text;
    regmatch_t pmatch;
    
    while (regexec(&regex, search_text, 1, &pmatch, 0) == 0) {
        RegexMatch* match = regex_create_match();
        if (!match) break;
        
        match->success = true;
        match->start = pmatch.rm_so + (search_text - text);
        match->end = pmatch.rm_eo + (search_text - text);
        
        // Extract the match
        int match_len = pmatch.rm_eo - pmatch.rm_so;
        match->match = malloc(match_len + 1);
        if (match->match) {
            strncpy(match->match, search_text + pmatch.rm_so, match_len);
            match->match[match_len] = '\0';
        }
        
        matches[*count] = match;
        (*count)++;
        
        // Move to next position
        if (pmatch.rm_so == pmatch.rm_eo) {
            search_text += 1; // Avoid infinite loop on empty matches
        } else {
            search_text += pmatch.rm_eo;
        }
        
        // Check if we should continue (global flag)
        if (!(flags & REGEX_FLAG_GLOBAL)) {
            break;
        }
    }
    
    regfree(&regex);
    return matches;
}

// Replace all matches of a pattern with replacement text
char* regex_replace(const char* pattern, const char* text, const char* replacement, int flags) {
    if (!pattern || !text || !replacement) return NULL;
    
    RegexMatch** matches;
    int match_count;
    
    matches = regex_find_all(pattern, text, flags, &match_count);
    if (!matches || match_count == 0) {
        return strdup(text);
    }
    
    // Calculate new string length
    size_t original_len = strlen(text);
    size_t replacement_len = strlen(replacement);
    size_t new_len = original_len;
    
    for (int i = 0; i < match_count; i++) {
        if (matches[i]->success) {
            int match_len = matches[i]->end - matches[i]->start;
            new_len = new_len - match_len + replacement_len;
        }
    }
    
    char* result = malloc(new_len + 1);
    if (!result) {
        regex_free_matches(matches, match_count);
        return NULL;
    }
    
    // Build the result string
    int result_pos = 0;
    int text_pos = 0;
    
    for (int i = 0; i < match_count; i++) {
        if (matches[i]->success) {
            // Copy text before match
            int copy_len = matches[i]->start - text_pos;
            strncpy(result + result_pos, text + text_pos, copy_len);
            result_pos += copy_len;
            
            // Copy replacement
            strcpy(result + result_pos, replacement);
            result_pos += replacement_len;
            
            text_pos = matches[i]->end;
        }
    }
    
    // Copy remaining text
    strcpy(result + result_pos, text + text_pos);
    
    regex_free_matches(matches, match_count);
    return result;
}

// Split text by a pattern
char** regex_split(const char* pattern, const char* text, int flags, int* count) {
    if (!pattern || !text || !count) return NULL;
    
    *count = 0;
    char** parts = malloc(100 * sizeof(char*)); // Max 100 parts
    if (!parts) return NULL;
    
    RegexMatch** matches;
    int match_count;
    
    matches = regex_find_all(pattern, text, flags, &match_count);
    if (!matches) {
        // No matches, return original text as single part
        parts[0] = strdup(text);
        *count = 1;
        return parts;
    }
    
    int text_pos = 0;
    
    for (int i = 0; i < match_count; i++) {
        if (matches[i]->success) {
            // Add part before match
            if (matches[i]->start > text_pos) {
                int part_len = matches[i]->start - text_pos;
                parts[*count] = malloc(part_len + 1);
                if (parts[*count]) {
                    strncpy(parts[*count], text + text_pos, part_len);
                    parts[*count][part_len] = '\0';
                    (*count)++;
                }
            }
            text_pos = matches[i]->end;
        }
    }
    
    // Add remaining text
    if (text_pos < (int)strlen(text)) {
        parts[*count] = strdup(text + text_pos);
        (*count)++;
    }
    
    regex_free_matches(matches, match_count);
    return parts;
}

// Test if pattern matches text
bool regex_test(const char* pattern, const char* text, int flags) {
    if (!pattern) return false;
    if (!text) text = ""; // Handle empty strings gracefully
    
    regex_t regex;
    int posix_flags = convert_flags(flags);
    
    if (regcomp(&regex, pattern, posix_flags) != 0) {
        return false;
    }
    
    int result = regexec(&regex, text, 0, NULL, 0);
    regfree(&regex);
    
    return result == 0;
}

// Extract all matches as strings
char** regex_extract(const char* pattern, const char* text, int flags, int* count) {
    if (!pattern || !text || !count) return NULL;
    
    RegexMatch** matches;
    int match_count;
    
    matches = regex_find_all(pattern, text, flags, &match_count);
    if (!matches) {
        *count = 0;
        return NULL;
    }
    
    char** results = malloc(match_count * sizeof(char*));
    if (!results) {
        regex_free_matches(matches, match_count);
        *count = 0;
        return NULL;
    }
    
    *count = match_count;
    for (int i = 0; i < match_count; i++) {
        results[i] = matches[i]->match ? strdup(matches[i]->match) : NULL;
    }
    
    regex_free_matches(matches, match_count);
    return results;
}

// Common pattern validation functions
bool regex_is_email(const char* text) {
    const char* email_pattern = "^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$";
    return regex_test(email_pattern, text, REGEX_FLAG_CASE_INSENSITIVE);
}

bool regex_is_url(const char* text) {
    const char* url_pattern = "^(https?|ftp)://[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}(/.*)?$";
    return regex_test(url_pattern, text, REGEX_FLAG_CASE_INSENSITIVE);
}

bool regex_is_phone(const char* text) {
    const char* phone_pattern = "^\\+?[1-9]\\d{1,14}$";
    return regex_test(phone_pattern, text, 0);
}

bool regex_is_ip_address(const char* text) {
    // Simplified IP pattern that works with POSIX regex (no lookahead)
    const char* ip_pattern = "^([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3})$";
    
    if (!regex_test(ip_pattern, text, 0)) {
        return false;
    }
    
    // Additional validation for IP ranges (0-255)
    // This is a simplified check - in a real implementation, you'd parse the groups
    // For now, we'll use a more permissive pattern that catches most invalid cases
    const char* strict_ip_pattern = "^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$";
    return regex_test(strict_ip_pattern, text, 0);
}

bool regex_is_hex_color(const char* text) {
    const char* hex_pattern = "^#([A-Fa-f0-9]{6}|[A-Fa-f0-9]{3})$";
    return regex_test(hex_pattern, text, 0);
}

// Memory management functions
void regex_free_match(RegexMatch* match) {
    if (!match) return;
    
    if (match->match) free(match->match);
    
    if (match->groups) {
        for (int i = 0; i < match->group_count; i++) {
            if (match->groups[i]) free(match->groups[i]);
        }
        free(match->groups);
    }
    
    free(match);
}

void regex_free_matches(RegexMatch** matches, int count) {
    if (!matches) return;
    
    for (int i = 0; i < count; i++) {
        regex_free_match(matches[i]);
    }
    
    free(matches);
}

void regex_free_strings(char** strings, int count) {
    if (!strings) return;
    
    for (int i = 0; i < count; i++) {
        if (strings[i]) free(strings[i]);
    }
    
    free(strings);
}

// Myco library functions
Value builtin_regex_match(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 2 || arg_count > 3) {
        interpreter_set_error(interpreter, "regex.match() requires 2-3 arguments (pattern, text, [flags])", line, column);
        return value_create_null();
    }
    
    Value pattern_val = args[0];
    Value text_val = args[1];
    int flags = 0;
    
    if (arg_count == 3) {
        Value flags_val = args[2];
        if (flags_val.type == VALUE_NUMBER) {
            flags = (int)flags_val.data.number_value;
        }
    }
    
    if (pattern_val.type != VALUE_STRING || text_val.type != VALUE_STRING) {
        interpreter_set_error(interpreter, "regex.match() arguments must be strings", line, column);
        return value_create_null();
    }
    
    RegexMatch* match = regex_match(pattern_val.data.string_value, text_val.data.string_value, flags);
    if (!match) {
        return value_create_null();
    }
    
    if (!match->success) {
        regex_free_match(match);
        return value_create_null();
    }
    
    // Create result object
    Value result = value_create_object(16);
    value_object_set(&result, "match", value_create_string(match->match));
    value_object_set(&result, "start", value_create_number(match->start));
    value_object_set(&result, "end", value_create_number(match->end));
    value_object_set(&result, "success", value_create_boolean(true));
    
    // Add groups if any
    if (match->group_count > 0) {
        Value groups = value_create_array(match->group_count);
        for (int i = 0; i < match->group_count; i++) {
            groups.data.array_value.elements[i] = malloc(sizeof(Value));
            *((Value*)groups.data.array_value.elements[i]) = value_create_string(match->groups[i]);
        }
        groups.data.array_value.count = match->group_count;
        value_object_set(&result, "groups", groups);
    }
    
    regex_free_match(match);
    return result;
}

Value builtin_regex_test(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 2 || arg_count > 3) {
        interpreter_set_error(interpreter, "regex.test() requires 2-3 arguments (pattern, text, [flags])", line, column);
        return value_create_null();
    }
    
    Value pattern_val = args[0];
    Value text_val = args[1];
    int flags = 0;
    
    if (arg_count == 3) {
        Value flags_val = args[2];
        if (flags_val.type == VALUE_NUMBER) {
            flags = (int)flags_val.data.number_value;
        }
    }
    
    if (pattern_val.type != VALUE_STRING || text_val.type != VALUE_STRING) {
        interpreter_set_error(interpreter, "regex.test() arguments must be strings", line, column);
        return value_create_null();
    }
    
    bool result = regex_test(pattern_val.data.string_value, text_val.data.string_value, flags);
    return value_create_boolean(result);
}

Value builtin_regex_is_email(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "regex.is_email() requires exactly 1 argument (text)", line, column);
        return value_create_null();
    }
    
    Value text_val = args[0];
    if (text_val.type != VALUE_STRING) {
        interpreter_set_error(interpreter, "regex.is_email() argument must be a string", line, column);
        return value_create_null();
    }
    
    bool result = regex_is_email(text_val.data.string_value);
    return value_create_boolean(result);
}

Value builtin_regex_is_url(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "regex.is_url() requires exactly 1 argument (text)", line, column);
        return value_create_null();
    }
    
    Value text_val = args[0];
    if (text_val.type != VALUE_STRING) {
        interpreter_set_error(interpreter, "regex.is_url() argument must be a string", line, column);
        return value_create_null();
    }
    
    bool result = regex_is_url(text_val.data.string_value);
    return value_create_boolean(result);
}

Value builtin_regex_is_ip(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "regex.is_ip() requires exactly 1 argument (text)", line, column);
        return value_create_null();
    }
    
    Value text_val = args[0];
    if (text_val.type != VALUE_STRING) {
        interpreter_set_error(interpreter, "regex.is_ip() argument must be a string", line, column);
        return value_create_null();
    }
    
    bool result = regex_is_ip_address(text_val.data.string_value);
    return value_create_boolean(result);
}

// Register regex library with interpreter
void regex_library_register(Interpreter* interpreter) {
    if (!interpreter || !interpreter->global_environment) return;
    
    // Create regex library object
    Value regex_lib = value_create_object(16);
    
    // Register core functions
    value_object_set(&regex_lib, "match", value_create_builtin_function(builtin_regex_match));
    value_object_set(&regex_lib, "test", value_create_builtin_function(builtin_regex_test));
    
    // Register validation functions
    value_object_set(&regex_lib, "is_email", value_create_builtin_function(builtin_regex_is_email));
    value_object_set(&regex_lib, "is_url", value_create_builtin_function(builtin_regex_is_url));
    value_object_set(&regex_lib, "is_ip", value_create_builtin_function(builtin_regex_is_ip));
    
    // Register flags as constants
    value_object_set(&regex_lib, "CASE_INSENSITIVE", value_create_number(REGEX_FLAG_CASE_INSENSITIVE));
    value_object_set(&regex_lib, "GLOBAL", value_create_number(REGEX_FLAG_GLOBAL));
    value_object_set(&regex_lib, "MULTILINE", value_create_number(REGEX_FLAG_MULTILINE));
    value_object_set(&regex_lib, "DOTALL", value_create_number(REGEX_FLAG_DOTALL));
    
    // Register the library in global environment
    environment_define(interpreter->global_environment, "regex", regex_lib);
}
