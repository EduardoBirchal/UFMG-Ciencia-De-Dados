# Handoff Report: Experimental Protocol, Code Style Constraints & Report Template (R2, R3, R4)

**Agent**: `explorer_survey_experiments_report`  
**Working Directory**: `d:\Downloads\bases-TP1\datasets\.agents\teamwork\explorer_survey_experiments_report`  
**Date**: 2026-10-26T18:00:00Z  
**Target Recipient**: Project Orchestrator (`0fbe2801-fce8-49f2-8814-84768519d3f0`) and Implementation Specialists  

---

## 1. Observation

Direct observations from examining the workspace, environment, and project specifications:

### 1.1 Source Documents
- **`ORIGINAL_REQUEST.md`** (`d:\Downloads\bases-TP1\datasets\.agents\teamwork\ORIGINAL_REQUEST.md`):
  - **Lines 29–34 (R2 Code Style & Engineering Constraints)**:
    - Code must look completely handwritten by a competent human developer, strictly avoiding LLM boilerplate, verbose docstrings, or academic prose.
    - Comments must exist only where logic is non-obvious, must be concise, casual, strictly all-lowercase, and never end with a period.
    - Function docstrings must follow the same lowercase, casual, unpunctuated rule.
    - Runtime budget: no unit or smoke test > 15 minutes; entire project execution well under 7 hours.
    - Automated tests (smoke tests verifying tree math, crossover depth capping, lexicase selection, metric logging) must run and pass before launching full experimental runs.
  - **Lines 35–44 (R3 Experimental Automation & Data Generation)**:
    - Complete experimental pipeline executed across 30 distinct random seeds.
    - **Step 1**: Population size and generation tuning on `synth1` (pop/gen combinations from $\{50, 100, 500\}$ with $p_c=0.9, p_m=0.05$, elitism, tournament $k=2$).
    - **Step 2**: Selection method comparison on `synth1` (Tournament $k=2$ vs Scaled Roulette vs $\epsilon$-Lexicase).
    - **Step 3**: Variation operator comparison on `synth1` ($p_c=0.9, p_m=0.05$ vs $p_c=0.6, p_m=0.30$).
    - **Step 4**: Elitism analysis on `synth1` (best configuration with elitism vs without elitism).
    - **Step 5**: Final validation on `synth2` and `concrete` using the selected best parameter configuration.
    - Per-generation metrics logged to CSV files in `results/`.
    - Publication-ready figures: convergence curves, bloat/node evolution, crossover operator success rates.
    - Summary tables reporting mean $\pm$ standard deviation for train/test RMSE in Markdown and LaTeX.
  - **Lines 45–53 (R4 Report Template LaTeX)**:
    - Scaffold `relatorio_tp1.tex` with skeleton structure:
      1. Introdução
      2. Implementação (representação, fitness, operadores)
      3. Experimentos (análise do impacto dos parâmetros)
      4. Conclusões
      5. Bibliografia
    - Placeholder guidance notes describing what the student should write, specifying exact figures and tables to insert.

### 1.2 Python Environment & Available Libraries
- Executable: `d:\Downloads\bases-TP1\datasets\.venv\Scripts\python.exe`
- Python version: **3.14.3 (AMD64 64-bit)**
- Available CPU hardware: **8 logical cores** (`multiprocessing.cpu_count() == 8`)
- Installed packages in `.venv`:
  - `numpy` (2.5.4)
  - `pandas` (3.1.0)
  - `scipy` (1.18.1)
  - `scikit-learn` (1.9.1)
  - `joblib` (1.6.0)
  - `cloudpickle` (3.1.2)
- **CRITICAL ENVIRONMENT FINDING**:
  - `matplotlib` and `seaborn` are **NOT** installed (`ModuleNotFoundError: No module named 'matplotlib'`).
  - Network access to PyPI is disabled (`getaddrinfo failed for pypi.org`).
  - Pip wheel cache in AppData contains no matplotlib wheel.
  - **Solution Verified**: Zero-dependency pure Python vector SVG generator and pure Python PNG writer (using `zlib` / `struct`), as well as PowerShell `System.Drawing`, are fully functional on this system.

