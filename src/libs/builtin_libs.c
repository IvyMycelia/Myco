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
    
    // Register regex library
    regex_library_register(interpreter);

    // Register json library
    json_library_register(interpreter);

    // Register http library
    http_library_register(interpreter);
    
    // Register server library
    server_library_register(interpreter);

    // Register web library
    web_library_register(interpreter);
    database_library_register(interpreter);

    // Register websocket library
    websocket_library_register(interpreter);

    // Register arduino library (host-simulated, cross-platform)
    arduino_library_register(interpreter);

    // Register graphics library
    graphics_library_register(interpreter);
}
