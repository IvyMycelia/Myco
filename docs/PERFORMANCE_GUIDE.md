# Myco Performance Guide

## Overview

Myco's optimization system provides performance improvements through multi-tier execution. This guide explains how to get the best performance from your Myco programs.

## Quick Start

### Enable Optimization

```bash
# Enable full optimization (recommended)
./bin/myco script.myco --jit 1

# Enable only bytecode optimization
./bin/myco script.myco --jit 0

# Disable optimization (AST interpreter only)
./bin/myco script.myco --jit 0
```

### Environment Variables

```bash
# Set optimization level globally
export MYCO_OPTIMIZE=1

# Run your program
./bin/myco script.myco
```

## Performance Features

### 1. Automatic Hot Spot Detection

The system automatically identifies frequently executed code and optimizes it:

```myco
# This function will be JIT-compiled after 100+ executions
func fibonacci(n):
    if n <= 1:
        return n;
    else:
        return fibonacci(n - 1) + fibonacci(n - 2);
    end
end

# Hot spot: This loop will be optimized
let sum = 0;
let i = 0;
while i < 100000:
    sum = sum + i;
    i = i + 1;
end
```

### 2. Type Specialization

The system optimizes code based on the types it observes:

```myco
# This function will be specialized for integer operations
func add_numbers(a, b):
    return a + b;  # Optimized for Int + Int
end

# First call: Int + Int (creates specialized version)
let result1 = add_numbers(5, 3);

# Second call: Float + Float (creates another specialized version)
let result2 = add_numbers(5.5, 3.2);
```

### 3. Bytecode Optimization

Frequently executed code is compiled to optimized bytecode:

```myco
# This code will be compiled to bytecode after first execution
let result = 0;
for i in range(1000):
    result = result + (i * 2 + 1);
end
```

## Performance Best Practices

### 1. Write Hot Paths Efficiently

**Good**: Type-stable functions
```myco
func process_numbers(data):
    let sum = 0;
    for i in range(data.length):
        sum = sum + data[i];  # Always Int + Int
    end
    return sum;
end
```

**Avoid**: Type-unstable functions
```myco
func process_mixed(data):
    let sum = 0;
    for i in range(data.length):
        if data[i].type == "Int":
            sum = sum + data[i];
        else:
            sum = sum + data[i].toInt();
        end
    end
    return sum;
end
```

### 2. Use Appropriate Data Types

**Good**: Homogeneous arrays
```myco
let numbers = [1, 2, 3, 4, 5];  # All Int
let names = ["Alice", "Bob", "Charlie"];  # All String
```

**Avoid**: Mixed-type arrays
```myco
let mixed = [1, "hello", 3.14, true];  # Mixed types
```

### 3. Minimize Function Call Overhead

**Good**: Inline small functions
```myco
# Small function - will be inlined
func square(x):
    return x * x;
end

let result = square(5);  # Inlined by JIT
```

**Good**: Batch operations
```myco
# Process multiple items at once
func process_batch(items):
    let results = [];
    for item in items:
        results.append(process_item(item));
    end
    return results;
end
```

### 4. Optimize Loops

**Good**: Simple loop conditions
```myco
let sum = 0;
let i = 0;
while i < 1000:
    sum = sum + i;
    i = i + 1;
end
```

**Good**: Avoid complex conditions in hot loops
```myco
# Pre-compute complex conditions
let limit = compute_limit();
let i = 0;
while i < limit:
    # Loop body
    i = i + 1;
end
```

## Performance Monitoring

### Check Optimization Status

```myco
# Print optimization statistics
print("Optimization enabled: " + interpreter.optimization_enabled);
print("JIT enabled: " + interpreter.jit_enabled);
print("Bytecode enabled: " + interpreter.bytecode_enabled);
```

### Benchmark Your Code

```myco
# Simple benchmarking
let start_time = time.now();
# Your code here
let end_time = time.now();
print("Execution time: " + (end_time - start_time) + " ms");
```

## Common Performance Issues