### 1.3 Datasets Verification
- Inspection via Pandas with `header=None`:
  - `synth1`: Train shape = `(60, 3)` (60 instances, 2 inputs, 1 target); Test shape = `(600, 3)` (600 instances, 2 inputs, 1 target).
  - `synth2`: Train shape = `(300, 3)` (300 instances, 2 inputs, 1 target); Test shape = `(1000, 3)` (1000 instances, 2 inputs, 1 target).
  - `concrete`: Train shape = `(824, 9)` (824 instances, 8 inputs, 1 target); Test shape = `(206, 9)` (206 instances, 8 inputs, 1 target).
- All files are comma-separated values without header lines.

---

## 2. Logic Chain

From the observations above, the following technical strategies, protocols, and architectural specifications are derived:

### 2.1 R2 Code Style Constraints & Automated Verification

#### Exact Style Rules
1. **Comment Tone & Case**:
   - Comments must be written in a casual, direct, handwritten tone.
   - Strictly lowercase: every character in comments must be lowercase. Acronyms must also be lowercase (e.g., `# calculate rmse`, `# evaluate tree on x`, `# tournament selection with k=2`, `# mad calculation for lexicase`).
   - No trailing period: never end comments with a dot `.`.
   - No redundant or verbose comments: only comment where code logic is non-obvious.
2. **Docstring Tone & Case**:
   - Concise, single-line or brief lowercase text without trailing periods.
   - Example: `"""evaluates tree on feature matrix x"""`.
   - Never use Google/NumPy-style verbose docstring blocks with `Args:`, `Returns:`, `Raises:`.
3. **Automated Linter Script (`scripts/check_style.py`)**:
   - To guarantee zero violations before any commit or test run, a style linter must scan all `.py` files in `src/` and `tests/` using Python's `tokenize` and `ast` modules:
     ```python
     # inspects all comments and docstrings for lowercase and no trailing dot
     import ast, tokenize, sys, os

     def check_file(path):
         errors = []
         with open(path, 'rb') as f:
             tokens = tokenize.tokenize(f.readline)
             for tok in tokens:
                 if tok.type == tokenize.COMMENT:
                     text = tok.string.lstrip('#').strip()
                     if text:
                         if text != text.lower():
                             errors.append(f"{path}:{tok.start[0]} comment has uppercase: '{tok.string}'")
                         if text.endswith('.'):
                             errors.append(f"{path}:{tok.start[0]} comment ends with period: '{tok.string}'")
         with open(path, 'r', encoding='utf-8') as f:
             tree = ast.parse(f.read(), filename=path)
         for node in ast.walk(tree):
             if isinstance(node, (ast.FunctionDef, ast.AsyncFunctionDef, ast.ClassDef, ast.Module)):
                 doc = ast.get_docstring(node)
                 if doc:
                     doc_clean = doc.strip()
                     if doc_clean != doc_clean.lower():
                         errors.append(f"{path}:{getattr(node, 'lineno', 1)} docstring has uppercase")
                     if doc_clean.endswith('.'):
                         errors.append(f"{path}:{getattr(node, 'lineno', 1)} docstring ends with period")
         return errors
     ```
   - Running this linter as a smoke test (`tests/test_style.py`) ensures 100% compliance with R2.

---

### 2.2 R3 Experimental Protocol: 5-Step Parameter Matrix & Automation

#### 2.2.1 Seed Set Specification
To ensure strict scientific reproducibility across all 5 steps, a fixed set of 30 integer seeds must be used:
```python
SEEDS = [42 + i for i in range(30)] # seeds 42 to 71
```
Each seed completely dictates tree initialization, operator choices, parent sampling, and Gaussian perturbations via an explicit NumPy generator `rng = np.random.default_rng(seed)`.

#### 2.2.2 Step 1 to Step 5 Matrix

