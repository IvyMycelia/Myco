# Myco Syntax Reference

This document provides a complete reference for Myco's syntax, including all language constructs, operators, and special forms.

## Table of Contents

1. [Comments](#comments)
2. [Identifiers](#identifiers)
3. [Literals](#literals)
4. [Types](#types)
5. [Variables](#variables)
6. [Operators](#operators)
7. [Expressions](#expressions)
8. [Statements](#statements)
9. [Control Flow](#control-flow)
10. [Functions](#functions)
11. [Classes](#classes)
12. [Pattern Matching](#pattern-matching)
13. [Modules](#modules)
14. [Precedence](#precedence)

## Comments

### Single-line Comments

```myco
# This is a single-line comment
let x = 42;  # Comments can be at the end of lines
```

### Multi-line Comments

```myco
/*
 * This is a multi-line comment
 * It can span multiple lines
 * and is useful for documentation
 */
```

## Identifiers

Identifiers are used for variable names, function names, class names, etc.

### Rules

- Must start with a letter or underscore
- Can contain letters, digits, and underscores
- Case-sensitive
- Cannot be reserved keywords

### Examples

```myco
# Valid identifiers
let name = "Alice";
let _private = 42;
let user123 = "valid";
let camelCase = True;
let snake_case = False;

# Invalid identifiers
# let 123invalid = "error";     # Cannot start with digit
# let if = "error";             # Cannot be keyword
# let my-name = "error";        # Cannot contain hyphens
```

### Reserved Keywords

```myco
# Control flow
if, else, while, for, break, continue, match, case

# Functions and classes
func, class, extends, return, self

# Types
Int, Float, String, Boolean, Null, Array, Map, Set

# Variables
let, const, var

# Operators
and, or, not, in, is

# Other
use, end, True, False, Null
```

## Literals

### Integer Literals

```myco
let decimal = 42;        # Decimal
let hex = 0x2A;          # Hexadecimal
let octal = 0o52;        # Octal
let binary = 0b101010;   # Binary
```

### Float Literals

```myco
let pi = 3.14159;        # Standard notation
let scientific = 1.23e4; # Scientific notation
let negative = -2.5;     # Negative
```

### String Literals

```myco
let single = "Hello, World!";           # Double quotes
let multi = "Line 1\nLine 2";           # With escape sequences
let raw = r"C:\Users\Name";             # Raw string (no escaping)
let template = "Hello, ${name}!";       # String interpolation (planned)
```

### Escape Sequences

```myco
let text = "Line 1\nLine 2\tTabbed\r\n";
# \n - newline
# \t - tab
# \r - carriage return
# \\ - backslash
# \" - double quote
# \' - single quote
```

### Boolean Literals

```myco
let trueValue = True;
let falseValue = False;
```

### Null Literal

```myco
let nothing = Null;
```

### Array Literals

```myco
let empty = [];                          # Empty array
let numbers = [1, 2, 3, 4, 5];           # Array of integers
let mixed = [1, "hello", True, 3.14];    # Mixed types
let nested = [[1, 2], [3, 4]];           # Nested arrays
```

### Map Literals

```myco
let empty = {};                          # Empty map
let person = {                           # Map with values
    name: "Alice",
    age: 30,
    city: "New York"
};
let nested = {                           # Nested maps
    user: {
        name: "Bob",
        settings: {
            theme: "dark",
            notifications: True
        }
    }
};
```

### Set Literals

```myco
let empty = {};                          # Empty set
let colors = {"red", "green", "blue"};   # Set of strings
let numbers = {1, 2, 3, 4, 5};           # Set of integers
```

## Types

### Basic Types

```myco
# Primitive types
let intValue: Int = 42;
let floatValue: Float = 3.14;
let stringValue: String = "hello";
let boolValue: Boolean = True;
let nullValue: Null = Null;
```

### Optional Types

```myco
# Optional String (can be String or Null)
let name: String? = "Alice";
let middleName: String? = Null;
```

### Union Types

```myco
# String or Int
let id: String | Int = "user123";
id = 456;  # Now it's an Int

# Multiple types
let value: String | Int | Boolean = "hello";
value = 42;
value = True;
```

### Array Types

```myco
# Array of integers
let numbers: Array<Int> = [1, 2, 3];

# Array of strings
let names: Array<String> = ["Alice", "Bob"];

# Mixed array (union type)
let mixed: Array<String | Int> = ["hello", 42];
```

### Map Types

```myco
# Map with string keys and any values
let data: Map<String, Any> = {
    name: "Alice",
    age: 30
};

# Map with specific types
let person: Map<String, String | Int> = {
    name: "Alice",
    age: 30
};
```

## Variables

### Variable Declaration

```myco
# Explicit type
let name: String = "Alice";

# Type inference
let age = 30;  # Inferred as Int

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

# print(local);     # Error: local is not in scope
```

## Operators

### Arithmetic Operators

```myco
let a = 10, b = 3;

a + b;   # Addition: 13
a - b;   # Subtraction: 7
a * b;   # Multiplication: 30
a / b;   # Division: 3.333...
a % b;   # Modulo: 1
a ** b;  # Exponentiation: 1000
```

### Comparison Operators

```myco
let a = 5, b = 10;

a == b;   # Equal: False
a != b;   # Not equal: True
a < b;    # Less than: True
a > b;    # Greater than: False
a <= b;   # Less than or equal: True
a >= b;   # Greater than or equal: False
```

### Logical Operators

```myco
let a = True, b = False;

a and b;  # Logical AND: False
a or b;   # Logical OR: True
not a;    # Logical NOT: False
```

### String Operators

```myco
let str1 = "Hello", str2 = "World";

str1 + str2;        # Concatenation: "HelloWorld"
str1 + " " + str2;  # "Hello World"
```

### Array Operators

```myco
let arr1 = [1, 2], arr2 = [3, 4];

arr1 + arr2;        # Concatenation: [1, 2, 3, 4]
arr1 * 3;           # Repetition: [1, 2, 1, 2, 1, 2]
```

### Set Operators

```myco
let set1 = {1, 2, 3}, set2 = {3, 4, 5};

set1 + set2;        # Union: {1, 2, 3, 4, 5}
set1 - set2;        # Difference: {1, 2}
set1 & set2;        # Intersection: {3}
```

### Type Checking Operators

```myco
let value = "hello";

value is String;    # Type check: True
value is Int;       # Type check: False
```

### Membership Operators

```myco
let arr = [1, 2, 3];
let set = {1, 2, 3};
let map = {a: 1, b: 2};

2 in arr;           # Array membership: True
4 in arr;           # Array membership: False
2 in set;           # Set membership: True
"a" in map;         # Map key membership: True
```

## Expressions

### Primary Expressions

```myco
# Literals
42;
"hello";
True;

# Identifiers
variable;

# Parenthesized expressions
(2 + 3) * 4;
```

### Member Access

```myco
# Dot notation
object.property;
object.method();

# Bracket notation
array[index];
map[key];
```

### Function Calls

```myco
# No arguments
func();

# With arguments
add(5, 3);
print("Hello, World!");

# Method calls
array.push(42);
string.upper("hello");
```

### Array/Map Access

```myco
# Array access
let arr = [1, 2, 3];
arr[0];     # First element
arr[1];     # Second element

# Map access
let map = {name: "Alice", age: 30};
map["name"];    # Bracket notation
map.name;       # Dot notation
```

## Statements

### Expression Statements

```myco
print("Hello, World!");
x = 42;
func();
```

### Variable Declaration

```myco
let name = "Alice";
const PI = 3.14159;
```

### Assignment

```myco
x = 42;
name = "Bob";
array[0] = 10;
map["key"] = "value";
```

### Return Statement

```myco
func add(a: Int, b: Int) -> Int:
    return a + b;
end
```

## Control Flow

### If Statements

```myco
# Basic if
if condition:
    statement;
end

# If-else
if condition:
    statement1;
else:
    statement2;
end

# If-else-if
if condition1:
    statement1;
else if condition2:
    statement2;
else:
    statement3;
end
```

### While Loops

```myco
# Basic while
while condition:
    statement;
end

# With break
while True:
    if condition:
        break;
    end
    statement;
end
```

### For Loops

```myco
# Range-based for
for i in range(1, 6):
    print(i);
end

# Array iteration
for item in array:
    print(item);
end

# With index
for i in range(0, array.length):
    print(i, ":", array[i]);
end
```

### Break and Continue

```myco
# Break
for i in range(1, 10):
    if i == 5:
        break;  # Exit the loop
    end
    print(i);
end

# Continue
for i in range(1, 6):
    if i == 3:
        continue;  # Skip this iteration
    end
    print(i);
end
```

## Functions

### Function Declaration

```myco
# No parameters, no return
func greet():
    print("Hello!");
end

# With parameters
func add(a: Int, b: Int) -> Int:
    return a + b;
end

# With default parameters
func greet(name: String, greeting: String = "Hello") -> String:
    return greeting + ", " + name + "!";
end
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
```

### Lambda Functions

```myco
# Anonymous functions
let square = func(x: Int) -> Int: return x * x; end;
let add = func(a: Int, b: Int) -> Int: return a + b; end;

# Lambda as parameter
func applyOperation(x: Int, op: (Int) -> Int) -> Int:
    return op(x);
end
```

## Classes

### Class Declaration

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
end
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
end
```

### Object Creation

```myco
let person = Person("Alice", 30);
let dog = Dog("Buddy");
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
```

### Type Pattern Matching

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

## Modules

### Module Import

```myco
# Import entire module
use math;
use string;
use file;

# Import specific functions
use math {sqrt, pow, pi};
use string {upper, lower, trim};
```

### Module Usage

```myco
use math;

let result = math.sqrt(16);  # 4.0
let power = math.pow(2, 3);  # 8.0
```

## Precedence

Operator precedence (from highest to lowest):

1. **Primary**: `()`, `[]`, `.`, function calls
2. **Unary**: `not`, `-`, `+`
3. **Exponentiation**: `**`
4. **Multiplicative**: `*`, `/`, `%`
5. **Additive**: `+`, `-`
6. **Relational**: `<`, `>`, `<=`, `>=`
7. **Equality**: `==`, `!=`
8. **Logical AND**: `and`
9. **Logical OR**: `or`
10. **Assignment**: `=`

### Examples

```myco
# Precedence examples
2 + 3 * 4;        # 14 (not 20)
(2 + 3) * 4;      # 20
not True and False;  # False
not (True and False);  # True
```

---

This syntax reference covers all the language constructs in Myco. For more detailed examples and usage patterns, see the [tutorials](../tutorials/) and [language tour](../getting-started/language-tour.md).
