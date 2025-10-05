import time

start = time.time()
result = ""
for i in range(10000):
    result += f"test{i}"
end = time.time()
print(f"Python: {len(result)} chars in {end - start:.6f} seconds")
