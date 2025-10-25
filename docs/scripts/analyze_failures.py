#!/usr/bin/env python3
"""
Analyze validation failures to identify common patterns and issues.
This script helps improve the documentation by understanding why examples fail.
"""

import json
import re
from collections import Counter

def analyze_validation_results():
    """Analyze validation results to identify common failure patterns."""
    
    # Load validation results
    with open('docs/generated/validation_results.json', 'r') as f:
        results = json.load(f)
    
    # Analyze failures
    failed_examples = [r for r in results if not r['result']['success']]
    successful_examples = [r for r in results if r['result']['success']]
    
    print(f"=== Validation Analysis ===")
    print(f"Total examples: {len(results)}")
    print(f"Successful: {len(successful_examples)}")
    print(f"Failed: {len(failed_examples)}")
    print(f"Success rate: {len(successful_examples)/len(results)*100:.1f}%")
    print()
    
    # Analyze error patterns
    error_patterns = Counter()
    timeout_count = 0
    empty_error_count = 0
    
    for result in failed_examples:
        error = result['result']['stderr']
        
        if 'Timeout expired' in error:
            timeout_count += 1
        elif not error.strip():
            empty_error_count += 1
        else:
            # Extract error patterns
            if 'E3005' in error:
                error_patterns['Argument count mismatch'] += 1
            elif 'E3004' in error:
                error_patterns['Type mismatch'] += 1
            elif 'E5001' in error:
                error_patterns['Server function error'] += 1
            elif 'E9008' in error:
                error_patterns['Method not found'] += 1
            elif 'requires exactly' in error:
                error_patterns['Function signature mismatch'] += 1
            elif 'must be a' in error:
                error_patterns['Type requirement error'] += 1
            else:
                error_patterns['Other errors'] += 1
    
    print("=== Error Pattern Analysis ===")
    print(f"Timeouts: {timeout_count}")
    print(f"Empty errors: {empty_error_count}")
    print()
    
    print("Common error patterns:")
    for pattern, count in error_patterns.most_common():
        print(f"  {pattern}: {count}")
    print()
    
    # Analyze by file
    file_failures = Counter()
    for result in failed_examples:
        file_failures[result['file']] += 1
    
    print("=== Files with Most Failures ===")
    for file, count in file_failures.most_common(10):
        print(f"  {file}: {count} failures")
    print()
    
    # Analyze by code pattern
    code_patterns = Counter()
    for result in failed_examples:
        code = result['code']
        
        # Identify common problematic patterns
        if 'server.create' in code:
            code_patterns['Server examples'] += 1
        elif 'class ' in code:
            code_patterns['Class examples'] += 1
        elif 'func ' in code:
            code_patterns['Function examples'] += 1
        elif 'while ' in code:
            code_patterns['Loop examples'] += 1
        elif 'if ' in code:
            code_patterns['Conditional examples'] += 1
        elif 'let ' in code:
            code_patterns['Variable examples'] += 1
    
    print("=== Failure Patterns by Code Type ===")
    for pattern, count in code_patterns.most_common():
        print(f"  {pattern}: {count} failures")
    print()
    
    # Generate improvement recommendations
    print("=== Improvement Recommendations ===")
    
    if timeout_count > 0:
        print(f"1. {timeout_count} examples timed out - consider reducing complexity or adding timeouts")
    
    if empty_error_count > 0:
        print(f"2. {empty_error_count} examples had empty errors - investigate execution issues")
    
    if error_patterns['Argument count mismatch'] > 0:
        print(f"3. {error_patterns['Argument count mismatch']} examples have argument count issues - update function signatures")
    
    if error_patterns['Type mismatch'] > 0:
        print(f"4. {error_patterns['Type mismatch']} examples have type issues - fix type requirements")
    
    if error_patterns['Server function error'] > 0:
        print(f"5. {error_patterns['Server function error']} server examples failed - fix server API")
    
    print()
    
    # Generate fix suggestions
    print("=== Specific Fix Suggestions ===")
    
    # Look for specific error patterns
    for result in failed_examples[:10]:  # Show first 10 failures
        error = result['result']['stderr']
        if error.strip():
            print(f"File: {result['file']}")
            print(f"Error: {error}")
            print(f"Code: {result['code'][:100]}...")
            print()
    
    return {
        'total': len(results),
        'successful': len(successful_examples),
        'failed': len(failed_examples),
        'error_patterns': dict(error_patterns),
        'timeout_count': timeout_count,
        'empty_error_count': empty_error_count
    }

