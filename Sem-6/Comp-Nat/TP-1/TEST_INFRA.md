# Test Infrastructure Specification: Genetic Programming Symbolic Regression

## 1. Overview & Test Architecture

This document defines the complete end-to-end (E2E) opaque-box test infrastructure for the Genetic Programming (GP) framework for symbolic regression. The test suite guarantees correctness, numerical robustness, algorithmic integrity, and strict adherence to the project specification (`ORIGINAL_REQUEST.md` and `PROJECT.md`).

### Test Suite Structure
```
tests/
├── e2e/
│   ├── __init__.py
│   ├── test_tier1_features.py       # Isolated feature happy-paths (>=5 tests per feature)
│   ├── test_tier2_boundaries.py     # Boundary values, numerical protections, corner cases (>=5 tests per feature)
│   ├── test_tier3_combinations.py   # Pairwise interactions between operators, selection, and elitism
│   └── test_tier4_scenarios.py      # Real-world regression runs on synth1, synth2, and concrete
```

---

## 2. Test Taxonomy (Tiers 1 to 4)

### Tier 1: Isolated Feature Happy-Paths (`tests/e2e/test_tier1_features.py`)
Validates each component in total isolation against standard inputs:
- **Feature 1: Dataset Ingestion (`src.data.load_dataset`)**
  - Shape, dtype (float64), contiguous array checks for `synth1`, `synth2`, and `concrete`.
  - Verification that features and target are correctly partitioned (last column is target).
  - Validation of non-existent dataset exception handling.
- **Feature 2: Protected Primitives (`src.gp.primitives`)**
  - Standard arithmetic: `safe_add`, `safe_sub`, `safe_mul`.
  - Non-linear functions: `safe_sin`, `safe_cos`, `safe_log`, `safe_exp`.
  - Protected division: `safe_div`.
- **Feature 3: Syntax Tree Node (`src.gp.node.Node`)**
  - Node creation: leaf constant, leaf variable, internal arithmetic/unary nodes.
  - Structural metrics: `depth()`, `node_count()`.
  - Canonical serialization: `to_string()`.
  - Deep copying: `copy()` creates independent clones.
- **Feature 4: Vectorized Tree Evaluation (`src.gp.node.Node.eval` / `src.gp.tree`)**
  - Vectorized evaluation over 2D NumPy feature matrices ($N \times D$).
  - Correct broadcasting of scalar constants across all rows.
  - Column indexing for variables $X_0 \dots X_{D-1}$.
- **Feature 5: Tree Initialization (`src.gp.tree`)**
  - Ramped Half-and-Half algorithm generating trees spanning depths 2 to 5.
  - Balanced allocation between `Full` and `Grow` methods.
  - Validity of generated syntax trees (well-formed terminals and primitives).
- **Feature 6: Fitness Evaluation (`src.gp.metrics` / `src.gp.tree`)**
  - Root Mean Squared Error (RMSE) against known targets.
  - Population batch RMSE evaluation returning 1D array of shape $(M,)$.
- **Feature 7: Selection Operators (`src.gp.selection`)**
  - Tournament selection ($k=2$).
  - Scaled Roulette wheel selection with linear shift ($f_{max} - f_i + \delta$).
  - $\epsilon$-Lexicase selection with dynamic MAD thresholding.
- **Feature 8: Variation Operators (`src.gp.operators`)**
  - Subtree crossover producing valid offspring.
  - Gaussian Ephemeral Random Constant (ERC) perturbation mutation.
  - Subtree replacement mutation using Grow method.
  - Direct cloning / reproduction.
- **Feature 9: Metric Logging (`src.gp.metrics`)**
  - Generation statistics: best, worst, mean train RMSE; test RMSE of best individual.
  - Canonical duplicate string counting ($M - |\text{unique}|$).
  - Crossover offspring improvement counters (`crossover_better`, `crossover_worse`).
  - Bloat tracking: average node count and average depth.
- **Feature 10: Evolution Engine (`src.gp.engine.GPEngine`)**
  - Population lifecycle coordination.
  - Seed reproducibility across consecutive runs.
  - Monotonic fitness preservation under elitism.

---

