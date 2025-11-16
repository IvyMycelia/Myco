#ifndef GATEWAY_H
#define GATEWAY_H

#include "../core/interpreter/interpreter_core.h"
#include "websocket.h"
#include <stdint.h>
#include <stdbool.h>

// Gateway opcodes (generic, can be used by Discord, Slack, etc.)
typedef enum {
    GATEWAY_OPCODE_DISPATCH = 0,        // Event dispatch
    GATEWAY_OPCODE_HEARTBEAT = 1,       // Client heartbeat
    GATEWAY_OPCODE_IDENTIFY = 2,        // Client identification
    GATEWAY_OPCODE_STATUS_UPDATE = 3,  // Status update
    GATEWAY_OPCODE_VOICE_STATE_UPDATE = 4, // Voice state update
    GATEWAY_OPCODE_RESUME = 6,          // Resume connection
    GATEWAY_OPCODE_RECONNECT = 7,       // Reconnect request
    GATEWAY_OPCODE_REQUEST_GUILD_MEMBERS = 8, // Request guild members
    GATEWAY_OPCODE_INVALID_SESSION = 9, // Invalid session
    GATEWAY_OPCODE_HELLO = 10,          // Hello (server -> client)
    GATEWAY_OPCODE_HEARTBEAT_ACK = 11,  // Heartbeat ACK
    GATEWAY_OPCODE_UNKNOWN = -1         // Unknown opcode
} GatewayOpcode;

// Gateway connection states
typedef enum {
    GATEWAY_STATE_DISCONNECTED,
    GATEWAY_STATE_CONNECTING,
    GATEWAY_STATE_CONNECTED,
    GATEWAY_STATE_AUTHENTICATING,
    GATEWAY_STATE_READY,
    GATEWAY_STATE_RECONNECTING,
    GATEWAY_STATE_RESUMING,
    GATEWAY_STATE_ERROR
} GatewayState;

// Gateway configuration
typedef struct {
    const char* url;                    // Gateway URL
    const char* token;                  // Authentication token (optional)
    int heartbeat_interval_ms;          // Heartbeat interval in milliseconds
    int heartbeat_timeout_ms;           // Heartbeat timeout in milliseconds
    int reconnect_delay_ms;              // Initial reconnect delay
    int max_reconnect_attempts;          // Maximum reconnection attempts
    bool auto_reconnect;                 // Enable automatic reconnection
    bool resume_on_reconnect;           // Resume session on reconnect
    const char* session_id;              // Session ID for resuming
    uint64_t sequence_number;           // Last sequence number
    int gateway_version;                 // Gateway protocol version
    const char* encoding;                // Encoding format (json, etf)
    const char* compression;            // Compression (zlib-stream, etc.)
} GatewayConfig;

// Gateway connection
typedef struct GatewayConnection {
    WebSocketConnection* ws_conn;       // Underlying WebSocket connection
    GatewayConfig config;               // Gateway configuration
    GatewayState state;                  // Current connection state
    
    // Heartbeat management
    uint64_t last_heartbeat_time;       // Last heartbeat sent time
    uint64_t last_heartbeat_ack_time;   // Last heartbeat ACK received time
    bool waiting_for_ack;                // Waiting for heartbeat ACK
    
    // Session management
    char* session_id;                    // Current session ID
    uint64_t sequence_number;           // Current sequence number
    bool can_resume;                     // Whether session can be resumed
    
    // Event handlers
    Value on_ready_callback;             // Called when gateway is ready
    Value on_event_callback;             // Called for all events
    Value on_error_callback;            // Called on errors
    Value on_close_callback;            // Called on close
    
    // Internal
    Interpreter* interpreter;            // Interpreter reference
    struct GatewayConnection* next;      // Linked list
} GatewayConnection;

// Gateway server (for managing multiple connections)
typedef struct {
    GatewayConnection* connections;      // List of gateway connections
    size_t connection_count;             // Number of connections
} GatewayServer;

// Function prototypes
GatewayConnection* gateway_create(const char* url, GatewayConfig* config, Interpreter* interpreter);
void gateway_free(GatewayConnection* gateway);
void gateway_connect(GatewayConnection* gateway);
void gateway_disconnect(GatewayConnection* gateway);
void gateway_send_heartbeat(GatewayConnection* gateway);
void gateway_send_identify(GatewayConnection* gateway, const char* token, Value* properties);
void gateway_send_resume(GatewayConnection* gateway);
void gateway_send_opcode(GatewayConnection* gateway, GatewayOpcode opcode, Value* data);
void gateway_process_messages(GatewayConnection* gateway);
void gateway_set_on_ready(GatewayConnection* gateway, Value callback);
void gateway_set_on_event(GatewayConnection* gateway, Value callback);
void gateway_set_on_error(GatewayConnection* gateway, Value callback);
void gateway_set_on_close(GatewayConnection* gateway, Value callback);
GatewayOpcode gateway_parse_opcode(int opcode_value);
const char* gateway_opcode_to_string(GatewayOpcode opcode);
GatewayState gateway_get_state(GatewayConnection* gateway);
void gateway_set_config(GatewayConnection* gateway, GatewayConfig* config);
GatewayConfig gateway_create_default_config(void);
void gateway_handle_websocket_message(WebSocketConnection* ws_conn, const char* message);

// Builtin functions for Myco
Value builtin_gateway_create(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_gateway_connect(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_gateway_disconnect(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_gateway_send(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_gateway_on(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_gateway_set_config(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_gateway_get_state(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
void gateway_library_register(Interpreter* interpreter);
bool gateway_has_connections(void);
bool gateway_has_active_connections(void);

#endif // GATEWAY_H

