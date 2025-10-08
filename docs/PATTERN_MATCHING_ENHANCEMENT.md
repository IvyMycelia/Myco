# Pattern Matching Enhancement Plan

## Current State Analysis

Myco currently has basic pattern matching through:
- **Match statements**: Simple equality matching
- **Spore statements**: Pattern matching with lambda and block syntax
- **Basic patterns**: Strings, numbers, null values

## Enhancement Goals

### 1. **Advanced Pattern Types**
- **Destructuring patterns**: Extract values from arrays and objects
- **Type patterns**: Match based on value types
- **Guard patterns**: Add conditions to patterns
- **Wildcard patterns**: Match anything with `_`
- **Range patterns**: Match numeric ranges
- **Regex patterns**: Match using regular expressions

### 2. **Pattern Combinators**
- **Or patterns**: `pattern1 | pattern2`
- **And patterns**: `pattern1 & pattern2` 
- **Not patterns**: `!pattern`
- **Optional patterns**: `pattern?`

### 3. **Destructuring Support**
- **Array destructuring**: `[a, b, c]` or `[head, ...tail]`
- **Object destructuring**: `{name, age}` or `{name: n, age: a}`
- **Nested destructuring**: `[{name}, {age}]`

### 4. **Guard Clauses**
- **When conditions**: `pattern when condition`
- **Type guards**: `value when value > 0`
- **Complex guards**: `pattern when pattern.length > 5`

## Implementation Plan

### Phase 1: Enhanced Pattern Types
1. Add new AST node types for advanced patterns
2. Extend parser to handle new pattern syntax
3. Implement pattern matching logic in interpreter
4. Add comprehensive tests

### Phase 2: Destructuring Support
1. Implement array destructuring patterns
2. Implement object destructuring patterns
3. Add variable binding in patterns
4. Handle nested destructuring

### Phase 3: Guard Clauses
1. Add `when` keyword support
2. Implement guard condition evaluation
3. Combine patterns with guards
4. Add guard-specific error handling

### Phase 4: Pattern Combinators
1. Implement `|` (or) patterns
2. Implement `&` (and) patterns
3. Implement `!` (not) patterns
4. Add precedence handling

## Syntax Examples

### Enhanced Match Statements
```myco
// Type patterns
match value:
    case Int:
        print("Integer: " + str(value));
    case String:
        print("String: " + value);
    case Array:
        print("Array with " + str(value.length) + " elements");
    else:
        print("Other type");
end

// Destructuring patterns
match person:
    case {name: n, age: a} when a >= 18:
        print("Adult: " + n);
    case {name: n, age: a} when a < 18:
        print("Minor: " + n);
    else:
        print("Invalid person");
end

// Array destructuring
match numbers:
    case [first, second, ...rest]:
        print("First: " + str(first) + ", Second: " + str(second));
        print("Rest: " + str(rest.length) + " elements");
    case [single]:
        print("Single element: " + str(single));
    case []:
        print("Empty array");
    else:
        print("Other array");
end

// Pattern combinators
match value:
    case Int | Float when value > 0:
        print("Positive number: " + str(value));
    case String when value.length > 5:
        print("Long string: " + value);
    case Array when value.length == 0:
        print("Empty array");
    else:
        print("No match");
end

// Regex patterns
match text:
    case /^\d+$/:
        print("All digits: " + text);
    case /^[a-zA-Z]+$/:
        print("All letters: " + text);
    case /^\w+@\w+\.\w+$/:
        print("Email: " + text);
    else:
        print("No pattern match");
end
```

### Enhanced Spore Statements
```myco
// Lambda style with advanced patterns
let result = spore person:
    case {name: n, age: a} when a >= 18 => "Adult: " + n
    case {name: n, age: a} when a < 18 => "Minor: " + n
    case {name: n} => "Name only: " + n
    root => "Unknown person"
end

// Block style with destructuring
spore data:
    case [x, y, z]:
        print("3D point: (" + str(x) + ", " + str(y) + ", " + str(z) + ")");
    case [x, y]:
        print("2D point: (" + str(x) + ", " + str(y) + ")");
    case [x]:
        print("1D point: " + str(x));
    root:
        print("Invalid point data");
end
```

## Benefits

1. **More Expressive**: Rich pattern matching capabilities
2. **Type Safety**: Pattern matching with type checking
3. **Destructuring**: Easy extraction of nested values
4. **Guards**: Conditional pattern matching
5. **Combinators**: Flexible pattern composition
6. **Performance**: Optimized pattern matching
7. **Developer Experience**: Intuitive and powerful syntax

This enhancement will make Myco's pattern matching competitive with modern functional languages while maintaining simplicity and performance.
