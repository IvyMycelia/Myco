# Myco Module System Design

## Current State

### Built-in Libraries
- Libraries are pre-registered in the global environment
- Accessed via `use` statement: `use math`, `use string as s`
- Libraries are essentially objects with methods (e.g., `math.abs`, `string.upper`)

### File Imports (In Progress)
- Basic file loading: `use "path/to/file.myco" as module`
- Files load in isolated environments
- **Issue**: No way to expose module functions/values to the importer

## Proposed Module System

### Two Import Mechanisms

#### 1. Library Imports (Built-in or Standard Library)
```myco
use math;                    # Import whole library as 'math'
use math as m;               # Import library with alias
use max, min from math;      # Import specific items
use max as mx, min as mn from math;  # Import with aliases
```

**Behavior:**
- Imports pre-registered library objects
- Libraries are singleton objects with methods
- No file loading involved
- Already implemented

#### 2. Module Imports (User Files)
```myco
use "utils/myco" as utils;    # Import file, aliased as 'utils'
use utils.someFunction();     # Access module's exported functions
use utils.someValue;          # Access module's exported values
```

**Behavior:**
- Loads and parses `.myco` file
- Executes in isolated module environment
- **Missing**: Export mechanism to expose functions/values

### Export Options

#### Option A: Explicit Export Keyword (Similar to JavaScript/Python)
```myco
# math_utils.myco
export func square(x: Number) -> Number:
    return x * x;
end

export let PI = 3.14159;

func internalHelper() -> Number:
    return 42;
end  # Not exported
```

**Access:**
```myco
use "math_utils.myco" as math;
let result = math.square(5);  # Can access
let pi = math.PI;             # Can access
math.internalHelper();         # ERROR: not exported
```

#### Option B: Implicit Exports (Similar to Lua)
```myco
# math_utils.myco
func square(x: Number) -> Number:
    return x * x;
end

let PI = 3.14159;

# Everything at module-level is exported by default
```

**Access:**
```myco
use "math_utils.myco" as math;
let result = math.square(5);  # Can access
let pi = math.PI;             # Can access
```

#### Option C: Explicit Import List (Current parser supports this!)
```myco
# math_utils.myco
func square(x: Number) -> Number:
    return x * x;
end

func cube(x: Number) -> Number:
    return x * x * x;
end
```

**Access:**
```myco
use square, cube from "math_utils.myco";
let result = square(5);  # Direct access, no namespace
```

### Recommendation: Hybrid Approach

Support both implicit exports for simplicity AND explicit imports for flexibility.

#### Syntax Design

**Two Complementary Mechanisms:**

1. **Explicit `export` keyword** - Marks symbols for intentional export
2. **Implicit exports** - All unmarked top-level symbols are public by default
3. **`private` keyword** - Explicitly marks symbols as internal-only

#### Best Hybrid Syntax

**Module Definition with File Directives (utils/myco):**

```myco
# File-level directives

# Global directives (at very top of file) - apply to entire file
#! export    # Export all top-level symbols by default (opt-in to exports)
#! private   # Private by default (defaul
#! strict    # Require explicit typing for all top-level symbols, no implicit returns

# Example: Per-section overrides

# Section 1: Public API (explicit export mode)
#! export
export func apiFunction(x: Number) -> Number:
    return x * 2;
end

func publicHelper() -> Number:  # Also exported (export mode)
    return 42;
end

# Section 2: Private implementation (private mode)
#! private
func internalFunc() -> Number:  # Private
    return 100;
end

export func publicAPI() -> Number:  # Explicitly public
    return internalFunc() + 1;
end

# Section 3: Back to default mode (no directive)
# Directive is scoped, so we're back to default export behavior
func normalFunc() -> Number:  # Exported (default)
    return 0;
end

# Nested scoping example
#! private
func wrapper() -> Number:
    # All symbols here are private by default
    func inner() -> Number:  # Still private (nested)
        return 5;
    end
    
    # But explicit export overrides
    export func publicInner() -> Number:  # Public despite nesting
        return 10;
    end
    
    return inner() + publicInner();
end
```

**Full Example with Default Behavior:**
```myco
# No file directives = export-default (implicit exports)

# Explicitly exported (recommended for public APIs)
export func publicFunction(x: Number) -> Number:
    return x * 2;
end

# Implicitly exported (public by default)
func anotherPublicFunction(x: Number) -> Number:
    return x * 3;
end

# Explicitly private (internal use only)
private func internalHelper() -> Number:
    return 42;
end

# Constants can be exported explicitly
export let API_VERSION = "1.0.0";

# Or implicitly
let MODULE_NAME = "utils";

# Private constant (won't be accessible outside)
private let SECRET_KEY = "hidden";
```

