#include "../../include/libs/builtin_libs.h"
#include "../../include/core/interpreter.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../../include/core/standardized_errors.h"
#include "../../include/utils/shared_utilities.h"

// Graph node structure
typedef struct GraphNode {
    Value data;
    struct GraphNode** neighbors;
    size_t neighbor_count;
    size_t capacity;
} GraphNode;

// Graph structure
typedef struct Graph {
    GraphNode** nodes;
    size_t node_count;
    size_t capacity;
    int is_directed; // 1 for directed, 0 for undirected
} Graph;

// Create a new graph node
GraphNode* graph_node_create(Value data) {
    GraphNode* node = shared_malloc_safe(sizeof(GraphNode), "libs", "unknown_function", 27);
    if (!node) return NULL;
    
    node->data = value_clone(&data);
    node->neighbors = NULL;
    node->neighbor_count = 0;
    node->capacity = 0;
    
    return node;
}

// Free a graph node
void graph_node_free(GraphNode* node) {
    if (!node) return;
    
    value_free(&node->data);
    if (node->neighbors) {
        shared_free_safe(node->neighbors, "libs", "unknown_function", 44);
    }
    shared_free_safe(node, "libs", "unknown_function", 46);
}

// Create a new graph
Graph* graph_create(int is_directed) {
    Graph* graph = shared_malloc_safe(sizeof(Graph), "libs", "unknown_function", 51);
    if (!graph) return NULL;
    
    graph->nodes = NULL;
    graph->node_count = 0;
    graph->capacity = 0;
    graph->is_directed = is_directed;
    
    return graph;
}

// Free entire graph
void graph_free(Graph* graph) {
    if (!graph) return;
    
    for (size_t i = 0; i < graph->node_count; i++) {
        graph_node_free(graph->nodes[i]);
    }
    
    if (graph->nodes) {
        shared_free_safe(graph->nodes, "libs", "unknown_function", 71);
    }
    shared_free_safe(graph, "libs", "unknown_function", 73);
}

// Add a node to the graph
GraphNode* graph_add_node(Graph* graph, Value data) {
    if (!graph) return NULL;
    
    GraphNode* new_node = graph_node_create(data);
    if (!new_node) return NULL;
    
    // Resize nodes array if needed
    if (graph->node_count >= graph->capacity) {
        size_t new_capacity = graph->capacity == 0 ? 4 : graph->capacity * 2;
        GraphNode** new_nodes = shared_realloc_safe(graph->nodes, new_capacity * sizeof(GraphNode*), "libs", "unknown_function", 86);
        if (!new_nodes) {
            graph_node_free(new_node);
            return NULL;
        }
        graph->nodes = new_nodes;
        graph->capacity = new_capacity;
    }
    
    graph->nodes[graph->node_count] = new_node;
    graph->node_count++;
    
    return new_node;
}

// Add an edge between two nodes
int graph_add_edge(Graph* graph, GraphNode* from, GraphNode* to) {
    if (!graph || !from || !to) return 0;
    
    // Add 'to' to 'from's neighbors
    if (from->neighbor_count >= from->capacity) {
        size_t new_capacity = from->capacity == 0 ? 4 : from->capacity * 2;
        GraphNode** new_neighbors = shared_realloc_safe(from->neighbors, new_capacity * sizeof(GraphNode*), "libs", "unknown_function", 108);
        if (!new_neighbors) return 0;
        from->neighbors = new_neighbors;
        from->capacity = new_capacity;
    }
    
    from->neighbors[from->neighbor_count] = to;
    from->neighbor_count++;
    
    // If undirected, add 'from' to 'to's neighbors as well
    if (!graph->is_directed) {
        if (to->neighbor_count >= to->capacity) {
            size_t new_capacity = to->capacity == 0 ? 4 : to->capacity * 2;
            GraphNode** new_neighbors = shared_realloc_safe(to->neighbors, new_capacity * sizeof(GraphNode*), "libs", "unknown_function", 121);
            if (!new_neighbors) return 0;
            to->neighbors = new_neighbors;
            to->capacity = new_capacity;
        }
        
        to->neighbors[to->neighbor_count] = from;
        to->neighbor_count++;
    }
    
    return 1;
}

// Get graph size
size_t graph_size(Graph* graph) {
    return graph ? graph->node_count : 0;
}

// Check if graph is empty
int graph_is_empty(Graph* graph) {
    return !graph || graph->node_count == 0;
}

