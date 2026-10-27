# Task Assignment: Milestone 3 Worker (Selection, Variation & Evolution Engine)

## Working Directory
d:\Downloads\bases-TP1\datasets\.agents\teamwork\worker_m3

## Objective
Implement and verify Milestone 3 based on `PROJECT.md`, `TEST_INFRA.md`, and `explorer_survey_gp_spec\handoff.md`:
1. `src/gp/selection.py`:
   - `tournament_selection(population, fitnesses, k=2, rng=None) -> int`
   - `scaled_roulette_selection(population, fitnesses, delta=1e-4, rng=None) -> int`:
     Linear shift: $w_i = (f_{max} - f_i) + \delta$, $p_i = w_i / \sum w_j$. Categorical sampling.
   - `epsilon_lexicase_selection(population, prediction_matrix, y_true, rng=None) -> int`:
     Vectorized over $\mathbf{E} = |\mathbf{Y}_{pred} - \mathbf{y}|$. Dynamic MAD threshold: $\epsilon_t = \text{median}(|\mathbf{e} - \text{median}(\mathbf{e})|)$. Candidate filtering: $e_i \le e^* + \epsilon_t + 10^{-12}$.
2. `src/gp/operators.py`:
   - `subtree_crossover(parent1, parent2, max_depth=7, rng=None) -> tuple[Tree, bool]`:
     Replaces random subtree in parent1 with random subtree from parent2. If offspring depth > 7, keeps parent1 (returns `parent1.copy()`, flag `False`).
   - `subtree_mutation(parent, num_features, max_depth=7, rng=None) -> tuple[Tree, bool]`:
     Replaces random node with random grow tree (depth 1-3). If depth > 7, keeps parent (returns `parent.copy()`, flag `False`).
   - `gaussian_constant_mutation(parent, sigma=0.5, rng=None) -> Tree`:
     Selects random constant node in parent and perturbs by $\mathcal{N}(0, \sigma^2)$. If no constants, returns copy.
   - `mutate(parent, num_features, max_depth=7, rng=None) -> Tree`:
     If parent has constants and `rng.random() < 0.5`, applies gaussian constant mutation; else subtree mutation.
   - `reproduce(parent) -> Tree`:
     Direct cloning (`parent.copy()`).
3. `src/gp/metrics.py`:
   - Metric collection per generation logging: `gen`, `train_rmse_best`, `train_rmse_worst`, `train_rmse_mean`, `test_rmse_best`, `unique_canonical_trees`, `duplicate_canonical_trees`, `crossover_better_count`, `crossover_worse_count`, `crossover_equal_count`, `crossover_rejected_count`, `avg_node_count`, `avg_depth`, `best_node_count`, `best_depth`, `elapsed_seconds`.
4. `src/gp/engine.py`:
   - `GPEngine(pop_size=100, generations=100, selection='tournament', tournament_k=2, pc=0.9, pm=0.05, elitism=True, max_depth=7, seed=42)`
   - `fit(X_train, y_train, X_test=None, y_test=None) -> dict`:
     Full generational evolutionary loop with precomputed prediction error matrices for lexicase selection, mutually exclusive operator selection per slot ($p_c, p_m$, cloning), elitism, and complete history logging.
   - `predict(X) -> np.ndarray`: returns predictions of best individual.
5. `tests/test_gp_evolution.py`:
   - Comprehensive unit tests verifying:
     - Tournament selection $k=2$.
     - Scaled roulette selection with linear shift.
     - $\epsilon$-Lexicase selection with dynamic MAD per Equations 1 & 2.
     - Crossover depth ceiling enforcement ($\le 7$).
     - Gaussian constant mutation and subtree mutation.
     - Elitism preservation.
     - 10-generation run of GPEngine on `synth1` verifying monotonic best fitness (with elitism), convergence, and history dictionary schema.
6. Verification:
   - Run `pytest tests/test_gp_evolution.py` and run full E2E suite `pytest tests/e2e/`.
   - Run `scripts/check_style.py` on all files to verify 0 style violations.

## Exclusively Owned Files
- `src/gp/selection.py`
- `src/gp/operators.py`
- `src/gp/metrics.py`
- `src/gp/engine.py`
- `tests/test_gp_evolution.py`

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
Write your report to `d:\Downloads\bases-TP1\datasets\.agents\teamwork\worker_m3\handoff.md`.

## 2026-10-26T18:37:29Z
You are a Worker subagent implementing Milestone 3: Selection, Variation & Evolution Engine.
Your working directory is: d:\Downloads\bases-TP1\datasets\.agents\teamwork\worker_m3

