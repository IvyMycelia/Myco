#!/usr/bin/env python3
"""
Generate code examples from existing Myco files.
This script scans the codebase for example files and generates documentation.
"""

import os
import re
from pathlib import Path

def extract_examples_from_file(file_path):
    """Extract code examples from a Myco file."""
    examples = []
    
    with open(file_path, 'r') as f:
        content = f.read()
    
    # Split content into lines
    lines = content.split('\n')
    
    # Find example blocks (functions, classes, etc.)
    current_example = []
    in_example = False
    example_type = None
    
    for i, line in enumerate(lines):
        # Check for function definitions
        if re.match(r'^\s*func\s+\w+', line):
            if current_example:
                examples.append({
                    'type': example_type,
                    'code': '\n'.join(current_example),
                    'line': i - len(current_example) + 1
                })
            current_example = [line]
            in_example = True
            example_type = 'function'
        
        # Check for class definitions
        elif re.match(r'^\s*class\s+\w+', line):
            if current_example:
                examples.append({
                    'type': example_type,
                    'code': '\n'.join(current_example),
                    'line': i - len(current_example) + 1
                })
            current_example = [line]
            in_example = True
            example_type = 'class'
        
        # Check for variable declarations
        elif re.match(r'^\s*let\s+\w+', line):
            if current_example:
                examples.append({
                    'type': example_type,
                    'code': '\n'.join(current_example),
                    'line': i - len(current_example) + 1
                })
            current_example = [line]
            in_example = True
            example_type = 'variable'
        
        # Continue building example
        elif in_example:
            current_example.append(line)
            
            # Check for end of example
            if line.strip() == 'end;' or line.strip() == 'end':
                examples.append({
                    'type': example_type,
                    'code': '\n'.join(current_example),
                    'line': i - len(current_example) + 1
                })
                current_example = []
                in_example = False
                example_type = None
    
    # Add any remaining example
    if current_example:
        examples.append({
            'type': example_type,
            'code': '\n'.join(current_example),
            'line': len(lines) - len(current_example) + 1
        })
    
    return examples

def scan_example_files():
    """Scan for example files in the codebase."""
    example_files = []
    
    # Directories to search for examples
    search_dirs = [
        'examples',
        'tests',
        'benchmark',
        'codesoc_website'
    ]
    
    for search_dir in search_dirs:
        if os.path.exists(search_dir):
            for root, dirs, files in os.walk(search_dir):
                for file in files:
                    if file.endswith('.myco'):
                        file_path = os.path.join(root, file)
                        example_files.append(file_path)
    
    return example_files

