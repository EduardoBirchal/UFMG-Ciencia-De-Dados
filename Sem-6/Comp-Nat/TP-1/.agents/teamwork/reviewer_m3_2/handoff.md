# Milestone 3 Independent Review & Adversarial Challenge Report

**Reviewer**: `reviewer_m3_2` (Reviewer 2, Roles: reviewer, critic)  
**Target Milestone**: Milestone 3 (Selection, Variation & Evolution Engine)  
**Parent Conversation ID**: `0fbe2801-fce8-49f2-8814-84768519d3f0`  
**Date**: 2026-10-26  
**Verdict**: **APPROVE**  

---

## 1. Observation

### 1.1 Direct Inspection of Work Products
- **`src/gp/selection.py`** (140 lines):
  - `tournament_selection` (lines 7–36): Implements $k$-candidate selection with replacement (`generator.integers(0, n_candidates, size=k_eff)`), picking candidate with minimal RMSE fitness via `np.argmin(selected_fits)`. Supports positional or keyword arguments for fitnesses.
  - `scaled_roulette_selection` (lines 38–74): Implements linear shift weights $w_i = (f_{max} - f_i) + \delta$ with floor clamping `np.maximum(weights, delta)` and robust normalization (`if not np.isfinite(total_w) or total_w <= 0.0: probs = 1.0 / n`).
  - `epsilon_lexicase_selection` (lines 79–125): Dynamic MAD threshold filtering following Equations 1 & 2. For each shuffled case $t$, computes $e^* = \min(e_j)$, $\text{med} = \text{median}(e_j)$, $\text{MAD} = \text{median}(|e_j - \text{med}|)$, and survival threshold $e^* + \text{MAD} + 10^{-12}$. Breaks when $|C| \le 1$, and draws uniformly if ties persist after all cases.
  - Exported aliases: `roulette_selection = scaled_roulette_selection`, `lexicase_selection = epsilon_lexicase_selection`.
- **`src/gp/operators.py`** (184 lines):
  - `subtree_crossover` (lines 10–55): Functional immutable tree replacement via `replace_subtree`. Rejects any offspring with `depth() > max_depth` (7) and cleanly returns an exact deep copy of parent 1 (`root1.copy()`, `success = False`).
  - `subtree_mutation` (lines 63–110): Replaces random prefix node with newly grown subtree (grow depth 1 to 3). Rejects any offspring with `depth() > max_depth` (7) and returns exact parent clone.
  - `gaussian_constant_mutation` (lines 112–137): Deep copies parent first, selects a random Ephemeral Random Constant (ERC), perturbs value with $\mathcal{N}(0, \sigma^2)$, and rounds to 4 decimals.
  - `mutate` (lines 142–167): Dispatches with 50% probability to Gaussian constant mutation if constants exist, otherwise subtree mutation.
  - `reproduce` (lines 169–172): Returns `parent.copy()`.
- **`src/gp/metrics.py`** (102 lines):
  - `compute_rmse` (lines 7–18): Vectorized RMSE calculation with numerical overflow and NaN protection (`np.nan_to_num(res, nan=1e9, posinf=1e9, neginf=0.0)`).
  - `count_duplicates` (lines 20–33): Syntactic duplicate counting via canonical tree serialization (`len(population) - len(set(strings))`).
  - `MetricTracker` (lines 35–97): Generational logging storing standard and alias names (`gen`/`generation`, `train_rmse_best`/`best_train_rmse`, `train_rmse_worst`, `train_rmse_mean`/`mean_train_rmse`, `test_rmse_best`/`best_test_rmse`, `duplicate_trees`/`duplicate_canonical_trees`, `unique_canonical_trees`, `crossover_better`/`crossover_better_count`, `crossover_worse`/`crossover_worse_count`, `crossover_equal`, `crossover_rejected`, `crossover_total`, `avg_nodes`/`avg_node_count`, `avg_depth`, `best_nodes`/`best_node_count`, `best_depth`, `elapsed_seconds`).
