import time

def main():
    start = time.time()
    
    # Simple mathematical operations
    sum_val = 0.0
    for i in range(10000):
        sum_val += i + (i * 2)
    
    # Array operations
    arr = []
    for i in range(1000):
        arr.append(i)
    
    # String operations
    str_val = ""
    for i in range(100):
        str_val += "A"
    
    end = time.time()
    duration = end - start
    
    print(f"Python benchmark completed in {duration:.6f} seconds")
    print(f"Sum = {sum_val}")
    print(f"Array length = {len(arr)}")
    print(f"String length = {len(str_val)}")

if __name__ == "__main__":
    main()
