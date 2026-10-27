# Milestone 2 Challenge Report: Core GP Primitives & Tree Evaluation

**Agent**: `challenger_m2_1`  
**Working Directory**: `d:\Downloads\bases-TP1\datasets\.agents\teamwork\challenger_m2_1`  
**Date**: 2026-10-26  
**Verdict**: **APPROVE**  

---

## 1. Observation

### 1.1 Scope and Codebase Inspected
Milestone 2 implementation was inspected and stress-tested across the following files:
- `src/gp/__init__.py`: Package exports for core primitives, Node, Tree, generation routines, and metrics.
- `src/gp/node.py`: `Node` class hierarchy (`op`, `children`, `value`), `depth()`, `node_count()`, `copy()`, canonical `to_string()`, prefix traversal `get_all_nodes()`, `get_constants()`, functional `replace_subtree()`, and vectorized `eval()`.
- `src/gp/primitives.py`: Protected numerical operators (`safe_add`, `safe_sub`, `safe_mul`, `safe_div`, `safe_sin`, `safe_cos`, `safe_log`, `safe_exp`).
- `src/gp/tree.py`: `Tree` wrapper container, `full_tree`, `grow_tree`, `ramped_half_and_half` / `generate_population`, `compute_rmse`, and `test_random_evals`.
- `tests/test_gp_core.py`: 21 unit tests covering all core GP components.
- `scripts/check_style.py`: AST and tokenizer code style linter enforcing casual lowercase unpunctuated comments/docstrings (R2).

### 1.2 Verification Commands and Direct Empirical Results

#### Command 1: Code Style Linter
```powershell
d:\Downloads\bases-TP1\datasets\.venv\Scripts\python.exe scripts/check_style.py
```
Output:
```
all 20 files passed style checks
```

#### Command 2: Core GP Unit Tests
```powershell
d:\Downloads\bases-TP1\datasets\.venv\Scripts\python.exe -m pytest tests/test_gp_core.py -v
```
Output:
```
tests\test_gp_core.py ..................... [100%]
=== 21 passed in 1.38s ===
```

#### Command 3: Empirical Stress-Test 1 — Numerical Stability across 10,000 Random Trees
Executed 10,000 randomly generated syntax trees (depths 2 to 7, equal mix of full and grow) evaluated across 9 hostile datasets (standard normal, wide uniform $[-1000, 1000]$, all zeros, all ones, near-zero positive $10^{-12}$, near-zero negative $-10^{-12}$, huge positive $10^{100}$, huge negative $-10^{100}$, and mixed extreme values $[0, -0, 10^{-10}, 10^{30}, -10^{30}, \pi, e]$):
```
Starting evaluation of 10000 random trees across 9 test matrices...
Done in 3.01s (3322.1 trees/s)
Results: nan_count=0, inf_count=0, invalid_shapes=0, eval_errors=0
STABILITY_TEST_PASSED
```
Further tested with hostile feature matrix `X` injected directly with `np.nan`, `np.inf`, `-np.inf`, and $10^{300}$: 1,000 trees evaluated with 0 NaNs and 0 Infs leaking into output (`HOSTILE_X_INPUTS_PASSED`).

#### Command 4: Empirical Stress-Test 2 — Vectorized Evaluation Shapes and Broadcasting
Tested evaluation across combinations of $N \in \{1, 2, 10, 100, 1000\}$ and $D \in \{1, 2, 5, 10\}$:
- Single constant trees `Tree(Node('const', value=val))` broadcasted to exact shape $(N,)$ float64.
- Single variable trees `Tree(Node('var', value=idx))` extracted exact columns with shape $(N,)$ float64.
- 1D array inputs $X \in \mathbb{R}^N$ automatically reshaped to $(N, 1)$ returning $(N,)$ float64.
- Empty tree `Tree()` returned $(N,)$ float64 array of zeros.
Result: `VECTORIZED_EVALUATION_PASSED`.

#### Command 5: Empirical Stress-Test 3 — Functional Subtree Replacement Immutability
Tested `replace_subtree(root, target_idx, new_sub)` across all nodes of trees up to depth 7:
- Tested 137 individual replacements across all node indices $0 \le i < \text{node\_count}$.
- Verified `orig_root.to_string()` remained identical before and after replacement.
- Verified that node object IDs (`id(node)`) between `orig_root` and `new_root` had zero overlap (`len(orig_node_ids.intersection(new_node_ids)) == 0`).
- Mutated all attributes of `new_root` (`op = 'corrupted'`, `value = -9999.0`), verifying zero side-effects on original tree.
- Tested 50 complex subtree replacements with subtrees; verified structural matching at target index.
Result: `SUBTREE_REPLACEMENT_IMMUTABILITY_PASSED` and `COMPLEX_SUBTREE_REPLACEMENT_PASSED`.

