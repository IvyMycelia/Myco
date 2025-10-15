#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../../include/core/interpreter.h"
#include "../../include/core/ast.h"
#include "../../include/core/standardized_errors.h"
#include "../../include/utils/shared_utilities.h"

// Forward declaration
void add_queue_methods(Value* queue);

// Queue utility functions
Value builtin_queue_create(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 0) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "create() requires no arguments", line, column);
        return value_create_null();
    }
    
    // Create a new queue object
    Value queue = value_create_object(16);
    value_object_set_member(&queue, "__class_name__", value_create_string(("Queue" ? strdup("Queue") : NULL)));
    value_object_set(&queue, "type", value_create_string("Queue"));
    value_object_set_member(&queue, "elements", value_create_array(0));
    value_object_set_member(&queue, "size", value_create_number(0.0));
    
    // Add methods to the queue object
    add_queue_methods(&queue);
    
    return queue;
}

Value builtin_queue_enqueue(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "queue.enqueue() expects exactly 1 argument: value", line, column);
        return value_create_null();
    }
    
    Value queue_arg = args[0];
    Value value = args[1];
    
    if (queue_arg.type != VALUE_OBJECT) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "queue.enqueue() can only be called on queue objects", line, column);
        return value_create_null();
    }
    
    // Get queue components
    Value elements = value_object_get(&queue_arg, "elements");
    Value size = value_object_get(&queue_arg, "size");
    
    if (elements.type != VALUE_ARRAY || size.type != VALUE_NUMBER) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "Invalid queue object structure", line, column);
        return value_create_null();
    }
    
    // Create a new queue with the enqueued element
    Value new_queue = value_create_object(16);
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
    
    // Add the new element to the end (FIFO - enqueue at back)
    Value cloned_value = value_clone(&value);
    value_array_push(&new_elements, cloned_value);
    
    // Set the new queue components
    value_object_set_member(&new_queue, "__class_name__", value_create_string(("Queue" ? strdup("Queue") : NULL)));
    value_object_set(&new_queue, "type", value_create_string("Queue"));
    value_object_set_member(&new_queue, "elements", new_elements);
    value_object_set_member(&new_queue, "size", new_size);
    
    return new_queue;
}

Value builtin_queue_dequeue(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "queue.dequeue() expects exactly 0 arguments", line, column);
        return value_create_null();
    }
    
    Value queue_arg = args[0];
    
    if (queue_arg.type != VALUE_OBJECT) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "queue.dequeue() can only be called on queue objects", line, column);
        return value_create_null();
    }
    
    // Get queue components
    Value elements = value_object_get(&queue_arg, "elements");
    Value size = value_object_get(&queue_arg, "size");
    
    if (elements.type != VALUE_ARRAY || size.type != VALUE_NUMBER) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "Invalid queue object structure", line, column);
        return value_create_null();
    }
    
    if (elements.data.array_value.count == 0) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "Cannot dequeue from empty queue", line, column);
        return value_create_null();
    }
    
    // Get the front element (first element - FIFO)
    Value* front_element = (Value*)elements.data.array_value.elements[0];
    if (!front_element) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "Invalid queue element", line, column);
        return value_create_null();
    }
    
    Value result = value_clone(front_element);
    
    // Create a new queue without the front element
    if (elements.data.array_value.count == 1) {
        // Return empty queue
        Value empty_queue = value_create_object(16);
        value_object_set_member(&empty_queue, "elements", value_create_array(0));
        value_object_set_member(&empty_queue, "size", value_create_number(0.0));
        return empty_queue;
    }
    
    // Create new queue with remaining elements (skip first element)
    Value new_queue = value_create_object(16);
    Value new_elements = value_create_array(elements.data.array_value.count - 1);
    Value new_size = value_create_number(size.data.number_value - 1.0);
    
    // Copy remaining elements (skip first element)
    for (size_t i = 1; i < elements.data.array_value.count; i++) {
        Value* element = (Value*)elements.data.array_value.elements[i];
        if (element) {
            Value cloned_element = value_clone(element);
            value_array_push(&new_elements, cloned_element);
        }
    }
    
    // Set the new queue components
    value_object_set_member(&new_queue, "__class_name__", value_create_string(("Queue" ? strdup("Queue") : NULL)));
    value_object_set(&new_queue, "type", value_create_string("Queue"));
    value_object_set_member(&new_queue, "elements", new_elements);
    value_object_set_member(&new_queue, "size", new_size);
    
    return new_queue;
}

