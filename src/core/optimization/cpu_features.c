/**
 * @file cpu_features.c
 * @brief CPU feature detection implementation for platform-specific optimizations
 */

#include "../../include/core/optimization/cpu_features.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Platform-specific includes
#ifdef __x86_64__
#include <cpuid.h>
#include <immintrin.h>
#elif defined(__aarch64__) && defined(__linux__)
#include <sys/auxv.h>
#include <asm/hwcap.h>
#endif

// ============================================================================
// CPU FEATURE DETECTION CONTEXT MANAGEMENT
// ============================================================================

CPUFeatureContext* cpu_features_create_context(void) {
    CPUFeatureContext* context = malloc(sizeof(CPUFeatureContext));
    if (!context) {
        return NULL;
    }
    
    // Initialize context
    memset(&context->features, 0, sizeof(CPUFeatures));
    context->is_initialized = 0;
    context->enable_simd = 1;
    context->enable_optimizations = 1;
    context->preferred_simd = SIMD_NONE;
    
    return context;
}

void cpu_features_free_context(CPUFeatureContext* context) {
    if (context) {
        free(context);
    }
}

// ============================================================================
// PLATFORM-SPECIFIC CPU DETECTION
// ============================================================================

#ifdef __x86_64__
static int detect_x86_64_features(CPUFeatureContext* context) {
    unsigned int eax, ebx, ecx, edx;
    uint64_t features = 0;
    
    // Get CPU vendor string
    __cpuid(0, eax, ebx, ecx, edx);
    memcpy(context->features.vendor_string, &ebx, 4);
    memcpy(context->features.vendor_string + 4, &edx, 4);
    memcpy(context->features.vendor_string + 8, &ecx, 4);
    context->features.vendor_string[12] = '\0';
    
    // Get CPU brand string
    if (eax >= 0x80000004) {
        char brand[48];
        __cpuid(0x80000002, eax, ebx, ecx, edx);
        memcpy(brand, &eax, 4);
        memcpy(brand + 4, &ebx, 4);
        memcpy(brand + 8, &ecx, 4);
        memcpy(brand + 12, &edx, 4);
        
        __cpuid(0x80000003, eax, ebx, ecx, edx);
        memcpy(brand + 16, &eax, 4);
        memcpy(brand + 20, &ebx, 4);
        memcpy(brand + 24, &ecx, 4);
        memcpy(brand + 28, &edx, 4);
        
        __cpuid(0x80000004, eax, ebx, ecx, edx);
        memcpy(brand + 32, &eax, 4);
        memcpy(brand + 36, &ebx, 4);
        memcpy(brand + 40, &ecx, 4);
        memcpy(brand + 44, &edx, 4);
        
        strncpy(context->features.brand_string, brand, 63);
        context->features.brand_string[63] = '\0';
    }
    
    // Detect features using CPUID
    __cpuid(1, eax, ebx, ecx, edx);
    
    // SSE features
    if (edx & (1 << 25)) features |= CPU_FEATURE_SSE;
    if (edx & (1 << 26)) features |= CPU_FEATURE_SSE2;
    if (ecx & (1 << 0)) features |= CPU_FEATURE_SSE3;
    if (ecx & (1 << 9)) features |= CPU_FEATURE_SSSE3;
    if (ecx & (1 << 19)) features |= CPU_FEATURE_SSE4_1;
    if (ecx & (1 << 20)) features |= CPU_FEATURE_SSE4_2;
    if (ecx & (1 << 28)) features |= CPU_FEATURE_AVX;
    if (ecx & (1 << 12)) features |= CPU_FEATURE_FMA;
    if (ecx & (1 << 3)) features |= CPU_FEATURE_MONITOR;
    if (ecx & (1 << 23)) features |= CPU_FEATURE_POPCNT;
    
    // Extended features
    __cpuid(7, eax, ebx, ecx, edx);
    if (ebx & (1 << 5)) features |= CPU_FEATURE_AVX2;
    if (ebx & (1 << 16)) features |= CPU_FEATURE_AVX512F;
    if (ebx & (1 << 30)) features |= CPU_FEATURE_AVX512BW;
    if (ebx & (1 << 17)) features |= CPU_FEATURE_AVX512DQ;
    if (ebx & (1 << 31)) features |= CPU_FEATURE_AVX512VL;
    if (ebx & (1 << 3)) features |= CPU_FEATURE_BMI1;
    if (ebx & (1 << 8)) features |= CPU_FEATURE_BMI2;
    if (ebx & (1 << 5)) features |= CPU_FEATURE_LZCNT;
    if (ebx & (1 << 23)) features |= CPU_FEATURE_TZCNT;
    
    context->features.architecture = CPU_ARCH_X86_64;
    context->features.features = features;
    
    // Set cache information (simplified)
    context->features.cache_line_size = 64; // Most modern x86_64 CPUs
    context->features.l1d_cache_size = 32768; // 32KB typical
    context->features.l1i_cache_size = 32768; // 32KB typical
    context->features.l2_cache_size = 262144; // 256KB typical
    context->features.l3_cache_size = 8388608; // 8MB typical
    
    return 1;
}
#elif defined(__aarch64__)
static int detect_arm64_features(CPUFeatureContext* context) {
    uint64_t features = 0;
    
#ifdef __linux__
    unsigned long hwcap = getauxval(AT_HWCAP);
    
    // Detect ARM64 features
    if (hwcap & HWCAP_NEON) features |= CPU_FEATURE_NEON;
    if (hwcap & HWCAP_CRC32) features |= CPU_FEATURE_CRC32;
    if (hwcap & HWCAP_AES) features |= CPU_FEATURE_AES;
    if (hwcap & HWCAP_SHA1) features |= CPU_FEATURE_SHA1;
    if (hwcap & HWCAP_SHA2) features |= CPU_FEATURE_SHA2;
    if (hwcap & HWCAP_ATOMICS) features |= CPU_FEATURE_ATOMICS;
    if (hwcap & HWCAP_FP) features |= CPU_FEATURE_FP16;
    
    // Check for additional features
    unsigned long hwcap2 = getauxval(AT_HWCAP2);
    if (hwcap2 & HWCAP2_SVE) features |= CPU_FEATURE_SVE;
    if (hwcap2 & HWCAP2_DOTPROD) features |= CPU_FEATURE_DOTPROD;
#else
    // macOS and other platforms - assume basic ARM64 features
    features |= CPU_FEATURE_NEON; // Most ARM64 CPUs have NEON
    features |= CPU_FEATURE_ATOMICS; // Most ARM64 CPUs have atomics
#endif
    
    context->features.architecture = CPU_ARCH_ARM64;
    context->features.features = features;
    
    // Set cache information (simplified)
    context->features.cache_line_size = 64; // Most ARM64 CPUs
    context->features.l1d_cache_size = 32768; // 32KB typical
    context->features.l1i_cache_size = 32768; // 32KB typical
    context->features.l2_cache_size = 262144; // 256KB typical
    context->features.l3_cache_size = 8388608; // 8MB typical
    
    // Set vendor and brand strings
    strcpy(context->features.vendor_string, "ARM");
    strcpy(context->features.brand_string, "ARM64 Processor");
    
    return 1;
}
#else
static int detect_generic_features(CPUFeatureContext* context) {
    // Generic fallback for unsupported architectures
    context->features.architecture = CPU_ARCH_UNKNOWN;
    context->features.features = 0;
    context->features.cache_line_size = 64;
    context->features.l1d_cache_size = 32768;
    context->features.l1i_cache_size = 32768;
    context->features.l2_cache_size = 262144;
    context->features.l3_cache_size = 8388608;
    
    strcpy(context->features.vendor_string, "Unknown");
    strcpy(context->features.brand_string, "Unknown Processor");
    
    return 1;
}
#endif