**Example with private-default:**
```myco
#! private-default  # Top-level is private by default

# This would be private
func wouldBePrivate(x: Number) -> Number:
    return x * 2;
end

# This is public (explicit export required)
export func actuallyPublic(x: Number) -> Number:
    return x * 3;
end
```

**Import Options:**

```myco
# Option 1: Full module import with namespace (default)
use "utils" as utils;
let result1 = utils.publicFunction(5);           # Can access
let result2 = utils.anotherPublicFunction(5);    # Can access
let version = utils.API_VERSION;                  # Can access
utils.internalHelper();                           # ERROR: not exported
utils.SECRET_KEY;                                # ERROR: private

# Option 2: Specific imports (bring into current scope)
use publicFunction, API_VERSION from "utils";
let result = publicFunction(5);                   # Direct access, no namespace
print(API_VERSION);                              # Direct access

# Option 3: Specific imports with aliases
use publicFunction as pf, anotherPublicFunction as apf from "utils";
let r1 = pf(5);
let r2 = apf(5);

# Option 4: Partial namespace import
use "utils" as u;
use publicFunction as pf from u;  # Re-export pattern
let result = pf(5);
```

#### Access Rules

**What's Considered Top-Level (Public by Default):**

**Top-level (implicitly exported):**
- Functions defined at module scope: `func square(x)` at file root
- Variables at module scope: `let CONSTANT = 42` at file root
- Classes at module scope: `class MyClass` at file root
- Constants at module scope: `const PI = 3.14` at file root

**NOT Top-level (private by default, never accessible outside):**
- Variables inside functions: `let local = x + 1` inside a function body
- Nested functions: `func helper()` defined inside another function
- Variables in control flow: `let temp` inside `if`/`while` blocks
- Function parameters: `func foo(x: Number)` - `x` is local scope
- Closure variables: Any variable captured in a function's environment

**Example:**
```myco
# Public (top-level)
func publicFunction(x: Number) -> Number:
    return x * 2;
end

let GLOBAL_CONSTANT = "public";

# Private (inside function)
func wrapper() -> Number:
    # This is NOT top-level - it's inside a function
    func internalHelper() -> Number:
        return 42;  # Not accessible from outside module
    end
    
    # This variable is also private
    let localVar = 10;
    
    return internalHelper() + localVar;
end

# Nested scoping makes things private
if someCondition:
    let temporary = 5;  # Private - not accessible outside if block
    func tempFunc():  # Private - inside control flow
        return temporary;
    end
end
```

**Priority System:**
1. If a symbol is marked `private` → Never accessible outside module
2. If a symbol is marked `export` → Always accessible (even if nested)
3. If neither and file is `strict` → ERROR (must be explicit)
4. If neither and file is `export-default` (default) → Public (implicit export)
5. If neither and file is `private-default` → Private (opt-in exports)
6. If neither and nested → Private (scope-based, regardless of mode)

**File-Level Directives (Scoped):**

**`#! export` (Default - No directive needed):**
- Top-level symbols in scope are public by default
- `export` keyword is optional (for clarity/documentation)
- `private` keyword marks as internal
- Can be applied to file or to code sections
- Best for: Scripts, utilities, development code
```myco
#! export  # or just omit (default behavior)
func publicFunc() -> Number: return 42; end  # Exported automatically
private func internal() -> Number: return 0; end  # Private
```

**`#! private`:**
- Top-level symbols in scope are private by default
- Must use `export` to make public
- Explicit opt-in for public API
- Can be applied to file or to code sections
- Best for: Library code, APIs, production modules
```myco
#! private
func privateFunc() -> Number: return 42; end  # Private (not exported)
export func publicFunc() -> Number: return 0; end  # Exported
```

**`#! strict`:**
- ALL symbols MUST have explicit type annotations
- No implicit return types, no type inference
- Forces explicit type decisions everywhere
- Can be combined with export/private directives
- Best for: Large projects, team development, public libraries
```myco
#! strict
export func publicFunc(x: Number) -> Number: return 42; end  # Explicit types required
private func privateFunc(x: Number) -> Number: return 0; end  # Explicit types required
# ERROR: func untypedFunc(x): return 1; end  # Missing type annotations
```

**Multiple Directives (Per-Section):**
```myco
# File-level default
#! export

# Section 1: Public API area
#! export
func api1() -> Number: return 1; end  # Exported (export mode)

# Section 2: Implementation detail (private by default)
#! private
func internal() -> Number: return 2; end  # Private
export func publicAPI() -> Number: return internal(); end  # Explicitly public

# Section 3: Back to default
func normal() -> Number: return 3; end  # Exported (default)

# Section 4: Strict typing required
#! strict
func typedFunc(x: Number) -> Number: return x; end  # Types required
# func untypedFunc(x): return x; end  # ERROR: missing types
```

