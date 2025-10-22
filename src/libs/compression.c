#include "../../include/libs/compression.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "../../include/utils/shared_utilities.h"

// Custom compression implementation to replace zlib dependency
// This provides a lightweight, dependency-free compression system

// Simple RLE (Run-Length Encoding) compression
static char* rle_compress(const char* data, size_t data_size, size_t* compressed_size) {
    if (!data || data_size == 0) return NULL;
    
    // Allocate output buffer (worst case: 2x input size)
    char* output = shared_malloc_safe(data_size * 2, "compression", "rle_compress", 0);
    if (!output) return NULL;
    
    size_t out_pos = 0;
    size_t in_pos = 0;
    
    while (in_pos < data_size) {
        char current_char = data[in_pos];
        size_t run_length = 1;
        
        // Count consecutive identical characters
        while (in_pos + run_length < data_size && 
               data[in_pos + run_length] == current_char && 
               run_length < 255) {
            run_length++;
        }
        
        // Encode run
        if (run_length > 3 || current_char == 0) {
            // Use RLE encoding: 0x00 count char
            if (out_pos + 3 >= data_size * 2) {
                // Buffer overflow, fallback to simple copy
                output[out_pos++] = current_char;
                in_pos++;
                continue;
            }
            
            output[out_pos++] = 0x00; // RLE marker
            output[out_pos++] = (char)run_length;
            output[out_pos++] = current_char;
        } else {
            // Copy characters directly
            for (size_t i = 0; i < run_length; i++) {
                if (out_pos >= data_size * 2) break;
                output[out_pos++] = current_char;
            }
        }
        
        in_pos += run_length;
    }
    
    *compressed_size = out_pos;
    return output;
}

// Simple RLE decompression
static char* rle_decompress(const char* data, size_t data_size, size_t* decompressed_size) {
    if (!data || data_size == 0) return NULL;
    
    // Estimate output size (worst case: same as input)
    char* output = shared_malloc_safe(data_size, "compression", "rle_decompress", 0);
    if (!output) return NULL;
    
    size_t out_pos = 0;
    size_t in_pos = 0;
    
    while (in_pos < data_size) {
        if (data[in_pos] == 0x00 && in_pos + 2 < data_size) {
            // RLE encoded run
            uint8_t count = (uint8_t)data[in_pos + 1];
            char char_to_repeat = data[in_pos + 2];
            
            for (uint8_t i = 0; i < count; i++) {
                if (out_pos >= data_size) break;
                output[out_pos++] = char_to_repeat;
            }
            
            in_pos += 3;
        } else {
            // Regular character
            if (out_pos >= data_size) break;
            output[out_pos++] = data[in_pos++];
        }
    }
    
    *decompressed_size = out_pos;
    return output;
}

// Simple dictionary compression
static char* dict_compress(const char* data, size_t data_size, size_t* compressed_size) {
    if (!data || data_size == 0) return NULL;
    
    // Simple dictionary: common patterns
    const char* patterns[] = {
        "the", "and", "or", "but", "in", "on", "at", "to", "for", "of", "with", "by",
        "http://", "https://", "www.", ".com", ".org", ".net", ".html", ".css", ".js"
    };
    const char* replacements[] = {
        "\x01", "\x02", "\x03", "\x04", "\x05", "\x06", "\x07", "\x08", "\x09", "\x0A",
        "\x0B", "\x0C", "\x0D", "\x0E", "\x0F", "\x10", "\x11", "\x12", "\x13", "\x14"
    };
    const size_t pattern_count = sizeof(patterns) / sizeof(patterns[0]);
    
    // Allocate output buffer
    char* output = shared_malloc_safe(data_size, "compression", "dict_compress", 0);
    if (!output) return NULL;
    
    size_t out_pos = 0;
    size_t in_pos = 0;
    
    while (in_pos < data_size) {
        bool pattern_found = false;
        
        // Check for patterns
        for (size_t i = 0; i < pattern_count; i++) {
            size_t pattern_len = strlen(patterns[i]);
            if (in_pos + pattern_len <= data_size && 
                strncmp(data + in_pos, patterns[i], pattern_len) == 0) {
                // Found pattern, use replacement
                output[out_pos++] = replacements[i][0];
                in_pos += pattern_len;
                pattern_found = true;
                break;
            }
        }
        
        if (!pattern_found) {
            output[out_pos++] = data[in_pos++];
        }
    }
    
    *compressed_size = out_pos;
    return output;
}

// Simple dictionary decompression
static char* dict_decompress(const char* data, size_t data_size, size_t* decompressed_size) {
    if (!data || data_size == 0) return NULL;
    
    // Dictionary for decompression
    const char* patterns[] = {
        "the", "and", "or", "but", "in", "on", "at", "to", "for", "of", "with", "by",
        "http://", "https://", "www.", ".com", ".org", ".net", ".html", ".css", ".js"
    };
    const size_t pattern_count = sizeof(patterns) / sizeof(patterns[0]);
    
    // Allocate output buffer (worst case: 3x input size)
    char* output = shared_malloc_safe(data_size * 3, "compression", "dict_decompress", 0);
    if (!output) return NULL;
    
    size_t out_pos = 0;
    size_t in_pos = 0;
    
    while (in_pos < data_size) {
        unsigned char c = (unsigned char)data[in_pos];
        
        if (c >= 1 && c <= pattern_count) {
            // Replace with pattern
            const char* pattern = patterns[c - 1];
            size_t pattern_len = strlen(pattern);
            
            if (out_pos + pattern_len < data_size * 3) {
                strcpy(output + out_pos, pattern);
                out_pos += pattern_len;
            } else {
                // Buffer overflow, copy character as-is
                output[out_pos++] = data[in_pos];
            }
        } else {
            // Regular character
            output[out_pos++] = data[in_pos];
        }
        
        in_pos++;
    }
    
    *decompressed_size = out_pos;
    return output;
}

// Compress data using specified algorithm
char* compress_data(const char* data, size_t data_size, CompressionType type, size_t* compressed_size) {
    if (!data || data_size == 0 || !compressed_size) return NULL;
    
    switch (type) {
        case COMPRESSION_RLE:
            return rle_compress(data, data_size, compressed_size);
            
        case COMPRESSION_DICT:
            return dict_compress(data, data_size, compressed_size);
            
        case COMPRESSION_NONE:
        default:
            // No compression, just copy
            *compressed_size = data_size;
            return shared_strdup(data);
    }
}

// Decompress data using specified algorithm
char* decompress_data(const char* data, size_t data_size, CompressionType type, size_t* decompressed_size) {
    if (!data || data_size == 0 || !decompressed_size) return NULL;
    
    switch (type) {
        case COMPRESSION_RLE:
            return rle_decompress(data, data_size, decompressed_size);
            
        case COMPRESSION_DICT:
            return dict_decompress(data, data_size, decompressed_size);
            
        case COMPRESSION_NONE:
        default:
            // No decompression, just copy
            *decompressed_size = data_size;
            return shared_strdup(data);
    }
}

// Get compression ratio
double get_compression_ratio(size_t original_size, size_t compressed_size) {
    if (original_size == 0) return 0.0;
    return (double)compressed_size / (double)original_size;
}

// Check if compression is beneficial
bool is_compression_beneficial(size_t original_size, size_t compressed_size) {
    return compressed_size < original_size;
}
