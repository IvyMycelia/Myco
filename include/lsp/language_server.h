#ifndef LANGUAGE_SERVER_H
#define LANGUAGE_SERVER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// LSP Protocol Version
#define LSP_VERSION "3.17.0"

// LSP Message Types
typedef enum {
    LSP_REQUEST,
    LSP_RESPONSE,
    LSP_NOTIFICATION
} LSPMessageType;

// LSP Request/Response IDs
typedef union {
    int64_t integer;
    char* string;
} LSPRequestId;

// LSP Position (0-based line and character)
typedef struct {
    uint32_t line;
    uint32_t character;
} LSPPosition;

// LSP Range
typedef struct {
    LSPPosition start;
    LSPPosition end;
} LSPRange;

// LSP Location
typedef struct {
    char* uri;
    LSPRange range;
} LSPLocation;

// LSP Diagnostic Severity
typedef enum {
    LSP_DIAGNOSTIC_ERROR = 1,
    LSP_DIAGNOSTIC_WARNING = 2,
    LSP_DIAGNOSTIC_INFORMATION = 3,
    LSP_DIAGNOSTIC_HINT = 4
} LSPDiagnosticSeverity;

// LSP Diagnostic
typedef struct {
    LSPRange range;
    LSPDiagnosticSeverity severity;
    char* code;
    char* source;
    char* message;
    char* data;
} LSPDiagnostic;

// LSP Completion Item Kind
typedef enum {
    LSP_COMPLETION_TEXT = 1,
    LSP_COMPLETION_METHOD = 2,
    LSP_COMPLETION_FUNCTION = 3,
    LSP_COMPLETION_CONSTRUCTOR = 4,
    LSP_COMPLETION_FIELD = 5,
    LSP_COMPLETION_VARIABLE = 6,
    LSP_COMPLETION_CLASS = 7,
    LSP_COMPLETION_INTERFACE = 8,
    LSP_COMPLETION_MODULE = 9,
    LSP_COMPLETION_PROPERTY = 10,
    LSP_COMPLETION_UNIT = 11,
    LSP_COMPLETION_VALUE = 12,
    LSP_COMPLETION_ENUM = 13,
    LSP_COMPLETION_KEYWORD = 14,
    LSP_COMPLETION_SNIPPET = 15,
    LSP_COMPLETION_COLOR = 16,
    LSP_COMPLETION_FILE = 17,
    LSP_COMPLETION_REFERENCE = 18
} LSPCompletionItemKind;

// LSP Completion Item
typedef struct {
    char* label;
    LSPCompletionItemKind kind;
    char* detail;
    char* documentation;
    char* insertText;
    char* filterText;
    LSPRange* textEdit;
    char** additionalTextEdits;
    size_t additionalTextEditsCount;
    char* sortText;
    char* data;
} LSPCompletionItem;

// LSP Hover
typedef struct {
    char** contents;
    size_t contentsCount;
    LSPRange range;
} LSPHover;

// LSP Parameter Information
typedef struct {
    char* label;
    LSPRange* range;
} LSPParameterInformation;

// LSP Signature Help
typedef struct {
    char* label;
    char* documentation;
    LSPParameterInformation* parameters;
    size_t parameterCount;
} LSPSignatureInformation;

typedef struct {
    LSPSignatureInformation* signatures;
    size_t signatureCount;
    uint32_t activeSignature;
    uint32_t activeParameter;
} LSPSignatureHelp;

// LSP Symbol Kind
typedef enum {
    LSP_SYMBOL_FILE = 1,
    LSP_SYMBOL_MODULE = 2,
    LSP_SYMBOL_NAMESPACE = 3,
    LSP_SYMBOL_PACKAGE = 4,
    LSP_SYMBOL_CLASS = 5,
    LSP_SYMBOL_METHOD = 6,
    LSP_SYMBOL_PROPERTY = 7,
    LSP_SYMBOL_FIELD = 8,
    LSP_SYMBOL_CONSTRUCTOR = 9,
    LSP_SYMBOL_ENUM = 10,
    LSP_SYMBOL_INTERFACE = 11,
    LSP_SYMBOL_FUNCTION = 12,
    LSP_SYMBOL_VARIABLE = 13,
    LSP_SYMBOL_CONSTANT = 14,
    LSP_SYMBOL_STRING = 15,
    LSP_SYMBOL_NUMBER = 16,
    LSP_SYMBOL_BOOLEAN = 17,
    LSP_SYMBOL_ARRAY = 18,
    LSP_SYMBOL_OBJECT = 19,
    LSP_SYMBOL_KEY = 20,
    LSP_SYMBOL_NULL = 21,
    LSP_SYMBOL_ENUM_MEMBER = 22,
    LSP_SYMBOL_STRUCT = 23,
    LP_SYMBOL_EVENT = 24,
    LSP_SYMBOL_OPERATOR = 25,
    LSP_SYMBOL_TYPE_PARAMETER = 26
} LSPSymbolKind;

// LSP Symbol Information
typedef struct {
    char* name;
    LSPSymbolKind kind;
    LSPLocation location;
    char* containerName;
    char* detail;
} LSPSymbolInformation;

// LSP Document Symbol
typedef struct {
    char* name;
    char* detail;
    LSPSymbolKind kind;
    bool deprecated;
    LSPRange range;
    LSPRange selectionRange;
    struct LSPSymbolInformation* children;
    size_t childrenCount;
} LSPDocumentSymbol;

// LSP Workspace Symbol
typedef struct {
    char* name;
    LSPSymbolKind kind;
    LSPLocation location;
    char* containerName;
} LSPWorkspaceSymbol;

