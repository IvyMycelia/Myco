#include "../../include/libs/builtin_libs.h"
#include "../../include/core/interpreter.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../../include/core/standardized_errors.h"
#include "../../include/utils/shared_utilities.h"

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

// Search for a value in the tree
TreeNode* tree_search(Tree* tree, Value data) {
    if (!tree || !tree->root) return NULL;
    
    // Simple depth-first search
    // TODO: Implement proper tree traversal
    return NULL; // Placeholder
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
    value_object_set(&tree_obj, "__class_name__", value_create_string(("Tree" ? strdup("Tree") : NULL)));
    value_object_set(&tree_obj, "size", value_create_number(0));
    
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
    
    Value* tree_obj = &args[0];
    Value data = args[1];
    
    if (tree_obj->type != VALUE_OBJECT) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "tree.insert() can only be called on a tree object", line, column);
        return value_create_null();
    }
    
    // For now, just return success
    // TODO: Implement actual tree insertion
    return value_clone(tree_obj);
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
    
    // For now, return false
    // TODO: Implement actual tree search
    return value_create_boolean(0);
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
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "tree.isEmpty() expects no arguments", line, column);
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
    
    return value_clone(tree_obj);
}

// Register the trees library
void trees_library_register(Interpreter* interpreter) {
    if (!interpreter || !interpreter->global_environment) return;
    
    // Create trees object with factory functions
    Value trees_obj = value_create_object(16);
    value_object_set(&trees_obj, "create", value_create_builtin_function(builtin_tree_create));
    
    // Register the trees object
    environment_define(interpreter->global_environment, "trees", trees_obj);
}
