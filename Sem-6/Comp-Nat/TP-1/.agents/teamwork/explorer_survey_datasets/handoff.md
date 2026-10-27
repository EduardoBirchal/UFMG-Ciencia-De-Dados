# Handoff Report: Survey of Datasets and Python Environment

**Agent**: `explorer_survey_datasets`  
**Date**: 2026-10-26T18:00:00Z  
**Target Recipient**: Orchestrator (`0fbe2801-fce8-49f2-8814-84768519d3f0`)

---

## 1. Observation

### 1.1 Filesystem Structure & Dataset Files
Direct inspection of workspace `d:\Downloads\bases-TP1\datasets` revealed 6 CSV files partitioned into 3 dataset subdirectories:

1. **`synth1/`**:
   - `synth1/synth1-train.csv` (3,443 bytes, 60 lines)
   - `synth1/synth1-test.csv` (34,509 bytes, 600 lines)
2. **`synth2/`**:
   - `synth2/synth2-train.csv` (11,849 bytes, 300 lines)
   - `synth2/synth2-test.csv` (39,555 bytes, 1,000 lines)
3. **`concrete/`**:
   - `concrete/concrete-train.csv` (32,362 bytes, 824 lines)
   - `concrete/concrete-test.csv` (7,956 bytes, 206 lines)

### 1.2 Dataset Properties & Formatting
All 6 files were parsed and validated using Python 3.14 / Pandas / NumPy:

| Dataset | Split | Row Count ($N$) | Col Count | Feature Dim ($D$) | Features Cols | Target Col | Missing Values | Delimiter | Header |
|---|---|---|---|---|---|---|---|---|---|
| `synth1` | Train | 60 | 3 | 2 | `0, 1` ($X_0, X_1$) | `2` ($y$) | 0 | Comma `,` | None |
| `synth1` | Test | 600 | 3 | 2 | `0, 1` ($X_0, X_1$) | `2` ($y$) | 0 | Comma `,` | None |
| `synth2` | Train | 300 | 3 | 2 | `0, 1` ($X_0, X_1$) | `2` ($y$) | 0 | Comma `,` | None |
| `synth2` | Test | 1000 | 3 | 2 | `0, 1` ($X_0, X_1$) | `2` ($y$) | 0 | Comma `,` | None |
| `concrete` | Train | 824 | 9 | 8 | `0..7` ($X_0..X_7$) | `8` ($y$) | 0 | Comma `,` | None |
| `concrete` | Test | 206 | 9 | 8 | `0..7` ($X_0..X_7$) | `8` ($y$) | 0 | Comma `,` | None |

### 1.3 Statistical Profiles & Baselines

#### A. `synth1`
- Feature $X_0$: Train range $[-2.8141, 2.9007]$, mean $0.4996$, std $1.5586$.
- Feature $X_1$: Train range $[-2.9780, 2.8928]$, mean $-0.0013$, std $1.5876$.
- Target $y$: Train range $[-0.0814, 84.6736]$, mean $12.8309$, std $18.6693$. Test range $[-0.5830, 115.5000]$, mean $17.7503$, std $25.8896$.
- **Ground Truth Discovery**: Polynomial regression fitting revealed the exact generative formula of `synth1` with training RMSE $1.88 \times 10^{-14}$:
  $$y = X_0^4 - X_0^3 + 0.5 X_1^2 - X_1$$
  Verification on test set: $X_0 = -3.0, X_1 = -3.0 \implies (-3)^4 - (-3)^3 + 0.5(-3)^2 - (-3) = 81 + 27 + 4.5 + 3 = 115.5$, exactly matching line 1 of `synth1-test.csv`.
- In `synth1-test.csv`, all 600 evaluation points have $X_0 = X_1$ evenly sampled along $[-3.0, 2.99]$ with step $0.01$.
- **Baselines**:
  - Dummy Mean Predictor: Train RMSE = 18.6693, Test RMSE = 26.3528
  - Linear Regression (OLS): Train RMSE = 18.3898, Test RMSE = 26.1186

#### B. `synth2`
- Feature $X_0$: Train range $[0.0711, 6.0487]$, mean $2.9392$, std $1.8061$.
- Feature $X_1$: Train range $[0.0697, 6.0449]$, mean $3.0274$, std $1.7989$.
- Target $y$: Train range $[-10.4548, 9.6156]$, mean $0.0710$, std $3.6799$. Test range $[-10.9463, 9.7395]$, mean $0.3579$, std $4.0262$.
- Domain: Approximately $[0, 2\pi] \times [0, 2\pi]$ bivariate nonlinear surface with oscillatory patterns.
- **Baselines**:
  - Dummy Mean Predictor: Train RMSE = 3.6799, Test RMSE = 4.0364
  - Linear Regression (OLS): Train RMSE = 3.6693, Test RMSE = 4.0486

