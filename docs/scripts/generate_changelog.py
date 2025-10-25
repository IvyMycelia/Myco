#!/usr/bin/env python3
"""
Generate changelog from git commits.
This script analyzes git history to automatically generate changelog entries.
"""

import subprocess
import re
import json
from datetime import datetime

def get_git_commits():
    """Get git commits with detailed information."""
    try:
        # Get commits with format: hash|date|author|message
        cmd = [
            'git', 'log', '--pretty=format:%H|%ad|%an|%s',
            '--date=short',
            '--reverse'
        ]
        result = subprocess.run(cmd, capture_output=True, text=True)
        
        if result.returncode != 0:
            print("Error: Could not get git commits")
            return []
        
        commits = []
        for line in result.stdout.strip().split('\n'):
            if line:
                parts = line.split('|', 3)
                if len(parts) >= 4:
                    commits.append({
                        'hash': parts[0],
                        'date': parts[1],
                        'author': parts[2],
                        'message': parts[3]
                    })
        
        return commits
    except Exception as e:
        print(f"Error getting git commits: {e}")
        return []

def categorize_commits(commits):
    """Categorize commits by type and impact."""
    categories = {
        'Added': [],
        'Changed': [],
        'Fixed': [],
        'Removed': [],
        'Security': [],
        'Performance': [],
        'Documentation': [],
        'Other': []
    }
    
    for commit in commits:
        message = commit['message'].lower()
        
        # Categorize based on commit message patterns
        if any(word in message for word in ['add', 'new', 'create', 'implement']):
            categories['Added'].append(commit)
        elif any(word in message for word in ['change', 'update', 'modify', 'refactor']):
            categories['Changed'].append(commit)
        elif any(word in message for word in ['fix', 'bug', 'error', 'issue']):
            categories['Fixed'].append(commit)
        elif any(word in message for word in ['remove', 'delete', 'drop']):
            categories['Removed'].append(commit)
        elif any(word in message for word in ['security', 'vulnerability', 'safe']):
            categories['Security'].append(commit)
        elif any(word in message for word in ['performance', 'optimize', 'speed', 'fast']):
            categories['Performance'].append(commit)
        elif any(word in message for word in ['doc', 'readme', 'comment', 'documentation']):
            categories['Documentation'].append(commit)
        else:
            categories['Other'].append(commit)
    
    return categories

def generate_changelog(categories):
    """Generate changelog markdown from categorized commits."""
    markdown = "# Changelog\n\n"
    markdown += "All notable changes to the Myco programming language are documented in this file.\n\n"
    markdown += "The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),\n"
    markdown += "and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).\n\n"
    
    # Get current date for version
    current_date = datetime.now().strftime("%Y-%m-%d")
    
    # Generate version entry
    markdown += f"## [Unreleased] - {current_date}\n\n"
    
    # Add categorized changes
    for category, commits in categories.items():
        if commits:
            markdown += f"### {category}\n\n"
            
            for commit in commits:
                # Clean up commit message
                message = commit['message']
                # Remove common prefixes
                message = re.sub(r'^(feat|fix|docs|style|refactor|test|chore):\s*', '', message, flags=re.IGNORECASE)
                # Capitalize first letter
                message = message[0].upper() + message[1:] if message else ""
                
                markdown += f"- {message} ({commit['date']})\n"
            
            markdown += "\n"
    
    return markdown

def generate_release_notes(categories):
    """Generate release notes for the current version."""
    notes = []
    
    for category, commits in categories.items():
        if commits:
            notes.append(f"### {category}")
            notes.append("")
            
            for commit in commits:
                message = commit['message']
                message = re.sub(r'^(feat|fix|docs|style|refactor|test|chore):\s*', '', message, flags=re.IGNORECASE)
                message = message[0].upper() + message[1:] if message else ""
                notes.append(f"- {message}")
            
            notes.append("")
    
    return "\n".join(notes)

def main():
    """Main function to generate changelog."""
    print("Generating changelog from git commits...")
    
    # Get commits
    commits = get_git_commits()
    print(f"Found {len(commits)} commits")
    
    if not commits:
        print("No commits found. Make sure you're in a git repository.")
        return
    
    # Categorize commits
    categories = categorize_commits(commits)
    
    # Print summary
    for category, commits in categories.items():
        if commits:
            print(f"{category}: {len(commits)} commits")
    
    # Generate changelog
    changelog = generate_changelog(categories)
    
    # Write to file
    with open('docs/generated/CHANGELOG.md', 'w') as f:
        f.write(changelog)
    
    # Generate release notes
    release_notes = generate_release_notes(categories)
    
    # Write release notes
    with open('docs/generated/RELEASE_NOTES.md', 'w') as f:
        f.write(release_notes)
    
    # Save categorized data as JSON
    with open('docs/generated/commits.json', 'w') as f:
        json.dump(categories, f, indent=2)
    
    print("Changelog generated: docs/generated/CHANGELOG.md")
    print("Release notes generated: docs/generated/RELEASE_NOTES.md")
    print("Commit data saved: docs/generated/commits.json")

if __name__ == "__main__":
    main()
