#!/usr/bin/env python3
"""
Master script to generate all documentation.
This script runs all documentation generation scripts in the correct order.
"""

import os
import sys
import subprocess
from pathlib import Path

def run_script(script_name):
    """Run a Python script and return success status."""
    script_path = f"docs/scripts/{script_name}"
    
    if not os.path.exists(script_path):
        print(f"Error: Script {script_path} not found")
        return False
    
    print(f"Running {script_name}...")
    
    try:
        result = subprocess.run([sys.executable, script_path], 
                              capture_output=True, text=True, cwd='.')
        
        if result.returncode == 0:
            print(f"✓ {script_name} completed successfully")
            if result.stdout:
                print(result.stdout)
            return True
        else:
            print(f"✗ {script_name} failed")
            if result.stderr:
                print(result.stderr)
            return False
    
    except Exception as e:
        print(f"✗ {script_name} failed with exception: {e}")
        return False

def generate_index():
    """Generate main documentation index."""
    markdown = "# Myco Documentation\n\n"
    markdown += "Welcome to the Myco programming language documentation.\n\n"
    
    markdown += "## Quick Start\n\n"
    markdown += "- [Language Specification](LANGUAGE_SPECIFICATION.md) - Complete language reference\n"
    markdown += "- [Syntax Reference](SYNTAX_REFERENCE.md) - Syntax guide and examples\n"
    markdown += "- [API Reference](API_REFERENCE.md) - Built-in functions and libraries\n"
    markdown += "- [Tutorial](TUTORIAL.md) - Step-by-step learning guide\n"
    markdown += "- [Examples](EXAMPLES.md) - Working code examples\n"
    markdown += "- [Changelog](CHANGELOG.md) - Version history\n\n"
    
    markdown += "## Getting Started\n\n"
    markdown += "1. **Install Myco** - Download and build from source\n"
    markdown += "2. **Read the Tutorial** - Learn the basics step by step\n"
    markdown += "3. **Try Examples** - Run the provided code examples\n"
    markdown += "4. **Reference the API** - Look up built-in functions\n\n"
    
    markdown += "## Language Features\n\n"
    markdown += "- **Static Typing** - Type safety with inference\n"
    markdown += "- **Object-Oriented** - Classes, inheritance, and polymorphism\n"
    markdown += "- **Functional** - First-class functions and lambdas\n"
    markdown += "- **Pattern Matching** - Powerful control flow\n"
    markdown += "- **HTTP Server** - Built-in web server capabilities\n"
    markdown += "- **JSON Support** - Native JSON parsing and generation\n"
    markdown += "- **File I/O** - Comprehensive file operations\n\n"
    
    markdown += "## Documentation Status\n\n"
    markdown += "This documentation is automatically generated from the Myco source code.\n"
    markdown += "All examples are tested and validated to ensure they work correctly.\n\n"
    
    return markdown

def main():
    """Main function to generate all documentation."""
    print("=== Myco Documentation Generator ===\n")
    
    # Ensure we're in the right directory
    if not os.path.exists('src') or not os.path.exists('include'):
        print("Error: Must be run from the Myco project root directory")
        sys.exit(1)
    
    # Create generated directory if it doesn't exist
    os.makedirs('docs/generated', exist_ok=True)
    
    # List of scripts to run in order
    scripts = [
        'extract_functions.py',
        'extract_syntax.py', 
        'generate_examples.py',
        'generate_changelog.py',
        'validate_examples.py'
    ]
    
    success_count = 0
    total_count = len(scripts)
    
    # Run each script
    for script in scripts:
        if run_script(script):
            success_count += 1
        print()  # Add spacing
    
    # Generate main index
    print("Generating main documentation index...")
    index_content = generate_index()
    
    with open('docs/generated/README.md', 'w') as f:
        f.write(index_content)
    
    print("✓ Main index generated")
    
    # Summary
    print(f"\n=== Generation Complete ===")
    print(f"Scripts run: {total_count}")
    print(f"Successful: {success_count}")
    print(f"Failed: {total_count - success_count}")
    
    if success_count == total_count:
        print("✓ All documentation generated successfully!")
    else:
        print("⚠ Some scripts failed. Check the output above for details.")
    
    print(f"\nGenerated files are in: docs/generated/")
    print(f"Main documentation: docs/generated/README.md")

if __name__ == "__main__":
    main()
