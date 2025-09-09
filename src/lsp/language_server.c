#include "language_server.h"
#include "../core/interpreter.h"
#include "../core/lexer.h"
#include "../core/parser.h"
#include "../core/ast.h"
#include "../core/type_checker.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

// JSON parsing and generation (simplified implementation)
static char* json_escape_string(const char* str) {
    if (!str) return NULL;
    
    size_t len = strlen(str);
    size_t escaped_len = len * 2 + 1; // Worst case: every char needs escaping
    char* result = malloc(escaped_len);
    if (!result) return NULL;
    
    size_t j = 0;
    for (size_t i = 0; i < len; i++) {
        switch (str[i]) {
            case '"':  result[j++] = '\\'; result[j++] = '"'; break;
            case '\\': result[j++] = '\\'; result[j++] = '\\'; break;
            case '\b': result[j++] = '\\'; result[j++] = 'b'; break;
            case '\f': result[j++] = '\\'; result[j++] = 'f'; break;
            case '\n': result[j++] = '\\'; result[j++] = 'n'; break;
            case '\r': result[j++] = '\\'; result[j++] = 'r'; break;
            case '\t': result[j++] = '\\'; result[j++] = 't'; break;
            default:   result[j++] = str[i]; break;
        }
    }
    result[j] = '\0';
    return result;
}

static char* json_serialize_string(const char* str) {
    char* escaped = json_escape_string(str);
    if (!escaped) return NULL;
    
    size_t len = strlen(escaped) + 3; // " + escaped + "
    char* result = malloc(len);
    if (!result) {
        free(escaped);
        return NULL;
    }
    
    snprintf(result, len, "\"%s\"", escaped);
    free(escaped);
    return result;
}

static char* json_serialize_number(double num) {
    char* result = malloc(64);
    if (!result) return NULL;
    snprintf(result, 64, "%.6g", num);
    return result;
}

static char* json_serialize_boolean(bool value) {
    char* result = malloc(6);
    if (!result) return NULL;
    strcpy(result, value ? "true" : "false");
    return result;
}

// LSP Language Server Implementation
LSPLanguageServer* lsp_server_create(void) {
    LSPLanguageServer* server = malloc(sizeof(LSPLanguageServer));
    if (!server) return NULL;
    
    memset(server, 0, sizeof(LSPLanguageServer));
    
    // Initialize server info
    server->serverInfo.name = strdup("Myco Language Server");
    server->serverInfo.version = strdup("1.0.0");
    
    // Initialize capabilities
    server->capabilities.textDocumentSync = true;
    server->capabilities.hoverProvider = true;
    server->capabilities.completionProvider = true;
    server->capabilities.signatureHelpProvider = true;
    server->capabilities.definitionProvider = true;
    server->capabilities.referencesProvider = true;
    server->capabilities.documentSymbolProvider = true;
    server->capabilities.workspaceSymbolProvider = true;
    server->capabilities.documentHighlightProvider = true;
    server->capabilities.diagnosticProvider = true;
    
    // Initialize document storage
    server->documentCapacity = 10;
    server->documents = malloc(sizeof(LSPTextDocumentItem) * server->documentCapacity);
    if (!server->documents) {
        lsp_server_free(server);
        return NULL;
    }
    
    // Initialize Myco components
    server->myco_interpreter = interpreter_create();
    if (!server->myco_interpreter) {
        lsp_server_free(server);
        return NULL;
    }
    
    return server;
}

void lsp_server_free(LSPLanguageServer* server) {
    if (!server) return;
    
    // Free server info
    free(server->serverInfo.name);
    free(server->serverInfo.version);
    
    // Free documents
    for (size_t i = 0; i < server->documentCount; i++) {
        free(server->documents[i].uri);
        free(server->documents[i].languageId);
        free(server->documents[i].text);
    }
    free(server->documents);
    
    // Free workspace folders
    for (size_t i = 0; i < server->workspaceFolderCount; i++) {
        free(server->workspaceFolders[i]);
    }
    free(server->workspaceFolders);
    
    // Free Myco components
    if (server->myco_interpreter) {
        interpreter_free((Interpreter*)server->myco_interpreter);
    }
    
    // Free error message
    free(server->lastError);
    
    // Free root path
    free(server->rootPath);
    
    free(server);
}

// Message handling
int lsp_handle_message(LSPLanguageServer* server, const char* message, char** response) {
    if (!server || !message) return 0;
    
    // TODO: Parse JSON message and route to appropriate handler
    // For now, return a simple response
    *response = strdup("{\"jsonrpc\":\"2.0\",\"result\":null,\"id\":1}");
    return 1;
}

