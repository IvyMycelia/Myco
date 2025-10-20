#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../../include/core/interpreter.h"
#include "../../include/core/ast.h"
#include "../../include/core/standardized_errors.h"
#include "../../include/utils/shared_utilities.h"

// Forward declaration
void add_heap_methods(Value* heap);

// Forward declarations for method functions
Value builtin_heap_isEmpty_method(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_heap_insert_method(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_heap_peek_method(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_heap_extract_method(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);

// Heap utility functions
Value builtin_heap_create(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_ARGUMENT_COUNT, "unknown", "unknown_function", "create() requires exactly 1 argument: max_heap (boolean)", line, column);
        return value_create_null();
    }
    
    Value max_heap_arg = args[0];
    
    if (max_heap_arg.type != VALUE_BOOLEAN) {
        std_error_report(ERROR_INVALID_ARGUMENT, "unknown", "unknown_function", "create() argument must be a boolean (true for max heap, false for min heap)", line, column);
        return value_create_null();
    }
    
    // Create a new heap object
    Value heap = value_create_object(16);
    value_object_set_member(&heap, "__class_name__", value_create_string(("Heap" ? strdup("Heap") : NULL)));
    value_object_set(&heap, "type", value_create_string("Heap"));
    value_object_set_member(&heap, "elements", value_create_array(0));
    value_object_set_member(&heap, "is_max_heap", value_clone(&max_heap_arg));
    value_object_set_member(&heap, "size", value_create_number(0.0));
    
    // Add instance methods
    value_object_set(&heap, "isEmpty", value_create_builtin_function(builtin_heap_isEmpty_method));
    value_object_set(&heap, "insert", value_create_builtin_function(builtin_heap_insert_method));
    value_object_set(&heap, "peek", value_create_builtin_function(builtin_heap_peek_method));
    value_object_set(&heap, "extract", value_create_builtin_function(builtin_heap_extract_method));
    
    // Add methods to the heap object
    add_heap_methods(&heap);
    
    return heap;
}

Value builtin_heap_insert(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "heap.insert() expects exactly 1 argument: value", line, column);
        return value_create_null();
    }
    
    Value heap_arg = args[0];
    Value value = args[1];
    
    if (heap_arg.type != VALUE_OBJECT) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "heap.insert() can only be called on heap objects", line, column);
        return value_create_null();
    }
    
    // Get heap components
    Value elements = value_object_get(&heap_arg, "elements");
    Value is_max_heap = value_object_get(&heap_arg, "is_max_heap");
    Value size = value_object_get(&heap_arg, "size");
    
    if (elements.type != VALUE_ARRAY || is_max_heap.type != VALUE_BOOLEAN || size.type != VALUE_NUMBER) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "Invalid heap object structure", line, column);
        return value_create_null();
    }
    
    // Create a new heap with the inserted element
    Value new_heap = value_create_object(16);
    Value new_elements = value_create_array(elements.data.array_value.count + 1);
    Value new_size = value_create_number(size.data.number_value + 1.0);
    
    // Copy all existing elements
    for (size_t i = 0; i < elements.data.array_value.count; i++) {
        Value* existing_element = (Value*)elements.data.array_value.elements[i];
        if (existing_element) {
            Value cloned_element = value_clone(existing_element);
            value_array_push(&new_elements, cloned_element);
        }
    }
    
    // Add the new element
    Value cloned_value = value_clone(&value);
    value_array_push(&new_elements, cloned_value);
    
    // Heapify up
    int current_index = (int)new_elements.data.array_value.count - 1;
    while (current_index > 0) {
        int parent_index = (current_index - 1) / 2;
        Value* current_element = (Value*)new_elements.data.array_value.elements[current_index];
        Value* parent_element = (Value*)new_elements.data.array_value.elements[parent_index];
        
        if (!current_element || !parent_element) break;
        
        // Compare based on heap type
        int should_swap = 0;
        if (is_max_heap.data.boolean_value) {
            // Max heap: parent should be greater than child
            if (current_element->type == VALUE_NUMBER && parent_element->type == VALUE_NUMBER) {
                should_swap = current_element->data.number_value > parent_element->data.number_value;
            }
        } else {
            // Min heap: parent should be less than child
            if (current_element->type == VALUE_NUMBER && parent_element->type == VALUE_NUMBER) {
                should_swap = current_element->data.number_value < parent_element->data.number_value;
            }
        }
        
        if (should_swap) {
            // Swap elements
            void* temp = new_elements.data.array_value.elements[current_index];
            new_elements.data.array_value.elements[current_index] = new_elements.data.array_value.elements[parent_index];
            new_elements.data.array_value.elements[parent_index] = temp;
            current_index = parent_index;
        } else {
            break;
        }
    }
    
    // Set the new heap components
    value_object_set_member(&new_heap, "__class_name__", value_create_string(("Heap" ? strdup("Heap") : NULL)));
    value_object_set(&new_heap, "type", value_create_string("Heap"));
    value_object_set_member(&new_heap, "elements", new_elements);
    value_object_set_member(&new_heap, "is_max_heap", value_clone(&is_max_heap));
    value_object_set_member(&new_heap, "size", new_size);
    
    // Add methods to the new heap
    add_heap_methods(&new_heap);
    
    return new_heap;
}

