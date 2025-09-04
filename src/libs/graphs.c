#include "../../include/libs/builtin_libs.h"
#include "../../include/core/interpreter.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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
    GraphNode* node = malloc(sizeof(GraphNode));
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
        free(node->neighbors);
    }
    free(node);
}

// Create a new graph
Graph* graph_create(int is_directed) {
    Graph* graph = malloc(sizeof(Graph));
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
        free(graph->nodes);
    }
    free(graph);
}

// Add a node to the graph
GraphNode* graph_add_node(Graph* graph, Value data) {
    if (!graph) return NULL;
    
    GraphNode* new_node = graph_node_create(data);
    if (!new_node) return NULL;
    
    // Resize nodes array if needed
    if (graph->node_count >= graph->capacity) {
        size_t new_capacity = graph->capacity == 0 ? 4 : graph->capacity * 2;
        GraphNode** new_nodes = realloc(graph->nodes, new_capacity * sizeof(GraphNode*));
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
        GraphNode** new_neighbors = realloc(from->neighbors, new_capacity * sizeof(GraphNode*));
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
            GraphNode** new_neighbors = realloc(to->neighbors, new_capacity * sizeof(GraphNode*));
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
        interpreter_set_error(interpreter, "graphs.create() expects 0 or 1 argument: is_directed (optional)", line, column);
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
        interpreter_set_error(interpreter, "Failed to create graph", line, column);
        return value_create_null();
    }
    
    // Store graph pointer in a custom value type
    // For now, return a simple object representation
    Value graph_obj = value_create_object(16);
    value_object_set(&graph_obj, "type", value_create_string(strdup("Graph")));
    value_object_set(&graph_obj, "size", value_create_number(0));
    value_object_set(&graph_obj, "isDirected", value_create_boolean(is_directed));
    
    return graph_obj;
}

Value builtin_graph_add_node(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        interpreter_set_error(interpreter, "graphs.addNode() expects exactly 2 arguments: graph and data", line, column);
        return value_create_null();
    }
    
    Value graph_obj = args[0];
    Value data = args[1];
    
    if (graph_obj.type != VALUE_OBJECT) {
        interpreter_set_error(interpreter, "graphs.addNode() first argument must be a graph object", line, column);
        return value_create_null();
    }
    
    // For now, just return success
    // TODO: Implement actual node addition
    return value_create_string(strdup("Node added to graph"));
}

Value builtin_graph_add_edge(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 3) {
        interpreter_set_error(interpreter, "graphs.addEdge() expects exactly 3 arguments: graph, from_node, to_node", line, column);
        return value_create_null();
    }
    
    Value graph_obj = args[0];
    Value from_node = args[1];
    Value to_node = args[2];
    
    if (graph_obj.type != VALUE_OBJECT) {
        interpreter_set_error(interpreter, "graphs.addEdge() first argument must be a graph object", line, column);
        return value_create_null();
    }
    
    // For now, just return success
    // TODO: Implement actual edge addition
    return value_create_string(strdup("Edge added to graph"));
}

Value builtin_graph_size(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "graphs.size() expects exactly 1 argument: graph", line, column);
        return value_create_null();
    }
    
    Value graph_obj = args[0];
    
    if (graph_obj.type != VALUE_OBJECT) {
        interpreter_set_error(interpreter, "graphs.size() argument must be a graph object", line, column);
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
        interpreter_set_error(interpreter, "graphs.isEmpty() expects exactly 1 argument: graph", line, column);
        return value_create_null();
    }
    
    Value graph_obj = args[0];
    
    if (graph_obj.type != VALUE_OBJECT) {
        interpreter_set_error(interpreter, "graphs.isEmpty() argument must be a graph object", line, column);
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
        interpreter_set_error(interpreter, "graphs.clear() expects exactly 1 argument: graph", line, column);
        return value_create_null();
    }
    
    Value* graph_obj = &args[0];
    
    if (graph_obj->type != VALUE_OBJECT) {
        interpreter_set_error(interpreter, "graphs.clear() argument must be a graph object", line, column);
        return value_create_null();
    }
    
    // Clear the graph by setting size to 0
    value_object_set(graph_obj, "size", value_create_number(0));
    
    return value_create_null();
}

// Register the graphs library
void graphs_library_register(Interpreter* interpreter) {
    if (!interpreter) return;
    
    // Create graphs module
    Value graphs_module = value_create_object(16);
    
    // Graph functions
    value_object_set(&graphs_module, "create", value_create_builtin_function(builtin_graph_create));
    value_object_set(&graphs_module, "addNode", value_create_builtin_function(builtin_graph_add_node));
    value_object_set(&graphs_module, "addEdge", value_create_builtin_function(builtin_graph_add_edge));
    value_object_set(&graphs_module, "size", value_create_builtin_function(builtin_graph_size));
    value_object_set(&graphs_module, "isEmpty", value_create_builtin_function(builtin_graph_is_empty));
    value_object_set(&graphs_module, "clear", value_create_builtin_function(builtin_graph_clear));
    
    // Register the module
    environment_define(interpreter->global_environment, "graphs", graphs_module);
}
