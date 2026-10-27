# Original User Request

## 2026-10-26T17:47:04Z

Implement a modular, clean, and human-like Genetic Programming (GP) framework in Python and NumPy for symbolic regression, execute the complete experimental protocol on synthetic and real datasets with 30 repetitions, produce all plots and tables, and scaffold a LaTeX report template.

Working directory: d:\Downloads\bases-TP1\datasets
Integrity mode: development

Note on Python environment: Use the existing virtual environment at d:\Downloads\bases-TP1\datasets\.venv (which already has numpy, pandas, scipy, scikit-learn installed).

## Requirements

### R1. Core Genetic Programming Framework
Implement a modular, lightweight GP framework from scratch in Python and NumPy:
- Individual representation: Tree-based syntax trees with max depth 7. Initial population generated using Ramped Half-and-Half (depths 2 to 5). Any genetic operation exceeding depth 7 must keep the parent.
- Primitive set: Arithmetic (`+`, `-`, `*`, protected division) and non-linear functions (`sin`, `cos`, protected `log`, protected `exp`).
- Terminal set: Continuous Ephemeral Random Constants (ERCs) with Gaussian perturbation mutation, and dataset feature variables $X_0, \dots, X_{D-1}$.
- Vectorized tree evaluation over NumPy 2D feature arrays ($N \times D$) returning 1D prediction arrays.
- Fitness function: Root Mean Squared Error (RMSE).
- Selection methods:
  1. Tournament selection with tournament size $k = 2$.
  2. Scaled Roulette wheel selection (using linear shift $f_{max} - f_i + \delta$ to handle minimization without scale distortion).
  3. $\epsilon$-Lexicase selection with dynamic Median Absolute Deviation (MAD) threshold per candidate set, following Equations 1 and 2 of the project specification and vectorized across pre-computed prediction matrices.
- Reproductive scheme: Mutually exclusive operator selection per offspring slot ($p_c$ probability of subtree crossover, $p_m$ probability of mutation — subtree or constant perturbation —, remainder direct reproduction/cloning).
- Elitism support: Configurable (with elitism keeping the best individual vs without elitism).
- Statistics collection per generation: Best, worst, and mean RMSE; test RMSE of best individual; count of syntactically repeated individuals (canonical tree string representation); count of crossover offspring better and worse than average parents' fitness; bloat measurement (average node count and average depth).

### R2. Code Style and Engineering Constraints
- The code must look completely handwritten by a competent human developer, strictly avoiding LLM boilerplate, overly verbose docstrings, or academic prose.
- Comments must only exist where the code logic is not obvious, must be concise, casual, strictly all-lowercase, and never end with a period. Function docstrings must follow the same lowercase, casual, unpunctuated rule.
- No unit test or smoke test may take longer than 15 minutes. The entire project execution must finish well within 7 hours.
- Automated tests (smoke tests verifying tree evaluation, crossover depth capping, lexicase selection, metric logging) must run and pass before launching full experimental runs.

### R3. Experimental Automation and Data Generation
- Complete experimental pipeline executed across 30 distinct random seeds:
  1. Step 1: Population size and generation tuning on `synth1` (pop/gen combinations from $\{50, 100, 500\}$ with $p_c=0.9, p_m=0.05$, elitism, tournament $k=2$).
  2. Step 2: Selection method comparison on `synth1` (Tournament $k=2$ vs Scaled Roulette vs $\epsilon$-Lexicase).
  3. Step 3: Variation operator comparison on `synth1` ($p_c=0.9, p_m=0.05$ vs $p_c=0.6, p_m=0.30$).
  4. Step 4: Elitism analysis on `synth1` (best configuration with elitism vs without elitism).
  5. Step 5: Final validation on `synth2` and `concrete` using the selected best parameter configuration.
- All per-generation metrics and final test metrics logged to CSV files in a `results/` folder.
- Generation of publication-ready figures (convergence curves, bloat/node evolution, crossover operator success rates) and summary Markdown/LaTeX tables reporting mean $\pm$ standard deviation for train/test RMSE.

### R4. Report Template (LaTeX)
- Generate a concise LaTeX template file (`relatorio_tp1.tex`) containing the skeleton structure required by the PDF:
  1. Introdução
  2. Implementação (representação, fitness, operadores)
  3. Experimentos (análise do impacto dos parâmetros)
  4. Conclusões
  5. Bibliografia
- The content inside each section should use vague, placeholder guidance notes describing what the student should write, and specifying exactly which generated figures and tables should be inserted in each section.

## Acceptance Criteria

### Correctness & Robustness
- [ ] All smoke tests execute in under 2 minutes and verify: tree math evaluation without NaN/Inf, depth limit enforcement ($\le 7$), MAD calculation for $\epsilon$-lexicase, and duplicate canonical counting.
- [ ] Experimental scripts successfully run all 30 seeds for the required configurations without unhandled exceptions or crashes.

### Performance & Runtime
- [ ] No single test run exceeds 15 minutes.
- [ ] Complete experimental suite executes and finishes well under the 7-hour limit.

### Code Style Compliance
- [ ] Zero uppercase comments or trailing periods in comments/docstrings; all comments are casual and lowercase.
- [ ] Code is modular, clean, readable, and free of typical LLM clichés or excessive verbose scaffolding.

### Artifacts & Deliverables
- [ ] CSV logs populated under `results/` for all experimental steps.
- [ ] Generated PNG/PDF charts for convergence, bloat, and operator comparison.
- [ ] Summary tables with mean $\pm$ std for train and test RMSE across the 30 runs.
- [ ] `relatorio_tp1.tex` generated with LaTeX template structure, figure placeholders, and guidance notes.
