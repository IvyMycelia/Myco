# Myco Web Development Examples

This directory contains examples for building web applications and APIs with Myco.

## Examples

### 🚀 Modern Web Server (`modern_server.myco`)
A fully functional web server demonstrating:
- **REST API endpoints** with JSON responses
- **CORS support** for cross-origin requests
- **Static file serving** for frontend assets
- **Error handling** and proper HTTP status codes
- **Modern server architecture** with automatic keep-alive

**Features:**
- GET `/api/events` - Retrieve events list
- POST `/api/auth/register` - User registration
- POST `/api/auth/login` - User authentication
- POST `/api/events` - Create new events
- Static file serving from root directory
- CORS headers for React/frontend compatibility

**Usage:**
```bash
# Run the server
myco modern_server.myco

# Test the API
curl http://localhost:3000/api/events
curl -X POST http://localhost:3000/api/auth/register

# Open in browser
open http://localhost:3000/
```

### 📝 Basic Web Server (`test_server.myco`)
A simpler example showing:
- Basic HTTP server setup
- Static file serving
- Simple API endpoints
- Manual request handling loop

**Usage:**
```bash
myco test_server.myco
curl http://localhost:8080/api/hello
```

### 🌐 Interactive Frontend (`index.html`)
A modern HTML page for testing the web server:
- **Interactive API testing** with buttons
- **Real-time response display** 
- **Modern UI** with responsive design
- **JavaScript integration** with Myco backend

## Web Development Features

### ✅ What Myco Web Servers Support

1. **HTTP Methods**: GET, POST, PUT, DELETE, OPTIONS
2. **REST API**: JSON request/response handling
3. **Static Files**: Serve HTML, CSS, JS, images
4. **CORS**: Cross-origin request support
5. **Route Parameters**: Dynamic URL parameters
6. **Error Handling**: Proper HTTP status codes
7. **Keep-Alive**: Automatic server persistence (like Node.js)

### 🔧 Development Workflow

#### Option 1: Full-Stack Myco
```bash
# Myco backend + static frontend
myco modern_server.myco
# Serves both API and frontend on port 3000
```

#### Option 2: Myco Backend + React Frontend
```bash
# Terminal 1: Myco backend
myco modern_server.myco

# Terminal 2: React frontend
npx create-react-app my-app
cd my-app
npm start
# React on port 3001, Myco on port 3000
```

#### Option 3: Production Deployment
```bash
# Build React app
npm run build

# Copy build files to Myco server directory
cp -r build/* /path/to/myco/server/

# Myco serves everything
myco modern_server.myco
```

## API Examples

### JavaScript/Frontend Integration

```javascript
// Fetch events from Myco server
async function loadEvents() {
    const response = await fetch('http://localhost:3000/api/events');
    const data = await response.json();
    
    if (data.success) {
        console.log('Events:', data.events);
    }
}

// Register user
async function registerUser(username, email) {
    const response = await fetch('http://localhost:3000/api/auth/register', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({ username, email })
    });
    
    return await response.json();
}
```

### React Integration

```jsx
import React, { useState, useEffect } from 'react';

function EventsList() {
    const [events, setEvents] = useState([]);
    
    useEffect(() => {
        fetch('http://localhost:3000/api/events')
            .then(res => res.json())
            .then(data => {
                if (data.success) {
                    setEvents(data.events);
                }
            });
    }, []);
    
    return (
        <div>
            {events.map(event => (
                <div key={event.id}>
                    <h3>{event.title}</h3>
                    <p>{event.description}</p>
                </div>
            ))}
        </div>
    );
}
```

## Server Architecture

### Myco Server Structure
```myco
#!/usr/bin/env myco

# 1. Create server instance
let app = server.create(3000);

# 2. Define API handlers
func handle_get_events():
    set_response_body('{"success": true, "events": [...]}');
    set_response_status(200);
end;

# 3. Register routes
app.get("/api/events", handle_get_events);
app.post("/api/auth/register", handle_register);

# 4. Static file serving
app.static("/", "./");

# 5. Start server (stays alive automatically)
app.listen();
```

### Key Features

- **Automatic Keep-Alive**: Server stays running like Node.js
- **CORS Support**: Works with any frontend framework
- **JSON API**: Standard REST API responses
- **Static Serving**: Serve frontend files
- **Error Handling**: Proper HTTP status codes
- **Route Parameters**: Dynamic URL handling

## Best Practices

### 1. Server Organization
```myco
# Group related functionality
# - Helper functions at the top
# - API handlers in the middle  
# - Route registration at the bottom
# - Server startup last
```

### 2. Error Handling
```myco
func handle_get_event():
    # Check if event exists
    if event_not_found:
        set_response_body('{"success": false, "error": "Event not found"}');
        set_response_status(404);
    else:
        set_response_body('{"success": true, "event": event_data}');
        set_response_status(200);
    end;
end;
```

### 3. JSON Responses
```myco
# Use consistent JSON structure
set_response_body('{"success": true, "data": {...}, "message": "Success"}');
set_response_body('{"success": false, "error": "Error message"}');
```

### 4. CORS Configuration
```myco
# CORS is automatically enabled
# Supports all origins, methods, and headers
# No additional configuration needed
```

## Troubleshooting

### Common Issues

**Server won't start:**
- Check if port is already in use
- Verify Myco installation
- Check for syntax errors

**CORS errors:**
- Ensure CORS headers are present (automatic)
- Check browser console for specific errors
- Verify server is running

**API not responding:**
- Check route registration
- Verify handler functions
- Test with curl first

### Debug Tips

```bash
# Test server with curl
curl http://localhost:3000/api/events
curl -X POST http://localhost:3000/api/auth/register

# Check server logs
myco modern_server.myco 2>&1 | tee server.log

# Test CORS
curl -H "Origin: http://localhost:3001" http://localhost:3000/api/events
```

## Next Steps

1. **Start with `modern_server.myco`** for a complete example
2. **Modify the API endpoints** to fit your needs
3. **Add your own frontend** (React, Vue, vanilla JS)
4. **Deploy to production** with proper configuration

Happy web development with Myco! 🚀