#### C. `concrete`
- Real-world UCI Concrete Compressive Strength dataset ($N = 1030$, pre-split into 824 train [80%] and 206 test [20%]).
- 8 Input Features ($X_0 \dots X_7$):
  - $X_0$: Cement (kg/m$^3$), range $[102.0, 540.0]$
  - $X_1$: Blast Furnace Slag (kg/m$^3$), range $[0.0, 359.4]$
  - $X_2$: Fly Ash (kg/m$^3$), range $[0.0, 195.0]$
  - $X_3$: Water (kg/m$^3$), range $[121.8, 247.0]$
  - $X_4$: Superplasticizer (kg/m$^3$), range $[0.0, 32.2]$
  - $X_5$: Coarse Aggregate (kg/m$^3$), range $[801.0, 1145.0]$
  - $X_6$: Fine Aggregate (kg/m$^3$), range $[594.0, 992.6]$
  - $X_7$: Age (days, integer values), range $[1, 365]$
- Target $y$: Concrete Compressive Strength (MPa), range $[2.33, 82.60]$, mean $35.83$, std $16.80$.
- **Baselines**:
  - Dummy Mean Predictor: Train RMSE = 16.8021, Test RMSE = 16.2732
  - Linear Regression (OLS): Train RMSE = 10.4160, Test RMSE = 10.1277

### 1.4 Python Virtual Environment Inspection
- **Python Executable**: `d:\Downloads\bases-TP1\datasets\.venv\Scripts\python.exe`
- **Python Version**: `3.14.3` (64-bit Windows)
- **Installed Packages (`pip list`)**:
  - `numpy`: `2.5.4`
  - `pandas`: `3.1.0`
  - `scipy`: `1.18.1`
  - `scikit-learn`: `1.9.1`
  - `joblib`: `1.6.0`
  - `threadpoolctl`: `3.7.0`
  - `cloudpickle`: `3.1.2`
  - `narwhals`: `2.27.1`
  - `python-dateutil`: `3.0.0`
  - `six`: `1.17.0`
  - `tzdata`: `2026.5`
  - `pip`: `26.2.2`
- **Critical Environment Limitation**:
  - `matplotlib` is **NOT** installed (`ModuleNotFoundError: No module named 'matplotlib'`).
  - Network access to PyPI is disabled in this environment (`HTTPSConnection(host='pypi.org', port=443): Failed to resolve 'pypi.org' ([Errno 11001] getaddrinfo failed)`).
  - Unsandboxed execution is prohibited by permissions.
  - Therefore, figure generation must not depend on `matplotlib`. A lightweight, pure-Python SVG generator (or TikZ/pgfplots export) must be used to generate publication-quality figures without external dependencies.

---

## 2. Logic Chain

1. **Pre-split Datasets**:
   - Observation: All three datasets already have explicit, separate `*-train.csv` and `*-test.csv` files.
   - Inference: The framework must not perform random train/test splitting. It must ingest the exact pre-existing train and test files directly to guarantee comparability and reproducibility across experimental steps.

2. **Standardized Feature & Target Indexing**:
   - Observation: Across all files, features are columns $0 \dots D-1$ and target $y$ is strictly column $D$. None have headers.
   - Inference: A single generic loader `load_dataset(name, root_dir)` can read each CSV via `pd.read_csv(filepath, header=None).values` (or `np.loadtxt(filepath, delimiter=',')`), slicing `X = data[:, :-1]` and `y = data[:, -1]`.

3. **Data Type and Memory Efficiency**:
   - Observation: The largest dataset (`concrete-train.csv`) has 824 rows and 8 features, taking just 59 KB in memory. The entire data footprint across all benchmarks is under 120 KB.
   - Inference: All datasets can be pre-loaded once into memory as contiguous `np.float64` arrays (`np.ascontiguousarray`). Vectorized tree evaluation across $N$ rows will execute in sub-millisecond timeframes, ensuring that 30 seeds of 100 generations will complete well within the required time budget.

4. **Preservation of Raw Feature Scales**:
   - Observation: For `synth1`, the ground truth is $y = X_0^4 - X_0^3 + 0.5 X_1^2 - X_1$ in the raw input domain.
   - Inference: Feature normalization / z-score standardization must NOT be applied to inputs, as scaling would destroy the exact polynomial target relationships that GP is tasked to recover.

5. **NumPy 2.x Compatibility**:
   - Observation: `numpy` version is `2.5.4`. In NumPy 2.x, deprecated types like `np.float_`, `np.int_`, or legacy scalar type aliases have been removed.
   - Inference: All GP code and node evaluations must explicitly use standard Python `float`, `int`, or canonical NumPy types `np.float64`, `np.int64`.

6. **Plotting Solution without Matplotlib**:
   - Observation: `matplotlib` is missing and cannot be installed via pip.
   - Inference: To satisfy Requirement R3 ("Generation of publication-ready figures: convergence curves, bloat/node evolution, crossover operator success rates"), the project can implement a clean, standalone, zero-dependency SVG generator in pure Python (e.g. `src/plot_svg.py`) that reads the CSV logs from `results/` and outputs crisp, publication-grade `.svg` vector graphics (which can be viewed in any browser or embedded into LaTeX/HTML). Furthermore, tables can be exported to clean LaTeX tabular code.

---

## 3. Caveats

