# Task Assignment: Milestone 1 Worker (Data Ingestion & Style Linter)

## Working Directory
d:\Downloads\bases-TP1\datasets\.agents\teamwork\worker_m1

## Objective
Implement and verify Milestone 1:
1. `src/__init__.py`
2. `src/data.py`:
   - `load_dataset(dataset_name: str, base_dir: str = ".") -> tuple[np.ndarray, np.ndarray, np.ndarray, np.ndarray]`
   - Supports 'synth1', 'synth2', 'concrete'.
   - Reads CSV files without headers (`header=None`), slices features $X = data[:, :-1]$ and target $y = data[:, -1]$.
   - Returns contiguous `float64` NumPy arrays (`np.ascontiguousarray`).
   - Validates shapes ($D=2$ for synth1/synth2, $D=8$ for concrete).
   - Zero NaNs/Infs, raw feature scale preserved.
3. `scripts/check_style.py`:
   - Linter script scanning all `.py` files using `tokenize` and `ast`.
   - Verifies R2 constraint: strictly all-lowercase comments without trailing periods, and strictly all-lowercase docstrings without trailing periods.
   - Returns exit code 0 if all clean, exit code 1 with line numbers and error details if violations found.
4. `tests/__init__.py`, `tests/test_data.py`, and `tests/test_style.py`:
   - Unit tests verifying:
     - All 3 datasets load with correct train/test row counts and dimensions.
     - Target values match expected ranges and ground truth.
     - Style linter correctly passes compliant files and detects uppercase or trailing periods.
     - Self-test: style linter verifies that all code written in `src/` and `tests/` passes style checks!
5. Run the tests using `.venv\Scripts\python.exe -m pytest tests/test_data.py tests/test_style.py` and run `scripts/check_style.py`.

## Exclusively Owned Files
- `src/__init__.py`
- `src/data.py`
- `scripts/check_style.py`
- `tests/__init__.py`
- `tests/test_data.py`
- `tests/test_style.py`

## Mandatory Warning
DO NOT CHEAT. All implementations must be genuine. DO NOT hardcode test results, create dummy/facade implementations, or circumvent the intended task. A teamwork_preview_auditor will independently verify your work. Integrity violations WILL be detected and your work WILL be rejected.

## Code Style Rule (R2)
All comments and docstrings MUST be strictly casual lowercase without trailing periods! Handwritten look.

## Inputs
- `d:\Downloads\bases-TP1\datasets\.agents\teamwork\ORIGINAL_REQUEST.md`
- `d:\Downloads\bases-TP1\datasets\PROJECT.md`
- `d:\Downloads\bases-TP1\datasets\.agents\teamwork\explorer_survey_datasets\handoff.md`
- `d:\Downloads\bases-TP1\datasets\.agents\teamwork\explorer_survey_experiments_report\handoff.md`

## Output
Write your report to `d:\Downloads\bases-TP1\datasets\.agents\teamwork\worker_m1\handoff.md` with:
- Commands executed and full passing test output
- Code structure and layout compliance


## 2026-10-26T18:01:42Z
You are a Worker subagent implementing Milestone 1: Data Ingestion & Style Linter.
Your working directory is: d:\Downloads\bases-TP1\datasets\.agents\teamwork\worker_m1
