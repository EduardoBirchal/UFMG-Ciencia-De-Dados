# Project: Modular Genetic Programming Framework for Symbolic Regression

## Architecture
Modular Python/NumPy architecture designed for symbolic regression, running under Python 3.14 (`.venv`):
- `src/data.py`: Pre-split dataset loader for `synth1`, `synth2`, and `concrete` (contiguous NumPy float64 arrays, header=None).
- `src/gp/node.py`: Syntax tree node hierarchy, tree copying, string serialization, depth and node counting, functional subtree replacement.
- `src/gp/primitives.py`: Vectorized primitives with numerical protections: `+`, `-`, `*`, safe `/`, `sin`, `cos`, safe `log`, safe `exp` (clamped to [-32, 32]).
- `src/gp/tree.py`: Individual syntax tree wrapper, ramped half-and-half generator (depths 2-5, grow and full), vectorized NumPy evaluation.
- `src/gp/selection.py`: Selection operators: Tournament ($k=2$), Scaled Roulette wheel ($f_{max} - f_i + \delta$), $\epsilon$-Lexicase with dynamic MAD threshold per candidate pool.
- `src/gp/operators.py`: Subtree crossover with depth $\le 7$ rejection (keeping parent), Gaussian constant mutation, subtree grow mutation, direct cloning.
- `src/gp/metrics.py`: Per-generation metric tracker: best/worst/mean train RMSE, best test RMSE, canonical tree duplicates, crossover success/failure, bloat metrics.
- `src/gp/engine.py`: Generational evolution engine coordinating population lifecycle, elitism, reproductive scheme, and history logging.
- `src/reporting/plotter.py`: Zero-dependency vector SVG and PNG plotting engine for convergence curves, bloat curves, and operator success charts.
- `src/reporting/tables.py`: Summary statistics calculator (mean $\pm$ std) exporting to Markdown and LaTeX booktabs.
- `scripts/check_style.py`: AST/tokenize linter enforcing R2: strictly casual lowercase comments and docstrings without trailing periods.
- `scripts/run_experiments.py`: Master multi-core runner executing Steps 1 to 5 across 30 seeds using `joblib.Parallel`.
- `relatorio_tp1.tex`: LaTeX template skeleton with placeholder guidance notes and figure/table insertion points.

## Feature Inventory
| # | Feature | Description | Milestone | Source |
|---|---------|-------------|-----------|--------|
| 1 | Dataset Ingestion | Load `synth1`, `synth2`, `concrete` CSVs without headers into contiguous NumPy float64 arrays | M1 | survey |
| 2 | Code Style Linter | Enforce casual lowercase comments/docstrings without trailing periods (R2) | M1 | survey |
| 3 | Syntax Tree Node | Node class with children, value, depth, node_count, copy, canonical to_string | M2 | survey |
| 4 | Protected Primitives | Vectorized +, -, *, safe /, sin, cos, safe log, safe exp clamped to [-32, 32] | M2 | survey |
| 5 | Vectorized Tree Eval | Post-order evaluation over $N \times D$ NumPy arrays returning $N$ predictions | M2 | survey |
| 6 | Tree Initialization | Ramped Half-and-Half (depths 2 to 5, 50% Full, 50% Grow) | M2 | survey |
| 7 | RMSE Fitness Function | Root Mean Squared Error computation | M2 | survey |
| 8 | Tournament Selection | Tournament selection with size $k = 2$ | M3 | survey |
| 9 | Scaled Roulette Selection | Roulette wheel selection with linear shift $f_{max} - f_i + \delta$ | M3 | survey |
| 10 | $\epsilon$-Lexicase Selection | Lexicase selection with dynamic MAD threshold (Eqs 1 & 2) over precomputed errors | M3 | survey |
| 11 | Variation Operators | Subtree crossover, Gaussian ERC mutation, subtree mutation | M3 | survey |
| 12 | Depth Ceiling $\le 7$ | Revert to unmodified parent if crossover or mutation exceeds depth 7 | M3 | survey |
| 13 | Elitism Support | Configurable preservation of the best individual in slot 0 | M3 | survey |
| 14 | Metric Logging | Log train best/worst/mean RMSE, test RMSE, duplicates, crossover success, bloat | M3 | survey |
| 15 | Evolution Engine | Generational GP loop coordinating selection, variation, elitism, and stopping | M3 | survey |
| 16 | Zero-Dep Plotting Engine | Vector SVG and PNG export for convergence, bloat, and operator success curves | M4 | survey |
| 17 | Summary Table Generator | Compute mean $\pm$ std across 30 seeds, export Markdown and LaTeX booktabs | M4 | survey |
| 18 | LaTeX Report Template | `relatorio_tp1.tex` skeleton with guidance notes and figure/table placements | M4 | survey |
| 19 | Step 1 Experiment Runner | Population size & generation tuning on `synth1` across 30 seeds | M5 | survey |
| 20 | Step 2 Experiment Runner | Selection operator comparison on `synth1` across 30 seeds | M5 | survey |
| 21 | Step 3 Experiment Runner | Variation operator comparison on `synth1` across 30 seeds | M5 | survey |
| 22 | Step 4 Experiment Runner | Elitism analysis on `synth1` across 30 seeds | M5 | survey |
| 23 | Step 5 Experiment Runner | Final validation on `synth2` and `concrete` across 30 seeds | M5 | survey |
| 24 | End-to-End Test Suite | Comprehensive opaque-box test suite verifying all features (Tiers 1-4) | E2E | survey |
| 25 | Adversarial Hardening | White-box stress testing and test coverage verification (Tier 5) | M6 | survey |

