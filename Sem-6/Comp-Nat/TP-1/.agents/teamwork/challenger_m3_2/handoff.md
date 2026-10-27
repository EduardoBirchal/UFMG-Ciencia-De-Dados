# Milestone 3 Empirical Challenger Report: Throughput, Diversity & Memory Profiling

**Agent**: `challenger_m3_2`  
**Working Directory**: `d:\Downloads\bases-TP1\datasets\.agents\teamwork\challenger_m3_2`  
**Target Milestone**: Milestone 3 (Selection, Variation & Evolution Engine)  
**Parent Conversation ID**: `0fbe2801-fce8-49f2-8814-84768519d3f0`  
**Verdict**: **FAIL**  
**Risk Assessment**: HIGH (Execution speed severely violates target latency thresholds; downstream 30-seed experimental suite at risk of excessive runtime)  
**Date**: 2026-10-26  

---

## 1. Observation

### 1.1 Baseline Test Suite & Code Style Compliance
- Executed full test suite:
  - Command: `.\.venv\Scripts\python.exe -m pytest tests/ -v`
  - Verbatim Output: `=== 164 passed in 2.49s ===` (0 failed, 0 skipped).
- Executed style linter:
  - Command: `.\.venv\Scripts\python.exe scripts/check_style.py`
  - Verbatim Output: `all 25 files passed style checks` (0 violations).

---

### 1.2 Execution Speed Benchmarks (Empirical Multi-Seed Measurements)
Empirical wall-clock runtimes were measured across multiple seeds using Python 3.14 (`.venv`) on the standard datasets (`synth1` $N=60, D=2$; `concrete` $N=824, D=8$).

#### Benchmark 1: `synth1` (100 generations, pop=100, Tournament $k=2$)
- **Target Specified in Dispatch**: **$< 1.0$s**
- Multi-seed empirical run (seeds 1, 2, 3):
  * Seed 1: `7.296s`
  * Seed 2: `7.522s`
  * Seed 3: `8.483s`
  * **Summary**: `min=7.296s`, **`mean=7.767s`**, `max=8.483s`
- **Deviation**: **7.77x slower than target (VIOLATION)**.

#### Benchmark 2: `concrete` (100 generations, pop=100, Tournament $k=2$)
- **Target Specified in Dispatch**: **$< 3.0$s**
- Multi-seed empirical run (seeds 1, 2, 3):
  * Seed 1: `5.352s`
  * Seed 2: `8.633s`
  * Seed 3: `8.863s`
  * **Summary**: `min=5.352s`, **`mean=7.616s`**, `max=8.863s`
- **Deviation**: **2.54x slower than target (VIOLATION)**.

#### Benchmark 3: `concrete` (50 generations, pop=100, $\epsilon$-Lexicase Dynamic MAD Selection)
- **Target Specified in Dispatch**: **$< 15.0$s**
- Multi-seed empirical run (seeds 1, 2):
  * Seed 1: `25.240s`
  * Seed 2: `40.343s`
  * Initial single run: `50.322s`
  * **Summary**: `min=25.240s`, **`mean=32.792s`**, `max=40.343s`
- **Deviation**: **2.19x to 3.35x slower than target (VIOLATION)**.

---

### 1.3 Profiling Analysis & Root Causes

Profiling was executed via `cProfile` and `pstats` to trace exact internal execution bottlenecks:

#### Bottleneck 1: Redundant `numpy.nan_to_num` Defensive Wrapping
- Profile of 100 generations on `synth1`:
  * Total runtime: `12.009s`, `14,401,059` function calls.
  * `numpy.lib._type_check_impl.py:368(nan_to_num)`:
    - **Calls**: `346,260` invocations in a single run.
    - **Time**: `2.359s` self time, **`5.412s` cumulative time** (**45.1% of entire runtime**).
- **Source location**:
  * `src/gp/primitives.py`: Every primitive (`safe_add`, `safe_sub`, `safe_mul`, `safe_div`, `safe_sin`, `safe_cos`, `safe_log`, `safe_exp`) enters `with np.errstate(all="ignore"):`, clips, and executes `np.nan_to_num(res, ...)`.
  * `src/gp/node.py` (line 125): Immediately after calling the primitive, `Node.eval()` executes `np.nan_to_num(np.clip(res, -1e10, 1e10), ...)` **a second time** on the same array.
  * For an average tree of 15 nodes evaluated over 100 individuals across 100 generations, this executes $>300,000$ redundant calls to `nan_to_num`.

#### Bottleneck 2: Double Evaluation of Crossover Offspring in `GPEngine.fit`
- In `src/gp/engine.py` (lines 224–226):
  ```python
  child_pred = child.eval(x_tr)
  child_fit = compute_rmse(y_tr, child_pred)
  ```
  During breeding, each crossover child (~90 per generation) is fully evaluated against `x_tr` to compute `crossover_better` / `crossover_worse`.
