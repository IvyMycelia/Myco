#include "../../include/libs/arduino.h"
#include "../../include/core/interpreter.h"
#include "../../include/core/standardized_errors.h"
#include "../../include/utils/shared_utilities.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

// Cross-platform host-simulated Arduino constants
#define ARDUINO_INPUT 0
#define ARDUINO_OUTPUT 1
#define ARDUINO_LOW 0
#define ARDUINO_HIGH 1

typedef struct SimPin {
    int mode;   // INPUT/OUTPUT
    int value;  // LOW/HIGH or 0-255 for analogWrite
} SimPin;

// Very small simulated board: 0..53 typical
#define MAX_PINS 64
static SimPin g_pins[MAX_PINS];
static int g_initialized = 0;

static void arduino_sim_init(void) {
    if (g_initialized) return;
    memset(g_pins, 0, sizeof(g_pins));
    g_initialized = 1;
}

static int get_int(Value v, int *out) {
    if (v.type == VALUE_NUMBER) { *out = (int)v.data.number_value; return 1; }
    return 0;
}

static void arduino_attach_constants(Value *arduino_obj) {
    value_object_set(arduino_obj, "INPUT", value_create_number(ARDUINO_INPUT));
    value_object_set(arduino_obj, "OUTPUT", value_create_number(ARDUINO_OUTPUT));
    value_object_set(arduino_obj, "LOW", value_create_number(ARDUINO_LOW));
    value_object_set(arduino_obj, "HIGH", value_create_number(ARDUINO_HIGH));
}

Value builtin_arduino_pinMode(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        std_error_report(ERROR_INVALID_ARGUMENT, "arduino", "builtin_arduino_pinMode", "pinMode(pin, mode) requires 2 arguments", line, column);
        return value_create_null();
    }
    arduino_sim_init();
    int pin = 0, mode = 0;
    if (!get_int(args[0], &pin) || !get_int(args[1], &mode)) {
        std_error_report(ERROR_INVALID_ARGUMENT, "arduino", "builtin_arduino_pinMode", "pin and mode must be numbers", line, column);
        return value_create_null();
    }
    if (pin < 0 || pin >= MAX_PINS) {
        std_error_report(ERROR_INVALID_ARGUMENT, "arduino", "builtin_arduino_pinMode", "pin out of range", line, column);
        return value_create_null();
    }
    g_pins[pin].mode = mode ? ARDUINO_OUTPUT : ARDUINO_INPUT;
    return value_create_null();
}

Value builtin_arduino_digitalWrite(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        std_error_report(ERROR_INVALID_ARGUMENT, "arduino", "builtin_arduino_digitalWrite", "digitalWrite(pin, value) requires 2 arguments", line, column);
        return value_create_null();
    }
    arduino_sim_init();
    int pin = 0, val = 0;
    if (!get_int(args[0], &pin) || !get_int(args[1], &val)) {
        std_error_report(ERROR_INVALID_ARGUMENT, "arduino", "builtin_arduino_digitalWrite", "pin and value must be numbers", line, column);
        return value_create_null();
    }
    if (pin < 0 || pin >= MAX_PINS) {
        std_error_report(ERROR_INVALID_ARGUMENT, "arduino", "builtin_arduino_digitalWrite", "pin out of range", line, column);
        return value_create_null();
    }
    if (g_pins[pin].mode != ARDUINO_OUTPUT) {
        std_error_report(ERROR_INVALID_ARGUMENT, "arduino", "builtin_arduino_digitalWrite", "pin not configured as OUTPUT", line, column);
        return value_create_null();
    }
    g_pins[pin].value = val ? ARDUINO_HIGH : ARDUINO_LOW;
    // Simulate by printing (non-intrusive)
    printf("[arduino] digitalWrite(%d,%s)\n", pin, g_pins[pin].value ? "HIGH" : "LOW");
    return value_create_null();
}

Value builtin_arduino_digitalRead(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_INVALID_ARGUMENT, "arduino", "builtin_arduino_digitalRead", "digitalRead(pin) requires 1 argument", line, column);
        return value_create_null();
    }
    arduino_sim_init();
    int pin = 0;
    if (!get_int(args[0], &pin)) {
        std_error_report(ERROR_INVALID_ARGUMENT, "arduino", "builtin_arduino_digitalRead", "pin must be a number", line, column);
        return value_create_null();
    }
    if (pin < 0 || pin >= MAX_PINS) {
        std_error_report(ERROR_INVALID_ARGUMENT, "arduino", "builtin_arduino_digitalRead", "pin out of range", line, column);
        return value_create_null();
    }
    return value_create_number((double)(g_pins[pin].value ? 1 : 0));
}