### 1. Type Instability

**Problem**: Function parameters change types frequently
```myco
func process(value):
    return value + 1;  # Sometimes Int, sometimes Float
end
```

**Solution**: Use type guards or separate functions
```myco
func process_int(value):
    return value + 1;  # Always Int
end

func process_float(value):
    return value + 1.0;  # Always Float
end
```

### 2. Excessive Function Calls

**Problem**: Too many small function calls in hot paths
```myco
let sum = 0;
for i in range(1000):
    sum = sum + get_value(i);  # Function call in loop
end
```

**Solution**: Inline or batch operations
```myco
let sum = 0;
for i in range(1000):
    sum = sum + i;  # Direct computation
end
```

### 3. Complex Control Flow

**Problem**: Complex conditionals in hot paths
```myco
for i in range(1000):
    if complex_condition(i):
        # Process
    else:
        # Different process
    end
end
```

**Solution**: Simplify or pre-compute conditions
```myco
let conditions = [];
for i in range(1000):
    conditions.append(complex_condition(i));
end

for i in range(1000):
    if conditions[i]:
        # Process
    else:
        # Different process
    end
end
```

## Performance Tuning

### 1. Adjust Hot Spot Thresholds

```c
// In your C code
HotSpotTracker* tracker = hot_spot_tracker_create();
hot_spot_tracker_set_threshold(tracker, 50);  // Lower threshold = more aggressive optimization
```

### 2. Configure JIT Settings

```c
// In your C code
MicroJitContext* jit = micro_jit_create(JIT_TARGET_AUTO, MICRO_JIT_MODE_FULL);
micro_jit_set_max_function_size(jit, 8192);  // Larger functions
micro_jit_set_inlining(jit, true);  // Enable inlining
```

### 3. Memory Management

```c
// In your C code
ValueSpecializer* specializer = value_specializer_create();
value_specializer_set_max_cache_size(specializer, 1024 * 1024);  // 1MB cache
```

## Benchmarking

### Run Built-in Benchmarks

```bash
# Run comprehensive performance suite
./bin/myco benchmark/performance_suite.myco --jit 1

# Run simple performance test
./bin/myco test_simple_performance.myco --jit 1
```

### Compare Performance

```bash
# Run without optimization
time ./bin/myco script.myco --jit 0

# Run with optimization
time ./bin/myco script.myco --jit 1
```

### Expected Performance Improvements

- **Arithmetic Operations**: 29% faster (measured)
- **Function Calls**: 2-3x faster (estimated)
- **Loop Performance**: 3-5x faster (estimated)
- **Array Operations**: 2-4x faster (estimated)
- **Overall System**: 2-4x average improvement (estimated)

## Troubleshooting

### Disable Optimization for Debugging

```bash
# Run with AST interpreter only
./bin/myco script.myco --jit 0
```

### Check for Optimization Failures

```myco
# The system automatically falls back to AST interpreter
# if optimization fails, so your code will still work
```

### Memory Usage

```bash
# Monitor memory usage
valgrind --tool=memcheck ./bin/myco script.myco --jit 1
```

## Advanced Configuration

### Custom Optimization Levels

```c
// In your C code
AdaptiveExecutor* executor = adaptive_executor_create(interpreter);
AdaptiveExecutorPolicy policy = {
    .bytecode_threshold = 2,      // Compile to bytecode after 2 executions
    .jit_threshold = 50,          // JIT compile after 50 executions
    .specialization_threshold = 100,  // Specialize after 100 executions
    .max_deoptimization_rate = 0.1    // Deoptimize if >10% guard failures
};
adaptive_executor_set_policy(executor, &policy);
```

### Platform-Specific Optimization

```c
// Target specific architecture
MicroJitContext* jit = micro_jit_create(JIT_TARGET_ARM64, MICRO_JIT_MODE_FULL);
```

## Conclusion

For more technical details, see [OPTIMIZATION_ARCHITECTURE.md](OPTIMIZATION_ARCHITECTURE.md).