// ============================================================================
// CPU FEATURE DETECTION
// ============================================================================

int cpu_features_detect(CPUFeatureContext* context) {
    if (!context) {
        return 0;
    }
    
    int result = 0;
    
#ifdef __x86_64__
    result = detect_x86_64_features(context);
#elif defined(__aarch64__)
    result = detect_arm64_features(context);
#else
    result = detect_generic_features(context);
#endif
    
    if (result) {
        context->is_initialized = 1;
        
        // Set preferred SIMD based on detected features
        context->preferred_simd = cpu_features_get_best_simd(context);
    }
    
    return result;
}

// ============================================================================
// FEATURE QUERY FUNCTIONS
// ============================================================================

int cpu_features_has_feature(CPUFeatureContext* context, uint64_t feature) {
    if (!context || !context->is_initialized) {
        return 0;
    }
    
    return (context->features.features & feature) != 0;
}

SIMDInstructionSet cpu_features_get_best_simd(CPUFeatureContext* context) {
    if (!context || !context->is_initialized) {
        return SIMD_NONE;
    }
    
    switch (context->features.architecture) {
        case CPU_ARCH_X86_64:
            if (cpu_features_has_feature(context, CPU_FEATURE_AVX512F)) {
                return SIMD_AVX512;
            } else if (cpu_features_has_feature(context, CPU_FEATURE_AVX2)) {
                return SIMD_AVX2;
            } else if (cpu_features_has_feature(context, CPU_FEATURE_AVX)) {
                return SIMD_AVX;
            } else if (cpu_features_has_feature(context, CPU_FEATURE_SSE4_2)) {
                return SIMD_SSE4_2;
            } else if (cpu_features_has_feature(context, CPU_FEATURE_SSE2)) {
                return SIMD_SSE2;
            }
            break;
            
        case CPU_ARCH_ARM64:
            if (cpu_features_has_feature(context, CPU_FEATURE_NEON)) {
                return SIMD_NEON;
            }
            break;
            
        default:
            break;
    }
    
    return SIMD_NONE;
}

