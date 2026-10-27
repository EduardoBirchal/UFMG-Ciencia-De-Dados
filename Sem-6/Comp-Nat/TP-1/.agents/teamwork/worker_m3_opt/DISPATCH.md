# Task Assignment: Worker M3 Optimization Pass

## Working Directory
d:\Downloads\bases-TP1\datasets\.agents\teamwork\worker_m3_opt

## Objective
Optimize execution throughput across `src/gp/primitives.py`, `src/gp/node.py`, `src/gp/selection.py`, and `src/gp/engine.py` to satisfy all performance benchmarks identified by `challenger_m3_2`:
1. Target 1: `synth1` 100 generations, pop_size=100 (Tournament $k=2$) must run in **$< 1.0$s** (currently ~7.8s).
2. Target 2: `concrete` 100 generations, pop_size=100 (Tournament $k=2$) must run in **$< 3.0$s** (currently ~7.6s).
3. Target 3: `concrete` 50 generations, pop_size=100 ($\epsilon$-Lexicase) must run in **$< 15.0$s** (currently ~32.8s).

## Bottlenecks and Concrete Fixes
1. **Bottleneck 1: Redundant `nan_to_num` Double-Wrapping** (consumed 45% of runtime):
   - In `src/gp/primitives.py`:
     - Remove `with np.errstate(all="ignore"):` and `np.nan_to_num(...)` inside every individual primitive function!
     - Instead, write fast vectorized numpy primitives:
       - `safe_add(x, y)`: `np.clip(x + y, -1e50, 1e50)`
       - `safe_sub(x, y)`: `np.clip(x - y, -1e50, 1e50)`
       - `safe_mul(x, y)`: `np.clip(x * y, -1e50, 1e50)`
       - `safe_div(x, y)`: `denom = np.where(np.abs(y) < 1e-6, 1.0, y); res = np.where(np.abs(y) < 1e-6, 1.0, x / denom); return np.clip(res, -1e50, 1e50)`
       - `safe_log(x)`: `safe_x = np.where(np.abs(x) < 1e-6, 1.0, np.abs(x)); return np.where(np.abs(x) < 1e-6, 0.0, np.log(safe_x))`
       - `safe_exp(x)`: `return np.exp(np.clip(x, -32.0, 32.0))`
       - `safe_sin(x)`: `return np.sin(x)`
       - `safe_cos(x)`: `return np.cos(x)`
     - Notice these formulations are already closed and strictly guaranteed never to produce NaNs, Infs, or divide-by-zero warnings!
   - In `src/gp/node.py`:
     - Remove redundant `np.nan_to_num` in `Node.eval()`. Only apply `np.clip(res, -1e10, 1e10)` at root level if needed, or single nan guard only at `Tree.eval()`.
2. **Bottleneck 2: Double Evaluation of Crossover Offspring in `GPEngine.fit`**:
   - In `src/gp/engine.py`:
     - Cache predictions and training RMSEs of newly created offspring so that when the next generation starts, already-evaluated individuals are reused rather than re-evaluating all 100 trees from scratch!
3. **Bottleneck 3: Unvectorized $\epsilon$-Lexicase Dynamic MAD**:
   - In `src/gp/selection.py`:
     - Optimize candidate filtering loop:
       - Use fast array indexing or partition:
         ```python
         # cand_errs is array of errors of remaining candidates on test case
         cand_errs = errors[candidates, case_idx]
         e_star = cand_errs.min()
         med = np.median(cand_errs)
         mad = np.median(np.abs(cand_errs - med))
         threshold = e_star + mad + 1e-12
         # keep survivors using boolean mask
         survivors = candidates[cand_errs <= threshold]
         ```
       - Convert `candidates` to a NumPy 1D array (`np.arange(pop_size)`), avoiding slow Python list comprehensions `[c for c, e in zip(...)]`!

## Strict Requirements
- All 164 unit/E2E tests in `tests/` MUST continue to pass 100%.
- Zero style violations: all comments and docstrings MUST remain strictly casual lowercase without trailing periods (R2).
- DO NOT CHEAT. All optimizations must be genuine algorithmic/vectorization improvements.

## Inputs
- `d:\Downloads\bases-TP1\datasets\.agents\teamwork\ORIGINAL_REQUEST.md`
- `d:\Downloads\bases-TP1\datasets\PROJECT.md`
- `d:\Downloads\bases-TP1\datasets\.agents\teamwork\challenger_m3_2\handoff.md`

## Output
Write your handoff report to:
`d:\Downloads\bases-TP1\datasets\.agents\teamwork\worker_m3_opt\handoff.md`
Report the new benchmark runtimes for all 3 targets and verification output.


## 2026-10-26T18:58:51Z
You are a Worker subagent performing an optimization pass on Milestone 3.
Your working directory is: d:\Downloads\bases-TP1\datasets\.agents\teamwork\worker_m3_opt

MANDATORY: Read ORIGINAL_REQUEST.md at:
d:\Downloads\bases-TP1\datasets\.agents\teamwork\ORIGINAL_REQUEST.md
and PROJECT.md at:
d:\Downloads\bases-TP1\datasets\PROJECT.md
before starting work.

Read your task instructions at:
d:\Downloads\bases-TP1\datasets\.agents\teamwork\worker_m3_opt\DISPATCH.md
and the challenger's profiling evidence at:
d:\Downloads\bases-TP1\datasets\.agents\teamwork\challenger_m3_2\handoff.md

MANDATORY INTEGRITY WARNING:
DO NOT CHEAT. All optimizations must be genuine algorithmic/vectorization improvements. A teamwork_preview_auditor will independently verify your work. Integrity violations WILL be detected and rejected.

CRITICAL CODE STYLE CONSTRAINT (R2):
All comments and docstrings MUST be strictly casual lowercase without trailing periods! Handwritten look.

Optimize src/gp/primitives.py, src/gp/node.py, src/gp/selection.py, and src/gp/engine.py.
Benchmark the 3 targets:
1. synth1 100 gen, pop=100 (Tournament) < 1.0s
2. concrete 100 gen, pop=100 (Tournament) < 3.0s
3. concrete 50 gen, pop=100 (Lexicase) < 15.0s

Ensure all 164 tests pass with pytest and scripts/check_style.py reports 0 violations.

Maintain your progress in d:\Downloads\bases-TP1\datasets\.agents\teamwork\worker_m3_opt\progress.md.
Write your final handoff report to:
d:\Downloads\bases-TP1\datasets\.agents\teamwork\worker_m3_opt\handoff.md

When done, message the orchestrator (conversation ID 0fbe2801-fce8-49f2-8814-84768519d3f0).
