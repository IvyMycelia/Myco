#include "../../include/libs/builtin_libs.h"

// Register all built-in libraries
void register_all_builtin_libraries(Interpreter* interpreter) {
    if (!interpreter) return;
    
    // Register math library
    math_library_register(interpreter);
    
    // Register string library
    string_library_register(interpreter);
    
    // Register array library
    array_library_register(interpreter);
}
