# Tutorial 2: Variables & Types

In this tutorial, you'll learn about Myco's type system, how to work with different data types, and how to use variables effectively.

## What You'll Learn

- Myco's type system and type inference
- Working with numbers (integers and floats)
- Working with strings and text
- Boolean values and logical operations
- Optional types and null handling
- Type checking and conversion

## Prerequisites

- Completed [Tutorial 1: Hello World](01-hello-world.md)
- Basic understanding of programming concepts

## Step 1: Understanding Types

Myco is a **statically and dynamically typed** language, which means variables can have specific types or be inferred at runtime.

### Basic Types

Myco has several built-in types:

- `Int` - Whole numbers (42, -10, 0)
- `Float` - Decimal numbers (3.14, -2.5, 0.0)
- `String` - Text ("hello", "world")
- `Boolean` - True/false values (True, False)
- `Null` - Represents "no value"

## Step 2: Working with Numbers

### Integers

```myco
# Explicit type declaration
let count: Int = 42;
let negative: Int = -10;

# Type inference (Myco figures out the type)
let age = 25;        # Inferred as Int
let temperature = -5; # Inferred as Int

# Integer operations
let sum = count + age;           # 67
let product = count * 2;         # 84
let remainder = count % 10;      # 2 (remainder of 42 ÷ 10)

print("Count:", count);
print("Age:", age);
print("Sum:", sum);
print("Product:", product);
print("Remainder:", remainder);
```

### Floating-Point Numbers

```myco
# Explicit type declaration
let pi: Float = 3.14159;
let temperature: Float = 98.6;

# Type inference
let price = 19.99;    # Inferred as Float
let ratio = 2.5;      # Inferred as Float

# Float operations
let total = price * 1.08;        # 21.5892 (with tax)
let average = (2.5 + 3.7) / 2;   # 3.1

print("Price:", price);
print("Total with tax:", total);
print("Average:", average);
```

### Number Conversion

```myco
let intValue = 42;
let floatValue = 3.14;

# Convert int to float
let intAsFloat = intValue.toFloat();
print("Int as float:", intAsFloat);  # 42.0

# Convert float to int (truncates decimal part)
let floatAsInt = floatValue.toInt();
print("Float as int:", floatAsInt);  # 3

# Convert numbers to strings
let intAsString = intValue.toString();
let floatAsString = floatValue.toString();
print("Int as string:", intAsString);    # "42"
print("Float as string:", floatAsString); # "3.14"
```

## Step 3: Working with Strings

### String Basics

```myco
# String literals
let name = "Alice";
let message = "Hello, World!";
let empty = "";

# String concatenation
let greeting = "Hello, " + name + "!";
let fullMessage = greeting + " How are you?";

print(greeting);        # "Hello, Alice!"
print(fullMessage);     # "Hello, Alice! How are you?"
```

### String Methods

```myco
let text = "  Hello, World!  ";

# Length
print("Length:", text.length);  # 15

# Case conversion
print("Upper:", text.upper());  # "  HELLO, WORLD!  "
print("Lower:", text.lower());  # "  hello, world!  "

# Trimming whitespace
print("Trimmed:", text.trim()); # "Hello, World!"

# Checking content
print("Starts with 'Hello':", text.startsWith("Hello"));  # False (due to spaces)
print("Contains 'World':", text.contains("World"));       # True
```

### String Interpolation (Planned Feature)

```myco
# Current way (string concatenation)
let name = "Alice";
let age = 25;
let info = "My name is " + name + " and I am " + age.toString() + " years old.";

# Future way (string interpolation - planned)
# let info = "My name is ${name} and I am ${age} years old.";
```

## Step 4: Boolean Values

### Boolean Basics

```myco
# Boolean literals
let isStudent = True;
let hasLicense = False;

# Boolean from expressions
let isAdult = age >= 18;        # True if age is 18 or more
let isEven = (count % 2) == 0;  # True if count is even

print("Is student:", isStudent);
print("Is adult:", isAdult);
print("Is even:", isEven);
```

### Logical Operations

```myco
let a = True;
let b = False;

# Logical AND
print("a AND b:", a and b);  # False

# Logical OR
print("a OR b:", a or b);    # True

# Logical NOT
print("NOT a:", not a);      # False
print("NOT b:", not b);      # True

# Complex expressions
let result = (a and b) or (not a and b);
print("Complex:", result);   # False
```

## Step 5: Optional Types

Optional types represent values that might be null. This is useful for handling cases where a value might not exist.

### Basic Optional Types

