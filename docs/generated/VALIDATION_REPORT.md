# Example Validation Report

**Total Examples:** 218
**Successful:** 137
**Failed:** 81
**Success Rate:** 62.8%

## Failed Examples

### docs/generated/EXAMPLES.md (Block 7)

**Line:** 77

**Error:** 

**Code:**
```myco
# While loop
let i = 0;
while i < 5:
    print("Count: " + i.toString());
    i = i + 1;
end;

# For loop
for let j = 0; j < 3; j++:
    print("For count: " + j.toString());
end;
```

### docs/generated/EXAMPLES.md (Block 9)

**Line:** 113

**Error:** 

**Code:**
```myco
# Read file
let content = file.read("data.txt");
print("File content: " + content);

# Write file
file.write("output.txt", "Hello from Myco!");
```

### docs/generated/EXAMPLES.md (Block 11)

**Line:** 142

**Error:** Timeout expired

**Code:**
```myco
let app = server.create(3000);

func handle_request(req, res):
    let response = {
        message: "Hello from Myco!",
        timestamp: time.now()
    };
    set_response_body(json.stringify(response));
    set_response_status(200);
end;

app.get("/api/hello", handle_request);
app.listen();
```

### docs/generated/SYNTAX_REFERENCE.md (Block 1)

**Line:** 55

**Error:** 

**Code:**
```myco
if condition:
    # code block
elif other_condition:
    # code block
else:
    # code block
end;
```

### docs/generated/SYNTAX_REFERENCE.md (Block 2)

**Line:** 67

**Error:** 

**Code:**
```myco
while condition:
    # code block
end;
```

### docs/generated/SYNTAX_REFERENCE.md (Block 3)

**Line:** 75

**Error:** 

**Code:**
```myco
for let i = 0; i < 10; i++:
    # code block
end;
```

### docs/generated/SYNTAX_REFERENCE.md (Block 7)

**Line:** 118

**Error:** 

**Code:**
```myco
let variable_name = value;
let mutable_var = value;
```

### docs/api-reference/stdlib.md (Block 2)

**Line:** 40

**Error:** 

**Code:**
```myco
let value = "hello";

# Type guards
value.isString();    # True
value.isInt();       # False
value.isFloat();     # False
value.isBoolean();   # False
value.isNull();      # False
value.isNumber();    # False (for strings)

# Type conversion
let num = 42;
num.toString();      # "42"
num.toFloat();       # 42.0

let str = "3.14";
str.toFloat();       # 3.14
str.toInt();         # 3 (truncates)
```

### docs/api-reference/stdlib.md (Block 5)

**Line:** 80

**Error:** 

**Code:**
```myco
# Absolute value
math.abs(-5);        # 5
math.abs(3.14);      # 3.14

# Square root
math.sqrt(16);       # 4.0
math.sqrt(2);        # 1.4142135623730951

# Power
math.pow(2, 3);      # 8.0
math.pow(10, 2);     # 100.0

# Logarithm
math.log(math.e);    # 1.0
math.log10(100);     # 2.0

# Trigonometric functions
math.sin(math.pi/2); # 1.0
math.cos(0);         # 1.0
math.tan(math.pi/4); # 1.0
```

### docs/api-reference/stdlib.md (Block 9)

**Line:** 133

**Error:** 

**Code:**
```myco
let text = "Hello, World!";

string.upper(text);  # "HELLO, WORLD!"
string.lower(text);  # "hello, world!"
```

### docs/api-reference/stdlib.md (Block 10)

**Line:** 142

**Error:** 

**Code:**
```myco
let text = "  Hello, World!  ";

string.trim(text);           # "Hello, World!"
string.trimLeft(text);       # "Hello, World!  "
string.trimRight(text);      # "  Hello, World!"
```

### docs/api-reference/stdlib.md (Block 11)

**Line:** 152

**Error:** 

**Code:**
```myco
let text = "Hello, World!";

text.length;                 # 13
text.isEmpty();              # False
text.startsWith("Hello");    # True
text.endsWith("!");          # True
text.contains("World");      # True
```

### docs/api-reference/stdlib.md (Block 12)

**Line:** 164

**Error:** 

**Code:**
```myco
let text = "Hello, World!";

text.replace("World", "Myco");  # "Hello, Myco!"
text.substring(0, 5);           # "Hello"
text.split(", ");               # ["Hello", "World!"]
```

