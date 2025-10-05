import time

start = time.time()

# Create and populate array
arr = [i * 2 for i in range(50000)]

# Perform operations
sum_val = sum(arr)
for i in range(0, 50000, 2):
    arr[i] = arr[i] * 3

# Find max
max_val = max(arr)

end = time.time()
print(f"Python Time: {end - start:.6f} seconds (Sum: {sum_val}, Max: {max_val})")