- **`src/gp/engine.py`** (273 lines):
  - `GPEngine`: Coordinates initial population via `ramped_half_and_half(pop_size, n_features, 2, 5)`, generational evaluation, selection, mutually exclusive variation sampling ($p_c, p_m$, reproduction), slot 0 elitism preservation (`offspring.append(self.best_individual_.copy())`), and metric tracking.
  - `predict`: Vectorized prediction over feature matrices using `best_individual_`.
- **`tests/test_gp_evolution.py`** (277 lines):
  - 20 unit tests verifying selection mechanisms, crossover depth ceiling, mutations, reproduction, and full 10-generation run on `synth1`.

### 1.2 Test Execution Results
- **Full Pytest Suite**:
  - Command: `d:\Downloads\bases-TP1\datasets\.venv\Scripts\python.exe -m pytest tests/ -v`
  - Output: `=== 164 passed in 2.50s ===` (0 skipped, 0 failed).
- **E2E Suite Verification**:
  - Command: `d:\Downloads\bases-TP1\datasets\.venv\Scripts\python.exe -m pytest tests/e2e/ -v`
  - Output: `=== 94 passed in 1.22s ===` (all 45 previously skipped E2E tests across Tiers 1–4 now run and pass).
- **M3 Evolution Unit Tests**:
  - Command: `d:\Downloads\bases-TP1\datasets\.venv\Scripts\python.exe -m pytest tests/test_gp_evolution.py -v`
  - Output: `=== 20 passed in 0.95s ===`.
- **Style Linter (R2 Compliance)**:
  - Command: `d:\Downloads\bases-TP1\datasets\.venv\Scripts\python.exe scripts/check_style.py`
  - Output: `all 26 files passed style checks` (0 uppercase comments/docstrings, 0 trailing periods).

### 1.3 Adversarial Stress-Test Verification
- **Empirical Stress Suite (`scripts/stress_m3.py`)**:
  - Command: `d:\Downloads\bases-TP1\datasets\.venv\Scripts\python.exe scripts/stress_m3.py`
  - Selection operator distributions:
    - Tournament $k=2$ over 10,000 trials: $\chi^2 = 3.7407, p = 0.9276$ (no significant deviation from theoretical ranks).
    - Scaled roulette over 30,000 trials: $\chi^2 = 1.5806, p = 0.4537$ (matches theoretical probabilities). Identical fitnesses sampled uniformly ($\sim 25\%$ each).
    - $\epsilon$-lexicase over 1,000 trials: Successfully filters sub-optimal candidates; handles $\text{MAD} = 0$ ties cleanly without candidate extinction.
  - Depth ceiling limit enforcement:
    - 5,000 trials with ceiling max_depth = 7: 1,024 rejections, 0 depth violations, 0 clone failures.
    - 5,000 trials with ceiling max_depth = 3: 1,166 rejections, 0 depth violations, 0 clone failures.
  - Elitism monotonicity:
    - 10 independent runs of 20 generations on `synth1` with `elitism=True`: 100% strictly non-increasing best train RMSE.
    - 1 run with `elitism=False`: 7 fitness degradations observed, demonstrating that monotonicity is specifically enforced by elitism preservation.
- **Direct Edge Case Shell Tests**:
  - Tested single-candidate pool selection ($N=1$): Tournament, Roulette, and Lexicase all safely return 0 without error.
  - Tested extreme fitness disparities ($[0.0, 10^{15}]$ and $[0.0, 10^{20}]$): Roulette selection computes finite weights without division by zero.
  - Tested 5,000 randomized crossover operations with depth 7 parents: Offspring depth is strictly $\le 7$ in 100% of operations.

---

## 2. Logic Chain

1. **Integrity and Authenticity**:
   - Inspected source code in `src/gp/selection.py`, `src/gp/operators.py`, `src/gp/metrics.py`, and `src/gp/engine.py`.
   - Implementations are built from scratch using NumPy and standard Python data structures.
   - No hardcoded test outputs or dummy facade methods exist.
   - Verification commands and test suites execute genuine calculations and achieve 100% pass rates.
   - Conclusion: Zero integrity violations.

