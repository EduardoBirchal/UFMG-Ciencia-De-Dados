# Task Assignment: E2E Test Suite Creation (Tiers 1 to 4)

## Working Directory
d:\Downloads\bases-TP1\datasets\.agents\teamwork\test_writer_e2e

## Objective
Design and implement the comprehensive, opaque-box E2E test suite for the Genetic Programming framework based on `ORIGINAL_REQUEST.md` and `PROJECT.md`:
1. Create `d:\Downloads\bases-TP1\datasets\TEST_INFRA.md` using the template from Project Pattern.
2. Implement E2E test files in `tests/e2e/`:
   - `tests/e2e/test_tier1_features.py`: >=5 test cases per feature (happy path in isolation, simple verification).
   - `tests/e2e/test_tier2_boundaries.py`: >=5 test cases per feature covering boundary/corner cases (max depth 7 capping, division by zero, log(0), exp overflow clipping, dynamic MAD with identical candidate errors, single individual pools, empty/large features).
   - `tests/e2e/test_tier3_combinations.py`: Pairwise cross-feature interactions (e.g. roulette vs tournament with elitism on/off, high mutation with lexicase, ramped half-and-half with various dimensions).
   - `tests/e2e/test_tier4_scenarios.py`: Real-world application scenarios (full symbolic regression runs on synth1, synth2, and concrete verifying RMSE convergence, metric logging, and duplicate tracking).
3. Test runner: Ensure all tests can be discovered and executed via `pytest tests/e2e` using the virtual environment `.venv\Scripts\python.exe`.
4. Create `d:\Downloads\bases-TP1\datasets\TEST_READY.md` when the test suite is complete.

## Strict Constraints
- MANDATORY: Read `d:\Downloads\bases-TP1\datasets\.agents\teamwork\ORIGINAL_REQUEST.md` before starting work.
- R2 Code Style: All comments and docstrings in test code must be strictly casual lowercase without trailing periods!
- No test may take longer than 15 minutes; smoke/unit assertions should execute quickly.
- Do NOT modify any implementation code in `src/`. You own `TEST_INFRA.md`, `TEST_READY.md`, and `tests/e2e/*`.

## Output
Write your handoff report to:
`d:\Downloads\bases-TP1\datasets\.agents\teamwork\test_writer_e2e\handoff.md`
Report the test runner command, test count per tier, and coverage summary.

## 2026-10-26T18:01:24Z
You are an E2E Test Writer subagent for the Genetic Programming project.
Your working directory is: d:\Downloads\bases-TP1\datasets\.agents\teamwork\test_writer_e2e

MANDATORY: Read ORIGINAL_REQUEST.md at:
d:\Downloads\bases-TP1\datasets\.agents\teamwork\ORIGINAL_REQUEST.md
and PROJECT.md at:
d:\Downloads\bases-TP1\datasets\PROJECT.md
before starting work.

Read your task assignment at:
d:\Downloads\bases-TP1\datasets\.agents\teamwork\test_writer_e2e\DISPATCH.md

Your responsibilities:
1. Design and write TEST_INFRA.md at the project root (d:\Downloads\bases-TP1\datasets\TEST_INFRA.md).
2. Write comprehensive, opaque-box test suites in tests/e2e/:
   - test_tier1_features.py (>=5 tests per feature, happy-path in isolation)
   - test_tier2_boundaries.py (>=5 tests per feature covering boundary/corner cases: depth 7 limit, div by zero, log(0), exp overflow, identical candidate dynamic MAD, etc.)
   - test_tier3_combinations.py (pairwise feature interactions)
   - test_tier4_scenarios.py (real-world application runs on synth1, synth2, concrete)
3. Ensure all tests run with pytest using d:\Downloads\bases-TP1\datasets\.venv\Scripts\python.exe.
4. CRITICAL STYLE CONSTRAINT (R2): Every single comment and docstring in your test code must be strictly casual lowercase without trailing periods!
5. When the full test suite is created and documented, generate TEST_READY.md at project root (d:\Downloads\bases-TP1\datasets\TEST_READY.md).

Maintain your progress in d:\Downloads\bases-TP1\datasets\.agents\teamwork\test_writer_e2e\progress.md.
Write your final handoff report to:
d:\Downloads\bases-TP1\datasets\.agents\teamwork\test_writer_e2e\handoff.md

When done, message the orchestrator (conversation ID 0fbe2801-fce8-49f2-8814-84768519d3f0).
