# Task Assignment: Milestone 6 Challenger (Tier 5 Adversarial Coverage Hardening)

## Working Directory
d:\Downloads\bases-TP1\datasets\.agents\teamwork\challenger_m6

## Objective
Execute Phase 2 (Adversarial Coverage Hardening - Tier 5) for Milestone 6:
1. White-box analysis:
   - Read the implementation source in `src/data.py`, `src/gp/*.py`, and `src/reporting/*.py` along with existing tests in `tests/`.
   - Identify untested code paths, extreme parameter combinations, and adversarial edge cases.
2. Implement `tests/e2e/test_tier5_adversarial.py`:
   - Design at least 15-20 rigorous adversarial test cases covering:
     - Numerical stress: extreme values (+/- 1e15, 1e-15, single sample N=1, high dimensional D=50).
     - Tree stress: maximum depth 7 trees with 127 nodes, deeply nested transcendental functions (`sin(cos(exp(log(...))))`), deep division chains.
     - Exponential clamping: verify `safe_exp` never produces overflow warnings or Infs even with extreme arguments.
     - Degenerate populations: identical fitness across population (zero variance in tournament, scaled roulette, and epsilon-lexicase).
     - Variation extremes: pure crossover ($p_c=1.0, p_m=0.0$), pure mutation ($p_c=0.0, p_m=1.0$), pure cloning ($p_c=0.0, p_m=0.0$).
     - Deterministic reproducibility under various seed conditions.
     - Parity and plotting with hostile/adversarial inputs (constant predictions, infinite error bounds, 1-point datasets).
3. Verification:
   - Ensure all new test cases follow the R2 code style: strictly casual lowercase comments and docstrings without trailing periods!
   - Run `& "d:\Downloads\bases-TP1\datasets\.venv\Scripts\python.exe" scripts/check_style.py` to confirm 0 violations.
   - Run `& "d:\Downloads\bases-TP1\datasets\.venv\Scripts\python.exe" -m pytest tests/` and verify that 100% of all tests pass (Tiers 1-5 + unit tests).
4. Deliver report:
   Write your handoff report to:
   `d:\Downloads\bases-TP1\datasets\.agents\teamwork\challenger_m6\handoff.md`
   Follow the Handoff Protocol (Observation, Logic Chain, Caveats, Conclusion, Verification Method).
   Conclude with a clear verdict: **APPROVE** or **REQUEST_CHANGES**.

## Inputs
- `d:\Downloads\bases-TP1\datasets\.agents\teamwork\ORIGINAL_REQUEST.md`
- `d:\Downloads\bases-TP1\datasets\PROJECT.md`
- `d:\Downloads\bases-TP1\datasets\TEST_INFRA.md`
- `d:\Downloads\bases-TP1\datasets\TEST_READY.md`

## 2026-10-27T00:24:49Z
You are the Milestone 6 Adversarial Challenger (Tier 5 Adversarial Coverage Hardening).
Your working directory is: d:\Downloads\bases-TP1\datasets\.agents\teamwork\challenger_m6

Read the authoritative specification files before starting:
1. ORIGINAL_REQUEST.md: d:\Downloads\bases-TP1\datasets\.agents\teamwork\ORIGINAL_REQUEST.md
2. DISPATCH.md: d:\Downloads\bases-TP1\datasets\.agents\teamwork\challenger_m6\DISPATCH.md
3. PROJECT.md: d:\Downloads\bases-TP1\datasets\PROJECT.md
4. TEST_INFRA.md: d:\Downloads\bases-TP1\datasets\TEST_INFRA.md
5. TEST_READY.md: d:\Downloads\bases-TP1\datasets\TEST_READY.md

MANDATORY STYLE RULE (R2):
Every comment and docstring in every Python file you create or edit MUST be strictly casual lowercase without trailing periods! Handwritten look.

YOUR MISSION:
1. Perform white-box adversarial analysis of the codebase (`src/data.py`, `src/gp/*.py`, `src/reporting/*.py`).
2. Implement `tests/e2e/test_tier5_adversarial.py` containing at least 15-20 rigorous adversarial test cases:
   - Numerical stress (+/- 1e15, 1e-15, N=1, D=50).
   - Maximum depth trees (depth=7, 127 nodes), deeply nested functions, safe_exp clamping without overflow warnings.
   - Degenerate populations (all identical fitness in tournament, roulette, and epsilon-lexicase).
   - Variation extremes (pc=1.0/pm=0.0, pc=0.0/pm=1.0, pc=0.0/pm=0.0).
   - Deterministic seed reproduction under various conditions.
   - Reporting/plotting stress with constant predictions or hostile input matrices.
3. Verify that all existing and new tests pass:
   - Run `scripts/check_style.py` to confirm 0 violations across all codebase files.
   - Run `pytest` to confirm 100% of tests pass.
4. Deliver your handoff report to `d:\Downloads\bases-TP1\datasets\.agents\teamwork\challenger_m6\handoff.md` with verdict APPROVE or REQUEST_CHANGES.
5. Send a brief message to your parent when complete.