def generate_examples_documentation(example_files):
    """Generate examples documentation from files."""
    markdown = "# Myco Examples\n\n"
    markdown += "This document contains working examples of Myco code.\n\n"
    
    # Basic examples
    markdown += "## Basic Examples\n\n"
    
    # Hello World
    markdown += "### Hello World\n\n"
    markdown += "```myco\n"
    markdown += "print(\"Hello, World!\");\n"
    markdown += "```\n\n"
    
    # Variables
    markdown += "### Variables and Types\n\n"
    markdown += "```myco\n"
    markdown += "let name = \"Myco\";\n"
    markdown += "let version = 1.0;\n"
    markdown += "let is_awesome = true;\n"
    markdown += "\n"
    markdown += "print(\"Language: \" + name);\n"
    markdown += "print(\"Version: \" + version.toString());\n"
    markdown += "print(\"Awesome: \" + is_awesome.toString());\n"
    markdown += "```\n\n"
    
    # Functions
    markdown += "### Functions\n\n"
    markdown += "```myco\n"
    markdown += "func greet(name):\n"
    markdown += "    return \"Hello, \" + name + \"!\";\n"
    markdown += "end;\n"
    markdown += "\n"
    markdown += "print(greet(\"World\"));\n"
    markdown += "```\n\n"
    
    # Arrays
    markdown += "### Arrays\n\n"
    markdown += "```myco\n"
    markdown += "let numbers = [1, 2, 3, 4, 5];\n"
    markdown += "let fruits = [\"apple\", \"banana\", \"orange\"];\n"
    markdown += "\n"
    markdown += "print(\"First number: \" + numbers[0].toString());\n"
    markdown += "print(\"First fruit: \" + fruits[0]);\n"
    markdown += "print(\"Array length: \" + numbers.length.toString());\n"
    markdown += "```\n\n"
    
    # Objects
    markdown += "### Objects\n\n"
    markdown += "```myco\n"
    markdown += "let person = {\n"
    markdown += "    name: \"Alice\",\n"
    markdown += "    age: 30,\n"
    markdown += "    city: \"New York\"\n"
    markdown += "};\n"
    markdown += "\n"
    markdown += "print(\"Name: \" + person.name);\n"
    markdown += "print(\"Age: \" + person.age.toString());\n"
    markdown += "```\n\n"
    
    # Control Flow
    markdown += "### Control Flow\n\n"
    markdown += "```myco\n"
    markdown += "let score = 85;\n"
    markdown += "\n"
    markdown += "if score >= 90:\n"
    markdown += "    print(\"Grade: A\");\n"
    markdown += "elif score >= 80:\n"
    markdown += "    print(\"Grade: B\");\n"
    markdown += "elif score >= 70:\n"
    markdown += "    print(\"Grade: C\");\n"
    markdown += "else:\n"
    markdown += "    print(\"Grade: F\");\n"
    markdown += "end;\n"
    markdown += "```\n\n"
    
    # Loops
    markdown += "### Loops\n\n"
    markdown += "```myco\n"
    markdown += "# While loop\n"
    markdown += "let i = 0;\n"
    markdown += "while i < 5:\n"
    markdown += "    print(\"Count: \" + i.toString());\n"
    markdown += "    i = i + 1;\n"
    markdown += "end;\n"
    markdown += "\n"
    markdown += "# For loop\n"
    markdown += "for let j = 0; j < 3; j++:\n"
    markdown += "    print(\"For count: \" + j.toString());\n"
    markdown += "end;\n"
    markdown += "```\n\n"
    
    # Intermediate examples
    markdown += "## Intermediate Examples\n\n"
    
    # Classes
    markdown += "### Classes\n\n"
    markdown += "```myco\n"
    markdown += "class Person:\n"
    markdown += "    func constructor(name, age):\n"
    markdown += "        self.name = name;\n"
    markdown += "        self.age = age;\n"
    markdown += "    end;\n"
    markdown += "    \n"
    markdown += "    func greet():\n"
    markdown += "        return \"Hello, I'm \" + self.name;\n"
    markdown += "    end;\n"
    markdown += "end;\n"
    markdown += "\n"
    markdown += "let alice = Person(\"Alice\", 30);\n"
    markdown += "print(alice.greet());\n"
    markdown += "```\n\n"
    
    # File I/O
    markdown += "### File Operations\n\n"
    markdown += "```myco\n"
    markdown += "# Read file\n"
    markdown += "let content = file.read(\"data.txt\");\n"
    markdown += "print(\"File content: \" + content);\n"
    markdown += "\n"
    markdown += "# Write file\n"
    markdown += "file.write(\"output.txt\", \"Hello from Myco!\");\n"
    markdown += "```\n\n"
    
    # JSON
    markdown += "### JSON Operations\n\n"
    markdown += "```myco\n"
    markdown += "let data = {\n"
    markdown += "    name: \"Myco\",\n"
    markdown += "    version: 1.0,\n"
    markdown += "    features: [\"fast\", \"safe\", \"simple\"]\n"
    markdown += "};\n"
    markdown += "\n"
    markdown += "let json_string = json.stringify(data);\n"
    markdown += "print(\"JSON: \" + json_string);\n"
    markdown += "\n"
    markdown += "let parsed_data = json.parse(json_string);\n"
    markdown += "print(\"Parsed name: \" + parsed_data.name);\n"
    markdown += "```\n\n"
    
    # Advanced examples
    markdown += "## Advanced Examples\n\n"
    
    # HTTP Server
    markdown += "### HTTP Server\n\n"
    markdown += "```myco\n"
    markdown += "let app = server.create(3000);\n"
    markdown += "\n"
    markdown += "func handle_request(req, res):\n"
    markdown += "    let response = {\n"
    markdown += "        message: \"Hello from Myco!\",\n"
    markdown += "        timestamp: time.now()\n"
    markdown += "    };\n"
    markdown += "    set_response_body(json.stringify(response));\n"
    markdown += "    set_response_status(200);\n"
    markdown += "end;\n"
    markdown += "\n"
    markdown += "app.get(\"/api/hello\", handle_request);\n"
    markdown += "app.listen();\n"
    markdown += "```\n\n"
    
    # Error Handling
    markdown += "### Error Handling\n\n"
    markdown += "```myco\n"
    markdown += "func safe_divide(a, b):\n"
    markdown += "    if b == 0:\n"
    markdown += "        return null;\n"
    markdown += "    end;\n"
    markdown += "    return a / b;\n"
    markdown += "end;\n"
    markdown += "\n"
    markdown += "let result = safe_divide(10, 2);\n"
    markdown += "if result != null:\n"
    markdown += "    print(\"Result: \" + result.toString());\n"
    markdown += "else:\n"
    markdown += "    print(\"Division by zero!\");\n"
    markdown += "end;\n"
    markdown += "```\n\n"
    
    return markdown

def main():
    """Main function to generate examples documentation."""
    print("Scanning for example files...")
    
    example_files = scan_example_files()
    print(f"Found {len(example_files)} example files")
    
    # Generate examples documentation
    examples_doc = generate_examples_documentation(example_files)
    
    # Write to file
    with open('docs/generated/EXAMPLES.md', 'w') as f:
        f.write(examples_doc)
    
    print("Examples documentation generated: docs/generated/EXAMPLES.md")

if __name__ == "__main__":
    main()
