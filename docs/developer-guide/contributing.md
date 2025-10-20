# Contributing to Myco

Thank you for your interest in contributing to Myco! This guide will help you get started with contributing to the project.

## Table of Contents

1. [Code of Conduct](#code-of-conduct)
2. [Getting Started](#getting-started)
3. [Development Setup](#development-setup)
4. [Contributing Guidelines](#contributing-guidelines)
5. [Pull Request Process](#pull-request-process)
6. [Issue Reporting](#issue-reporting)
7. [Development Workflow](#development-workflow)
8. [Testing](#testing)
9. [Documentation](#documentation)
10. [Release Process](#release-process)

## Code of Conduct

This project follows the [Contributor Covenant Code of Conduct](CODE_OF_CONDUCT.md). By participating, you agree to uphold this code.

## Getting Started

### Prerequisites

- **C Compiler**: GCC 7+ or Clang 6+
- **Make**: GNU Make 4.0+
- **Git**: For version control
- **Python 3.6+**: For build scripts and tools

### Fork and Clone

1. **Fork the repository** on GitHub
2. **Clone your fork**:
   ```bash
   git clone https://github.com/YOUR_USERNAME/Myco-Lang.git
   cd Myco-Lang
   ```

3. **Add upstream remote**:
   ```bash
   git remote add upstream https://github.com/ivy-lang/Myco-Lang.git
   ```

## Development Setup

### Building from Source

```bash
# Clone the repository
git clone https://github.com/ivy-lang/Myco-Lang.git
cd Myco-Lang

# Build Myco
make

# Run tests
myco pass.myco

# Install (optional)
sudo make install
```

### Development Dependencies

```bash
# Install development tools
make dev-deps

# Run development server
make dev-server
```

### IDE Setup

#### Visual Studio Code

1. Install the Myco extension
2. Open the project folder
3. Configure C/C++ IntelliSense for the `src/` directory

#### Other Editors

- **Vim/Neovim**: Use LSP with the Myco language server
- **Emacs**: Use LSP mode with the Myco language server
- **Sublime Text**: Use LSP package with Myco support

## Contributing Guidelines

### Types of Contributions

We welcome contributions in many areas:

- **Bug fixes**: Fix issues in the issue tracker
- **Features**: Implement new language features
- **Documentation**: Improve docs, tutorials, and examples
- **Tests**: Add test cases and improve coverage
- **Performance**: Optimize existing code
- **Tools**: Improve build system, tooling, and developer experience

### Contribution Process

1. **Check existing issues** and pull requests
2. **Create an issue** for significant changes
3. **Fork and create a branch** for your changes
4. **Make your changes** following our coding standards
5. **Add tests** for new functionality
6. **Update documentation** as needed
7. **Submit a pull request**

### Coding Standards

#### C Code Style

```c
// Use 4 spaces for indentation
// Use snake_case for function and variable names
// Use UPPER_CASE for constants
// Use PascalCase for type names

// Function declarations
int myco_function_name(int parameter1, char* parameter2);

// Type definitions
typedef struct MycoType {
    int field1;
    char* field2;
} MycoType;

// Constants
#define MYCO_CONSTANT 42
```

#### Myco Code Style

```myco
# Use 4 spaces for indentation
# Use camelCase for variable names
# Use PascalCase for class names
# Use snake_case for function names

# Function declarations
func myco_function_name(parameter1: Int, parameter2: String) -> String:
    return "Hello, " + parameter2;
end

# Class definitions
class MycoClass:
    let field1: Int
    let field2: String
    
    func init(field1: Int, field2: String):
        self.field1 = field1;
        self.field2 = field2;
    end
end
```

### Commit Message Format

Use conventional commits format:

```
type(scope): description

[optional body]

[optional footer]
```

**Types:**
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `style`: Code style changes
- `refactor`: Code refactoring
- `test`: Test changes
- `chore`: Build process or auxiliary tool changes

**Examples:**
```
feat(parser): add support for pattern matching
fix(vm): resolve memory leak in bytecode execution
docs(api): update standard library reference
test(arrays): add tests for array operations
```

## Pull Request Process

### Before Submitting

1. **Run tests**:
   ```bash
   myco pass.myco
   make test
   ```

2. **Check code style**:
   ```bash
   make lint
   make format
   ```

3. **Update documentation** if needed

4. **Add tests** for new functionality

### Pull Request Template

```markdown
## Description
Brief description of changes

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Breaking change
- [ ] Documentation update

## Testing
- [ ] Tests pass locally
- [ ] New tests added for new functionality
- [ ] All existing tests still pass

## Checklist
- [ ] Code follows project style guidelines
- [ ] Self-review completed
- [ ] Documentation updated
- [ ] No new warnings introduced
```

### Review Process

1. **Automated checks** must pass
2. **Code review** by maintainers
3. **Testing** on multiple platforms
4. **Documentation** review if applicable

## Issue Reporting

### Bug Reports

Use the bug report template:

```markdown
**Describe the bug**
A clear description of what the bug is.

**To Reproduce**
Steps to reproduce the behavior:
1. Go to '...'
2. Click on '....'
3. See error

**Expected behavior**
What you expected to happen.

**Screenshots**
If applicable, add screenshots.

**Environment:**
- OS: [e.g. macOS, Linux, Windows]
- Myco version: [e.g. 1.0.0]
- Compiler: [e.g. GCC 9.3.0]

**Additional context**
Add any other context about the problem.
```

### Feature Requests

Use the feature request template:

```markdown
**Is your feature request related to a problem?**
A clear description of what the problem is.

**Describe the solution you'd like**
A clear description of what you want to happen.

**Describe alternatives you've considered**
A clear description of any alternative solutions.

**Additional context**
Add any other context or screenshots.
```

## Development Workflow

### Branch Naming

- `feature/description` - New features
- `fix/description` - Bug fixes
- `docs/description` - Documentation changes
- `refactor/description` - Code refactoring
- `test/description` - Test improvements

### Development Cycle

1. **Create issue** for significant changes
2. **Create branch** from main
3. **Make changes** with frequent commits
4. **Test changes** thoroughly
5. **Update documentation** if needed
6. **Submit pull request**
7. **Address review feedback**
8. **Merge** after approval

### Continuous Integration

All pull requests are automatically tested:

- **Build verification** on multiple platforms
- **Test suite execution** with full coverage
- **Code style checking** with automated formatting
- **Performance regression** testing
- **Memory leak detection**

## Testing

### Running Tests

```bash
# Run all tests
myco pass.myco

# Run specific test categories
myco tests/arithmetic.myco
myco tests/strings.myco
myco tests/collections.myco

# Run with verbose output
myco --verbose pass.myco

# Run with debug information
myco --debug pass.myco
```

### Adding Tests

1. **Create test file** in appropriate directory
2. **Follow naming convention**: `test_feature.myco`
3. **Use descriptive test names**
4. **Include edge cases and error conditions**
5. **Add to test suite** in `pass.myco`

### Test Structure

```myco
# Test file: tests/test_arrays.myco
print("Testing array operations...");

# Test 1: Basic array creation
let arr = [1, 2, 3];
if arr.length != 3:
    print("ERROR: Array length should be 3");
    exit(1);
end
print("✓ Array creation test passed");

# Test 2: Array access
if arr[0] != 1:
    print("ERROR: First element should be 1");
    exit(1);
end
print("✓ Array access test passed");

print("All array tests passed!");
```

## Documentation

### Documentation Structure

```
docs/
├── getting-started/     # Installation and quick start
├── tutorials/          # Step-by-step learning guides
├── language-reference/ # Complete language documentation
├── api-reference/      # Standard library reference
└── developer-guide/    # Development and contribution guides
```

### Writing Documentation

1. **Use clear, concise language**
2. **Include code examples** for all features
3. **Keep examples up-to-date** with the codebase
4. **Use consistent formatting** and style
5. **Test all code examples** before committing

### Documentation Standards

- **Markdown format** for all documentation
- **Code blocks** with language specification
- **Consistent heading structure**
- **Cross-references** between related topics
- **Regular updates** with code changes

## Release Process

### Version Numbering

We use [Semantic Versioning](https://semver.org/):

- **MAJOR**: Incompatible API changes
- **MINOR**: New functionality in a backwards compatible manner
- **PATCH**: Backwards compatible bug fixes

### Release Checklist

1. **Update version numbers** in all relevant files
2. **Update CHANGELOG.md** with new features and fixes
3. **Run full test suite** to ensure everything works
4. **Update documentation** for new features
5. **Create release tag** with proper version number
6. **Build and test** release binaries
7. **Publish release** on GitHub
8. **Update website** and documentation

### Release Commands

```bash
# Update version
make version VERSION=1.1.0

# Create release
make release

# Publish release
make publish
```

## Getting Help

### Community Resources

- **GitHub Discussions**: [Community discussions](https://github.com/ivymycelia/Myco/discussions)
- **Discord**: [Join our Discord server](https://discord.gg/myco)
- **Email**: [Contact maintainers](mailto:ivymycelia@gmail.com)

### Development Resources

- **Architecture Guide**: [Understanding Myco's internals](architecture.md)
- **Performance Guide**: [Writing efficient Myco code](performance.md)
- **API Reference**: [Complete function reference](../api-reference/)

## Recognition

Contributors are recognized in:

- **CONTRIBUTORS.md** file
- **Release notes** for significant contributions
- **GitHub contributor graph**
- **Project documentation**

---

Thank you for contributing to Myco! Your contributions help make Myco better for everyone.
