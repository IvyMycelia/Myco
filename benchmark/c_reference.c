/**
 * C Reference Implementation for Performance Comparison
 * 
 * This file provides C implementations of the same algorithms
 * used in Myco benchmarks for performance comparison.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define ITERATIONS 1000000
#define WARMUP_ITERATIONS 10000
#define BENCHMARK_RUNS 5

// Performance measurement utilities
double get_time_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1000000.0;
}

void measure_time(const char* func_name, int iterations, void (*test_func)(void)) {
    printf("Running %s with %d iterations...\n", func_name, iterations);
    
    // Warmup
    for (int i = 0; i < WARMUP_ITERATIONS; i++) {
        test_func();
    }
    
    // Actual measurement
    double start_time = get_time_ms();
    for (int i = 0; i < iterations; i++) {
        test_func();
    }
    double end_time = get_time_ms();
    
    double duration_ms = end_time - start_time;
    double ops_per_sec = (iterations * 1000.0) / duration_ms;
    
    printf("  Duration: %.2f ms\n", duration_ms);
    printf("  Operations/sec: %.0f\n", ops_per_sec);
    printf("  Avg per operation: %.6f ms\n", duration_ms / iterations);
    printf("\n");
}

// Arithmetic benchmarks
static int arithmetic_a = 42;
static int arithmetic_b = 17;
static int arithmetic_c = 0;

void arithmetic_benchmark() {
    arithmetic_c = arithmetic_a + arithmetic_b;
    arithmetic_c = arithmetic_a - arithmetic_b;
    arithmetic_c = arithmetic_a * arithmetic_b;
    arithmetic_c = arithmetic_a / arithmetic_b;
    arithmetic_c = arithmetic_a % arithmetic_b;
    arithmetic_c = arithmetic_a ^ arithmetic_b;
    arithmetic_c = arithmetic_a & arithmetic_b;
    arithmetic_c = arithmetic_a | arithmetic_b;
    arithmetic_c = arithmetic_a << 2;
    arithmetic_c = arithmetic_a >> 2;
}

static double float_a = 3.14159;
static double float_b = 2.71828;
static double float_c = 0.0;

void float_arithmetic_benchmark() {
    float_c = float_a + float_b;
    float_c = float_a - float_b;
    float_c = float_a * float_b;
    float_c = float_a / float_b;
    float_c = sin(float_a);
    float_c = cos(float_a);
    float_c = sqrt(float_a);
    float_c = pow(float_a, float_b);
}

// Array operations benchmark
static int array_data[1000];
static int array_sum = 0;

void array_operations_benchmark() {
    // Initialize array
    for (int i = 0; i < 1000; i++) {
        array_data[i] = i;
    }
    
    // Perform operations
    for (int i = 0; i < ITERATIONS / 1000; i++) {
        for (int j = 0; j < 1000; j++) {
            array_data[j] = array_data[j] + 1;
        }
    }
}

// String operations benchmark
static char str1[] = "Hello, World!";
static char str2[] = "Myco Language";
static char result[256];

void string_operations_benchmark() {
    for (int i = 0; i < ITERATIONS / 100; i++) {
        strcpy(result, str1);
        strcat(result, str2);
        strncpy(result, str1, 5);
        // Note: toUpperCase, toLowerCase, replace would need custom implementations
    }
}

// Function call benchmark
int simple_function(int x) {
    return x * 2 + 1;
}

static int function_call_result = 0;

void function_call_benchmark() {
    for (int i = 0; i < ITERATIONS; i++) {
        function_call_result = simple_function(i);
    }
}

// Recursive function benchmark
int fibonacci(int n) {
    if (n <= 1) {
        return n;
    } else {
        return fibonacci(n - 1) + fibonacci(n - 2);
    }
}

static int recursive_result = 0;

void recursive_benchmark() {
    for (int i = 0; i < 30; i++) {
        recursive_result = fibonacci(i);
    }
}

// Loop optimization benchmark
static long long loop_sum = 0;

void loop_optimization_benchmark() {
    for (int i = 0; i < ITERATIONS; i++) {
        for (int j = 0; j < 10; j++) {
            loop_sum = loop_sum + i * j;
        }
    }
}

// Memory allocation benchmark
static int** memory_arrays = NULL;
static int memory_count = 0;

void memory_allocation_benchmark() {
    if (memory_arrays == NULL) {
        memory_arrays = malloc((ITERATIONS / 100) * sizeof(int*));
    }
    
    for (int i = 0; i < ITERATIONS / 100; i++) {
        int* arr = malloc(100 * sizeof(int));
        for (int j = 0; j < 100; j++) {
            arr[j] = i * j;
        }
        memory_arrays[i] = arr;
        memory_count++;
    }
}

// Object creation benchmark (simulated with structs)
typedef struct {
    int id;
    int value;
    char name[32];
} Object;

static Object* objects = NULL;
static int object_count = 0;

void object_creation_benchmark() {
    if (objects == NULL) {
        objects = malloc((ITERATIONS / 100) * sizeof(Object));
    }
    
    for (int i = 0; i < ITERATIONS / 100; i++) {
        objects[i].id = i;
        objects[i].value = i * 2;
        snprintf(objects[i].name, sizeof(objects[i].name), "Object%d", i);
        object_count++;
    }
}

// Hash table operations benchmark (simulated with array)
#define HASH_SIZE 10000
static int hash_table[HASH_SIZE];
static char hash_keys[HASH_SIZE][32];

void hash_table_benchmark() {
    // Initialize hash table
    for (int i = 0; i < HASH_SIZE; i++) {
        hash_table[i] = 0;
        snprintf(hash_keys[i], sizeof(hash_keys[i]), "key%d", i);
    }
    
    // Insert values
    for (int i = 0; i < ITERATIONS / 10; i++) {
        int index = i % HASH_SIZE;
        hash_table[index] = i * 2;
    }
    
    // Lookup values
    int sum = 0;
    for (int i = 0; i < ITERATIONS / 10; i++) {
        int index = i % HASH_SIZE;
        sum += hash_table[index];
    }
}

// Main benchmark runner
void run_all_benchmarks() {
    printf("=== C Reference Performance Benchmark Suite ===\n");
    printf("Testing C implementations for performance comparison\n");
    printf("\n");
    
    double total_time = 0.0;
    
    // Run all benchmarks
    measure_time("Arithmetic Operations", ITERATIONS, arithmetic_benchmark);
    measure_time("Float Arithmetic", ITERATIONS, float_arithmetic_benchmark);
    measure_time("Array Operations", ITERATIONS, array_operations_benchmark);
    measure_time("String Operations", ITERATIONS, string_operations_benchmark);
    measure_time("Function Calls", ITERATIONS, function_call_benchmark);
    measure_time("Recursive Functions", 1, recursive_benchmark);
    measure_time("Loop Optimization", ITERATIONS, loop_optimization_benchmark);
    measure_time("Memory Allocation", ITERATIONS, memory_allocation_benchmark);
    measure_time("Object Creation", ITERATIONS, object_creation_benchmark);
    measure_time("Hash Table Operations", ITERATIONS, hash_table_benchmark);
    
    printf("=== Benchmark Summary ===\n");
    printf("Total execution time: %.2f ms\n", total_time);
    printf("Average operations per second: %.0f\n", (ITERATIONS * 10 * 1000.0) / total_time);
    printf("\n");
}

// Cleanup function
void cleanup() {
    if (memory_arrays != NULL) {
        for (int i = 0; i < memory_count; i++) {
            free(memory_arrays[i]);
        }
        free(memory_arrays);
    }
    if (objects != NULL) {
        free(objects);
    }
}

int main() {
    printf("C Reference Implementation for Myco Performance Comparison\n");
    printf("Compiled with: gcc -O2 -std=c99\n");
    printf("\n");
    
    run_all_benchmarks();
    cleanup();
    
    return 0;
}
