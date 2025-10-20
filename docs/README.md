# Myco Language Documentation

Welcome to the official documentation for **Myco**, a modern, high-performance programming language designed for clarity, efficiency, and developer productivity.

## 🌟 What is Myco?

Myco is a bytecode-compiled programming language that combines the simplicity of modern syntax with the performance of compiled languages. Built with a focus on developer experience and runtime efficiency, Myco offers:

- **Clean, readable syntax** inspired by modern languages
- **Bytecode VM** for fast execution
- **Advanced optimization** with JIT compilation capabilities
- **Comprehensive standard library** with built-in data structures
- **Cross-platform compatibility** (macOS, Linux, Windows)
- **Memory safety** with automatic memory management
- **Type safety** with static typing and type inference

## 📚 Documentation Structure

### Getting Started
- [Installation Guide](getting-started/installation.md) - Install Myco on your system
- [Quick Start](getting-started/quick-start.md) - Your first Myco program
- [Language Tour](getting-started/language-tour.md) - Overview of Myco features

### Language Reference
- [Syntax Reference](language-reference/syntax.md) - Complete syntax documentation
- [Type System](language-reference/types.md) - Types, type checking, and type inference
- [Control Flow](language-reference/control-flow.md) - Conditionals, loops, and pattern matching
- [Functions](language-reference/functions.md) - Function definitions, calls, and closures
- [Classes & Objects](language-reference/classes.md) - Object-oriented programming
- [Modules](language-reference/modules.md) - Code organization and imports

### API Reference
- [Standard Library](api-reference/stdlib.md) - Built-in functions and types
- [Collections](api-reference/collections.md) - Arrays, Maps, Sets, and more
- [I/O Operations](api-reference/io.md) - File and network operations
- [Math & Utilities](api-reference/math.md) - Mathematical functions and utilities

### Tutorials
- [Tutorial 1: Hello World](tutorials/01-hello-world.md) - Your first program
- [Tutorial 2: Variables & Types](tutorials/02-variables-types.md) - Working with data
- [Tutorial 3: Control Flow](tutorials/03-control-flow.md) - Making decisions
- [Tutorial 4: Functions](tutorials/04-functions.md) - Reusable code
- [Tutorial 5: Collections](tutorials/05-collections.md) - Working with data structures
- [Tutorial 6: Classes & Objects](tutorials/06-classes-objects.md) - Object-oriented programming
- [Tutorial 7: Error Handling](tutorials/07-error-handling.md) - Robust error management
- [Tutorial 8: Advanced Features](tutorials/08-advanced-features.md) - Pattern matching, generics, and more

### Developer Guide
- [Contributing](developer-guide/contributing.md) - How to contribute to Myco
- [Building from Source](developer-guide/building.md) - Compile Myco from source
- [Architecture](developer-guide/architecture.md) - Understanding Myco's internals
- [Performance Guide](developer-guide/performance.md) - Writing efficient Myco code

## 🚀 Quick Links

- **Website**: [https://mycolang.org](https://mycolang.org)
- **GitHub**: [https://github.com/ivy-lang/Myco-Lang](https://github.com/ivy-lang/Myco-Lang)
- **Examples**: [View on GitHub](https://github.com/ivy-lang/Myco-Lang/tree/main/examples)
- **Issues**: [Report bugs](https://github.com/ivy-lang/Myco-Lang/issues)
- **Discussions**: [Community discussions](https://github.com/ivy-lang/Myco-Lang/discussions)

## 💡 Example: Your First Myco Program

```myco
# Hello World in Myco
print("Hello, World!");

# Variables and types
let name: String = "Myco";
let version: Float = 1.0;
let isAwesome: Boolean = True;

# Functions
func greet(person: String) -> String:
    return "Hello, " + person + "!";
end

# Use the function
let message = greet(name);
print(message);
```

## 🎯 Key Features

### Type System
- **Static typing** with type inference
- **Union types** for flexible data modeling
- **Optional types** for safe null handling
- **Type guards** for runtime type checking

### Performance
- **Bytecode compilation** for fast execution
- **JIT optimization** for hot code paths
- **Memory pooling** for efficient allocation
- **Branch prediction** hints for CPU optimization

### Standard Library
- **Collections**: Arrays, Maps, Sets, Trees, Graphs, Heaps, Queues, Stacks
- **I/O**: File operations, HTTP client, JSON parsing
- **Math**: Mathematical functions and constants
- **Utilities**: String manipulation, regular expressions, time handling

### Developer Experience
- **Clear error messages** with helpful suggestions
- **Comprehensive tooling** with LSP support
- **Cross-platform** compatibility
- **Memory safety** with automatic garbage collection

## 📖 Learn More

Ready to start coding in Myco? Check out our [Quick Start Guide](getting-started/quick-start.md) to write your first program, or dive into the [Language Tour](getting-started/language-tour.md) for a comprehensive overview of Myco's features.

---

**Myco** - *Where simplicity meets performance* 🚀
