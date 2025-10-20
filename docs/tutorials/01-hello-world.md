# Tutorial 1: Hello World

Welcome to your first Myco tutorial! In this tutorial, you'll learn how to write, compile, and run your first Myco program.

## What You'll Learn

- How to write a simple Myco program
- How to run Myco programs
- Basic Myco syntax
- How to use the Myco REPL (Read-Eval-Print Loop)

## Prerequisites

Before starting this tutorial, make sure you have:

1. **Myco installed** on your system (see [Installation Guide](../getting-started/installation.md))
2. **A text editor** (VS Code, Vim, Emacs, or any editor you prefer)
3. **Basic familiarity** with programming concepts (variables, functions, etc.)

## Step 1: Your First Program

Let's start with the classic "Hello, World!" program.

1. **Create a new file** called `hello.myco` in your preferred directory:

```myco
print("Hello, World!");
```

2. **Save the file** and make sure it has the `.myco` extension.

3. **Run the program** using the Myco interpreter:

```bash
myco hello.myco
```

You should see the output:

```
Hello, World!
```

🎉 **Congratulations!** You've just run your first Myco program!

## Step 2: Understanding the Code

Let's break down what happened:

- `print()` is a built-in function that outputs text to the console
- `"Hello, World!"` is a string literal (text enclosed in double quotes)
- The semicolon (`;`) marks the end of the statement
- Myco executed the program line by line and displayed the result

## Step 3: Adding More Output

Let's make our program more interesting by adding multiple print statements:

```myco
print("Hello, World!");
print("Welcome to Myco!");
print("This is my first program.");
```

Save and run the program again:

```bash
myco hello.myco
```

You should see:

```
Hello, World!
Welcome to Myco!
This is my first program.
```

## Step 4: Using Variables

Now let's learn about variables in Myco:

```myco
let message = "Hello, World!";
print(message);

let greeting = "Welcome to Myco!";
print(greeting);
```

**Key points about variables:**
- `let` declares a new variable
- `message` is the variable name
- `=` assigns a value to the variable
- Myco can infer the type automatically (this is a String)

Run the program:

```bash
myco hello.myco
```

## Step 5: String Concatenation

Let's combine strings together:

```myco
let name = "Alice";
let greeting = "Hello, " + name + "!";
print(greeting);

let age = 25;
let info = "I am " + age.toString() + " years old.";
print(info);
```

**Key points:**
- `+` concatenates (joins) strings
- `age.toString()` converts the number to a string
- You can mix strings and numbers

## Step 6: Using the REPL

Myco includes an interactive mode called the REPL (Read-Eval-Print Loop). This lets you experiment with code without creating files.

1. **Start the REPL:**

```bash
myco
```

You should see a prompt like this:

```
Myco REPL v1.0.0
Type 'exit' to quit
> 
```

2. **Try some expressions:**

```
> print("Hello from REPL!");
Hello from REPL!
> let x = 42;
> print(x);
42
> let y = x * 2;
> print(y);
84
> exit
```

**REPL features:**
- Type expressions and see results immediately
- Variables persist between commands
- Type `exit` to quit
- Use arrow keys to navigate command history

## Step 7: Comments

Add comments to your code to explain what it does:

```myco
# This is a single-line comment
print("Hello, World!");

/*
 * This is a multi-line comment
 * It can span multiple lines
 */
let name = "Alice";
let greeting = "Hello, " + name + "!";  # Inline comment
print(greeting);
```

**Comment types:**
- `#` for single-line comments
- `/* */` for multi-line comments
- Comments are ignored by the interpreter

## Step 8: A Complete Example

Let's create a more complete program that demonstrates what you've learned:

```myco
# My First Myco Program
# This program demonstrates basic Myco features

print("=== Welcome to Myco! ===");

# Variables
let name = "Alice";
let age = 25;
let city = "New York";

# String concatenation
let greeting = "Hello, " + name + "!";
print(greeting);

# Number to string conversion
let info = "I am " + age.toString() + " years old.";
print(info);

# More string operations
let location = "I live in " + city + ".";
print(location);

# Simple calculation
let nextAge = age + 1;
let birthday = "Next year I'll be " + nextAge.toString() + ".";
print(birthday);

print("=== End of Program ===");
```

Save this as `complete_hello.myco` and run it:

```bash
myco complete_hello.myco
```

Expected output:

```
=== Welcome to Myco! ===
Hello, Alice!
I am 25 years old.
I live in New York.
Next year I'll be 26.
=== End of Program ===
```

## Step 9: Common Errors

As you learn, you might encounter some errors. Here are common ones and how to fix them:

### Syntax Error

```myco
# Missing semicolon
print("Hello, World!")  # Error: missing semicolon
```

**Fix:** Add a semicolon at the end:
```myco
print("Hello, World!");  # Correct
```

### Undefined Variable

```myco
print(message);  # Error: 'message' is not defined
```

**Fix:** Declare the variable first:
```myco
let message = "Hello, World!";
print(message);  # Correct
```

### Type Error

```myco
let name = "Alice";
let result = name + 25;  # Error: can't add string and number
```

**Fix:** Convert the number to a string:
```myco
let name = "Alice";
let result = name + 25.toString();  # Correct
```

## Step 10: Practice Exercises

Try these exercises to reinforce what you've learned:

### Exercise 1: Personal Introduction
Create a program that introduces yourself with:
- Your name
- Your age
- Your favorite color
- Your hometown

### Exercise 2: Simple Calculator
Create a program that:
- Declares two numbers
- Adds them together
- Multiplies them
- Prints both results

### Exercise 3: Story Generator
Create a program that:
- Uses variables for character names and places
- Combines them into a short story
- Prints the complete story

## What's Next?

You've successfully completed your first Myco tutorial! You now know how to:

- ✅ Write and run Myco programs
- ✅ Use variables and basic data types
- ✅ Work with strings and numbers
- ✅ Use the Myco REPL
- ✅ Add comments to your code
- ✅ Handle common errors

## Next Tutorial

Ready for more? In [Tutorial 2: Variables & Types](../tutorials/02-variables-types.md), you'll learn about:

- Different data types in Myco
- Type annotations and inference
- Working with numbers and strings
- Type checking and conversion

## Additional Resources

- [Quick Start Guide](../getting-started/quick-start.md) - More examples and features
- [Language Tour](../getting-started/language-tour.md) - Comprehensive language overview
- [API Reference](../api-reference/) - Complete function reference
- [Examples](https://github.com/ivymycelia/Myco/tree/main/examples) - More code examples

---

**Keep coding!** The best way to learn Myco is by writing lots of programs. Try experimenting with the examples and see what happens! 🚀
