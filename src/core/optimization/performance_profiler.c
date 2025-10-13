/**
 * @file performance_profiler.c
 * @brief Real-time performance monitoring implementation
 */

#include "../../include/core/optimization/performance_profiler.h"
#include "../../include/core/ast.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

// Forward declarations
static BottleneckAnalysis* add_bottleneck(PerformanceProfiler* profiler);
static OptimizationSuggestion* add_suggestion(PerformanceProfiler* profiler);

// ============================================================================
// PROFILER CONTEXT MANAGEMENT
// ============================================================================

PerformanceProfiler* performance_profiler_create(void) {
    PerformanceProfiler* profiler = malloc(sizeof(PerformanceProfiler));
    if (!profiler) {
        return NULL;
    }
    
    // Initialize tier data
    profiler->tier_data = malloc(TIER_COUNT * sizeof(TierEffectiveness));
    if (!profiler->tier_data) {
        free(profiler);
        return NULL;
    }
    
    // Initialize bottlenecks and suggestions arrays
    profiler->bottlenecks = NULL;
    profiler->bottleneck_count = 0;
    profiler->suggestions = NULL;
    profiler->suggestion_count = 0;
    
    // Initialize other fields
    profiler->total_samples = 0;
    profiler->profiling_start_time = 0;
    profiler->is_profiling = 0;
    profiler->overall_efficiency = 0.0;
    
    // Initialize tier data
    for (int i = 0; i < TIER_COUNT; i++) {
        profiler->tier_data[i].tier = (PerformanceTier)i;
        memset(&profiler->tier_data[i].metrics, 0, sizeof(PerformanceMetrics));
        profiler->tier_data[i].speedup_factor = 1.0;
        profiler->tier_data[i].efficiency_score = 0.0;
        profiler->tier_data[i].is_optimal = 0;
        profiler->tier_data[i].sample_count = 0;
    }
    
    return profiler;
}

void performance_profiler_free(PerformanceProfiler* profiler) {
    if (!profiler) {
        return;
    }
    
    if (profiler->tier_data) {
        free(profiler->tier_data);
    }
    
    if (profiler->bottlenecks) {
        for (size_t i = 0; i < profiler->bottleneck_count; i++) {
            if (profiler->bottlenecks[i].bottleneck_type) {
                free(profiler->bottlenecks[i].bottleneck_type);
            }
            if (profiler->bottlenecks[i].description) {
                free(profiler->bottlenecks[i].description);
            }
            if (profiler->bottlenecks[i].suggestion) {
                free(profiler->bottlenecks[i].suggestion);
            }
        }
        free(profiler->bottlenecks);
    }
    
    if (profiler->suggestions) {
        for (size_t i = 0; i < profiler->suggestion_count; i++) {
            if (profiler->suggestions[i].suggestion_type) {
                free(profiler->suggestions[i].suggestion_type);
            }
            if (profiler->suggestions[i].description) {
                free(profiler->suggestions[i].description);
            }
            if (profiler->suggestions[i].implementation_hint) {
                free(profiler->suggestions[i].implementation_hint);
            }
        }
        free(profiler->suggestions);
    }
    
    free(profiler);
}

// ============================================================================
// PROFILING CONTROL
// ============================================================================

int performance_profiler_start(PerformanceProfiler* profiler) {
    if (!profiler) {
        return 0;
    }
    
    profiler->is_profiling = 1;
    profiler->profiling_start_time = performance_profiler_get_time_ns();
    
    return 1;
}

int performance_profiler_stop(PerformanceProfiler* profiler) {
    if (!profiler) {
        return 0;
    }
    
    profiler->is_profiling = 0;
    
    // Analyze collected data
    performance_profiler_analyze_effectiveness(profiler);
    performance_profiler_detect_bottlenecks(profiler);
    performance_profiler_generate_suggestions(profiler);
    
    return 1;
}

// ============================================================================
// METRICS RECORDING
// ============================================================================