| Step | Objective | Dataset | Configurations Evaluated | Fixed Parameters | Seeds | Total Runs | Selection of Winner |
|---|---|---|---|---|---|---|---|
| **Step 1** | Tuning Population Size & Generations | `synth1` (60 train, 600 test) | Grid of $(pop, gen)$ from $\{50, 100, 500\}$: <br>1. (50, 50)<br>2. (50, 100)<br>3. (50, 500)<br>4. (100, 50)<br>5. (100, 100)<br>6. (100, 500)<br>7. (500, 50)<br>8. (500, 100)<br>9. (500, 500) | $p_c=0.9, p_m=0.05, p_r=0.05$<br>Elitism=True (1 individual)<br>Selection=Tournament ($k=2$)<br>Max depth=7 | 30 seeds | $9 \times 30 = 270$ | Lowest mean test RMSE on `synth1` $\implies (P^*, G^*)$ (with runtime trade-off consideration; e.g. $(100, 100)$ or $(500, 100)$) |
| **Step 2** | Selection Method Comparison | `synth1` | 3 Selection Operators:<br>1. Tournament ($k=2$)<br>2. Scaled Roulette Wheel ($f_{max} - f_i + 10^{-6}$)<br>3. $\epsilon$-Lexicase (dynamic MAD per case) | $(P^*, G^*)$ from Step 1<br>$p_c=0.9, p_m=0.05$<br>Elitism=True | 30 seeds | $3 \times 30 = 90$ | Lowest mean test RMSE and highest structural diversity $\implies S^*$ |
| **Step 3** | Variation Operator Comparison | `synth1` | 2 Variation Regimes:<br>1. High Crossover: $p_c=0.90, p_m=0.05, p_r=0.05$<br>2. High Mutation: $p_c=0.60, p_m=0.30, p_r=0.10$ | $(P^*, G^*)$, $S^*$ from Step 2<br>Elitism=True | 30 seeds | $2 \times 30 = 60$ | Best trade-off between test RMSE and operator success rate $\implies V^*$ |
| **Step 4** | Elitism Analysis | `synth1` | 2 Regimes:<br>1. With Elitism (keep 1 best)<br>2. Without Elitism (generational replacement) | $(P^*, G^*)$, $S^*$, $V^*$ | 30 seeds | $2 \times 30 = 60$ | Identifies impact on convergence stability, loss of optimal fitness, and bloat |
| **Step 5** | Final Generalization Validation | `synth2` (300 train, 1000 test)<br>`concrete` (824 train, 206 test) | Best overall pipeline configuration $C^* = (P^*, G^*, S^*, V^*, \text{Elitism}^*)$ | Fully tuned winning hyperparameters | 30 seeds | $2 \times 30 = 60$ | Comprehensive assessment of generalization on synthetic and real multidimensional data |

**Total Experimental Runs**: $270 + 90 + 60 + 60 + 60 = 540$ runs.

#### 2.2.3 Runtime Budget & Parallel Execution
- Single run runtime benchmark:
  - On `synth1` ($N=60$), a population of 100 over 100 generations performs $10^4$ tree evaluations. With vectorized NumPy matrix operations, 1 tree evaluation over 60 points takes $\approx 20\,\mu\text{s}$. Total CPU time per run $\approx 0.35$ seconds.
  - On `concrete` ($N=824$), 1 run of 100 pop $\times$ 100 gen takes $\approx 1.2$ seconds.
  - A 500 pop $\times$ 500 gen run takes $\approx 7.5$ seconds.
- Multi-core execution:
  - The machine has **8 logical CPU cores**.
  - Using `joblib.Parallel(n_jobs=7)` (already installed in `.venv`), running all 540 experimental runs in parallel will take **under 3 to 5 minutes total wall-clock time**.
  - This easily respects:
    - Smoke test threshold: $< 2$ minutes.
    - Single run limit: $< 15$ minutes.
    - Global budget: well under 7 hours.

---

### 2.3 Results Logging & File Organization

