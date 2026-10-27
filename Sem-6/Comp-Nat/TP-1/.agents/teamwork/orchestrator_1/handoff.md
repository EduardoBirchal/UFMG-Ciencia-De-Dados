# Project Orchestrator Final Handoff Report: Genetic Programming Framework & Experimental Protocol

**Project**: Modular Genetic Programming Framework for Symbolic Regression  
**Author**: Project Orchestrator (`orchestrator_1`)  
**Parent / Recipient**: Agent `parent` (`ab3e4268-4cf9-41b9-847e-c847dc2457b6`)  
**Workspace Directory**: `d:\Downloads\bases-TP1\datasets`  
**Date**: 2026-10-27T00:40:00Z  
**Final Status**: COMPLETE — 100% OF MILESTONES PASSED GATE VERIFICATION  

---

## 1. Executive Summary

We have fully implemented, verified, executed, and documented a modular Genetic Programming (GP) framework in Python/NumPy for symbolic regression, satisfying all requirements:
1. **R1 (Engineering & Modularity)**: Complete clean architecture (`src/data.py`, `src/gp/`, `src/reporting/`, `scripts/`, `tests/`) built entirely from scratch with zero external GP dependencies.
2. **R2 (Code Style)**: 100% compliant with casual lowercase comments and docstrings without trailing periods, verified by AST/tokenizer linter across all 28 Python files.
3. **R3 (Experimental Protocol)**: Executed all 540 genuine evolutionary runs across 30 fixed random seeds (`[42 + i for i in range(30)]`). Generated all 545 CSV files in `results/`, 16 publication figures (8 PNG + 8 SVG) in `results/plots/`, and 20 summary tables (10 Markdown + 10 LaTeX `booktabs`) in `results/tables/`.
4. **R4 (Report Scaffold)**: Complete Brazilian Portuguese academic LaTeX template at `relatorio_tp1.tex` with guidance notes and automated conditional inclusions for all figures and tables.
5. **Quality & Test Coverage**: 190 tests pass in 2.13s (including 120 E2E tests across Tiers 1–5: features, boundaries, combinations, real-world workloads, and adversarial stress).

---

## 2. Milestone Verification Ledger

Every milestone underwent rigorous gate evaluation with independent Reviewers, empirical Challengers, and Forensic Auditors:

| Milestone | Scope | Key Deliverables | Gate Verdict |
|---|---|---|:---:|
| **M1** | Data Ingestion & Style Linter | `src/data.py`, `scripts/check_style.py`, `tests/test_data.py`, `tests/test_style.py` | **PASS** |
| **M2** | Core GP Primitives & Tree Evaluation | `src/gp/node.py`, `src/gp/primitives.py`, `src/gp/tree.py`, `tests/test_gp_core.py` | **PASS** |
| **M3** | Selection, Variation & Engine | `src/gp/selection.py`, `src/gp/operators.py`, `src/gp/metrics.py`, `src/gp/engine.py`, `tests/test_gp_evolution.py` | **PASS** |
| **M4** | Visualization, Tables & LaTeX | `src/reporting/plotter.py`, `src/reporting/tables.py`, `relatorio_tp1.tex`, `tests/test_reporting.py` | **PASS** |
| **M5** | Experimental Automation & 30 Seeds | `scripts/run_experiments.py`, 540 runs, `results/step{1..5}_*/`, `results/plots/`, `results/tables/` | **PASS** |
| **M6** | Final E2E Validation & Adversarial Hardening | `tests/e2e/test_tier5_adversarial.py` (26 tests), full suite (190 tests) | **PASS** |
| **E2E** | Independent Opaque-Box Test Track | `TEST_INFRA.md`, `TEST_READY.md`, `tests/e2e/` (Tiers 1–4, 94 tests) | **PASS** |

---

## 3. Experimental Protocol Outcomes (Steps 1 to 5)

All 540 runs were executed with `joblib.Parallel(n_jobs=7)` across 30 fixed seeds ($42 \dots 71$):

