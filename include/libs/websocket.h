#ifndef MYCO_WEBSOCKET_H
#define MYCO_WEBSOCKET_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <pthread.h>
#include <time.h>
#include "../core/interpreter.h"

// WebSocket frame opcodes
typedef enum {
    WS_OPCODE_CONTINUATION = 0x0,
    WS_OPCODE_TEXT = 0x1,
    WS_OPCODE_BINARY = 0x2,
    WS_OPCODE_CLOSE = 0x8,
    WS_OPCODE_PING = 0x9,
    WS_OPCODE_PONG = 0xA
} WebSocketOpcode;

// WebSocket connection state
typedef enum {
    WS_STATE_CONNECTING,
    WS_STATE_OPEN,
    WS_STATE_CLOSING,
    WS_STATE_CLOSED
} WebSocketState;

// Message queue entry
typedef struct WebSocketMessage {
    char* data;
    size_t length;
    bool is_binary;
    struct WebSocketMessage* next;
} WebSocketMessage;

// WebSocket connection structure
typedef struct WebSocketConnection {
    int socket_fd;
    SSL* ssl;  // TLS/SSL context for WSS
    WebSocketState state;
    bool is_server;
    bool is_secure;  // WSS vs WS
    char* url;  // For client connections
    int port;   // For server connections
    Value on_message_callback;
    Value on_open_callback;
    Value on_close_callback;
    Value on_error_callback;
    
    // Message queuing
    WebSocketMessage* message_queue;
    WebSocketMessage* message_queue_tail;
    size_t queue_size;
    size_t max_queue_size;
    
    // Reconnection logic
    bool auto_reconnect;
    int reconnect_attempts;
    int max_reconnect_attempts;
    time_t last_reconnect_time;
    int reconnect_delay_ms;
    
    // Ping/pong keepalive
    time_t last_ping_time;
    time_t last_pong_time;
    int ping_interval_seconds;
    bool waiting_for_pong;
    
    // Non-blocking I/O
    bool non_blocking;
    Interpreter* interpreter;  // For async event loop integration
    
    // Receive buffer for accumulating data across multiple reads
    uint8_t* receive_buffer;
    size_t receive_buffer_size;
    size_t receive_buffer_capacity;
    
    struct WebSocketConnection* next;
} WebSocketConnection;

// WebSocket server structure
typedef struct WebSocketServer {
    int listen_fd;
    int port;
    bool running;
    Value on_connection_callback;
    WebSocketConnection* connections;
    struct WebSocketServer* next;
} WebSocketServer;

// WebSocket frame structure
typedef struct {
    bool fin;
    bool rsv1;
    bool rsv2;
    bool rsv3;
    WebSocketOpcode opcode;
    bool masked;
    uint64_t payload_length;
    uint32_t masking_key;
    uint8_t* payload;
} WebSocketFrame;

// Core WebSocket functions
WebSocketConnection* websocket_connect(const char* url);
WebSocketConnection* websocket_connect_async(const char* url, Interpreter* interpreter);
WebSocketServer* websocket_create_server(int port);
void websocket_send(WebSocketConnection* conn, const char* message, bool is_binary);
void websocket_send_queued(WebSocketConnection* conn);
void websocket_close(WebSocketConnection* conn);
void websocket_server_close(WebSocketServer* server);
void websocket_free_connection(WebSocketConnection* conn);
void websocket_free_server(WebSocketServer* server);

// TLS/SSL functions
bool websocket_init_ssl(void);
void websocket_cleanup_ssl(void);
bool websocket_ssl_connect(WebSocketConnection* conn, const char* hostname);
bool websocket_ssl_accept(WebSocketConnection* conn);
SSL_CTX* websocket_create_ssl_context(bool is_server);

// Message queuing
void websocket_queue_message(WebSocketConnection* conn, const char* message, size_t length, bool is_binary);
void websocket_clear_message_queue(WebSocketConnection* conn);

// Reconnection
void websocket_set_auto_reconnect(WebSocketConnection* conn, bool enabled, int max_attempts, int delay_ms);
bool websocket_attempt_reconnect(WebSocketConnection* conn);

// Ping/pong
void websocket_send_ping(WebSocketConnection* conn);
void websocket_send_pong(WebSocketConnection* conn);
void websocket_set_ping_interval(WebSocketConnection* conn, int seconds);
void websocket_check_keepalive(WebSocketConnection* conn);

// Async event loop integration
void websocket_process_connections(Interpreter* interpreter);
void websocket_set_non_blocking(WebSocketConnection* conn, bool enabled);

// Frame encoding/decoding
int websocket_encode_frame(WebSocketFrame* frame, uint8_t* buffer, size_t buffer_size);
int websocket_decode_frame(uint8_t* data, size_t data_len, WebSocketFrame* frame);
void websocket_free_frame(WebSocketFrame* frame);

// Handshake functions
bool websocket_client_handshake(WebSocketConnection* conn, const char* url);
bool websocket_server_handshake(int client_fd, const char* request_headers);

// Utility functions
char* websocket_generate_key(void);
char* websocket_compute_accept(const char* key);
void websocket_set_on_message(WebSocketConnection* conn, Value callback);
void websocket_set_on_open(WebSocketConnection* conn, Value callback);
void websocket_set_on_close(WebSocketConnection* conn, Value callback);
void websocket_set_on_error(WebSocketConnection* conn, Value callback);
void websocket_server_set_on_connection(WebSocketServer* server, Value callback);

// Library registration
void websocket_library_register(Interpreter* interpreter);

#endif // MYCO_WEBSOCKET_H

