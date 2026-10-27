# BRIEFING — 2026-10-26T17:59:00Z

## Mission
Investigate and document the mathematical and technical specifications for the Core GP framework for symbolic regression.

## 🔒 My Identity
- Archetype: explorer
- Roles: investigation, synthesis
- Working directory: d:\Downloads\bases-TP1\datasets\.agents\teamwork\explorer_survey_gp_spec
- Original parent: 0fbe2801-fce8-49f2-8814-84768519d3f0
- Milestone: survey_gp_spec

## 🔒 Key Constraints
- Read-only investigation — do NOT implement code outside .agents/teamwork
- Focus on mathematical and technical specifications of Core GP framework (R1)
- Strict compliance with prompt and original request requirements

## Current Parent
- Conversation ID: 0fbe2801-fce8-49f2-8814-84768519d3f0
- Updated: not yet

## Investigation State
- **Explored paths**:
  - `d:\Downloads\bases-TP1\datasets\.agents\teamwork\ORIGINAL_REQUEST.md`
  - `d:\Downloads\bases-TP1\datasets\.agents\teamwork\explorer_survey_gp_spec\DISPATCH.md`
  - Dataset paths: `synth1/`, `synth2/`, `concrete/`
  - Local Python venv: verified NumPy 2.5.4 execution
- **Key findings**:
  - Validated vectorized tree evaluation: achieves ~55,000 tree evals/sec on $N=824$, confirming generation evaluations take $<2$ ms.
  - Closed protected arithmetic definitions: division with denominator fallback, log with $|x| < 10^{-6}$ fallback, exp clipped to $[-32, 32]$, arithmetic operations clipped to $[-10^{50}, 10^{50}]$. Zero NaNs or Infs generated across 1,000 random depth-7 trees.
  - Formulated and verified $\epsilon$-Lexicase selection with dynamic MAD threshold (Equations 1 & 2): 100 selections take $0.03$s; 500 selections on $N=824$ take $0.16$s using precomputed error matrix.
  - Formulated Scaled Roulette wheel with linear shift ($f_{max} - f_i + \delta$), resolving minimization scale distortion and tied population edge cases.
  - Formulated Tournament selection ($k=2$).
  - Formulated Ramped Half-and-Half (depths 2-5, Full and Grow 50/50) and verified exact depth distributions.
  - Formulated subtree crossover, subtree mutation, Gaussian constant perturbation mutation, and strict max depth 7 enforcement (keeping parent if depth exceeded).
  - Defined comprehensive metrics schema including canonical tree hashing, crossover parent comparison, and bloat tracking.
- **Unexplored areas**:
  - None within Core GP specification scope.

## Key Decisions Made
- Recommending immutable/functional subtree replacement for crossover/mutation to guarantee parent preservation on depth violations.
- Precomputed $(M \times N)$ error matrix for $\epsilon$-lexicase to achieve blazing performance without recomputing predictions per test case.
- Output predictions clipped to $[-10^{10}, 10^{10}]$ at tree root to prevent $(y_{pred} - y)^2$ float64 overflow.

## Artifact Index
- `d:\Downloads\bases-TP1\datasets\.agents\teamwork\explorer_survey_gp_spec\progress.md` — liveness heartbeat and progress tracking
- `d:\Downloads\bases-TP1\datasets\.agents\teamwork\explorer_survey_gp_spec\handoff.md` — complete specification handoff report
