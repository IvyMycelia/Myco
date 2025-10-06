import time

start = time.time()
sum_val = 0
for i in range(1, 100001):
    sum_val += i
end = time.time()
print(f"Python: {sum_val} in {end - start:.6f} seconds")