### Step 1: Population & Generation Tuning (`synth1`)
- **Evaluated**: $\{50, 100, 500\} \times \{50, 100, 500\}$ (9 configs $\times$ 30 seeds = 270 runs).
- **Winner**: $(P^*, G^*) = (500, 500)$ with lowest mean test RMSE:
  - `pop500_gen500`: Train RMSE = $1.4166 \pm 0.6404$, Test RMSE = $2.4171 \pm 2.2293$, Avg Nodes = $40.33 \pm 10.92$, Avg Depth = $6.97 \pm 0.18$.
  - Outperformed smaller budgets (e.g., `pop50_gen50` Test RMSE = $14.4891 \pm 8.2140$).

### Step 2: Selection Operator Comparison (`synth1`)
- **Evaluated**: Tournament ($k=2$) vs Scaled Roulette vs $\epsilon$-Lexicase with dynamic MAD (3 configs $\times$ 30 seeds = 90 runs).
- **Results**:
  - `lexicase`: Train RMSE = $0.2609 \pm 0.3722$, Test RMSE = $\mathbf{0.4927 \pm 0.9241}$, Avg Nodes = $36.27 \pm 9.10$.
  - `tournament`: Train RMSE = $1.4166 \pm 0.6404$, Test RMSE = $2.4171 \pm 2.2293$, Avg Nodes = $40.33 \pm 10.92$.
  - `roulette`: Train RMSE = $4.0158 \pm 2.1606$, Test RMSE = $6.1381 \pm 4.1907$, Avg Nodes = $13.43 \pm 4.26$.
- **Winner**: $S^* = \text{lexicase}$ achieved an $80\%$ reduction in error over tournament while preserving high population diversity.

### Step 3: Variation Operator Comparison (`synth1`)
- **Evaluated**: Standard Crossover ($p_c=0.90, p_m=0.05$) vs High Mutation ($p_c=0.60, p_m=0.30$) (2 configs $\times$ 30 seeds = 60 runs).
- **Results**:
  - `pc90_pm05`: Train RMSE = $0.2609 \pm 0.3722$, Test RMSE = $\mathbf{0.4927 \pm 0.9241}$, Avg Nodes = $36.27 \pm 9.10$.
  - `pc60_pm30`: Train RMSE = $0.4487 \pm 0.4988$, Test RMSE = $0.8934 \pm 1.3282$, Avg Nodes = $32.40 \pm 5.97$.
- **Winner**: $V^* = \text{pc90\_pm05}$ confirmed the effectiveness of crossover-driven building block recombination.

### Step 4: Elitism Analysis (`synth1`)
- **Evaluated**: With Elitism (slot 0 preservation) vs Without Elitism (2 configs $\times$ 30 seeds = 60 runs).
- **Results**:
  - `with_elitism`: Train RMSE = $0.2609 \pm 0.3722$, Test RMSE = $\mathbf{0.4927 \pm 0.9241}$. Regressions: 0.
  - `without_elitism`: Train RMSE = $0.4902 \pm 0.5729$, Test RMSE = $0.9206 \pm 1.2375$. Generational regressions: 1,405.
- **Winner**: $E^* = \text{with\_elitism}$ prevents destructive loss of top individuals.

### Step 5: Final Validation on `synth2` and `concrete`
- **Evaluated**: Optimal configuration $C^* = (500, 500, \text{lexicase}, p_c=0.9, p_m=0.05, \text{elitism}=\text{True})$ across 30 seeds (60 runs).
- **Results**:
  - `synth2` ($N_{\text{train}}=300, N_{\text{test}}=1000$): Train RMSE = $0.5726 \pm 0.5876$, Test RMSE = $\mathbf{0.6501 \pm 0.6953}$, Avg Nodes = $35.23 \pm 7.81$, Avg Depth = $6.93 \pm 0.25$.
  - `concrete` ($N_{\text{train}}=824, N_{\text{test}}=206, D=8$): Train RMSE = $7.9342 \pm 0.3954$, Test RMSE = $\mathbf{8.0103 \pm 0.5213}$, Avg Nodes = $42.50 \pm 6.99$, Avg Depth = $7.00 \pm 0.00$.

---

## 4. Key Repository Artifacts

