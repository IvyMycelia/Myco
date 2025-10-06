import time
import math

def main():
    start = time.time()
    
    # Fibonacci calculation (recursive)
    def fib(n):
        if n <= 1:
            return n
        return fib(n-1) + fib(n-2)
    
    # Prime number calculation
    def is_prime(n):
        if n < 2:
            return False
        for i in range(2, int(n**0.5) + 1):
            if n % i == 0:
                return False
        return True
    
    # Array operations
    arr = [i * i for i in range(1000)]
    
    # Mathematical operations
    sum_val = 0
    for i in range(1000000):
        sum_val += math.sqrt(i) + math.sin(i) + math.cos(i)
    
    # String operations
    str_val = ''.join(chr(ord('A') + (i % 26)) for i in range(1000))
    
    end = time.time()
    time_spent = end - start
    
    print(f"Python benchmark completed in {time_spent:.6f} seconds")
    print(f"Fibonacci(30) = {fib(30)}")
    print(f"Sum of math operations = {sum_val:.2f}")
    print(f"String length = {len(str_val)}")

if __name__ == "__main__":
    main()
