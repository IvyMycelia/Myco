#include "../../include/libs/builtin_libs.h"
#include "../../include/core/interpreter.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../../include/core/standardized_errors.h"
#include "../../include/utils/shared_utilities.h"

// Forward declarations
void add_tree_methods(Value* tree);

// Tree node structure
typedef struct TreeNode {
    Value data;
    struct TreeNode* left;
    struct TreeNode* right;
    struct TreeNode* parent;
} TreeNode;

// Tree structure
typedef struct Tree {
    TreeNode* root;
    size_t size;
} Tree;

// Create a new tree node
TreeNode* tree_node_create(Value data) {
    TreeNode* node = shared_malloc_safe(sizeof(TreeNode), "libs", "unknown_function", 25);
    if (!node) return NULL;
    
    node->data = value_clone(&data);
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;
    
    return node;
}

// Free a tree node
void tree_node_free(TreeNode* node) {
    if (!node) return;
    
    value_free(&node->data);
    shared_free_safe(node, "libs", "unknown_function", 41);
}

// Free entire tree
void tree_free(Tree* tree) {
    if (!tree) return;
    
    // TODO: Implement tree traversal to free all nodes
    // For now, just free the root
    if (tree->root) {
        tree_node_free(tree->root);
    }
    shared_free_safe(tree, "libs", "unknown_function", 53);
}

// Create a new tree
Tree* tree_create() {
    Tree* tree = shared_malloc_safe(sizeof(Tree), "libs", "unknown_function", 58);
    if (!tree) return NULL;
    
    tree->root = NULL;
    tree->size = 0;
    
    return tree;
}

// Insert a value into the tree (simple binary tree, not BST)
TreeNode* tree_insert(Tree* tree, Value data) {
    if (!tree) return NULL;
    
    TreeNode* new_node = tree_node_create(data);
    if (!new_node) return NULL;
    
    if (!tree->root) {
        tree->root = new_node;
        tree->size = 1;
        return new_node;
    }
    
    // Simple insertion: find first available spot
    // This is not a balanced tree, just a basic binary tree
    TreeNode* current = tree->root;
    while (current) {
        if (!current->left) {
            current->left = new_node;
            new_node->parent = current;
            break;
        } else if (!current->right) {
            current->right = new_node;
            new_node->parent = current;
            break;
        } else {
            // Move to next level (simple breadth-first approach)
            current = current->left;
        }
    }
    
    tree->size++;
    return new_node;
}

// Recursive search helper
TreeNode* tree_search_recursive(TreeNode* node, Value data) {
    if (!node) return NULL;
    
    // Check if current node matches
    if (value_equals(&node->data, &data)) {
        return node;
    }
    
    // Search left subtree
    TreeNode* left_result = tree_search_recursive(node->left, data);
    if (left_result) return left_result;
    
    // Search right subtree
    TreeNode* right_result = tree_search_recursive(node->right, data);
    if (right_result) return right_result;
    
    return NULL;
}

// Search for a value in the tree
TreeNode* tree_search(Tree* tree, Value data) {
    if (!tree || !tree->root) return NULL;
    
    // Simple depth-first search
    return tree_search_recursive(tree->root, data);
}

// Get tree size
size_t tree_size(Tree* tree) {
    return tree ? tree->size : 0;
}

// Check if tree is empty
int tree_is_empty(Tree* tree) {
    return !tree || !tree->root || tree->size == 0;
}

// Tree operations for Myco
Value builtin_tree_create(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 0) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "trees.create() expects no arguments", line, column);
        return value_create_null();
    }
    
    Tree* tree = tree_create();
    if (!tree) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "Failed to create tree", line, column);
        return value_create_null();
    }
    
    // Store tree pointer in a custom value type
    // For now, return a simple object representation
    Value tree_obj = value_create_object(16);
    value_object_set(&tree_obj, "__class_name__", value_create_string("Tree"));
    value_object_set(&tree_obj, "type", value_create_string("Tree"));
    value_object_set(&tree_obj, "size", value_create_number(0));
    
    // Add instance methods
    add_tree_methods(&tree_obj);
    
    // Set the type field for method call support
    tree_obj.type = VALUE_OBJECT;
    
    // Store the actual tree pointer for internal use
    value_object_set(&tree_obj, "__tree_ptr__", value_create_number((double)(uintptr_t)tree));
    
    return tree_obj;
}

