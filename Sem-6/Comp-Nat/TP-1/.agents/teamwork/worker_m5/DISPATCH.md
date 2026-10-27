# Task Assignment: Milestone 5 Worker (Experimental Automation & 30-Seed Execution)

## Working Directory
d:\Downloads\bases-TP1\datasets\.agents\teamwork\worker_m5

## Objective
Implement and execute the complete experimental automation pipeline across 30 distinct random seeds based on `ORIGINAL_REQUEST.md`, `PROJECT.md`, and `explorer_survey_experiments_report\handoff.md`:
1. Implement `scripts/run_experiments.py`:
   - Fixed 30 seeds: `SEEDS = [42 + i for i in range(30)]`.
   - Uses `joblib.Parallel(n_jobs=7)` for multi-core parallel execution over available CPU cores.
   - Executes Steps 1 to 5 sequentially:
     - **Step 1**: Pop/gen combinations from $\{50, 100, 500\}$ on `synth1` ($p_c=0.9, p_m=0.05$, elitism=True, tournament $k=2$). (9 configs x 30 seeds = 270 runs). Identifies winner $(P^*, G^*)$ based on lowest mean test RMSE.
     - **Step 2**: Selection comparison on `synth1` (Tournament $k=2$ vs Scaled Roulette vs $\epsilon$-Lexicase) using $(P^*, G^*)$, $p_c=0.9, p_m=0.05$, elitism=True. (3 configs x 30 seeds = 90 runs). Identifies winner $S^*$.
     - **Step 3**: Variation comparison on `synth1` ($p_c=0.9, p_m=0.05$ vs $p_c=0.6, p_m=0.30$) using $(P^*, G^*), S^*$, elitism=True. (2 configs x 30 seeds = 60 runs). Identifies winner $V^*$.
     - **Step 4**: Elitism analysis on `synth1` (with elitism vs without elitism) using $(P^*, G^*), S^*, V^*$. (2 configs x 30 seeds = 60 runs). Identifies winner $E^*$.
     - **Step 5**: Final validation on `synth2` and `concrete` using best overall configuration $C^* = (P^*, G^*, S^*, V^*, E^*)$. (2 datasets x 30 seeds = 60 runs).
   - Total runs: 540 runs.
2. Logging to `results/`:
   - Create directories: `results/step1_pop_gen/`, `results/step2_selection/`, `results/step3_variation/`, `results/step4_elitism/`, `results/step5_validation/`, `results/plots/`, `results/tables/`.
   - Log per-generation metrics for every single run to CSV.
   - Log per-step aggregated `summary.csv` with mean and standard deviation.
3. Visualization:
   - Use `src/reporting/plotter.py` to generate publication-ready `.png` and `.svg` plots in `results/plots/`:
     - `fig1_step1_convergence.png`
     - `fig2_step1_bloat.png`
     - `fig3_step2_selection_convergence.png`
     - `fig4_step2_diversity.png`
     - `fig5_step3_operator_success.png`
     - `fig6_step4_elitism_comparison.png`
     - `fig7_step5_synth2_concrete_convergence.png`
     - `fig8_step5_generalization_scatter.png` (or parity plot)
4. Tables:
   - Use `src/reporting/tables.py` to generate summary tables in both Markdown (`.md`) and LaTeX `booktabs` (`.tex`) in `results/tables/`:
     - `step1_summary.md` and `step1_summary.tex`
     - `step2_summary.md` and `step2_summary.tex`
     - `step3_summary.md` and `step3_summary.tex`
     - `step4_summary.md` and `step4_summary.tex`
     - `step5_summary.md` and `step5_summary.tex`
5. Verification:
   - Verify that all CSV files are non-empty and have valid rows.
   - Verify that all 8 plots exist in `results/plots/`.
   - Verify that all 5 `.tex` and `.md` tables exist in `results/tables/`.
   - Run `scripts/check_style.py` to ensure 0 style violations across all files.
   - Ensure the entire execution finishes in under 15 minutes.

## Exclusively Owned Files
- `scripts/run_experiments.py`
- `results/*`

## Mandatory Warning
DO NOT CHEAT. All runs must be genuine GP executions with distinct seeds. DO NOT hardcode or fabricate results. A teamwork_preview_auditor will independently inspect the generated data and execution logs.

## Code Style Rule (R2)
All comments and docstrings MUST be strictly casual lowercase without trailing periods! Handwritten look.

## Inputs
- `d:\Downloads\bases-TP1\datasets\.agents\teamwork\ORIGINAL_REQUEST.md`
- `d:\Downloads\bases-TP1\datasets\PROJECT.md`
- `d:\Downloads\bases-TP1\datasets\.agents\teamwork\explorer_survey_experiments_report\handoff.md`

## Output
Write your handoff report to:
`d:\Downloads\bases-TP1\datasets\.agents\teamwork\worker_m5\handoff.md`
Report the winners of Steps 1-4, runtime of each step, and paths to all generated tables and figures.


## 2026-10-26T19:37:59Z
[Message] timestamp=2026-10-26T19:37:59Z sender=0fbe2801-fce8-49f2-8814-84768519d3f0 priority=MESSAGE_PRIORITY_HIGH
You are the Milestone 5 Worker (Experimental Automation & 30-Seed Execution).
Implement scripts/run_experiments.py using Python/NumPy/Joblib (`.venv\Scripts\python.exe`):
- Fixed 30 seeds: `SEEDS = [42 + i for i in range(30)]`.
- Uses `joblib.Parallel(n_jobs=7)` to run experiments concurrently across CPU cores.
- Executes Steps 1 to 5 sequentially:
  - Step 1: Pop/gen grid search on synth1 with pop_size in {50, 100, 500} and generations in {50, 100, 500}, pc=0.9, pm=0.05, elitism=True, selection='tournament' (k=2). 9 configurations x 30 seeds = 270 runs. Identifies winning pair (P*, G*) based on lowest mean test RMSE.
  - Step 2: Selection comparison on synth1 comparing 'tournament' (k=2), 'roulette' (scaled), and 'lexicase' (epsilon-lexicase) using (P*, G*), pc=0.9, pm=0.05, elitism=True. 3 configurations x 30 seeds = 90 runs. Identifies winning selection S*.
  - Step 3: Variation operator comparison on synth1 comparing (pc=0.9, pm=0.05) vs (pc=0.6, pm=0.30) using (P*, G*), S*, elitism=True. 2 configurations x 30 seeds = 60 runs. Identifies winning variation V*.
  - Step 4: Elitism analysis on synth1 comparing elitism=True vs elitism=False using (P*, G*), S*, V*. 2 configurations x 30 seeds = 60 runs. Identifies winning elitism E*.
  - Step 5: Final validation on synth2 and concrete using best overall configuration C* = (P*, G*, S*, V*, E*). 2 datasets x 30 seeds = 60 runs.
- Total runs: 540 runs.
Ensure full logging into results/:
- directories: results/step1_pop_gen/, results/step2_selection/, results/step3_variation/, results/step4_elitism/, results/step5_validation/, results/plots/, results/tables/.
- log per-generation metrics for every single run to CSV.
- log per-step aggregated summary.csv with mean and std.
Visualization:
- Use src/reporting/plotter.py to generate plots in results/plots/ (fig1-fig8).
Tables:
- Use src/reporting/tables.py to generate summary tables in results/tables/ (step1-5 .md and .tex).
Verification:
- Run scripts/run_experiments.py, check all CSVs, plots, tables.
- Run scripts/check_style.py (0 style violations).
- Run pytest (no regressions).
Deliver handoff to worker_m5/handoff.md.
