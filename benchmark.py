#!/usr/bin/env python3

import subprocess
import time
import statistics
import sys

def run_benchmark(test_file, iterations=10):
    """Run benchmark on a test file multiple times and return timing statistics"""
    
    # Test with bytecode VM (default)
    bytecode_times = []
    for i in range(iterations):
        start = time.time()
        result = subprocess.run(['./bin/myco', test_file], 
                              capture_output=True, text=True)
        end = time.time()
        if result.returncode == 0:
            bytecode_times.append(end - start)
        else:
            print(f"Bytecode VM failed on iteration {i+1}")
            return None, None
    
    # Test with AST only (force AST mode)
    ast_times = []
    for i in range(iterations):
        start = time.time()
        result = subprocess.run(['./bin/myco', test_file, '--ast'], 
                              capture_output=True, text=True)
        end = time.time()
        if result.returncode == 0:
            ast_times.append(end - start)
        else:
            print(f"AST interpreter failed on iteration {i+1}")
            return None, None
    
    return bytecode_times, ast_times

def print_stats(name, times):
    """Print timing statistics"""
    if not times:
        print(f"{name}: No data")
        return
    
    mean_time = statistics.mean(times)
    median_time = statistics.median(times)
    min_time = min(times)
    max_time = max(times)
    std_dev = statistics.stdev(times) if len(times) > 1 else 0
    
    print(f"{name}:")
    print(f"  Mean:   {mean_time:.4f}s")
    print(f"  Median: {median_time:.4f}s")
    print(f"  Min:    {min_time:.4f}s")
    print(f"  Max:    {max_time:.4f}s")
    print(f"  StdDev: {std_dev:.4f}s")
    print()

def main():
    test_files = [
        'test_bytecode_basic.myco',
        'test_simple_print.myco',
        'benchmark_no_loops.myco',
        'benchmark_simple_numeric.myco'
    ]
    
    iterations = 5
    
    print("=== Myco Bytecode VM vs AST Interpreter Benchmark ===\n")
    
    for test_file in test_files:
        print(f"Testing: {test_file}")
        print("-" * 50)
        
        bytecode_times, ast_times = run_benchmark(test_file, iterations)
        
        if bytecode_times and ast_times:
            print_stats("Bytecode VM", bytecode_times)
            print_stats("AST Interpreter", ast_times)
            
            # Calculate speedup
            bytecode_mean = statistics.mean(bytecode_times)
            ast_mean = statistics.mean(ast_times)
            speedup = ast_mean / bytecode_mean if bytecode_mean > 0 else 0
            
            print(f"Speedup: {speedup:.2f}x faster with bytecode VM")
            print()
        else:
            print("Benchmark failed for this test file\n")

if __name__ == "__main__":
    main()
