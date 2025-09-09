#include "language_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Simple LSP server that reads from stdin and writes to stdout
int main(int argc, char* argv[]) {
    LSPLanguageServer* server = lsp_server_create();
    if (!server) {
        fprintf(stderr, "Failed to create language server\n");
        return 1;
    }
    
    // Enable debug mode if requested
    if (argc > 1 && strcmp(argv[1], "--debug") == 0) {
        server->debugMode = true;
    }
    
    if (server->debugMode) {
        fprintf(stderr, "Myco Language Server started in debug mode\n");
    }
    
    char* line = NULL;
    size_t line_size = 0;
    ssize_t line_len;
    
    // Read JSON-RPC messages from stdin
    while ((line_len = getline(&line, &line_size, stdin)) != -1) {
        if (server->debugMode) {
            fprintf(stderr, "Received: %s", line);
        }
        
        char* response = NULL;
        if (lsp_handle_message(server, line, &response)) {
            if (response) {
                printf("%s\n", response);
                fflush(stdout);
                free(response);
            }
        }
        
        // Check if server should shutdown
        if (server->shutdown) {
            break;
        }
    }
    
    free(line);
    lsp_server_free(server);
    return 0;
}
