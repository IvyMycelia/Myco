#!/usr/bin/env python3
"""
Complete documentation automation system for Myco.
This script provides a comprehensive solution for generating, validating, and maintaining documentation.
"""

import os
import sys
import subprocess
import json
import time
from pathlib import Path

class MycoDocumentationSystem:
    """Complete documentation automation system for Myco."""
    
    def __init__(self):
        self.base_dir = Path.cwd()
        self.docs_dir = self.base_dir / 'docs'
        self.generated_dir = self.docs_dir / 'generated'
        self.scripts_dir = self.docs_dir / 'scripts'
        
        # Ensure directories exist
        self.generated_dir.mkdir(exist_ok=True)
        
    def run_script(self, script_name, description=""):
        """Run a Python script and return success status."""
        script_path = self.scripts_dir / script_name
        
        if not script_path.exists():
            print(f"Error: Script {script_path} not found")
            return False
        
        print(f"Running {script_name}...")
        if description:
            print(f"  {description}")
        
        try:
            result = subprocess.run([sys.executable, str(script_path)], 
                                capture_output=True, text=True, cwd=str(self.base_dir))
            
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
    
    def generate_all_documentation(self):
        """Generate all documentation components."""
        print("=== Myco Documentation Generation ===")
        print()
        
        scripts = [
            ('extract_functions.py', 'Extracting API functions from source code'),
            ('extract_syntax.py', 'Extracting syntax patterns from lexer/parser'),
            ('generate_examples.py', 'Generating code examples'),
            ('generate_changelog.py', 'Generating changelog from git history'),
        ]
        
        success_count = 0
        for script, description in scripts:
            if self.run_script(script, description):
                success_count += 1
            print()
        
        return success_count == len(scripts)
    
    def validate_examples(self, use_improved=True):
        """Validate all code examples."""
        print("=== Example Validation ===")
        print()
        
        if use_improved:
            success = self.run_script('improved_validate_examples.py', 
                                    'Validating examples with automatic fixes')
        else:
            success = self.run_script('validate_examples.py', 
                                    'Validating examples')
        
        return success
    
    def analyze_results(self):
        """Analyze validation results and generate improvement plan."""
        print("=== Analysis and Improvement Planning ===")
        print()
        
        success = self.run_script('analyze_failures.py', 
                                'Analyzing validation failures and generating improvement plan')
        return success
    
    def generate_summary_report(self):
        """Generate a comprehensive summary report."""
        print("=== Generating Summary Report ===")
        print()
        
        # Read validation results
        validation_file = self.generated_dir / 'improved_validation_results.json'
        if validation_file.exists():
            with open(validation_file, 'r') as f:
                results = json.load(f)
            
            total = len(results)
            successful = sum(1 for r in results if r['result']['success'])
            failed = total - successful
            fixes_applied = sum(len(r['result']['fixes_applied']) for r in results)
            
            # Generate summary
            summary = f"""# Myco Documentation System - Summary Report

Generated: {time.strftime('%Y-%m-%d %H:%M:%S')}

## Documentation Status

### Generated Documentation
- **API Reference**: Complete function documentation extracted from source
- **Syntax Reference**: Language syntax patterns and examples
- **Examples**: Working code examples with explanations
- **Changelog**: Version history from git commits
- **Validation Report**: Test results for all examples

### Example Validation Results
- **Total Examples**: {total}
- **Successful**: {successful}
- **Failed**: {failed}
- **Success Rate**: {(successful/total*100):.1f}%
- **Fixes Applied**: {fixes_applied}

### Key Features
- **Automatic Generation**: All documentation generated from source code
- **Example Validation**: Every code example tested for correctness
- **Automatic Fixes**: Common issues automatically resolved
- **Comprehensive Coverage**: API, syntax, examples, and changelog
- **Maintainable**: Easy to update and extend

### Files Generated
- `README.md` - Main documentation index
- `API_REFERENCE.md` - Built-in functions and libraries
- `SYNTAX_REFERENCE.md` - Language syntax guide
- `EXAMPLES.md` - Working code examples
- `CHANGELOG.md` - Version history
- `IMPROVED_VALIDATION_REPORT.md` - Example test results
- `IMPROVEMENT_PLAN.md` - Improvement recommendations

### Usage
```bash
# Generate all documentation
python3 docs/scripts/complete_docs_system.py

# Generate specific components
python3 docs/scripts/extract_functions.py
python3 docs/scripts/validate_examples.py
```

### Integration
- **CI/CD**: Run on every commit to keep docs current
- **Release Process**: Generate docs before each release
- **Development**: Update docs during development
- **Website**: Use generated markdown for documentation websites

## Next Steps
1. Review validation results and apply improvements
2. Integrate into development workflow
3. Set up automated generation
4. Customize for specific needs
"""
            
            with open(self.generated_dir / 'SUMMARY_REPORT.md', 'w') as f:
                f.write(summary)
            
            print("✓ Summary report generated")
            return True
        
        return False
    
    def run_complete_system(self):
        """Run the complete documentation system."""
        print("=== Myco Complete Documentation System ===")
        print("Automated documentation generation, validation, and analysis")
        print()
        
        start_time = time.time()
        
        # Step 1: Generate all documentation
        print("Step 1: Generating Documentation")
        doc_success = self.generate_all_documentation()
        
        # Step 2: Validate examples
        print("Step 2: Validating Examples")
        validation_success = self.validate_examples(use_improved=True)
        
        # Step 3: Analyze results
        print("Step 3: Analyzing Results")
        analysis_success = self.analyze_results()
        
        # Step 4: Generate summary
        print("Step 4: Generating Summary")
        summary_success = self.generate_summary_report()
        
        # Final report
        end_time = time.time()
        duration = end_time - start_time
        
        print("=== System Complete ===")
        print(f"Duration: {duration:.1f} seconds")
        print(f"Documentation: {'✓' if doc_success else '✗'}")
        print(f"Validation: {'✓' if validation_success else '✗'}")
        print(f"Analysis: {'✓' if analysis_success else '✗'}")
        print(f"Summary: {'✓' if summary_success else '✗'}")
        
        if all([doc_success, validation_success, analysis_success, summary_success]):
            print("✓ All systems operational!")
        else:
            print("⚠ Some systems had issues. Check the output above.")
        
        print(f"\nGenerated files are in: {self.generated_dir}")
        print(f"Main documentation: {self.generated_dir / 'README.md'}")
        print(f"Summary report: {self.generated_dir / 'SUMMARY_REPORT.md'}")

def main():
    """Main function to run the complete documentation system."""
    system = MycoDocumentationSystem()
    system.run_complete_system()

if __name__ == "__main__":
    main()