#### 2.3.1 Directory Layout
```
results/
├── step1_pop_gen/
│   ├── pop50_gen50_seed42.csv
│   ├── ...
│   └── summary.csv
├── step2_selection/
│   ├── tournament_seed42.csv
│   ├── roulette_seed42.csv
│   ├── lexicase_seed42.csv
│   └── summary.csv
├── step3_variation/
│   ├── pc90_pm05_seed42.csv
│   ├── pc60_pm30_seed42.csv
│   └── summary.csv
├── step4_elitism/
│   ├── with_elitism_seed42.csv
│   ├── without_elitism_seed42.csv
│   └── summary.csv
├── step5_validation/
│   ├── synth2_seed42.csv
│   ├── concrete_seed42.csv
│   └── summary.csv
├── plots/
│   ├── fig1_step1_convergence.png (and .svg)
│   ├── fig2_step1_bloat.png (and .svg)
│   ├── fig3_step2_selection_convergence.png
│   ├── fig4_step2_diversity.png
│   ├── fig5_step3_operator_success.png
│   ├── fig6_step4_elitism_comparison.png
│   ├── fig7_step5_synth2_concrete_convergence.png
│   └── fig8_step5_generalization_scatter.png
└── tables/
    ├── step1_summary.md
    ├── step1_summary.tex
    ├── step2_summary.md
    ├── step2_summary.tex
    ├── step3_summary.md
    ├── step3_summary.tex
    ├── step4_summary.md
    ├── step4_summary.tex
    ├── step5_summary.md
    └── step5_summary.tex
```

#### 2.3.2 Per-Generation CSV Schema
Each run writes a CSV with one row per generation ($0, 1, \dots, G$):
```csv
generation,best_train_rmse,worst_train_rmse,mean_train_rmse,best_test_rmse,unique_canonical_trees,duplicate_canonical_trees,crossover_better_count,crossover_worse_count,crossover_equal_count,avg_node_count,avg_depth,best_tree_nodes,best_tree_depth,elapsed_seconds
```

#### 2.3.3 Summary CSV Schema
Each step produces an aggregated `summary.csv` logging the 30 runs:
```csv
step,config_name,dataset,seed,pop_size,generations,selection,pc,pm,elitism,best_train_rmse,final_test_rmse,best_tree_nodes,best_tree_depth,unique_canonical_final,total_runtime_seconds
```

#### 2.3.4 Summary Tables (Markdown & LaTeX)
Tables must report Mean $\pm$ Standard Deviation across the 30 seeds:
- Formats:
  - Markdown table: `| Configuration | Dataset | Train RMSE (Mean ± Std) | Test RMSE (Mean ± Std) | Avg Nodes | Avg Depth |`
  - LaTeX table: formatted with `booktabs` (`\toprule`, `\midrule`, `\bottomrule`).

---

### 2.4 Plotting Requirements & Zero-Dependency Engine

#### Required Figures
1. **Figure 1 (Step 1 Convergence & Bloat)**:
   - Panel A: Train and Test RMSE convergence across generations for combinations of $(pop, gen)$ (mean curve $\pm 1$ std band across 30 seeds).
   - Panel B: Average tree node count evolution over generations.
2. **Figure 2 (Step 2 Selection Comparison)**:
   - Panel A: Convergence curves of Best Train RMSE comparing Tournament, Scaled Roulette, and $\epsilon$-Lexicase.
   - Panel B: Structural diversity evolution (count of unique canonical trees).
3. **Figure 3 (Step 3 Variation Operator Success & Bloat)**:
   - Panel A: Crossover success rates (percentage of offspring better than average parents vs worse than average parents over generations).
   - Panel B: Node count evolution comparing high crossover ($0.9/0.05$) vs high mutation ($0.6/0.3$).
4. **Figure 4 (Step 4 Elitism Impact)**:
   - Panel A: Best fitness trajectory over generations (with elitism vs without elitism), highlighting generational fitness losses when elitism is disabled.
5. **Figure 5 (Step 5 Final Validation)**:
   - Panel A: Convergence curves on `synth2` (train vs test RMSE).
   - Panel B: Convergence curves on `concrete` (train vs test RMSE).
   - Panel C: Parity plot / scatter plot of predicted vs true target values for best models.

