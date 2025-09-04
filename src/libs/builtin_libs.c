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
    
    // Register file library
    file_library_register(interpreter);
    
    // Register directory library
    dir_library_register(interpreter);
    
    // Register maps library
    maps_library_register(interpreter);
    
    // Register sets library
    sets_library_register(interpreter);
    
    // Register trees library
    trees_library_register(interpreter);
    
    // Register graphs library
    graphs_library_register(interpreter);
    
    // Register heaps library
    heaps_library_register(interpreter);
    
    // Register queues library
    queues_library_register(interpreter);
    
    // Register stacks library
    stacks_library_register(interpreter);
    
    // Register time library
    time_library_register(interpreter);
}
