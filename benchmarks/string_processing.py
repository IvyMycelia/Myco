import time

def process_string(input_str):
    result = ""
    for char in input_str:
        if char.islower():
            result += char.upper()
        elif char.isupper():
            result += char.lower()
        else:
            result += char
        result += char  # duplicate
    return result

test_string = "Hello World 123 ABC xyz"
start = time.time()

for i in range(10000):
    result = process_string(test_string)

end = time.time()
print(f"Python Time: {end - start:.6f} seconds")
