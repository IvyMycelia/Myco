/**
 * @file cpu_features.h
 * @brief CPU feature detection for platform-specific optimizations
 * 
 * Detects available CPU features at runtime and enables
 * platform-specific optimizations like SIMD instructions.
 */

#ifndef MYCO_CPU_FEATURES_H
#define MYCO_CPU_FEATURES_H

#include <stdint.h>
#include <stddef.h>

/**
 * @brief CPU architecture types
 */
typedef enum {
    CPU_ARCH_UNKNOWN = 0,           // Unknown architecture
    CPU_ARCH_X86_64 = 1,            // x86_64 (AMD64)
    CPU_ARCH_ARM64 = 2,             // ARM64 (AArch64)
    CPU_ARCH_ARM = 3,               // ARM (32-bit)
    CPU_ARCH_RISCV64 = 4,           // RISC-V 64-bit
    CPU_ARCH_PPC64 = 5              // PowerPC 64-bit
} CPUArchitecture;

/**
 * @brief x86_64 CPU features
 */
typedef enum {
    CPU_FEATURE_SSE = 1 << 0,       // SSE (Streaming SIMD Extensions)
    CPU_FEATURE_SSE2 = 1 << 1,      // SSE2
    CPU_FEATURE_SSE3 = 1 << 2,      // SSE3
    CPU_FEATURE_SSSE3 = 1 << 3,     // SSSE3
    CPU_FEATURE_SSE4_1 = 1 << 4,    // SSE4.1
    CPU_FEATURE_SSE4_2 = 1 << 5,    // SSE4.2
    CPU_FEATURE_AVX = 1 << 6,       // AVX (Advanced Vector Extensions)
    CPU_FEATURE_AVX2 = 1 << 7,      // AVX2
    CPU_FEATURE_AVX512F = 1 << 8,   // AVX-512 Foundation
    CPU_FEATURE_AVX512BW = 1 << 9,  // AVX-512 Byte and Word
    CPU_FEATURE_AVX512DQ = 1 << 10, // AVX-512 Doubleword and Quadword
    CPU_FEATURE_AVX512VL = 1 << 11, // AVX-512 Vector Length
    CPU_FEATURE_FMA = 1 << 12,      // FMA (Fused Multiply-Add)
    CPU_FEATURE_BMI1 = 1 << 13,     // BMI1 (Bit Manipulation Instructions)
    CPU_FEATURE_BMI2 = 1 << 14,     // BMI2
    CPU_FEATURE_LZCNT = 1 << 15,    // LZCNT (Leading Zero Count)
    CPU_FEATURE_POPCNT = 1 << 16,   // POPCNT (Population Count)
    CPU_FEATURE_TZCNT = 1 << 17     // TZCNT (Trailing Zero Count)
} CPUFeatureX86_64;

/**
 * @brief ARM64 CPU features
 */
typedef enum {
    CPU_FEATURE_NEON = 1 << 0,      // NEON (Advanced SIMD)
    CPU_FEATURE_CRC32 = 1 << 1,     // CRC32 instructions
    CPU_FEATURE_AES = 1 << 2,       // AES instructions
    CPU_FEATURE_SHA1 = 1 << 3,      // SHA1 instructions
    CPU_FEATURE_SHA2 = 1 << 4,      // SHA2 instructions
    CPU_FEATURE_ATOMICS = 1 << 5,   // Atomic instructions
    CPU_FEATURE_FP16 = 1 << 6,      // Half-precision floating point
    CPU_FEATURE_DOTPROD = 1 << 7,   // Dot product instructions
    CPU_FEATURE_SVE = 1 << 8        // Scalable Vector Extension
} CPUFeatureARM64;

/**
 * @brief CPU feature flags (union of all architectures)
 */
typedef struct {
    CPUArchitecture architecture;   // Detected CPU architecture
    uint64_t features;              // Feature flags (architecture-specific)
    uint32_t cache_line_size;       // Cache line size in bytes
    uint32_t l1d_cache_size;        // L1 data cache size in bytes
    uint32_t l1i_cache_size;        // L1 instruction cache size in bytes
    uint32_t l2_cache_size;         // L2 cache size in bytes
    uint32_t l3_cache_size;         // L3 cache size in bytes
    uint32_t cores;                 // Number of CPU cores
    uint32_t threads;               // Number of hardware threads
    uint32_t max_frequency_mhz;     // Maximum CPU frequency in MHz
    char vendor_string[16];         // CPU vendor string
    char brand_string[64];          // CPU brand string
} CPUFeatures;

/**
 * @brief SIMD instruction set support
 */
typedef enum {
    SIMD_NONE = 0,                  // No SIMD support
    SIMD_SSE2 = 1,                  // SSE2 support
    SIMD_SSE4_2 = 2,                // SSE4.2 support
    SIMD_AVX = 3,                   // AVX support
    SIMD_AVX2 = 4,                  // AVX2 support
    SIMD_AVX512 = 5,                // AVX-512 support
    SIMD_NEON = 6                   // ARM NEON support
} SIMDInstructionSet;

/**
 * @brief CPU feature detection context
 */
