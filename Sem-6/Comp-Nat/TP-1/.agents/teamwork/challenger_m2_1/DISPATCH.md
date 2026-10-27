# Task Assignment: Challenger 1 for Milestone 2

## Working Directory
d:\Downloads\bases-TP1\datasets\.agents\teamwork\challenger_m2_1

## Objective
Empirically stress-test Milestone 2:
1. Numerical stability: Generate and evaluate 10,000 random syntax trees across edge-case data (zeros, large values, negative numbers, extreme scale differences). Verify strictly 0 NaNs and 0 Infs.
2. Vectorized evaluation: Verify that tree evaluation over $N \times D$ returns 1D array of shape $(N,)$ float64, with single constant trees properly broadcast to length $N$.
3. Subtree replacement: Test `replace_subtree` on deep trees, ensuring parent trees remain strictly unmodified.
4. Synth1 ground truth: Manually construct $X_0^4 - X_0^3 + 0.5 X_1^2 - X_1$ using `Node` and evaluate on `synth1-train.csv` and `synth1-test.csv`. Verify RMSE $< 1e-12$.
5. Ramped half-and-half: Generate 1,000 individuals and verify depth distribution strictly in [2, 5].
6. Provide a clear verdict: APPROVE or FAIL.

## Inputs
- `d:\Downloads\bases-TP1\datasets\.agents\teamwork\ORIGINAL_REQUEST.md`
- `d:\Downloads\bases-TP1\datasets\PROJECT.md`
- `d:\Downloads\bases-TP1\datasets\.agents\teamwork\worker_m2\handoff.md`

## Output
Write your report and verdict to:
`d:\Downloads\bases-TP1\datasets\.agents\teamwork\challenger_m2_1\handoff.md`

## 2026-10-26T18:27:36Z

You are Challenger 1 for Milestone 2.
Your working directory is: d:\Downloads\bases-TP1\datasets\.agents\teamwork\challenger_m2_1

MANDATORY: Read ORIGINAL_REQUEST.md at:
d:\Downloads\bases-TP1\datasets\.agents\teamwork\ORIGINAL_REQUEST.md
and PROJECT.md at:
d:\Downloads\bases-TP1\datasets\PROJECT.md
before starting work.

Read your task instructions at:
d:\Downloads\bases-TP1\datasets\.agents\teamwork\challenger_m2_1\DISPATCH.md

Empirically challenge numerical stability (10,000 random trees evaluated with 0 NaNs and 0 Infs), vectorized evaluation shapes, subtree replacement immutability, synth1 ground truth polynomial (RMSE < 1e-12), and ramped half-and-half depth bounds.
Provide a clear verdict: APPROVE or FAIL in handoff.md.

Write your final report to:
d:\Downloads\bases-TP1\datasets\.agents\teamwork\challenger_m2_1\handoff.md

When done, message the orchestrator (conversation ID 0fbe2801-fce8-49f2-8814-84768519d3f0).
