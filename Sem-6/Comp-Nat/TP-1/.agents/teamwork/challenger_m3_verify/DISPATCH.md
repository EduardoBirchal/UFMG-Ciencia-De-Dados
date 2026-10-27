# Task Assignment: Challenger for Milestone 3 Optimization Verification

## Working Directory
d:\Downloads\bases-TP1\datasets\.agents\teamwork\challenger_m3_verify

## Objective
Independently verify that the performance optimizations applied in Milestone 3 pass all 3 target thresholds and maintain 100% test correctness:
1. Run the exact speed benchmark harness from `challenger_m3_2/handoff.md`:
   - Target 1: `synth1` 100 gen, pop=100 (Tournament $k=2$) must run in **$< 1.0$s**.
   - Target 2: `concrete` 100 gen, pop=100 (Tournament $k=2$) must run in **$< 3.0$s**.
   - Target 3: `concrete` 50 gen, pop=100 ($\epsilon$-Lexicase) must run in **$< 15.0$s**.
2. Run pytest suite (`.venv\Scripts\python.exe -m pytest tests/ -v`). Confirm all 164 tests pass.
3. Run style linter (`.venv\Scripts\python.exe scripts/check_style.py`). Confirm 0 violations.
4. Run memory profiling across 5 consecutive runs. Confirm zero cumulative leaks.
5. Provide a clear verdict: APPROVE or FAIL.

## Inputs
- `d:\Downloads\bases-TP1\datasets\.agents\teamwork\ORIGINAL_REQUEST.md`
- `d:\Downloads\bases-TP1\datasets\PROJECT.md`
- `d:\Downloads\bases-TP1\datasets\.agents\teamwork\worker_m3_opt\handoff.md`
- `d:\Downloads\bases-TP1\datasets\.agents\teamwork\challenger_m3_2\handoff.md`

## Output
Write your report and verdict to:
`d:\Downloads\bases-TP1\datasets\.agents\teamwork\challenger_m3_verify\handoff.md`


## 2026-10-26T19:28:37Z
[Message] sender=0fbe2801-fce8-49f2-8814-84768519d3f0
You are a Challenger subagent verifying Milestone 3 performance optimizations.
Your working directory is: d:\Downloads\bases-TP1\datasets\.agents\teamwork\challenger_m3_verify

MANDATORY: Read ORIGINAL_REQUEST.md at:
d:\Downloads\bases-TP1\datasets\.agents\teamwork\ORIGINAL_REQUEST.md
and PROJECT.md at:
d:\Downloads\bases-TP1\datasets\PROJECT.md
before starting work.

Read your task instructions at:
d:\Downloads\bases-TP1\datasets\.agents\teamwork\challenger_m3_verify\DISPATCH.md
and the previous benchmark requirements at:
d:\Downloads\bases-TP1\datasets\.agents\teamwork\challenger_m3_2\handoff.md

Run the 3 speed benchmarks (synth1 100 gen < 1.0s, concrete 100 gen < 3.0s, concrete lexicase 50 gen < 15.0s), run full pytest suite (164 tests), and check_style.py.
Provide a clear verdict: APPROVE or FAIL in handoff.md.

Write your final report to:
d:\Downloads\bases-TP1\datasets\.agents\teamwork\challenger_m3_verify\handoff.md

When done, message the orchestrator (conversation ID 0fbe2801-fce8-49f2-8814-84768519d3f0).