```myco
# Optional String (can be String or Null)
let name: String? = "Alice";
let middleName: String? = Null;

# Check if value exists
if name != Null:
    print("Name:", name);
else:
    print("No name provided");
end

if middleName != Null:
    print("Middle name:", middleName);
else:
    print("No middle name");
end
```

### Working with Optionals

```myco
# Function that might return null
func findUser(id: Int) -> String?:
    if id == 1:
        return "Alice";
    else if id == 2:
        return "Bob";
    else:
        return Null;  # User not found
    end
end

# Using the function
let user1 = findUser(1);
let user2 = findUser(3);

if user1 != Null:
    print("Found user:", user1);
else:
    print("User 1 not found");
end

if user2 != Null:
    print("Found user:", user2);
else:
    print("User 3 not found");
end
```

## Step 6: Type Checking

Myco provides built-in functions to check types at runtime:

### Type Guards

```myco
let value: String | Int = "hello";

# Check if value is a string
if value.isString():
    print("It's a string:", value);
end

# Check if value is an integer
if value.isInt():
    print("It's an integer:", value);
end

# Check if value is a number (int or float)
if value.isNumber():
    print("It's a number:", value);
end
```

### Type Conversion with Guards

```myco
func processValue(value: String | Int | Boolean):
    if value.isString():
        let str = value;  # Type narrowing
        print("String length:", str.length);
    else if value.isInt():
        let num = value;  # Type narrowing
        print("Number squared:", num * num);
    else if value.isBoolean():
        let flag = value;  # Type narrowing
        print("Boolean value:", flag);
    end
end

processValue("hello");  # "String length: 5"
processValue(42);       # "Number squared: 1764"
processValue(True);     # "Boolean value: True"
```

## Step 7: Type Annotations

While Myco can infer types automatically, you can also specify them explicitly:

### Explicit Type Annotations

```myco
# Explicit types
let name: String = "Alice";
let age: Int = 25;
let height: Float = 5.9;
let isStudent: Boolean = True;

# Type inference (recommended when type is obvious)
let city = "New York";        # Inferred as String
let count = 42;               # Inferred as Int
let price = 19.99;            # Inferred as Float
let active = True;            # Inferred as Boolean
```

### When to Use Explicit Types

Use explicit types when:
- The type isn't obvious from the value
- You want to document your code
- You're working with complex types
- You want to catch type errors early

```myco
# Type isn't obvious
let result: Int = calculateSomething();

# Documenting complex types
let user: {name: String, age: Int, email: String?} = {
    name: "Alice",
    age: 25,
    email: "alice@example.com"
};

# Optional types
let data: String? = fetchData();
```

## Step 8: Practice Exercises

### Exercise 1: Type Detective
Create a program that:
- Declares variables of different types
- Uses type guards to check each variable's type
- Prints what type each variable is

### Exercise 2: Number Converter
Create a program that:
- Takes a number (int or float)
- Converts it to different types
- Prints the original and converted values

### Exercise 3: String Analyzer
Create a program that:
- Takes a string
- Analyzes its properties (length, case, etc.)
- Prints a detailed analysis

### Exercise 4: Optional Handler
Create a program that:
- Has a function that might return null
- Handles both the null and non-null cases
- Demonstrates safe optional handling

## Step 9: Common Type Errors

### Type Mismatch

```myco
let name = "Alice";
let age = 25;
let info = name + age;  # Error: can't add string and int
```

**Fix:**
```myco
let info = name + age.toString();  # Convert int to string
```

### Null Access

```myco
let name: String? = Null;
let length = name.length;  # Error: name might be null
```

**Fix:**
```myco
if name != Null:
    let length = name.length;  # Safe access
end
```

### Wrong Type Guard

```myco
let value = "hello";
if value.isInt():  # This will always be false
    print("It's an int");
end
```

**Fix:**
```myco
if value.isString():  # Correct guard
    print("It's a string");
end
```

## What's Next?

You've learned about Myco's type system! You now know how to:

- ✅ Work with different data types
- ✅ Use type inference and explicit annotations
- ✅ Handle optional types and null values
- ✅ Use type guards for runtime type checking
- ✅ Convert between different types
- ✅ Avoid common type-related errors

## Next Tutorial

In [Tutorial 3: Control Flow](03-control-flow.md), you'll learn about:

- If statements and conditionals
- Loops (for and while)
- Pattern matching
- Break and continue statements

## Additional Resources

- [Language Reference: Types](../language-reference/types.md) - Complete type system documentation
- [API Reference: Type Guards](../api-reference/) - All available type checking functions
- [Examples](https://github.com/ivymycelia/Myco/tree/main/examples) - More type-related examples

---

**Keep practicing!** Try the exercises and experiment with different types to solidify your understanding! 🚀
