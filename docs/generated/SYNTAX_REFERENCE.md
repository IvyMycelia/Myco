# Myco Syntax Reference

This document provides a comprehensive reference for Myco syntax.

## Keywords

Myco has the following reserved keywords:


## Data Types

### Primitive Types

- `Number` - Integer and floating-point numbers
- `String` - Text data
- `Boolean` - True or false values
- `Null` - Empty or undefined values

### Composite Types

- `Array` - Ordered collections of values
- `Object` - Key-value pairs
- `Function` - Callable code blocks
- `Class` - User-defined types

## Operators

### Arithmetic Operators

- `+` - Addition
- `-` - Subtraction
- `*` - Multiplication
- `/` - Division
- `%` - Modulo

### Comparison Operators

- `==` - Equality
- `!=` - Inequality
- `<` - Less than
- `>` - Greater than
- `<=` - Less than or equal
- `>=` - Greater than or equal

### Logical Operators

- `and` - Logical AND
- `or` - Logical OR
- `not` - Logical NOT

## Control Structures

### If Statements

```myco
if condition:
    # code block
elif other_condition:
    # code block
else:
    # code block
end;
```

### While Loops

```myco
while condition:
    # code block
end;
```

### For Loops

```myco
for let i = 0; i < 10; i++:
    # code block
end;
```

## Functions

### Function Declaration

```myco
func function_name(param1, param2):
    # function body
    return value;
end;
```

### Lambda Functions

```myco
let lambda = func(x): x * 2; end;
```

## Classes

### Class Declaration

```myco
class ClassName:
    func constructor(param):
        self.property = param;
    end;
    
    func method():
        return self.property;
    end;
end;
```

## Variables

### Variable Declaration

```myco
let variable_name = value;
let mutable_var = value;
```

### Constants

```myco
const CONSTANT_NAME = value;
```

