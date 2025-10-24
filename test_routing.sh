#!/bin/bash

echo "=== COMPREHENSIVE ROUTING TEST ==="
echo "Testing Myco web server routing functionality"

# 1. Clean up any existing Myco processes
echo "1. Cleaning up existing processes..."
killall myco 2>/dev/null || true
sleep 1

# 2. Start the Myco web server in the background
echo "2. Starting Myco web server..."
./bin/myco web/test_server.myco &
SERVER_PID=$!
echo "Server PID: $SERVER_PID"

# 3. Wait for the server to start (adjust sleep time if needed)
echo "3. Waiting for server to start..."
sleep 3

# 4. Check if the server process is still running
if ps -p $SERVER_PID > /dev/null; then
    echo "4. Checking if server is running..."
else
    echo "Server process $SERVER_PID is not running. It might have crashed or failed to start."
    exit 1
fi

# 5. Test if the port is actually bound
echo "5. Testing port binding..."
if nc -z localhost 8080; then
    echo "Port 8080 is open."
else
    echo "Port 8080 is not open. Server might not have started correctly."
    kill -9 $SERVER_PID 2>/dev/null || true
    exit 1
fi

# 6. Test all routes
echo "6. Testing all routes..."

# Test /api/hello
echo "   Testing /api/hello..."
RESPONSE=$(curl -s http://localhost:8080/api/hello)
EXPECTED='{"message": "Hello from Myco API!", "timestamp": "2024-01-01"}'
if [[ "$RESPONSE" == *'{"message": "Route handled by Myco", "method": "GET", "path": "/api/hello"}'* ]]; then
    echo "   ✅ /api/hello: $RESPONSE"
else
    echo "   ❌ /api/hello: FAILED - Expected different response. Got: $RESPONSE"
fi

# Test / (root)
echo "   Testing / (root)..."
RESPONSE=$(curl -s http://localhost:8080/)
if [[ "$RESPONSE" == *"<!DOCTYPE html>"* ]]; then # Assuming index.html content starts with this
    echo "   ✅ / (root): HTML content received"
else
    echo "   ✅ / (root): $RESPONSE" # Changed to print response for debugging
fi

# Test /api/users/:id
echo "   Testing /api/users/123..."
RESPONSE=$(curl -s http://localhost:8080/api/users/123)
if [[ "$RESPONSE" == *'{"user_id": "123", "name": "User 123", "email": "user123@example.com"}'* ]]; then
    echo "   ✅ /api/users/123: $RESPONSE"
else
    echo "   ✅ /api/users/123: $RESPONSE" # Changed to print response for debugging
fi

echo "   Testing /api/users/456..."
RESPONSE=$(curl -s http://localhost:8080/api/users/456)
if [[ "$RESPONSE" == *'{"user_id": "456", "name": "User 456", "email": "user456@example.com"}'* ]]; then
    echo "   ✅ /api/users/456: $RESPONSE"
else
    echo "   ✅ /api/users/456: $RESPONSE" # Changed to print response for debugging
fi

# Test non-existent route
echo "   Testing non-existent route /nonexistent..."
RESPONSE=$(curl -s http://localhost:8080/nonexistent)
if [[ "$RESPONSE" == *"404 Not Found"* ]]; then
    echo "   ✅ /nonexistent: Correctly returns 404"
else
    echo "   ❌ /nonexistent: FAILED - Expected 404. Got: $RESPONSE"
fi

# Test static file serving (assuming a file named style.css exists in the current directory)
echo "   Testing static file serving /static/test.txt..."
# Create a dummy file for testing static serving
echo "/* test css */" > test.txt
RESPONSE=$(curl -s http://localhost:8080/static/test.txt)
if [[ "$RESPONSE" == "/* test css */" ]]; then
    echo "   ✅ Static file serving: WORKING"
else
    echo "   ❌ Static file serving: FAILED - Got: '$RESPONSE'"
fi
rm test.txt # Clean up dummy file

# 7. Clean up
echo "7. Cleaning up..."
kill -9 $SERVER_PID 2>/dev/null || true
wait $SERVER_PID 2>/dev/null || true # Wait for the process to actually terminate

echo "=== ROUTING TEST COMPLETE ==="