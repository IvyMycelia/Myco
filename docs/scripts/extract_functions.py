#!/usr/bin/env python3
"""
Extract function signatures and documentation from Myco C source files.
This script parses C files to find built-in functions and their signatures.
"""

import re
import os
import json
from pathlib import Path

def extract_function_signatures(file_path):
    """Extract function signatures from a C file."""
    functions = []
    
    with open(file_path, 'r') as f:
        content = f.read()
    
    # Pattern to match function definitions
    # Matches: Value function_name(parameters) { ... }
    pattern = r'Value\s+(\w+)\s*\([^)]*\)\s*\{'
    
    for match in re.finditer(pattern, content):
        func_name = match.group(1)
        
        # Skip internal functions
        if func_name.startswith('_') or func_name.startswith('builtin_'):
            continue
            
        # Extract the full function signature
        start = match.start()
        # Find the opening parenthesis
        paren_start = content.find('(', start)
        # Find the closing parenthesis
        paren_count = 0
        paren_end = paren_start
        for i, char in enumerate(content[paren_start:], paren_start):
            if char == '(':
                paren_count += 1
            elif char == ')':
                paren_count -= 1
                if paren_count == 0:
                    paren_end = i
                    break
        
        signature = content[start:paren_end + 1]
        
        # Extract parameters
        params_start = content.find('(', start) + 1
        params_end = paren_end
        params_text = content[params_start:params_end].strip()
        
        # Parse parameters
        params = []
        if params_text:
            for param in params_text.split(','):
                param = param.strip()
                if param:
                    # Extract parameter name (last word)
                    param_parts = param.split()
                    if param_parts:
                        param_name = param_parts[-1]
                        param_type = ' '.join(param_parts[:-1]) if len(param_parts) > 1 else 'Value'
                        params.append({
                            'name': param_name,
                            'type': param_type
                        })
        
        functions.append({
            'name': func_name,
            'signature': signature,
            'parameters': params,
            'file': os.path.basename(file_path)
        })
    
    return functions

def extract_builtin_functions():
    """Extract all built-in functions from the codebase."""
    all_functions = []
    
    # Directories to search for functions
    search_dirs = [
        'src/core/interpreter',
        'src/libs',
        'src/core'
    ]
    
    for search_dir in search_dirs:
        if os.path.exists(search_dir):
            for root, dirs, files in os.walk(search_dir):
                for file in files:
                    if file.endswith('.c'):
                        file_path = os.path.join(root, file)
                        functions = extract_function_signatures(file_path)
                        all_functions.extend(functions)
    
    return all_functions

def generate_api_reference(functions):
    """Generate API reference documentation."""
    # Group functions by category
    categories = {
        'Core Functions': [],
        'String Functions': [],
        'Array Functions': [],
        'Object Functions': [],
        'HTTP Server': [],
        'File Operations': [],
        'JSON Operations': [],
        'Math Functions': [],
        'Time Functions': [],
        'Debug Functions': []
    }
    
    for func in functions:
        name = func['name']
        
        # Categorize functions based on name patterns
        if name in ['print', 'len', 'type', 'toString']:
            categories['Core Functions'].append(func)
        elif 'string' in name.lower() or name.startswith('str_'):
            categories['String Functions'].append(func)
        elif 'array' in name.lower() or name.startswith('array_'):
            categories['Array Functions'].append(func)
        elif 'object' in name.lower() or name.startswith('obj_'):
            categories['Object Functions'].append(func)
        elif 'http' in name.lower() or 'server' in name.lower():
            categories['HTTP Server'].append(func)
        elif 'file' in name.lower() or name.startswith('file_'):
            categories['File Operations'].append(func)
        elif 'json' in name.lower():
            categories['JSON Operations'].append(func)
        elif 'math' in name.lower() or name.startswith('math_'):
            categories['Math Functions'].append(func)
        elif 'time' in name.lower() or name.startswith('time_'):
            categories['Time Functions'].append(func)
        elif 'debug' in name.lower() or name.startswith('debug_'):
            categories['Debug Functions'].append(func)
        else:
            categories['Core Functions'].append(func)
    
    # Generate markdown
    markdown = "# API Reference\n\n"
    markdown += "This document contains all built-in functions available in Myco.\n\n"
    
    for category, funcs in categories.items():
        if funcs:
            markdown += f"## {category}\n\n"
            
            for func in sorted(funcs, key=lambda x: x['name']):
                markdown += f"### {func['name']}()\n\n"
                markdown += f"**Signature:** `{func['signature']}`\n\n"
                
                if func['parameters']:
                    markdown += "**Parameters:**\n"
                    for param in func['parameters']:
                        markdown += f"- `{param['name']}` ({param['type']})\n"
                    markdown += "\n"
                
                markdown += f"**Source:** `{func['file']}`\n\n"
                markdown += "---\n\n"
    
    return markdown

def main():
    """Main function to generate API documentation."""
    print("Extracting function signatures from Myco source code...")
    
    functions = extract_builtin_functions()
    print(f"Found {len(functions)} functions")
    
    # Generate API reference
    api_ref = generate_api_reference(functions)
    
    # Write to file
    with open('docs/generated/API_REFERENCE.md', 'w') as f:
        f.write(api_ref)
    
    # Also save as JSON for other tools
    with open('docs/generated/functions.json', 'w') as f:
        json.dump(functions, f, indent=2)
    
    print("API reference generated: docs/generated/API_REFERENCE.md")
    print("Function data saved: docs/generated/functions.json")

if __name__ == "__main__":
    main()
