#!/bin/bash
echo "Testing server..."
sleep 2
curl -v http://localhost:8080/test
echo "Test completed"
