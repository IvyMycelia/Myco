#!/usr/bin/env python3
"""
Validate all code examples in documentation.
This script tests all Myco code examples to ensure they work correctly.
"""

import os
import subprocess
import tempfile
import re
from pathlib import Path

def extract_code_blocks(file_path):
    """Extract code blocks from markdown files."""
    code_blocks = []
    
    with open(file_path, 'r') as f:
        content = f.read()
    
    # Pattern to match code blocks
    pattern = r'```myco\n(.*?)\n```'
    
    for match in re.finditer(pattern, content, re.DOTALL):
        code = match.group(1).strip()
        if code:
            code_blocks.append({
                'code': code,
                'file': file_path,
                'line': content[:match.start()].count('\n') + 1
            })
    
    return code_blocks

def test_myco_code(code, timeout=10):
    """Test a Myco code snippet."""
    try:
        # Create temporary file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.myco', delete=False) as f:
            f.write(code)
            temp_file = f.name
        
        # Run Myco on the temporary file
        cmd = ['../bin/myco', temp_file]
        result = subprocess.run(
            cmd,
            capture_output=True,
            text=True,
            timeout=timeout,
            cwd='codesoc_website'  # Change to appropriate directory
        )
        
        # Clean up
        os.unlink(temp_file)
        
        return {
            'success': result.returncode == 0,
            'stdout': result.stdout,
            'stderr': result.stderr,
            'returncode': result.returncode
        }
    
    except subprocess.TimeoutExpired:
        return {
            'success': False,
            'stdout': '',
            'stderr': 'Timeout expired',
            'returncode': -1
        }
    except Exception as e:
        return {
            'success': False,
            'stdout': '',
            'stderr': str(e),
            'returncode': -1
        }

def validate_examples():
    """Validate all examples in documentation."""
    results = []
    
    # Find all markdown files with examples
    markdown_files = []
    for root, dirs, files in os.walk('docs'):
        for file in files:
            if file.endswith('.md'):
                markdown_files.append(os.path.join(root, file))
    
    print(f"Found {len(markdown_files)} markdown files")
    
    for file_path in markdown_files:
        print(f"Processing {file_path}...")
        
        # Extract code blocks
        code_blocks = extract_code_blocks(file_path)
        print(f"  Found {len(code_blocks)} code blocks")
        
        for i, block in enumerate(code_blocks):
            print(f"  Testing code block {i + 1}...")
            
            # Test the code
            result = test_myco_code(block['code'])
            
            results.append({
                'file': file_path,
                'block': i + 1,
                'line': block['line'],
                'code': block['code'],
                'result': result
            })
            
            if result['success']:
                print(f"    ✓ Success")
            else:
                print(f"    ✗ Failed: {result['stderr']}")
    
    return results

def generate_validation_report(results):
    """Generate validation report."""
    total = len(results)
    successful = sum(1 for r in results if r['result']['success'])
    failed = total - successful
    
    markdown = "# Example Validation Report\n\n"
    markdown += f"**Total Examples:** {total}\n"
    markdown += f"**Successful:** {successful}\n"
    markdown += f"**Failed:** {failed}\n"
    markdown += f"**Success Rate:** {(successful/total*100):.1f}%\n\n"
    
    if failed > 0:
        markdown += "## Failed Examples\n\n"
        
        for result in results:
            if not result['result']['success']:
                markdown += f"### {result['file']} (Block {result['block']})\n\n"
                markdown += f"**Line:** {result['line']}\n\n"
                markdown += f"**Error:** {result['result']['stderr']}\n\n"
                markdown += "**Code:**\n```myco\n"
                markdown += result['code']
                markdown += "\n```\n\n"
    
    return markdown

def main():
    """Main function to validate examples."""
    print("Validating all code examples...")
    
    # Change to the correct directory
    os.chdir('/Users/Ivy/Documents/GitHub/Myco Lang')
    
    # Validate examples
    results = validate_examples()
    
    # Generate report
    report = generate_validation_report(results)
    
    # Write report
    with open('docs/generated/VALIDATION_REPORT.md', 'w') as f:
        f.write(report)
    
    # Save detailed results as JSON
    import json
    with open('docs/generated/validation_results.json', 'w') as f:
        json.dump(results, f, indent=2)
    
    print(f"\nValidation complete!")
    print(f"Total examples: {len(results)}")
    print(f"Successful: {sum(1 for r in results if r['result']['success'])}")
    print(f"Failed: {sum(1 for r in results if not r['result']['success'])}")
    print("Report saved: docs/generated/VALIDATION_REPORT.md")

if __name__ == "__main__":
    main()
