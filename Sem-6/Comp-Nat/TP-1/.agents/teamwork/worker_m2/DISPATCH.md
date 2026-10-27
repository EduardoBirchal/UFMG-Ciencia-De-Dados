# Task Assignment: Milestone 2 Worker (Core GP Primitives & Tree Evaluation)

## Working Directory
d:\Downloads\bases-TP1\datasets\.agents\teamwork\worker_m2

## Objective
Implement and verify Milestone 2 based on `PROJECT.md`, `TEST_INFRA.md`, and `explorer_survey_gp_spec\handoff.md`:
1. `src/gp/__init__.py`: Package initialization with casual lowercase docstring.
2. `src/gp/node.py`:
   - `Node(op, children=None, value=None)`
   - Operations: binary `+`, `-`, `*`, `/`; unary `sin`, `cos`, `log`, `exp`; terminals `'var'` (column index $j \in \{0 \dots D-1\}$), `'const'` (float value).
   - `depth() -> int`: leaf is 1; root with terminals is 2; recursive depth.
   - `node_count() -> int`: leaf is 1; internal is $1 + \sum \text{node\_count}(c)$.
   - `copy() -> Node`: deep clone of node and children.
   - `to_string() -> str`: canonical string serialization for duplicate detection (`X0`, `1.2345`, `(+ X0 X1)`, etc.).
   - `get_all_nodes() -> list[Node]`: returns all nodes in prefix order.
   - `get_constants() -> list[Node]`: returns all constant nodes.
   - `replace_subtree(root: Node, target_idx: int, new_subtree: Node) -> Node`: functional, immutable subtree replacement.
3. `src/gp/primitives.py`:
   - Vectorized functions operating on NumPy arrays/scalars:
     - `safe_add(x, y)`: `np.clip(x + y, -1e50, 1e50)`
     - `safe_sub(x, y)`: `np.clip(x - y, -1e50, 1e50)`
     - `safe_mul(x, y)`: `np.clip(x * y, -1e50, 1e50)`
     - `safe_div(x, y)`: if `abs(y) < 1e-6` return 1.0; else `np.clip(x / y, -1e50, 1e50)`.
     - `safe_sin(x)`: `np.sin(x)`
     - `safe_cos(x)`: `np.cos(x)`
     - `safe_log(x)`: if `abs(x) < 1e-6` return 0.0; else `np.log(abs(x))`
     - `safe_exp(x)`: `np.exp(np.clip(x, -32.0, 32.0))`
   - Robust against NaNs/Infs.
4. `src/gp/tree.py`:
   - `Tree(root: Node = None)`
   - `eval(X: np.ndarray) -> np.ndarray`: vectorized post-order evaluation over 2D array $N \times D$, returning 1D array of shape $(N,)$ float64. Broadcasts constant scalars to length $N$. Clamps output to $[-1e10, 1e10]$, converts any accidental NaNs to $1e9$.
   - `compute_rmse(y_true: np.ndarray, y_pred: np.ndarray) -> float`: Root Mean Squared Error $\sqrt{\frac{1}{N} \sum (y_i - \hat{y}_i)^2}$.
   - Ramped Half-and-Half generator:
     - Depths 2 to 5.
     - 50% Full method (all branches grown to exact target depth).
     - 50% Grow method (branches can terminate before max depth).
     - Terminals: 50% variable $X_j$ ($j \in \{0 \dots D-1\}$), 50% continuous ERC $c \sim \text{Uniform}(-5.0, 5.0)$.
     - `generate_population(pop_size: int, n_features: int, min_depth: int = 2, max_depth: int = 5, rng: np.random.Generator = None) -> list[Tree]`.
