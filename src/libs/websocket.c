#include "../../include/libs/websocket.h"
#include "../../include/libs/builtin_libs.h"
#include "../../include/core/interpreter.h"
#include "../../include/core/ast.h"
#include "../../include/core/standardized_errors.h"
#include "../../include/utils/shared_utilities.h"
#include "../../include/core/interpreter/value_operations.h"
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/select.h>

// Forward declarations
Value builtin_websocket_connect(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_websocket_send(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_websocket_connection_close(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_websocket_on(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_websocket_create_server(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_websocket_server_on(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_websocket_server_close(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_websocket_set_auto_reconnect(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_websocket_set_ping_interval(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_websocket_process_connections(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);

// Global server registry
static WebSocketServer* g_servers = NULL;
static WebSocketConnection* g_connections = NULL;

// Base64 encoding (simplified implementation)
static const char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static void base64_encode(const unsigned char* input, size_t input_len, char* output) {
    size_t i = 0, j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];
    
    while (input_len--) {
        char_array_3[i++] = *(input++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;
            
            for (i = 0; i < 4; i++) {
                output[j++] = base64_chars[char_array_4[i]];
            }
            i = 0;
        }
    }
    
    if (i) {
        for (size_t k = i; k < 3; k++) {
            char_array_3[k] = '\0';
        }
        
        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;
        
        for (size_t k = 0; k < i + 1; k++) {
            output[j++] = base64_chars[char_array_4[k]];
        }
        
        while (i++ < 3) {
            output[j++] = '=';
        }
    }
    
    output[j] = '\0';
}

// Generate WebSocket key
char* websocket_generate_key(void) {
    unsigned char random_bytes[16];
    for (int i = 0; i < 16; i++) {
        random_bytes[i] = (unsigned char)(rand() % 256);
    }
    
    char* base64_key = shared_malloc_safe(25, "websocket", "generate_key", 0);
    if (!base64_key) return NULL;
    
    base64_encode(random_bytes, 16, base64_key);
    return base64_key;
}

// Compute WebSocket accept key
char* websocket_compute_accept(const char* key) {
    if (!key) return NULL;
    
    const char* magic_string = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    size_t combined_len = strlen(key) + strlen(magic_string) + 1;
    char* combined = shared_malloc_safe(combined_len, "websocket", "compute_accept", 0);
    if (!combined) return NULL;
    
    snprintf(combined, combined_len, "%s%s", key, magic_string);
    
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1((unsigned char*)combined, strlen(combined), hash);
    
    shared_free_safe(combined, "websocket", "compute_accept", 0);
    
    char* base64_hash = shared_malloc_safe(29, "websocket", "compute_accept", 1);
    if (!base64_hash) return NULL;
    
    base64_encode(hash, SHA_DIGEST_LENGTH, base64_hash);
    return base64_hash;
}

// Parse URL
static bool parse_websocket_url(const char* url, char* host, int* port, char* path, bool* is_ssl) {
    if (!url || !host || !port || !path) return false;
    
    *is_ssl = (strncmp(url, "wss://", 6) == 0);
    if (!*is_ssl && strncmp(url, "ws://", 5) != 0) {
        return false;
    }
    
    const char* start = *is_ssl ? url + 6 : url + 5;
    const char* colon = strchr(start, ':');
    const char* slash = strchr(start, '/');
    
    if (colon && (!slash || colon < slash)) {
        // Port specified
        size_t host_len = colon - start;
        if (host_len >= 256) return false;
        strncpy(host, start, host_len);
        host[host_len] = '\0';
        
        const char* port_start = colon + 1;
        const char* port_end = slash ? slash : port_start + strlen(port_start);
        size_t port_len = port_end - port_start;
        char port_str[16];
        if (port_len >= sizeof(port_str)) return false;
        strncpy(port_str, port_start, port_len);
        port_str[port_len] = '\0';
        *port = atoi(port_str);
    } else {
        // No port specified
        const char* host_end = slash ? slash : start + strlen(start);
        size_t host_len = host_end - start;
        if (host_len >= 256) return false;
        strncpy(host, start, host_len);
        host[host_len] = '\0';
        *port = *is_ssl ? 443 : 80;
    }
    
    if (slash) {
        strncpy(path, slash, 255);
        path[255] = '\0';
    } else {
        strcpy(path, "/");
    }
    
    return true;
}

// Client handshake
bool websocket_client_handshake(WebSocketConnection* conn, const char* url) {
    if (!conn || !url) {
        fprintf(stderr, "[DEBUG WEBSOCKET] websocket_client_handshake: invalid parameters\n");
        return false;
    }
    
    char host[256];
    int port;
    char path[256];
    bool is_ssl;
    
    if (!parse_websocket_url(url, host, &port, path, &is_ssl)) {
        fprintf(stderr, "[DEBUG WEBSOCKET] websocket_client_handshake: failed to parse URL: %s\n", url);
        return false;
    }
    fprintf(stderr, "[DEBUG WEBSOCKET] Parsed URL: host=%s, port=%d, path=%s, is_ssl=%d\n", host, port, path, is_ssl);
    
    // Create socket
    struct sockaddr_in server_addr;
    struct hostent* he = gethostbyname(host);
    if (!he) {
        fprintf(stderr, "[DEBUG WEBSOCKET] websocket_client_handshake: gethostbyname failed for %s\n", host);
        return false;
    }
    
    conn->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (conn->socket_fd < 0) {
        fprintf(stderr, "[DEBUG WEBSOCKET] websocket_client_handshake: socket() failed\n");
        return false;
    }
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    memcpy(&server_addr.sin_addr, he->h_addr_list[0], he->h_length);
    
    fprintf(stderr, "[DEBUG WEBSOCKET] Attempting to connect to %s:%d\n", host, port);
    if (connect(conn->socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("[DEBUG WEBSOCKET] connect() failed");
        close(conn->socket_fd);
        conn->socket_fd = -1;
        return false;
    }
    fprintf(stderr, "[DEBUG WEBSOCKET] TCP connection established\n");
    
    // Generate handshake key
    char* key = websocket_generate_key();
    if (!key) {
        close(conn->socket_fd);
        conn->socket_fd = -1;
        return false;
    }
    
    // Build handshake request
    char request[2048];
    snprintf(request, sizeof(request),
        "GET %s HTTP/1.1\r\n"
        "Host: %s:%d\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Key: %s\r\n"
        "Sec-WebSocket-Version: 13\r\n"
        "\r\n",
        path, host, port, key);
    
    // For WSS, we need to do SSL first, then handshake over SSL
    // So we only do TCP connection here, SSL and handshake will be done separately
    if (is_ssl) {
        fprintf(stderr, "[DEBUG WEBSOCKET] WSS connection - TCP established, SSL and handshake will be done separately\n");
        shared_free_safe(key, "websocket", "client_handshake", 0);
        return true;  // Return success - TCP connection is established
    }
    
    // For non-SSL, do the full handshake now
    // Send handshake
    fprintf(stderr, "[DEBUG WEBSOCKET] Sending WebSocket handshake request\n");
    if (send(conn->socket_fd, request, strlen(request), 0) < 0) {
        perror("[DEBUG WEBSOCKET] send() failed");
        shared_free_safe(key, "websocket", "client_handshake", 0);
        close(conn->socket_fd);
        conn->socket_fd = -1;
        return false;
    }
    
    // Read response
    char response[4096];
    ssize_t received = recv(conn->socket_fd, response, sizeof(response) - 1, 0);
    if (received <= 0) {
        perror("[DEBUG WEBSOCKET] recv() failed");
        shared_free_safe(key, "websocket", "client_handshake", 0);
        close(conn->socket_fd);
        conn->socket_fd = -1;
        return false;
    }
    
    response[received] = '\0';
    fprintf(stderr, "[DEBUG WEBSOCKET] Received handshake response (first 200 chars): %.200s\n", response);
    
    // Check for 101 Switching Protocols
    if (strstr(response, "101") == NULL && strstr(response, "Switching Protocols") == NULL) {
        fprintf(stderr, "[DEBUG WEBSOCKET] Handshake failed - no 101 Switching Protocols in response\n");
        shared_free_safe(key, "websocket", "client_handshake", 0);
        close(conn->socket_fd);
        conn->socket_fd = -1;
        return false;
    }
    
    shared_free_safe(key, "websocket", "client_handshake", 0);
    conn->state = WS_STATE_OPEN;
    fprintf(stderr, "[DEBUG WEBSOCKET] WebSocket handshake completed successfully\n");
    return true;
}

// Server handshake
bool websocket_server_handshake(int client_fd, const char* request_headers) {
    if (!request_headers) return false;
    
    // Extract Sec-WebSocket-Key
    const char* key_start = strstr(request_headers, "Sec-WebSocket-Key:");
    if (!key_start) return false;
    
    key_start += strlen("Sec-WebSocket-Key:");
    while (*key_start == ' ' || *key_start == '\t') key_start++;
    
    const char* key_end = strstr(key_start, "\r\n");
    if (!key_end) return false;
    
    size_t key_len = key_end - key_start;
    char key[64];
    if (key_len >= sizeof(key)) return false;
    strncpy(key, key_start, key_len);
    key[key_len] = '\0';
    
    // Compute accept key
    char* accept_key = websocket_compute_accept(key);
    if (!accept_key) return false;
    
    // Send handshake response
    char response[512];
    snprintf(response, sizeof(response),
        "HTTP/1.1 101 Switching Protocols\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Accept: %s\r\n"
        "\r\n",
        accept_key);
    
    bool success = (send(client_fd, response, strlen(response), 0) > 0);
    
    shared_free_safe(accept_key, "websocket", "server_handshake", 0);
    return success;
}

// Encode WebSocket frame
int websocket_encode_frame(WebSocketFrame* frame, uint8_t* buffer, size_t buffer_size) {
    if (!frame || !buffer || buffer_size < 2) return -1;
    
    size_t offset = 0;
    
    // First byte: FIN, RSV, Opcode
    buffer[offset++] = (frame->fin ? 0x80 : 0x00) | (frame->opcode & 0x0F);
    
    // Second byte: MASK, Payload length
    uint8_t mask_bit = frame->masked ? 0x80 : 0x00;
    uint64_t payload_len = frame->payload_length;
    
    if (payload_len < 126) {
        buffer[offset++] = mask_bit | (uint8_t)payload_len;
    } else if (payload_len < 65536) {
        buffer[offset++] = mask_bit | 126;
        if (offset + 2 > buffer_size) return -1;
        buffer[offset++] = (payload_len >> 8) & 0xFF;
        buffer[offset++] = payload_len & 0xFF;
    } else {
        buffer[offset++] = mask_bit | 127;
        if (offset + 8 > buffer_size) return -1;
        for (int i = 7; i >= 0; i--) {
            buffer[offset++] = (payload_len >> (i * 8)) & 0xFF;
        }
    }
    
    // Masking key (if masked)
    if (frame->masked) {
        if (offset + 4 > buffer_size) return -1;
        buffer[offset++] = (frame->masking_key >> 24) & 0xFF;
        buffer[offset++] = (frame->masking_key >> 16) & 0xFF;
        buffer[offset++] = (frame->masking_key >> 8) & 0xFF;
        buffer[offset++] = frame->masking_key & 0xFF;
    }
    
    // Payload
    if (offset + payload_len > buffer_size) return -1;
    if (frame->payload && payload_len > 0) {
        memcpy(buffer + offset, frame->payload, payload_len);
        
        // Apply masking if needed
        if (frame->masked) {
            for (size_t i = 0; i < payload_len; i++) {
                buffer[offset + i] ^= ((frame->masking_key >> ((i % 4) * 8)) & 0xFF);
            }
        }
    }
    
    return offset + payload_len;
}

// Decode WebSocket frame
int websocket_decode_frame(uint8_t* data, size_t data_len, WebSocketFrame* frame) {
    if (!data || !frame || data_len < 2) return -1;
    
    size_t offset = 0;
    
    // First byte
    frame->fin = (data[offset] & 0x80) != 0;
    frame->rsv1 = (data[offset] & 0x40) != 0;
    frame->rsv2 = (data[offset] & 0x20) != 0;
    frame->rsv3 = (data[offset] & 0x10) != 0;
    frame->opcode = (WebSocketOpcode)(data[offset] & 0x0F);
    offset++;
    
    // Second byte
    frame->masked = (data[offset] & 0x80) != 0;
    uint8_t payload_len_byte = data[offset] & 0x7F;
    offset++;
    
    // Payload length
    if (payload_len_byte < 126) {
        frame->payload_length = payload_len_byte;
    } else if (payload_len_byte == 126) {
        if (offset + 2 > data_len) return -1;
        frame->payload_length = ((uint64_t)data[offset] << 8) | data[offset + 1];
        offset += 2;
    } else {
        if (offset + 8 > data_len) return -1;
        frame->payload_length = 0;
        for (int i = 0; i < 8; i++) {
            frame->payload_length = (frame->payload_length << 8) | data[offset++];
        }
    }
    
    // Masking key
    if (frame->masked) {
        if (offset + 4 > data_len) return -1;
        frame->masking_key = ((uint32_t)data[offset] << 24) |
                            ((uint32_t)data[offset + 1] << 16) |
                            ((uint32_t)data[offset + 2] << 8) |
                            data[offset + 3];
        offset += 4;
    } else {
        frame->masking_key = 0;
    }
    
    // Payload
    if (offset + frame->payload_length > data_len) return -1;
    if (frame->payload_length > 0) {
        frame->payload = shared_malloc_safe(frame->payload_length, "websocket", "decode_frame", 0);
        if (!frame->payload) return -1;
        memcpy(frame->payload, data + offset, frame->payload_length);
        
        // Unmask if needed
        if (frame->masked) {
            for (size_t i = 0; i < frame->payload_length; i++) {
                frame->payload[i] ^= ((frame->masking_key >> ((i % 4) * 8)) & 0xFF);
            }
        }
    } else {
        frame->payload = NULL;
    }
    
    return offset + frame->payload_length;
}

// Free frame
void websocket_free_frame(WebSocketFrame* frame) {
    if (!frame) return;
    if (frame->payload) {
        shared_free_safe(frame->payload, "websocket", "free_frame", 0);
        frame->payload = NULL;
    }
}

// Connect to WebSocket server
WebSocketConnection* websocket_connect(const char* url) {
    if (!url) return NULL;
    
    WebSocketConnection* conn = shared_malloc_safe(sizeof(WebSocketConnection), "websocket", "connect", 0);
    if (!conn) return NULL;
    
    memset(conn, 0, sizeof(WebSocketConnection));
    conn->socket_fd = -1;
    conn->ssl = NULL;
    conn->state = WS_STATE_CONNECTING;
    conn->is_server = false;
    conn->is_secure = (strncmp(url, "wss://", 6) == 0);
    conn->url = strdup(url);
    conn->max_queue_size = 100;
    conn->ping_interval_seconds = 30;
    conn->non_blocking = false;
    conn->auto_reconnect = false;
    conn->max_reconnect_attempts = 0;
    conn->reconnect_delay_ms = 5000;
    
    if (!websocket_client_handshake(conn, url)) {
        shared_free_safe(conn->url, "websocket", "connect", 0);
        shared_free_safe(conn, "websocket", "connect", 0);
        return NULL;
    }
    
    // Initialize SSL if WSS (must be done before WebSocket handshake)
    if (conn->is_secure) {
        char host[256];
        int port;
        char path[256];
        bool is_ssl;
        if (parse_websocket_url(url, host, &port, path, &is_ssl)) {
            fprintf(stderr, "[DEBUG WEBSOCKET] Establishing SSL connection for WSS\n");
            if (!websocket_ssl_connect(conn, host)) {
                fprintf(stderr, "[DEBUG WEBSOCKET] websocket_ssl_connect failed\n");
                shared_free_safe(conn->url, "websocket", "connect", 0);
                close(conn->socket_fd);
                shared_free_safe(conn, "websocket", "connect", 0);
                return NULL;
            }
            fprintf(stderr, "[DEBUG WEBSOCKET] SSL connection established, now doing WebSocket handshake over SSL\n");
            
            // Now do WebSocket handshake over SSL
            char* key = websocket_generate_key();
            if (!key) {
                SSL_shutdown(conn->ssl);
                SSL_free(conn->ssl);
                conn->ssl = NULL;
                close(conn->socket_fd);
                shared_free_safe(conn->url, "websocket", "connect", 0);
                shared_free_safe(conn, "websocket", "connect", 0);
                return NULL;
            }
            
            // Build handshake request
            char request[2048];
            snprintf(request, sizeof(request),
                "GET %s HTTP/1.1\r\n"
                "Host: %s:%d\r\n"
                "Upgrade: websocket\r\n"
                "Connection: Upgrade\r\n"
                "Sec-WebSocket-Key: %s\r\n"
                "Sec-WebSocket-Version: 13\r\n"
                "\r\n",
                path, host, port, key);
            
            // Send handshake over SSL
            fprintf(stderr, "[DEBUG WEBSOCKET] Sending WebSocket handshake over SSL\n");
            int sent = SSL_write(conn->ssl, request, strlen(request));
            if (sent <= 0) {
                fprintf(stderr, "[DEBUG WEBSOCKET] SSL_write() failed\n");
                shared_free_safe(key, "websocket", "connect", 0);
                SSL_shutdown(conn->ssl);
                SSL_free(conn->ssl);
                conn->ssl = NULL;
                close(conn->socket_fd);
                shared_free_safe(conn->url, "websocket", "connect", 0);
                shared_free_safe(conn, "websocket", "connect", 0);
                return NULL;
            }
            
            // Read response over SSL
            char response[4096];
            int received = SSL_read(conn->ssl, response, sizeof(response) - 1);
            if (received <= 0) {
                fprintf(stderr, "[DEBUG WEBSOCKET] SSL_read() failed\n");
                shared_free_safe(key, "websocket", "connect", 0);
                SSL_shutdown(conn->ssl);
                SSL_free(conn->ssl);
                conn->ssl = NULL;
                close(conn->socket_fd);
                shared_free_safe(conn->url, "websocket", "connect", 0);
                shared_free_safe(conn, "websocket", "connect", 0);
                return NULL;
            }
            
            response[received] = '\0';
            fprintf(stderr, "[DEBUG WEBSOCKET] Received handshake response over SSL (first 200 chars): %.200s\n", response);
            
            // Check for 101 Switching Protocols
            if (strstr(response, "101") == NULL && strstr(response, "Switching Protocols") == NULL) {
                fprintf(stderr, "[DEBUG WEBSOCKET] Handshake failed - no 101 Switching Protocols in response\n");
                shared_free_safe(key, "websocket", "connect", 0);
                SSL_shutdown(conn->ssl);
                SSL_free(conn->ssl);
                conn->ssl = NULL;
                close(conn->socket_fd);
                shared_free_safe(conn->url, "websocket", "connect", 0);
                shared_free_safe(conn, "websocket", "connect", 0);
                return NULL;
            }
            
            shared_free_safe(key, "websocket", "connect", 0);
            conn->state = WS_STATE_OPEN;
            fprintf(stderr, "[DEBUG WEBSOCKET] WebSocket handshake over SSL completed successfully\n");
        } else {
            fprintf(stderr, "[DEBUG WEBSOCKET] Failed to parse URL for SSL connection\n");
            shared_free_safe(conn->url, "websocket", "connect", 0);
            close(conn->socket_fd);
            shared_free_safe(conn, "websocket", "connect", 0);
            return NULL;
        }
    }
    
    // Add to global connections list
    conn->next = g_connections;
    g_connections = conn;
    
    return conn;
}

// Send message
void websocket_send(WebSocketConnection* conn, const char* message, bool is_binary) {
    if (!conn || !message) return;
    
    // Queue message if connection not ready
    if (conn->state != WS_STATE_OPEN) {
        websocket_queue_message(conn, message, strlen(message), is_binary);
        return;
    }
    
    size_t msg_len = strlen(message);
    if (msg_len == 0) return;
    
    WebSocketFrame frame;
    frame.fin = true;
    frame.rsv1 = false;
    frame.rsv2 = false;
    frame.rsv3 = false;
    frame.opcode = is_binary ? WS_OPCODE_BINARY : WS_OPCODE_TEXT;
    frame.masked = !conn->is_server;  // Client must mask, server must not
    frame.payload_length = msg_len;
    frame.masking_key = rand();  // Simple random key
    frame.payload = (uint8_t*)strdup(message);
    
    uint8_t buffer[4096];
    int frame_len = websocket_encode_frame(&frame, buffer, sizeof(buffer));
    
    if (frame_len > 0) {
        if (conn->ssl) {
            SSL_write(conn->ssl, buffer, frame_len);
        } else {
            send(conn->socket_fd, buffer, frame_len, 0);
        }
    }
    
    websocket_free_frame(&frame);
    shared_free_safe(frame.payload, "websocket", "send", 0);
}

// Close connection
void websocket_close(WebSocketConnection* conn) {
    if (!conn || conn->state == WS_STATE_CLOSED) return;
    
    conn->state = WS_STATE_CLOSING;
    
    // Send close frame
    WebSocketFrame frame;
    frame.fin = true;
    frame.opcode = WS_OPCODE_CLOSE;
    frame.masked = !conn->is_server;
    frame.payload_length = 0;
    frame.payload = NULL;
    
    uint8_t buffer[10];
    int frame_len = websocket_encode_frame(&frame, buffer, sizeof(buffer));
    if (frame_len > 0) {
        send(conn->socket_fd, buffer, frame_len, 0);
    }
    
    close(conn->socket_fd);
    conn->state = WS_STATE_CLOSED;
}

// Free connection
void websocket_free_connection(WebSocketConnection* conn) {
    if (!conn) return;
    
    if (conn->ssl) {
        SSL_shutdown(conn->ssl);
        SSL_free(conn->ssl);
        conn->ssl = NULL;
    }
    
    if (conn->socket_fd >= 0) {
        close(conn->socket_fd);
    }
    
    if (conn->url) {
        shared_free_safe(conn->url, "websocket", "free_connection", 0);
    }
    
    // Clear message queue
    websocket_clear_message_queue(conn);
    
    value_free(&conn->on_message_callback);
    value_free(&conn->on_open_callback);
    value_free(&conn->on_close_callback);
    value_free(&conn->on_error_callback);
    
    shared_free_safe(conn, "websocket", "free_connection", 0);
}

// Create WebSocket server
WebSocketServer* websocket_create_server(int port) {
    WebSocketServer* server = shared_malloc_safe(sizeof(WebSocketServer), "websocket", "create_server", 0);
    if (!server) return NULL;
    
    memset(server, 0, sizeof(WebSocketServer));
    server->port = port;
    server->listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (server->listen_fd < 0) {
        shared_free_safe(server, "websocket", "create_server", 0);
        return NULL;
    }
    
    int opt = 1;
    setsockopt(server->listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    
    if (bind(server->listen_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(server->listen_fd);
        shared_free_safe(server, "websocket", "create_server", 0);
        return NULL;
    }
    
    if (listen(server->listen_fd, 10) < 0) {
        close(server->listen_fd);
        shared_free_safe(server, "websocket", "create_server", 0);
        return NULL;
    }
    
    server->running = true;
    
    // Add to global servers list
    server->next = g_servers;
    g_servers = server;
    
    return server;
}

// Close server
void websocket_server_close(WebSocketServer* server) {
    if (!server) return;
    
    server->running = false;
    
    // Close all connections
    WebSocketConnection* conn = server->connections;
    while (conn) {
        WebSocketConnection* next = conn->next;
        websocket_close(conn);
        websocket_free_connection(conn);
        conn = next;
    }
    
    if (server->listen_fd >= 0) {
        close(server->listen_fd);
    }
}

// Free server
void websocket_free_server(WebSocketServer* server) {
    if (!server) return;
    
    websocket_server_close(server);
    value_free(&server->on_connection_callback);
    shared_free_safe(server, "websocket", "free_server", 0);
}

// Set callbacks
void websocket_set_on_message(WebSocketConnection* conn, Value callback) {
    if (!conn) return;
    value_free(&conn->on_message_callback);
    conn->on_message_callback = value_clone(&callback);
}

void websocket_set_on_open(WebSocketConnection* conn, Value callback) {
    if (!conn) return;
    value_free(&conn->on_open_callback);
    conn->on_open_callback = value_clone(&callback);
}

void websocket_set_on_close(WebSocketConnection* conn, Value callback) {
    if (!conn) return;
    value_free(&conn->on_close_callback);
    conn->on_close_callback = value_clone(&callback);
}

void websocket_set_on_error(WebSocketConnection* conn, Value callback) {
    if (!conn) return;
    value_free(&conn->on_error_callback);
    conn->on_error_callback = value_clone(&callback);
}

void websocket_server_set_on_connection(WebSocketServer* server, Value callback) {
    if (!server) return;
    value_free(&server->on_connection_callback);
    server->on_connection_callback = value_clone(&callback);
}

// Builtin functions for Myco
Value builtin_websocket_connect(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 1 || args[0].type != VALUE_STRING) {
        std_error_report(ERROR_TYPE_MISMATCH, "websocket", "connect", "websocket.connect() requires a URL string", line, column);
        return value_create_null();
    }
    
    // Use async version for non-blocking I/O
    WebSocketConnection* conn = websocket_connect_async(args[0].data.string_value, interpreter);
    if (!conn) {
        std_error_report(ERROR_INTERNAL_ERROR, "websocket", "connect", "Failed to connect to WebSocket server", line, column);
        return value_create_null();
    }
    
    Value conn_obj = value_create_object(12);
    value_object_set(&conn_obj, "__websocket_conn__", value_create_number((double)(intptr_t)conn));
    value_object_set(&conn_obj, "state", value_create_string("open"));
    value_object_set(&conn_obj, "send", value_create_builtin_function(builtin_websocket_send));
    value_object_set(&conn_obj, "close", value_create_builtin_function(builtin_websocket_connection_close));
    value_object_set(&conn_obj, "on", value_create_builtin_function(builtin_websocket_on));
    value_object_set(&conn_obj, "setAutoReconnect", value_create_builtin_function(builtin_websocket_set_auto_reconnect));
    value_object_set(&conn_obj, "setPingInterval", value_create_builtin_function(builtin_websocket_set_ping_interval));
    
    return conn_obj;
}

Value builtin_websocket_send(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 1) {
        std_error_report(ERROR_INTERNAL_ERROR, "websocket", "send", "websocket.send() requires a message", line, column);
        return value_create_null();
    }
    
    Value* self = interpreter_get_self_context(interpreter);
    if (!self || self->type != VALUE_OBJECT) {
        std_error_report(ERROR_INTERNAL_ERROR, "websocket", "send", "websocket.send() must be called on a connection", line, column);
        return value_create_null();
    }
    
    Value conn_val = value_object_get(self, "__websocket_conn__");
    if (conn_val.type != VALUE_NUMBER) {
        std_error_report(ERROR_INTERNAL_ERROR, "websocket", "send", "Invalid WebSocket connection", line, column);
        return value_create_null();
    }
    
    WebSocketConnection* conn = (WebSocketConnection*)(intptr_t)conn_val.data.number_value;
    if (!conn) {
        std_error_report(ERROR_INTERNAL_ERROR, "websocket", "send", "WebSocket connection not found", line, column);
        return value_create_null();
    }
    
    const char* message = NULL;
    if (args[0].type == VALUE_STRING) {
        message = args[0].data.string_value;
    } else {
        // Try to convert to string
        Value str_val = value_to_string(&args[0]);
        if (str_val.type == VALUE_STRING && str_val.data.string_value) {
            websocket_send(conn, str_val.data.string_value, false);
            value_free(&str_val);
            return value_create_null();
        }
        value_free(&str_val);
        std_error_report(ERROR_TYPE_MISMATCH, "websocket", "send", "websocket.send() message must be a string", line, column);
        return value_create_null();
    }
    
    bool is_binary = (arg_count >= 2 && args[1].type == VALUE_BOOLEAN && args[1].data.boolean_value);
    websocket_send(conn, message, is_binary);
    
    return value_create_null();
}

Value builtin_websocket_connection_close(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    Value* self = interpreter_get_self_context(interpreter);
    if (!self || self->type != VALUE_OBJECT) {
        return value_create_null();
    }
    
    Value conn_val = value_object_get(self, "__websocket_conn__");
    if (conn_val.type != VALUE_NUMBER) {
        return value_create_null();
    }
    
    WebSocketConnection* conn = (WebSocketConnection*)(intptr_t)conn_val.data.number_value;
    if (conn) {
        websocket_close(conn);
    }
    
    return value_create_null();
}

Value builtin_websocket_on(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 2 || args[0].type != VALUE_STRING || args[1].type != VALUE_FUNCTION) {
        std_error_report(ERROR_TYPE_MISMATCH, "websocket", "on", "websocket.on() requires (event: string, callback: function)", line, column);
        return value_create_null();
    }
    
    Value* self = interpreter_get_self_context(interpreter);
    if (!self || self->type != VALUE_OBJECT) {
        std_error_report(ERROR_INTERNAL_ERROR, "websocket", "on", "websocket.on() must be called on a connection", line, column);
        return value_create_null();
    }
    
    Value conn_val = value_object_get(self, "__websocket_conn__");
    if (conn_val.type != VALUE_NUMBER) {
        std_error_report(ERROR_INTERNAL_ERROR, "websocket", "on", "Invalid WebSocket connection", line, column);
        return value_create_null();
    }
    
    WebSocketConnection* conn = (WebSocketConnection*)(intptr_t)conn_val.data.number_value;
    if (!conn) {
        return value_create_null();
    }
    
    const char* event = args[0].data.string_value;
    Value callback = args[1];
    
    if (strcmp(event, "message") == 0) {
        websocket_set_on_message(conn, callback);
    } else if (strcmp(event, "open") == 0) {
        websocket_set_on_open(conn, callback);
    } else if (strcmp(event, "close") == 0) {
        websocket_set_on_close(conn, callback);
    } else if (strcmp(event, "error") == 0) {
        websocket_set_on_error(conn, callback);
    }
    
    return value_create_null();
}

Value builtin_websocket_create_server(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 1 || args[0].type != VALUE_NUMBER) {
        std_error_report(ERROR_TYPE_MISMATCH, "websocket", "createServer", "websocket.createServer() requires a port number", line, column);
        return value_create_null();
    }
    
    int port = (int)args[0].data.number_value;
    WebSocketServer* server = websocket_create_server(port);
    
    if (!server) {
        std_error_report(ERROR_INTERNAL_ERROR, "websocket", "createServer", "Failed to create WebSocket server", line, column);
        return value_create_null();
    }
    
    Value server_obj = value_create_object(6);
    value_object_set(&server_obj, "__websocket_server__", value_create_number((double)(intptr_t)server));
    value_object_set(&server_obj, "port", value_create_number(port));
    value_object_set(&server_obj, "running", value_create_boolean(true));
    value_object_set(&server_obj, "on", value_create_builtin_function(builtin_websocket_server_on));
    value_object_set(&server_obj, "close", value_create_builtin_function(builtin_websocket_server_close));
    
    return server_obj;
}

Value builtin_websocket_server_on(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 2 || args[0].type != VALUE_STRING || args[1].type != VALUE_FUNCTION) {
        std_error_report(ERROR_TYPE_MISMATCH, "websocket", "on", "websocket.on() requires (event: string, callback: function)", line, column);
        return value_create_null();
    }
    
    Value* self = interpreter_get_self_context(interpreter);
    if (!self || self->type != VALUE_OBJECT) {
        return value_create_null();
    }
    
    Value server_val = value_object_get(self, "__websocket_server__");
    if (server_val.type != VALUE_NUMBER) {
        return value_create_null();
    }
    
    WebSocketServer* server = (WebSocketServer*)(intptr_t)server_val.data.number_value;
    if (!server) {
        return value_create_null();
    }
    
    const char* event = args[0].data.string_value;
    Value callback = args[1];
    
    if (strcmp(event, "connection") == 0) {
        websocket_server_set_on_connection(server, callback);
    }
    
    return value_create_null();
}

Value builtin_websocket_server_close(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    Value* self = interpreter_get_self_context(interpreter);
    if (!self || self->type != VALUE_OBJECT) {
        return value_create_null();
    }
    
    Value server_val = value_object_get(self, "__websocket_server__");
    if (server_val.type != VALUE_NUMBER) {
        return value_create_null();
    }
    
    WebSocketServer* server = (WebSocketServer*)(intptr_t)server_val.data.number_value;
    if (server) {
        websocket_server_close(server);
    }
    
    return value_create_null();
}

// ============================================================================
// TLS/SSL SUPPORT
// ============================================================================

static SSL_CTX* g_ssl_ctx_client = NULL;
static SSL_CTX* g_ssl_ctx_server = NULL;
static bool g_ssl_initialized = false;

bool websocket_init_ssl(void) {
    if (g_ssl_initialized) return true;
    
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
    
    g_ssl_ctx_client = SSL_CTX_new(TLS_client_method());
    g_ssl_ctx_server = SSL_CTX_new(TLS_server_method());
    
    if (!g_ssl_ctx_client || !g_ssl_ctx_server) {
        websocket_cleanup_ssl();
        return false;
    }
    
    SSL_CTX_set_options(g_ssl_ctx_client, SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3);
    SSL_CTX_set_options(g_ssl_ctx_server, SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3);
    
    g_ssl_initialized = true;
    return true;
}

void websocket_cleanup_ssl(void) {
    if (g_ssl_ctx_client) {
        SSL_CTX_free(g_ssl_ctx_client);
        g_ssl_ctx_client = NULL;
    }
    if (g_ssl_ctx_server) {
        SSL_CTX_free(g_ssl_ctx_server);
        g_ssl_ctx_server = NULL;
    }
    g_ssl_initialized = false;
}

SSL_CTX* websocket_create_ssl_context(bool is_server) {
    if (!websocket_init_ssl()) return NULL;
    return is_server ? g_ssl_ctx_server : g_ssl_ctx_client;
}

bool websocket_ssl_connect(WebSocketConnection* conn, const char* hostname) {
    if (!conn || !hostname) return false;
    
    SSL_CTX* ctx = websocket_create_ssl_context(false);
    if (!ctx) return false;
    
    conn->ssl = SSL_new(ctx);
    if (!conn->ssl) return false;
    
    SSL_set_fd(conn->ssl, conn->socket_fd);
    SSL_set_tlsext_host_name(conn->ssl, hostname);
    
    int result = SSL_connect(conn->ssl);
    if (result <= 0) {
        SSL_free(conn->ssl);
        conn->ssl = NULL;
        return false;
    }
    
    return true;
}

bool websocket_ssl_accept(WebSocketConnection* conn) {
    if (!conn) return false;
    
    SSL_CTX* ctx = websocket_create_ssl_context(true);
    if (!ctx) return false;
    
    conn->ssl = SSL_new(ctx);
    if (!conn->ssl) return false;
    
    SSL_set_fd(conn->ssl, conn->socket_fd);
    
    int result = SSL_accept(conn->ssl);
    if (result <= 0) {
        SSL_free(conn->ssl);
        conn->ssl = NULL;
        return false;
    }
    
    return true;
}

// ============================================================================
// MESSAGE QUEUING
// ============================================================================

void websocket_queue_message(WebSocketConnection* conn, const char* message, size_t length, bool is_binary) {
    if (!conn || !message || length == 0) return;
    
    // Check queue size limit
    if (conn->max_queue_size > 0 && conn->queue_size >= conn->max_queue_size) {
        return;  // Queue full
    }
    
    WebSocketMessage* msg = shared_malloc_safe(sizeof(WebSocketMessage), "websocket", "queue_message", 0);
    if (!msg) return;
    
    msg->data = shared_malloc_safe(length + 1, "websocket", "queue_message", 1);
    if (!msg->data) {
        shared_free_safe(msg, "websocket", "queue_message", 0);
        return;
    }
    
    memcpy(msg->data, message, length);
    msg->data[length] = '\0';
    msg->length = length;
    msg->is_binary = is_binary;
    msg->next = NULL;
    
    if (!conn->message_queue) {
        conn->message_queue = msg;
        conn->message_queue_tail = msg;
    } else {
        conn->message_queue_tail->next = msg;
        conn->message_queue_tail = msg;
    }
    
    conn->queue_size++;
}

void websocket_clear_message_queue(WebSocketConnection* conn) {
    if (!conn) return;
    
    WebSocketMessage* msg = conn->message_queue;
    while (msg) {
        WebSocketMessage* next = msg->next;
        if (msg->data) {
            shared_free_safe(msg->data, "websocket", "clear_queue", 0);
        }
        shared_free_safe(msg, "websocket", "clear_queue", 1);
        msg = next;
    }
    
    conn->message_queue = NULL;
    conn->message_queue_tail = NULL;
    conn->queue_size = 0;
}

void websocket_send_queued(WebSocketConnection* conn) {
    if (!conn || conn->state != WS_STATE_OPEN) return;
    
    while (conn->message_queue) {
        WebSocketMessage* msg = conn->message_queue;
        
        // Try to send
        websocket_send(conn, msg->data, msg->is_binary);
        
        // Remove from queue
        conn->message_queue = msg->next;
        if (!conn->message_queue) {
            conn->message_queue_tail = NULL;
        }
        
        if (msg->data) {
            shared_free_safe(msg->data, "websocket", "send_queued", 0);
        }
        shared_free_safe(msg, "websocket", "send_queued", 1);
        conn->queue_size--;
    }
}

// ============================================================================
// RECONNECTION LOGIC
// ============================================================================

void websocket_set_auto_reconnect(WebSocketConnection* conn, bool enabled, int max_attempts, int delay_ms) {
    if (!conn) return;
    
    conn->auto_reconnect = enabled;
    conn->max_reconnect_attempts = max_attempts;
    conn->reconnect_delay_ms = delay_ms;
    conn->reconnect_attempts = 0;
}

bool websocket_attempt_reconnect(WebSocketConnection* conn) {
    if (!conn || !conn->auto_reconnect || !conn->url) return false;
    
    // Check if we've exceeded max attempts
    if (conn->max_reconnect_attempts > 0 && 
        conn->reconnect_attempts >= conn->max_reconnect_attempts) {
        return false;
    }
    
    // Check delay
    time_t now = time(NULL);
    if (conn->last_reconnect_time > 0) {
        int elapsed_ms = (int)((now - conn->last_reconnect_time) * 1000);
        if (elapsed_ms < conn->reconnect_delay_ms) {
            return false;  // Not time yet
        }
    }
    
    // Close old connection if still open
    if (conn->socket_fd >= 0) {
        if (conn->ssl) {
            SSL_shutdown(conn->ssl);
            SSL_free(conn->ssl);
            conn->ssl = NULL;
        }
        close(conn->socket_fd);
        conn->socket_fd = -1;
    }
    
    // Attempt reconnection
    conn->state = WS_STATE_CONNECTING;
    bool success = websocket_client_handshake(conn, conn->url);
    
    if (success) {
        conn->reconnect_attempts = 0;
        conn->state = WS_STATE_OPEN;
        
        // Send queued messages
        websocket_send_queued(conn);
        
        // Trigger on_open callback if set
        if (conn->on_open_callback.type == VALUE_FUNCTION && conn->interpreter) {
            Value null_val = value_create_null();
            Value result = value_function_call(&conn->on_open_callback, &null_val, 1, conn->interpreter, 0, 0);
            value_free(&result);
            value_free(&null_val);
        }
    } else {
        conn->reconnect_attempts++;
        conn->last_reconnect_time = now;
    }
    
    return success;
}

// ============================================================================
// PING/PONG KEEPALIVE
// ============================================================================

void websocket_send_ping(WebSocketConnection* conn) {
    if (!conn || conn->state != WS_STATE_OPEN) return;
    
    WebSocketFrame frame;
    frame.fin = true;
    frame.rsv1 = false;
    frame.rsv2 = false;
    frame.rsv3 = false;
    frame.opcode = WS_OPCODE_PING;
    frame.masked = !conn->is_server;
    frame.payload_length = 0;
    frame.payload = NULL;
    frame.masking_key = rand();
    
    uint8_t buffer[10];
    int frame_len = websocket_encode_frame(&frame, buffer, sizeof(buffer));
    
    if (frame_len > 0) {
        if (conn->ssl) {
            SSL_write(conn->ssl, buffer, frame_len);
        } else {
            send(conn->socket_fd, buffer, frame_len, 0);
        }
        conn->last_ping_time = time(NULL);
        conn->waiting_for_pong = true;
    }
}

void websocket_send_pong(WebSocketConnection* conn) {
    if (!conn || conn->state != WS_STATE_OPEN) return;
    
    WebSocketFrame frame;
    frame.fin = true;
    frame.opcode = WS_OPCODE_PONG;
    frame.masked = !conn->is_server;
    frame.payload_length = 0;
    frame.payload = NULL;
    frame.masking_key = rand();
    
    uint8_t buffer[10];
    int frame_len = websocket_encode_frame(&frame, buffer, sizeof(buffer));
    
    if (frame_len > 0) {
        if (conn->ssl) {
            SSL_write(conn->ssl, buffer, frame_len);
        } else {
            send(conn->socket_fd, buffer, frame_len, 0);
        }
        conn->last_pong_time = time(NULL);
    }
}

void websocket_set_ping_interval(WebSocketConnection* conn, int seconds) {
    if (!conn) return;
    conn->ping_interval_seconds = seconds;
    conn->last_ping_time = 0;
    conn->waiting_for_pong = false;
}

void websocket_check_keepalive(WebSocketConnection* conn) {
    if (!conn || conn->state != WS_STATE_OPEN || conn->ping_interval_seconds <= 0) return;
    
    time_t now = time(NULL);
    
    // Check if it's time to send ping
    if (conn->last_ping_time == 0 || 
        (now - conn->last_ping_time) >= conn->ping_interval_seconds) {
        websocket_send_ping(conn);
    }
    
    // Check if we're waiting for pong and it's been too long
    if (conn->waiting_for_pong && conn->last_pong_time > 0) {
        if ((now - conn->last_ping_time) > (conn->ping_interval_seconds * 2)) {
            // No pong received, connection might be dead
            websocket_close(conn);
            if (conn->auto_reconnect) {
                websocket_attempt_reconnect(conn);
            }
        }
    }
}

// ============================================================================
// ASYNC EVENT LOOP INTEGRATION
// ============================================================================

void websocket_set_non_blocking(WebSocketConnection* conn, bool enabled) {
    if (!conn || conn->socket_fd < 0) return;
    
    int flags = fcntl(conn->socket_fd, F_GETFL, 0);
    if (flags < 0) return;
    
    if (enabled) {
        fcntl(conn->socket_fd, F_SETFL, flags | O_NONBLOCK);
    } else {
        fcntl(conn->socket_fd, F_SETFL, flags & ~O_NONBLOCK);
    }
    
    conn->non_blocking = enabled;
}

void websocket_process_connections(Interpreter* interpreter) {
    if (!interpreter) return;
    
    WebSocketConnection* conn = g_connections;
    while (conn) {
        // Check keepalive
        websocket_check_keepalive(conn);
        
        // Process queued messages
        if (conn->state == WS_STATE_OPEN && conn->message_queue) {
            websocket_send_queued(conn);
        }
        
        // Check for reconnection
        if (conn->state == WS_STATE_CLOSED && conn->auto_reconnect) {
            websocket_attempt_reconnect(conn);
        }
        
        // Try to read messages (non-blocking)
        if (conn->state == WS_STATE_OPEN && conn->non_blocking) {
            uint8_t buffer[4096];
            ssize_t received = 0;
            
            if (conn->ssl) {
                received = SSL_read(conn->ssl, buffer, sizeof(buffer));
            } else {
                received = recv(conn->socket_fd, buffer, sizeof(buffer), MSG_DONTWAIT);
            }
            
            if (received > 0) {
                WebSocketFrame frame;
                int decoded = websocket_decode_frame(buffer, received, &frame);
                
                if (decoded > 0) {
                    if (frame.opcode == WS_OPCODE_PING) {
                        websocket_send_pong(conn);
                    } else if (frame.opcode == WS_OPCODE_PONG) {
                        conn->waiting_for_pong = false;
                        conn->last_pong_time = time(NULL);
                    } else if (frame.opcode == WS_OPCODE_TEXT || frame.opcode == WS_OPCODE_BINARY) {
                        // Check if this is a gateway connection
                        extern void gateway_handle_websocket_message(WebSocketConnection* ws_conn, const char* message);
                        if (frame.payload && frame.payload_length > 0) {
                            // Ensure null-terminated string for gateway handler
                            char* message_str = shared_malloc_safe(frame.payload_length + 1, "websocket", "process_connections", 0);
                            if (message_str) {
                                memcpy(message_str, frame.payload, frame.payload_length);
                                message_str[frame.payload_length] = '\0';
                                gateway_handle_websocket_message(conn, message_str);
                                shared_free_safe(message_str, "websocket", "process_connections", 1);
                            }
                        }
                        
                        // Trigger message callback (for non-gateway connections)
                        if (conn->on_message_callback.type == VALUE_FUNCTION && frame.payload) {
                            Value msg_val = value_create_string((char*)frame.payload);
                            Value result = value_function_call(&conn->on_message_callback, &msg_val, 1, interpreter, 0, 0);
                            value_free(&result);
                            value_free(&msg_val);
                        }
                    } else if (frame.opcode == WS_OPCODE_CLOSE) {
                        websocket_close(conn);
                    }
                    
                    websocket_free_frame(&frame);
                }
            } else if (received == 0 || (errno != EAGAIN && errno != EWOULDBLOCK)) {
                // Connection closed or error
                websocket_close(conn);
                if (conn->auto_reconnect) {
                    websocket_attempt_reconnect(conn);
                }
            }
        }
        
        conn = conn->next;
    }
}

// Update connect function to initialize new fields
WebSocketConnection* websocket_connect_async(const char* url, Interpreter* interpreter) {
    WebSocketConnection* conn = websocket_connect(url);
    if (conn) {
        conn->interpreter = interpreter;
        conn->non_blocking = true;
        websocket_set_non_blocking(conn, true);
    }
    return conn;
}

// Additional builtin functions
Value builtin_websocket_set_auto_reconnect(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 1) {
        std_error_report(ERROR_INTERNAL_ERROR, "websocket", "setAutoReconnect", "setAutoReconnect() requires at least enabled flag", line, column);
        return value_create_null();
    }
    
    Value* self = interpreter_get_self_context(interpreter);
    if (!self || self->type != VALUE_OBJECT) {
        return value_create_null();
    }
    
    Value conn_val = value_object_get(self, "__websocket_conn__");
    if (conn_val.type != VALUE_NUMBER) {
        return value_create_null();
    }
    
    WebSocketConnection* conn = (WebSocketConnection*)(intptr_t)conn_val.data.number_value;
    if (!conn) {
        return value_create_null();
    }
    
    bool enabled = (args[0].type == VALUE_BOOLEAN && args[0].data.boolean_value);
    int max_attempts = (arg_count >= 2 && args[1].type == VALUE_NUMBER) ? (int)args[1].data.number_value : 10;
    int delay_ms = (arg_count >= 3 && args[2].type == VALUE_NUMBER) ? (int)args[2].data.number_value : 5000;
    
    websocket_set_auto_reconnect(conn, enabled, max_attempts, delay_ms);
    return value_create_null();
}

Value builtin_websocket_set_ping_interval(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 1 || args[0].type != VALUE_NUMBER) {
        std_error_report(ERROR_TYPE_MISMATCH, "websocket", "setPingInterval", "setPingInterval() requires seconds (number)", line, column);
        return value_create_null();
    }
    
    Value* self = interpreter_get_self_context(interpreter);
    if (!self || self->type != VALUE_OBJECT) {
        return value_create_null();
    }
    
    Value conn_val = value_object_get(self, "__websocket_conn__");
    if (conn_val.type != VALUE_NUMBER) {
        return value_create_null();
    }
    
    WebSocketConnection* conn = (WebSocketConnection*)(intptr_t)conn_val.data.number_value;
    if (!conn) {
        return value_create_null();
    }
    
    int seconds = (int)args[0].data.number_value;
    websocket_set_ping_interval(conn, seconds);
    return value_create_null();
}

Value builtin_websocket_process_connections(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    websocket_process_connections(interpreter);
    return value_create_null();
}

// Register WebSocket library
void websocket_library_register(Interpreter* interpreter) {
    if (!interpreter || !interpreter->global_environment) return;
    
    // Initialize SSL
    websocket_init_ssl();
    
    Value ws_lib = value_create_object(10);
    value_object_set(&ws_lib, "__type__", value_create_string("Library"));
    value_object_set(&ws_lib, "type", value_create_string("Library"));
    value_object_set(&ws_lib, "__library_name__", value_create_string("websocket"));
    
    value_object_set(&ws_lib, "connect", value_create_builtin_function(builtin_websocket_connect));
    value_object_set(&ws_lib, "createServer", value_create_builtin_function(builtin_websocket_create_server));
    value_object_set(&ws_lib, "processConnections", value_create_builtin_function(builtin_websocket_process_connections));
    
    environment_define(interpreter->global_environment, "websocket", ws_lib);
}

