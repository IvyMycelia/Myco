#ifndef COMPRESSION_H
#define COMPRESSION_H

#include <stddef.h>
#include <stdbool.h>

// Custom compression to replace zlib dependency
// This provides a lightweight, dependency-free compression system

// Compression algorithm types
typedef enum {
    COMPRESSION_NONE,    // No compression
    COMPRESSION_RLE,     // Run-Length Encoding
    COMPRESSION_DICT     // Dictionary compression
} CompressionType;

// Core compression functions
char* compress_data(const char* data, size_t data_size, CompressionType type, size_t* compressed_size);
char* decompress_data(const char* data, size_t data_size, CompressionType type, size_t* decompressed_size);

// Utility functions
double get_compression_ratio(size_t original_size, size_t compressed_size);
bool is_compression_beneficial(size_t original_size, size_t compressed_size);

#endif // COMPRESSION_H
