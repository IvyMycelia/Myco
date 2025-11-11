# Myco Language Syntax Reference

## Basic Syntax Rules

- **Comments**: Use `#` for single-line comments
- **Statements**: End with semicolon `;` (except in blocks)
- **Blocks**: Use `end` to close blocks (not braces)
- **Case sensitive**: `let` not `Let`, `func` not `Func`

## Variable Declarations

```myco
let variable_name = value;                 # variable
let typed_var: Type = value;               # variable with type annotation
```

## Data Types

```myco
let int: Int = 42;                         # Numbers (int)
let float: Float = 3.14;                   # Numbers (float)
let str: String = "hello";                 # Strings
let bool: Bool = True;                     # Booleans (True/False)
let arr: Array = [1, 2, 3];                # Arrays
let obj: Object = {key: "value"};          # Objects
let null_val = Null;                       # Null value
```

## Function Declarations

```myco
# Basic function
func function_name():
    return value;
end

# Function with parameters
func add(a, b):
    return a + b;
end

# Function with return type
func multiply(x, y) -> Int:
    return x * y;
end

# Function with typed parameters
func typed_add(x: Int, y: Int) -> Int:
    return x + y;
end

# Mixed parameter types
func mixed_func(name: String, age: Int) -> String:
    return name + " is", age.toString();
end
```

## Control Flow

```myco
# If statements
if condition:
    # code
else:
    # code
end

# While loops
while condition:
    # code
end

# For loops
for item in collection:
    # code
end
```

## Array Operations

```myco
let arr = [1, 2, 3];
arr.push(4);                                 # Add element: [1, 2, 3, 4]
let popped = arr.pop();                      # Remove last: 4
let len = arr.length;                        # Get length: 3
let joined = arr.join(", ");                 # Join elements: "1, 2, 3"
```

## Object Operations

```myco
let obj = {name: "Myco", version: 2.0};
let name = obj.name;                          # Access property
obj.version = 2.1;                            # Set property
let has_key = obj.has("name");                # Check property
```

## Method Calls

```myco
# Array methods
arr.push(value);                              # CORRECT: var.method()
arr.pop();                                    # CORRECT: var.method()
arr.join(separator);                          # CORRECT: var.method()

# NOT: array.push(arr, val) - this is WRONG
```

# Use Statements
ALL use statements must import either a path or built-in library. `as` and `from` are optional.

## Library Imports

```myco
use math as math;                             # Import math library
use file as file;                             # Import file library
use maps as maps;                             # Import maps library
use sets as sets;                             # Import sets library
use trees as trees;                           # Import trees library
use graphs as graphs;                         # Import graphs library
use time as time;                             # Import time library
use regex as regex;                           # Import regex library
```

## File Imports

```myco
use "./file.myco" as file;                                 # Import .myco file
use public_function from "./file.myco" as public_function; # Import an exported function from .myco file
```

## From-Use Syntax (Recommended for IDE Autocomplete)

The `from ... use ...` syntax places the module path first, making it easier for IDE plugins to provide autocomplete suggestions:

```myco
from "utils" use publicFunction;               # Import specific function from file
from "utils" use func1, func2;                # Import multiple items from file
from math use Pi, E;                          # Import specific constants from library
from "module.myco" use Class1, Class2;         # Import classes from module
```

## Class Declarations

```myco
class ClassName:
    let property = value;
    
    func method():
        return self.property;
    end
end

# Create instance
let instance = ClassName();
let result = instance.method();
```

## Built-in Functions

```myco
print(value);                                # Print to console
let input = input();                         # Get user input
let length = collection.length;              # Get length
let type_name = value.type;                  # Get type
let str_val = value.toString();              # Convert to string
let num_val = value.toInt();                 # Convert to integer
let bool_val = value.toBool();               # Convert to boolean
```

## String Operations

```myco
let str = "Hello World";
let upper = str.upper();                        # "HELLO WORLD"
let lower = str.lower();                        # "hello world"
let trimmed = str.trim();                       # Remove whitespace
let split = str.split(" ");                     # ["Hello", "World"]
let replaced = str.replace("World", "Myco");    # "Hello Myco"
```

## Common Patterns

```myco
# Array creation and manipulation
let arr = [];
arr.push(1);
arr.push(2);
arr.push(3);

# Object creation and manipulation
let obj = {};
obj.key = "value";
obj.another_key = 42;

# Function with array return
func create_array() -> Array:
    let result = [];
    result.push(1);
    result.push(2);
    return result;
end

# Library usage
use math as math;
let Pi = math.Pi;                       # 3.141592653589793
let max_val = math.Max(10, 20);         # 20
```

## Error Handling

```myco
# Check for errors
if interpreter.hasError():
    # Handle error
end

# Type checking
if value.type == "String":
    # Handle string
end
```

## Key Points to Remember

1. **Method calls**: Always use `var.method()`, never `library.method(var, ...)` or `built_in_method(var, ...)`
2. **Array methods**: `arr.push(val)`, `arr.pop()`, `arr.join(sep)`
3. **Function syntax**: `func name(params) -> ReturnType:` or `func name(params):` (You can mix and match the different syntaxes in functions)
4. **Block syntax**: Use `end` to close blocks, not `{}`
5. **Comments**: Use `#` not `//`
6. **Imports**: Use `use library as alias;` or `use library;` or `use method1, method2 from library;` or `use method1, method2 from library as alias1, alias2;` or `from "module" use item1, item2;` (recommended for IDE autocomplete)
7. **Types**: `Int`, `Float`, `String`, `Bool`, `Array`, `Object`
8. **Booleans**: `True` and `False` (capitalized)
9. **Null**: `Null` (capitalized)
10. **Semicolons**: Required at end of statements
11. **Use Statements**: Use `use library as alias;` or `use library;` or `use method1, method2 from library;` or `use method1, method2 from library as alias1, alias2;` or `from "module" use item1, item2;` (recommended for IDE autocomplete)
12. **Dot Methos vs. Dot Variables**: If a dot operator mutates, manipulates, or changes a variable, then it needs parenthesis. If it has parameters, then it needs paranthesis. If it is just a property (e.g. `arr.length`, `obj.keys`, `heap.size`, `array.type`), then it does not need parenthesis.


use library as alias; alias.type = "Library";                  # Import library
use "./file.myco" as mycoFile; file.type = "Import";           # Import file