Value builtin_heap_extract(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "heap.extract() expects exactly 0 arguments", line, column);
        return value_create_null();
    }
    
    Value heap_arg = args[0];
    
    if (heap_arg.type != VALUE_OBJECT) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "heap.extract() can only be called on heap objects", line, column);
        return value_create_null();
    }
    
    // Get heap components
    Value elements = value_object_get(&heap_arg, "elements");
    Value is_max_heap = value_object_get(&heap_arg, "is_max_heap");
    Value size = value_object_get(&heap_arg, "size");
    
    if (elements.type != VALUE_ARRAY || is_max_heap.type != VALUE_BOOLEAN || size.type != VALUE_NUMBER) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "Invalid heap object structure", line, column);
        return value_create_null();
    }
    
    if (elements.data.array_value.count == 0) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "Cannot extract from empty heap", line, column);
        return value_create_null();
    }
    
    // Get the root element (first element)
    Value* root_element = (Value*)elements.data.array_value.elements[0];
    if (!root_element) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "Invalid heap element", line, column);
        return value_create_null();
    }
    
    Value result = value_clone(root_element);
    
    // Create a new heap without the extracted element
    if (elements.data.array_value.count == 1) {
        // Return empty heap
        Value empty_heap = value_create_object(16);
        value_object_set_member(&empty_heap, "__class_name__", value_create_string(("Heap" ? strdup("Heap") : NULL)));
        value_object_set(&empty_heap, "type", value_create_string("Heap"));
        value_object_set_member(&empty_heap, "elements", value_create_array(0));
        value_object_set_member(&empty_heap, "is_max_heap", value_clone(&is_max_heap));
        value_object_set_member(&empty_heap, "size", value_create_number(0.0));
        return empty_heap;
    }
    
    // Create new heap with remaining elements
    Value new_heap = value_create_object(16);
    Value new_elements = value_create_array(elements.data.array_value.count - 1);
    Value new_size = value_create_number(size.data.number_value - 1.0);
    
    // Move last element to root position
    Value* last_element = (Value*)elements.data.array_value.elements[elements.data.array_value.count - 1];
    if (last_element) {
        Value cloned_last = value_clone(last_element);
        value_array_push(&new_elements, cloned_last);
    }
    
    // Copy remaining elements (skip first and last)
    for (size_t i = 1; i < elements.data.array_value.count - 1; i++) {
        Value* element = (Value*)elements.data.array_value.elements[i];
        if (element) {
            Value cloned_element = value_clone(element);
            value_array_push(&new_elements, cloned_element);
        }
    }
    
    // Heapify down
    int current_index = 0;
    while (current_index < (int)new_elements.data.array_value.count) {
        int left_child = 2 * current_index + 1;
        int right_child = 2 * current_index + 2;
        int target_index = current_index;
        
        // Find the target child to swap with
        if (left_child < (int)new_elements.data.array_value.count) {
            Value* current_element = (Value*)new_elements.data.array_value.elements[current_index];
            Value* left_element = (Value*)new_elements.data.array_value.elements[left_child];
            
            if (current_element && left_element && 
                current_element->type == VALUE_NUMBER && left_element->type == VALUE_NUMBER) {
                
                int should_swap_left = 0;
                if (is_max_heap.data.boolean_value) {
                    should_swap_left = left_element->data.number_value > current_element->data.number_value;
                } else {
                    should_swap_left = left_element->data.number_value < current_element->data.number_value;
                }
                
                if (should_swap_left) {
                    target_index = left_child;
                }
            }
        }
        
        if (right_child < (int)new_elements.data.array_value.count) {
            Value* current_element = (Value*)new_elements.data.array_value.elements[current_index];
            Value* right_element = (Value*)new_elements.data.array_value.elements[right_child];
            Value* target_element = (Value*)new_elements.data.array_value.elements[target_index];
            
            if (current_element && right_element && target_element &&
                current_element->type == VALUE_NUMBER && right_element->type == VALUE_NUMBER && target_element->type == VALUE_NUMBER) {
                
                int should_swap_right = 0;
                if (is_max_heap.data.boolean_value) {
                    should_swap_right = right_element->data.number_value > target_element->data.number_value;
                } else {
                    should_swap_right = right_element->data.number_value < target_element->data.number_value;
                }
                
                if (should_swap_right) {
                    target_index = right_child;
                }
            }
        }
        
        if (target_index != current_index) {
            // Swap elements
            void* temp = new_elements.data.array_value.elements[current_index];
            new_elements.data.array_value.elements[current_index] = new_elements.data.array_value.elements[target_index];
            new_elements.data.array_value.elements[target_index] = temp;
            current_index = target_index;
        } else {
            break;
        }
    }
    
    // Set the new heap components
    value_object_set_member(&new_heap, "__class_name__", value_create_string(("Heap" ? strdup("Heap") : NULL)));
    value_object_set(&new_heap, "type", value_create_string("Heap"));
    value_object_set_member(&new_heap, "elements", new_elements);
    value_object_set_member(&new_heap, "is_max_heap", value_clone(&is_max_heap));
    value_object_set_member(&new_heap, "size", new_size);
    
    // Add methods to the new heap
    add_heap_methods(&new_heap);
    
    return new_heap;
}