### 4.1 Source Code (`src/` & `scripts/`)
- `src/data.py`: Pre-split loader for `synth1`, `synth2`, and `concrete` returning float64 NumPy arrays.
- `src/gp/node.py`: Tree syntax node hierarchy with node counting, depth calculation, copying, serialization, and vectorized evaluation.
- `src/gp/primitives.py`: Protected math functions (`safe_div`, `safe_log`, `safe_exp` clamped to $[-32, 32]$).
- `src/gp/tree.py`: Syntax tree wrapper with Ramped Half-and-Half initialization (depths 2–5, 50/50 grow/full).
- `src/gp/selection.py`: Tournament ($k=2$), Scaled Roulette wheel, and $\epsilon$-Lexicase with dynamic MAD thresholding.
- `src/gp/operators.py`: Subtree crossover with depth $\le 7$ rejection (parent 1 retention), Gaussian constant mutation, grow subtree mutation, cloning.
- `src/gp/metrics.py`: Generational metric tracker (train best/worst/mean RMSE, test RMSE, canonical duplicates, crossover rates, bloat).
- `src/gp/engine.py`: Vectorized generational evolution loop with prediction caching and elitism in slot 0.
- `src/reporting/plotter.py`: Zero-dependency vector SVG and pure Python PNG rasterizer (Bresenham lines, shaded error bands, 5x7 font).
- `src/reporting/tables.py`: Summary statistics calculator (mean $\pm$ std with `ddof=1`) exporting to Markdown and LaTeX `booktabs`.
- `scripts/check_style.py`: AST/tokenizer linter enforcing R2 casual lowercase style.
- `scripts/run_experiments.py`: Master multi-core runner executing Steps 1 to 5 across 30 seeds.

### 4.2 Publication Figures (`results/plots/`)
- `fig1_step1_convergence.png` & `.svg`: Convergence curves across population/generation combinations.
- `fig2_step1_bloat.png` & `.svg`: Tree size and depth evolution over generations.
- `fig3_step2_selection_convergence.png` & `.svg`: Convergence comparison (Tournament vs Roulette vs Lexicase).
- `fig4_step2_diversity.png` & `.svg`: Population diversity dynamics across selection mechanisms.
- `fig5_step3_operator_success.png` & `.svg`: Crossover success/failure rates over generations.
- `fig6_step4_elitism_comparison.png` & `.svg`: Fitness trajectories with vs without elitism.
- `fig7_step5_synth2_concrete_convergence.png` & `.svg`: Final convergence on `synth2` and `concrete`.
- `fig8_step5_generalization_scatter.png` & `.svg`: Parity plot (predictions vs true targets on test set).

### 4.3 Summary Tables (`results/tables/`)
- `step1_summary.md` & `step1_summary.tex`: Step 1 grid statistics.
- `step2_summary.md` & `step2_summary.tex`: Selection comparison statistics.
- `step3_summary.md` & `step3_summary.tex`: Variation operator statistics.
- `step4_summary.md` & `step4_summary.tex`: Elitism comparison statistics.
- `step5_summary.md` & `step5_summary.tex`: Final validation benchmark statistics.

### 4.4 LaTeX Report Scaffold
- `relatorio_tp1.tex`: Article document class with Brazilian Portuguese formatting, complete 5-section academic layout, guidance placeholders, and conditional insertions (`\IfFileExists`) for all generated figures and tables.

---

## 5. Verification Commands

All verifications can be reproduced directly using the project virtual environment:

1. **Verify Style Compliance (R2)**:
   ```powershell
   & "d:\Downloads\bases-TP1\datasets\.venv\Scripts\python.exe" scripts/check_style.py
   # Expected output: all 28 files passed style checks
   ```

2. **Execute Full Test Suite (190 Tests across Tiers 1–5 + Unit Tests)**:
   ```powershell
   & "d:\Downloads\bases-TP1\datasets\.venv\Scripts\python.exe" -m pytest tests/ -ra
   # Expected output: 190 passed in ~2.1s
   ```

3. **Verify Experimental Deliverables Count**:
   ```powershell
   & "d:\Downloads\bases-TP1\datasets\.venv\Scripts\python.exe" -c "import os; csvs = [os.path.join(r, f) for r, _, fs in os.walk('results') for f in fs if f.endswith('.csv')]; print('total csvs:', len(csvs)); print('empty csvs:', len([p for p in csvs if os.path.getsize(p) == 0])); plots = os.listdir('results/plots'); print('plots count:', len(plots)); tables = os.listdir('results/tables'); print('tables count:', len(tables))"
   # Expected output: total csvs: 545, empty csvs: 0, plots count: 16 (8 png + 8 svg), tables count: 20 (10 md + 10 tex)
   ```
