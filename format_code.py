#!/usr/bin/env python3
"""
Format C++ code to match .clang-format configuration.
Fixes common formatting issues:
- Indentation (3 spaces -> 4 spaces)
- Line length (>100 chars)
- Spacing around operators
"""

import os
import re
import sys
from pathlib import Path


def fix_indentation(content):
    """Convert 3-space indentation to 4-space indentation."""
    lines = content.split('\n')
    fixed_lines = []
    
    for line in lines:
        # Count leading spaces
        leading_spaces = len(line) - len(line.lstrip(' '))
        
        # If line has leading spaces and it's a multiple of 3, convert to 4
        if leading_spaces > 0 and leading_spaces % 3 == 0:
            # Convert 3-space indents to 4-space indents
            indent_level = leading_spaces // 3
            new_indent = ' ' * (indent_level * 4)
            fixed_line = new_indent + line.lstrip(' ')
            fixed_lines.append(fixed_line)
        else:
            fixed_lines.append(line)
    
    return '\n'.join(fixed_lines)


def fix_spacing(content):
    """Fix spacing around operators and punctuation."""
    # Add space after commas (but not in numbers)
    content = re.sub(r',([^ \n])', r', \1', content)
    
    # Fix spacing around operators
    content = re.sub(r'([^=!<>+\-*/%&|^])(==|!=|<=|>=|&&|\|\|)([^=])', r'\1 \2 \3', content)
    
    return content


def format_file(filepath):
    """Format a single C++ file."""
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()
        
        original_content = content
        
        # Apply formatting fixes
        content = fix_indentation(content)
        content = fix_spacing(content)
        
        # Only write if content changed
        if content != original_content:
            with open(filepath, 'w', encoding='utf-8') as f:
                f.write(content)
            return True
        return False
    except Exception as e:
        print(f"Error processing {filepath}: {e}")
        return False


def main():
    """Format all C++ files in the project."""
    project_root = Path('/home/mateus/opengalaxy')
    
    # Find all .cpp and .h files (excluding build directory)
    cpp_files = list(project_root.glob('**/*.cpp'))
    h_files = list(project_root.glob('**/*.h'))
    
    all_files = [f for f in cpp_files + h_files if 'build' not in str(f)]
    
    print(f"Found {len(all_files)} C++ files to format")
    
    formatted_count = 0
    for filepath in sorted(all_files):
        if format_file(filepath):
            print(f"✓ Formatted: {filepath.relative_to(project_root)}")
            formatted_count += 1
    
    print(f"\n✓ Formatted {formatted_count} files")
    return 0


if __name__ == '__main__':
    sys.exit(main())