Value builtin_heap_peek(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "heap.peek() expects exactly 0 arguments", line, column);
        return value_create_null();
    }
    
    Value heap_arg = args[0];
    
    if (heap_arg.type != VALUE_OBJECT) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "heap.peek() can only be called on heap objects", line, column);
        return value_create_null();
    }
    
    // Get heap elements
    Value elements = value_object_get(&heap_arg, "elements");
    
    if (elements.type != VALUE_ARRAY) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "Invalid heap object structure", line, column);
        return value_create_null();
    }
    
    if (elements.data.array_value.count == 0) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "Cannot peek at empty heap", line, column);
        return value_create_null();
    }
    
    // Return the root element (first element)
    Value* root_element = (Value*)elements.data.array_value.elements[0];
    if (!root_element) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "Invalid heap element", line, column);
        return value_create_null();
    }
    
    return value_clone(root_element);
}

Value builtin_heap_size(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "heap.size() expects exactly 0 arguments", line, column);
        return value_create_null();
    }
    
    Value heap_arg = args[0];
    
    if (heap_arg.type != VALUE_OBJECT) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "heap.size() can only be called on heap objects", line, column);
        return value_create_null();
    }
    
    // Get heap size
    Value size = value_object_get(&heap_arg, "size");
    
    if (size.type != VALUE_NUMBER) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "Invalid heap object structure", line, column);
        return value_create_null();
    }
    
    return value_clone(&size);
}

Value builtin_heap_isEmpty(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "heap.isEmpty() expects exactly 0 arguments", line, column);
        return value_create_null();
    }
    
    Value heap_arg = args[0];
    
    if (heap_arg.type != VALUE_OBJECT) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "heap.isEmpty() can only be called on heap objects", line, column);
        return value_create_null();
    }
    
    // Get heap size
    Value size = value_object_get(&heap_arg, "size");
    
    if (size.type != VALUE_NUMBER) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "Invalid heap object structure", line, column);
        return value_create_null();
    }
    
    return value_create_boolean(size.data.number_value == 0.0);
}

Value builtin_heap_clear(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "heap.clear() expects exactly 0 arguments", line, column);
        return value_create_null();
    }
    
    Value heap_arg = args[0];
    
    if (heap_arg.type != VALUE_OBJECT) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "heap.clear() can only be called on heap objects", line, column);
        return value_create_null();
    }
    
    // Get heap type
    Value is_max_heap = value_object_get(&heap_arg, "is_max_heap");
    
    // Return empty heap with same type
    Value empty_heap = value_create_object(16);
    value_object_set_member(&empty_heap, "__class_name__", value_create_string(("Heap" ? strdup("Heap") : NULL)));
    value_object_set(&empty_heap, "type", value_create_string("Heap"));
    value_object_set_member(&empty_heap, "elements", value_create_array(0));
    value_object_set_member(&empty_heap, "is_max_heap", value_clone(&is_max_heap));
    value_object_set_member(&empty_heap, "size", value_create_number(0.0));
    
    return empty_heap;
}

