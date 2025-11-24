#include "../../include/libs/gateway.h"
#include "../../include/core/interpreter/value_operations.h"
#include "../../include/core/interpreter/interpreter_core.h"
#include "../../include/libs/json.h"
#include "../../include/core/standardized_errors.h"
#include "../../include/utils/shared_utilities.h"
#include "../../include/core/interpreter/method_handlers.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>

// Forward declarations for JSON functions
extern Value builtin_json_parse(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
extern Value builtin_json_stringify(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);

// Forward declarations
Value builtin_gateway_create(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_gateway_connect(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_gateway_disconnect(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_gateway_send(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_gateway_on(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_gateway_set_config(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_gateway_get_state(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);

// Global gateway registry
static GatewayConnection* g_gateway_connections = NULL;

// Check if any gateway connections exist (not just active ones)
bool gateway_has_connections(void) {
    bool has_conns = g_gateway_connections != NULL;
    if (has_conns) {
    }
    return has_conns;
}

// Check if any gateway connections are active
bool gateway_has_active_connections(void) {
    GatewayConnection* gateway = g_gateway_connections;
    while (gateway) {
        if (gateway->state == GATEWAY_STATE_CONNECTING || 
            gateway->state == GATEWAY_STATE_CONNECTED || 
            gateway->state == GATEWAY_STATE_READY ||
            gateway->state == GATEWAY_STATE_RECONNECTING ||
            gateway->state == GATEWAY_STATE_RESUMING) {
            return true;
        }
        gateway = gateway->next;
    }
    return false;
}

// Get current time in milliseconds
static uint64_t get_time_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000 + (uint64_t)ts.tv_nsec / 1000000;
}

// Create default gateway configuration
GatewayConfig gateway_create_default_config(void) {
    GatewayConfig config = {0};
    config.heartbeat_interval_ms = 41250;  // Default ~41 seconds
    config.heartbeat_timeout_ms = 60000;   // 60 second timeout
    config.reconnect_delay_ms = 1000;      // 1 second initial delay
    config.max_reconnect_attempts = 10;
    config.auto_reconnect = true;
    config.resume_on_reconnect = true;
    config.gateway_version = 10;           // Default version
    config.encoding = "json";
    config.compression = NULL;
    config.sequence_number = 0;
    return config;
}

// Parse opcode from integer
GatewayOpcode gateway_parse_opcode(int opcode_value) {
    switch (opcode_value) {
        case 0: return GATEWAY_OPCODE_DISPATCH;
        case 1: return GATEWAY_OPCODE_HEARTBEAT;
        case 2: return GATEWAY_OPCODE_IDENTIFY;
        case 3: return GATEWAY_OPCODE_STATUS_UPDATE;
        case 4: return GATEWAY_OPCODE_VOICE_STATE_UPDATE;
        case 6: return GATEWAY_OPCODE_RESUME;
        case 7: return GATEWAY_OPCODE_RECONNECT;
        case 8: return GATEWAY_OPCODE_REQUEST_GUILD_MEMBERS;
        case 9: return GATEWAY_OPCODE_INVALID_SESSION;
        case 10: return GATEWAY_OPCODE_HELLO;
        case 11: return GATEWAY_OPCODE_HEARTBEAT_ACK;
        default: return GATEWAY_OPCODE_UNKNOWN;
    }
}

// Convert opcode to string
const char* gateway_opcode_to_string(GatewayOpcode opcode) {
    switch (opcode) {
        case GATEWAY_OPCODE_DISPATCH: return "DISPATCH";
        case GATEWAY_OPCODE_HEARTBEAT: return "HEARTBEAT";
        case GATEWAY_OPCODE_IDENTIFY: return "IDENTIFY";
        case GATEWAY_OPCODE_STATUS_UPDATE: return "STATUS_UPDATE";
        case GATEWAY_OPCODE_VOICE_STATE_UPDATE: return "VOICE_STATE_UPDATE";
        case GATEWAY_OPCODE_RESUME: return "RESUME";
        case GATEWAY_OPCODE_RECONNECT: return "RECONNECT";
        case GATEWAY_OPCODE_REQUEST_GUILD_MEMBERS: return "REQUEST_GUILD_MEMBERS";
        case GATEWAY_OPCODE_INVALID_SESSION: return "INVALID_SESSION";
        case GATEWAY_OPCODE_HELLO: return "HELLO";
        case GATEWAY_OPCODE_HEARTBEAT_ACK: return "HEARTBEAT_ACK";
        default: return "UNKNOWN";
    }
}

// Create a new gateway connection
GatewayConnection* gateway_create(const char* url, GatewayConfig* config, Interpreter* interpreter) {
    if (!url) return NULL;
    
    GatewayConnection* gateway = (GatewayConnection*)shared_malloc_safe(
        sizeof(GatewayConnection), "gateway", "gateway_create", 0);
    if (!gateway) return NULL;
    
    // Initialize configuration
    if (config) {
        gateway->config = *config;
        // Note: config->token is a pointer that may not persist, so we'll copy it later
    } else {
        gateway->config = gateway_create_default_config();
    }
    
    // Set URL if not already set
    if (!gateway->config.url) {
        gateway->config.url = url;
    }
    
    // Create WebSocket connection
    gateway->ws_conn = websocket_connect_async(url, interpreter);
    if (!gateway->ws_conn) {
        shared_free_safe(gateway, "gateway", "gateway_create", 0);
        return NULL;
    }
    
    // Set WebSocket to non-blocking mode for message processing
    websocket_set_non_blocking(gateway->ws_conn, true);
    
    // Initialize gateway state
    gateway->state = GATEWAY_STATE_DISCONNECTED;
    gateway->interpreter = interpreter;
    gateway->session_id = NULL;
    gateway->token_copy = NULL;
    gateway->sequence_number = 0;
    gateway->can_resume = false;
    gateway->waiting_for_ack = false;
    gateway->last_heartbeat_time = 0;
    gateway->last_heartbeat_ack_time = 0;
    
    // Initialize callbacks
    gateway->on_ready_callback = value_create_null();
    gateway->on_event_callback = value_create_null();
    gateway->on_error_callback = value_create_null();
    gateway->on_close_callback = value_create_null();
    
    // Initialize intents
    gateway->intents = value_create_null();
    
    // Add to global registry
    gateway->next = g_gateway_connections;
    g_gateway_connections = gateway;
    
    
    return gateway;
}

// Free gateway connection
void gateway_free(GatewayConnection* gateway) {
    if (!gateway) return;
    
    // Disconnect WebSocket
    if (gateway->ws_conn) {
        websocket_close(gateway->ws_conn);
        websocket_free_connection(gateway->ws_conn);
    }
    
    // Free session ID
    if (gateway->session_id) {
        shared_free_safe(gateway->session_id, "gateway", "gateway_free", 0);
    }
    
    // Free token copy
    if (gateway->token_copy) {
        shared_free_safe(gateway->token_copy, "gateway", "gateway_free", 0);
    }
    
    // Free callbacks
    value_free(&gateway->on_ready_callback);
    value_free(&gateway->on_event_callback);
    value_free(&gateway->on_error_callback);
    value_free(&gateway->on_close_callback);
    value_free(&gateway->intents);
    
    // Remove from registry
    if (g_gateway_connections == gateway) {
        g_gateway_connections = gateway->next;
    } else {
        GatewayConnection* current = g_gateway_connections;
        while (current && current->next != gateway) {
            current = current->next;
        }
        if (current) {
            current->next = gateway->next;
        }
    }
    
    shared_free_safe(gateway, "gateway", "gateway_free", 0);
}

// Connect gateway
void gateway_connect(GatewayConnection* gateway) {
    if (!gateway || !gateway->ws_conn) return;
    
    gateway->state = GATEWAY_STATE_CONNECTING;
    
    // WebSocket connection is already established by websocket_connect_async
    // We just need to wait for HELLO and then identify
    gateway->state = GATEWAY_STATE_CONNECTED;
}

// Disconnect gateway
void gateway_disconnect(GatewayConnection* gateway) {
    if (!gateway || !gateway->ws_conn) return;
    
    gateway->state = GATEWAY_STATE_DISCONNECTED;
    websocket_close(gateway->ws_conn);
}

// Send heartbeat
void gateway_send_heartbeat(GatewayConnection* gateway) {
    if (!gateway || !gateway->ws_conn) return;
    
    // Create heartbeat payload (sequence number or null)
    Value heartbeat_data = value_create_null();
    if (gateway->sequence_number > 0) {
        heartbeat_data = value_create_number((double)gateway->sequence_number);
    }
    
    // Send as Gateway message with opcode 1 (HEARTBEAT)
    gateway_send_opcode(gateway, GATEWAY_OPCODE_HEARTBEAT, &heartbeat_data);
    
    gateway->last_heartbeat_time = get_time_ms();
    gateway->waiting_for_ack = true;
    
    value_free(&heartbeat_data);
}

// Send identify payload
void gateway_send_identify(GatewayConnection* gateway, const char* token, Value* properties) {
    if (!gateway || !gateway->ws_conn) {
        fprintf(stderr, "[GATEWAY] Cannot send IDENTIFY: gateway=%p, ws_conn=%p\n", gateway, gateway ? gateway->ws_conn : NULL);
        return;
    }
    
    fprintf(stderr, "[GATEWAY] Creating IDENTIFY payload\n");
    
    // Create identify payload - use minimal required fields only
    // Required: token, properties, intents
    Value identify = value_create_object(3);
    
    // 1. Token (required)
    if (token) {
        value_object_set(&identify, "token", value_create_string(token));
    }
    
    // 2. Properties (required) - minimal set
    if (properties) {
        value_object_set(&identify, "properties", value_clone(properties));
    } else {
        // Default properties (Discord requires $ prefix on property keys)
        Value props = value_create_object(3);
        value_object_set(&props, "$os", value_create_string("linux"));
        value_object_set(&props, "$browser", value_create_string("myco"));
        value_object_set(&props, "$device", value_create_string("myco"));
        value_object_set(&identify, "properties", props);
    }
    
    // 3. Intents (required for bots)
    // Discord requires intents to be an integer, not a float
    if (gateway->intents.type == VALUE_NUMBER) {
        // Ensure intents is an integer (Discord is strict about this)
        uint64_t intents_int = (uint64_t)gateway->intents.data.number_value;
        fprintf(stderr, "[GATEWAY] Adding intents: %llu (as integer)\n", (unsigned long long)intents_int);
        value_object_set(&identify, "intents", value_create_number((double)intents_int));
    } else {
        fprintf(stderr, "[GATEWAY] WARNING: No intents set (type=%d)\n", gateway->intents.type);
        // Set default intents if not provided (GUILDS only)
        value_object_set(&identify, "intents", value_create_number(1.0));
    }
    
    // Note: Not including optional fields like 'v', 'compress', 'large_threshold' to minimize payload
    
    // Send identify
    fprintf(stderr, "[GATEWAY] Sending IDENTIFY opcode\n");
    gateway_send_opcode(gateway, GATEWAY_OPCODE_IDENTIFY, &identify);
    
    value_free(&identify);
    gateway->state = GATEWAY_STATE_AUTHENTICATING;
    fprintf(stderr, "[GATEWAY] IDENTIFY sent, state=AUTHENTICATING\n");
}

// Send resume payload
void gateway_send_resume(GatewayConnection* gateway) {
    if (!gateway || !gateway->ws_conn || !gateway->session_id) return;
    
    Value resume = value_create_object(3);
    value_object_set(&resume, "token", value_create_string(gateway->config.token ? gateway->config.token : ""));
    value_object_set(&resume, "session_id", value_create_string(gateway->session_id));
    value_object_set(&resume, "seq", value_create_number((double)gateway->sequence_number));
    
    gateway_send_opcode(gateway, GATEWAY_OPCODE_RESUME, &resume);
    
    value_free(&resume);
    gateway->state = GATEWAY_STATE_RESUMING;
}

// Send opcode with data
void gateway_send_opcode(GatewayConnection* gateway, GatewayOpcode opcode, Value* data) {
    if (!gateway || !gateway->ws_conn) return;
    
    // Create gateway message
    Value message = value_create_object(2);
    value_object_set(&message, "op", value_create_number((double)opcode));
    
    if (data && data->type != VALUE_NULL) {
        value_object_set(&message, "d", value_clone(data));
    } else {
        value_object_set(&message, "d", value_create_null());
    }
    
    // Convert to JSON and send
    Value json_val = builtin_json_stringify(NULL, (Value[]){message}, 1, 0, 0);
    if (json_val.type == VALUE_STRING && json_val.data.string_value) {
        const char* json_str = json_val.data.string_value;
        size_t json_len = strlen(json_str);
        fprintf(stderr, "[GATEWAY] Sending WebSocket message, length=%zu, opcode=%d\n", json_len, opcode);
        if (opcode == GATEWAY_OPCODE_IDENTIFY) {
            fprintf(stderr, "[GATEWAY] IDENTIFY payload: %s\n", json_str);
            // Debug: Print first 50 bytes as hex to check encoding
            fprintf(stderr, "[GATEWAY] Payload hex (first 50 bytes): ");
            for (size_t i = 0; i < 50 && i < json_len; i++) {
                fprintf(stderr, "%02x ", (unsigned char)json_str[i]);
            }
            fprintf(stderr, "\n");
            // Validate JSON by trying to parse it back
            extern Value builtin_json_parse(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
            Value parse_test = builtin_json_parse(NULL, (Value[]){json_val}, 1, 0, 0);
            if (parse_test.type == VALUE_NULL) {
                fprintf(stderr, "[GATEWAY] WARNING: Generated JSON failed to parse back - may be invalid!\n");
            } else {
                fprintf(stderr, "[GATEWAY] JSON validation: OK (parsed back successfully)\n");
                value_free(&parse_test);
            }
        }
        websocket_send(gateway->ws_conn, json_str, false);
        fprintf(stderr, "[GATEWAY] WebSocket send completed\n");
    } else {
        fprintf(stderr, "[GATEWAY] ERROR: Failed to stringify message (type=%d)\n", json_val.type);
    }
    value_free(&json_val);
    
    value_free(&message);
}

// Handle WebSocket close for gateway connections
void gateway_handle_close(WebSocketConnection* ws_conn, uint16_t status_code, const char* reason) {
    if (!ws_conn) return;
    
    // Find the gateway connection for this WebSocket
    GatewayConnection* gateway = g_gateway_connections;
    while (gateway) {
        if (gateway->ws_conn == ws_conn) {
            gateway->state = GATEWAY_STATE_ERROR;
            fprintf(stderr, "[GATEWAY] Connection closed by Discord: status=%d, reason=%s\n", status_code, reason ? reason : "unknown");
            
            // Call error callback if set
            if (gateway->on_error_callback.type == VALUE_FUNCTION && gateway->interpreter) {
                Value error_obj = value_create_object(3);
                value_object_set(&error_obj, "code", value_create_number((double)status_code));
                if (reason) {
                    value_object_set(&error_obj, "message", value_create_string(reason));
                } else {
                    value_object_set(&error_obj, "message", value_create_string("Connection closed by Discord"));
                }
                value_object_set(&error_obj, "type", value_create_string("close"));
                value_function_call(&gateway->on_error_callback, &error_obj, 1, gateway->interpreter, 0, 0);
                value_free(&error_obj);
            }
            break;
        }
        gateway = gateway->next;
    }
}

// Handle incoming gateway message
static void gateway_handle_message(GatewayConnection* gateway, const char* message) {
    if (!gateway || !message) return;
    
    fprintf(stderr, "[GATEWAY] Handling message, length=%zu\n", strlen(message));
    
    // Parse JSON message
    Value parsed = json_parse_silent(message);
    if (parsed.type != VALUE_OBJECT) {
        value_free(&parsed);
        return;
    }
    
    // Extract opcode
    Value opcode_val = value_object_get(&parsed, "op");
    if (opcode_val.type != VALUE_NUMBER) {
        value_free(&opcode_val);
        value_free(&parsed);
        return;
    }
    
    
    GatewayOpcode opcode = gateway_parse_opcode((int)opcode_val.data.number_value);
    Value data = value_object_get(&parsed, "d");
    Value sequence = value_object_get(&parsed, "s");
    Value event_name = value_object_get(&parsed, "t");
    
    // Update sequence number if present
    if (sequence.type == VALUE_NUMBER) {
        gateway->sequence_number = (uint64_t)sequence.data.number_value;
    }
    
    // Handle opcodes
    switch (opcode) {
        case GATEWAY_OPCODE_HELLO: {
            fprintf(stderr, "[GATEWAY] HELLO received\n");
            // Extract heartbeat interval
            if (data.type == VALUE_OBJECT) {
                Value heartbeat_interval = value_object_get(&data, "heartbeat_interval");
                if (heartbeat_interval.type == VALUE_NUMBER) {
                    gateway->config.heartbeat_interval_ms = (int)heartbeat_interval.data.number_value;
                }
                value_free(&heartbeat_interval);
            }
            
            // Send identify if we have a token (prefer token_copy over config.token)
            const char* token_to_use = gateway->token_copy ? gateway->token_copy : gateway->config.token;
            fprintf(stderr, "[GATEWAY] Token available: %s\n", token_to_use ? "yes" : "no");
            
            // Verify WebSocket connection is fully open before sending IDENTIFY
            if (gateway->ws_conn && gateway->ws_conn->state != WS_STATE_OPEN) {
                fprintf(stderr, "[GATEWAY] WARNING: WebSocket not fully open (state=%d), waiting...\n", gateway->ws_conn->state);
            }
            
            if (token_to_use && gateway->ws_conn && gateway->ws_conn->state == WS_STATE_OPEN) {
                fprintf(stderr, "[GATEWAY] WebSocket is open, sending IDENTIFY\n");
                gateway_send_identify(gateway, token_to_use, NULL);
            } else {
                fprintf(stderr, "[GATEWAY] ERROR: Cannot send IDENTIFY - token=%p, ws_conn=%p, state=%d\n",
                        token_to_use, gateway->ws_conn, gateway->ws_conn ? gateway->ws_conn->state : -1);
            }
            
            gateway->state = GATEWAY_STATE_CONNECTED;
            break;
        }
        
        case GATEWAY_OPCODE_HEARTBEAT_ACK: {
            gateway->waiting_for_ack = false;
            gateway->last_heartbeat_ack_time = get_time_ms();
            break;
        }
        
        case GATEWAY_OPCODE_DISPATCH: {
            // Update sequence number
            if (sequence.type == VALUE_NUMBER) {
                gateway->sequence_number = (uint64_t)sequence.data.number_value;
            }
            
            // Handle READY event
            if (event_name.type == VALUE_STRING) {
                const char* event = event_name.data.string_value;
                
                if (strcmp(event, "READY") == 0) {
                    fprintf(stderr, "[GATEWAY] READY event received!\n");
                    // Extract session ID
                    if (data.type == VALUE_OBJECT) {
                        Value session = value_object_get(&data, "session_id");
                        if (session.type == VALUE_STRING) {
                            if (gateway->session_id) {
                                shared_free_safe(gateway->session_id, "gateway", "handle_message", 0);
                            }
                            gateway->session_id = shared_malloc_safe(strlen(session.data.string_value) + 1, "gateway", "handle_message", 0);
                            if (gateway->session_id) {
                                strcpy(gateway->session_id, session.data.string_value);
                            }
                        }
                        value_free(&session);
                        
                    }
                    
                    gateway->state = GATEWAY_STATE_READY;
                    gateway->can_resume = true;
                    
                    // Call ready callback
                    if (gateway->on_ready_callback.type == VALUE_FUNCTION && gateway->interpreter) {
                        Value event_obj = value_create_object(3);
                        value_object_set(&event_obj, "data", value_clone(&data));
                        
                        // Extract username and id from user object for easier access
                        if (data.type == VALUE_OBJECT) {
                            Value user_obj = value_object_get(&data, "user");
                            if (user_obj.type == VALUE_OBJECT) {
                                Value username_val = value_object_get(&user_obj, "username");
                                Value userid_val = value_object_get(&user_obj, "id");
                                if (username_val.type == VALUE_STRING) {
                                    value_object_set(&event_obj, "username", value_clone(&username_val));
                                }
                                if (userid_val.type == VALUE_STRING) {
                                    value_object_set(&event_obj, "user_id", value_clone(&userid_val));
                                }
                                value_free(&username_val);
                                value_free(&userid_val);
                            }
                            value_free(&user_obj);
                        }
                        
                        Value result = value_function_call(&gateway->on_ready_callback, &event_obj, 1, gateway->interpreter, 0, 0);
                        value_free(&result);
                        value_free(&event_obj);
                    }
                } else {
                    fprintf(stderr, "[GATEWAY] Event: %s (data type=%d)\n", event, data.type);
                    // Check for error events in DISPATCH
                    if (strcmp(event, "ERROR") == 0) {
                        fprintf(stderr, "[GATEWAY] ERROR event received in DISPATCH!\n");
                        gateway->state = GATEWAY_STATE_ERROR;
                        if (data.type == VALUE_OBJECT) {
                            Value code = value_object_get(&data, "code");
                            Value message = value_object_get(&data, "message");
                            if (code.type == VALUE_NUMBER) {
                                fprintf(stderr, "[GATEWAY] Error code: %g\n", code.data.number_value);
                            }
                            if (message.type == VALUE_STRING) {
                                fprintf(stderr, "[GATEWAY] Error message: %s\n", message.data.string_value);
                            }
                            // Call error callback if set
                            if (gateway->on_error_callback.type == VALUE_FUNCTION && gateway->interpreter) {
                                Value error_obj = value_create_object(2);
                                if (code.type == VALUE_NUMBER) {
                                    value_object_set(&error_obj, "code", value_clone(&code));
                                }
                                if (message.type == VALUE_STRING) {
                                    value_object_set(&error_obj, "message", value_clone(&message));
                                }
                                value_function_call(&gateway->on_error_callback, &error_obj, 1, gateway->interpreter, 0, 0);
                                value_free(&error_obj);
                            }
                            value_free(&code);
                            value_free(&message);
                        }
                    }
                }
                
                // Call event callback
                if (gateway->on_event_callback.type == VALUE_FUNCTION && gateway->interpreter) {
                    Value event_data = value_create_object(5);
                    value_object_set(&event_data, "name", value_clone(&event_name));
                    value_object_set(&event_data, "data", value_clone(&data));
                    value_object_set(&event_data, "sequence", value_clone(&sequence));
                    
                    // For READY event, also extract username and user_id for easier access
                    if (strcmp(event, "READY") == 0 && data.type == VALUE_OBJECT) {
                        Value user_obj = value_object_get(&data, "user");
                        if (user_obj.type == VALUE_OBJECT) {
                            Value username_val = value_object_get(&user_obj, "username");
                            Value userid_val = value_object_get(&user_obj, "id");
                            if (username_val.type == VALUE_STRING) {
                                value_object_set(&event_data, "username", value_clone(&username_val));
                            }
                            if (userid_val.type == VALUE_STRING) {
                                value_object_set(&event_data, "user_id", value_clone(&userid_val));
                            }
                            value_free(&username_val);
                            value_free(&userid_val);
                        }
                        value_free(&user_obj);
                    }
                    
                    // Check for errors before calling
                    extern int interpreter_has_error(Interpreter* interpreter);
                    extern void interpreter_clear_error(Interpreter* interpreter);
                    int had_error_before = interpreter_has_error(gateway->interpreter);
                    if (had_error_before) {
                        fprintf(stderr, "[GATEWAY] WARNING: Interpreter has error before calling event callback, clearing it\n");
                        interpreter_clear_error(gateway->interpreter);
                    }
                    
                    fprintf(stderr, "[GATEWAY] Calling event callback (function type=%d, arg type=%d)\n", 
                            gateway->on_event_callback.type, event_data.type);
                    Value result = value_function_call(&gateway->on_event_callback, &event_data, 1, gateway->interpreter, 0, 0);
                    
                    // Check for errors after calling
                    int has_error_after = interpreter_has_error(gateway->interpreter);
                    if (has_error_after) {
                        fprintf(stderr, "[GATEWAY] ERROR: Interpreter error after calling event callback\n");
                    }
                    
                    value_free(&result);
                    value_free(&event_data);
                }
            }
            break;
        }
        
        case GATEWAY_OPCODE_RECONNECT: {
            // Server requested reconnect
            gateway->state = GATEWAY_STATE_RECONNECTING;
            gateway_disconnect(gateway);
            if (gateway->config.auto_reconnect) {
                gateway_connect(gateway);
            }
            break;
        }
        
        case GATEWAY_OPCODE_INVALID_SESSION: {
            fprintf(stderr, "[GATEWAY] INVALID_SESSION received - Token may be invalid!\n");
            // Check if data contains error information
            if (data.type == VALUE_OBJECT) {
                Value code = value_object_get(&data, "code");
                Value message = value_object_get(&data, "message");
                if (code.type == VALUE_NUMBER) {
                    fprintf(stderr, "[GATEWAY] Error code: %g\n", code.data.number_value);
                }
                if (message.type == VALUE_STRING) {
                    fprintf(stderr, "[GATEWAY] Error message: %s\n", message.data.string_value);
                }
                value_free(&code);
                value_free(&message);
            }
            gateway->state = GATEWAY_STATE_ERROR;
            gateway->can_resume = false;
            gateway->session_id = NULL;
            gateway->sequence_number = 0;
            
            // Call error callback if set
            if (gateway->on_error_callback.type == VALUE_FUNCTION && gateway->interpreter) {
                Value error_obj = value_create_object(1);
                value_object_set(&error_obj, "message", value_create_string("Invalid session - token may be invalid or expired"));
                value_function_call(&gateway->on_error_callback, &error_obj, 1, gateway->interpreter, 0, 0);
                value_free(&error_obj);
            }
            break;
        }
        
        default:
            break;
    }
    
    value_free(&opcode_val);
    value_free(&data);
    value_free(&sequence);
    value_free(&event_name);
    value_free(&parsed);
}

// Process incoming gateway messages
void gateway_process_messages(GatewayConnection* gateway) {
    if (!gateway || !gateway->ws_conn) return;
    
    // Check heartbeat timeout
    if (gateway->waiting_for_ack) {
        uint64_t now = get_time_ms();
        if (now - gateway->last_heartbeat_time > (uint64_t)gateway->config.heartbeat_timeout_ms) {
            // Heartbeat timeout - reconnect
            gateway->state = GATEWAY_STATE_ERROR;
            if (gateway->config.auto_reconnect) {
                gateway_disconnect(gateway);
                gateway_connect(gateway);
            }
            return;
        }
    }
    
    // Send heartbeat if needed
    // NOTE: Do NOT send heartbeat immediately after IDENTIFY - wait for READY event
    // Discord requires IDENTIFY to be sent first, then we wait for READY before starting heartbeats
    uint64_t now = get_time_ms();
    if (gateway->state == GATEWAY_STATE_READY) {
        // Only send heartbeat after we've received READY event
        if (now - gateway->last_heartbeat_time >= (uint64_t)gateway->config.heartbeat_interval_ms) {
            gateway_send_heartbeat(gateway);
        }
    }
}

// Set up WebSocket message handler for gateway
static void gateway_setup_websocket_handler(GatewayConnection* gateway) {
    if (!gateway || !gateway->ws_conn) return;
    
    // Create callback function that processes gateway messages
    // We'll use a closure-like approach by storing gateway pointer
    // For now, we'll handle this in gateway_process_all_connections
}

// Set event handlers
void gateway_set_on_ready(GatewayConnection* gateway, Value callback) {
    if (!gateway) return;
    value_free(&gateway->on_ready_callback);
    gateway->on_ready_callback = value_clone(&callback);
}

void gateway_set_on_event(GatewayConnection* gateway, Value callback) {
    if (!gateway) {
        fprintf(stderr, "[GATEWAY] gateway_set_on_event: gateway is NULL\n");
        return;
    }
    fprintf(stderr, "[GATEWAY] gateway_set_on_event: callback.type=%d\n", callback.type);
    value_free(&gateway->on_event_callback);
    gateway->on_event_callback = value_clone(&callback);
    // Interpreter should already be set when gateway was created
    fprintf(stderr, "[GATEWAY] Event callback set (type=%d, interpreter=%p)\n", 
            gateway->on_event_callback.type, (void*)gateway->interpreter);
}

void gateway_set_on_error(GatewayConnection* gateway, Value callback) {
    if (!gateway) return;
    value_free(&gateway->on_error_callback);
    gateway->on_error_callback = value_clone(&callback);
}

void gateway_set_on_close(GatewayConnection* gateway, Value callback) {
    if (!gateway) return;
    value_free(&gateway->on_close_callback);
    gateway->on_close_callback = value_clone(&callback);
}

// Get gateway state
GatewayState gateway_get_state(GatewayConnection* gateway) {
    if (!gateway) return GATEWAY_STATE_DISCONNECTED;
    return gateway->state;
}

// Set gateway configuration
void gateway_set_config(GatewayConnection* gateway, GatewayConfig* config) {
    if (!gateway || !config) return;
    gateway->config = *config;
}

// Process all gateway connections (called from async event loop)
void gateway_process_all_connections(Interpreter* interpreter) {
    // Note: websocket_process_connections() is already called in async_event_loop_run()
    // before this function, so we don't need to call it again here
    
    // Process gateway-specific logic (heartbeats, etc.)
    GatewayConnection* gateway = g_gateway_connections;
    while (gateway) {
        gateway_process_messages(gateway);
        
        gateway = gateway->next;
    }
}

// Hook into WebSocket message callback (called from WebSocket library)
void gateway_handle_websocket_message(WebSocketConnection* ws_conn, const char* message) {
    if (!ws_conn || !message) return;
    
    // Find gateway connection for this WebSocket
    GatewayConnection* gateway = g_gateway_connections;
    while (gateway) {
        if (gateway->ws_conn == ws_conn) {
            gateway_handle_message(gateway, message);
            break;
        }
        gateway = gateway->next;
    }
}

// Builtin functions for Myco
Value builtin_gateway_create(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 1 || args[0].type != VALUE_STRING) {
        std_error_report(ERROR_TYPE_MISMATCH, "gateway", "create", "gateway.create() requires a URL string", line, column);
        return value_create_null();
    }
    
    GatewayConfig config = gateway_create_default_config();
    
    // Parse optional config object (can be VALUE_OBJECT, VALUE_HASH_MAP, or VALUE_SET)
    if (arg_count >= 2 && (args[1].type == VALUE_OBJECT || args[1].type == VALUE_HASH_MAP || args[1].type == VALUE_SET)) {
        Value config_obj = args[1];
        
        // Debug: Print all keys in the config object
        if (config_obj.type == VALUE_HASH_MAP) {
            for (size_t i = 0; i < config_obj.data.hash_map_value.count; i++) {
                Value* key = (Value*)config_obj.data.hash_map_value.keys[i];
                if (key && key->type == VALUE_STRING) {
                }
            }
        } else if (config_obj.type == VALUE_OBJECT) {
            for (size_t i = 0; i < config_obj.data.object_value.count; i++) {
                if (config_obj.data.object_value.keys[i]) {
                }
            }
        }
        
        // Helper macro to get value from config (handles object, hash map, and set)
        #define GET_CONFIG_VALUE(key_name, var_name) \
            Value var_name; \
            if (config_obj.type == VALUE_HASH_MAP || config_obj.type == VALUE_SET) { \
                Value key_val_##var_name = value_create_string(key_name); \
                var_name = value_hash_map_get(&config_obj, key_val_##var_name); \
                value_free(&key_val_##var_name); \
            } else { \
                var_name = value_object_get(&config_obj, key_name); \
            }
        
        GET_CONFIG_VALUE("heartbeatInterval", heartbeat_interval)
        if (heartbeat_interval.type == VALUE_NUMBER) {
            config.heartbeat_interval_ms = (int)heartbeat_interval.data.number_value;
        }
        value_free(&heartbeat_interval);
        
        GET_CONFIG_VALUE("heartbeatTimeout", heartbeat_timeout)
        if (heartbeat_timeout.type == VALUE_NUMBER) {
            config.heartbeat_timeout_ms = (int)heartbeat_timeout.data.number_value;
        }
        value_free(&heartbeat_timeout);
        
        GET_CONFIG_VALUE("reconnectDelay", reconnect_delay)
        if (reconnect_delay.type == VALUE_NUMBER) {
            config.reconnect_delay_ms = (int)reconnect_delay.data.number_value;
        }
        value_free(&reconnect_delay);
        
        GET_CONFIG_VALUE("maxReconnectAttempts", max_attempts)
        if (max_attempts.type == VALUE_NUMBER) {
            config.max_reconnect_attempts = (int)max_attempts.data.number_value;
        }
        value_free(&max_attempts);
        
        GET_CONFIG_VALUE("autoReconnect", auto_reconnect)
        if (auto_reconnect.type == VALUE_BOOLEAN) {
            config.auto_reconnect = auto_reconnect.data.boolean_value;
        }
        value_free(&auto_reconnect);
        
        GET_CONFIG_VALUE("resumeOnReconnect", resume)
        if (resume.type == VALUE_BOOLEAN) {
            config.resume_on_reconnect = resume.data.boolean_value;
        }
        value_free(&resume);
        
        GET_CONFIG_VALUE("token", token)
        if (token.type == VALUE_STRING && token.data.string_value) {
            config.token = token.data.string_value;  // Temporary pointer, will be copied after gateway creation
        } else {
        }
        value_free(&token);
        
        GET_CONFIG_VALUE("version", version)
        if (version.type == VALUE_NUMBER) {
            config.gateway_version = (int)version.data.number_value;
        }
        value_free(&version);
        
        GET_CONFIG_VALUE("intents", intents)
        if (intents.type == VALUE_NUMBER || intents.type == VALUE_OBJECT) {
            // Store intents for later use in IDENTIFY
            // We'll store it in the gateway connection after creation
        }
        value_free(&intents);
        
        #undef GET_CONFIG_VALUE
    }
    
    GatewayConnection* gateway = gateway_create(args[0].data.string_value, &config, interpreter);
    if (!gateway) {
        std_error_report(ERROR_INTERNAL_ERROR, "gateway", "create", "Failed to create gateway connection", line, column);
        return value_create_null();
    }
    
    // Copy token if provided in config (must persist after config object is freed)
    // Config can be VALUE_OBJECT, VALUE_HASH_MAP, or VALUE_SET (empty hash map literals may be created as sets)
    if (arg_count >= 2 && (args[1].type == VALUE_OBJECT || args[1].type == VALUE_HASH_MAP || args[1].type == VALUE_SET)) {
        Value config_obj = args[1];
        
        Value token;
        if (config_obj.type == VALUE_HASH_MAP) {
            Value token_key = value_create_string("token");
            token = value_hash_map_get(&config_obj, token_key);
            value_free(&token_key);
        } else if (config_obj.type == VALUE_SET) {
            // Sets don't support key-value lookups, so we can't get the token
            token = value_create_null();
        } else {
            token = value_object_get(&config_obj, "token");
        }
        if (token.type == VALUE_STRING && token.data.string_value) {
            size_t token_len = strlen(token.data.string_value);
            gateway->token_copy = shared_malloc_safe(token_len + 1, "gateway", "gateway_create", 0);
            if (gateway->token_copy) {
                strcpy(gateway->token_copy, token.data.string_value);
                gateway->config.token = gateway->token_copy;
            }
        }
        value_free(&token);
    } else {
    }
    
    // Store intents if provided in config
    if (arg_count >= 2 && (args[1].type == VALUE_OBJECT || args[1].type == VALUE_HASH_MAP || args[1].type == VALUE_SET)) {
        Value config_obj = args[1];
        Value intents;
        fprintf(stderr, "[GATEWAY] Extracting intents from config (type=%d)\n", config_obj.type);
        if (config_obj.type == VALUE_HASH_MAP) {
            fprintf(stderr, "[GATEWAY] Config is hash map, count=%zu\n", config_obj.data.hash_map_value.count);
            Value intents_key = value_create_string("intents");
            intents = value_hash_map_get(&config_obj, intents_key);
            value_free(&intents_key);
            fprintf(stderr, "[GATEWAY] Intents from hash map: type=%d\n", intents.type);
            if (intents.type == VALUE_NULL) {
                // value_hash_map_get is not working correctly, manually search for intents
                for (size_t i = 0; i < config_obj.data.hash_map_value.count; i++) {
                    Value* key = (Value*)config_obj.data.hash_map_value.keys[i];
                    if (key && key->type == VALUE_STRING && key->data.string_value) {
                        if (strcmp(key->data.string_value, "intents") == 0) {
                            Value* value = (Value*)config_obj.data.hash_map_value.values[i];
                            if (value && value->type == VALUE_NUMBER) {
                                gateway->intents = value_clone(value);
                                break;
                            }
                        }
                    }
                }
            }
        } else if (config_obj.type == VALUE_SET) {
            fprintf(stderr, "[GATEWAY] Config is SET, cannot extract intents\n");
            intents = value_create_null();
        } else {
            intents = value_object_get(&config_obj, "intents");
            fprintf(stderr, "[GATEWAY] Intents from object: type=%d\n", intents.type);
        }
        if (intents.type == VALUE_NUMBER) {
            fprintf(stderr, "[GATEWAY] Storing intents: %g\n", intents.data.number_value);
            gateway->intents = value_clone(&intents);
        } else if (intents.type == VALUE_FUNCTION && interpreter) {
            // Intents might be a function that needs to be called (e.g., Intents.default())
            Value result = value_function_call(&intents, NULL, 0, interpreter, 0, 0);
            if (result.type == VALUE_NUMBER) {
                gateway->intents = value_clone(&result);
            }
            value_free(&result);
        } else if (intents.type == VALUE_OBJECT) {
            // Intents might be an object - try to call it as a function or get a value
            // For Discord, intents should be a Number, so try common patterns
            Value intents_value = value_object_get(&intents, "value");
            if (intents_value.type == VALUE_NUMBER) {
                gateway->intents = value_clone(&intents_value);
            } else {
                // Try calling as function if it has a callable property
                gateway->intents = value_clone(&intents);
            }
            value_free(&intents_value);
        }
        value_free(&intents);
    }
    
    Value gateway_obj = value_create_object(10);
    value_object_set(&gateway_obj, "__gateway_conn__", value_create_number((double)(intptr_t)gateway));
    value_object_set(&gateway_obj, "state", value_create_string("disconnected"));
    value_object_set(&gateway_obj, "connect", value_create_builtin_function(builtin_gateway_connect));
    value_object_set(&gateway_obj, "disconnect", value_create_builtin_function(builtin_gateway_disconnect));
    value_object_set(&gateway_obj, "send", value_create_builtin_function(builtin_gateway_send));
    value_object_set(&gateway_obj, "on", value_create_builtin_function(builtin_gateway_on));
    value_object_set(&gateway_obj, "setConfig", value_create_builtin_function(builtin_gateway_set_config));
    value_object_set(&gateway_obj, "getState", value_create_builtin_function(builtin_gateway_get_state));
    
    return gateway_obj;
}

Value builtin_gateway_connect(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    Value* self = interpreter_get_self_context(interpreter);
    if (!self || self->type != VALUE_OBJECT) {
        return value_create_null();
    }
    
    Value gateway_val = value_object_get(self, "__gateway_conn__");
    if (gateway_val.type != VALUE_NUMBER) {
        return value_create_null();
    }
    
    GatewayConnection* gateway = (GatewayConnection*)(intptr_t)gateway_val.data.number_value;
    if (!gateway) {
        return value_create_null();
    }
    
    gateway_connect(gateway);
    return value_create_null();
}

Value builtin_gateway_disconnect(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    Value* self = interpreter_get_self_context(interpreter);
    if (!self || self->type != VALUE_OBJECT) {
        return value_create_null();
    }
    
    Value gateway_val = value_object_get(self, "__gateway_conn__");
    if (gateway_val.type != VALUE_NUMBER) {
        return value_create_null();
    }
    
    GatewayConnection* gateway = (GatewayConnection*)(intptr_t)gateway_val.data.number_value;
    if (!gateway) {
        return value_create_null();
    }
    
    gateway_disconnect(gateway);
    return value_create_null();
}

Value builtin_gateway_send(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 2) {
        std_error_report(ERROR_INTERNAL_ERROR, "gateway", "send", "gateway.send() requires opcode and data", line, column);
        return value_create_null();
    }
    
    Value* self = interpreter_get_self_context(interpreter);
    if (!self || self->type != VALUE_OBJECT) {
        return value_create_null();
    }
    
    Value gateway_val = value_object_get(self, "__gateway_conn__");
    if (gateway_val.type != VALUE_NUMBER) {
        return value_create_null();
    }
    
    GatewayConnection* gateway = (GatewayConnection*)(intptr_t)gateway_val.data.number_value;
    if (!gateway) {
        return value_create_null();
    }
    
    if (args[0].type != VALUE_NUMBER) {
        std_error_report(ERROR_TYPE_MISMATCH, "gateway", "send", "gateway.send() requires opcode as number", line, column);
        return value_create_null();
    }
    
    GatewayOpcode opcode = gateway_parse_opcode((int)args[0].data.number_value);
    Value data = arg_count >= 2 ? args[1] : value_create_null();
    
    gateway_send_opcode(gateway, opcode, &data);
    return value_create_null();
}

Value builtin_gateway_on(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    // When called as a method, args[0] is the object (self), args[1] is event name, args[2] is callback
    // Check self_context first, but if not set, use args[0] as the object
    Value* self = interpreter_get_self_context(interpreter);
    size_t event_name_idx = 0;
    size_t callback_idx = 1;
    
    if (!self || self->type != VALUE_OBJECT) {
        // self_context not set - this is a method call, so args[0] is the object
        if (arg_count > 0 && args[0].type == VALUE_OBJECT) {
            self = &args[0];
            event_name_idx = 1;
            callback_idx = 2;
        } else {
            return value_create_null();
        }
    }
    // else: self_context is set, so args[0] is event_name, args[1] is callback
    
    if (arg_count < callback_idx + 1 || args[event_name_idx].type != VALUE_STRING) {
        std_error_report(ERROR_TYPE_MISMATCH, "gateway", "on", "gateway.on() requires event name and callback", line, column);
        return value_create_null();
    }
    
    Value gateway_val = value_object_get(self, "__gateway_conn__");
    if (gateway_val.type != VALUE_NUMBER) {
        return value_create_null();
    }
    
    GatewayConnection* gateway = (GatewayConnection*)(intptr_t)gateway_val.data.number_value;
    if (!gateway) {
        return value_create_null();
    }
    
    const char* event_name = args[event_name_idx].data.string_value;
    Value callback = args[callback_idx];
    
    fprintf(stderr, "[GATEWAY] builtin_gateway_on called: event_name='%s', callback.type=%d, gateway=%p\n",
            event_name, callback.type, (void*)gateway);
    
    if (strcmp(event_name, "ready") == 0) {
        fprintf(stderr, "[GATEWAY] Setting ready callback\n");
        gateway_set_on_ready(gateway, callback);
    } else if (strcmp(event_name, "event") == 0) {
        fprintf(stderr, "[GATEWAY] Registering event callback (type=%d, is_function=%d)\n", 
                callback.type, (callback.type == VALUE_FUNCTION ? 1 : 0));
        gateway_set_on_event(gateway, callback);
    } else if (strcmp(event_name, "error") == 0) {
        gateway_set_on_error(gateway, callback);
    } else if (strcmp(event_name, "close") == 0) {
        gateway_set_on_close(gateway, callback);
    } else {
        fprintf(stderr, "[GATEWAY] Unknown event name: %s\n", event_name);
    }
    
    return value_create_null();
}

Value builtin_gateway_set_config(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 1 || args[0].type != VALUE_OBJECT) {
        std_error_report(ERROR_TYPE_MISMATCH, "gateway", "setConfig", "gateway.setConfig() requires a config object", line, column);
        return value_create_null();
    }
    
    Value* self = interpreter_get_self_context(interpreter);
    if (!self || self->type != VALUE_OBJECT) {
        return value_create_null();
    }
    
    Value gateway_val = value_object_get(self, "__gateway_conn__");
    if (gateway_val.type != VALUE_NUMBER) {
        return value_create_null();
    }
    
    GatewayConnection* gateway = (GatewayConnection*)(intptr_t)gateway_val.data.number_value;
    if (!gateway) {
        return value_create_null();
    }
    
    // Update config (similar to builtin_gateway_create)
    Value config_obj = args[0];
    
    Value heartbeat_interval = value_object_get(&config_obj, "heartbeatInterval");
    if (heartbeat_interval.type == VALUE_NUMBER) {
        gateway->config.heartbeat_interval_ms = (int)heartbeat_interval.data.number_value;
    }
    
    Value heartbeat_timeout = value_object_get(&config_obj, "heartbeatTimeout");
    if (heartbeat_timeout.type == VALUE_NUMBER) {
        gateway->config.heartbeat_timeout_ms = (int)heartbeat_timeout.data.number_value;
    }
    
    Value reconnect_delay = value_object_get(&config_obj, "reconnectDelay");
    if (reconnect_delay.type == VALUE_NUMBER) {
        gateway->config.reconnect_delay_ms = (int)reconnect_delay.data.number_value;
    }
    
    Value max_attempts = value_object_get(&config_obj, "maxReconnectAttempts");
    if (max_attempts.type == VALUE_NUMBER) {
        gateway->config.max_reconnect_attempts = (int)max_attempts.data.number_value;
    }
    
    Value auto_reconnect = value_object_get(&config_obj, "autoReconnect");
    if (auto_reconnect.type == VALUE_BOOLEAN) {
        gateway->config.auto_reconnect = auto_reconnect.data.boolean_value;
    }
    
    return value_create_null();
}

Value builtin_gateway_get_state(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    Value* self = interpreter_get_self_context(interpreter);
    if (!self || self->type != VALUE_OBJECT) {
        return value_create_null();
    }
    
    Value gateway_val = value_object_get(self, "__gateway_conn__");
    if (gateway_val.type != VALUE_NUMBER) {
        return value_create_null();
    }
    
    GatewayConnection* gateway = (GatewayConnection*)(intptr_t)gateway_val.data.number_value;
    if (!gateway) {
        return value_create_null();
    }
    
    const char* state_str = "disconnected";
    switch (gateway_get_state(gateway)) {
        case GATEWAY_STATE_CONNECTING: state_str = "connecting"; break;
        case GATEWAY_STATE_CONNECTED: state_str = "connected"; break;
        case GATEWAY_STATE_AUTHENTICATING: state_str = "authenticating"; break;
        case GATEWAY_STATE_READY: state_str = "ready"; break;
        case GATEWAY_STATE_RECONNECTING: state_str = "reconnecting"; break;
        case GATEWAY_STATE_RESUMING: state_str = "resuming"; break;
        case GATEWAY_STATE_ERROR: state_str = "error"; break;
        default: break;
    }
    
    return value_create_string(state_str);
}

// Register gateway library
void gateway_library_register(Interpreter* interpreter) {
    if (!interpreter || !interpreter->global_environment) return;
    
    Value gateway_lib = value_create_object(8);
    value_object_set(&gateway_lib, "__type__", value_create_string("Library"));
    value_object_set(&gateway_lib, "type", value_create_string("Library"));
    value_object_set(&gateway_lib, "__library_name__", value_create_string("gateway"));
    
    value_object_set(&gateway_lib, "create", value_create_builtin_function(builtin_gateway_create));
    
    // Opcode constants
    value_object_set(&gateway_lib, "OPCODE_DISPATCH", value_create_number(0));
    value_object_set(&gateway_lib, "OPCODE_HEARTBEAT", value_create_number(1));
    value_object_set(&gateway_lib, "OPCODE_IDENTIFY", value_create_number(2));
    value_object_set(&gateway_lib, "OPCODE_RESUME", value_create_number(6));
    value_object_set(&gateway_lib, "OPCODE_HELLO", value_create_number(10));
    value_object_set(&gateway_lib, "OPCODE_HEARTBEAT_ACK", value_create_number(11));
    
    environment_define(interpreter->global_environment, "gateway", gateway_lib);
}