int cpu_features_simd_available(CPUFeatureContext* context) {
    if (!context || !context->is_initialized || !context->enable_simd) {
        return 0;
    }
    
    return cpu_features_get_best_simd(context) != SIMD_NONE;
}

uint32_t cpu_features_get_cache_line_size(CPUFeatureContext* context) {
    if (!context || !context->is_initialized) {
        return 64; // Default fallback
    }
    
    return context->features.cache_line_size;
}

uint32_t cpu_features_get_l1d_cache_size(CPUFeatureContext* context) {
    if (!context || !context->is_initialized) {
        return 32768; // Default fallback
    }
    
    return context->features.l1d_cache_size;
}

uint32_t cpu_features_get_core_count(CPUFeatureContext* context) {
    if (!context || !context->is_initialized) {
        return 1; // Default fallback
    }
    
    return context->features.cores;
}

CPUArchitecture cpu_features_get_architecture(CPUFeatureContext* context) {
    if (!context || !context->is_initialized) {
        return CPU_ARCH_UNKNOWN;
    }
    
    return context->features.architecture;
}

const char* cpu_features_get_vendor_string(CPUFeatureContext* context) {
    if (!context || !context->is_initialized) {
        return "Unknown";
    }
    
    return context->features.vendor_string;
}

const char* cpu_features_get_brand_string(CPUFeatureContext* context) {
    if (!context || !context->is_initialized) {
        return "Unknown Processor";
    }
    
    return context->features.brand_string;
}

// ============================================================================
// CONFIGURATION FUNCTIONS
// ============================================================================

void cpu_features_set_simd_enabled(CPUFeatureContext* context, int enable) {
    if (context) {
        context->enable_simd = enable;
    }
}

void cpu_features_set_preferred_simd(CPUFeatureContext* context, SIMDInstructionSet simd) {
    if (context) {
        context->preferred_simd = simd;
    }
}

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

