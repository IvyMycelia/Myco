# Myco Optimization Plan (Private)

Scope: Make Myco fast with minimal complexity. Baseline preserved (295/295 in `pass.myco`). Each change must be measurable and reversible.

## Targets (initial)
- Baseline arithmetic (arithmetic.myco): <= 0.15s on this machine
- Baseline loops (loops.myco): <= 0.004s
- No regression in language semantics; identical outputs.

## Phase 1 — Bytecode Execution (High Impact, Low Risk)
- Implement a simple stack-based bytecode engine.
- Compiler: AST -> BytecodeProgram with constants table and locals map.
- Interpreter: tight dispatch loop; inline arithmetic; no allocations on hot path.
- Fast paths for numbers; fallback to generic Value ops.
- Instrument per-op counters and wall time.

Arithmetic: 0.527s → 0.15s (3.5x faster)
Loops: 0.012s → 0.004s (3x faster)
Why: Eliminates AST traversal overhead

Deliverables:
- `include/core/bytecode.h`, `src/core/bytecode_compiler.c`, `src/core/bytecode_vm.c`.
- Flag in CLI to enable bytecode; default remains AST until parity and speed proven.
- Bench report showing 3–5x on arithmetic, ~3x on loops.

## Phase 2 — Value Representation
- Small-int tagging or NaN-boxing for Number/Bool/Null.
- Avoid heap allocs for temporaries; reuse stack slots.
- String interning for small literals.

Arithmetic: 0.15s → 0.08s (2x faster)
Overall: 2x speedup
Why: Reduces memory allocation and boxing

Deliverables:
- `include/core/value_fast.h`, minimal integration in VM only (AST path unchanged).
- Bench: +2x arithmetic over Phase 1; memory usage chart.

## Phase 3 — Hot Loop Specialization (No general JIT yet)
- Detect hot loops (exec count threshold + time).
- Loop-specialize bytecode: strength reduction, invariant hoist, induction recognition.
- Generate specialized bytecode variants keyed by simple type profiles (Number-only path).

Hot Loops: 0.004s → 0.001s (4x faster)
Overall: 1.5x speedup on hot code
Why: Native machine code execution

Deliverables:
- `src/core/bytecode_specializer.c` with guard + deopt back to generic bytecode.
- Bench: additional 1.5–2x on loops-heavy code.

## Phase 4 — Memory + Dispatch Overheads
- Arena for VM stack frames and temporaries.
- Direct-threaded dispatch (computed goto) where supported.
- Inline common ops (ADD/SUB/MUL/DIV, CMP) in the VM loop.

Deliverables:
- VM stack arenas in `runtime/`; direct-threaded VM behind `#ifdef`.
- Bench: +15–30% overall.

## Phase 5 — Optional JIT for Numeric Kernels (Only if profitable)
- Tiny JIT that lowers a restricted bytecode subset to native (x86_64/ARM64).
- Only for monomorphic Number loops; guards + fallback to bytecode.
- No complex scheduler; rely on compiler backend for now.

Deliverables:
- `src/core/jit_minimal.c` behind a flag; disabled by default.
- Bench: 2–4x on targeted hot numeric loops.

## Guardrails
- Keep AST interpreter unchanged and always available.
- Feature flags per phase; default off until each phase shows wins and parity.
- pass.myco must remain 295/295.
- Commit only after measurable improvement and zero regressions.

## Measurement
- Add micro-bench harness calling the VM directly.
- Record: wall time, op counts, allocations, branch mispredicts (if accessible).
- Store results under `benchmark/results/` (already gitignored).

## Risks and Mitigations
- Complexity creep: keep each phase <= 2–3 focused files; strict code review.
- Platform differences: stick to portable C; isolate arch-specific code.
- Semantics drift: compare outputs between AST and VM on every run.

Arithmetic: 0.527s → 0.08s (6.6x faster)
Loops: 0.012s → 0.001s (12x faster)
Overall: 5-10x faster than current state