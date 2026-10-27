# Task Assignment: Explorer Survey GP Spec

## Working Directory
d:\Downloads\bases-TP1\datasets\.agents\teamwork\explorer_survey_gp_spec

## Objective
Thoroughly analyze the Core GP Framework specification from `d:\Downloads\bases-TP1\datasets\.agents\teamwork\ORIGINAL_REQUEST.md` (and any local PDF or text documentation in the workspace if present):
1. Representation: Node types (primitive, terminal ERC, terminal variable X_0..X_{D-1}), tree structure, depth calculation, ramped half-and-half generation (grow and full, depths 2 to 5), max depth 7 enforcement (keeping parent if operation exceeds depth 7).
2. Primitives: Arithmetic (+, -, *, protected division with denominator near zero handling), non-linear (sin, cos, protected log, protected exp). Detail exact protected definitions (e.g. log(|x|) or protected against x<=0, exp clamped to avoid overflow/inf, division x/y with y=0 or abs(y)<1e-6 returning 1.0 or similar standard GP conventions).
3. Continuous ERCs: Range, initialization, Gaussian perturbation mutation (mean 0, std, etc.), terminal variables mapping.
4. Vectorized Evaluation: NumPy 2D array (N x D) evaluation, recursive or post-order evaluation, avoiding NaN/Inf, handling constants.
5. Fitness: RMSE calculation.
6. Selection Operators:
   - Tournament selection (k=2)
   - Scaled Roulette Wheel selection (linear shift f_max - f_i + delta to handle minimization)
   - Epsilon-Lexicase selection with dynamic Median Absolute Deviation (MAD) threshold per candidate set (check Equations 1 & 2 from specification/PDF if present, vectorized across prediction matrices)
7. Reproductive scheme: Mutually exclusive operator selection per offspring slot (p_c crossover, p_m mutation, remainder direct reproduction/cloning).
8. Elitism: Best individual preservation.
9. Statistics per generation: Best, worst, mean train RMSE; test RMSE of best individual; syntactically repeated canonical tree strings; crossover offspring better/worse than average parent; bloat (average node count, average depth).

## Inputs
- `d:\Downloads\bases-TP1\datasets\.agents\teamwork\ORIGINAL_REQUEST.md`

## Output
Write your findings to `d:\Downloads\bases-TP1\datasets\.agents\teamwork\explorer_survey_gp_spec\handoff.md`.
Include:
- Observation (detailed breakdown of mathematical formulations, equations, operators)
- Logic Chain (exact modular architecture recommendations: Node, Tree, Population, Operators, Engine)
- Caveats (numerical stability edge cases, epsilon-lexicase MAD ties/empty cases, depth calculation edge cases)
- Conclusion (exact specification to guide implementation and tests)


## 2026-10-26T17:50:42Z
[Message] sender=0fbe2801-fce8-49f2-8814-84768519d3f0 priority=MESSAGE_PRIORITY_HIGH
You are an Explorer subagent for the Project Orchestrator.
Your working directory is: d:\Downloads\bases-TP1\datasets\.agents\teamwork\explorer_survey_gp_spec

MANDATORY: You MUST read ORIGINAL_REQUEST.md at:
d:\Downloads\bases-TP1\datasets\.agents\teamwork\ORIGINAL_REQUEST.md
before starting work. Do not assume requirements.

Read your task instructions at:
d:\Downloads\bases-TP1\datasets\.agents\teamwork\explorer_survey_gp_spec\DISPATCH.md

Investigate and document the mathematical and technical specifications for the Core GP framework:
1. Tree representation, node hierarchy, max depth 7 limit enforcement (keeping parent if depth exceeded), Ramped Half-and-Half (depths 2-5).
2. Primitives: +, -, *, protected /, sin, cos, protected log, protected exp. Define robust numerical protections (avoiding NaN/Inf/overflow).
3. Continuous ERCs with Gaussian perturbation mutation, and terminal feature mapping X_0..X_{D-1}.
4. Vectorized evaluation over NumPy 2D feature arrays returning 1D prediction arrays.
5. Fitness: RMSE.
6. Selection methods: Tournament (k=2), Scaled Roulette wheel (linear shift f_max - f_i + delta), and epsilon-Lexicase selection with dynamic Median Absolute Deviation (MAD) threshold per candidate set (check formulas, Equations 1 & 2 in PDF/spec, vectorized across prediction matrices).
7. Reproductive scheme: mutually exclusive operator selection (p_c crossover, p_m mutation, cloning remainder).
8. Elitism and per-generation statistics collection (best/worst/mean RMSE, test RMSE of best, duplicate canonical strings, crossover better/worse than parents, bloat metrics).

Maintain your progress in d:\Downloads\bases-TP1\datasets\.agents\teamwork\explorer_survey_gp_spec\progress.md.
Write your final report to:
d:\Downloads\bases-TP1\datasets\.agents\teamwork\explorer_survey_gp_spec\handoff.md

When done, message the orchestrator (conversation ID 0fbe2801-fce8-49f2-8814-84768519d3f0) with your summary and handoff path.