int performance_profiler_record_tier_execution(PerformanceProfiler* profiler,
                                               PerformanceTier tier,
                                               PerformanceMetrics metrics) {
    if (!profiler || tier >= TIER_COUNT) {
        return 0;
    }
    
    TierEffectiveness* tier_data = &profiler->tier_data[tier];
    
    // Update metrics (simple averaging)
    uint64_t new_sample_count = tier_data->sample_count + 1;
    
    tier_data->metrics.execution_time_ns = 
        (tier_data->metrics.execution_time_ns * tier_data->sample_count + 
         metrics.execution_time_ns) / new_sample_count;
    
    tier_data->metrics.instruction_count = 
        (tier_data->metrics.instruction_count * tier_data->sample_count + 
         metrics.instruction_count) / new_sample_count;
    
    tier_data->metrics.memory_allocations = 
        (tier_data->metrics.memory_allocations * tier_data->sample_count + 
         metrics.memory_allocations) / new_sample_count;
    
    tier_data->metrics.memory_bytes = 
        (tier_data->metrics.memory_bytes * tier_data->sample_count + 
         metrics.memory_bytes) / new_sample_count;
    
    tier_data->metrics.cache_hits = 
        (tier_data->metrics.cache_hits * tier_data->sample_count + 
         metrics.cache_hits) / new_sample_count;
    
    tier_data->metrics.cache_misses = 
        (tier_data->metrics.cache_misses * tier_data->sample_count + 
         metrics.cache_misses) / new_sample_count;
    
    tier_data->metrics.branch_predictions = 
        (tier_data->metrics.branch_predictions * tier_data->sample_count + 
         metrics.branch_predictions) / new_sample_count;
    
    tier_data->metrics.branch_mispredictions = 
        (tier_data->metrics.branch_mispredictions * tier_data->sample_count + 
         metrics.branch_mispredictions) / new_sample_count;
    
    tier_data->sample_count = new_sample_count;
    profiler->total_samples++;
    
    return 1;
}

// ============================================================================
// ANALYSIS FUNCTIONS
// ============================================================================

int performance_profiler_analyze_effectiveness(PerformanceProfiler* profiler) {
    if (!profiler) {
        return 0;
    }
    
    // Calculate speedup factors relative to AST tier
    uint64_t ast_time = profiler->tier_data[TIER_AST].metrics.execution_time_ns;
    if (ast_time == 0) {
        return 0; // No baseline data
    }
    
    for (int i = 0; i < TIER_COUNT; i++) {
        TierEffectiveness* tier_data = &profiler->tier_data[i];
        
        if (tier_data->metrics.execution_time_ns > 0) {
            tier_data->speedup_factor = performance_profiler_calculate_speedup(
                ast_time, tier_data->metrics.execution_time_ns);
        } else {
            tier_data->speedup_factor = 1.0;
        }
        
        tier_data->efficiency_score = performance_profiler_calculate_efficiency(
            tier_data->metrics);
    }
    
    // Find optimal tier (highest efficiency score)
    double max_efficiency = 0.0;
    int optimal_tier = TIER_AST;
    
    for (int i = 0; i < TIER_COUNT; i++) {
        if (profiler->tier_data[i].efficiency_score > max_efficiency) {
            max_efficiency = profiler->tier_data[i].efficiency_score;
            optimal_tier = i;
        }
    }
    
    // Mark optimal tier
    for (int i = 0; i < TIER_COUNT; i++) {
        profiler->tier_data[i].is_optimal = (i == optimal_tier);
    }
    
    // Calculate overall efficiency
    profiler->overall_efficiency = max_efficiency;
    
    return 1;
}