Value builtin_arduino_analogRead(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_INVALID_ARGUMENT, "arduino", "builtin_arduino_analogRead", "analogRead(pin) requires 1 argument", line, column);
        return value_create_null();
    }
    arduino_sim_init();
    int pin = 0;
    if (!get_int(args[0], &pin)) {
        std_error_report(ERROR_INVALID_ARGUMENT, "arduino", "builtin_arduino_analogRead", "pin must be a number", line, column);
        return value_create_null();
    }
    if (pin < 0 || pin >= MAX_PINS) {
        std_error_report(ERROR_INVALID_ARGUMENT, "arduino", "builtin_arduino_analogRead", "pin out of range", line, column);
        return value_create_null();
    }
    // Simulated analog value: reflect stored value * 1023 if output, else midpoint
    int val = (g_pins[pin].mode == ARDUINO_OUTPUT) ? (g_pins[pin].value ? 1023 : 0) : 512;
    return value_create_number((double)val);
}

Value builtin_arduino_analogWrite(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        std_error_report(ERROR_INVALID_ARGUMENT, "arduino", "builtin_arduino_analogWrite", "analogWrite(pin, value) requires 2 arguments", line, column);
        return value_create_null();
    }
    arduino_sim_init();
    int pin = 0, val = 0;
    if (!get_int(args[0], &pin) || !get_int(args[1], &val)) {
        std_error_report(ERROR_INVALID_ARGUMENT, "arduino", "builtin_arduino_analogWrite", "pin and value must be numbers", line, column);
        return value_create_null();
    }
    if (pin < 0 || pin >= MAX_PINS) {
        std_error_report(ERROR_INVALID_ARGUMENT, "arduino", "builtin_arduino_analogWrite", "pin out of range", line, column);
        return value_create_null();
    }
    if (g_pins[pin].mode != ARDUINO_OUTPUT) {
        std_error_report(ERROR_INVALID_ARGUMENT, "arduino", "builtin_arduino_analogWrite", "pin not configured as OUTPUT", line, column);
        return value_create_null();
    }
    if (val < 0) val = 0; if (val > 255) val = 255;
    g_pins[pin].value = val;
    printf("[arduino] analogWrite(%d,%d)\n", pin, val);
    return value_create_null();
}

Value builtin_arduino_delay(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_INVALID_ARGUMENT, "arduino", "builtin_arduino_delay", "delay(ms) requires 1 argument", line, column);
        return value_create_null();
    }
    int ms = 0;
    if (!get_int(args[0], &ms)) {
        std_error_report(ERROR_INVALID_ARGUMENT, "arduino", "builtin_arduino_delay", "ms must be a number", line, column);
        return value_create_null();
    }
    if (ms < 0) ms = 0;
#if MYCO_PLATFORM_WINDOWS
    Sleep((DWORD)ms);
#else
    struct timespec ts; ts.tv_sec = ms / 1000; ts.tv_nsec = (ms % 1000) * 1000000L;
    nanosleep(&ts, NULL);
#endif
    return value_create_null();
}

// arduino.run(setupFn, loopFn)
Value builtin_arduino_run(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    // For portability (no direct callable API), simulate run lifecycle without invoking functions.
    // We validate signatures and provide deterministic loop iterations.
    if (arg_count != 2) {
        std_error_report(ERROR_INVALID_ARGUMENT, "arduino", "builtin_arduino_run", "run(setup, loop) requires 2 function arguments", line, column);
        return value_create_null();
    }
    if (args[0].type != VALUE_FUNCTION || args[1].type != VALUE_FUNCTION) {
        std_error_report(ERROR_INVALID_ARGUMENT, "arduino", "builtin_arduino_run", "setup and loop must be functions", line, column);
        return value_create_null();
    }
    arduino_sim_init();
    printf("[arduino] run: setup() and loop() lifecycle simulation start\n");
    // Simulate limited iterations to avoid infinite loops on host
    for (int i = 0; i < 5; i++) {
        printf("[arduino] loop iteration %d\n", i + 1);
    }
    printf("[arduino] run: lifecycle simulation end\n");
    return value_create_null();
}

void arduino_library_register(Interpreter* interpreter) {
    if (!interpreter || !interpreter->global_environment) return;
    Value arduino_obj = value_create_object(16);
    arduino_attach_constants(&arduino_obj);
    value_object_set(&arduino_obj, "pinMode", value_create_builtin_function(builtin_arduino_pinMode));
    value_object_set(&arduino_obj, "digitalWrite", value_create_builtin_function(builtin_arduino_digitalWrite));
    value_object_set(&arduino_obj, "digitalRead", value_create_builtin_function(builtin_arduino_digitalRead));
    value_object_set(&arduino_obj, "analogRead", value_create_builtin_function(builtin_arduino_analogRead));
    value_object_set(&arduino_obj, "analogWrite", value_create_builtin_function(builtin_arduino_analogWrite));
    value_object_set(&arduino_obj, "delay", value_create_builtin_function(builtin_arduino_delay));
    value_object_set(&arduino_obj, "run", value_create_builtin_function(builtin_arduino_run));
    environment_define(interpreter->global_environment, "arduino", arduino_obj);
}


