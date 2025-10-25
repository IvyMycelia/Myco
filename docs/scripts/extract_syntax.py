#!/usr/bin/env python3
"""
Extract syntax patterns from Myco source code and generate syntax reference.
This script parses the lexer and parser to understand the language syntax.
"""

import re
import os
import json
from pathlib import Path

def extract_tokens(file_path):
    """Extract token definitions from lexer.c"""
    tokens = []
    
    with open(file_path, 'r') as f:
        content = f.read()
    
    # Pattern to match token definitions
    # Matches: TOKEN_NAME = value,
    pattern = r'TOKEN_(\w+)\s*=\s*(\d+)'
    
    for match in re.finditer(pattern, content):
        token_name = match.group(1)
        token_value = match.group(2)
        tokens.append({
            'name': token_name,
            'value': int(token_value)
        })
    
    return tokens

def extract_ast_nodes(file_path):
    """Extract AST node types from ast.h"""
    nodes = []
    
    with open(file_path, 'r') as f:
        content = f.read()
    
    # Pattern to match AST node definitions
    # Matches: AST_NODE_NAME,
    pattern = r'AST_NODE_(\w+)'
    
    for match in re.finditer(pattern, content):
        node_name = match.group(1)
        nodes.append(node_name)
    
    return nodes

def extract_keywords(file_path):
    """Extract keywords from lexer.c"""
    keywords = []
    
    with open(file_path, 'r') as f:
        content = f.read()
    
    # Look for keyword definitions
    # Pattern: "keyword" -> TOKEN_KEYWORD
    pattern = r'"(\w+)"\s*->\s*TOKEN_(\w+)'
    
    for match in re.finditer(pattern, content):
        keyword = match.group(1)
        token = match.group(2)
        keywords.append({
            'keyword': keyword,
            'token': token
        })
    
    return keywords

def generate_syntax_reference(tokens, nodes, keywords):
    """Generate syntax reference documentation."""
    markdown = "# Myco Syntax Reference\n\n"
    markdown += "This document provides a comprehensive reference for Myco syntax.\n\n"
    
    # Keywords section
    markdown += "## Keywords\n\n"
    markdown += "Myco has the following reserved keywords:\n\n"
    
    for kw in sorted(keywords, key=lambda x: x['keyword']):
        markdown += f"- `{kw['keyword']}`\n"
    
    markdown += "\n"
    
    # Data types section
    markdown += "## Data Types\n\n"
    markdown += "### Primitive Types\n\n"
    markdown += "- `Number` - Integer and floating-point numbers\n"
    markdown += "- `String` - Text data\n"
    markdown += "- `Boolean` - True or false values\n"
    markdown += "- `Null` - Empty or undefined values\n\n"
    
    markdown += "### Composite Types\n\n"
    markdown += "- `Array` - Ordered collections of values\n"
    markdown += "- `Object` - Key-value pairs\n"
    markdown += "- `Function` - Callable code blocks\n"
    markdown += "- `Class` - User-defined types\n\n"
    
    # Operators section
    markdown += "## Operators\n\n"
    markdown += "### Arithmetic Operators\n\n"
    markdown += "- `+` - Addition\n"
    markdown += "- `-` - Subtraction\n"
    markdown += "- `*` - Multiplication\n"
    markdown += "- `/` - Division\n"
    markdown += "- `%` - Modulo\n\n"
    
    markdown += "### Comparison Operators\n\n"
    markdown += "- `==` - Equality\n"
    markdown += "- `!=` - Inequality\n"
    markdown += "- `<` - Less than\n"
    markdown += "- `>` - Greater than\n"
    markdown += "- `<=` - Less than or equal\n"
    markdown += "- `>=` - Greater than or equal\n\n"
    
    markdown += "### Logical Operators\n\n"
    markdown += "- `and` - Logical AND\n"
    markdown += "- `or` - Logical OR\n"
    markdown += "- `not` - Logical NOT\n\n"
    
    # Control structures section
    markdown += "## Control Structures\n\n"
    markdown += "### If Statements\n\n"
    markdown += "```myco\n"
    markdown += "if condition:\n"
    markdown += "    # code block\n"
    markdown += "elif other_condition:\n"
    markdown += "    # code block\n"
    markdown += "else:\n"
    markdown += "    # code block\n"
    markdown += "end;\n"
    markdown += "```\n\n"
    
    markdown += "### While Loops\n\n"
    markdown += "```myco\n"
    markdown += "while condition:\n"
    markdown += "    # code block\n"
    markdown += "end;\n"
    markdown += "```\n\n"
    
    markdown += "### For Loops\n\n"
    markdown += "```myco\n"
    markdown += "for let i = 0; i < 10; i++:\n"
    markdown += "    # code block\n"
    markdown += "end;\n"
    markdown += "```\n\n"
    
    # Functions section
    markdown += "## Functions\n\n"
    markdown += "### Function Declaration\n\n"
    markdown += "```myco\n"
    markdown += "func function_name(param1, param2):\n"
    markdown += "    # function body\n"
    markdown += "    return value;\n"
    markdown += "end;\n"
    markdown += "```\n\n"
    
    markdown += "### Lambda Functions\n\n"
    markdown += "```myco\n"
    markdown += "let lambda = func(x): x * 2; end;\n"
    markdown += "```\n\n"
    
    # Classes section
    markdown += "## Classes\n\n"
    markdown += "### Class Declaration\n\n"
    markdown += "```myco\n"
    markdown += "class ClassName:\n"
    markdown += "    func constructor(param):\n"
    markdown += "        self.property = param;\n"
    markdown += "    end;\n"
    markdown += "    \n"
    markdown += "    func method():\n"
    markdown += "        return self.property;\n"
    markdown += "    end;\n"
    markdown += "end;\n"
    markdown += "```\n\n"
    
    # Variables section
    markdown += "## Variables\n\n"
    markdown += "### Variable Declaration\n\n"
    markdown += "```myco\n"
    markdown += "let variable_name = value;\n"
    markdown += "let mutable_var = value;\n"
    markdown += "```\n\n"
    
    markdown += "### Constants\n\n"
    markdown += "```myco\n"
    markdown += "const CONSTANT_NAME = value;\n"
    markdown += "```\n\n"
    
    return markdown

def main():
    """Main function to generate syntax documentation."""
    print("Extracting syntax information from Myco source code...")
    
    # Extract tokens
    tokens = []
    if os.path.exists('src/core/lexer.c'):
        tokens = extract_tokens('src/core/lexer.c')
        print(f"Found {len(tokens)} tokens")
    
    # Extract AST nodes
    nodes = []
    if os.path.exists('include/core/ast.h'):
        nodes = extract_ast_nodes('include/core/ast.h')
        print(f"Found {len(nodes)} AST node types")
    
    # Extract keywords
    keywords = []
    if os.path.exists('src/core/lexer.c'):
        keywords = extract_keywords('src/core/lexer.c')
        print(f"Found {len(keywords)} keywords")
    
    # Generate syntax reference
    syntax_ref = generate_syntax_reference(tokens, nodes, keywords)
    
    # Write to file
    with open('docs/generated/SYNTAX_REFERENCE.md', 'w') as f:
        f.write(syntax_ref)
    
    # Save data as JSON
    data = {
        'tokens': tokens,
        'nodes': nodes,
        'keywords': keywords
    }
    
    with open('docs/generated/syntax_data.json', 'w') as f:
        json.dump(data, f, indent=2)
    
    print("Syntax reference generated: docs/generated/SYNTAX_REFERENCE.md")
    print("Syntax data saved: docs/generated/syntax_data.json")

if __name__ == "__main__":
    main()