int performance_profiler_detect_bottlenecks(PerformanceProfiler* profiler) {
    if (!profiler) {
        return 0;
    }
    
    // Clear existing bottlenecks
    if (profiler->bottlenecks) {
        for (size_t i = 0; i < profiler->bottleneck_count; i++) {
            if (profiler->bottlenecks[i].bottleneck_type) {
                free(profiler->bottlenecks[i].bottleneck_type);
            }
            if (profiler->bottlenecks[i].description) {
                free(profiler->bottlenecks[i].description);
            }
            if (profiler->bottlenecks[i].suggestion) {
                free(profiler->bottlenecks[i].suggestion);
            }
        }
        free(profiler->bottlenecks);
    }
    
    profiler->bottlenecks = NULL;
    profiler->bottleneck_count = 0;
    
    // Detect common bottlenecks
    for (int i = 0; i < TIER_COUNT; i++) {
        TierEffectiveness* tier_data = &profiler->tier_data[i];
        
        // Check for memory bottlenecks
        if (tier_data->metrics.memory_allocations > 1000) {
            BottleneckAnalysis* bottleneck = add_bottleneck(profiler);
            if (bottleneck) {
                bottleneck->bottleneck_type = strdup("memory_allocation");
                bottleneck->description = strdup("Excessive memory allocations");
                bottleneck->impact_score = 0.7;
                bottleneck->suggestion = strdup("Consider using arena allocation or object pooling");
                bottleneck->affected_tier = (PerformanceTier)i;
            }
        }
        
        // Check for cache misses
        if (tier_data->metrics.cache_misses > tier_data->metrics.cache_hits) {
            BottleneckAnalysis* bottleneck = add_bottleneck(profiler);
            if (bottleneck) {
                bottleneck->bottleneck_type = strdup("cache_miss");
                bottleneck->description = strdup("High cache miss ratio");
                bottleneck->impact_score = 0.6;
                bottleneck->suggestion = strdup("Improve data locality and access patterns");
                bottleneck->affected_tier = (PerformanceTier)i;
            }
        }
        
        // Check for branch mispredictions
        if (tier_data->metrics.branch_mispredictions > tier_data->metrics.branch_predictions / 4) {
            BottleneckAnalysis* bottleneck = add_bottleneck(profiler);
            if (bottleneck) {
                bottleneck->bottleneck_type = strdup("branch_misprediction");
                bottleneck->description = strdup("High branch misprediction rate");
                bottleneck->impact_score = 0.5;
                bottleneck->suggestion = strdup("Optimize branch patterns and use branch hints");
                bottleneck->affected_tier = (PerformanceTier)i;
            }
        }
    }
    
    return 1;
}

int performance_profiler_generate_suggestions(PerformanceProfiler* profiler) {
    if (!profiler) {
        return 0;
    }
    
    // Clear existing suggestions
    if (profiler->suggestions) {
        for (size_t i = 0; i < profiler->suggestion_count; i++) {
            if (profiler->suggestions[i].suggestion_type) {
                free(profiler->suggestions[i].suggestion_type);
            }
            if (profiler->suggestions[i].description) {
                free(profiler->suggestions[i].description);
            }
            if (profiler->suggestions[i].implementation_hint) {
                free(profiler->suggestions[i].implementation_hint);
            }
        }
        free(profiler->suggestions);
    }
    
    profiler->suggestions = NULL;
    profiler->suggestion_count = 0;
    
    // Generate suggestions based on analysis
    for (int i = 0; i < TIER_COUNT; i++) {
        TierEffectiveness* tier_data = &profiler->tier_data[i];
        
        // Suggest tier upgrades if efficiency is low
        if (tier_data->efficiency_score < 0.5 && i < TIER_COUNT - 1) {
            OptimizationSuggestion* suggestion = add_suggestion(profiler);
            if (suggestion) {
                suggestion->suggestion_type = strdup("tier_upgrade");
                suggestion->description = strdup("Consider upgrading to next optimization tier");
                suggestion->expected_gain = 0.3;
                suggestion->difficulty = 2;
                suggestion->implementation_hint = strdup("Enable next tier in adaptive executor");
            }
        }
        
        // Suggest vectorization for arithmetic-heavy code
        if (i < TIER_VECTORIZED && tier_data->metrics.instruction_count > 10000) {
            OptimizationSuggestion* suggestion = add_suggestion(profiler);
            if (suggestion) {
                suggestion->suggestion_type = strdup("vectorization");
                suggestion->description = strdup("Enable vectorization for arithmetic operations");
                suggestion->expected_gain = 0.5;
                suggestion->difficulty = 3;
                suggestion->implementation_hint = strdup("Use vectorizer for loop optimization");
            }
        }
    }
    
    return 1;
}

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

