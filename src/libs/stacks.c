#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../../include/core/interpreter.h"
#include "../../include/core/ast.h"

// Stack utility functions
Value builtin_stack_create(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 0) {
        interpreter_set_error(interpreter, "create() requires no arguments", line, column);
        return value_create_null();
    }
    
    // Create a new stack object
    Value stack = value_create_object(16);
    value_object_set_member(&stack, "__class_name__", value_create_string(strdup("Stack")));
    value_object_set_member(&stack, "elements", value_create_array(0));
    value_object_set_member(&stack, "size", value_create_number(0.0));
    
    return stack;
}

Value builtin_stack_push(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        interpreter_set_error(interpreter, "stack.push() expects exactly 1 argument: value", line, column);
        return value_create_null();
    }
    
    Value stack_arg = args[0];
    Value value = args[1];
    
    if (stack_arg.type != VALUE_OBJECT) {
        interpreter_set_error(interpreter, "stack.push() can only be called on stack objects", line, column);
        return value_create_null();
    }
    
    // Get stack components
    Value elements = value_object_get(&stack_arg, "elements");
    Value size = value_object_get(&stack_arg, "size");
    
    if (elements.type != VALUE_ARRAY || size.type != VALUE_NUMBER) {
        interpreter_set_error(interpreter, "Invalid stack object structure", line, column);
        return value_create_null();
    }
    
    // Create a new stack with the pushed element
    Value new_stack = value_create_object(16);
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
    
    // Add the new element to the end (LIFO - push at top)
    Value cloned_value = value_clone(&value);
    value_array_push(&new_elements, cloned_value);
    
    // Set the new stack components
    value_object_set_member(&new_stack, "__class_name__", value_create_string(strdup("Stack")));
    value_object_set_member(&new_stack, "elements", new_elements);
    value_object_set_member(&new_stack, "size", new_size);
    
    return new_stack;
}

Value builtin_stack_pop(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "stack.pop() expects exactly 0 arguments", line, column);
        return value_create_null();
    }
    
    Value stack_arg = args[0];
    
    if (stack_arg.type != VALUE_OBJECT) {
        interpreter_set_error(interpreter, "stack.pop() can only be called on stack objects", line, column);
        return value_create_null();
    }
    
    // Get stack components
    Value elements = value_object_get(&stack_arg, "elements");
    Value size = value_object_get(&stack_arg, "size");
    
    if (elements.type != VALUE_ARRAY || size.type != VALUE_NUMBER) {
        interpreter_set_error(interpreter, "Invalid stack object structure", line, column);
        return value_create_null();
    }
    
    if (elements.data.array_value.count == 0) {
        interpreter_set_error(interpreter, "Cannot pop from empty stack", line, column);
        return value_create_null();
    }
    
    // Get the top element (last element - LIFO)
    size_t top_index = elements.data.array_value.count - 1;
    Value* top_element = (Value*)elements.data.array_value.elements[top_index];
    if (!top_element) {
        interpreter_set_error(interpreter, "Invalid stack element", line, column);
        return value_create_null();
    }
    
    Value result = value_clone(top_element);
    
    // Create a new stack without the top element
    if (elements.data.array_value.count == 1) {
        // Return empty stack
        Value empty_stack = value_create_object(16);
        value_object_set_member(&empty_stack, "elements", value_create_array(0));
        value_object_set_member(&empty_stack, "size", value_create_number(0.0));
        return empty_stack;
    }
    
    // Create new stack with remaining elements (skip last element)
    Value new_stack = value_create_object(16);
    Value new_elements = value_create_array(elements.data.array_value.count - 1);
    Value new_size = value_create_number(size.data.number_value - 1.0);
    
    // Copy remaining elements (skip last element)
    for (size_t i = 0; i < elements.data.array_value.count - 1; i++) {
        Value* element = (Value*)elements.data.array_value.elements[i];
        if (element) {
            Value cloned_element = value_clone(element);
            value_array_push(&new_elements, cloned_element);
        }
    }
    
    // Set the new stack components
    value_object_set_member(&new_stack, "__class_name__", value_create_string(strdup("Stack")));
    value_object_set_member(&new_stack, "elements", new_elements);
    value_object_set_member(&new_stack, "size", new_size);
    
    return new_stack;
}