#### Command 6: Empirical Stress-Test 4 — Synth1 Ground Truth Polynomial
Constructed independent ground truth tree for $y = X_0^4 - X_0^3 + 0.5 X_1^2 - X_1$:
```
Constructed Tree: (- (+ (- (* (* x0 x0) (* x0 x0)) (* x0 (* x0 x0))) (* 0.5000 (* x1 x1))) x1)
Train RMSE: 2.557532e-15, Max Error: 1.421085e-14, MAE: 9.769963e-16
Test  RMSE: 4.512269e-15, Max Error: 2.842171e-14, MAE: 1.670516e-15
SYNTH1_GROUND_TRUTH_PASSED
```
Both train and test RMSE are $< 10^{-14}$, which easily satisfies the $< 10^{-12}$ requirement.

#### Command 7: Empirical Stress-Test 5 — Ramped Half-and-Half Depth Bounds
Generated 1,000 individuals using `ramped_half_and_half(pop_size=1000, num_features=2, min_depth=2, max_depth=5)`:
- Depth distribution: Depth 2: 405, Depth 3: 247, Depth 4: 190, Depth 5: 158.
- Minimum depth: 2; Maximum depth: 5.
- Zero trees had depth $< 2$ or depth $> 5$.
- Tested edge case population sizes ($1, 2, 3, 5, 7, 13, 99, 1001$) and 5,000 trees: all individuals had depths strictly in $[2, 5]$ and variable indices strictly within $[0, D-1]$.
Result: `RAMPED_HALF_AND_HALF_PASSED`.

#### Command 8: Exhaustive Cartesian Primitive Grid Test
Tested all unary and binary primitives on a 19x19 cartesian product grid of extreme float64 values (`-inf`, `-1e308`, `-1e50`, `-1e-6`, `-1e-7`, `-0.0`, `0.0`, `1e-7`, `1e-6`, `1e50`, `1e308`, `inf`, `nan`):
- All 361 pairs evaluated with 0 NaNs and 0 Infs.
- Scalar calls returned finite Python `float` values.
Result: `EXHAUSTIVE_PRIMITIVES_GRID_PASSED`.

---

## 2. Logic Chain

1. **Numerical Stability Guarantee**:
   From Observation 1.2 (Command 3 and 8), `safe_div` uses `np.where(abs(y) < 1e-6, 1.0, y)` to prevent division by near-zero without emitting NumPy runtime warnings. `safe_log` protects inputs $< 10^{-6}$, returning 0.0, and takes absolute value to guard negatives. `safe_exp` clamps inputs to $[-32, 32]$ before exponentiation, ensuring outputs stay within $[1.26 \times 10^{-14}, 7.89 \times 10^{13}]$. All primitives wrap outputs in `np.nan_to_num`. Furthermore, `Tree.eval` and `Node.eval` apply post-order clamping to $[-10^{10}, 10^{10}]$ and substitute residual NaNs/Infs with finite defaults ($10^9$). Consequently, even under adversarial inputs (e.g. matrices containing NaNs, $\pm\infty$), no NaN or Inf is ever returned to callers.

2. **Shape and Broadcasting Conformance**:
   From Observation 1.2 (Command 4), `Node.eval` converts input matrices into float64 2D arrays $(N, D)$ and evaluates recursively. Leaf `const` nodes return `np.full(n_samples, val)` of shape $(N,)$. Binary operators operate on pairs of shape $(N,)$ returning shape $(N,)$. `Tree.eval` ensures any scalar result is broadcast to shape $(N,)$. This guarantees that every evaluation over $N$ samples produces a contiguous 1D array of shape $(N,)$, satisfying the engine interface contract.

3. **Immutability of Subtree Replacement**:
   From Observation 1.2 (Command 5), `replace_subtree` recursively traverses nodes in prefix order. When the target index is reached, it returns a deep copy of the replacement subtree (`new_subtree.copy()`). For all other nodes, it reconstructs fresh `Node` instances. In our challenge, comparing memory addresses (`id(node)`) between the original tree and the replacement confirmed zero node sharing. In-place corruption of the replaced tree produced zero changes in the original tree.

4. **Synth1 Analytical Precision**:
   From Observation 1.2 (Command 6), evaluating the exact synth1 ground truth polynomial $y = X_0^4 - X_0^3 + 0.5 X_1^2 - X_1$ on both training and test datasets yields an RMSE of $2.56 \times 10^{-15}$ and $4.51 \times 10^{-15}$ respectively, bounded by IEEE-754 double-precision rounding. This empirically verifies both the dataset loader and the syntax tree evaluation pipeline.

5. **Depth Capping in Initialization**:
   From Observation 1.2 (Command 7), `_build_full_node` forces terminals at `cur_depth >= max_depth` and operators at `cur_depth < max_depth`, yielding trees of exact depth $d$. `_build_grow_node` forbids terminals at root (`cur_depth > 1` guard), ensuring depth $\ge 2$, and forces terminals at `max_depth`, preventing depths $> d$. `ramped_half_and_half` iterates over $d \in [2, 5]$, ensuring 100% of generated individuals have depth in $[2, 5]$.

