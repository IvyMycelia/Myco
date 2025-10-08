#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H

#include "interpreter.h"
#include "error_system.h"

// Enhanced error reporting with colors and formatting
void interpreter_report_error_enhanced(Interpreter* interpreter, const char* message, int line, int column);

// Error code extraction and analysis
int get_error_code_from_message(const char* message);
const char* get_fungus_error_name(int error_code);
const char* get_error_solution(int error_code);

// Error system management
void error_system_initialize(void);
void error_system_cleanup(void);
ErrorSystem* error_system_get_global(void);

// Enhanced error reporting with new error system
void interpreter_report_error_enhanced_v2(Interpreter* interpreter, int error_code, const char* message, 
                                         const char* file_name, int line, int column);

// Terminal color control for error output
void error_colors_enable(int enable);

// Print the most recent error recorded in the global error system
void error_print_last(void);

// Try/catch/finally support for interpreter
void interpreter_enter_try(Interpreter* interpreter);
void interpreter_exit_try(Interpreter* interpreter);
void interpreter_enter_catch(Interpreter* interpreter, const char* variable_name);
void interpreter_exit_catch(Interpreter* interpreter);
void interpreter_enter_finally(Interpreter* interpreter);
void interpreter_exit_finally(Interpreter* interpreter);
int interpreter_has_exception(Interpreter* interpreter);
ErrorInfo* interpreter_catch_exception(Interpreter* interpreter);
void interpreter_clear_exception(Interpreter* interpreter);

// Enhanced error handling functions
void interpreter_report_error_comprehensive(Interpreter* interpreter, int error_code, const char* message, 
                                          const char* file_name, int line, int column, const char* context);
double safe_divide(double a, double b, Interpreter* interpreter, int line, int column);
Value safe_array_access(Value* array, int index, Interpreter* interpreter, int line, int column);
void* safe_malloc(size_t size, Interpreter* interpreter, const char* context, int line, int column);
char* safe_strdup(const char* str, Interpreter* interpreter, const char* context, int line, int column);
FILE* safe_fopen(const char* filename, const char* mode, Interpreter* interpreter, int line, int column);
Value safe_object_access(Value* object, const char* property, Interpreter* interpreter, int line, int column);
Value safe_arithmetic_operation(Value* left, Value* right, char operation, Interpreter* interpreter, int line, int column);
Value safe_comparison_operation(Value* left, Value* right, char operation, Interpreter* interpreter, int line, int column);

#endif // ERROR_HANDLING_H