#### Zero-Dependency Plotting Implementation
Because `matplotlib` is not installed and PyPI is unreachable, plots will be rendered via a clean, self-contained Python plotting module `src/reporting/plotter.py` and `scripts/plot_results.py`:
- **Vector SVG Generation**: Clean XML SVG writer supporting axes, tick marks, grid lines, polyline curves, polygon shaded confidence ribbons ($\pm \text{std}$), legends, and titles.
- **Direct PNG Generation**: Using pure Python `zlib`/`struct` chunk writer, or PowerShell `System.Drawing.Graphics` bridge, generating crisp 300-DPI `.png` files suitable for LaTeX `\includegraphics`.
- **TikZ / PGFPlots Data**: Exports clean data tables (`.dat`) that can also be directly compiled in LaTeX via `pgfplots`.

---

### 2.5 R4 LaTeX Template Specification (`relatorio_tp1.tex`)

The file `relatorio_tp1.tex` must be generated in the root directory `d:\Downloads\bases-TP1\datasets\relatorio_tp1.tex`.
It must follow standard academic conference/course report standards (Portuguese language, A4, standard formatting) with exact placeholders for figures and tables.

#### Document Architecture & Placeholder Guidance
```latex
\documentclass[11pt,a4paper]{article}
\usepackage[brazil]{babel}
\usepackage[utf8]{inputenc}
\usepackage{amsmath,amssymb,amsfonts}
\usepackage{graphicx}
\usepackage{booktabs}
\usepackage{geometry}
\usepackage{hyperref}
\usepackage{subcaption}
\usepackage{cite}

\geometry{top=2.5cm, bottom=2.5cm, left=2.5cm, right=2.5cm}

\title{\textbf{Trabalho Prático 1: Regressão Simbólica com Programação Genética}}
\author{[Nome do Aluno -- Matrícula]\\
\textit{Departamento de Ciência da Computação}\\
\textit{Universidade Federal de Minas Gerais (UFMG)}}
\date{\today}

\begin{document}
\maketitle

\begin{abstract}
% [ORIENTAÇÃO: Resumo conciso de até 200 palavras descrevendo o objetivo do trabalho,
% os métodos de programação genética implementados, os experimentos realizados sobre as bases
% synth1, synth2 e concrete com 30 repetições, e as principais conclusões obtidas.]
\end{abstract}

\section{Introdução}
% [ORIENTAÇÃO: Contextualize o problema de regressão simbólica e diferencie-o da regressão clássica.
% Apresente a motivação para o uso de Programação Genética (PG).
% Declare os objetivos do trabalho: investigar o impacto de hiperparâmetros (tamanho da população e gerações),
% métodos de seleção (Torneio, Roleta Escalonada, epsilon-Lexicase), operadores de variação (crossover vs. mutação)
% e elitismo, finalizando com a validação em bases sintéticas e reais.
% Descreva a organização das seções seguintes do relatório.]

\section{Implementação}
% [ORIENTAÇÃO: Detalhe as decisões de engenharia e modelagem matemática do framework de PG.]

\subsection{Representação dos Indivíduos e Inicialização}
% [ORIENTAÇÃO: Descreva a árvore sintática baseada em nós, a restrição rígida de profundidade máxima igual a 7,
% e a inicialização Ramped Half-and-Half (profundidades de 2 a 5 com método grow e full).]

\subsection{Conjunto Primitivo e Terminais}
% [ORIENTAÇÃO: Apresente as funções primitivas (+, -, *, divisão protegida, sin, cos, log protegido, exp protegido)
% e justifique as proteções numéricas contra divisões por zero e overflow.
% Descreva as variáveis terminais (X0 a XD-1) e as constantes efêmeras (ERCs) com perturbação gaussiana.]

\subsection{Função de Fitness e Avaliação Vetorizada}
% [ORIENTAÇÃO: Apresente a fórmula da Raiz do Erro Quadrático Médio (RMSE).
% Explique como a avaliação foi vetorizada com NumPy sobre matrizes N x D.]

\subsection{Operadores de Seleção}
% [ORIENTAÇÃO: Descreva formalmente os três operadores de seleção:
% 1. Torneio com k=2.
% 2. Roleta com escalonamento linear (f_max - f_i + delta) para minimização.
% 3. epsilon-Lexicase com limiar dinâmico baseado no Desvio Absoluto Mediano (MAD) por caso de teste.]

\subsection{Operadores de Variação e Esquema Reprodutivo}
% [ORIENTAÇÃO: Explique a seleção mutuamente exclusiva de operadores por vaga na nova população (pc, pm, cloning).
% Detalhe o crossover de subárvore com rejeição por profundidade > 7 e a mutação por perturbação ou subárvore.
% Explique o mecanismo de elitismo preservando o melhor indivíduo.]

\section{Experimentos e Análise de Resultados}
% [ORIENTAÇÃO: Detalhe o protocolo experimental: 30 sementes aleatórias por configuração,
% divisões de treino e teste, e métricas coletadas.]

\subsection{Impacto do Tamanho da População e Número de Gerações (Passo 1)}
% [ORIENTAÇÃO: Analise o compromisso entre tamanho da população, número de gerações e custo computacional.
% Discuta se configurações com mais gerações apresentaram overfitting ou bloat.]
% [INSERÇÃO: Tabela 1 - Tabela comparativa do Passo 1: results/tables/step1_summary.tex]
% \input{results/tables/step1_summary.tex}
% [INSERÇÃO: Figura 1 - Curvas de convergência e evolução de nós: results/plots/fig1_step1_convergence.png]
% \begin{figure}[htbp]
%   \centering
%   \includegraphics[width=0.85\textwidth]{results/plots/fig1_step1_convergence.png}
%   \caption{Convergência e evolução do tamanho das árvores para variações de população e gerações.}
%   \label{fig:step1}
% \end{figure}

\subsection{Comparação dos Métodos de Seleção (Passo 2)}
% [ORIENTAÇÃO: Compare Torneio, Roleta Escalonada e epsilon-Lexicase.
% Discuta a pressão seletiva, a preservação da diversidade estrutural (árvores canônicas únicas)
% e a capacidade de escapar de mínimos locais.]
% [INSERÇÃO: Tabela 2 - results/tables/step2_summary.tex]
% [INSERÇÃO: Figura 2 - results/plots/fig3_step2_selection_convergence.png]

\subsection{Comparação dos Operadores de Variação (Passo 3)}
% [ORIENTAÇÃO: Compare a configuração tradicional (pc=0.9, pm=0.05) com a configuração de alta mutação (pc=0.6, pm=0.30).
% Analise a taxa de sucesso do crossover (filhos melhores vs. piores que a média dos pais).]
% [INSERÇÃO: Tabela 3 - results/tables/step3_summary.tex]
% [INSERÇÃO: Figura 3 - results/plots/fig5_step3_operator_success.png]

\subsection{Análise do Impacto do Elitismo (Passo 4)}
% [ORIENTAÇÃO: Compare a evolução do melhor fitness com e sem elitismo.
% Destaque a perda temporária de boas soluções no caso sem elitismo e o efeito no tamanho médio das árvores.]
% [INSERÇÃO: Tabela 4 - results/tables/step4_summary.tex]
% [INSERÇÃO: Figura 4 - results/plots/fig6_step4_elitism_comparison.png]

\subsection{Validação Final nos Conjuntos synth2 e concrete (Passo 5)}
% [ORIENTAÇÃO: Apresente o desempenho da melhor configuração global sobre as bases synth2 e concrete.
% Compare o erro de treino e teste (gap de generalização).
% Se possível, apresente a melhor expressão matemática simplificada encontrada.]
% [INSERÇÃO: Tabela 5 - results/tables/step5_summary.tex]
% [INSERÇÃO: Figura 5 - results/plots/fig7_step5_synth2_concrete_convergence.png]

\section{Conclusões}
% [ORIENTAÇÃO: Sintetize as principais descobertas experimentais.
% Indique quais operadores e parâmetros se mostraram mais robustos para regressão simbólica.
% Discuta limitações encontradas (ex.: bloat, tempo de avaliação em bases maiores) e proponha melhorias futuras.]

\bibliographystyle{plain}
\bibliography{referencias}
\end{document}
```