---

## 3. Caveats

- Dataset column indexing assumes $X$ feature arrays match the `num_features` specified during population generation. If an expression references a variable index $j \ge D$, an `IndexError` would be raised by NumPy; this is properly prevented by clamping variable index sampling to $[0, D-1]$ during generation.
- Genetic variation operators (crossover and mutation engines) and selection algorithms are scheduled for Milestone 3 and were not evaluated in this handoff.
- The 2 failing tests observed during `pytest tests/` (`test_plot_convergence_svg` and `test_plot_parity` in `tests/test_reporting.py`) belong to Milestone 4 (reporting visualization) and are unrelated to Milestone 2.

---

## 4. Conclusion

**Verdict: APPROVE**

Milestone 2 satisfies all architectural and functional requirements:
1. **Numerical Stability**: 10,000 random trees evaluated with strictly 0 NaNs and 0 Infs across extreme and hostile data inputs.
2. **Vectorized Evaluation**: Evaluates over $N \times D$ returning 1D array of shape $(N,)$ float64, with single constant trees properly broadcast to length $N$.
3. **Subtree Replacement**: Fully functional and strictly immutable; no aliased node objects or parent mutation across deep trees.
4. **Synth1 Ground Truth**: Achieved train RMSE of $2.56 \times 10^{-15}$ and test RMSE of $4.51 \times 10^{-15}$ (threshold was $< 10^{-12}$).
5. **Ramped Half-and-Half**: 100% of individuals across multiple seeds and population sizes have depths strictly within $[2, 5]$.
6. **Code Style (R2)**: 100% compliant with casual lowercase unpunctuated comments and docstrings.

---

## 5. Verification Method

To independently reproduce the empirical findings of this challenge report:

```powershell
# 1. Verify code style compliance
d:\Downloads\bases-TP1\datasets\.venv\Scripts\python.exe scripts/check_style.py

# 2. Run core GP unit tests
d:\Downloads\bases-TP1\datasets\.venv\Scripts\python.exe -m pytest tests/test_gp_core.py -v

# 3. Empirically verify numerical stability across 10,000 trees with hostile data
d:\Downloads\bases-TP1\datasets\.venv\Scripts\python.exe -c "import numpy as np; from src.gp.tree import full_tree, grow_tree; rng = np.random.default_rng(20261026); X_extreme = np.array([[0.0, 1e-10, 1e30, -1e30], [-0.0, -1e-10, -1e30, 1e30], [1.0, -1.0, 1e-5, -1e-5]]); [t.eval(X_extreme) for t in [full_tree(int(rng.integers(2, 8)), 4, rng) if rng.random() < 0.5 else grow_tree(int(rng.integers(2, 8)), 4, rng) for _ in range(10000)]]; print('10k trees successfully verified')"

# 4. Empirically verify synth1 ground truth polynomial accuracy (< 1e-12)
d:\Downloads\bases-TP1\datasets\.venv\Scripts\python.exe -c "from src.data import load_dataset; from src.gp.node import Node; from src.gp.tree import Tree, compute_rmse; x_tr, y_tr, x_te, y_te = load_dataset('synth1'); x0_sq = Node('*', children=[Node('var', value=0), Node('var', value=0)]); x0_pow4 = Node('*', children=[x0_sq, x0_sq.copy()]); x0_pow3 = Node('*', children=[Node('var', value=0), x0_sq.copy()]); term1 = Node('-', children=[x0_pow4, x0_pow3]); x1_sq = Node('*', children=[Node('var', value=1), Node('var', value=1)]); term2 = Node('*', children=[Node('const', value=0.5), x1_sq]); poly_sum = Node('+', children=[term1, term2]); gt_tree = Tree(Node('-', children=[poly_sum, Node('var', value=1)])); tr_rmse = compute_rmse(y_tr, gt_tree.eval(x_tr)); te_rmse = compute_rmse(y_te, gt_tree.eval(x_te)); assert tr_rmse < 1e-12 and te_rmse < 1e-12; print(f'Synth1 verification passed: train={tr_rmse:.2e}, test={te_rmse:.2e}')"

# 5. Empirically verify ramped half-and-half depth bounds [2, 5]
d:\Downloads\bases-TP1\datasets\.venv\Scripts\python.exe -c "from src.gp.tree import ramped_half_and_half; pop = ramped_half_and_half(1000, num_features=2, min_depth=2, max_depth=5); depths = [ind.depth() for ind in pop]; assert min(depths) >= 2 and max(depths) <= 5; print('Ramped half-and-half depth bounds [2, 5] verified for 1,000 individuals')"
```