Value builtin_stack_top(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "stack.top() expects exactly 0 arguments", line, column);
        return value_create_null();
    }
    
    Value stack_arg = args[0];
    
    if (stack_arg.type != VALUE_OBJECT) {
        interpreter_set_error(interpreter, "stack.top() can only be called on stack objects", line, column);
        return value_create_null();
    }
    
    // Get stack elements
    Value elements = value_object_get(&stack_arg, "elements");
    
    if (elements.type != VALUE_ARRAY) {
        interpreter_set_error(interpreter, "Invalid stack object structure", line, column);
        return value_create_null();
    }
    
    if (elements.data.array_value.count == 0) {
        interpreter_set_error(interpreter, "Cannot get top of empty stack", line, column);
        return value_create_null();
    }
    
    // Return the top element (last element)
    size_t top_index = elements.data.array_value.count - 1;
    Value* top_element = (Value*)elements.data.array_value.elements[top_index];
    if (!top_element) {
        interpreter_set_error(interpreter, "Invalid stack element", line, column);
        return value_create_null();
    }
    
    return value_clone(top_element);
}

Value builtin_stack_size(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "stack.size() expects exactly 0 arguments", line, column);
        return value_create_null();
    }
    
    Value stack_arg = args[0];
    
    if (stack_arg.type != VALUE_OBJECT) {
        interpreter_set_error(interpreter, "stack.size() can only be called on stack objects", line, column);
        return value_create_null();
    }
    
    // Get stack size
    Value size = value_object_get(&stack_arg, "size");
    
    if (size.type != VALUE_NUMBER) {
        interpreter_set_error(interpreter, "Invalid stack object structure", line, column);
        return value_create_null();
    }
    
    return value_clone(&size);
}

Value builtin_stack_isEmpty(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "stack.isEmpty() expects exactly 0 arguments", line, column);
        return value_create_null();
    }
    
    Value stack_arg = args[0];
    
    if (stack_arg.type != VALUE_OBJECT) {
        interpreter_set_error(interpreter, "stack.isEmpty() can only be called on stack objects", line, column);
        return value_create_null();
    }
    
    // Get stack size
    Value size = value_object_get(&stack_arg, "size");
    
    if (size.type != VALUE_NUMBER) {
        interpreter_set_error(interpreter, "Invalid stack object structure", line, column);
        return value_create_null();
    }
    
    return value_create_boolean(size.data.number_value == 0.0);
}

Value builtin_stack_clear(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "stack.clear() expects exactly 0 arguments", line, column);
        return value_create_null();
    }
    
    Value stack_arg = args[0];
    
    if (stack_arg.type != VALUE_OBJECT) {
        interpreter_set_error(interpreter, "stack.clear() can only be called on stack objects", line, column);
        return value_create_null();
    }
    
    // Return empty stack
    Value empty_stack = value_create_object(16);
    value_object_set_member(&empty_stack, "__class_name__", value_create_string(strdup("Stack")));
    value_object_set_member(&empty_stack, "elements", value_create_array(0));
    value_object_set_member(&empty_stack, "size", value_create_number(0.0));
    
    return empty_stack;
}

// Register stacks library with interpreter
void stacks_library_register(Interpreter* interpreter) {
    if (!interpreter || !interpreter->global_environment) return;
    
    // Create stacks object with factory functions
    Value stacks_obj = value_create_object(16);
    value_object_set(&stacks_obj, "create", value_create_builtin_function(builtin_stack_create));
    
    // Register the stacks object
    environment_define(interpreter->global_environment, "stacks", stacks_obj);
}