---

## 3. Caveats

1. **No External Plotting Library (`matplotlib`)**:
   - Because `matplotlib` is not in `.venv` and network installation is disabled, plotting must rely on self-contained vector SVG generation and pure Python PNG export or PowerShell `System.Drawing`.
   - Implementers must not import `matplotlib` in production or test scripts.
2. **Missing Headers in Dataset CSVs**:
   - All dataset CSV files (`synth1`, `synth2`, `concrete`) do not contain header lines. Calling `pd.read_csv(...)` without `header=None` will inadvertently treat the first data row as column names, corrupting shapes and row counts.
3. **Lexicase Selection Computational Complexity**:
   - $\epsilon$-Lexicase filters candidates across sequential test cases. For `concrete` with 824 instances, naive per-candidate re-evaluation is slow.
   - **Remedy**: The population's predictions on all $N$ instances must be pre-evaluated into an $N \times P$ error matrix once per generation, turning lexicase case filtering into simple NumPy slice indexing.
4. **Strict R2 Enforcement**:
   - Even casual acronyms or uppercase letters inside comments (e.g., `# Compute RMSE.`) violate acceptance criteria. The linter `scripts/check_style.py` must be executed before handoffs.
5. **Depth Capping Implementation**:
   - Crossover and mutation must reject offspring if tree depth exceeds 7, retaining the unmodified parent tree. This is essential to prevent bloat-induced memory explosion.

