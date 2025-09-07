#include "../../include/libs/builtin_libs.h"
#include "../../include/core/interpreter.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Set operations
Value builtin_set_add(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        interpreter_set_error(interpreter, "set.add() expects exactly 1 argument: element", line, column);
        return value_create_null();
    }
    
    Value* set = &args[0];
    Value element = args[1];
    
    if (set->type != VALUE_SET) {
        interpreter_set_error(interpreter, "set.add() can only be called on a set", line, column);
        return value_create_null();
    }
    
    value_set_add(set, element);
    return value_clone(set);
}

Value builtin_set_has(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        interpreter_set_error(interpreter, "set.has() expects exactly 1 argument: element", line, column);
        return value_create_null();
    }
    
    Value set = args[0];
    Value element = args[1];
    
    if (set.type != VALUE_SET) {
        interpreter_set_error(interpreter, "set.has() can only be called on a set", line, column);
        return value_create_null();
    }
    
    int has_element = value_set_has(&set, element);
    return value_create_boolean(has_element);
}

Value builtin_set_remove(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        interpreter_set_error(interpreter, "set.remove() expects exactly 1 argument: element", line, column);
        return value_create_null();
    }
    
    Value* set = &args[0];
    Value element = args[1];
    
    if (set->type != VALUE_SET) {
        interpreter_set_error(interpreter, "set.remove() can only be called on a set", line, column);
        return value_create_null();
    }
    
    value_set_remove(set, element);
    return value_clone(set);
}

Value builtin_set_size(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "set.size() expects no arguments", line, column);
        return value_create_null();
    }
    
    Value set = args[0];
    
    if (set.type != VALUE_SET) {
        interpreter_set_error(interpreter, "set.size() can only be called on a set", line, column);
        return value_create_null();
    }
    
    size_t size = value_set_size(&set);
    return value_create_number((double)size);
}

Value builtin_set_clear(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "set.clear() expects no arguments", line, column);
        return value_create_null();
    }
    
    Value* set = &args[0];
    
    if (set->type != VALUE_SET) {
        interpreter_set_error(interpreter, "set.clear() can only be called on a set", line, column);
        return value_create_null();
    }
    
    // Get all elements and remove them
    Value array = value_set_to_array(set);
    if (array.type == VALUE_ARRAY) {
        for (size_t i = 0; i < array.data.array_value.count; i++) {
            Value* element = (Value*)array.data.array_value.elements[i];
            if (element) {
                value_set_remove(set, *element);
            }
        }
        value_free(&array);
    }
    
    return value_clone(set);
}

Value builtin_set_to_array(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "set.toArray() expects no arguments", line, column);
        return value_create_null();
    }
    
    Value set = args[0];
    
    if (set.type != VALUE_SET) {
        interpreter_set_error(interpreter, "set.toArray() can only be called on a set", line, column);
        return value_create_null();
    }
    
    // Use the built-in value_set_to_array function
    return value_set_to_array(&set);
}

Value builtin_set_union(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        interpreter_set_error(interpreter, "set.union() expects exactly 1 argument: other_set", line, column);
        return value_create_null();
    }
    
    Value set1 = args[0];
    Value set2 = args[1];
    
    if (set1.type != VALUE_SET || set2.type != VALUE_SET) {
        interpreter_set_error(interpreter, "set.union() can only be called on a set with another set", line, column);
        return value_create_null();
    }
    
    // Create new set for the union
    Value result = value_create_set(16);
    
    // Add all elements from set1
    Value array1 = value_set_to_array(&set1);
    if (array1.type == VALUE_ARRAY) {
        for (size_t i = 0; i < array1.data.array_value.count; i++) {
            Value* element = (Value*)array1.data.array_value.elements[i];
            if (element) {
                value_set_add(&result, *element);
            }
        }
        value_free(&array1);
    }
    
    // Add all elements from set2
    Value array2 = value_set_to_array(&set2);
    if (array2.type == VALUE_ARRAY) {
        for (size_t i = 0; i < array2.data.array_value.count; i++) {
            Value* element = (Value*)array2.data.array_value.elements[i];
            if (element) {
                value_set_add(&result, *element);
            }
        }
        value_free(&array2);
    }
    
    return result;
}

Value builtin_set_intersection(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        interpreter_set_error(interpreter, "set.intersection() expects exactly 1 argument: other_set", line, column);
        return value_create_null();
    }
    
    Value set1 = args[0];
    Value set2 = args[1];
    
    if (set1.type != VALUE_SET || set2.type != VALUE_SET) {
        interpreter_set_error(interpreter, "set.intersection() can only be called on a set with another set", line, column);
        return value_create_null();
    }
    
    // Create new set for the intersection
    Value result = value_create_set(16);
    
    // Get elements from set1 and check if they exist in set2
    Value array1 = value_set_to_array(&set1);
    if (array1.type == VALUE_ARRAY) {
        for (size_t i = 0; i < array1.data.array_value.count; i++) {
            Value* element = (Value*)array1.data.array_value.elements[i];
            if (element && value_set_has(&set2, *element)) {
                value_set_add(&result, *element);
            }
        }
        value_free(&array1);
    }
    
    return result;
}

// Register the sets library
void sets_library_register(Interpreter* interpreter) {
    if (!interpreter) return;
    
    // Sets library is now built-in and methods are called directly on set objects
    // No global sets object needed
}