2. **Mathematical and Algorithmic Correctness**:
   - `tournament_selection` directly samples indices with replacement and picks the minimal error index via `np.argmin`, matching the minimization objective of RMSE.
   - `scaled_roulette_selection` computes $w_i = (f_{max} - f_i) + \delta$, which assigns highest selection probability to the lowest RMSE candidate while maintaining non-zero selection chance $\delta$ for the worst candidate.
   - `epsilon_lexicase_selection` implements the exact specification of Equations 1 and 2: dynamic median absolute deviation threshold per test case over the active candidate pool $C_t$. The inclusion of the $10^{-12}$ buffer handles the edge case where all candidates have identical errors ($\text{MAD} = 0$), preventing spurious disqualification.
   - `subtree_crossover` and `subtree_mutation` use the functional `replace_subtree` method from `node.py`. When `new_root.depth() > 7`, the candidate tree is rejected and `root1.copy()` is returned. This guarantees that no individual in the population ever exceeds depth 7.
   - `GPEngine` preserves the overall best individual in slot 0 via deep copy (`self.best_individual_.copy()`) when `elitism=True`. This mathematically guarantees non-increasing best training RMSE across generations, as verified by the 10 monotonicity test runs.

3. **Code Style and Human Engineering Constraints (R2)**:
   - All comments and docstrings in `src/gp/selection.py`, `src/gp/operators.py`, `src/gp/metrics.py`, `src/gp/engine.py`, and `tests/test_gp_evolution.py` are casual, all-lowercase, and omit trailing periods.
   - The automated linter `scripts/check_style.py` validated all 26 files in the repository with zero violations.

4. **Integration Compatibility**:
   - `MetricTracker` normalizes key names and aliases, ensuring downstream reporting modules (`src/reporting/plotter.py` and `src/reporting/tables.py`) as well as the independent E2E test suites (`tests/e2e/`) consume generational history seamlessly.

---

## 3. Caveats

- **No Caveats**: The entire scope of Milestone 3 was independently reviewed, stress-tested, and validated against analytical distributions, extreme boundary conditions, and end-to-end integration requirements. All tests pass with zero warnings, zero skips, and zero failures.

---

## 4. Conclusion

Milestone 3 meets all specification requirements, maintains high code quality, adheres strictly to the R2 casual lowercase style convention, and demonstrates complete mathematical robustness under adversarial conditions.

**Final Verdict: APPROVE**

---

## 5. Verification Method

To independently reproduce and verify this review:

1. **Run Full Pytest Suite**:
   ```powershell
   d:\Downloads\bases-TP1\datasets\.venv\Scripts\python.exe -m pytest tests/ -v
   ```
   *Expected outcome*: 164 passed in ~2.5s.

2. **Run E2E Test Suite**:
   ```powershell
   d:\Downloads\bases-TP1\datasets\.venv\Scripts\python.exe -m pytest tests/e2e/ -v
   ```
   *Expected outcome*: 94 passed in ~1.2s.

3. **Run M3 Unit Test Suite**:
   ```powershell
   d:\Downloads\bases-TP1\datasets\.venv\Scripts\python.exe -m pytest tests/test_gp_evolution.py -v
   ```
   *Expected outcome*: 20 passed in ~1.0s.

4. **Run Empirical Stress Suite**:
   ```powershell
   d:\Downloads\bases-TP1\datasets\.venv\Scripts\python.exe scripts/stress_m3.py
   ```
   *Expected outcome*: Passes all selection chi-square tests, 10,000 depth ceiling trials, and 10/10 elitism monotonicity checks; outputs "APPROVE".

5. **Run R2 Code Style Linter**:
   ```powershell
   d:\Downloads\bases-TP1\datasets\.venv\Scripts\python.exe scripts/check_style.py
   ```
   *Expected outcome*: "all 26 files passed style checks" (exit code 0).

### Invalidation Conditions
- Any occurrence of tree depth $> 7$ in crossover or mutation outputs.
- Any non-monotonic increase in best train RMSE when `elitism=True`.
- Any style linter violation reported by `scripts/check_style.py`.
- Any test failure in `pytest tests/`.