- In `src/gp/engine.py` (line 109):
  ```python
  preds = np.array([ind.eval(x_tr) for ind in population])
  ```
  At the beginning of the subsequent generation, the entire population of 100 trees is re-evaluated from scratch against `x_tr`.
  * Result: ~90% of individuals are evaluated twice per generation.

#### Bottleneck 3: Unvectorized Nested Median Reductions in $\epsilon$-Lexicase
- Profile of 5 generations of $\epsilon$-lexicase on `concrete`:
  * `src/gp/selection.py:79(epsilon_lexicase_selection)`: 758 parent selections took `3.576s` out of `3.883s` (**92.1% of runtime**).
  * `numpy.lib._function_base_impl.py:4004(_median)`: Called **`87,940` times** in just 5 generations.
- In `src/gp/selection.py` (lines 109–121):
  ```python
  for case_idx in case_order:
      ...
      cand_errs = errors[candidates, case_idx]
      e_star = float(np.min(cand_errs))
      med = float(np.median(cand_errs))
      mad = float(np.median(np.abs(cand_errs - med)))
      threshold = e_star + mad + 1e-12
      candidates = [c for c, e in zip(candidates, cand_errs) if e <= threshold]
  ```
  Because `concrete` has $N=824$ cases and lexicase is invoked ~190 times per generation, calling Python `np.median` and list comprehensions in an inner loop across dynamically indexed sub-arrays causes massive CPU overhead.

---

### 1.4 Memory Profiling (Zero Cumulative Leak Verification)
Heap allocations were monitored using `tracemalloc` across 5 consecutive 50-generation `GPEngine.fit` runs on `synth1` ($N=60$):
```
Run 0: current=56.19 KB, peak=2574.86 KB
Run 1: current=53.79 KB, peak=2574.86 KB
Run 2: current=56.46 KB, peak=2956.07 KB
Run 3: current=53.60 KB, peak=2956.07 KB
Run 4: current=58.12 KB, peak=3135.36 KB
```
- **Result**: Post-run memory after garbage collection remained strictly bounded between $53.60$ KB and $58.12$ KB across all 5 runs.
- **Verdict on Memory**: **PASS** (Zero cumulative memory leak detected).

---

### 1.5 Metric Tracker Consistency & Numeric Integrity
Evaluated across all supported selection schemes (`tournament`, `roulette`, `lexicase`), both with and without elitism:
- Inspected all 25 logged keys in `history_`: `gen`, `generation`, `train_rmse_best`, `best_train_rmse`, `train_rmse_worst`, `train_rmse_mean`, `mean_train_rmse`, `test_rmse_best`, `best_test_rmse`, `duplicate_trees`, `duplicate_canonical_trees`, `unique_canonical_trees`, `crossover_better`, `crossover_better_count`, `crossover_worse`, `crossover_worse_count`, `crossover_equal`, `crossover_equal_count`, `crossover_rejected`, `crossover_rejected_count`, `crossover_total`, `avg_nodes`, `avg_node_count`, `avg_depth`, `best_nodes`, `best_node_count`, `best_depth`, `elapsed_seconds`.
- Every key returned valid, finite numeric types (`int` or `float`).
- Zero `NaN`, `None`, or `Inf` values occurred across all generations.
- **Verdict on Metric Tracker**: **PASS**.

---

### 1.6 Syntactic Duplicate Canonical Tree Counting
Evaluated `count_duplicates(population)` against manual ground-truth set comparison (`len(pop) - len(set(ind.to_string() for ind in pop))`):
- Tested edge cases:
  1. All identical trees ($N=20$ copies of constant $1.0$): returned 19 duplicates. (PASS)
  2. All unique trees ($N=20$ distinct constants): returned 0 duplicates. (PASS)
  3. Mixed population ($3$ copies of Tree A, $2$ copies of Tree B, $5$ unique trees): returned 3 duplicates. (PASS)
  4. 10 random ramped half-and-half populations (pop=100, depths 2-5): $100\%$ exact match between `count_duplicates` and independent set oracle.
  5. Invariant in `MetricTracker`: `duplicate_trees + unique_canonical_trees == pop_size` strictly held across all generations.
- **Verdict on Duplicate Counting**: **PASS**.

---

## 2. Logic Chain

1. **Target Thresholds Defined**: `DISPATCH.md` established explicit empirical performance thresholds:
   - Target 1: `synth1` 100 gen, pop=100 $< 1.0$s.
   - Target 2: `concrete` 100 gen, pop=100 $< 3.0$s.
   - Target 3: `concrete` 50 gen, pop=100, $\epsilon$-lexicase $< 15.0$s.
2. **Empirical Measurement Against Targets**:
   - `synth1` runs averaged `7.767s` ($7.8\times$ over target).
   - `concrete` tournament runs averaged `7.616s` ($2.5\times$ over target).
   - `concrete` lexicase runs averaged `32.792s` ($2.2\times$ over target).
   - Every single speed benchmark failed its target threshold by a substantial margin.
