# Task Assignment: Milestone 4 Worker (Visualization, Tables & LaTeX Scaffold)

## Working Directory
d:\Downloads\bases-TP1\datasets\.agents\teamwork\worker_m4

## Objective
Implement and verify Milestone 4 based on `PROJECT.md`, `TEST_INFRA.md`, and `explorer_survey_experiments_report\handoff.md`:
1. `src/reporting/__init__.py`: Package initialization with casual lowercase docstring.
2. `src/reporting/plotter.py`:
   - Pure Python zero-dependency vector SVG generator (and PNG exporter via standard library or System.Drawing bridge):
     - `plot_convergence(csv_path: str, output_path: str, title: str = "convergence")`: plots best train RMSE, mean train RMSE, and best test RMSE over generations with shaded ribbon or separate curves, legend, axis labels, grid.
     - `plot_bloat(csv_path: str, output_path: str, title: str = "tree size and depth")`: plots average node count and average depth over generations.
     - `plot_operator_success(csv_path: str, output_path: str, title: str = "crossover operator success")`: plots better vs worse crossover counts/rates over generations.
     - `plot_selection_diversity(csv_paths: dict[str, str], output_path: str)`: plots unique canonical tree counts across selection methods.
     - `plot_parity(y_true: np.ndarray, y_pred: np.ndarray, output_path: str, title: str = "predictions vs true")`: scatter plot with ideal $y=x$ reference line.
   - Clean, readable SVG vector graphics.
3. `src/reporting/tables.py`:
   - `compute_summary_stats(df_or_csvs: list) -> pd.DataFrame`: computes mean $\pm$ standard deviation for train RMSE, test RMSE, avg nodes, avg depth across runs/seeds.
   - `export_markdown_table(df: pd.DataFrame, output_path: str) -> str`: formats clean GitHub-flavored Markdown table.
   - `export_latex_table(df: pd.DataFrame, output_path: str, caption: str = "", label: str = "") -> str`: formats publication-quality LaTeX table using `booktabs` (`\toprule`, `\midrule`, `\bottomrule`).
4. `relatorio_tp1.tex`:
   - Full LaTeX template scaffold at `d:\Downloads\bases-TP1\datasets\relatorio_tp1.tex` matching R4:
     - Document class `article`, Portuguese babel, utf8, standard math packages, graphicx, booktabs, geometry.
     - Sections:
       1. Introdução
       2. Implementação (representação, fitness, operadores)
       3. Experimentos (análise do impacto dos parâmetros)
          - Subsections for Step 1, Step 2, Step 3, Step 4, Step 5 with exact figure and table inclusion placeholders (e.g. `\includegraphics{results/plots/...}` and `\input{results/tables/...}`).
       4. Conclusões
       5. Bibliografia
     - Portuguese placeholder guidance notes for the student describing what to write and which artifacts to insert.
5. `tests/test_reporting.py`:
   - Unit tests verifying:
     - SVG plotting functions generate valid, well-formed XML SVG files with correct tags (`<svg>`, `<polyline>`, `<text>`, etc.).
     - Summary table generation computes correct mean and std on sample data.
     - Markdown and LaTeX table exporters produce valid syntax and contain booktabs commands.
     - `relatorio_tp1.tex` exists, is non-empty, contains all 5 required sections, and includes placeholders for all 5 steps' figures/tables.
6. Verification:
   - Run `pytest tests/test_reporting.py`.
   - Run `scripts/check_style.py` on all created files to verify R2 compliance.

## Exclusively Owned Files
- `src/reporting/__init__.py`
- `src/reporting/plotter.py`
- `src/reporting/tables.py`
- `relatorio_tp1.tex`
- `tests/test_reporting.py`

## Mandatory Warning
DO NOT CHEAT. All implementations must be genuine. DO NOT hardcode test results, create dummy/facade implementations, or circumvent the intended task. A teamwork_preview_auditor will independently verify your work. Integrity violations WILL be detected and your work WILL be rejected.

## Code Style Rule (R2)
All comments and docstrings MUST be strictly casual lowercase without trailing periods! Handwritten look.

## Inputs
- `d:\Downloads\bases-TP1\datasets\.agents\teamwork\ORIGINAL_REQUEST.md`
- `d:\Downloads\bases-TP1\datasets\PROJECT.md`
- `d:\Downloads\bases-TP1\datasets\.agents\teamwork\explorer_survey_experiments_report\handoff.md`

## Output
Write your handoff report to:
`d:\Downloads\bases-TP1\datasets\.agents\teamwork\worker_m4\handoff.md`


## 2026-10-26T18:18:09Z
You are a Worker subagent implementing Milestone 4: Visualization, Tables & LaTeX Scaffold.
Your working directory is: d:\Downloads\bases-TP1\datasets\.agents\teamwork\worker_m4


## 2026-10-26T18:26:23Z
**Context**: Milestone 4 Status Check
**Content**: Checking in on progress for Milestone 4 (Visualization, Tables & LaTeX Scaffold).
**Action**: Please report your current implementation step and update progress.md.