// Graph operations for Myco
Value builtin_graph_create(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count > 1) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "graphs.create() expects 0 or 1 argument: is_directed (optional)", line, column);
        return value_create_null();
    }
    
    int is_directed = 0; // Default to undirected
    if (arg_count == 1) {
        Value directed_arg = args[0];
        if (directed_arg.type == VALUE_BOOLEAN) {
            is_directed = directed_arg.data.boolean_value ? 1 : 0;
        }
    }
    
    Graph* graph = graph_create(is_directed);
    if (!graph) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "Failed to create graph", line, column);
        return value_create_null();
    }
    
    // Store graph pointer in a custom value type
    // For now, return a simple object representation
    Value graph_obj = value_create_object(16);
    value_object_set(&graph_obj, "__class_name__", value_create_string(("Graph" ? strdup("Graph") : NULL)));
    value_object_set(&graph_obj, "type", value_create_string("Graph"));
    value_object_set(&graph_obj, "size", value_create_number(0));
    value_object_set(&graph_obj, "isDirected", value_create_boolean(is_directed));
    
    return graph_obj;
}

Value builtin_graph_add_node(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "graph.addNode() expects exactly 1 argument: data", line, column);
        return value_create_null();
    }
    
    Value graph_obj = args[0];
    Value data = args[1];
    
    if (graph_obj.type != VALUE_OBJECT) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "graph.addNode() can only be called on graph objects", line, column);
        return value_create_null();
    }
    
    // For now, just return success
    // TODO: Implement actual node addition
    Value result = value_clone(&graph_obj);
    value_object_set_member(&result, "__class_name__", value_create_string(("Graph" ? strdup("Graph") : NULL)));
    return result;
}

Value builtin_graph_add_edge(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 3) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "graph.addEdge() expects exactly 2 arguments: from_node, to_node", line, column);
        return value_create_null();
    }
    
    Value graph_obj = args[0];
    Value from_node = args[1];
    Value to_node = args[2];
    
    if (graph_obj.type != VALUE_OBJECT) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "graph.addEdge() can only be called on graph objects", line, column);
        return value_create_null();
    }
    
    // For now, just return success
    // TODO: Implement actual edge addition
    Value result = value_clone(&graph_obj);
    value_object_set_member(&result, "__class_name__", value_create_string(("Graph" ? strdup("Graph") : NULL)));
    return result;
}

Value builtin_graph_size(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "graph.size() expects exactly 0 arguments", line, column);
        return value_create_null();
    }
    
    Value graph_obj = args[0];
    
    if (graph_obj.type != VALUE_OBJECT) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "graph.size() can only be called on graph objects", line, column);
        return value_create_null();
    }
    
    // Get size from graph object
    Value size_value = value_object_get(&graph_obj, "size");
    if (size_value.type == VALUE_NUMBER) {
        return value_clone(&size_value);
    }
    
    return value_create_number(0);
}

Value builtin_graph_is_empty(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "graph.isEmpty() expects exactly 0 arguments", line, column);
        return value_create_null();
    }
    
    Value graph_obj = args[0];
    
    if (graph_obj.type != VALUE_OBJECT) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "graph.isEmpty() can only be called on graph objects", line, column);
        return value_create_null();
    }
    
    // Get size from graph object
    Value size_value = value_object_get(&graph_obj, "size");
    if (size_value.type == VALUE_NUMBER && size_value.data.number_value == 0) {
        return value_create_boolean(1);
    }
    
    return value_create_boolean(0);
}

Value builtin_graph_clear(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "graph.clear() expects exactly 0 arguments", line, column);
        return value_create_null();
    }
    
    Value* graph_obj = &args[0];
    
    if (graph_obj->type != VALUE_OBJECT) {
        std_error_report(ERROR_INTERNAL_ERROR, "unknown", "unknown_function", "graph.clear() can only be called on graph objects", line, column);
        return value_create_null();
    }
    
    // Clear the graph by setting size to 0
    value_object_set(graph_obj, "size", value_create_number(0));
    
    return value_clone(graph_obj);
}

// Register the graphs library
void graphs_library_register(Interpreter* interpreter) {
    if (!interpreter || !interpreter->global_environment) return;
    
    // Create graphs object with factory functions
    Value graphs_obj = value_create_object(16);
    value_object_set(&graphs_obj, "__type__", value_create_string("Library"));
    value_object_set(&graphs_obj, "type", value_create_string("Library"));
    value_object_set(&graphs_obj, "create", value_create_builtin_function(builtin_graph_create));
    value_object_set(&graphs_obj, "isEmpty", value_create_builtin_function(builtin_graph_is_empty));
    value_object_set(&graphs_obj, "size", value_create_builtin_function(builtin_graph_size));
    value_object_set(&graphs_obj, "add_node", value_create_builtin_function(builtin_graph_add_node));
    value_object_set(&graphs_obj, "add_edge", value_create_builtin_function(builtin_graph_add_edge));
    value_object_set(&graphs_obj, "clear", value_create_builtin_function(builtin_graph_clear));
    
    // Register the graphs object
    environment_define(interpreter->global_environment, "graphs", graphs_obj);
}
