#ifndef MYCO_REGEX_H
#define MYCO_REGEX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <regex.h>
#include "../core/interpreter.h"

// Regex match result structure
typedef struct {
    char* match;           // The matched string
    int start;             // Start position of match
    int end;               // End position of match
    char** groups;         // Captured groups
    int group_count;       // Number of captured groups
    bool success;          // Whether match was successful
} RegexMatch;

// Regex flags
#define REGEX_FLAG_CASE_INSENSITIVE 1
#define REGEX_FLAG_GLOBAL           2
#define REGEX_FLAG_MULTILINE        4
#define REGEX_FLAG_DOTALL           8

// Core regex functions
RegexMatch* regex_match(const char* pattern, const char* text, int flags);
RegexMatch** regex_find_all(const char* pattern, const char* text, int flags, int* count);
char* regex_replace(const char* pattern, const char* text, const char* replacement, int flags);
char** regex_split(const char* pattern, const char* text, int flags, int* count);
bool regex_test(const char* pattern, const char* text, int flags);
char** regex_extract(const char* pattern, const char* text, int flags, int* count);

// Utility functions
bool regex_is_valid_pattern(const char* pattern);
char* regex_escape(const char* text);
void regex_free_match(RegexMatch* match);
void regex_free_matches(RegexMatch** matches, int count);
void regex_free_strings(char** strings, int count);

// Common pattern functions
bool regex_is_email(const char* text);
bool regex_is_url(const char* text);
bool regex_is_phone(const char* text);
bool regex_is_ip_address(const char* text);
bool regex_is_hex_color(const char* text);

// Library registration function
void regex_library_register(Interpreter* interpreter);

#endif // MYCO_REGEX_H