int lsp_handle_initialize(LSPLanguageServer* server, const char* params, char** response) {
    if (!server || !params) return 0;
    
    // Create initialize result
    char* result = malloc(1024);
    if (!result) return 0;
    
    snprintf(result, 1024,
        "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{"
        "\"capabilities\":{"
        "\"textDocumentSync\":{\"openClose\":true,\"change\":1},"
        "\"hoverProvider\":true,"
        "\"completionProvider\":{\"resolveProvider\":false},"
        "\"signatureHelpProvider\":{\"triggerCharacters\":[\"(\",\",\"]},"
        "\"definitionProvider\":true,"
        "\"referencesProvider\":true,"
        "\"documentSymbolProvider\":true,"
        "\"workspaceSymbolProvider\":true,"
        "\"documentHighlightProvider\":true,"
        "\"diagnosticProvider\":{\"identifier\":\"myco\"}"
        "},"
        "\"serverInfo\":{\"name\":\"Myco Language Server\",\"version\":\"1.0.0\"}"
        "}}");
    
    *response = result;
    return 1;
}

int lsp_handle_initialized(LSPLanguageServer* server, const char* params) {
    if (!server) return 0;
    
    server->initialized = true;
    return 1;
}

int lsp_handle_shutdown(LSPLanguageServer* server, const char* params, char** response) {
    if (!server) return 0;
    
    server->shutdown = true;
    
    char* result = malloc(64);
    if (!result) return 0;
    
    strcpy(result, "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":null}");
    *response = result;
    return 1;
}

int lsp_handle_exit(LSPLanguageServer* server) {
    if (!server) return 0;
    
    // Cleanup and exit
    return 1;
}

// Text document synchronization
int lsp_handle_did_open(LSPLanguageServer* server, const char* params) {
    if (!server || !params) return 0;
    
    // TODO: Parse document URI and content from params
    // For now, just return success
    return 1;
}

int lsp_handle_did_change(LSPLanguageServer* server, const char* params) {
    if (!server || !params) return 0;
    
    // TODO: Parse document changes and update internal representation
    // For now, just return success
    return 1;
}

int lsp_handle_did_close(LSPLanguageServer* server, const char* params) {
    if (!server || !params) return 0;
    
    // TODO: Remove document from internal storage
    // For now, just return success
    return 1;
}

int lsp_handle_did_save(LSPLanguageServer* server, const char* params) {
    if (!server || !params) return 0;
    
    // TODO: Process saved document and send diagnostics
    // For now, just return success
    return 1;
}

// Language features
int lsp_handle_completion(LSPLanguageServer* server, const char* params, char** response) {
    if (!server || !params) return 0;
    
    // TODO: Parse position and provide completions
    char* result = malloc(256);
    if (!result) return 0;
    
    strcpy(result, "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"items\":[]}}");
    *response = result;
    return 1;
}

int lsp_handle_hover(LSPLanguageServer* server, const char* params, char** response) {
    if (!server || !params) return 0;
    
    // TODO: Parse position and provide hover information
    char* result = malloc(256);
    if (!result) return 0;
    
    strcpy(result, "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":null}");
    *response = result;
    return 1;
}

int lsp_handle_signature_help(LSPLanguageServer* server, const char* params, char** response) {
    if (!server || !params) return 0;
    
    // TODO: Parse position and provide signature help
    char* result = malloc(256);
    if (!result) return 0;
    
    strcpy(result, "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"signatures\":[]}}");
    *response = result;
    return 1;
}

int lsp_handle_definition(LSPLanguageServer* server, const char* params, char** response) {
    if (!server || !params) return 0;
    
    // TODO: Parse position and find definition
    char* result = malloc(256);
    if (!result) return 0;
    
    strcpy(result, "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":[]}");
    *response = result;
    return 1;
}

int lsp_handle_references(LSPLanguageServer* server, const char* params, char** response) {
    if (!server || !params) return 0;
    
    // TODO: Parse position and find references
    char* result = malloc(256);
    if (!result) return 0;
    
    strcpy(result, "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":[]}");
    *response = result;
    return 1;
}

int lsp_handle_document_symbol(LSPLanguageServer* server, const char* params, char** response) {
    if (!server || !params) return 0;
    
    // TODO: Parse document and return symbols
    char* result = malloc(256);
    if (!result) return 0;
    
    strcpy(result, "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":[]}");
    *response = result;
    return 1;
}

int lsp_handle_workspace_symbol(LSPLanguageServer* server, const char* params, char** response) {
    if (!server || !params) return 0;
    
    // TODO: Search workspace for symbols
    char* result = malloc(256);
    if (!result) return 0;
    
    strcpy(result, "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":[]}");
    *response = result;
    return 1;
}