typedef struct {
    CPUFeatures features;           // Detected CPU features
    int is_initialized;             // 1 if features have been detected
    int enable_simd;                // 1 if SIMD should be enabled
    int enable_optimizations;       // 1 if CPU-specific optimizations enabled
    SIMDInstructionSet preferred_simd; // Preferred SIMD instruction set
} CPUFeatureContext;

/**
 * @brief Create CPU feature detection context
 * 
 * @return CPUFeatureContext* New context or NULL on failure
 */
CPUFeatureContext* cpu_features_create_context(void);

/**
 * @brief Free CPU feature detection context
 * 
 * @param context Context to free
 */
void cpu_features_free_context(CPUFeatureContext* context);

/**
 * @brief Detect CPU features
 * 
 * @param context CPU feature context
 * @return int 1 on success, 0 on failure
 */
int cpu_features_detect(CPUFeatureContext* context);

/**
 * @brief Check if a specific feature is available
 * 
 * @param context CPU feature context
 * @param feature Feature to check
 * @return int 1 if available, 0 otherwise
 */
int cpu_features_has_feature(CPUFeatureContext* context, uint64_t feature);

/**
 * @brief Get the best SIMD instruction set for the CPU
 * 
 * @param context CPU feature context
 * @return SIMDInstructionSet Best available SIMD instruction set
 */
SIMDInstructionSet cpu_features_get_best_simd(CPUFeatureContext* context);

/**
 * @brief Check if SIMD is available and enabled
 * 
 * @param context CPU feature context
 * @return int 1 if SIMD is available and enabled, 0 otherwise
 */
int cpu_features_simd_available(CPUFeatureContext* context);

/**
 * @brief Get cache line size
 * 
 * @param context CPU feature context
 * @return uint32_t Cache line size in bytes
 */
uint32_t cpu_features_get_cache_line_size(CPUFeatureContext* context);

/**
 * @brief Get L1 data cache size
 * 
 * @param context CPU feature context
 * @return uint32_t L1 data cache size in bytes
 */
uint32_t cpu_features_get_l1d_cache_size(CPUFeatureContext* context);

/**
 * @brief Get number of CPU cores
 * 
 * @param context CPU feature context
 * @return uint32_t Number of CPU cores
 */
uint32_t cpu_features_get_core_count(CPUFeatureContext* context);

/**
 * @brief Get CPU architecture
 * 
 * @param context CPU feature context
 * @return CPUArchitecture Detected CPU architecture
 */
CPUArchitecture cpu_features_get_architecture(CPUFeatureContext* context);

/**
 * @brief Get CPU vendor string
 * 
 * @param context CPU feature context
 * @return const char* CPU vendor string
 */
const char* cpu_features_get_vendor_string(CPUFeatureContext* context);

/**
 * @brief Get CPU brand string
 * 
 * @param context CPU feature context
 * @return const char* CPU brand string
 */
const char* cpu_features_get_brand_string(CPUFeatureContext* context);

/**
 * @brief Enable or disable SIMD
 * 
 * @param context CPU feature context
 * @param enable 1 to enable, 0 to disable
 */
void cpu_features_set_simd_enabled(CPUFeatureContext* context, int enable);

/**
 * @brief Set preferred SIMD instruction set
 * 
 * @param context CPU feature context
 * @param simd Preferred SIMD instruction set
 */
void cpu_features_set_preferred_simd(CPUFeatureContext* context, SIMDInstructionSet simd);

/**
 * @brief Get CPU feature summary
 * 
 * @param context CPU feature context
 * @param summary Output buffer for summary string
 * @param max_length Maximum length of summary buffer
 * @return int 1 on success, 0 on failure
 */
int cpu_features_get_summary(CPUFeatureContext* context, char* summary, size_t max_length);

/**
 * @brief Check if CPU supports vectorization
 * 
 * @param context CPU feature context
 * @param vector_width Required vector width (4, 8, 16)
 * @return int 1 if supported, 0 otherwise
 */
int cpu_features_supports_vectorization(CPUFeatureContext* context, int vector_width);

/**
 * @brief Get optimal vector width for the CPU
 * 
 * @param context CPU feature context
 * @return int Optimal vector width (4, 8, 16, or 32)
 */
int cpu_features_get_optimal_vector_width(CPUFeatureContext* context);

/**
 * @brief Check if CPU supports specific optimizations
 * 
 * @param context CPU feature context
 * @param optimization_type Type of optimization to check
 * @return int 1 if supported, 0 otherwise
 */
int cpu_features_supports_optimization(CPUFeatureContext* context, const char* optimization_type);

/**
 * @brief Get CPU performance characteristics
 * 
 * @param context CPU feature context
 * @param cores Number of cores
 * @param threads Number of threads
 * @param max_freq_mhz Maximum frequency in MHz
 * @param cache_line_size Cache line size in bytes
 */
void cpu_features_get_performance_info(CPUFeatureContext* context,
                                       uint32_t* cores,
                                       uint32_t* threads,
                                       uint32_t* max_freq_mhz,
                                       uint32_t* cache_line_size);

#endif // MYCO_CPU_FEATURES_H