**Export Semantics:**
- `export` is explicit intent: "I want this to be public" (overrides file mode)
- `private` is explicit intent: "Keep this internal" (overrides file mode)
- No keyword behavior depends on file mode
- Nested/local symbols are always private regardless of mode

**Why this hybrid approach works:**

1. **Explicit control when needed** - `export`/`private` for library APIs
2. **Simplicity for scripts** - Write code normally, it just works
3. **Gradual discipline** - Start implicit, add keywords as project grows
4. **Clear intent** - Easy to see public API of a module
5. **Namespace-friendly** - Supports both namespaced and direct access
6. **Scope-based privacy** - Natural privacy without keywords for nested code

### Semantics

#### Environment Model

**Module Execution:**
```
Module A Environment (isolated)
├── square (function) [top-level, exported]
├── cube (function) [top-level, exported]
├── PI (variable) [top-level, exported]
├── _helper (function) [top-level, but 'private' keyword]
└── ... (module's local scope)
```

**Importer Environment:**
```
Importer Environment
├── math (module object with internal env)
└── ... (importer's code)
```

**Access Flow for `math.square`:**
1. Resolve `math` → gets module object
2. Module object stores its internal environment
3. Look up `square` in module's internal environment
4. Check if symbol is exported:
   - If `export` keyword → accessible
   - If `private` keyword → error
   - If top-level → export
   - If nested → error
5. Return function if accessible

**Scope-Based Privacy:**
```myco
# In module mymath.myco
let public = 10;        # Top-level, exported

func publicFunc():      # Top-level, exported
    let local = 5;      # NOT top-level, not exported
    func nested():      # NOT top-level, not exported
        return local;
    end
    return nested();
end

func otherFunc():
    return publicFunc();  # Can access because same module
end
```

#### Export Rules

**What Gets Exported:**

**Automatically Exported (Top-Level Only):**
- All top-level `func` definitions: `func square(x: Number)`
- All top-level `let` variable declarations: `let CONSTANT = 42`
- All top-level `class` definitions: `class MyClass`
- All top-level `const` declarations: `const PI = 3.14159`

**Never Exported (Automatically Private):**
- Variables inside functions: `let local = x` (inside a function body)
- Nested functions: `func helper()` (inside another function)
- Variables in control flow: `let temp` (inside `if`/`while` blocks)
- Function parameters: `func foo(x: Number)` - parameter `x` is local
- Temporary variables in expressions

**Explicit Control (Phase 3):**
- `export` keyword overrides: Makes nested symbols exportable
- `private` keyword overrides: Makes top-level symbols private
- Underscore prefix convention: `_internalHelper()` (future enhancement)

**Examples of Scope-Based Privacy:**
```myco
# These are exported (top-level)
func publicFunction() -> Number:
    return 42;
end

let GLOBAL_CONSTANT = 10;

# These are NOT exported (nested/local)
func wrapper() -> Number:
    let localVar = 5;           # Private (inside function)
    func internalHelper():     # Private (nested function)
        return localVar;
    end
    return internalHelper();
end

# Print statements don't export anything
print("Hello");  # Executes but nothing to export
```

### Syntax Examples

```myco
# Module: utils/helpers.myco

func add(a: Number, b: Number) -> Number:
    return a + b;
end

func multiply(a: Number, b: Number) -> Number:
    return a * b;
end

let version = "1.0.0";
```

```myco
# Importer: main.myco

# Option 1: Import whole module
use "utils/helpers.myco" as helpers;
let sum = helpers.add(1, 2);
let product = helpers.multiply(3, 4);

# Option 2: Import specific items
use add, multiply from "utils/helpers.myco";
let sum = add(1, 2);        # Direct access
let product = multiply(3, 4);  # Direct access

# Option 3: Import with aliases
use add as plus, multiply as times from "utils/helpers.myco";
let sum = plus(1, 2);
let product = times(3, 4);
```

### Implementation Priority

1. **Current implementation**: Module loading works, environment isolation works
2. **Missing**: Module value creation with exports populated
3. **Next**: Make all module-level symbols accessible via namespace
4. **Future**: Add explicit `export` keyword for selective exports

### Questions to Resolve

1. **Member access pattern**: 
   - Current: `module.function()` 
   - Should we support `module->function()` for clarity?

2. **Default exports**:
   - Should modules have a "main" export?
   - Example: `use MainFunction from "module.myco"`

3. **Circular imports**:
   - How to handle A imports B, B imports A?
   - Detection and error vs. deferred loading