Value builtin_tree_insert(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "tree.insert() expects exactly 1 argument: value", line, column);
        return value_create_null();
    }
    
    Value tree_obj = args[0];
    Value data = args[1];
    
    if (tree_obj.type != VALUE_OBJECT) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "tree.insert() can only be called on a tree object", line, column);
        return value_create_null();
    }
    
    // Extract tree pointer from the tree object
    Value tree_ptr_value = value_object_get(&tree_obj, "__tree_ptr__");
    if (tree_ptr_value.type != VALUE_NUMBER) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "Invalid tree object", line, column);
        return value_create_null();
    }
    
    Tree* tree = (Tree*)(uintptr_t)tree_ptr_value.data.number_value;
    if (!tree) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "Tree pointer is null", line, column);
        return value_create_null();
    }
    
    // Insert the data into the tree
    TreeNode* inserted_node = tree_insert(tree, data);
    if (!inserted_node) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "Failed to insert data into tree", line, column);
        return value_create_null();
    }
    
    // Return the updated tree object
    Value result = value_clone(&tree_obj);
    add_tree_methods(&result);
    
    // Update the size field in the result object
    Value size_value = value_create_number((double)tree->size);
    value_object_set_member(&result, "size", size_value);
    value_free(&size_value);
    
    // Ensure the tree pointer is preserved in the cloned object
    Value tree_ptr_new_value = value_create_number((double)(uintptr_t)tree);
    value_object_set(&result, "__tree_ptr__", tree_ptr_new_value);
    value_free(&tree_ptr_new_value);
    
    return result;
}

Value builtin_tree_search(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "tree.search() expects exactly 1 argument: value", line, column);
        return value_create_null();
    }
    
    Value tree_obj = args[0];
    Value data = args[1];
    
    if (tree_obj.type != VALUE_OBJECT) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "tree.search() can only be called on a tree object", line, column);
        return value_create_null();
    }
    
    // Extract tree pointer from the tree object
    Value tree_ptr_value = value_object_get(&tree_obj, "__tree_ptr__");
    if (tree_ptr_value.type != VALUE_NUMBER) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "Invalid tree object", line, column);
        return value_create_boolean(0);
    }
    
    Tree* tree = (Tree*)(uintptr_t)tree_ptr_value.data.number_value;
    if (!tree) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "Tree pointer is null", line, column);
        return value_create_boolean(0);
    }
    
    // Search for the data in the tree
    TreeNode* found_node = tree_search(tree, data);
    return value_create_boolean(found_node != NULL);
}

Value builtin_tree_size(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "tree.size() expects no arguments", line, column);
        return value_create_null();
    }
    
    Value tree_obj = args[0];
    
    if (tree_obj.type != VALUE_OBJECT) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "tree.size() can only be called on a tree object", line, column);
        return value_create_null();
    }
    
    // Get size from tree object
    Value size_value = value_object_get(&tree_obj, "size");
    if (size_value.type == VALUE_NUMBER) {
        return value_clone(&size_value);
    }
    
    return value_create_number(0);
}

Value builtin_tree_is_empty(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "tree.isEmpty() expects exactly 1 argument: self", line, column);
        return value_create_null();
    }
    
    Value tree_obj = args[0];
    
    if (tree_obj.type != VALUE_OBJECT) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "tree.isEmpty() can only be called on a tree object", line, column);
        return value_create_null();
    }
    
    // Get size from tree object
    Value size_value = value_object_get(&tree_obj, "size");
    if (size_value.type == VALUE_NUMBER && size_value.data.number_value == 0) {
        return value_create_boolean(1);
    }
    
    return value_create_boolean(0);
}

Value builtin_tree_clear(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "tree.clear() expects no arguments", line, column);
        return value_create_null();
    }
    
    Value* tree_obj = &args[0];
    
    if (tree_obj->type != VALUE_OBJECT) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "tree.clear() can only be called on a tree object", line, column);
        return value_create_null();
    }
    
    // Clear the tree by setting size to 0
    value_object_set(tree_obj, "size", value_create_number(0));
    
    Value result = value_clone(tree_obj);
    add_tree_methods(&result);
    return result;
}

void add_tree_methods(Value* tree) {
    value_object_set(tree, "isEmpty", value_create_builtin_function(builtin_tree_is_empty));
    value_object_set(tree, "insert", value_create_builtin_function(builtin_tree_insert));
    value_object_set(tree, "search", value_create_builtin_function(builtin_tree_search));
    value_object_set(tree, "clear", value_create_builtin_function(builtin_tree_clear));
}

// Register the trees library
void trees_library_register(Interpreter* interpreter) {
    if (!interpreter || !interpreter->global_environment) return;
    
    // Create trees object with factory functions
    Value trees_obj = value_create_object(16);
    value_object_set(&trees_obj, "__type__", value_create_string("Library"));
    value_object_set(&trees_obj, "type", value_create_string("Library"));
    value_object_set(&trees_obj, "create", value_create_builtin_function(builtin_tree_create));
    
    // Register the trees object
    environment_define(interpreter->global_environment, "trees", trees_obj);
}