static BottleneckAnalysis* add_bottleneck(PerformanceProfiler* profiler) {
    if (!profiler) {
        return NULL;
    }
    
    profiler->bottlenecks = realloc(profiler->bottlenecks, 
                                   (profiler->bottleneck_count + 1) * sizeof(BottleneckAnalysis));
    if (!profiler->bottlenecks) {
        return NULL;
    }
    
    BottleneckAnalysis* bottleneck = &profiler->bottlenecks[profiler->bottleneck_count];
    profiler->bottleneck_count++;
    
    bottleneck->bottleneck_type = NULL;
    bottleneck->description = NULL;
    bottleneck->impact_score = 0.0;
    bottleneck->suggestion = NULL;
    bottleneck->affected_tier = TIER_AST;
    
    return bottleneck;
}

static OptimizationSuggestion* add_suggestion(PerformanceProfiler* profiler) {
    if (!profiler) {
        return NULL;
    }
    
    profiler->suggestions = realloc(profiler->suggestions, 
                                   (profiler->suggestion_count + 1) * sizeof(OptimizationSuggestion));
    if (!profiler->suggestions) {
        return NULL;
    }
    
    OptimizationSuggestion* suggestion = &profiler->suggestions[profiler->suggestion_count];
    profiler->suggestion_count++;
    
    suggestion->suggestion_type = NULL;
    suggestion->description = NULL;
    suggestion->expected_gain = 0.0;
    suggestion->difficulty = 1;
    suggestion->implementation_hint = NULL;
    
    return suggestion;
}

// ============================================================================
// QUERY FUNCTIONS
// ============================================================================

PerformanceTier performance_profiler_get_optimal_tier(PerformanceProfiler* profiler,
                                                      ASTNode* function_node) {
    if (!profiler) {
        return TIER_AST;
    }
    
    // Find the tier with highest efficiency score
    double max_efficiency = 0.0;
    PerformanceTier optimal_tier = TIER_AST;
    
    for (int i = 0; i < TIER_COUNT; i++) {
        if (profiler->tier_data[i].efficiency_score > max_efficiency) {
            max_efficiency = profiler->tier_data[i].efficiency_score;
            optimal_tier = (PerformanceTier)i;
        }
    }
    
    return optimal_tier;
}

TierEffectiveness* performance_profiler_get_tier_data(PerformanceProfiler* profiler,
                                                      PerformanceTier tier) {
    if (!profiler || tier >= TIER_COUNT) {
        return NULL;
    }
    
    return &profiler->tier_data[tier];
}

BottleneckAnalysis* performance_profiler_get_bottleneck(PerformanceProfiler* profiler,
                                                        size_t bottleneck_index) {
    if (!profiler || bottleneck_index >= profiler->bottleneck_count) {
        return NULL;
    }
    
    return &profiler->bottlenecks[bottleneck_index];
}

OptimizationSuggestion* performance_profiler_get_suggestion(PerformanceProfiler* profiler,
                                                            size_t suggestion_index) {
    if (!profiler || suggestion_index >= profiler->suggestion_count) {
        return NULL;
    }
    
    return &profiler->suggestions[suggestion_index];
}

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

void performance_profiler_get_stats(PerformanceProfiler* profiler,
                                    uint64_t* total_samples,
                                    double* overall_efficiency,
                                    size_t* bottleneck_count,
                                    size_t* suggestion_count) {
    if (!profiler || !total_samples || !overall_efficiency || !bottleneck_count || !suggestion_count) {
        return;
    }
    
    *total_samples = profiler->total_samples;
    *overall_efficiency = profiler->overall_efficiency;
    *bottleneck_count = profiler->bottleneck_count;
    *suggestion_count = profiler->suggestion_count;
}

