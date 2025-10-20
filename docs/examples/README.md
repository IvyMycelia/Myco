# Myco Examples

This directory contains examples demonstrating various features and use cases of the Myco programming language.

## Table of Contents

1. [Basic Examples](#basic-examples)
2. [Language Features](#language-features)
3. [Standard Library](#standard-library)
4. [Advanced Examples](#advanced-examples)
5. [Platform-Specific](#platform-specific)
6. [Performance Examples](#performance-examples)

## Basic Examples

### Hello World
- **[hello_world.myco](hello_world.myco)** - Your first Myco program
- **[variables.myco](variables.myco)** - Working with variables and types
- **[functions.myco](functions.myco)** - Function definitions and calls

### Control Flow
- **[conditionals.myco](conditionals.myco)** - If statements and conditionals
- **[loops.myco](loops.myco)** - For and while loops
- **[pattern_matching.myco](pattern_matching.myco)** - Pattern matching examples

## Language Features

### Object-Oriented Programming
- **[classes.myco](classes.myco)** - Basic class definitions
- **[inheritance.myco](inheritance.myco)** - Class inheritance
- **[polymorphism.myco](polymorphism.myco)** - Method overriding and polymorphism

### Advanced Types
- **[union_types.myco](union_types.myco)** - Union types and type guards
- **[optional_types.myco](optional_types.myco)** - Optional types and null handling
- **[generics.myco](generics.myco)** - Generic programming (planned feature)

### Error Handling
- **[error_handling.myco](error_handling.myco)** - Safe error handling patterns
- **[validation.myco](validation.myco)** - Input validation and sanitization

## Standard Library

### Collections
- **[arrays.myco](arrays.myco)** - Array operations and methods
- **[maps.myco](maps.myco)** - Map (dictionary) operations
- **[sets.myco](sets.myco)** - Set operations and methods
- **[advanced_collections.myco](advanced_collections.myco)** - Trees, graphs, heaps, queues, stacks

### I/O Operations
- **[file_operations.myco](file_operations.myco)** - File reading and writing
- **[directory_operations.myco](directory_operations.myco)** - Directory manipulation
- **[http_client.myco](http_client.myco)** - HTTP requests and responses

### Data Processing
- **[json_processing.myco](json_processing.myco)** - JSON parsing and generation
- **[string_processing.myco](string_processing.myco)** - String manipulation and regex
- **[math_operations.myco](math_operations.myco)** - Mathematical functions and operations

## Advanced Examples

### Algorithms
- **[sorting.myco](sorting.myco)** - Various sorting algorithms
- **[searching.myco](searching.myco)** - Search algorithms
- **[graph_algorithms.myco](graph_algorithms.myco)** - Graph traversal and algorithms

### Data Structures
- **[linked_list.myco](linked_list.myco)** - Custom linked list implementation
- **[binary_tree.myco](binary_tree.myco)** - Binary tree operations
- **[hash_table.myco](hash_table.myco)** - Hash table implementation

### Concurrency (Planned)
- **[async_operations.myco](async_operations.myco)** - Asynchronous programming
- **[parallel_processing.myco](parallel_processing.myco)** - Parallel data processing

## Platform-Specific

### Web Development
- **[web_app.myco](web_app.myco)** - Simple web application
- **[api_server.myco](api_server.myco)** - REST API server
- **[static_site.myco](static_site.myco)** - Static site generator

### Embedded Systems
- **[arduino_blink.myco](arduino_blink.myco)** - Arduino LED blinking
- **[sensor_reading.myco](sensor_reading.myco)** - Sensor data collection
- **[motor_control.myco](motor_control.myco)** - Motor control system

### Desktop Applications
- **[cli_tool.myco](cli_tool.myco)** - Command-line interface tool
- **[file_manager.myco](file_manager.myco)** - Simple file manager
- **[calculator.myco](calculator.myco)** - Calculator application

## Performance Examples

### Benchmarking
- **[performance_test.myco](performance_test.myco)** - Performance testing framework
- **[memory_usage.myco](memory_usage.myco)** - Memory usage analysis
- **[cpu_intensive.myco](cpu_intensive.myco)** - CPU-intensive operations

### Optimization
- **[optimized_algorithms.myco](optimized_algorithms.myco)** - Optimized algorithm implementations
- **[cache_friendly.myco](cache_friendly.myco)** - Cache-friendly data access patterns
- **[memory_pooling.myco](memory_pooling.myco)** - Memory pooling examples

## Running Examples

### Basic Execution

```bash
# Run a single example
myco examples/hello_world.myco

# Run with debug output
myco --debug examples/functions.myco

# Run with verbose output
myco --verbose examples/arrays.myco
```

### Interactive Mode

```bash
# Start REPL and load example
myco
> load("examples/hello_world.myco")
> run()
```

### Batch Execution

```bash
# Run all basic examples
for file in examples/basic_*.myco; do
    echo "Running $file..."
    myco "$file"
done
```

## Example Categories

### Beginner Examples
Perfect for those new to Myco:
- `hello_world.myco`
- `variables.myco`
- `functions.myco`
- `conditionals.myco`
- `loops.myco`

### Intermediate Examples
For those familiar with basic concepts:
- `classes.myco`
- `inheritance.myco`
- `arrays.myco`
- `maps.myco`
- `file_operations.myco`

### Advanced Examples
For experienced developers:
- `advanced_collections.myco`
- `graph_algorithms.myco`
- `performance_test.myco`
- `web_app.myco`
- `optimized_algorithms.myco`

## Contributing Examples

We welcome contributions of new examples! Please follow these guidelines:

1. **Use descriptive names** for example files
2. **Include comments** explaining the code
3. **Test all examples** before submitting
4. **Follow coding standards** outlined in the contributing guide
5. **Add to appropriate category** in this README

### Example Template

```myco
# Example: [Feature Name]
# Description: [Brief description of what this example demonstrates]
# Difficulty: [Beginner/Intermediate/Advanced]
# Prerequisites: [Any required knowledge]

print("=== [Feature Name] Example ===");

# Your example code here
# with clear comments explaining
# what each part does

print("Example completed successfully!");
```

## Learning Path

### Recommended Learning Order

1. **Start with basics**: `hello_world.myco` → `variables.myco` → `functions.myco`
2. **Learn control flow**: `conditionals.myco` → `loops.myco` → `pattern_matching.myco`
3. **Explore collections**: `arrays.myco` → `maps.myco` → `sets.myco`
4. **Try OOP**: `classes.myco` → `inheritance.myco` → `polymorphism.myco`
5. **Advanced features**: `union_types.myco` → `error_handling.myco` → `advanced_collections.myco`
6. **Platform-specific**: Choose based on your interests (web, embedded, desktop)
7. **Performance**: `performance_test.myco` → `optimized_algorithms.myco`

### Practice Exercises

After running examples, try these exercises:

1. **Modify existing examples** to add new features
2. **Combine multiple examples** to create more complex programs
3. **Create your own examples** based on the templates
4. **Optimize examples** for better performance
5. **Add error handling** to examples that don't have it

## Troubleshooting

### Common Issues

**Example won't run:**
- Check that Myco is properly installed
- Verify the example file exists and is readable
- Look for syntax errors in the example

**Unexpected output:**
- Check if the example requires specific input
- Verify that all dependencies are available
- Run with `--debug` flag for more information

**Performance issues:**
- Some examples are intentionally CPU-intensive
- Use `--profile` flag to analyze performance
- Check system resources (memory, CPU)

### Getting Help

- **GitHub Issues**: [Report problems](https://github.com/ivymycelia/Myco/issues)
- **Discussions**: [Ask questions](https://github.com/ivymycelia/Myco/discussions)
- **Documentation**: [Language reference](https://mycolang.org/docs)

---

**Happy coding!** These examples should help you learn Myco and build amazing applications. 🚀
