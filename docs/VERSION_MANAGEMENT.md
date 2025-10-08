# Myco Version Management System

## Overview

Myco now has a comprehensive automated version management system that handles semantic versioning (SemVer) automatically. This system eliminates the need to manually edit version numbers and provides intelligent version bumping based on commit analysis.

## How It Works

### 1. **Manual Version Bumping**
You can manually bump versions using these commands:

```bash
# Bug fixes (2.0.0 → 2.0.1)
make version-patch

# New features (2.0.0 → 2.1.0)  
make version-minor

# Breaking changes (2.0.0 → 3.0.0)
make version-major
```

### 2. **Automatic Version Detection**
The system can automatically detect what type of version bump is needed:

```bash
# Auto-detect based on recent commits
make version-auto
```

**How Auto-Detection Works:**
- Analyzes recent git commits for keywords
- **Major bump** triggers: `breaking`, `major`, `incompatible`
- **Minor bump** triggers: `feat`, `feature`, `new`, `add`
- **Patch bump** (default): bug fixes, improvements

### 3. **Version Status**
Check current version and recent changes:

```bash
make version-status
```

## Version Types Explained

### **Patch Version (x.x.PATCH)**
- **When to use**: Bug fixes, small improvements, documentation updates
- **Examples**: `2.0.0 → 2.0.1`
- **Command**: `make version-patch`

### **Minor Version (x.MINOR.0)**
- **When to use**: New features, enhancements, new functionality
- **Examples**: `2.0.0 → 2.1.0`
- **Command**: `make version-minor`

### **Major Version (MAJOR.0.0)**
- **When to use**: Breaking changes, incompatible API changes
- **Examples**: `2.0.0 → 3.0.0`
- **Command**: `make version-major`

## What Gets Updated Automatically

When you run a version command, the system automatically:

1. **Updates `include/core/version.h`**:
   - `MYCO_VERSION_MAJOR`
   - `MYCO_VERSION_MINOR` 
   - `MYCO_VERSION_PATCH`
   - `MYCO_VERSION_STRING`
   - `MYCO_VERSION_FULL`

2. **Updates `CHANGELOG.md`**:
   - Adds new version entry with date
   - Includes version type and description

3. **Rebuilds the project**:
   - Version changes are compiled into the binary
   - `./bin/myco --version` shows new version

## Example Workflow

### **Scenario 1: Bug Fix**
```bash
# You fixed a bug in the parser
git add .
git commit -m "Fix parser infinite loop issue"

# Bump patch version
make version-patch
# Result: v2.0.0 → v2.0.1
```

### **Scenario 2: New Feature**
```bash
# You added a new language feature
git add .
git commit -m "Add pattern matching support"

# Bump minor version  
make version-minor
# Result: v2.0.0 → v2.1.0
```

### **Scenario 3: Breaking Change**
```bash
# You changed the API in a breaking way
git add .
git commit -m "Breaking: Change function signature for parse()"

# Bump major version
make version-major  
# Result: v2.0.0 → v3.0.0
```

### **Scenario 4: Auto-Detection**
```bash
# You made several commits with different types
git add .
git commit -m "Add new async/await feature"
git commit -m "Fix memory leak in interpreter"  
git commit -m "Breaking: Remove deprecated syntax"

# Let the system decide
make version-auto
# Result: Detects "Breaking" keyword → v3.0.0
```

## Advanced Usage

### **Custom Release Names**
You can specify custom release names:

```bash
python3 scripts/version_manager.py patch --name "Memory Optimizations"
```

### **Direct Script Usage**
Use the Python script directly for more control:

```bash
# Show current status
python3 scripts/version_manager.py status

# Manual version bumping
python3 scripts/version_manager.py patch
python3 scripts/version_manager.py minor  
python3 scripts/version_manager.py major

# Auto-detection
python3 scripts/version_manager.py auto
```

## Integration with Development

### **Pre-commit Hooks**
You can set up automatic version bumping:

```bash
# In your .git/hooks/pre-commit
#!/bin/bash
# Auto-bump patch version on every commit
make version-patch
```

### **CI/CD Integration**
For automated builds:

```bash
# In your CI pipeline
make version-auto
make
make test
```

## Benefits

1. **No Manual Version Editing**: Never edit version numbers manually
2. **Semantic Versioning**: Follows SemVer standards automatically  
3. **Intelligent Detection**: Analyzes commits to suggest appropriate bumps
4. **Changelog Generation**: Automatically updates CHANGELOG.md
5. **Build Integration**: Version changes are compiled into binaries
6. **Git Integration**: Works seamlessly with git workflows

## File Structure

```
scripts/
├── version_manager.py    # Main version management script
docs/
├── VERSION_MANAGEMENT.md # This documentation
include/core/
├── version.h            # Version constants (auto-updated)
CHANGELOG.md            # Version history (auto-updated)
Makefile               # Version management targets
```

## Troubleshooting

### **Version Not Updating**
- Make sure to rebuild after version changes: `make`
- Check that `include/core/version.h` was updated
- Verify the script has execute permissions: `chmod +x scripts/version_manager.py`

### **Auto-Detection Issues**
- The system looks for keywords in commit messages
- Use clear, descriptive commit messages
- Keywords are case-insensitive

### **Git Integration**
- Make sure you're in a git repository
- Recent commits are analyzed for version detection
- Use `git log --oneline -5` to see what commits will be analyzed

This automated version system makes Myco development much more streamlined and professional!