def generate_improvement_plan(analysis):
    """Generate a plan to improve documentation examples."""
    
    plan = "# Documentation Improvement Plan\n\n"
    plan += f"Based on validation analysis of {analysis['total']} examples.\n\n"
    
    plan += "## Current Status\n\n"
    plan += f"- **Total Examples:** {analysis['total']}\n"
    plan += f"- **Successful:** {analysis['successful']}\n"
    plan += f"- **Failed:** {analysis['failed']}\n"
    plan += f"- **Success Rate:** {analysis['successful']/analysis['total']*100:.1f}%\n\n"
    
    plan += "## Priority Fixes\n\n"
    
    if analysis['timeout_count'] > 0:
        plan += f"### 1. Fix Timeout Issues ({analysis['timeout_count']} examples)\n"
        plan += "- Reduce complexity of examples\n"
        plan += "- Add proper timeout handling\n"
        plan += "- Break down complex examples into smaller parts\n\n"
    
    if analysis['empty_error_count'] > 0:
        plan += f"### 2. Fix Empty Error Issues ({analysis['empty_error_count']} examples)\n"
        plan += "- Investigate why examples produce no output\n"
        plan += "- Check for syntax errors in examples\n"
        plan += "- Ensure examples are complete and runnable\n\n"
    
    if analysis['error_patterns'].get('Argument count mismatch', 0) > 0:
        plan += f"### 3. Fix Function Signatures ({analysis['error_patterns']['Argument count mismatch']} examples)\n"
        plan += "- Update examples to match actual function signatures\n"
        plan += "- Verify function parameter requirements\n"
        plan += "- Update documentation to reflect correct usage\n\n"
    
    if analysis['error_patterns'].get('Type mismatch', 0) > 0:
        plan += f"### 4. Fix Type Issues ({analysis['error_patterns']['Type mismatch']} examples)\n"
        plan += "- Ensure examples use correct data types\n"
        plan += "- Add type checking to examples\n"
        plan += "- Update type requirements in documentation\n\n"
    
    plan += "## Implementation Steps\n\n"
    plan += "1. **Fix Critical Issues** - Address timeouts and empty errors first\n"
    plan += "2. **Update Function Signatures** - Ensure all examples use correct APIs\n"
    plan += "3. **Fix Type Issues** - Correct data type usage in examples\n"
    plan += "4. **Test Incrementally** - Validate fixes as they're applied\n"
    plan += "5. **Re-run Validation** - Ensure improvements are working\n\n"
    
    plan += "## Success Metrics\n\n"
    plan += "- Target: 90%+ success rate\n"
    plan += "- All critical examples working\n"
    plan += "- No timeout issues\n"
    plan += "- All function signatures correct\n\n"
    
    return plan

def main():
    """Main function to analyze validation failures."""
    print("Analyzing validation failures...")
    
    try:
        # Analyze results
        analysis = analyze_validation_results()
        
        # Generate improvement plan
        plan = generate_improvement_plan(analysis)
        
        # Write improvement plan
        with open('docs/generated/IMPROVEMENT_PLAN.md', 'w') as f:
            f.write(plan)
        
        print("Improvement plan generated: docs/generated/IMPROVEMENT_PLAN.md")
        
    except FileNotFoundError:
        print("Error: validation_results.json not found. Run validation first.")
    except Exception as e:
        print(f"Error analyzing results: {e}")

if __name__ == "__main__":
    main()