Value builtin_queue_front(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "queue.front() expects exactly 0 arguments", line, column);
        return value_create_null();
    }
    
    Value queue_arg = args[0];
    
    if (queue_arg.type != VALUE_OBJECT) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "queue.front() can only be called on queue objects", line, column);
        return value_create_null();
    }
    
    // Get queue elements
    Value elements = value_object_get(&queue_arg, "elements");
    
    if (elements.type != VALUE_ARRAY) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "Invalid queue object structure", line, column);
        return value_create_null();
    }
    
    if (elements.data.array_value.count == 0) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "Cannot get front of empty queue", line, column);
        return value_create_null();
    }
    
    // Return the front element (first element)
    Value* front_element = (Value*)elements.data.array_value.elements[0];
    if (!front_element) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "Invalid queue element", line, column);
        return value_create_null();
    }
    
    return value_clone(front_element);
}

Value builtin_queue_back(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "queue.back() expects exactly 0 arguments", line, column);
        return value_create_null();
    }
    
    Value queue_arg = args[0];
    
    if (queue_arg.type != VALUE_OBJECT) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "queue.back() can only be called on queue objects", line, column);
        return value_create_null();
    }
    
    // Get queue elements
    Value elements = value_object_get(&queue_arg, "elements");
    
    if (elements.type != VALUE_ARRAY) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "Invalid queue object structure", line, column);
        return value_create_null();
    }
    
    if (elements.data.array_value.count == 0) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "Cannot get back of empty queue", line, column);
        return value_create_null();
    }
    
    // Return the back element (last element)
    size_t last_index = elements.data.array_value.count - 1;
    Value* back_element = (Value*)elements.data.array_value.elements[last_index];
    if (!back_element) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "Invalid queue element", line, column);
        return value_create_null();
    }
    
    return value_clone(back_element);
}

Value builtin_queue_size(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "queue.size() expects exactly 0 arguments", line, column);
        return value_create_null();
    }
    
    Value queue_arg = args[0];
    
    if (queue_arg.type != VALUE_OBJECT) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "queue.size() can only be called on queue objects", line, column);
        return value_create_null();
    }
    
    // Get queue size
    Value size = value_object_get(&queue_arg, "size");
    
    if (size.type != VALUE_NUMBER) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "Invalid queue object structure", line, column);
        return value_create_null();
    }
    
    return value_clone(&size);
}

Value builtin_queue_isEmpty(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "queue.isEmpty() expects exactly 0 arguments", line, column);
        return value_create_null();
    }
    
    Value queue_arg = args[0];
    
    if (queue_arg.type != VALUE_OBJECT) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "queue.isEmpty() can only be called on queue objects", line, column);
        return value_create_null();
    }
    
    // Get queue size
    Value size = value_object_get(&queue_arg, "size");
    
    if (size.type != VALUE_NUMBER) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "Invalid queue object structure", line, column);
        return value_create_null();
    }
    
    return value_create_boolean(size.data.number_value == 0.0);
}

