# Myco Bootstrapping Roadmap

## Current Status

### ✅ Completed
- **Module System**: Full import/export support implemented
  - File-to-file imports: `use "module.myco" as m`
  - Specific imports: `use f1, f2 from "module"`
  - Aliased imports: `use f1, f2 from "module" as a1, a2`
  - Export keywords: `export func`, `export let`
  - Private keywords: `private func`, `private let`
- **Number Precision**: 64-bit IEEE 754 doubles with full precision printing
- **File I/O**: Reading files supported (for imports)

### 🚧 In Progress
- **Visibility Enforcement**: Export/private flags are parsed but not enforced

### 📋 Next Steps for Bootstrapping

#### 1. File Write Support
**Priority: HIGH**
- Need: `file.write(path, content)` function
- Why: Codegen needs to write `.c` files
- Current: Only `file.read()` exists

#### 2. Visibility Enforcement
**Priority: MEDIUM**
- Need: Module imports should respect `export`/`private` flags
- Why: Proper encapsulation for compiler modules
- Current: All symbols accessible regardless of visibility

#### 3. File Directives
**Priority: LOW**
- Need: `#! strict`, `#! export`, `#! private` directives
- Why: File-level module configuration
- Current: Per-symbol `export`/`private` only

#### 4. Compiler Core in Myco
**Priority: HIGH**
Start with:
- **Lexer**: Tokenize Myco source → Token objects
- **Parser**: Build AST from tokens
- **Codegen**: Convert AST → C code

Basic structure exists (`myco_compiler/` directory) but needs:
- String operations (substring, index, length)
- Array/list manipulation
- Error handling for compiler errors

#### 5. Compiler Features
**Priority: MEDIUM**
As needed:
- Type inference annotations
- Variable scope tracking
- Expression evaluation
- Statement compilation

## Recommended Next Steps

1. **Implement `file.write()`**
   - Most blocking for bootstrapping
   - Enables codegen to output C files

2. **Build basic lexer in Myco**
   - Tokenize Myco source
   - Test with simple strings

3. **Add missing string operations**
   - Substring, charAt, etc.
   - Needed for lexer implementation

4. **Gradually expand compiler**
   - Start with `print()` statements
   - Add variable declarations
   - Add arithmetic
   - Continue incrementally

## Testing Bootstrap

Once basic compiler works:
```myco
# bootstrap_test.myco
use "myco_compiler/minimal_compiler.myco" as compiler;

let source = "...";
let c_code = compiler.compile(source);
file.write("output.c", c_code);
# Compile and run the generated C
```

## Long-term Goal

Self-hosting compiler:
- Write Myco compiler in Myco itself
- Compile Myco compiler with itself
- Full bootstrapping cycle achieved

