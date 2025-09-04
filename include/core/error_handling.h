#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H

#include "interpreter.h"

// Enhanced error reporting with colors and formatting
void interpreter_report_error_enhanced(Interpreter* interpreter, const char* message, int line, int column);

// Error code extraction and analysis
int get_error_code_from_message(const char* message);
const char* get_fungus_error_name(int error_code);
const char* get_error_solution(int error_code);

#endif // ERROR_HANDLING_H