### docs/api-reference/stdlib.md (Block 13)

**Line:** 176

**Error:** 

**Code:**
```myco
# Creation
let numbers = [1, 2, 3, 4, 5];
let fruits = ["apple", "banana", "orange"];

# Access
numbers[0];              # 1
numbers[2];              # 3
fruits[1];               # "banana"

# Properties
numbers.length;          # 5
numbers.isEmpty();       # False

# Methods
numbers.push(6);         # [1, 2, 3, 4, 5, 6]
numbers.pop();           # 6, array becomes [1, 2, 3, 4, 5]
numbers.join(", ");      # "1, 2, 3, 4, 5"
numbers.contains(3);     # True
numbers.indexOf(3);      # 2
numbers.slice(1, 3);     # [2, 3]
numbers.reverse();       # [5, 4, 3, 2, 1]
numbers.sort();          # [1, 2, 3, 4, 5]

# Functional methods
let doubled = numbers.map(func(x: Int) -> Int: return x * 2; end);
let evens = numbers.filter(func(x: Int) -> Boolean: return x % 2 == 0; end);
let sum = numbers.reduce(0, func(acc: Int, x: Int) -> Int: return acc + x; end);
```

### docs/api-reference/stdlib.md (Block 14)

**Line:** 208

**Error:** 

**Code:**
```myco
# Creation
let person = {
    name: "Alice",
    age: 30,
    city: "New York"
};

# Access
person.name;             # "Alice"
person["age"];           # 30

# Properties
person.size;             # 3
person.isEmpty();        # False

# Methods
person.has("name");      # True
person.keys();           # ["name", "age", "city"]
person.values();         # ["Alice", 30, "New York"]
person.set("email", "alice@example.com");
person.delete("city");
person.clear();
```

### docs/api-reference/stdlib.md (Block 15)

**Line:** 235

**Error:** 

**Code:**
```myco
# Creation
let colors = {"red", "green", "blue"};
let numbers = {1, 2, 3, 4, 5};

# Properties
colors.size;             # 3
colors.isEmpty();        # False

# Methods
colors.has("red");       # True
colors.add("yellow");    # {"red", "green", "blue", "yellow"}
colors.remove("green");  # {"red", "blue", "yellow"}
colors.toArray();        # ["red", "blue", "yellow"]

# Set operations
let primary = {"red", "green", "blue"};
let secondary = {"yellow", "cyan", "magenta"};
let all = primary.union(secondary);
let common = primary.intersection({"red", "blue", "purple"});
let diff = primary.difference({"red"});
```

### docs/api-reference/stdlib.md (Block 17)

**Line:** 281

**Error:** 

**Code:**
```myco
use graphs;

let graph = graphs.create();
graph = graph.addNode("A");
graph = graph.addNode("B");
graph = graph.addNode("C");
graph = graph.addEdge("A", "B");
graph = graph.addEdge("B", "C");

graph.size;              # 3
graph.isEmpty();         # False
graph.hasNode("A");      # True
graph.hasEdge("A", "B"); # True
graph = graph.clear();   # Empty graph
```

### docs/api-reference/stdlib.md (Block 18)

**Line:** 300