int lsp_handle_document_highlight(LSPLanguageServer* server, const char* params, char** response) {
    if (!server || !params) return 0;
    
    // TODO: Parse position and highlight related symbols
    char* result = malloc(256);
    if (!result) return 0;
    
    strcpy(result, "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":[]}");
    *response = result;
    return 1;
}

int lsp_handle_diagnostics(LSPLanguageServer* server, const char* params, char** response) {
    if (!server || !params) return 0;
    
    // TODO: Parse document and return diagnostics
    char* result = malloc(256);
    if (!result) return 0;
    
    strcpy(result, "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":[]}");
    *response = result;
    return 1;
}

// Utility functions
char* lsp_uri_to_path(const char* uri) {
    if (!uri) return NULL;
    
    // Simple implementation - remove file:// prefix
    if (strncmp(uri, "file://", 7) == 0) {
        return strdup(uri + 7);
    }
    return strdup(uri);
}

char* lsp_path_to_uri(const char* path) {
    if (!path) return NULL;
    
    size_t len = strlen(path) + 8; // file:// + path + \0
    char* result = malloc(len);
    if (!result) return NULL;
    
    snprintf(result, len, "file://%s", path);
    return result;
}

LSPPosition lsp_offset_to_position(const char* text, size_t offset) {
    LSPPosition pos = {0, 0};
    if (!text) return pos;
    
    size_t len = strlen(text);
    if (offset > len) offset = len;
    
    for (size_t i = 0; i < offset; i++) {
        if (text[i] == '\n') {
            pos.line++;
            pos.character = 0;
        } else {
            pos.character++;
        }
    }
    
    return pos;
}

size_t lsp_position_to_offset(const char* text, LSPPosition position) {
    if (!text) return 0;
    
    size_t offset = 0;
    size_t current_line = 0;
    size_t current_char = 0;
    
    while (text[offset] && current_line < position.line) {
        if (text[offset] == '\n') {
            current_line++;
            current_char = 0;
        } else {
            current_char++;
        }
        offset++;
    }
    
    // Add character offset on the target line
    while (text[offset] && text[offset] != '\n' && current_char < position.character) {
        offset++;
        current_char++;
    }
    
    return offset;
}

// JSON serialization functions
char* lsp_json_serialize_position(LSPPosition pos) {
    char* result = malloc(64);
    if (!result) return NULL;
    snprintf(result, 64, "{\"line\":%u,\"character\":%u}", pos.line, pos.character);
    return result;
}

char* lsp_json_serialize_range(LSPRange range) {
    char* start = lsp_json_serialize_position(range.start);
    char* end = lsp_json_serialize_position(range.end);
    if (!start || !end) {
        free(start);
        free(end);
        return NULL;
    }
    
    char* result = malloc(128);
    if (!result) {
        free(start);
        free(end);
        return NULL;
    }
    
    snprintf(result, 128, "{\"start\":%s,\"end\":%s}", start, end);
    free(start);
    free(end);
    return result;
}

char* lsp_json_serialize_location(LSPLocation location) {
    char* uri = json_serialize_string(location.uri);
    char* range = lsp_json_serialize_range(location.range);
    if (!uri || !range) {
        free(uri);
        free(range);
        return NULL;
    }
    
    char* result = malloc(256);
    if (!result) {
        free(uri);
        free(range);
        return NULL;
    }
    
    snprintf(result, 256, "{\"uri\":%s,\"range\":%s}", uri, range);
    free(uri);
    free(range);
    return result;
}

char* lsp_json_serialize_diagnostic(LSPDiagnostic diagnostic) {
    char* range = lsp_json_serialize_range(diagnostic.range);
    char* message = json_serialize_string(diagnostic.message);
    char* source = json_serialize_string(diagnostic.source);
    char* code = json_serialize_string(diagnostic.code);
    
    if (!range || !message || !source || !code) {
        free(range);
        free(message);
        free(source);
        free(code);
        return NULL;
    }
    
    char* result = malloc(512);
    if (!result) {
        free(range);
        free(message);
        free(source);
        free(code);
        return NULL;
    }
    
    snprintf(result, 512,
        "{\"range\":%s,\"severity\":%d,\"source\":%s,\"code\":%s,\"message\":%s}",
        range, diagnostic.severity, source, code, message);
    
    free(range);
    free(message);
    free(source);
    free(code);
    return result;
}