5. `tests/test_gp_core.py`:
   - Comprehensive unit tests:
     - Node methods: depth, node_count, copy, to_string, subtree indexing and functional replacement.
     - Primitive numerical protections: safe div (zero/near-zero denominators), safe log (zero/negatives), safe exp (large positive/negative values clamped to [-32, 32]), trig functions.
     - Tree vectorized evaluation: scalar constants broadcasted, feature indexing verified, shape $(N,)$ returned.
     - Ramped half-and-half: verifies population generation, depth bounds (2 to 5), mix of full and grow.
     - Stability stress test: 1,000 random trees evaluated over random data, asserting 0 NaNs and 0 Infs.
     - Ground truth evaluation on `synth1`: manual construction of $X_0^4 - X_0^3 + 0.5 X_1^2 - X_1$ evaluated on loaded dataset, asserting RMSE $< 1e-12$.
6. Verification:
   - Run `pytest tests/test_gp_core.py` and `pytest tests/e2e/test_tier1_features.py`.
   - Run `scripts/check_style.py` on all files.

## Exclusively Owned Files
- `src/gp/__init__.py`
- `src/gp/node.py`
- `src/gp/primitives.py`
- `src/gp/tree.py`
- `tests/test_gp_core.py`

## Mandatory Warning
DO NOT CHEAT. All implementations must be genuine. DO NOT hardcode test results, create dummy/facade implementations, or circumvent the intended task. A teamwork_preview_auditor will independently verify your work. Integrity violations WILL be detected and your work WILL be rejected.

## Code Style Rule (R2)
All comments and docstrings MUST be strictly casual lowercase without trailing periods! Handwritten look.

## Inputs
- `d:\Downloads\bases-TP1\datasets\.agents\teamwork\ORIGINAL_REQUEST.md`
- `d:\Downloads\bases-TP1\datasets\PROJECT.md`
- `d:\Downloads\bases-TP1\datasets\TEST_INFRA.md`
- `d:\Downloads\bases-TP1\datasets\TEST_READY.md`
- `d:\Downloads\bases-TP1\datasets\.agents\teamwork\explorer_survey_gp_spec\handoff.md`

## Output
Write your handoff report to:
`d:\Downloads\bases-TP1\datasets\.agents\teamwork\worker_m2\handoff.md`


## 2026-10-26T18:18:02Z
You are a Worker subagent implementing Milestone 2: Core GP Primitives & Tree Evaluation.
Your working directory is: d:\Downloads\bases-TP1\datasets\.agents\teamwork\worker_m2

MANDATORY: Read ORIGINAL_REQUEST.md at:
d:\Downloads\bases-TP1\datasets\.agents\teamwork\ORIGINAL_REQUEST.md
and PROJECT.md at:
d:\Downloads\bases-TP1\datasets\PROJECT.md
before starting work.

Read your task instructions at:
d:\Downloads\bases-TP1\datasets\.agents\teamwork\worker_m2\DISPATCH.md

MANDATORY INTEGRITY WARNING:
DO NOT CHEAT. All implementations must be genuine. DO NOT hardcode test results, create dummy/facade implementations, or circumvent the intended task. A teamwork_preview_auditor will independently verify your work. Integrity violations WILL be detected and your work WILL be rejected.

CRITICAL CODE STYLE CONSTRAINT (R2):
All comments and docstrings MUST be strictly casual lowercase without trailing periods! Handwritten look.

Files you exclusively own:
- src/gp/__init__.py
- src/gp/node.py
- src/gp/primitives.py
- src/gp/tree.py
- tests/test_gp_core.py

Implement the modules, run pytest using d:\Downloads\bases-TP1\datasets\.venv\Scripts\python.exe, run scripts/check_style.py, verify 0 NaNs/Infs over 1,000 random trees, and ground truth RMSE < 1e-12 on synth1.

Maintain your progress in d:\Downloads\bases-TP1\datasets\.agents\teamwork\worker_m2\progress.md.
Write your final handoff report to:
d:\Downloads\bases-TP1\datasets\.agents\teamwork\worker_m2\handoff.md

When done, message the orchestrator (conversation ID 0fbe2801-fce8-49f2-8814-84768519d3f0).
