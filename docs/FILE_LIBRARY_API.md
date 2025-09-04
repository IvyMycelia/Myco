# Myco File Library API

The Myco File Library provides comprehensive file I/O operations for reading, writing, and managing files. It's designed to be simple, safe, and consistent with Myco's built-in library system.

## Import

```myco
use file as file;
```

## Functions

### `file.read(filename)`
Reads the entire contents of a file as a string.

**Parameters:**
- `filename` (String): Path to the file to read

**Returns:**
- String: File contents, or `Null` if error

**Example:**
```myco
use file as file;
let content = file.read("data.txt");
if content != Null:
    print("File content: " + content);
end
```

### `file.write(filename, content)`
Writes a string to a file, overwriting existing content.

**Parameters:**
- `filename` (String): Path to the file to write
- `content` (String): Content to write to the file

**Returns:**
- `Null` on success, or error value on failure

**Example:**
```myco
use file as file;
let result = file.write("output.txt", "Hello, World!");
if result == Null:
    print("File written successfully");
end
```

### `file.append(filename, content)`
Appends a string to the end of a file.

**Parameters:**
- `filename` (String): Path to the file to append to
- `content` (String): Content to append

**Returns:**
- `Null` on success, or error value on failure

**Example:**
```myco
use file as file;
file.append("log.txt", "New log entry\n");
```

### `file.exists(filename)`
Checks if a file exists.

**Parameters:**
- `filename` (String): Path to the file to check

**Returns:**
- Boolean: `True` if file exists, `False` otherwise

**Example:**
```myco
use file as file;
if file.exists("config.txt"):
    print("Configuration file found");
else:
    print("Configuration file not found");
end
```

### `file.size(filename)`
Gets the size of a file in bytes.

**Parameters:**
- `filename` (String): Path to the file

**Returns:**
- Number: File size in bytes, or `Null` if error

**Example:**
```myco
use file as file;
let size = file.size("data.txt");
if size != Null:
    print("File size: " + str(size) + " bytes");
end
```

### `file.delete(filename)`
Deletes a file.

**Parameters:**
- `filename` (String): Path to the file to delete

**Returns:**
- `Null` on success, or error value on failure

**Example:**
```myco
use file as file;
let result = file.delete("temp.txt");
if result == Null:
    print("File deleted successfully");
end
```

### `file.read_lines(filename)`
Reads a file and returns its contents as an array of lines.

**Parameters:**
- `filename` (String): Path to the file to read

**Returns:**
- Array: Array of strings (one per line), or `Null` if error

**Example:**
```myco
use file as file;
let lines = file.read_lines("data.txt");
if lines != Null:
    for i in range(len(lines)):
        print("Line " + str(i) + ": " + lines[i]);
    end
end
```

### `file.write_lines(filename, lines)`
Writes an array of strings to a file, one string per line.

**Parameters:**
- `filename` (String): Path to the file to write
- `lines` (Array): Array of strings to write

**Returns:**
- `Null` on success, or error value on failure

**Example:**
```myco
use file as file;
let lines = ["Line 1", "Line 2", "Line 3"];
file.write_lines("output.txt", lines);
```

## Error Handling

The file library uses Myco's enhanced error handling system with fungus-themed error names:

- **SPORE_FILE_MISSING**: File not found or cannot be opened
- **CAP_ACCESS_DENIED**: Permission denied
- **SPORE_IO_FAILED**: I/O error during file operation

**Example with error handling:**
```myco
use file as file;

try:
    let content = file.read("nonexistent.txt");
    print("Content: " + content);
catch error:
    print("Error reading file: " + error);
end
```

## Complete Example

```myco
use file as file;

# Write some data
file.write("test.txt", "Hello, Myco!\nThis is a test file.");

# Check if file exists
if file.exists("test.txt"):
    print("File exists");
    
    # Read the content
    let content = file.read("test.txt");
    print("Content: " + content);
    
    # Get file size
    let size = file.size("test.txt");
    print("Size: " + str(size) + " bytes");
    
    # Read as lines
    let lines = file.read_lines("test.txt");
    print("Number of lines: " + str(len(lines)));
    
    # Append more content
    file.append("test.txt", "\nAppended line");
    
    # Clean up
    file.delete("test.txt");
    print("File deleted");
end
```

## Notes

- All file operations are synchronous
- File paths are relative to the current working directory
- The library handles memory management automatically
- Error conditions return `Null` or trigger exceptions depending on context
- File operations are safe and won't cause segmentation faults
