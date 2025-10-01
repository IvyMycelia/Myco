#!/bin/bash

# Script to remove expression functions from compiler.c

COMPILER_FILE="src/compilation/compiler.c"

# Function names to remove
FUNCTIONS=(
    "codegen_generate_c_literal"
    "codegen_generate_c_identifier" 
    "codegen_generate_c_binary_op"
    "codegen_generate_c_unary_op"
    "codegen_generate_c_assignment"
    "codegen_generate_c_function_call"
    "codegen_generate_c_member_access"
    "codegen_generate_c_array_literal"
    "codegen_generate_c_hash_map_literal"
    "codegen_generate_c_set_literal"
    "codegen_generate_c_lambda"
    "codegen_generate_c_array_access"
    "codegen_generate_c_await"
)

# Create a temporary file
TEMP_FILE=$(mktemp)

# Process each function
for func in "${FUNCTIONS[@]}"; do
    echo "Removing function: $func"
    
    # Find the function start and end
    START_LINE=$(grep -n "^int $func(" "$COMPILER_FILE" | cut -d: -f1)
    if [ -n "$START_LINE" ]; then
        # Find the matching closing brace
        END_LINE=$(awk -v start="$START_LINE" '
        BEGIN { brace_count = 0; found_start = 0 }
        NR >= start {
            if (found_start == 0) {
                found_start = 1
                brace_count = 1
            } else {
                for (i = 1; i <= length($0); i++) {
                    char = substr($0, i, 1)
                    if (char == "{") brace_count++
                    if (char == "}") brace_count--
                    if (brace_count == 0) {
                        print NR
                        exit
                    }
                }
            }
        }' "$COMPILER_FILE")
        
        if [ -n "$END_LINE" ]; then
            echo "Removing lines $START_LINE to $END_LINE"
            # Remove the function
            sed "${START_LINE},${END_LINE}d" "$COMPILER_FILE" > "$TEMP_FILE"
            mv "$TEMP_FILE" "$COMPILER_FILE"
        fi
    fi
done

echo "Expression functions removed from $COMPILER_FILE"
