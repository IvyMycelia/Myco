#include "include/core/interpreter.h"
#include "include/utils/shared_utilities.h"

int main() {
    // Initialize memory tracker
    MemoryTracker* tracker = memory_tracker_create();
    if (!tracker) {
        printf("Failed to create memory tracker\n");
        return 1;
    }
    
    // Create interpreter
    Interpreter* interpreter = interpreter_create();
    if (!interpreter) {
        printf("Failed to create interpreter\n");
        return 1;
    }
    
    // Test object creation
    Value test_obj = value_create_object(16);
    if (test_obj.type != VALUE_OBJECT) {
        printf("Object creation failed\n");
        return 1;
    }
    
    // Test setting a simple value
    value_object_set(&test_obj, "test", value_create_number(42.0));
    
    printf("Object creation and setting successful\n");
    
    // Cleanup
    value_free(&test_obj);
    interpreter_free(interpreter);
    memory_tracker_free(tracker);
    
    return 0;
}
