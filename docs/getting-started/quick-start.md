# Quick Start Guide

Get up and running with Myco in just a few minutes! This guide will walk you through writing your first Myco program and introduce you to the language's key features.

## Your First Myco Program

Let's start with the classic "Hello, World!" program:

1. **Create a new file** called `hello.myco`:
   ```myco
   print("Hello, World!");
   ```

2. **Run the program**:
   ```bash
   myco hello.myco
   ```

3. **See the output**:
   ```
   Hello, World!
   ```

Congratulations! You've just run your first Myco program! 🎉

## Variables and Types

Myco is both statically and dynamically typed, which means variables can have specific types or be inferred at runtime. Let's explore the basic types:

```myco
# Numbers
let age: Int = 25;        # Statically typed: Int
let height: Float = 5.9;  # Statically typed: Float
let temperature = -10.5;  # Dynamically typed: Float

# Text
let name: String = "Alice";      # Statically typed: String
let message = "Hello, " + name;  # String concatenation

# Boolean
let isStudent: Boolean = True;   # Statically typed: Boolean
let hasLicense = False;          # Dynamically typed: Boolean

# Null
let middleName: String? = Null;  # Optional type: String?

# Print everything
print("Name:", name);
print("Age:", age);
print("Height:", height);
print("Is student:", isStudent);
```

## Control Flow

### If Statements

```myco
let score = 85;

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

```myco
# For loop
for i in range(1, 6):
    print("Count:", i);
end

# While loop
let count = 0;
while count < 3:
    print("Hello!");
    count = count + 1;
end
```

## Functions

Functions let you organize and reuse code:

```myco
# Function definition
func greet(name: String) -> String:
    return "Hello, " + name + "!";
end

# Function with multiple parameters
func add(a: Int, b: Int) -> Int:
    return a + b;
end

# Function with no return type
func sayGoodbye(name: String):
    print("Goodbye, " + name + "!");
end

# Use the functions
let greeting = greet("Alice");
print(greeting);

let sum = add(5, 3);
print("5 + 3 =", sum);

sayGoodbye("Bob");
```

## Collections

Myco provides powerful built-in data structures:

### Arrays

```myco
# Create an array
let numbers = [1, 2, 3, 4, 5];
let fruits = ["apple", "banana", "orange"];

# Access elements
print("First number:", numbers[0]);
print("Last fruit:", fruits[2]);

# Add elements
numbers = numbers + [6, 7];
fruits = fruits + ["grape"];

# Get length
print("Numbers count:", numbers.length);
```

### Maps (Dictionaries)

```myco
# Create a map
let person = {
    name: "Alice",
    age: 30,
    city: "New York"
};

# Access values
print("Name:", person.name);
print("Age:", person.age);

# Add/update values
person.email = "alice@example.com";
person.age = 31;

# Check if key exists
if person.has("email"):
    print("Email:", person.email);
end
```

### Sets

```myco
# Create a set
let colors = {"red", "green", "blue"};

# Add elements
colors = colors + {"yellow"};

# Check membership
if colors.has("red"):
    print("Red is in the set!");
end

# Get size
print("Colors count:", colors.size);
```

## Classes and Objects

Myco supports object-oriented programming:

```myco
# Define a class
class Person:
    let name: String
    let age: Int
    
    func greet() -> String:
        return "Hi, I'm " + self.name + " and I'm " + self.age.toString() + " years old.";
    end
    
    func haveBirthday():
        self.age = self.age + 1;
    end
end

# Create objects
let alice = Person("Alice", 30);
let bob = Person("Bob", 25);

# Use methods
print(alice.greet());
alice.haveBirthday();
print("After birthday:", alice.greet());
```

## Error Handling

Myco provides safe error handling:

```myco
# Safe division
func safeDivide(a: Float, b: Float) -> Float?:
    if b == 0:
        return Null;  # Return null for division by zero
    else:
        return a / b;
    end
end

# Use the function
let result = safeDivide(10, 2);
if result != Null:
    print("Result:", result);
else:
    print("Cannot divide by zero!");
end
```

## Pattern Matching

Myco supports powerful pattern matching:

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

## Working with Files

```myco
# Write to a file
let content = "Hello, File!";
file.write("output.txt", content);

# Read from a file
let data = file.read("output.txt");
if data != Null:
    print("File content:", data);
end

# Check if file exists
if file.exists("output.txt"):
    print("File exists!");
end
```

## Running Your Programs

### Command Line

```bash
# Run a single file
myco program.myco

# Run with debug output
myco --debug program.myco

# Run and show version
myco --version
```

### Interactive Mode (REPL)

```bash
# Start the REPL
myco

# In the REPL, you can type expressions directly:
> let x = 42;
> print(x);
42
> let y = x * 2;
> print(y);
84
> exit
```

## Next Steps

You've learned the basics of Myco! Here's what to explore next:

1. **[Language Tour](language-tour.md)** - Deep dive into Myco's features
2. **[Tutorials](../tutorials/)** - Step-by-step learning guides
3. **[API Reference](../api-reference/)** - Complete standard library documentation
4. **[Examples](https://github.com/ivymycelia/Myco/tree/main/examples)** - More code examples

## Common Patterns

Here are some common programming patterns in Myco:

### Data Processing

```myco
let numbers = [1, 2, 3, 4, 5];
let doubled = [];

for num in numbers:
    doubled = doubled + [num * 2];
end

print("Original:", numbers);
print("Doubled:", doubled);
```

### Configuration

```myco
let config = {
    app_name: "MyApp",
    version: "1.0.0",
    debug: True,
    port: 8080
};

print("Starting", config.app_name, "v" + config.version);
if config.debug:
    print("Debug mode enabled");
end
```

### Data Validation

```myco
func validateEmail(email: String) -> Boolean:
    if email.length == 0:
        return False;
    end
    
    if email.contains("@"):
        return True;
    else:
        return False;
    end
end

let email = "user@example.com";
if validateEmail(email):
    print("Valid email!");
else:
    print("Invalid email!");
end
```

---

**Ready for more?** Check out the [Language Tour](language-tour.md) to explore Myco's advanced features! 🚀
