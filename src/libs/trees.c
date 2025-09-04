#include "../../include/libs/builtin_libs.h"
#include "../../include/core/interpreter.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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
    TreeNode* node = malloc(sizeof(TreeNode));
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
    free(node);
}

// Free entire tree
void tree_free(Tree* tree) {
    if (!tree) return;
    
    // TODO: Implement tree traversal to free all nodes
    // For now, just free the root
    if (tree->root) {
        tree_node_free(tree->root);
    }
    free(tree);
}

// Create a new tree
Tree* tree_create() {
    Tree* tree = malloc(sizeof(Tree));
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
        interpreter_set_error(interpreter, "trees.create() expects no arguments", line, column);
        return value_create_null();
    }
    
    Tree* tree = tree_create();
    if (!tree) {
        interpreter_set_error(interpreter, "Failed to create tree", line, column);
        return value_create_null();
    }
    
    // Store tree pointer in a custom value type
    // For now, return a simple object representation
    Value tree_obj = value_create_object(16);
    value_object_set(&tree_obj, "type", value_create_string(strdup("Tree")));
    value_object_set(&tree_obj, "size", value_create_number(0));
    
    return tree_obj;
}

Value builtin_tree_insert(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        interpreter_set_error(interpreter, "trees.insert() expects exactly 2 arguments: tree and value", line, column);
        return value_create_null();
    }
    
    Value tree_obj = args[0];
    Value data = args[1];
    
    if (tree_obj.type != VALUE_OBJECT) {
        interpreter_set_error(interpreter, "trees.insert() first argument must be a tree object", line, column);
        return value_create_null();
    }
    
    // For now, just return success
    // TODO: Implement actual tree insertion
    return value_create_string(strdup("Value inserted into tree"));
}

Value builtin_tree_search(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        interpreter_set_error(interpreter, "trees.search() expects exactly 2 arguments: tree and value", line, column);
        return value_create_null();
    }
    
    Value tree_obj = args[0];
    Value data = args[1];
    
    if (tree_obj.type != VALUE_OBJECT) {
        interpreter_set_error(interpreter, "trees.search() first argument must be a tree object", line, column);
        return value_create_null();
    }
    
    // For now, return false
    // TODO: Implement actual tree search
    return value_create_boolean(0);
}

Value builtin_tree_size(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "trees.size() expects exactly 1 argument: tree", line, column);
        return value_create_null();
    }
    
    Value tree_obj = args[0];
    
    if (tree_obj.type != VALUE_OBJECT) {
        interpreter_set_error(interpreter, "trees.size() argument must be a tree object", line, column);
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
        interpreter_set_error(interpreter, "trees.isEmpty() expects exactly 1 argument: tree", line, column);
        return value_create_null();
    }
    
    Value tree_obj = args[0];
    
    if (tree_obj.type != VALUE_OBJECT) {
        interpreter_set_error(interpreter, "trees.isEmpty() argument must be a tree object", line, column);
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
        interpreter_set_error(interpreter, "trees.clear() expects exactly 1 argument: tree", line, column);
        return value_create_null();
    }
    
    Value* tree_obj = &args[0];
    
    if (tree_obj->type != VALUE_OBJECT) {
        interpreter_set_error(interpreter, "trees.clear() argument must be a tree object", line, column);
        return value_create_null();
    }
    
    // Clear the tree by setting size to 0
    value_object_set(tree_obj, "size", value_create_number(0));
    
    return value_create_null();
}

// Register the trees library
void trees_library_register(Interpreter* interpreter) {
    if (!interpreter) return;
    
    // Create trees module
    Value trees_module = value_create_object(16);
    
    // Tree functions
    value_object_set(&trees_module, "create", value_create_builtin_function(builtin_tree_create));
    value_object_set(&trees_module, "insert", value_create_builtin_function(builtin_tree_insert));
    value_object_set(&trees_module, "search", value_create_builtin_function(builtin_tree_search));
    value_object_set(&trees_module, "size", value_create_builtin_function(builtin_tree_size));
    value_object_set(&trees_module, "isEmpty", value_create_builtin_function(builtin_tree_is_empty));
    value_object_set(&trees_module, "clear", value_create_builtin_function(builtin_tree_clear));
    
    // Register the module
    environment_define(interpreter->global_environment, "trees", trees_module);
}
