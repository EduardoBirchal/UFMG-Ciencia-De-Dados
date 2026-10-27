# Progress: Challenger 2 (Milestone 3)

**Last visited**: 2026-10-26T18:57:30Z  
**Status**: COMPLETED  
**Verdict**: FAIL (Speed benchmark targets violated; Memory, Consistency & Duplicates approved)

## Empirical Findings Summary
1. **Execution Speed Benchmarks**:
   - `synth1` (N=60, pop=100, 100 gen, tournament): mean **7.767s** (min 7.296s, max 8.483s) vs target **< 1.0s** -> **FAIL (7.8x over target)**
   - `concrete` (N=824, pop=100, 100 gen, tournament): mean **7.616s** (min 5.352s, max 8.863s) vs target **< 3.0s** -> **FAIL (2.5x over target)**
   - `concrete` (N=824, pop=100, 50 gen, epsilon-lexicase): mean **32.792s** (min 25.240s, max 40.343s) vs target **< 15.0s** -> **FAIL (2.2x over target)**
2. **Memory Profiling**:
   - 5 consecutive 50-gen runs monitored via `tracemalloc`: post-GC heap allocations remained strictly flat at 53.6 KB – 58.1 KB (peak 3.1 MB) -> **PASS (Zero cumulative memory leak)**
3. **Metric Tracker Consistency**:
   - Verified across tournament, roulette, and lexicase selections (with and without elitism): all generational keys numeric, zero NaNs, zero Infs -> **PASS**
4. **Syntactic Duplicate Tracking**:
   - Verified across edge cases (all-identical, all-unique, mixed, and random ramped half-and-half populations): 100% exact match against independent oracle `len(pop) - len(set(to_string))` -> **PASS**

## Root Causes Identified
- Profiled via `cProfile`: `numpy.nan_to_num` consumed 5.412s (>45% of total CPU time) across 346,260 invocations in a single 100-gen run due to redundant wrapping in both `primitives.py` and `node.py`.
- Redundant re-evaluation of ~90% of individuals in `engine.py` (evaluated during breeding for crossover statistics, then re-evaluated at the start of the next generation).
- In `epsilon_lexicase_selection`, dynamic MAD computation triggered 87,940 calls to `np.median` in just 5 generations on `concrete`, taking 92% of lexicase runtime.