### Tier 2: Boundary & Corner Cases (`tests/e2e/test_tier2_boundaries.py`)
Validates system limits, extreme inputs, and protective guards:
- **Depth Ceiling $\le 7$ Limit Enforcement**:
  - Rejection of crossover offspring exceeding depth 7, cleanly reverting to parent $P_1$.
  - Rejection of mutation offspring exceeding depth 7, cleanly reverting to parent $P$.
  - Trees of depth exactly 7 are allowed; depth 8 is rejected.
- **Numerical Protections & Overflow Elimination**:
  - Division by zero: $|y| < 10^{-6} \implies 1.0$.
  - Division by tiny numbers: avoiding float overflow.
  - Logarithm of zero: $|x| < 10^{-6} \implies 0.0$.
  - Logarithm of negative inputs: $|x| < 10^{-6} \implies 0.0$ else $\ln(|x|)$.
  - Exponential overflow clamping: $\exp(\text{clip}(x, -32.0, 32.0))$.
  - Multiplicative and additive extreme bounds clipping.
  - Absolute guarantee of zero `NaN` and zero `Inf` outputs across all evaluations.
- **Selection Edge Conditions**:
  - $\epsilon$-Lexicase selection with identical candidate error vectors ($\text{MAD} = 0$, numerical buffer $+10^{-12}$).
  - $\epsilon$-Lexicase when candidate pool reduces to exactly 1 candidate (immediate return).
  - $\epsilon$-Lexicase when all test cases are exhausted with multiple candidates remaining (uniform random tie-break).
  - Scaled Roulette selection when all individuals have identical fitness ($f_{max} == f_i \implies w_i = \delta$, uniform selection).
  - Scaled Roulette selection under extreme fitness disparities ($10^9$ vs $0.0$, no zero-probability crash).
  - Tournament selection with $M=2$ (minimal population).
- **Data & Dimension Extremes**:
  - Single sample feature matrix ($N=1$).
  - High dimensional feature matrix ($D=100$).
  - Minimal population ($M=2$) and 0 generations.

---

### Tier 3: Pairwise Combinations (`tests/e2e/test_tier3_combinations.py`)
Validates cross-feature interactions and parameter regimes:
- **Selection Operator $\times$ Elitism**:
  - Tournament selection with Elitism=True vs Elitism=False.
  - Scaled Roulette selection with Elitism=True vs Elitism=False.
  - $\epsilon$-Lexicase selection with Elitism=True vs Elitism=False.
- **Operator Probability Regimes**:
  - High crossover regime: $p_c=0.90, p_m=0.05, p_r=0.05$.
  - High mutation regime: $p_c=0.60, p_m=0.30, p_r=0.10$.
  - Crossover-only regime: $p_c=1.0, p_m=0.0, p_r=0.0$.
  - Mutation-only regime: $p_c=0.0, p_m=1.0, p_r=0.0$.
  - Reproduction-only regime: $p_c=0.0, p_m=0.0, p_r=1.0$ (preserves population).
- **Initialization Depths $\times$ Problem Dimensions**:
  - Ramped Half-and-Half on $D=1, 2, 8$ dimensions.
  - Verification that variable terminals never sample column indices $\ge D$.
- **Selection Pressure $\times$ Syntactic Diversity**:
  - Measurement of duplicate canonical strings across selection regimes over generations.

---

### Tier 4: Real-World Scenarios (`tests/e2e/test_tier4_scenarios.py`)
Validates complete regression workflows on actual benchmark datasets:
- **Scenario 1: `synth1` Symbolic Regression Run**:
  - Load `synth1` ($N_{train}=60, N_{test}=600, D=2$).
  - Run GPEngine for multiple generations with tournament selection and elitism.
  - Verify convergence (decreasing or non-increasing best train RMSE).
  - Verify full per-generation metric history logging.
- **Scenario 2: `synth1` Analytical Ground Truth Verification**:
  - Manual construction of the ground truth tree:
    $$y = X_0^4 - X_0^3 + 0.5 X_1^2 - X_1$$
  - Evaluate on `synth1-train.csv` and verify RMSE is near zero ($< 10^{-10}$).
  - Evaluate on `synth1-test.csv` and verify test RMSE is near zero ($< 10^{-10}$).
- **Scenario 3: `synth2` Non-linear Surface Run**:
  - Load `synth2` ($N_{train}=300, N_{test}=1000, D=2$).
  - Run GPEngine with $\epsilon$-Lexicase selection.
  - Verify stability on bivariate oscillatory landscape.
