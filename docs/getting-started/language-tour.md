# Myco Language Tour

Welcome to the Myco Language Tour! This comprehensive guide will take you through all of Myco's features, from basic syntax to advanced concepts.

## Table of Contents

1. [Basic Syntax](#basic-syntax)
2. [Type System](#type-system)
3. [Variables and Constants](#variables-and-constants)
4. [Control Flow](#control-flow)
5. [Functions](#functions)
6. [Collections](#collections)
7. [Classes and Objects](#classes-and-objects)
8. [Pattern Matching](#pattern-matching)
9. [Error Handling](#error-handling)
10. [Modules and Imports](#modules-and-imports)
11. [Standard Library](#standard-library)
12. [Advanced Features](#advanced-features)

## Basic Syntax

### Comments

```myco
# Single-line comment

/*
 * Multi-line comment
 * Can span multiple lines
 */

# Comments can be at the end of lines
let x = 42;  # This is a comment
```

### Statements and Expressions

```myco
# Statements end with semicolons
let name = "Alice";
print("Hello, " + name);

# Expressions can be used in many contexts
let result = 2 + 3 * 4;  # 14
let message = "The result is " + result.toString();
```

### Code Blocks

```myco
# Code blocks use 'end' keyword
if condition:
    # Block content
    print("Inside block");
end

# Functions also use 'end'
func example():
    # Function body
end
```

## Type System

Myco has a strong, static type system with type inference.

### Basic Types

```myco
# Integers
let count: Int = 42;
let negative = -10;  # Inferred as Int

# Floating-point numbers
let pi: Float = 3.14159;
let temperature = 98.6;  # Inferred as Float

# Strings
let greeting: String = "Hello, World!";
let empty = "";  # Inferred as String

# Booleans
let isActive: Boolean = True;
let isComplete = False;  # Inferred as Boolean

# Null
let nothing: Null = Null;
```

### Type Inference

Myco can often infer types automatically:

```myco
# These are all inferred correctly
let number = 42;        # Int
let decimal = 3.14;     # Float
let text = "hello";     # String
let flag = True;        # Boolean
let empty = Null;       # Null
```

### Union Types

Union types allow a variable to hold one of several types:

```myco
# String or Int
let id: String | Int = "user123";
id = 456;  # Now it's an Int

# Multiple types
let value: String | Int | Boolean = "hello";
value = 42;
value = True;
```

### Optional Types

Optional types represent values that might be null:

```myco
# Optional String (can be String or Null)
let name: String? = "Alice";
name = Null;  # Valid

# Working with optionals
if name != Null:
    print("Name is:", name);
else:
    print("No name provided");
end
```

### Type Guards

Check types at runtime:

```myco
let value: String | Int = "hello";

if value.isString():
    print("It's a string:", value);
else if value.isInt():
    print("It's an integer:", value);
end

# Built-in type guards
let x = 42;
print(x.isInt());     # True
print(x.isFloat());   # False
print(x.isNumber());  # True
```

## Variables and Constants

### Variable Declaration

```myco
# Explicit type
let name: String = "Alice";

# Type inference
let age = 30;

# Multiple variables
let x = 1, y = 2, z = 3;

# Assignment
name = "Bob";
age = 31;
```

### Constants

```myco
# Constants cannot be reassigned
const PI: Float = 3.14159;
const APP_NAME = "MyApp";

# This would cause an error:
# PI = 3.14;  # Error: cannot assign to constant
```

### Variable Scope

```myco
let global = "I'm global";

func example():
    let local = "I'm local";
    print(global);  # Can access global
    print(local);   # Can access local
end

# print(local);  # Error: local is not in scope
```

## Control Flow

### If Statements

```myco
let score = 85;

# Basic if
if score >= 90:
    print("Excellent!");
end

# If-else
if score >= 70:
    print("Pass");
else:
    print("Fail");
end

# If-else-if
if score >= 90:
    print("Grade: A");
else if score >= 80:
    print("Grade: B");
else if score >= 70:
    print("Grade: C");
else:
    print("Grade: F");
end
```

### Loops

#### For Loops

```myco
# Range-based for loop
for i in range(1, 6):
    print("Count:", i);
end

# Array iteration
let fruits = ["apple", "banana", "orange"];
for fruit in fruits:
    print("Fruit:", fruit);
end

# With index
for i in range(0, fruits.length):
    print(i, ":", fruits[i]);
end
```

#### While Loops

```myco
let count = 0;
while count < 5:
    print("Count:", count);
    count = count + 1;
end

# Infinite loop with break
let i = 0;
while True:
    print("Loop iteration:", i);
    i = i + 1;
    if i >= 3:
        break;
    end
end
```

### Break and Continue

```myco
# Break out of loop
for i in range(1, 10):
    if i == 5:
        break;  # Exit the loop
    end
    print(i);
end

# Skip iteration
for i in range(1, 6):
    if i == 3:
        continue;  # Skip this iteration
    end
    print(i);
end
```

## Functions

### Basic Functions

```myco
# Function with no parameters
func greet() -> String:
    return "Hello!";
end

# Function with parameters
func add(a: Int, b: Int) -> Int:
    return a + b;
end

# Function with no return type
func printSum(a: Int, b: Int):
    let sum = a + b;
    print("Sum:", sum);
end

# Function calls
let result = add(5, 3);
printSum(10, 20);
```

### Function Overloading

```myco
# Multiple functions with same name but different parameters
func multiply(a: Int, b: Int) -> Int:
    return a * b;
end

func multiply(a: Float, b: Float) -> Float:
    return a * b;
end

# Myco will choose the correct function based on argument types
let intResult = multiply(5, 3);      # Int version
let floatResult = multiply(2.5, 4.0); # Float version
```

### Default Parameters

```myco
func greet(name: String, greeting: String = "Hello") -> String:
    return greeting + ", " + name + "!";
end

print(greet("Alice"));           # "Hello, Alice!"
print(greet("Bob", "Hi"));       # "Hi, Bob!"
```

### Lambda Functions

```myco
# Anonymous functions
let square = func(x: Int) -> Int: return x * x; end;
let add = func(a: Int, b: Int) -> Int: return a + b; end;

print(square(5));    # 25
print(add(3, 4));    # 7

# Lambda as parameter
func applyOperation(x: Int, op: (Int) -> Int) -> Int:
    return op(x);
end

let result = applyOperation(5, square);  # 25
```

### Recursion

```myco
func factorial(n: Int) -> Int:
    if n <= 1:
        return 1;
    else:
        return n * factorial(n - 1);
    end
end

print(factorial(5));  # 120
```

## Collections

### Arrays

```myco
# Array creation
let numbers = [1, 2, 3, 4, 5];
let fruits = ["apple", "banana", "orange"];
let mixed = [1, "hello", True, 3.14];

# Accessing elements
print(numbers[0]);    # First element
print(fruits[2]);     # Third element

# Array length
print("Length:", numbers.length);

# Adding elements
numbers = numbers + [6, 7];
fruits = fruits + ["grape"];

# Array methods
let doubled = numbers.map(func(x: Int) -> Int: return x * 2; end);
let evens = numbers.filter(func(x: Int) -> Boolean: return x % 2 == 0; end);
let sum = numbers.reduce(0, func(acc: Int, x: Int) -> Int: return acc + x; end);
```

### Maps (Dictionaries)

```myco
# Map creation
let person = {
    name: "Alice",
    age: 30,
    city: "New York"
};

# Accessing values
print(person.name);   # Dot notation
print(person["age"]); # Bracket notation

# Adding/updating values
person.email = "alice@example.com";
person["phone"] = "555-1234";

# Checking existence
if person.has("email"):
    print("Email:", person.email);
end

# Map methods
print("Keys:", person.keys());
print("Size:", person.size);
```

### Sets

```myco
# Set creation
let colors = {"red", "green", "blue"};
let numbers = {1, 2, 3, 4, 5};

# Adding elements
colors = colors + {"yellow"};
numbers = numbers + {6, 7};

# Checking membership
if colors.has("red"):
    print("Red is in the set!");
end

# Set operations
let primary = {"red", "green", "blue"};
let secondary = {"yellow", "cyan", "magenta"};
let allColors = primary.union(secondary);
let common = primary.intersection({"red", "blue", "purple"});

# Set methods
print("Size:", colors.size);
print("To array:", colors.toArray());
```

### Advanced Collections

```myco
# Trees
let tree = trees.create();
tree = tree.insert(5);
tree = tree.insert(3);
tree = tree.insert(7);
print("Tree size:", tree.size);

# Graphs
let graph = graphs.create();
graph = graph.addNode("A");
graph = graph.addNode("B");
graph = graph.addEdge("A", "B");
print("Graph size:", graph.size);

# Heaps
let heap = heaps.create();
heap = heap.insert(10);
heap = heap.insert(5);
heap = heap.insert(15);
print("Heap peek:", heap.peek());

# Queues
let queue = queues.create();
queue = queue.enqueue("first");
queue = queue.enqueue("second");
print("Queue front:", queue.front());

# Stacks
let stack = stacks.create();
stack = stack.push("bottom");
stack = stack.push("top");
print("Stack top:", stack.top());
```

## Classes and Objects

### Basic Classes

```myco
class Person:
    let name: String
    let age: Int
    
    # Constructor
    func init(name: String, age: Int):
        self.name = name;
        self.age = age;
    end
    
    # Methods
    func greet() -> String:
        return "Hello, I'm " + self.name;
    end
    
    func haveBirthday():
        self.age = self.age + 1;
    end
end

# Creating objects
let alice = Person("Alice", 30);
let bob = Person("Bob", 25);

# Using methods
print(alice.greet());
alice.haveBirthday();
print("Alice is now", alice.age, "years old");
```

### Inheritance

```myco
class Animal:
    let name: String
    
    func init(name: String):
        self.name = name;
    end
    
    func speak() -> String:
        return "Some sound";
    end
end

class Dog extends Animal:
    func speak() -> String:
        return "Woof!";
    end
    
    func fetch() -> String:
        return self.name + " fetches the ball!";
    end
end

# Using inheritance
let dog = Dog("Buddy");
print(dog.speak());    # "Woof!"
print(dog.fetch());    # "Buddy fetches the ball!"
```

### Method Overriding

```myco
class Shape:
    func area() -> Float:
        return 0.0;
    end
end

class Rectangle extends Shape:
    let width: Float
    let height: Float
    
    func init(width: Float, height: Float):
        self.width = width;
        self.height = height;
    end
    
    func area() -> Float:
        return self.width * self.height;
    end
end

let rect = Rectangle(5.0, 3.0);
print("Area:", rect.area());  # 15.0
```

## Pattern Matching

### Basic Pattern Matching

```myco
func describeNumber(n: Int) -> String:
    match n:
        case 0:
            return "Zero";
        case 1:
            return "One";
        case 2:
            return "Two";
        else:
            return "Some other number";
    end
end

print(describeNumber(0));  # "Zero"
print(describeNumber(5));  # "Some other number"
```

### Pattern Matching with Types

```myco
func processValue(value: String | Int | Boolean) -> String:
    match value:
        case str: String:
            return "String: " + str;
        case num: Int:
            return "Number: " + num.toString();
        case flag: Boolean:
            return "Boolean: " + flag.toString();
    end
end

print(processValue("hello"));  # "String: hello"
print(processValue(42));       # "Number: 42"
print(processValue(True));     # "Boolean: True"
```

### Pattern Matching with Guards

```myco
func categorizeAge(age: Int) -> String:
    match age:
        case n if n < 0:
            return "Invalid age";
        case n if n < 13:
            return "Child";
        case n if n < 20:
            return "Teenager";
        case n if n < 65:
            return "Adult";
        else:
            return "Senior";
    end
end
```

## Error Handling

### Optional Types

```myco
# Safe division
func safeDivide(a: Float, b: Float) -> Float?:
    if b == 0:
        return Null;
    else:
        return a / b;
    end
end

# Using optionals
let result = safeDivide(10, 2);
if result != Null:
    print("Result:", result);
else:
    print("Cannot divide by zero!");
end
```

### Error Propagation

```myco
func processFile(filename: String) -> String?:
    if not file.exists(filename):
        return Null;
    end
    
    let content = file.read(filename);
    if content == Null:
        return Null;
    end
    
    return content.upper();
end

# Using the function
let processed = processFile("data.txt");
if processed != Null:
    print("Processed:", processed);
else:
    print("Failed to process file");
end
```

## Modules and Imports

### Basic Imports

```myco
# Import entire module
use math;
use string;
use file;

# Use imported functions
let result = math.sqrt(16);  # 4.0
let upper = string.upper("hello");  # "HELLO"
let content = file.read("data.txt");
```

### Selective Imports

```myco
# Import specific functions
use math {sqrt, pow, pi};
use string {upper, lower, trim};

let result = sqrt(pow(2, 3));  # sqrt(8) ≈ 2.83
let text = upper("hello world");
```

### Custom Modules

```myco
# In math_utils.myco
func factorial(n: Int) -> Int:
    if n <= 1:
        return 1;
    else:
        return n * factorial(n - 1);
    end
end

# In main.myco
use math_utils {factorial};

let result = factorial(5);  # 120
```

## Standard Library

### Math Library

```myco
use math;

print(math.pi);        # 3.14159...
print(math.e);         # 2.71828...
print(math.sqrt(16));  # 4.0
print(math.pow(2, 3)); # 8.0
print(math.abs(-5));   # 5
print(math.min(3, 7)); # 3
print(math.max(3, 7)); # 7
```

### String Library

```myco
use string;

let text = "  Hello, World!  ";
print(string.upper(text));    # "  HELLO, WORLD!  "
print(string.lower(text));    # "  hello, world!  "
print(string.trim(text));     # "Hello, World!"
print(string.length(text));   # 15
```

### File Library

```myco
use file;

# Write to file
file.write("output.txt", "Hello, File!");

# Read from file
let content = file.read("output.txt");
if content != Null:
    print("Content:", content);
end

# Check if file exists
if file.exists("output.txt"):
    print("File exists!");
end

# Delete file
file.delete("output.txt");
```

### HTTP Library

```myco
use http;

# GET request
let response = http.get("https://api.example.com/data");
if response.statusOk():
    let data = http.getJson(response);
    print("Data:", data);
end

# POST request
let postData = {name: "Alice", age: 30};
let postResponse = http.post("https://api.example.com/users", postData);
```

### Time Library

```myco
use time;

# Current time
let now = time.now();
print("Current time:", now.isoString());

# Create specific time
let birthday = time.create(1990, 5, 15, 14, 30, 0);
print("Birthday:", birthday.format("YYYY-MM-DD"));

# Time operations
let future = now.add(365, "days");
let diff = future.difference(now);
print("Days difference:", diff.days());
```

## Advanced Features

### Generics (Coming Soon)

```myco
# Generic function (planned feature)
func identity<T>(value: T) -> T:
    return value;
end

let num = identity(42);        # Int
let str = identity("hello");   # String
```

### Async/Await (Planned)

```myco
# Async function (planned feature)
async func fetchData(url: String) -> String:
    let response = await http.get(url);
    return response.text();
end

# Using async function
let data = await fetchData("https://api.example.com/data");
```

### Memory Management

Myco handles memory management automatically:

```myco
# No need to manually free memory
let largeArray = [];
for i in range(1, 1000000):
    largeArray = largeArray + [i];
end

# Memory is automatically freed when variables go out of scope
```

### Performance Optimization

Myco includes several performance optimizations:

- **Bytecode compilation** for fast execution
- **JIT optimization** for hot code paths
- **Memory pooling** for efficient allocation
- **Branch prediction** hints for CPU optimization

## Next Steps

You've completed the Myco Language Tour! Here's what to explore next:

1. **[Tutorials](../tutorials/)** - Step-by-step learning guides
2. **[API Reference](../api-reference/)** - Complete standard library documentation
3. **[Examples](https://github.com/ivymycelia/Myco/tree/main/examples)** - More code examples
4. **[Developer Guide](../developer-guide/)** - Advanced topics and internals

---

**Congratulations!** You now have a comprehensive understanding of the Myco programming language. Start building amazing applications! 🚀