4. **Hot reloading**:
   - Should modules be cached or re-loaded each time?
   - Development vs. production behavior

## Current Status

- ✅ Built-in library imports working
- ✅ File loading implemented
- ✅ Module environment isolation working
- ✅ Member access handler for modules added
- ❌ Module functions not accessible (implementation bug)
- ❌ Export mechanism not implemented
- ❌ Specific imports not working for file imports

## Implementation Roadmap

### Phase 1: Core Module Access (Immediate)
**Goal:** Make implicit exports work

1. Fix bug where module functions aren't accessible
   - Issue: Module environment populated but not exposed
   - Fix: Properly register module exports when creating module value

2. Implement implicit "export all" behavior
   - All top-level `func`, `let`, `class` automatically exported
   - No keyword needed initially

3. Test basic module access
   ```myco
   use "utils.myco" as utils;
   utils.someFunction();
   ```

**Deliverable:** File imports with namespace access working

### Phase 2: Specific Imports (Short-term)
**Goal:** Support parser's specific import syntax for file modules

1. Implement specific imports from file modules
   - Parser already supports: `use item1, item2 from library`
   - Extend to work with file imports: `use item1, item2 from "file.myco"`
   - Bring items into current scope (no namespace)

2. Add alias support for specific imports
   - `use func1 as f1, func2 as f2 from "file.myco"`

3. Test specific imports
   ```myco
   use square, cube from "math_utils.myco";
   let result = square(5);
   ```

**Deliverable:** Direct function access without namespace

### Phase 3: Explicit Export Keywords + File Directives (Medium-term)
**Goal:** Add `export`/`private` keywords and file-level directives

1. Add file-level directive parsing with scoping
   - Parse `#! strict`, `#! export`, `#! private` directives
   - Support directives at file top (global) or before code sections (scoped)
   - Each directive applies until the next directive or end of file
   - Store directive state in module context
   - Apply directive rules during export filtering

2. Add `export` keyword to lexer and parser
   - `export func ...`, `export let ...`, `export class ...`
   - Mark symbol for intentional export
   - Explicit intent for public API
   - Overrides file-level default

3. Add `private` keyword to lexer and parser
   - `private func ...`, `private let ...`, `private class ...`
   - Mark symbol as internal-only
   - Prevent access from outside module
   - Overrides file-level default

4. Update export logic in interpreter
   - Track active directive state (starts with default `#! export`)
   - Check scoped directive for each code section
   - Check for `export`/`private` modifiers on AST nodes
   - Apply rules based on directive and keyword presence:
     - `#! export`: Export unmarked top-level (default behavior)
     - `#! private`: Private unmarked top-level
     - `#! strict`: Require explicit type annotations for all symbols
   - Filter exported symbols when creating module value
   - Return error on access to private symbols
   - Error on untyped symbols when in strict mode

5. Test explicit control with different modes
   ```myco
   # Module 1: Strict mode
   #! strict
   export func publicAPI(x: Number) -> Number: return x; end  # OK
   private func internalOnly(x: Number) -> Number: return x; end  # OK
   # func untyped(): return 42; end  # ERROR: strict mode requires types
   
   # Module 2: Private mode
   #! private
   func wouldBePrivate(x: Number) -> Number: return x; end  # Private
   export func wouldBePublic(x: Number) -> Number: return x; end  # Public
   
   # Module 3: Export mode (default)
   #! export
   func autoExported(x: Number) -> Number: return x; end  # Exported
   
   # Importer
   module1.publicAPI(5);       # OK
   module1.internalOnly(5);    # ERROR: private
   module2.wouldBePrivate(5);  # ERROR: private
   module2.wouldBePublic(5);  # OK
   module3.autoExported(5);   # OK
   ```

**Deliverable:** Full control over module exports with file-level configuration

### Phase 4: Optimizations (Long-term)
**Goal:** Production-ready module system

1. Module caching
   - Avoid re-parsing files on every `use`
   - Cache module environments and ASTs
   - Hash-based invalidation

2. Circular import detection
   - Track import chain
   - Error on circular dependencies
   - Consider deferred loading for certain cases

3. Module reloading (dev mode)
   - Watch files for changes
   - Hot reload during development
   - Clear cache on file modification

## Syntax Summary

**Export Keywords:**
- `export` - Explicit public API
- `private` - Explicit internal only
- No keyword - Implicit public (default)

**Import Styles:**
- Full: `use "module" as m; m.func();`
- Specific: `use func from "module"; func();`
- Aliased: `use func as f from "module"; f();`

**Priority Rules:**
1. `private` → Never accessible
2. `export` → Always accessible  
3. Default → Public by default