// LSP Text Document Item
typedef struct {
    char* uri;
    char* languageId;
    int32_t version;
    char* text;
} LSPTextDocumentItem;

// LSP Text Document Identifier
typedef struct {
    char* uri;
} LSPTextDocumentIdentifier;

// LSP Versioned Text Document Identifier
typedef struct {
    char* uri;
    int32_t version;
} LSPVersionedTextDocumentIdentifier;

// LSP Text Document Position Parameters
typedef struct {
    LSPTextDocumentIdentifier textDocument;
    LSPPosition position;
} LSPTextDocumentPositionParams;

// LSP Language Server Capabilities
typedef struct {
    bool textDocumentSync;
    bool hoverProvider;
    bool completionProvider;
    bool signatureHelpProvider;
    bool definitionProvider;
    bool typeDefinitionProvider;
    bool implementationProvider;
    bool referencesProvider;
    bool documentHighlightProvider;
    bool documentSymbolProvider;
    bool workspaceSymbolProvider;
    bool codeActionProvider;
    bool codeLensProvider;
    bool documentFormattingProvider;
    bool documentRangeFormattingProvider;
    bool documentOnTypeFormattingProvider;
    bool renameProvider;
    bool documentLinkProvider;
    bool colorProvider;
    bool foldingRangeProvider;
    bool declarationProvider;
    bool selectionRangeProvider;
    bool callHierarchyProvider;
    bool semanticTokensProvider;
    bool monikerProvider;
    bool linkedEditingRangeProvider;
    bool typeHierarchyProvider;
    bool inlineValueProvider;
    bool inlayHintProvider;
    bool diagnosticProvider;
} LSPCapabilities;

// LSP Server Info
typedef struct {
    char* name;
    char* version;
} LSPServerInfo;

// LSP Initialize Result
typedef struct {
    LSPCapabilities capabilities;
    LSPServerInfo serverInfo;
} LSPInitializeResult;

// LSP Language Server Context
typedef struct {
    // Server state
    bool initialized;
    bool shutdown;
    LSPCapabilities capabilities;
    LSPServerInfo serverInfo;
    
    // Document management
    LSPTextDocumentItem* documents;
    size_t documentCount;
    size_t documentCapacity;
    
    // Myco-specific context
    void* myco_interpreter;
    void* myco_lexer;
    void* myco_parser;
    
    // Error handling
    char* lastError;
    int errorCount;
    
    // Configuration
    bool debugMode;
    char* rootPath;
    char** workspaceFolders;
    size_t workspaceFolderCount;
} LSPLanguageServer;

// LSP Language Server Functions
LSPLanguageServer* lsp_server_create(void);
void lsp_server_free(LSPLanguageServer* server);

// Message handling
int lsp_handle_message(LSPLanguageServer* server, const char* message, char** response);
int lsp_handle_initialize(LSPLanguageServer* server, const char* params, char** response);
int lsp_handle_initialized(LSPLanguageServer* server, const char* params);
int lsp_handle_shutdown(LSPLanguageServer* server, const char* params, char** response);
int lsp_handle_exit(LSPLanguageServer* server);

// Text document synchronization
int lsp_handle_did_open(LSPLanguageServer* server, const char* params);
int lsp_handle_did_change(LSPLanguageServer* server, const char* params);
int lsp_handle_did_close(LSPLanguageServer* server, const char* params);
int lsp_handle_did_save(LSPLanguageServer* server, const char* params);

// Language features
int lsp_handle_completion(LSPLanguageServer* server, const char* params, char** response);
int lsp_handle_hover(LSPLanguageServer* server, const char* params, char** response);
int lsp_handle_signature_help(LSPLanguageServer* server, const char* params, char** response);
int lsp_handle_definition(LSPLanguageServer* server, const char* params, char** response);
int lsp_handle_references(LSPLanguageServer* server, const char* params, char** response);
int lsp_handle_document_symbol(LSPLanguageServer* server, const char* params, char** response);
int lsp_handle_workspace_symbol(LSPLanguageServer* server, const char* params, char** response);
int lsp_handle_document_highlight(LSPLanguageServer* server, const char* params, char** response);
int lsp_handle_diagnostics(LSPLanguageServer* server, const char* params, char** response);

// Utility functions
char* lsp_uri_to_path(const char* uri);
char* lsp_path_to_uri(const char* path);
LSPPosition lsp_offset_to_position(const char* text, size_t offset);
size_t lsp_position_to_offset(const char* text, LSPPosition position);
char* lsp_create_response(const char* id, const char* result);
char* lsp_create_error_response(const char* id, int code, const char* message);
char* lsp_create_notification(const char* method, const char* params);

// JSON utilities
char* lsp_json_escape(const char* str);
char* lsp_json_serialize_position(LSPPosition pos);
char* lsp_json_serialize_range(LSPRange range);
char* lsp_json_serialize_location(LSPLocation location);
char* lsp_json_serialize_diagnostic(LSPDiagnostic diagnostic);
char* lsp_json_serialize_completion_item(LSPCompletionItem item);
char* lsp_json_serialize_hover(LSPHover hover);
char* lsp_json_serialize_signature_help(LSPSignatureHelp signatureHelp);
char* lsp_json_serialize_symbol_information(LSPSymbolInformation symbol);
char* lsp_json_serialize_document_symbol(LSPDocumentSymbol symbol);
char* lsp_json_serialize_workspace_symbol(LSPWorkspaceSymbol symbol);

#endif // LANGUAGE_SERVER_H