1. **No External Libraries for Plotting**:
   `matplotlib`, `seaborn`, and `plotly` are unavailable. The visualization pipeline must rely on pure Python SVG generation or LaTeX TikZ/pgfplots generation.
2. **`synth1-test.csv` Diagonal Geometry**:
   In `synth1-test.csv`, $X_0 == X_1$ for all rows (evaluating along the diagonal line $X_0 = X_1 \in [-3, 3]$). The training set `synth1-train.csv`, however, samples $X_0$ and $X_1$ across the full 2D domain. The GP tree must generalize from 2D scattered points to the 1D slice.
3. **`concrete` Feature Scales**:
   The `concrete` dataset has large feature differences ($X_5$ Coarse Aggregate reaches 1145, while $X_4$ Superplasticizer is between 0 and 32). In unnormalized GP, Ephemeral Random Constants (ERCs) generated in $[-1, 1]$ or $[-5, 5]$ may require arithmetic combinations to match large coefficients; this is standard behavior in symbolic regression benchmarks.

---

## 4. Conclusion

1. **Data Ingestion Contract**:
   A dedicated module `src/data.py` should be implemented with the following minimal signature:
   ```python
   # loads pre-split benchmark dataset as contiguous numpy arrays
   def load_dataset(dataset_name: str, base_dir: str = "."):
       # returns: (X_train, y_train, X_test, y_test)
       # X: np.ndarray shape (N, D), dtype float64
       # y: np.ndarray shape (N,), dtype float64
   ```
2. **Dataset Dimensionality Metadata**:
   - `synth1`: $D = 2$, train $N = 60$, test $N = 600$, $X \in \mathbb{R}^{N \times 2}$, $y \in \mathbb{R}^N$.
   - `synth2`: $D = 2$, train $N = 300$, test $N = 1000$, $X \in \mathbb{R}^{N \times 2}$, $y \in \mathbb{R}^N$.
   - `concrete`: $D = 8$, train $N = 824$, test $N = 206$, $X \in \mathbb{R}^{N \times 8}$, $y \in \mathbb{R}^N$.
3. **Execution Feasibility**:
   With data shapes between 60 and 824 rows, NumPy-vectorized tree evaluation will easily achieve $< 5$ seconds per 100-generation GP run, guaranteeing that the 30-seed experimental suite finishes in tens of minutes, well below the 7-hour limit.
4. **Zero-dependency SVG Plotting**:
   A pure Python SVG plotting script is recommended to create the publication figures for convergence, bloat, and operator success rates.

---

## 5. Verification Method

### 5.1 Verification Commands
Run the following commands using `.venv\Scripts\python.exe` to independently verify data loading, shapes, and ground-truth validation:

1. **Verify Dataset Loading and Shapes**:
   ```powershell
   & "d:\Downloads\bases-TP1\datasets\.venv\Scripts\python.exe" -c "
   import pandas as pd
   for name, tr, te, d in [('synth1','synth1/synth1-train.csv','synth1/synth1-test.csv',2),
                           ('synth2','synth2/synth2-train.csv','synth2/synth2-test.csv',2),
                           ('concrete','concrete/concrete-train.csv','concrete/concrete-test.csv',8)]:
       d_tr = pd.read_csv(tr, header=None).values
       d_te = pd.read_csv(te, header=None).values
       assert d_tr.shape[1] == d + 1, f'bad train dim {d_tr.shape}'
       assert d_te.shape[1] == d + 1, f'bad test dim {d_te.shape}'
       assert not pd.isna(d_tr).any(), 'nan in train'
       assert not pd.isna(d_te).any(), 'nan in test'
   print('ALL DATASETS VERIFIED OK')
   "
   ```

2. **Verify `synth1` Ground Truth Polynomial**:
   ```powershell
   & "d:\Downloads\bases-TP1\datasets\.venv\Scripts\python.exe" -c "
   import pandas as pd, numpy as np
   d = pd.read_csv('synth1/synth1-train.csv', header=None).values
   x0, x1, y = d[:,0], d[:,1], d[:,2]
   y_true = x0**4 - x0**3 + 0.5*x1**2 - x1
   rmse = np.sqrt(np.mean((y - y_true)**2))
   assert rmse < 1e-12, f'rmse mismatch: {rmse}'
   print('SYNTH1 GROUND TRUTH VERIFIED: RMSE =', rmse)
   "
   ```

3. **Verify Environment Packages & Missing Matplotlib**:
   ```powershell
   & "d:\Downloads\bases-TP1\datasets\.venv\Scripts\python.exe" -c "
   import numpy, pandas, scipy, sklearn
   print('NumPy:', numpy.__version__)
   print('Pandas:', pandas.__version__)
   print('SciPy:', scipy.__version__)
   print('Scikit-Learn:', sklearn.__version__)
   try:
       import matplotlib
   except ModuleNotFoundError:
       print('Matplotlib confirmed NOT installed')
   "
   ```

### 5.2 Invalidation Conditions
- Any CSV file found to have headers or non-numeric entries.
- Any change in the filesystem structure moving files outside `synth1/`, `synth2/`, or `concrete/`.
- Discovery that `matplotlib` is required and must be supplied by an external offline wheel.
