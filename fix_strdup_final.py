#!/usr/bin/env python3
"""
Final strdup() safety fix script for Myco codebase.
This script properly fixes strdup() calls without creating double null checks.
"""

import os
import re
import sys
from pathlib import Path

def find_strdup_calls(file_path):
    """Find all strdup() calls in a file and return their details."""
    strdup_calls = []
    
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
            lines = content.split('\n')
            
        for line_num, line in enumerate(lines, 1):
            # Find strdup() calls
            strdup_pattern = r'strdup\s*\([^)]*\)'
            matches = re.finditer(strdup_pattern, line)
            
            for match in matches:
                strdup_call = match.group(0)
                strdup_calls.append({
                    'line_num': line_num,
                    'line_content': line,
                    'strdup_call': strdup_call,
                    'start_pos': match.start(),
                    'end_pos': match.end()
                })
    
    except Exception as e:
        print(f"Error reading {file_path}: {e}")
    
    return strdup_calls

def analyze_strdup_call(strdup_call):
    """Analyze a strdup() call to determine if it needs fixing."""
    # Extract the argument inside strdup()
    match = re.search(r'strdup\s*\(([^)]*)\)', strdup_call)
    if not match:
        return None
    
    argument = match.group(1).strip()
    
    # Check if it already has proper null safety (not double null checks)
    if '?' in argument and ':' in argument:
        # Check if it's already properly formatted
        if re.match(r'^\s*\w+\s*\?\s*strdup\s*\(\s*\w+\s*\)\s*:\s*NULL\s*$', argument):
            return {
                'needs_fix': False,
                'reason': 'Already has proper null check',
                'argument': argument
            }
        # Check if it has double null checks (needs fixing)
        elif re.search(r'\(\s*\w+\s*\?\s*\(\s*\w+\s*\?', argument):
            return {
                'needs_fix': True,
                'reason': 'Has double null check - needs simplification',
                'argument': argument,
                'fix_type': 'simplify'
            }
    
    # Check if it's a string literal
    if argument.startswith('"') and argument.endswith('"'):
        return {
            'needs_fix': False,
            'reason': 'String literal - safe',
            'argument': argument
        }
    
    # Check if it's a variable or expression that needs null check
    if not argument.startswith('"'):
        return {
            'needs_fix': True,
            'reason': 'Variable/expression - needs null check',
            'argument': argument,
            'fix_type': 'variable'
        }
    
    return {
        'needs_fix': False,
        'reason': 'Unknown pattern',
        'argument': argument
    }

def fix_strdup_call(line, strdup_call, analysis):
    """Fix a strdup() call based on its analysis."""
    if not analysis['needs_fix']:
        return line
    
    argument = analysis['argument']
    
    if analysis['fix_type'] == 'variable':
        # For variables, add null check
        safe_strdup = f"({argument} ? strdup({argument}) : NULL)"
        return line.replace(strdup_call, safe_strdup)
    
    elif analysis['fix_type'] == 'simplify':
        # Extract the variable name from the double null check
        match = re.search(r'\(\s*(\w+)\s*\?\s*\(\s*\1\s*\?', argument)
        if match:
            var_name = match.group(1)
            safe_strdup = f"({var_name} ? strdup({var_name}) : NULL)"
            return line.replace(strdup_call, safe_strdup)
    
    return line

def process_file(file_path):
    """Process a single file to fix strdup() calls."""
    print(f"Processing {file_path}...")
    
    strdup_calls = find_strdup_calls(file_path)
    if not strdup_calls:
        return False
    
    print(f"  Found {len(strdup_calls)} strdup() calls")
    
    # Read the file
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            lines = f.readlines()
    except Exception as e:
        print(f"  Error reading file: {e}")
        return False
    
    modified = False
    fixes_applied = 0
    
    for call_info in strdup_calls:
        line_num = call_info['line_num'] - 1  # Convert to 0-based index
        line = lines[line_num]
        strdup_call = call_info['strdup_call']
        
        analysis = analyze_strdup_call(strdup_call)
        if analysis and analysis['needs_fix']:
            print(f"  Line {call_info['line_num']}: {analysis['reason']}")
            print(f"    Before: {line.strip()}")
            
            fixed_line = fix_strdup_call(line, strdup_call, analysis)
            if fixed_line != line:
                lines[line_num] = fixed_line
                modified = True
                fixes_applied += 1
                print(f"    After:  {fixed_line.strip()}")
            else:
                print(f"    No change needed")
        else:
            if analysis:
                print(f"  Line {call_info['line_num']}: {analysis['reason']}")
    
    if modified:
        # Write the modified file
        try:
            with open(file_path, 'w', encoding='utf-8') as f:
                f.writelines(lines)
            print(f"  Applied {fixes_applied} fixes")
            return True
        except Exception as e:
            print(f"  Error writing file: {e}")
            return False
    else:
        print(f"  No fixes needed")
        return False

def main():
    """Main function to process all C files in the project."""
    project_root = Path(__file__).parent
    src_dir = project_root / "src"
    include_dir = project_root / "include"
    
    # Find all C files
    c_files = []
    for directory in [src_dir, include_dir]:
        if directory.exists():
            c_files.extend(directory.rglob("*.c"))
            c_files.extend(directory.rglob("*.h"))
    
    print(f"Found {len(c_files)} C files to process")
    
    total_files_modified = 0
    total_fixes = 0
    
    for file_path in c_files:
        if process_file(file_path):
            total_files_modified += 1
    
    print(f"\nSummary:")
    print(f"  Files processed: {len(c_files)}")
    print(f"  Files modified: {total_files_modified}")
    print(f"  Total fixes applied: {total_fixes}")

if __name__ == "__main__":
    main()