## Milestones
| # | Name | Scope | Dependencies | Status |
|---|------|-------|-------------|--------|
| M1 | Data Ingestion & Style Linter | `src/data.py`, `scripts/check_style.py`, style smoke tests | none | DONE |
| M2 | Core GP Primitives & Tree Evaluation | `src/gp/node.py`, `src/gp/primitives.py`, `src/gp/tree.py` | M1 | DONE |
| M3 | Selection, Variation & Evolution Engine | `src/gp/selection.py`, `src/gp/operators.py`, `src/gp/metrics.py`, `src/gp/engine.py` | M2 | DONE |
| M4 | Visualization, Tables & LaTeX Scaffold | `src/reporting/plotter.py`, `src/reporting/tables.py`, `relatorio_tp1.tex` | M1 | DONE |
| M5 | Experimental Automation & 30 Seeds | `scripts/run_experiments.py`, running Steps 1-5, populating `results/` | M3, M4 | DONE |
| M6 | Final E2E Test Pass & Adversarial Hardening | Pass 100% of E2E tests + Tier 5 coverage audit | M5, TEST_READY | DONE |
| E2E | E2E Testing Track | Independent opaque-box test suite (Tiers 1-4), harness, `TEST_READY.md` | none | DONE |

## Interface Contracts

### Data Ingestion Contract (`src/data.py`)
```python
def load_dataset(dataset_name: str, base_dir: str = ".") -> tuple[np.ndarray, np.ndarray, np.ndarray, np.ndarray]:
    """loads dataset returning x_train, y_train, x_test, y_test as contiguous float64 arrays"""
    # dataset_name in ('synth1', 'synth2', 'concrete')
    # returns:
    #   X_train: np.ndarray shape (N_train, D), dtype float64
    #   y_train: np.ndarray shape (N_train,), dtype float64
    #   X_test:  np.ndarray shape (N_test, D), dtype float64
    #   y_test:  np.ndarray shape (N_test,), dtype float64
```

### Tree Node Contract (`src/gp/node.py`)
```python
class Node:
    op: str # '+', '-', '*', '/', 'sin', 'cos', 'log', 'exp', 'var', 'const'
    children: list['Node']
    value: float | int # float for const, int (column index) for var
    def depth(self) -> int: ...
    def node_count(self) -> int: ...
    def copy(self) -> 'Node': ...
    def to_string(self) -> str: ...
    def eval(self, X: np.ndarray) -> np.ndarray: ...
```

### Evolution Engine Contract (`src/gp/engine.py`)
```python
class GPEngine:
    def __init__(self, pop_size=100, generations=100, selection='tournament',
                 pc=0.9, pm=0.05, elitism=True, max_depth=7, seed=42): ...
    def fit(self, X_train: np.ndarray, y_train: np.ndarray,
            X_test: np.ndarray = None, y_test: np.ndarray = None) -> dict:
        # returns history dict with per-generation metrics and best individual
        ...
```

### Reporting Contract (`src/reporting/`)
```python
def plot_convergence(csv_path: str, output_path: str) -> None: ...
def plot_bloat(csv_path: str, output_path: str) -> None: ...
def plot_operator_success(csv_path: str, output_path: str) -> None: ...
def generate_summary_tables(results_dir: str, output_dir: str) -> None: ...
```

## Code Layout
```
d:\Downloads\bases-TP1\datasets\
├── src/
│   ├── __init__.py
│   ├── data.py
│   ├── gp/
│   │   ├── __init__.py
│   │   ├── node.py
│   │   ├── primitives.py
│   │   ├── tree.py
│   │   ├── selection.py
│   │   ├── operators.py
│   │   ├── metrics.py
│   │   └── engine.py
│   └── reporting/
│       ├── __init__.py
│       ├── plotter.py
│       └── tables.py
├── scripts/
│   ├── check_style.py
│   └── run_experiments.py
├── tests/
│   ├── test_style.py
│   ├── test_data.py
│   ├── test_gp_core.py
│   ├── test_gp_evolution.py
│   ├── test_reporting.py
│   └── e2e/
│       ├── test_tier1_features.py
│       ├── test_tier2_boundaries.py
│       ├── test_tier3_combinations.py
│       └── test_tier4_scenarios.py
├── results/
│   ├── step1_pop_gen/
│   ├── step2_selection/
│   ├── step3_variation/
│   ├── step4_elitism/
│   ├── step5_validation/
│   ├── plots/
│   └── tables/
├── relatorio_tp1.tex
└── PROJECT.md
```
