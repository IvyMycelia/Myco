#ifndef MYCO_CLI_ARDUINO_EMITTER_H
#define MYCO_CLI_ARDUINO_EMITTER_H

// Emits a minimal Arduino .ino sketch translating a subset of Myco's arduino.* calls.
// Returns 0 on success, non-zero on error.
int emit_arduino_sketch_from_file(const char* myco_input_path, const char* out_ino_path);

#endif