Value builtin_queue_clear(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "queue.clear() expects exactly 0 arguments", line, column);
        return value_create_null();
    }
    
    Value queue_arg = args[0];
    
    if (queue_arg.type != VALUE_OBJECT) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "queue.clear() can only be called on queue objects", line, column);
        return value_create_null();
    }
    
    // Return empty queue
    Value empty_queue = value_create_object(16);
    value_object_set_member(&empty_queue, "__class_name__", value_create_string(("Queue" ? strdup("Queue") : NULL)));
    value_object_set(&empty_queue, "type", value_create_string("Queue"));
    value_object_set_member(&empty_queue, "elements", value_create_array(0));
    value_object_set_member(&empty_queue, "size", value_create_number(0.0));
    
    return empty_queue;
}

// Register queues library with interpreter
void queues_library_register(Interpreter* interpreter) {
    if (!interpreter || !interpreter->global_environment) return;
    
    // Create queues object with factory functions
    Value queues_obj = value_create_object(16);
    value_object_set(&queues_obj, "create", value_create_builtin_function(builtin_queue_create));
    
    // Register the queues object
    environment_define(interpreter->global_environment, "queues", queues_obj);
}

// Queue method functions (for object method calls with self context)
Value builtin_queue_isEmpty_method(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 0) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "queue.isEmpty() expects exactly 0 arguments", line, column);
        return value_create_null();
    }
    
    Value* self = interpreter_get_self_context(interpreter);
    if (!self || self->type != VALUE_OBJECT) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "queue.isEmpty() can only be called on queue objects", line, column);
        return value_create_null();
    }
    
    Value queue_args[1] = { *self };
    return builtin_queue_isEmpty(interpreter, queue_args, 1, line, column);
}

Value builtin_queue_enqueue_method(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "queue.enqueue() expects exactly 1 argument: value", line, column);
        return value_create_null();
    }
    
    Value* self = interpreter_get_self_context(interpreter);
    if (!self || self->type != VALUE_OBJECT) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "queue.enqueue() can only be called on queue objects", line, column);
        return value_create_null();
    }
    
    Value queue_args[2] = { *self, args[0] };
    return builtin_queue_enqueue(interpreter, queue_args, 2, line, column);
}

Value builtin_queue_dequeue_method(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 0) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "queue.dequeue() expects exactly 0 arguments", line, column);
        return value_create_null();
    }
    
    Value* self = interpreter_get_self_context(interpreter);
    if (!self || self->type != VALUE_OBJECT) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "queue.dequeue() can only be called on queue objects", line, column);
        return value_create_null();
    }
    
    Value queue_args[1] = { *self };
    return builtin_queue_dequeue(interpreter, queue_args, 1, line, column);
}

Value builtin_queue_front_method(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 0) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "queue.front() expects exactly 0 arguments", line, column);
        return value_create_null();
    }
    
    Value* self = interpreter_get_self_context(interpreter);
    if (!self || self->type != VALUE_OBJECT) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "queue.front() can only be called on queue objects", line, column);
        return value_create_null();
    }
    
    Value queue_args[1] = { *self };
    return builtin_queue_front(interpreter, queue_args, 1, line, column);
}

Value builtin_queue_clear_method(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 0) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "queue.clear() expects exactly 0 arguments", line, column);
        return value_create_null();
    }
    
    Value* self = interpreter_get_self_context(interpreter);
    if (!self || self->type != VALUE_OBJECT) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "queue.clear() can only be called on queue objects", line, column);
        return value_create_null();
    }
    
    Value queue_args[1] = { *self };
    return builtin_queue_clear(interpreter, queue_args, 1, line, column);
}

// Helper function to add methods to a queue object
void add_queue_methods(Value* queue) {
    value_object_set(queue, "isEmpty", value_create_builtin_function(builtin_queue_isEmpty_method));
    value_object_set(queue, "enqueue", value_create_builtin_function(builtin_queue_enqueue_method));
    value_object_set(queue, "dequeue", value_create_builtin_function(builtin_queue_dequeue_method));
    value_object_set(queue, "front", value_create_builtin_function(builtin_queue_front_method));
    value_object_set(queue, "clear", value_create_builtin_function(builtin_queue_clear_method));
}