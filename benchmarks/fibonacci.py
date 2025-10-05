import time

def fibonacci(n):
    if n <= 1:
        return n
    return fibonacci(n-1) + fibonacci(n-2)

start = time.time()
result = fibonacci(35)
end = time.time()
print(f"Python Result: {result}, Time: {end - start:.6f} seconds")
