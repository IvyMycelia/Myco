# Standard Library Reference

The Myco standard library provides a comprehensive set of built-in functions, types, and utilities for common programming tasks.

## Table of Contents

1. [Core Types](#core-types)
2. [Math Library](#math-library)
3. [String Library](#string-library)
4. [Collections](#collections)
5. [I/O Operations](#io-operations)
6. [Time Library](#time-library)
7. [HTTP Library](#http-library)
8. [JSON Library](#json-library)
9. [Regex Library](#regex-library)
10. [File System](#file-system)
11. [Directory Operations](#directory-operations)

## Core Types

### Basic Types

```myco
# Numbers
let intValue: Int = 42;
let floatValue: Float = 3.14;

# Text
let stringValue: String = "Hello, World!";

# Boolean
let boolValue: Boolean = True;

# Null
let nullValue: Null = Null;
```

### Type Checking

```myco
let value = "hello";

# Type guards
value.isString();    # True
value.isInt();       # False
value.isFloat();     # False
value.isBoolean();   # False
value.isNull();      # False
value.isNumber();    # False (for strings)

# Type conversion
let num = 42;
num.toString();      # "42"
num.toFloat();       # 42.0

let str = "3.14";
str.toFloat();       # 3.14
str.toInt();         # 3 (truncates)
```

## Math Library

The math library provides mathematical functions and constants.

### Import

```myco
use math;
```

### Constants

```myco
math.pi;    # 3.141592653589793
math.e;     # 2.718281828459045
```

### Basic Functions

```myco
# Absolute value
math.abs(-5);        # 5
math.abs(3.14);      # 3.14

# Square root
math.sqrt(16);       # 4.0
math.sqrt(2);        # 1.4142135623730951

# Power
math.pow(2, 3);      # 8.0
math.pow(10, 2);     # 100.0

# Logarithm
math.log(math.e);    # 1.0
math.log10(100);     # 2.0

# Trigonometric functions
math.sin(math.pi/2); # 1.0
math.cos(0);         # 1.0
math.tan(math.pi/4); # 1.0
```

### Min/Max Functions

```myco
math.min(5, 10);     # 5
math.max(5, 10);     # 10
math.min(1, 2, 3);   # 1
math.max(1, 2, 3);   # 3
```

### Rounding Functions

```myco
math.floor(3.7);     # 3.0
math.ceil(3.2);      # 4.0
math.round(3.5);     # 4.0
math.round(3.4);     # 3.0
```

## String Library

The string library provides text manipulation functions.

### Import

```myco
use string;
```

### Case Conversion

```myco
let text = "Hello, World!";

string.upper(text);  # "HELLO, WORLD!"
string.lower(text);  # "hello, world!"
```

### Whitespace Handling

```myco
let text = "  Hello, World!  ";

string.trim(text);           # "Hello, World!"
string.trimLeft(text);       # "Hello, World!  "
string.trimRight(text);      # "  Hello, World!"
```

### String Properties

```myco
let text = "Hello, World!";

text.length;                 # 13
text.isEmpty();              # False
text.startsWith("Hello");    # True
text.endsWith("!");          # True
text.contains("World");      # True
```

### String Manipulation

```myco
let text = "Hello, World!";

text.replace("World", "Myco");  # "Hello, Myco!"
text.substring(0, 5);           # "Hello"
text.split(", ");               # ["Hello", "World!"]
```

## Collections

### Arrays

```myco
# Creation
let numbers = [1, 2, 3, 4, 5];
let fruits = ["apple", "banana", "orange"];

# Access
numbers[0];              # 1
numbers[2];              # 3
fruits[1];               # "banana"

# Properties
numbers.length;          # 5
numbers.isEmpty();       # False

# Methods
numbers.push(6);         # [1, 2, 3, 4, 5, 6]
numbers.pop();           # 6, array becomes [1, 2, 3, 4, 5]
numbers.join(", ");      # "1, 2, 3, 4, 5"
numbers.contains(3);     # True
numbers.indexOf(3);      # 2
numbers.slice(1, 3);     # [2, 3]
numbers.reverse();       # [5, 4, 3, 2, 1]
numbers.sort();          # [1, 2, 3, 4, 5]

# Functional methods
let doubled = numbers.map(func(x: Int) -> Int: return x * 2; end);
let evens = numbers.filter(func(x: Int) -> Boolean: return x % 2 == 0; end);
let sum = numbers.reduce(0, func(acc: Int, x: Int) -> Int: return acc + x; end);
```

### Maps (Dictionaries)

```myco
# Creation
let person = {
    name: "Alice",
    age: 30,
    city: "New York"
};

# Access
person.name;             # "Alice"
person["age"];           # 30

# Properties
person.size;             # 3
person.isEmpty();        # False

# Methods
person.has("name");      # True
person.keys();           # ["name", "age", "city"]
person.values();         # ["Alice", 30, "New York"]
person.set("email", "alice@example.com");
person.delete("city");
person.clear();
```

### Sets

```myco
# Creation
let colors = {"red", "green", "blue"};
let numbers = {1, 2, 3, 4, 5};

# Properties
colors.size;             # 3
colors.isEmpty();        # False

# Methods
colors.has("red");       # True
colors.add("yellow");    # {"red", "green", "blue", "yellow"}
colors.remove("green");  # {"red", "blue", "yellow"}
colors.toArray();        # ["red", "blue", "yellow"]

# Set operations
let primary = {"red", "green", "blue"};
let secondary = {"yellow", "cyan", "magenta"};
let all = primary.union(secondary);
let common = primary.intersection({"red", "blue", "purple"});
let diff = primary.difference({"red"});
```

### Advanced Collections

#### Trees

```myco
use trees;

let tree = trees.create();
tree = tree.insert(5);
tree = tree.insert(3);
tree = tree.insert(7);
tree = tree.insert(1);
tree = tree.insert(9);

tree.size;               # 5
tree.isEmpty();          # False
tree.search(3);          # True
tree.search(4);          # False
tree = tree.clear();     # Empty tree
```

#### Graphs

```myco
use graphs;

let graph = graphs.create();
graph = graph.addNode("A");
graph = graph.addNode("B");
graph = graph.addNode("C");
graph = graph.addEdge("A", "B");
graph = graph.addEdge("B", "C");

graph.size;              # 3
graph.isEmpty();         # False
graph.hasNode("A");      # True
graph.hasEdge("A", "B"); # True
graph = graph.clear();   # Empty graph
```

#### Heaps

```myco
use heaps;

let heap = heaps.create();
heap = heap.insert(10);
heap = heap.insert(5);
heap = heap.insert(15);
heap = heap.insert(3);

heap.size;               # 4
heap.isEmpty();          # False
heap.peek();             # 3 (minimum)
let min = heap.extract(); # 3, heap becomes [5, 10, 15]
```

#### Queues

```myco
use queues;

let queue = queues.create();
queue = queue.enqueue("first");
queue = queue.enqueue("second");
queue = queue.enqueue("third");

queue.size;              # 3
queue.isEmpty();         # False
queue.front();           # "first"
queue.back();            # "third"
let item = queue.dequeue(); # "first", queue becomes ["second", "third"]
```

#### Stacks

```myco
use stacks;

let stack = stacks.create();
stack = stack.push("bottom");
stack = stack.push("middle");
stack = stack.push("top");

stack.size;              # 3
stack.isEmpty();         # False
stack.top();             # "top"
let item = stack.pop();  # "top", stack becomes ["bottom", "middle"]
```

## I/O Operations

### Print Functions

```myco
# Basic printing
print("Hello, World!");
print("Number:", 42);
print("Multiple values:", 1, 2, 3);

# Print without newline
print("Same line");
print(" continuation");

# Print to stderr
printError("Error message");
```

### Input Functions

```myco
# Read input from user
let name = input("Enter your name: ");
let age = input("Enter your age: ").toInt();
```

## Time Library

The time library provides date and time operations.

### Import

```myco
use time;
```

### Current Time

```myco
let now = time.now();
print(now.isoString());      # "2024-01-15T10:30:45Z"
print(now.unixTimestamp());  # 1705312245
```

### Creating Time

```myco
let birthday = time.create(1990, 5, 15, 14, 30, 0);
print(birthday.format("YYYY-MM-DD"));  # "1990-05-15"
```

### Time Components

```myco
let now = time.now();

now.year();     # 2024
now.month();    # 1
now.day();      # 15
now.hour();     # 10
now.minute();   # 30
now.second();   # 45
```

### Time Operations

```myco
let now = time.now();
let future = now.add(365, "days");
let past = now.subtract(30, "days");
let diff = future.difference(now);

print("Days difference:", diff.days());
print("Hours difference:", diff.hours());
```

## HTTP Library

The HTTP library provides web request functionality.

### Import

```myco
use http;
```

### GET Requests

```myco
let response = http.get("https://api.example.com/data");
if response.statusOk():
    let data = http.getJson(response);
    print("Data:", data);
end
```

### POST Requests

```myco
let data = {name: "Alice", age: 30};
let response = http.post("https://api.example.com/users", data);
if response.statusOk():
    print("User created successfully");
end
```

### Response Handling

```myco
let response = http.get("https://api.example.com/data");

response.statusCode();           # 200
response.statusOk();             # True
http.getHeader(response, "content-type");  # "application/json"
let jsonData = http.getJson(response);
```

## JSON Library

The JSON library provides JSON parsing and generation.

### Import

```myco
use json;
```

### Stringify

```myco
let data = {name: "Alice", age: 30, active: True};
let jsonString = json.stringify(data);
print(jsonString);  # '{"name":"Alice","age":30,"active":true}'
```

### Parse

```myco
let jsonString = '{"name":"Alice","age":30}';
let data = json.parse(jsonString);
if data != Null:
    print("Name:", data.name);  # "Alice"
    print("Age:", data.age);    # 30
end
```

### Validation

```myco
let validJson = '{"name":"Alice"}';
let invalidJson = '{name:"Alice"}';

json.validate(validJson);   # True
json.validate(invalidJson); # False
```

## Regex Library

The regex library provides regular expression functionality.

### Import

```myco
use regex;
```

### Basic Matching

```myco
let text = "Hello, World!";
let pattern = "Hello";

regex.test(text, pattern);  # True
let match = regex.match(text, pattern);
if match != Null:
    print("Found:", match.text());
end
```

### Email Validation

```myco
let email = "user@example.com";
regex.isEmail(email);  # True

let invalidEmail = "not-an-email";
regex.isEmail(invalidEmail);  # False
```

### URL Validation

```myco
let url = "https://www.example.com";
regex.isUrl(url);  # True

let invalidUrl = "not-a-url";
regex.isUrl(invalidUrl);  # False
```

### IP Address Validation

```myco
let ip = "192.168.1.1";
regex.isIp(ip);  # True

let invalidIp = "999.999.999.999";
regex.isIp(invalidIp);  # False
```

## File System

The file library provides file operations.

### Import

```myco
use file;
```

### File Operations

```myco
# Write to file
file.write("data.txt", "Hello, File!");

# Read from file
let content = file.read("data.txt");
if content != Null:
    print("Content:", content);
end

# Check if file exists
if file.exists("data.txt"):
    print("File exists!");
end

# Delete file
file.delete("data.txt");
```

## Directory Operations

The directory library provides directory operations.

### Import

```myco
use dir;
```

### Directory Operations

```myco
# Get current directory
let currentDir = dir.current();
print("Current directory:", currentDir);

# List directory contents
let files = dir.list(".");
for file in files:
    print("File:", file);
end

# Create directory
let result = dir.create("new_folder");
if result == Null:
    print("Directory created successfully");
end

# Check if directory exists
if dir.exists("new_folder"):
    print("Directory exists!");
end

# Remove directory
dir.remove("new_folder");
```

## Built-in Functions

### Type Conversion

```myco
# Number to string
let num = 42;
let str = num.toString();  # "42"

# String to number
let text = "3.14";
let float = text.toFloat();  # 3.14
let int = text.toInt();      # 3

# Boolean to string
let flag = True;
let boolStr = flag.toString();  # "True"
```

### Array Functions

```myco
# Range function
let numbers = range(1, 6);  # [1, 2, 3, 4, 5]

# Length function
let length = len("hello");  # 5
let arrayLength = len([1, 2, 3]);  # 3
```

### Type Checking

```myco
let value = "hello";

# Type guards
isString(value);    # True
isInt(value);       # False
isFloat(value);     # False
isBoolean(value);   # False
isNull(value);      # False
isNumber(value);    # False
isArray(value);     # False
isObject(value);    # False
```

---

This reference covers the core functionality of the Myco standard library. For more detailed examples and advanced usage, see the [tutorials](../tutorials/) and [examples](https://github.com/ivymycelia/Myco/tree/main/examples).
