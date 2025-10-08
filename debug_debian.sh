#!/bin/bash
# Debug script for Debian segfault issues

echo "=== Myco Debian Debug Information ==="
echo "Date: $(date)"
echo "System: $(uname -a)"
echo ""

echo "=== Checking dependencies ==="
echo "GCC version:"
gcc --version 2>/dev/null || echo "GCC not found"
echo ""

echo "Required libraries:"
ldd bin/myco 2>/dev/null || echo "ldd not available"
echo ""

echo "=== Testing minimal functionality ==="
echo "Testing basic print:"
echo 'print("Test");' > test_minimal.myco
./bin/myco test_minimal.myco 2>&1
echo "Exit code: $?"
echo ""

echo "=== Testing with strace (if available) ==="
if command -v strace >/dev/null 2>&1; then
    echo "Running with strace:"
    timeout 5 strace -e trace=all ./bin/myco test_minimal.myco 2>&1 | head -20
else
    echo "strace not available"
fi
echo ""

echo "=== Testing with gdb (if available) ==="
if command -v gdb >/dev/null 2>&1; then
    echo "Running with gdb:"
    echo "run test_minimal.myco" | gdb --batch --quiet ./bin/myco 2>&1 | head -10
else
    echo "gdb not available"
fi
echo ""

echo "=== Memory check ==="
if command -v valgrind >/dev/null 2>&1; then
    echo "Running with valgrind:"
    timeout 10 valgrind --tool=memcheck --leak-check=full ./bin/myco test_minimal.myco 2>&1 | head -20
else
    echo "valgrind not available"
fi

# Cleanup
rm -f test_minimal.myco