3. **Downstream Impact on Milestone 5**:
   - Milestone 5 executes 5 experimental steps across 30 random seeds (totaling hundreds of GP runs, including population sizes up to 500 and generations up to 500).
   - At current throughput, running the full 30-seed suite will consume many hours and risks violating the overall 7-hour execution ceiling.
4. **Conclusion Derivation**:
   - Because all three speed targets are violated, Milestone 3 cannot be approved in its current state, requiring a **FAIL** verdict to trigger a fast performance optimization pass by the worker.

---

## 3. Caveats

- Functional correctness, numerical robustness, elitism monotonicity, depth limit enforcement, memory stability, and metric schema compliance are all in excellent working order.
- The failure is strictly focused on execution throughput and runtime latency bottlenecks.

---

## 4. Conclusion

**Verdict: FAIL**

While memory management, duplicate tracking, and metric logging are fully verified and bug-free, **GPEngine execution speed severely violates the target runtime constraints on all three benchmark configurations** (`synth1` 7.8s vs <1s, `concrete` 7.6s vs <3s, and `concrete` lexicase 32.8s vs <15s).

### Concrete Mitigations Recommended for Worker Pass:
1. **Remove Double `nan_to_num` Wrapping**:
   - In `src/gp/primitives.py`, replace expensive `nan_to_num` and `np.errstate` context managers with direct NumPy array operations (e.g. `np.clip` or single post-operation cleanup).
   - In `src/gp/node.py`, eliminate redundant second call to `np.nan_to_num` in `Node.eval()`.
2. **Eliminate Redundant Offspring Evaluations**:
   - In `src/gp/engine.py`, cache evaluated offspring predictions/fitnesses so that individuals evaluated during crossover accounting are not re-evaluated in the next generation loop.
3. **Vectorize $\epsilon$-Lexicase Dynamic MAD Selection**:
   - In `src/gp/selection.py`, optimize the MAD calculation by avoiding scalar Python loops or using `np.partition` instead of full `np.median` calls on dynamic candidate lists, or precalculating case MADs when candidate errors are stable.

---

## 5. Verification Method

To independently reproduce the benchmark results:

1. **Run Speed Benchmark Harness**:
   ```powershell
   .\.venv\Scripts\python.exe -u -c "
   import time
   from src.data import load_dataset
   from src.gp.engine import GPEngine

   # synth1 tournament 100 gen (target < 1.0s)
   x_tr, y_tr, x_te, y_te = load_dataset('synth1')
   eng = GPEngine(pop_size=100, generations=100, selection='tournament', seed=42)
   t0 = time.perf_counter()
   eng.fit(x_tr, y_tr, x_te, y_te)
   dur1 = time.perf_counter() - t0
   print(f'synth1_100gen: {dur1:.3f}s (target < 1.0s, FAIL if > 1.0s)')

   # concrete tournament 100 gen (target < 3.0s)
   x_tr2, y_tr2, x_te2, y_te2 = load_dataset('concrete')
   eng2 = GPEngine(pop_size=100, generations=100, selection='tournament', seed=42)
   t0 = time.perf_counter()
   eng2.fit(x_tr2, y_tr2, x_te2, y_te2)
   dur2 = time.perf_counter() - t0
   print(f'concrete_100gen: {dur2:.3f}s (target < 3.0s, FAIL if > 3.0s)')

   # concrete lexicase 50 gen (target < 15.0s)
   eng3 = GPEngine(pop_size=100, generations=50, selection='lexicase', seed=42)
   t0 = time.perf_counter()
   eng3.fit(x_tr2, y_tr2, x_te2, y_te2)
   dur3 = time.perf_counter() - t0
   print(f'concrete_lexicase_50gen: {dur3:.3f}s (target < 15.0s, FAIL if > 15.0s)')
   "
   ```

2. **Run Memory Profiling Harness**:
   ```powershell
   .\.venv\Scripts\python.exe -u -c "
   import tracemalloc, gc
   from src.data import load_dataset
   from src.gp.engine import GPEngine

   x_tr, y_tr, x_te, y_te = load_dataset('synth1')
   tracemalloc.start()
   for run in range(5):
       eng = GPEngine(pop_size=50, generations=50, selection='tournament', seed=run)
       eng.fit(x_tr, y_tr)
       gc.collect()
       cur, peak = tracemalloc.get_traced_memory()
       print(f'run {run}: cur={cur/1024:.2f} KB, peak={peak/1024:.2f} KB')
   tracemalloc.stop()
   "
   ```

3. **Run Full Pytest & Style Linter**:
   ```powershell
   .\.venv\Scripts\python.exe -m pytest tests/ -v
   .\.venv\Scripts\python.exe scripts/check_style.py
   ```

### Invalidation Conditions
- `synth1` 100-generation runtime drops below $1.0$s.
- `concrete` 100-generation tournament runtime drops below $3.0$s.
- `concrete` 50-generation $\epsilon$-lexicase runtime drops below $15.0$s.