char* lsp_json_serialize_completion_item(LSPCompletionItem item) {
    char* label = json_serialize_string(item.label);
    char* detail = json_serialize_string(item.detail);
    char* documentation = json_serialize_string(item.documentation);
    char* insertText = json_serialize_string(item.insertText);
    
    if (!label || !detail || !documentation || !insertText) {
        free(label);
        free(detail);
        free(documentation);
        free(insertText);
        return NULL;
    }
    
    char* result = malloc(512);
    if (!result) {
        free(label);
        free(detail);
        free(documentation);
        free(insertText);
        return NULL;
    }
    
    snprintf(result, 512,
        "{\"label\":%s,\"kind\":%d,\"detail\":%s,\"documentation\":%s,\"insertText\":%s}",
        label, item.kind, detail, documentation, insertText);
    
    free(label);
    free(detail);
    free(documentation);
    free(insertText);
    return result;
}

char* lsp_json_serialize_hover(LSPHover hover) {
    // TODO: Implement hover serialization
    char* result = malloc(64);
    if (!result) return NULL;
    strcpy(result, "{\"contents\":[]}");
    return result;
}

char* lsp_json_serialize_signature_help(LSPSignatureHelp signatureHelp) {
    // TODO: Implement signature help serialization
    char* result = malloc(64);
    if (!result) return NULL;
    strcpy(result, "{\"signatures\":[]}");
    return result;
}

char* lsp_json_serialize_symbol_information(LSPSymbolInformation symbol) {
    char* name = json_serialize_string(symbol.name);
    char* containerName = json_serialize_string(symbol.containerName);
    char* detail = json_serialize_string(symbol.detail);
    char* location = lsp_json_serialize_location(symbol.location);
    
    if (!name || !containerName || !detail || !location) {
        free(name);
        free(containerName);
        free(detail);
        free(location);
        return NULL;
    }
    
    char* result = malloc(512);
    if (!result) {
        free(name);
        free(containerName);
        free(detail);
        free(location);
        return NULL;
    }
    
    snprintf(result, 512,
        "{\"name\":%s,\"kind\":%d,\"location\":%s,\"containerName\":%s,\"detail\":%s}",
        name, symbol.kind, location, containerName, detail);
    
    free(name);
    free(containerName);
    free(detail);
    free(location);
    return result;
}

char* lsp_json_serialize_document_symbol(LSPDocumentSymbol symbol) {
    // TODO: Implement document symbol serialization
    char* result = malloc(64);
    if (!result) return NULL;
    strcpy(result, "{\"name\":\"\",\"kind\":1}");
    return result;
}

char* lsp_json_serialize_workspace_symbol(LSPWorkspaceSymbol symbol) {
    // TODO: Implement workspace symbol serialization
    char* result = malloc(64);
    if (!result) return NULL;
    strcpy(result, "{\"name\":\"\",\"kind\":1}");
    return result;
}

// Response creation helpers
char* lsp_create_response(const char* id, const char* result) {
    if (!id || !result) return NULL;
    
    char* id_str = json_serialize_string(id);
    if (!id_str) return NULL;
    
    char* response = malloc(256);
    if (!response) {
        free(id_str);
        return NULL;
    }
    
    snprintf(response, 256, "{\"jsonrpc\":\"2.0\",\"id\":%s,\"result\":%s}", id_str, result);
    free(id_str);
    return response;
}

char* lsp_create_error_response(const char* id, int code, const char* message) {
    if (!id || !message) return NULL;
    
    char* id_str = json_serialize_string(id);
    char* msg_str = json_serialize_string(message);
    if (!id_str || !msg_str) {
        free(id_str);
        free(msg_str);
        return NULL;
    }
    
    char* response = malloc(256);
    if (!response) {
        free(id_str);
        free(msg_str);
        return NULL;
    }
    
    snprintf(response, 256,
        "{\"jsonrpc\":\"2.0\",\"id\":%s,\"error\":{\"code\":%d,\"message\":%s}}",
        id_str, code, msg_str);
    
    free(id_str);
    free(msg_str);
    return response;
}

char* lsp_create_notification(const char* method, const char* params) {
    if (!method) return NULL;
    
    char* method_str = json_serialize_string(method);
    if (!method_str) return NULL;
    
    char* notification = malloc(256);
    if (!notification) {
        free(method_str);
        return NULL;
    }
    
    if (params) {
        snprintf(notification, 256, "{\"jsonrpc\":\"2.0\",\"method\":%s,\"params\":%s}", method_str, params);
    } else {
        snprintf(notification, 256, "{\"jsonrpc\":\"2.0\",\"method\":%s}", method_str);
    }
    
    free(method_str);
    return notification;
}