**Error:** [31m[ERROR][0m E3005 [unknown::unknown_function]:3:12: create() requires exactly 1 argument: max_heap (boolean)


**Code:**
```myco
use heaps;

let heap = heaps.create();
heap = heap.insert(10);
heap = heap.insert(5);
heap = heap.insert(15);
heap = heap.insert(3);

heap.size;               # 4
heap.isEmpty();          # False
heap.peek();             # 3 (minimum)
let min = heap.extract(); # 3, heap becomes [5, 10, 15]
```

### docs/api-reference/stdlib.md (Block 21)

**Line:** 352

**Error:** 

**Code:**
```myco
# Basic printing
print("Hello, World!");
print("Number:", 42);
print("Multiple values:", 1, 2, 3);

# Print without newline
print("Same line");
print(" continuation");

# Print to stderr
printError("Error message");
```

### docs/api-reference/stdlib.md (Block 22)

**Line:** 368

**Error:** Timeout expired

**Code:**
```myco
# Read input from user
let name = input("Enter your name: ");
let age = input("Enter your age: ").toInt();
```

### docs/api-reference/stdlib.md (Block 24)

**Line:** 386

**Error:** 

**Code:**
```myco
let now = time.now();
print(now.isoString());      # "2024-01-15T10:30:45Z"
print(now.unixTimestamp());  # 1705312245
```

### docs/api-reference/stdlib.md (Block 25)

**Line:** 394

**Error:** 

**Code:**
```myco
let birthday = time.create(1990, 5, 15, 14, 30, 0);
print(birthday.format("YYYY-MM-DD"));  # "1990-05-15"
```

### docs/api-reference/stdlib.md (Block 26)

**Line:** 401

**Error:** 

**Code:**
```myco
let now = time.now();

now.year();     # 2024
now.month();    # 1
now.day();      # 15
now.hour();     # 10
now.minute();   # 30
now.second();   # 45
```

### docs/api-reference/stdlib.md (Block 27)

**Line:** 414

**Error:** 

**Code:**
```myco
let now = time.now();
let future = now.add(365, "days");
let past = now.subtract(30, "days");
let diff = future.difference(now);

print("Days difference:", diff.days());
print("Hours difference:", diff.hours());
```

### docs/api-reference/stdlib.md (Block 29)

**Line:** 436

**Error:** 

**Code:**
```myco
let response = http.get("https://api.example.com/data");
if response.statusOk():
    let data = http.getJson(response);
    print("Data:", data);
end
```

### docs/api-reference/stdlib.md (Block 30)

**Line:** 446

**Error:** [31m[ERROR][0m E3004 [http::unknown_function]:2:16: http.post() data must be a string


**Code:**
```myco
let data = {name: "Alice", age: 30};
let response = http.post("https://api.example.com/users", data);
if response.statusOk():
    print("User created successfully");
end
```

### docs/api-reference/stdlib.md (Block 31)

**Line:** 456

**Error:** 

**Code:**
```myco
let response = http.get("https://api.example.com/data");

response.statusCode();           # 200
response.statusOk();             # True
http.getHeader(response, "content-type");  # "application/json"
let jsonData = http.getJson(response);
```

### docs/api-reference/stdlib.md (Block 45)

**Line:** 630

**Error:** 

**Code:**
```myco
# Number to string
let num = 42;
let str = num.toString();  # "42"

# String to number
let text = "3.14";
let float = text.toFloat();  # 3.14
let int = text.toInt();      # 3

# Boolean to string
let flag = True;
let boolStr = flag.toString();  # "True"
```

### docs/api-reference/stdlib.md (Block 46)

**Line:** 647

**Error:** 

**Code:**
```myco
# Range function
let numbers = range(1, 6);  # [1, 2, 3, 4, 5]

# Length function
let length = len("hello");  # 5
let arrayLength = len([1, 2, 3]);  # 3
```

### docs/api-reference/stdlib.md (Block 47)

**Line:** 658

**Error:** 

**Code:**
```myco
let value = "hello";

# Type guards
isString(value);    # True
isInt(value);       # False
isFloat(value);     # False
isBoolean(value);   # False
isNull(value);      # False
isNumber(value);    # False
isArray(value);     # False
isObject(value);    # False
```

### docs/getting-started/quick-start.md (Block 3)

**Line:** 74

**Error:** 

**Code:**
```myco
# For loop
for i in range(1, 6):
    print("Count:", i);
end

# While loop
let count = 0;
while count < 3:
    print("Hello!");
    count = count + 1;
end
```

### docs/getting-started/quick-start.md (Block 7)

**Line:** 167

**Error:** 

**Code:**
```myco
# Create a set
let colors = {"red", "green", "blue"};

# Add elements
colors = colors + {"yellow"};

# Check membership
if colors.has("red"):
    print("Red is in the set!");
end

# Get size
print("Colors count:", colors.size);
```

### docs/getting-started/quick-start.md (Block 14)

**Line:** 352

**Error:** 

**Code:**
```myco
func validateEmail(email: String) -> Boolean:
    if email.length == 0:
        return False;
    end
    
    if email.contains("@"):
        return True;
    else:
        return False;
    end
end

let email = "user@example.com";
if validateEmail(email):
    print("Valid email!");
else:
    print("Invalid email!");
end
```

### docs/getting-started/language-tour.md (Block 3)

**Line:** 50

**Error:** 

**Code:**
```myco
# Code blocks use 'end' keyword
if condition:
    # Block content
    print("Inside block");
end

# Functions also use 'end'
func example():
    # Function body
end
```

### docs/getting-started/language-tour.md (Block 13)

**Line:** 234

**Error:** 

**Code:**
```myco
# Range-based for loop
for i in range(1, 6):
    print("Count:", i);
end

# Array iteration
let fruits = ["apple", "banana", "orange"];
for fruit in fruits:
    print("Fruit:", fruit);
end

# With index
for i in range(0, fruits.length):
    print(i, ":", fruits[i]);
end
```

### docs/getting-started/language-tour.md (Block 14)

**Line:** 254

**Error:** 

**Code:**
```myco
let count = 0;
while count < 5:
    print("Count:", count);
    count = count + 1;
end

# Infinite loop with break
let i = 0;
while True:
    print("Loop iteration:", i);
    i = i + 1;
    if i >= 3:
        break;
    end
end
```

### docs/getting-started/language-tour.md (Block 15)

**Line:** 274

**Error:** 

**Code:**
```myco
# Break out of loop
for i in range(1, 10):
    if i == 5:
        break;  # Exit the loop
    end
    print(i);
end

# Skip iteration
for i in range(1, 6):
    if i == 3:
        continue;  # Skip this iteration
    end
    print(i);
end
```

### docs/getting-started/language-tour.md (Block 18)

**Line:** 337

**Error:** 

**Code:**
```myco
func greet(name: String, greeting: String = "Hello") -> String:
    return greeting + ", " + name + "!";
end

print(greet("Alice"));           # "Hello, Alice!"
print(greet("Bob", "Hi"));       # "Hi, Bob!"
```

### docs/getting-started/language-tour.md (Block 19)

**Line:** 348

**Error:** 

**Code:**
```myco
# Anonymous functions
let square = func(x: Int) -> Int: return x * x; end;
let add = func(a: Int, b: Int) -> Int: return a + b; end;

print(square(5));    # 25
print(add(3, 4));    # 7

# Lambda as parameter
func applyOperation(x: Int, op: (Int) -> Int) -> Int:
    return op(x);
end

let result = applyOperation(5, square);  # 25
```

### docs/getting-started/language-tour.md (Block 21)

**Line:** 382

**Error:** 

**Code:**
```myco
# Array creation
let numbers = [1, 2, 3, 4, 5];
let fruits = ["apple", "banana", "orange"];
let mixed = [1, "hello", True, 3.14];

# Accessing elements
print(numbers[0]);    # First element
print(fruits[2]);     # Third element

# Array length
print("Length:", numbers.length);

# Adding elements
numbers = numbers + [6, 7];
fruits = fruits + ["grape"];

# Array methods
let doubled = numbers.map(func(x: Int) -> Int: return x * 2; end);
let evens = numbers.filter(func(x: Int) -> Boolean: return x % 2 == 0; end);
let sum = numbers.reduce(0, func(acc: Int, x: Int) -> Int: return acc + x; end);
```

### docs/getting-started/language-tour.md (Block 22)

**Line:** 407

**Error:** 

**Code:**
```myco
# Map creation
let person = {
    name: "Alice",
    age: 30,
    city: "New York"
};

# Accessing values
print(person.name);   # Dot notation
print(person["age"]); # Bracket notation

# Adding/updating values
person.email = "alice@example.com";
person["phone"] = "555-1234";

# Checking existence
if person.has("email"):
    print("Email:", person.email);
end

# Map methods
print("Keys:", person.keys());
print("Size:", person.size);
```

### docs/getting-started/language-tour.md (Block 23)

**Line:** 435

**Error:** 

**Code:**
```myco
# Set creation
let colors = {"red", "green", "blue"};
let numbers = {1, 2, 3, 4, 5};

# Adding elements
colors = colors + {"yellow"};
numbers = numbers + {6, 7};

# Checking membership
if colors.has("red"):
    print("Red is in the set!");
end

# Set operations
let primary = {"red", "green", "blue"};
let secondary = {"yellow", "cyan", "magenta"};
let allColors = primary.union(secondary);
let common = primary.intersection({"red", "blue", "purple"});

# Set methods
print("Size:", colors.size);
print("To array:", colors.toArray());
```

### docs/getting-started/language-tour.md (Block 24)

**Line:** 462

**Error:** [31m[ERROR][0m E3005 [unknown::unknown_function]:16:12: create() requires exactly 1 argument: max_heap (boolean)


**Code:**
```myco
# Trees
let tree = trees.create();
tree = tree.insert(5);
tree = tree.insert(3);
tree = tree.insert(7);
print("Tree size:", tree.size);

# Graphs
let graph = graphs.create();
graph = graph.addNode("A");
graph = graph.addNode("B");
graph = graph.addEdge("A", "B");
print("Graph size:", graph.size);

# Heaps
let heap = heaps.create();
heap = heap.insert(10);
heap = heap.insert(5);
heap = heap.insert(15);
print("Heap peek:", heap.peek());

# Queues
let queue = queues.create();
queue = queue.enqueue("first");
queue = queue.enqueue("second");
print("Queue front:", queue.front());

# Stacks
let stack = stacks.create();
stack = stack.push("bottom");
stack = stack.push("top");
print("Stack top:", stack.top());
```

### docs/getting-started/language-tour.md (Block 33)

**Line:** 702

**Error:** 

**Code:**
```myco
# Import entire module
use math;
use string;
use file;

# Use imported functions
let result = math.sqrt(16);  # 4.0
let upper = string.upper("hello");  # "HELLO"
let content = file.read("data.txt");
```

### docs/getting-started/language-tour.md (Block 34)

**Line:** 716

**Error:** 

**Code:**
```myco
# Import specific functions
use math {sqrt, pow, pi};
use string {upper, lower, trim};

let result = sqrt(pow(2, 3));  # sqrt(8) ≈ 2.83
let text = upper("hello world");
```

### docs/getting-started/language-tour.md (Block 37)

**Line:** 761

**Error:** 

**Code:**
```myco
use string;

let text = "  Hello, World!  ";
print(string.upper(text));    # "  HELLO, WORLD!  "
print(string.lower(text));    # "  hello, world!  "
print(string.trim(text));     # "Hello, World!"
print(string.length(text));   # 15
```

### docs/getting-started/language-tour.md (Block 39)

**Line:** 796

**Error:** 

**Code:**
```myco
use http;

# GET request
let response = http.get("https://api.example.com/data");
if response.statusOk():
    let data = http.getJson(response);
    print("Data:", data);
end

# POST request
let postData = {name: "Alice", age: 30};
let postResponse = http.post("https://api.example.com/users", postData);
```

### docs/getting-started/language-tour.md (Block 40)

**Line:** 813

**Error:** 

**Code:**
```myco
use time;

# Current time
let now = time.now();
print("Current time:", now.isoString());

# Create specific time
let birthday = time.create(1990, 5, 15, 14, 30, 0);
print("Birthday:", birthday.format("YYYY-MM-DD"));

# Time operations
let future = now.add(365, "days");
let diff = future.difference(now);
print("Days difference:", diff.days());
```

### docs/getting-started/language-tour.md (Block 43)

**Line:** 861

**Error:** 

**Code:**
```myco
# No need to manually free memory
let largeArray = [];
for i in range(1, 1000000):
    largeArray = largeArray + [i];
end

# Memory is automatically freed when variables go out of scope
```

### docs/examples/web/README.md (Block 1)

**Line:** 191

**Error:** [35m[CRITICAL][0m E5001 [server::unknown_function]:14:1: server.post() handler must be a function


**Code:**
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

### docs/examples/web/README.md (Block 4)

**Line:** 249

**Error:** 

**Code:**
```myco
# Use consistent JSON structure
set_response_body('{"success": true, "data": {...}, "message": "Success"}');
set_response_body('{"success": false, "error": "Error message"}');
```

### docs/tutorials/02-variables-types.md (Block 3)

**Line:** 80

**Error:** 

**Code:**
```myco
let intValue = 42;
let floatValue = 3.14;

# Convert int to float
let intAsFloat = intValue.toFloat();
print("Int as float:", intAsFloat);  # 42.0

# Convert float to int (truncates decimal part)
let floatAsInt = floatValue.toInt();
print("Float as int:", floatAsInt);  # 3

# Convert numbers to strings
let intAsString = intValue.toString();
let floatAsString = floatValue.toString();
print("Int as string:", intAsString);    # "42"
print("Float as string:", floatAsString); # "3.14"
```

### docs/tutorials/02-variables-types.md (Block 5)

**Line:** 119

**Error:** 

**Code:**
```myco
let text = "  Hello, World!  ";

# Length
print("Length:", text.length);  # 15

# Case conversion
print("Upper:", text.upper());  # "  HELLO, WORLD!  "
print("Lower:", text.lower());  # "  hello, world!  "

# Trimming whitespace
print("Trimmed:", text.trim()); # "Hello, World!"

# Checking content
print("Starts with 'Hello':", text.startsWith("Hello"));  # False (due to spaces)
print("Contains 'World':", text.contains("World"));       # True
```

### docs/tutorials/02-variables-types.md (Block 7)

**Line:** 153

**Error:** 

**Code:**
```myco
# Boolean literals
let isStudent = True;
let hasLicense = False;

# Boolean from expressions
let isAdult = age >= 18;        # True if age is 18 or more
let isEven = (count % 2) == 0;  # True if count is even

print("Is student:", isStudent);
print("Is adult:", isAdult);
print("Is even:", isEven);
```

### docs/tutorials/02-variables-types.md (Block 12)

**Line:** 271

**Error:** 

**Code:**
```myco
func processValue(value: String | Int | Boolean):
    if value.isString():
        let str = value;  # Type narrowing
        print("String length:", str.length);
    else if value.isInt():
        let num = value;  # Type narrowing
        print("Number squared:", num * num);
    else if value.isBoolean():
        let flag = value;  # Type narrowing
        print("Boolean value:", flag);
    end
end

processValue("hello");  # "String length: 5"
processValue(42);       # "Number squared: 1764"
processValue(True);     # "Boolean value: True"
```

### docs/tutorials/02-variables-types.md (Block 14)

**Line:** 318

**Error:** 

**Code:**
```myco
# Type isn't obvious
let result: Int = calculateSomething();

# Documenting complex types
let user: {name: String, age: Int, email: String?} = {
    name: "Alice",
    age: 25,
    email: "alice@example.com"
};

# Optional types
let data: String? = fetchData();
```

### docs/tutorials/02-variables-types.md (Block 16)

**Line:** 370

**Error:** 

**Code:**
```myco
let info = name + age.toString();  # Convert int to string
```

### docs/tutorials/02-variables-types.md (Block 17)

**Line:** 376

**Error:** 

**Code:**
```myco
let name: String? = Null;
let length = name.length;  # Error: name might be null
```

### docs/tutorials/02-variables-types.md (Block 18)

**Line:** 382

**Error:** 

**Code:**
```myco
if name != Null:
    let length = name.length;  # Safe access
end
```

### docs/tutorials/02-variables-types.md (Block 20)

**Line:** 398

**Error:** 

**Code:**
```myco
if value.isString():  # Correct guard
    print("It's a string");
end
```

### docs/tutorials/01-hello-world.md (Block 9)

**Line:** 252

**Error:** 

**Code:**
```myco
print(message);  # Error: 'message' is not defined
```

### docs/language-reference/syntax.md (Block 5)

**Line:** 94

**Error:** 

**Code:**
```myco
let decimal = 42;        # Decimal
let hex = 0x2A;          # Hexadecimal
let octal = 0o52;        # Octal
let binary = 0b101010;   # Binary
```

### docs/language-reference/syntax.md (Block 6)

**Line:** 103

**Error:** 

**Code:**
```myco
let pi = 3.14159;        # Standard notation
let scientific = 1.23e4; # Scientific notation
let negative = -2.5;     # Negative
```

### docs/language-reference/syntax.md (Block 7)

**Line:** 111

**Error:** 

**Code:**
```myco
let single = "Hello, World!";           # Double quotes
let multi = "Line 1\nLine 2";           # With escape sequences
let raw = r"C:\Users\Name";             # Raw string (no escaping)
let template = "Hello, ${name}!";       # String interpolation (planned)
```

### docs/language-reference/syntax.md (Block 22)

**Line:** 291

**Error:** 

**Code:**
```myco
let a = 10, b = 3;

a + b;   # Addition: 13
a - b;   # Subtraction: 7
a * b;   # Multiplication: 30
a / b;   # Division: 3.333...
a % b;   # Modulo: 1
a ** b;  # Exponentiation: 1000
```

### docs/language-reference/syntax.md (Block 23)

**Line:** 304

**Error:** 

**Code:**
```myco
let a = 5, b = 10;

a == b;   # Equal: False
a != b;   # Not equal: True
a < b;    # Less than: True
a > b;    # Greater than: False
a <= b;   # Less than or equal: True
a >= b;   # Greater than or equal: False
```

### docs/language-reference/syntax.md (Block 24)

**Line:** 317

**Error:** 

**Code:**
```myco
let a = True, b = False;

a and b;  # Logical AND: False
a or b;   # Logical OR: True
not a;    # Logical NOT: False
```

### docs/language-reference/syntax.md (Block 25)

**Line:** 327

**Error:** 

**Code:**
```myco
let str1 = "Hello", str2 = "World";

str1 + str2;        # Concatenation: "HelloWorld"
str1 + " " + str2;  # "Hello World"
```

### docs/language-reference/syntax.md (Block 26)

**Line:** 336

**Error:** 

**Code:**
```myco
let arr1 = [1, 2], arr2 = [3, 4];

arr1 + arr2;        # Concatenation: [1, 2, 3, 4]
arr1 * 3;           # Repetition: [1, 2, 1, 2, 1, 2]
```

### docs/language-reference/syntax.md (Block 27)

**Line:** 345

**Error:** 

**Code:**
```myco
let set1 = {1, 2, 3}, set2 = {3, 4, 5};

set1 + set2;        # Union: {1, 2, 3, 4, 5}
set1 - set2;        # Difference: {1, 2}
set1 & set2;        # Intersection: {3}
```

### docs/language-reference/syntax.md (Block 30)

**Line:** 379

**Error:** 

**Code:**
```myco
# Literals
42;
"hello";
True;

# Identifiers
variable;

# Parenthesized expressions
(2 + 3) * 4;
```

### docs/language-reference/syntax.md (Block 31)

**Line:** 394

**Error:** 

**Code:**
```myco
# Dot notation
object.property;
object.method();

# Bracket notation
array[index];
map[key];
```

### docs/language-reference/syntax.md (Block 32)

**Line:** 406

**Error:** 

**Code:**
```myco
# No arguments
func();

# With arguments
add(5, 3);
print("Hello, World!");

# Method calls
array.push(42);
string.upper("hello");
```

### docs/language-reference/syntax.md (Block 33)

**Line:** 421

**Error:** 

**Code:**
```myco
# Array access
let arr = [1, 2, 3];
arr[0];     # First element
arr[1];     # Second element

# Map access
let map = {name: "Alice", age: 30};
map["name"];    # Bracket notation
map.name;       # Dot notation
```

### docs/language-reference/syntax.md (Block 36)

**Line:** 452

**Error:** 

**Code:**
```myco
x = 42;
name = "Bob";
array[0] = 10;
map["key"] = "value";
```

### docs/language-reference/syntax.md (Block 38)

**Line:** 471

**Error:** 

**Code:**
```myco
# Basic if
if condition:
    statement;
end

# If-else
if condition:
    statement1;
else:
    statement2;
end

# If-else-if
if condition1:
    statement1;
else if condition2:
    statement2;
else:
    statement3;
end
```

### docs/language-reference/syntax.md (Block 39)

**Line:** 496

**Error:** 

**Code:**
```myco
# Basic while
while condition:
    statement;
end

# With break
while True:
    if condition:
        break;
    end
    statement;
end
```

### docs/language-reference/syntax.md (Block 40)

**Line:** 513

**Error:** 

**Code:**
```myco
# Range-based for
for i in range(1, 6):
    print(i);
end

# Array iteration
for item in array:
    print(item);
end

# With index
for i in range(0, array.length):
    print(i, ":", array[i]);
end
```

### docs/language-reference/syntax.md (Block 41)

**Line:** 532

**Error:** 

**Code:**
```myco
# Break
for i in range(1, 10):
    if i == 5:
        break;  # Exit the loop
    end
    print(i);
end

# Continue
for i in range(1, 6):
    if i == 3:
        continue;  # Skip this iteration
    end
    print(i);
end
```

### docs/language-reference/syntax.md (Block 47)

**Line:** 643

**Error:** 

**Code:**
```myco
let person = Person("Alice", 30);
let dog = Dog("Buddy");
```

### docs/language-reference/syntax.md (Block 51)

**Line:** 705

**Error:** 

**Code:**
```myco
# Import entire module
use math;
use string;
use file;

# Import specific functions
use math {sqrt, pow, pi};
use string {upper, lower, trim};
```

