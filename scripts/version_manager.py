#!/usr/bin/env python3
"""
Myco Language Version Manager
Automatically manages semantic versioning for Myco development
"""

import os
import sys
import re
import subprocess
from datetime import datetime

class MycoVersionManager:
    def __init__(self, project_root):
        self.project_root = project_root
        self.version_file = os.path.join(project_root, "include/core/version.h")
        self.changelog_file = os.path.join(project_root, "CHANGELOG.md")
        
    def get_current_version(self):
        """Extract current version from version.h"""
        with open(self.version_file, 'r') as f:
            content = f.read()
            
        major = int(re.search(r'#define MYCO_VERSION_MAJOR (\d+)', content).group(1))
        minor = int(re.search(r'#define MYCO_VERSION_MINOR (\d+)', content).group(1))
        patch = int(re.search(r'#define MYCO_VERSION_PATCH (\d+)', content).group(1))
        
        return (major, minor, patch)
    
    def update_version(self, version_type="patch", release_name=None):
        """Update version based on type: patch, minor, major"""
        major, minor, patch = self.get_current_version()
        
        if version_type == "patch":
            patch += 1
        elif version_type == "minor":
            minor += 1
            patch = 0
        elif version_type == "major":
            major += 1
            minor = 0
            patch = 0
        else:
            raise ValueError("Version type must be 'patch', 'minor', or 'major'")
        
        # Update version.h
        self._update_version_file(major, minor, patch, release_name)
        
        # Update changelog
        self._update_changelog(major, minor, patch, version_type)
        
        print(f"Version updated to v{major}.{minor}.{patch}")
        return (major, minor, patch)
    
    def _update_version_file(self, major, minor, patch, release_name):
        """Update the version.h file with new version numbers"""
        with open(self.version_file, 'r') as f:
            content = f.read()
        
        # Update version numbers
        content = re.sub(r'#define MYCO_VERSION_MAJOR \d+', f'#define MYCO_VERSION_MAJOR {major}', content)
        content = re.sub(r'#define MYCO_VERSION_MINOR \d+', f'#define MYCO_VERSION_MINOR {minor}', content)
        content = re.sub(r'#define MYCO_VERSION_PATCH \d+', f'#define MYCO_VERSION_PATCH {patch}', content)
        content = re.sub(r'#define MYCO_VERSION_STRING "[^"]*"', f'#define MYCO_VERSION_STRING "{major}.{minor}.{patch}"', content)
        content = re.sub(r'#define MYCO_VERSION_FULL "[^"]*"', f'#define MYCO_VERSION_FULL "v{major}.{minor}.{patch}"', content)
        
        # Update release name if provided
        if release_name:
            content = re.sub(r'#define MYCO_RELEASE_NAME "[^"]*"', f'#define MYCO_RELEASE_NAME "{release_name}"', content)
        
        with open(self.version_file, 'w') as f:
            f.write(content)
    
    def _update_changelog(self, major, minor, patch, version_type):
        """Update CHANGELOG.md with new version entry"""
        changelog_entry = f"""## [v{major}.{minor}.{patch}] - {datetime.now().strftime('%Y-%m-%d')}

### {version_type.title()} Changes
- Bug fixes and improvements
- Enhanced stability and performance

"""
        
        # Read existing changelog
        if os.path.exists(self.changelog_file):
            with open(self.changelog_file, 'r') as f:
                content = f.read()
        else:
            content = "# Changelog\n\nAll notable changes to Myco Language will be documented in this file.\n\n"
        
        # Insert new entry after the header
        lines = content.split('\n')
        insert_index = 0
        for i, line in enumerate(lines):
            if line.startswith('## [v'):
                insert_index = i
                break
            insert_index = i + 1
        
        lines.insert(insert_index, changelog_entry)
        
        with open(self.changelog_file, 'w') as f:
            f.write('\n'.join(lines))
    
    def auto_version(self):
        """Automatically determine version bump based on git commits"""
        try:
            # Get commits since last tag
            result = subprocess.run(['git', 'log', '--oneline', '--since=1.week.ago'], 
                                 capture_output=True, text=True, cwd=self.project_root)
            
            if result.returncode != 0:
                print("Could not determine git history, defaulting to patch version")
                return self.update_version("patch")
            
            commits = result.stdout.strip().split('\n') if result.stdout.strip() else []
            
            # Analyze commit types
            major_keywords = ['breaking', 'major', 'incompatible']
            minor_keywords = ['feat', 'feature', 'new', 'add']
            
            version_type = "patch"  # default
            
            for commit in commits:
                commit_lower = commit.lower()
                if any(keyword in commit_lower for keyword in major_keywords):
                    version_type = "major"
                    break
                elif any(keyword in commit_lower for keyword in minor_keywords):
                    version_type = "minor"
            
            print(f"Detected {len(commits)} commits, suggesting {version_type} version bump")
            return self.update_version(version_type)
            
        except Exception as e:
            print(f"Auto-version failed: {e}, defaulting to patch")
            return self.update_version("patch")
    
    def show_status(self):
        """Show current version and recent changes"""
        major, minor, patch = self.get_current_version()
        print(f"Current Version: v{major}.{minor}.{patch}")
        
        # Show recent commits
        try:
            result = subprocess.run(['git', 'log', '--oneline', '-5'], 
                                 capture_output=True, text=True, cwd=self.project_root)
            if result.returncode == 0 and result.stdout.strip():
                print("\nRecent Commits:")
                for line in result.stdout.strip().split('\n'):
                    print(f"  {line}")
        except:
            pass

def main():
    if len(sys.argv) < 2:
        print("Myco Version Manager")
        print("Usage:")
        print("  python scripts/version_manager.py status          # Show current version")
        print("  python scripts/version_manager.py patch          # Bump patch version (bug fixes)")
        print("  python scripts/version_manager.py minor          # Bump minor version (new features)")
        print("  python scripts/version_manager.py major          # Bump major version (breaking changes)")
        print("  python scripts/version_manager.py auto           # Auto-detect version bump")
        print("  python scripts/version_manager.py patch --name 'Bug Fixes'  # With release name")
        return
    
    # Get project root (parent of scripts directory)
    project_root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    manager = MycoVersionManager(project_root)
    
    command = sys.argv[1]
    
    if command == "status":
        manager.show_status()
    elif command == "auto":
        manager.auto_version()
    elif command in ["patch", "minor", "major"]:
        release_name = None
        if "--name" in sys.argv:
            name_index = sys.argv.index("--name")
            if name_index + 1 < len(sys.argv):
                release_name = sys.argv[name_index + 1]
        manager.update_version(command, release_name)
    else:
        print(f"Unknown command: {command}")
        sys.exit(1)

if __name__ == "__main__":
    main()
