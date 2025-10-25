# Myco Examples

This document contains working examples of Myco code.

## Basic Examples

### Hello World

```myco
print("Hello, World!");
```

### Variables and Types

```myco
let name = "Myco";
let version = 1.0;
let is_awesome = true;

print("Language: " + name);
print("Version: " + version.toString());
print("Awesome: " + is_awesome.toString());
```

### Functions

```myco
func greet(name):
    return "Hello, " + name + "!";
end;

print(greet("World"));
```

### Arrays

```myco
let numbers = [1, 2, 3, 4, 5];
let fruits = ["apple", "banana", "orange"];

print("First number: " + numbers[0].toString());
print("First fruit: " + fruits[0]);
print("Array length: " + numbers.length.toString());
```

### Objects

```myco
let person = {
    name: "Alice",
    age: 30,
    city: "New York"
};

print("Name: " + person.name);
print("Age: " + person.age.toString());
```

### Control Flow

```myco
let score = 85;

if score >= 90:
    print("Grade: A");
elif score >= 80:
    print("Grade: B");
elif score >= 70:
    print("Grade: C");
else:
    print("Grade: F");
end;
```

### Loops

```myco
# While loop
let i = 0;
while i < 5:
    print("Count: " + i.toString());
    i = i + 1;
end;

# For loop
for let j = 0; j < 3; j++:
    print("For count: " + j.toString());
end;
```

## Intermediate Examples

### Classes

```myco
class Person:
    func constructor(name, age):
        self.name = name;
        self.age = age;
    end;
    
    func greet():
        return "Hello, I'm " + self.name;
    end;
end;

let alice = Person("Alice", 30);
print(alice.greet());
```

### File Operations

```myco
# Read file
let content = file.read("data.txt");
print("File content: " + content);

# Write file
file.write("output.txt", "Hello from Myco!");
```

### JSON Operations

```myco
let data = {
    name: "Myco",
    version: 1.0,
    features: ["fast", "safe", "simple"]
};

let json_string = json.stringify(data);
print("JSON: " + json_string);

let parsed_data = json.parse(json_string);
print("Parsed name: " + parsed_data.name);
```

## Advanced Examples

### HTTP Server

```myco
let app = server.create(3000);

func handle_request(req, res):
    let response = {
        message: "Hello from Myco!",
        timestamp: time.now()
    };
    set_response_body(json.stringify(response));
    set_response_status(200);
end;

app.get("/api/hello", handle_request);
app.listen();
```

### Error Handling

```myco
func safe_divide(a, b):
    if b == 0:
        return null;
    end;
    return a / b;
end;

let result = safe_divide(10, 2);
if result != null:
    print("Result: " + result.toString());
else:
    print("Division by zero!");
end;
```