void performance_profiler_reset(PerformanceProfiler* profiler) {
    if (!profiler) {
        return;
    }
    
    // Reset tier data
    for (int i = 0; i < TIER_COUNT; i++) {
        memset(&profiler->tier_data[i].metrics, 0, sizeof(PerformanceMetrics));
        profiler->tier_data[i].speedup_factor = 1.0;
        profiler->tier_data[i].efficiency_score = 0.0;
        profiler->tier_data[i].is_optimal = 0;
        profiler->tier_data[i].sample_count = 0;
    }
    
    // Clear bottlenecks and suggestions
    if (profiler->bottlenecks) {
        for (size_t i = 0; i < profiler->bottleneck_count; i++) {
            if (profiler->bottlenecks[i].bottleneck_type) {
                free(profiler->bottlenecks[i].bottleneck_type);
            }
            if (profiler->bottlenecks[i].description) {
                free(profiler->bottlenecks[i].description);
            }
            if (profiler->bottlenecks[i].suggestion) {
                free(profiler->bottlenecks[i].suggestion);
            }
        }
        free(profiler->bottlenecks);
        profiler->bottlenecks = NULL;
        profiler->bottleneck_count = 0;
    }
    
    if (profiler->suggestions) {
        for (size_t i = 0; i < profiler->suggestion_count; i++) {
            if (profiler->suggestions[i].suggestion_type) {
                free(profiler->suggestions[i].suggestion_type);
            }
            if (profiler->suggestions[i].description) {
                free(profiler->suggestions[i].description);
            }
            if (profiler->suggestions[i].implementation_hint) {
                free(profiler->suggestions[i].implementation_hint);
            }
        }
        free(profiler->suggestions);
        profiler->suggestions = NULL;
        profiler->suggestion_count = 0;
    }
    
    profiler->total_samples = 0;
    profiler->overall_efficiency = 0.0;
}

int performance_profiler_export_report(PerformanceProfiler* profiler,
                                       const char* filename) {
    if (!profiler || !filename) {
        return 0;
    }
    
    FILE* file = fopen(filename, "w");
    if (!file) {
        return 0;
    }
    
    fprintf(file, "Myco Performance Profiler Report\n");
    fprintf(file, "================================\n\n");
    
    fprintf(file, "Overall Statistics:\n");
    fprintf(file, "  Total Samples: %llu\n", (unsigned long long)profiler->total_samples);
    fprintf(file, "  Overall Efficiency: %.2f\n", profiler->overall_efficiency);
    fprintf(file, "  Bottlenecks Detected: %zu\n", profiler->bottleneck_count);
    fprintf(file, "  Suggestions Generated: %zu\n\n", profiler->suggestion_count);
    
    fprintf(file, "Tier Effectiveness:\n");
    for (int i = 0; i < TIER_COUNT; i++) {
        TierEffectiveness* tier_data = &profiler->tier_data[i];
        fprintf(file, "  Tier %d: Speedup=%.2fx, Efficiency=%.2f, Samples=%llu\n",
                i, tier_data->speedup_factor, tier_data->efficiency_score,
                (unsigned long long)tier_data->sample_count);
    }
    
    fclose(file);
    return 1;
}

uint64_t performance_profiler_get_time_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + (uint64_t)ts.tv_nsec;
}

double performance_profiler_calculate_speedup(uint64_t baseline_time,
                                              uint64_t optimized_time) {
    if (optimized_time == 0) {
        return 1.0;
    }
    return (double)baseline_time / (double)optimized_time;
}

double performance_profiler_calculate_efficiency(PerformanceMetrics metrics) {
    // Simple efficiency calculation based on execution time and resource usage
    double time_efficiency = 1.0;
    double memory_efficiency = 1.0;
    double cache_efficiency = 1.0;
    
    // Time efficiency (lower is better)
    if (metrics.execution_time_ns > 0) {
        time_efficiency = 1.0 / (1.0 + metrics.execution_time_ns / 1000000.0); // Normalize to ms
    }
    
    // Memory efficiency (lower allocation is better)
    if (metrics.memory_allocations > 0) {
        memory_efficiency = 1.0 / (1.0 + metrics.memory_allocations / 1000.0);
    }
    
    // Cache efficiency (higher hit rate is better)
    if (metrics.cache_hits + metrics.cache_misses > 0) {
        cache_efficiency = (double)metrics.cache_hits / (metrics.cache_hits + metrics.cache_misses);
    }
    
    // Weighted average
    return (time_efficiency * 0.5 + memory_efficiency * 0.3 + cache_efficiency * 0.2);
}
