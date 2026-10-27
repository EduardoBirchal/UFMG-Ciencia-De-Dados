# Task Assignment: Challenger 1 for Milestone 3

## Working Directory
d:\Downloads\bases-TP1\datasets\.agents\teamwork\challenger_m3_1

## Objective
Empirically stress-test Milestone 3:
1. Selection operators empirical tests:
   - Tournament $k=2$: bias towards best individual verified over 10,000 trials.
   - Scaled roulette: non-zero probability for worst individual verified, linear shift correctly scales probabilities.
   - $\epsilon$-Lexicase: MAD formula match against manual calculation, cases with identical candidate error (MAD=0) filter down to only best candidates without error.
2. Depth limit enforcement: Run 5,000 crossovers and mutations with max_depth=7 and max_depth=3; assert 100% of offspring have depth $\le$ ceiling, and rejected offspring are identical clones of parent 1.
3. Elitism check: Run 10 runs of 20 generations of GPEngine on `synth1` with elitism=True, asserting best training RMSE is strictly monotonically non-increasing.
4. Provide a clear verdict: APPROVE or FAIL.

## Inputs
- `d:\Downloads\bases-TP1\datasets\.agents\teamwork\ORIGINAL_REQUEST.md`
- `d:\Downloads\bases-TP1\datasets\PROJECT.md`
- `d:\Downloads\bases-TP1\datasets\.agents\teamwork\worker_m3\handoff.md`

## Output
Write your report and verdict to:
`d:\Downloads\bases-TP1\datasets\.agents\teamwork\challenger_m3_1\handoff.md`