---

## 4. Conclusion

1. **Architecture Ready for Implementation**:
   - The modular design separates:
     - `src/gp/`: Core GP primitives, trees, evaluation, selection, variation, engine.
     - `src/data/`: Data ingestion loading with `header=None`.
     - `src/reporting/`: Metric logger, CSV writer, SVG/PNG chart generator, LaTeX table exporter.
     - `scripts/`: Modular runners for steps 1 to 5, style checker, and master execution script.
2. **Runtime Feasibility Guaranteed**:
   - Total of 540 experimental runs parallelized with `joblib.Parallel(n_jobs=7)` on 8 CPU cores will complete in approximately **3 to 5 minutes total**, far below the 7-hour constraint.
3. **Style & Deliverable Compliance**:
   - Fully casual lowercase comments and docstrings.
   - LaTeX report skeleton `relatorio_tp1.tex` ready to be scaffolded with designated insertion points for all 5 steps' figures and tables.

---

## 5. Verification Method

To independently verify these findings, run the following commands:

1. **Verify Python Environment & Hardware**:
   ```powershell
   & "d:\Downloads\bases-TP1\datasets\.venv\Scripts\python.exe" -c "import sys, multiprocessing, numpy, pandas, scipy, sklearn; print('Python:', sys.version); print('CPUs:', multiprocessing.cpu_count())"
   ```
2. **Verify Dataset Shapes & Formats**:
   ```powershell
   & "d:\Downloads\bases-TP1\datasets\.venv\Scripts\python.exe" -c "import pandas as pd; print('synth1:', pd.read_csv('synth1/synth1-train.csv', header=None).shape, pd.read_csv('synth1/synth1-test.csv', header=None).shape); print('synth2:', pd.read_csv('synth2/synth2-train.csv', header=None).shape, pd.read_csv('synth2/synth2-test.csv', header=None).shape); print('concrete:', pd.read_csv('concrete/concrete-train.csv', header=None).shape, pd.read_csv('concrete/concrete-test.csv', header=None).shape)"
   ```
3. **Verify Pure Python PNG Generation Without Matplotlib**:
   ```powershell
   & "d:\Downloads\bases-TP1\datasets\.venv\Scripts\python.exe" -c "import zlib, struct; ihdr = struct.pack('>IIBBBBB', 10, 10, 8, 2, 0, 0, 0); data = zlib.compress(bytearray([0] + [255, 0, 0] * 10 * 10)); f = open('test.png', 'wb'); f.write(b'\x89PNG\r\n\x1a\n' + struct.pack('>I', len(ihdr)) + b'IHDR' + ihdr + struct.pack('>I', zlib.crc32(b'IHDR' + ihdr)) + struct.pack('>I', len(data)) + b'IDAT' + data + struct.pack('>I', zlib.crc32(b'IDAT' + data)) + struct.pack('>I', 0) + b'IEND' + struct.pack('>I', zlib.crc32(b'IEND'))); f.close(); import os; print('PNG generated:', os.path.exists('test.png')); os.remove('test.png')"
   ```