// Register heaps library with interpreter
void heaps_library_register(Interpreter* interpreter) {
    if (!interpreter || !interpreter->global_environment) return;
    
    // Create heaps object with factory functions
    Value heaps_obj = value_create_object(16);
    value_object_set(&heaps_obj, "create", value_create_builtin_function(builtin_heap_create));
    
    // Register the heaps object
    environment_define(interpreter->global_environment, "heaps", heaps_obj);
}

// Wrapper functions for method calls
Value builtin_heap_isEmpty_method(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    // When called as heap.isEmpty(), we need to get the heap object from self_context
    fprintf(stderr, "DEBUG: builtin_heap_isEmpty_method called with %zu arguments\n", arg_count);
    if (arg_count != 0) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "heap.isEmpty() expects exactly 0 arguments", line, column);
        return value_create_null();
    }
    
    // Get the heap object from self_context
    Value* self = interpreter_get_self_context(interpreter);
    fprintf(stderr, "DEBUG: self = %p, self->type = %d\n", (void*)self, self ? self->type : -1);
    if (!self || self->type != VALUE_OBJECT) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "heap.isEmpty() can only be called on heap objects", line, column);
        return value_create_null();
    }
    
    // Call the original function with the heap object
    Value heap_args[1] = { *self };
    Value result = builtin_heap_isEmpty(interpreter, heap_args, 1, line, column);
    fprintf(stderr, "DEBUG: builtin_heap_isEmpty returned type %d\n", result.type);
    return result;
}

Value builtin_heap_insert_method(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    // When called as heap.insert(value), we need to get the heap object from self_context
    if (arg_count != 1) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "heap.insert() expects exactly 1 argument: value", line, column);
        return value_create_null();
    }
    
    // Get the heap object from self_context
    Value* self = interpreter_get_self_context(interpreter);
    if (!self || self->type != VALUE_OBJECT) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "heap.insert() can only be called on heap objects", line, column);
        return value_create_null();
    }
    
    // Call the original function with the heap object and value
    Value heap_args[2] = { *self, args[0] };
    return builtin_heap_insert(interpreter, heap_args, 2, line, column);
}

Value builtin_heap_peek_method(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 0) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "heap.peek() expects exactly 0 arguments", line, column);
        return value_create_null();
    }
    
    Value* self = interpreter_get_self_context(interpreter);
    if (!self || self->type != VALUE_OBJECT) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "heap.peek() can only be called on heap objects", line, column);
        return value_create_null();
    }
    
    Value heap_args[1] = { *self };
    return builtin_heap_peek(interpreter, heap_args, 1, line, column);
}

Value builtin_heap_extract_method(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 0) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "heap.extract() expects exactly 0 arguments", line, column);
        return value_create_null();
    }
    
    Value* self = interpreter_get_self_context(interpreter);
    if (!self || self->type != VALUE_OBJECT) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "heap.extract() can only be called on heap objects", line, column);
        return value_create_null();
    }
    
    Value heap_args[1] = { *self };
    return builtin_heap_extract(interpreter, heap_args, 1, line, column);
}

Value builtin_heap_clear_method(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 0) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "heap.clear() expects exactly 0 arguments", line, column);
        return value_create_null();
    }
    
    Value* self = interpreter_get_self_context(interpreter);
    if (!self || self->type != VALUE_OBJECT) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "heap.clear() can only be called on heap objects", line, column);
        return value_create_null();
    }
    
    Value heap_args[1] = { *self };
    return builtin_heap_clear(interpreter, heap_args, 1, line, column);
}

// Helper function to add methods to a heap object
void add_heap_methods(Value* heap) {
    value_object_set(heap, "isEmpty", value_create_builtin_function(builtin_heap_isEmpty_method));
    value_object_set(heap, "insert", value_create_builtin_function(builtin_heap_insert_method));
    value_object_set(heap, "peek", value_create_builtin_function(builtin_heap_peek_method));
    value_object_set(heap, "extract", value_create_builtin_function(builtin_heap_extract_method));
    value_object_set(heap, "clear", value_create_builtin_function(builtin_heap_clear_method));
}