int cpu_features_get_summary(CPUFeatureContext* context, char* summary, size_t max_length) {
    if (!context || !context->is_initialized || !summary || max_length == 0) {
        return 0;
    }
    
    const char* arch_name = "Unknown";
    switch (context->features.architecture) {
        case CPU_ARCH_X86_64: arch_name = "x86_64"; break;
        case CPU_ARCH_ARM64: arch_name = "ARM64"; break;
        case CPU_ARCH_ARM: arch_name = "ARM"; break;
        case CPU_ARCH_RISCV64: arch_name = "RISC-V64"; break;
        case CPU_ARCH_PPC64: arch_name = "PowerPC64"; break;
        default: break;
    }
    
    SIMDInstructionSet simd = cpu_features_get_best_simd(context);
    const char* simd_name = "None";
    switch (simd) {
        case SIMD_SSE2: simd_name = "SSE2"; break;
        case SIMD_SSE4_2: simd_name = "SSE4.2"; break;
        case SIMD_AVX: simd_name = "AVX"; break;
        case SIMD_AVX2: simd_name = "AVX2"; break;
        case SIMD_AVX512: simd_name = "AVX-512"; break;
        case SIMD_NEON: simd_name = "NEON"; break;
        default: break;
    }
    
    snprintf(summary, max_length,
             "Architecture: %s, SIMD: %s, Cores: %u, Cache: %uKB L1D",
             arch_name, simd_name, context->features.cores,
             context->features.l1d_cache_size / 1024);
    
    return 1;
}

int cpu_features_supports_vectorization(CPUFeatureContext* context, int vector_width) {
    if (!context || !context->is_initialized || !context->enable_simd) {
        return 0;
    }
    
    SIMDInstructionSet simd = cpu_features_get_best_simd(context);
    
    switch (simd) {
        case SIMD_SSE2:
        case SIMD_SSE4_2:
            return vector_width <= 4;
        case SIMD_AVX:
        case SIMD_AVX2:
            return vector_width <= 8;
        case SIMD_AVX512:
            return vector_width <= 16;
        case SIMD_NEON:
            return vector_width <= 4;
        default:
            return 0;
    }
}

int cpu_features_get_optimal_vector_width(CPUFeatureContext* context) {
    if (!context || !context->is_initialized || !context->enable_simd) {
        return 4; // Default fallback
    }
    
    SIMDInstructionSet simd = cpu_features_get_best_simd(context);
    
    switch (simd) {
        case SIMD_SSE2:
        case SIMD_SSE4_2:
        case SIMD_NEON:
            return 4;
        case SIMD_AVX:
        case SIMD_AVX2:
            return 8;
        case SIMD_AVX512:
            return 16;
        default:
            return 4;
    }
}

int cpu_features_supports_optimization(CPUFeatureContext* context, const char* optimization_type) {
    if (!context || !context->is_initialized || !optimization_type) {
        return 0;
    }
    
    if (strcmp(optimization_type, "vectorization") == 0) {
        return cpu_features_simd_available(context);
    } else if (strcmp(optimization_type, "fma") == 0) {
        return cpu_features_has_feature(context, CPU_FEATURE_FMA);
    } else if (strcmp(optimization_type, "popcnt") == 0) {
        return cpu_features_has_feature(context, CPU_FEATURE_POPCNT);
    } else if (strcmp(optimization_type, "bmi") == 0) {
        return cpu_features_has_feature(context, CPU_FEATURE_BMI1) ||
               cpu_features_has_feature(context, CPU_FEATURE_BMI2);
    }
    
    return 0;
}

void cpu_features_get_performance_info(CPUFeatureContext* context,
                                       uint32_t* cores,
                                       uint32_t* threads,
                                       uint32_t* max_freq_mhz,
                                       uint32_t* cache_line_size) {
    if (!context || !context->is_initialized) {
        if (cores) *cores = 1;
        if (threads) *threads = 1;
        if (max_freq_mhz) *max_freq_mhz = 2000;
        if (cache_line_size) *cache_line_size = 64;
        return;
    }
    
    if (cores) *cores = context->features.cores;
    if (threads) *threads = context->features.threads;
    if (max_freq_mhz) *max_freq_mhz = context->features.max_frequency_mhz;
    if (cache_line_size) *cache_line_size = context->features.cache_line_size;
}