- **Scenario 4: `concrete` Multidimensional Engineering Run**:
  - Load `concrete` ($N_{train}=824, N_{test}=206, D=8$).
  - Run GPEngine with Scaled Roulette selection.
  - Verify all 8 features ($X_0 \dots X_7$) are supported without indexing errors.
- **Scenario 5: Statistical Summary & Table Consistency**:
  - Verify aggregation across multiple seeds yields valid mean and standard deviation metrics.

---

## 3. Authoritative Oracles & Derivations

| Test Domain | Input Condition | Expected Oracle Output | Source |
|---|---|---|---|
| `synth1` Ground Truth | $X_0, X_1$ inputs | $y = X_0^4 - X_0^3 + 0.5 X_1^2 - X_1$ (RMSE $< 10^{-10}$) | `ORIGINAL_REQUEST.md`, empirical polynomial fitting |
| Protected Division | $x \in \mathbb{R}, |y| < 10^{-6}$ | $1.0$ | Koza protected division specification |
| Protected Logarithm | $|x| < 10^{-6}$ | $0.0$ | Koza protected log specification |
| Protected Logarithm | $|x| \ge 10^{-6}$ | $\ln(|x|)$ | Koza protected log specification |
| Protected Exponential | $x \in \mathbb{R}$ | $\exp(\text{clip}(x, -32.0, 32.0))$ | Explorer specification, prevents float64 overflow |
| Maximum Depth Ceiling | Tree with depth $> 7$ after variation | Offspring rejected; unmodified parent preserved | `ORIGINAL_REQUEST.md` line 16 |
| Dynamic MAD Lexicase | Errors $\mathbf{e}$ on test case $t$ | $\text{MAD} = \text{median}(\|\mathbf{e} - \text{median}(\mathbf{e})\|)$, filter $\le e^* + \text{MAD} + 10^{-12}$ | Equation 1 & 2 of project specification |
| Scaled Roulette Selection | Fitness vector $\mathbf{f}$ (RMSE) | Weight $w_i = (f_{max} - f_i) + 10^{-4}$, normalized to $p_i$ | Linear shift specification |
| Elitism Preservation | Generation $g+1$, slot 0 | Identical copy of individual with minimum train RMSE at gen $g$ | `ORIGINAL_REQUEST.md` line 26 |

---

## 4. Test Execution Instructions

### Running via Pytest
All tests must be executed with the project's Python 3.14 virtual environment:

```powershell
# Run the entire E2E test suite
.\.venv\Scripts\python.exe -m pytest tests/e2e/ -v

# Run individual tiers
.\.venv\Scripts\python.exe -m pytest tests/e2e/test_tier1_features.py -v
.\.venv\Scripts\python.exe -m pytest tests/e2e/test_tier2_boundaries.py -v
.\.venv\Scripts\python.exe -m pytest tests/e2e/test_tier3_combinations.py -v
.\.venv\Scripts\python.exe -m pytest tests/e2e/test_tier4_scenarios.py -v
```

### Style Compliance Verification (R2)
All comments and docstrings in the test suite must strictly follow the casual lowercase rule:
```powershell
# Verify zero uppercase characters and zero trailing periods in comments and docstrings
.\.venv\Scripts\python.exe -c "
import ast, tokenize, glob

def check(p):
    with open(p, 'rb') as f:
        for tok in tokenize.tokenize(f.readline):
            if tok.type == tokenize.COMMENT:
                c = tok.string.lstrip('#').strip()
                if c and (c != c.lower() or c.endswith('.')):
                    print(f'comment violation in {p}:{tok.start[0]}: {tok.string}')
    with open(p, 'r', encoding='utf-8') as f:
        tree = ast.parse(f.read())
        for n in ast.walk(tree):
            if isinstance(n, (ast.FunctionDef, ast.ClassDef, ast.Module)):
                d = ast.get_docstring(n)
                if d and (d.strip() != d.strip().lower() or d.strip().endswith('.')):
                    print(f'docstring violation in {p}:{getattr(n, \"lineno\", 1)}')

for p in glob.glob('tests/e2e/*.py'):
    check(p)
"
```

---

## 5. Performance & Safety Limits

- **Maximum Single Test Runtime**: $< 15$ seconds (far below the 15-minute global threshold).
- **Total E2E Suite Runtime**: $< 60$ seconds.
- **Memory Consumption**: $< 500$ MB RAM.
- **Determinism**: Fully deterministic via explicit random seeds (`seed=42`).
