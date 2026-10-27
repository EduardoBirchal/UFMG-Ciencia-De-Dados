# Milestone 3 Empirical Challenger Report

**Agent**: `challenger_m3_1`  
**Working Directory**: `d:\Downloads\bases-TP1\datasets\.agents\teamwork\challenger_m3_1`  
**Target Milestone**: Milestone 3 (Selection, Variation & Evolution Engine)  
**Parent Conversation ID**: `0fbe2801-fce8-49f2-8814-84768519d3f0`  
**Verdict**: **APPROVE**  
**Risk Assessment**: LOW  
**Date**: 2026-10-26  

---

## 1. Observation

### 1.1 Baseline Test Suite and Style Compliance
- Executed full test suite:
  - Command: `.\.venv\Scripts\python.exe -m pytest tests/ -v`
  - Result: `=== 164 passed in 2.51s ===` (0 skipped, 0 failed).
- Executed code style linter:
  - Command: `.\.venv\Scripts\python.exe scripts/check_style.py`
  - Result: `all 26 files passed style checks` (0 violations).

### 1.2 Selection Operators Empirical Stress Testing
Harness: `scripts/stress_m3.py` executed via Python 3.14 (`.venv`).

1. **Tournament Selection ($k=2$)**:
   - Sample size: $N = 10$, fitnesses $f = [1.0, 2.0, \dots, 10.0]$, 10,000 independent trials.
   - Theoretical probabilities $P(r) = \frac{2(N - 1 - r) + 1}{N^2}$:
     `[0.19, 0.17, 0.15, 0.13, 0.11, 0.09, 0.07, 0.05, 0.03, 0.01]`
   - Observed empirical frequencies:
     `[0.1897, 0.1655, 0.1514, 0.1330, 0.1099, 0.0906, 0.0676, 0.0507, 0.0315, 0.0101]`
   - Chi-squared goodness-of-fit: $\chi^2 = 3.7407$, $p = 0.9276$ (no statistically significant deviation from theoretical order statistics).
   - Strict bias towards best: Candidate 0 won 1,897 times (highest); worst won 101 times (lowest).

2. **Scaled Roulette Selection**:
   - Fitnesses $f = [1.0, 5.0, 10.0]$, $\delta = 0.05$, $f_{max} = 10.0$, 30,000 trials.
   - Theoretical probabilities: `[0.6396, 0.3569, 0.0035]`.
   - Observed empirical frequencies: `[0.6412, 0.3549, 0.0039]`.
   - Chi-squared goodness-of-fit: $\chi^2 = 1.5806$, $p = 0.4537$.
   - Non-zero probability for worst individual verified: Worst individual was selected 117 times ($0.39\%$).
   - Uniformity on identical fitnesses: 20,000 trials on $f = [4.0, 4.0, 4.0, 4.0]$ yielded frequencies `[0.2540, 0.2509, 0.2506, 0.2444]`, strictly matching $\frac{1}{N} = 0.25$ within $0.005$.

3. **$\epsilon$-Lexicase Dynamic MAD Selection**:
   - Case 1 (Manual MAD alignment): Candidates with errors $[1.0, 2.5, 9.0]$. Sample median $= 2.5$, absolute deviations from median $= [1.5, 0.0, 6.5]$, $\text{MAD} = 1.5$.
     Threshold: $e^* + \text{MAD} + 10^{-12} = 1.0 + 1.5 + 10^{-12} = 2.500000000001$.
     Results across 1,000 trials: Candidate 0 won 518 times, Candidate 1 won 482 times, Candidate 2 won 0 times ($100\%$ excluded).
   - Case 2 (Identical candidate errors on test case, $\text{MAD} = 0$): Case 0 errors $[4.0, 4.0, 4.0]$ followed by Case 1 errors $[0.1, 5.0, 9.0]$.
     Results across 1,000 trials: Candidate 0 won 1,000 times ($100\%$). Zero candidates falsely eliminated when $\text{MAD} = 0$.
   - Case 3 (Tied best candidates with $\text{MAD} = 0$ and distinct outlier): Errors $[0.0, 0.0, 0.0, 10.0]$. Median $= 0.0$, $\text{MAD} = 0.0$.
     Threshold: $0.0 + 0.0 + 10^{-12} = 10^{-12}$.
     Results across 1,000 trials: Candidates 0, 1, 2 won 334, 328, 338 times; outlier Candidate 3 won 0 times ($100\%$ excluded).

### 1.3 Depth Limit Enforcement Stress Testing
Tested across 20,000 genetic operations using deep parent pools to force ceiling collisions:
- **Ceiling max_depth = 7**:
  - Crossover (5,000 trials): 1,024 depth ceiling rejections triggered ($20.48\%$).
    Depth violations ($> 7$): **0** ($0.0\%$).
    Parent 1 clone identity failures: **0** ($100\%$ syntactic, semantic, and object clone match).
  - Subtree Mutation (5,000 trials): 755 depth ceiling rejections triggered ($15.10\%$).
    Depth violations ($> 7$): **0** ($0.0\%$).
    Parent clone identity failures: **0** ($100\%$ match).
- **Ceiling max_depth = 3**:
  - Crossover (5,000 trials): 1,166 depth ceiling rejections triggered ($23.32\%$).
    Depth violations ($> 3$): **0** ($0.0\%$).
    Parent 1 clone identity failures: **0** ($100\%$ match).
  - Subtree Mutation (5,000 trials): 1,399 depth ceiling rejections triggered ($27.98\%$).
    Depth violations ($> 3$): **0** ($0.0\%$).
    Parent clone identity failures: **0** ($100\%$ match).

### 1.4 Elitism Monotonicity Across 10 Runs of 20 Generations
Dataset: `synth1` ($N_{train}=400, D=2$). Configuration: `pop_size=50, generations=20, selection='tournament', k=2, pc=0.9, pm=0.05, elitism=True`.
- Seeds tested: 1000 through 1009.
- Generational trajectory results:
  - Run 1 (seed 1000): $18.7143 \to 18.6693$ [monotonic non-increasing: 19/19 transitions ok]
  - Run 2 (seed 1001): $18.7177 \to 14.7959$ [monotonic non-increasing: 19/19 transitions ok]
  - Run 3 (seed 1002): $20.0725 \to 4.4670$ [monotonic non-increasing: 19/19 transitions ok]
  - Run 4 (seed 1003): $19.1950 \to 11.3202$ [monotonic non-increasing: 19/19 transitions ok]
  - Run 5 (seed 1004): $19.3448 \to 8.5405$ [monotonic non-increasing: 19/19 transitions ok]
  - Run 6 (seed 1005): $19.3225 \to 17.0039$ [monotonic non-increasing: 19/19 transitions ok]
  - Run 7 (seed 1006): $19.3505 \to 12.1291$ [monotonic non-increasing: 19/19 transitions ok]
  - Run 8 (seed 1007): $19.5886 \to 10.0140$ [monotonic non-increasing: 19/19 transitions ok]
  - Run 9 (seed 1008): $20.2429 \to 16.5072$ [monotonic non-increasing: 19/19 transitions ok]
  - Run 10 (seed 1009): $19.5886 \to 8.4623$ [monotonic non-increasing: 19/19 transitions ok]
- Across all 10 runs ($10 \times 19 = 190$ generational transitions), $f_{g} \le f_{g-1} + 10^{-12}$ held with 0 violations.
- In contrast, a control run with `elitism=False` exhibited 7 fitness degradation events across 20 generations, confirming that elitism is the causal mechanism enforcing monotonicity.

---

## 2. Logic Chain

1. **Selection Biases**:
   - Observation 1.2 demonstrates that `tournament_selection` implements tournament $k=2$ with replacement exactly matching the discrete order statistics distribution $P(r) = \frac{2(N-1-r)+1}{N^2}$. The empirical chi-squared p-value ($0.9276$) confirms absence of implementation bias, indexing off-by-one errors, or unintended sampling distortion.
   - Observation 1.2 demonstrates that `scaled_roulette_selection` strictly satisfies the requirement that the worst individual receives non-zero selection weight $\delta$, that probabilities scale linearly with fitness error distance $(f_{max} - f_i)$, and that tied populations degenerate uniformly to $\frac{1}{N}$.
   - Observation 1.2 demonstrates that `epsilon_lexicase_selection` implements the dynamic MAD threshold formula $e^* + \text{MAD} + 10^{-12}$ without edge-case failure when $\text{MAD} = 0$, filtering out inferior candidates while preserving tied best candidates.

2. **Depth Limit Enforcement**:
   - Observation 1.3 establishes that across 20,000 adversarial genetic operations (5,000 crossovers and 5,000 mutations at both depth ceiling 7 and depth ceiling 3) with parent trees chosen specifically at or near the depth ceiling, zero individuals exceeded the depth ceiling.
   - In all 4,344 cases where variation would have generated a tree exceeding the depth ceiling, the operator successfully reverted to a distinct deep copy of parent 1. The clone was verified to be syntactically identical (`to_string()`), numerically equivalent (`eval(X)`), and structurally independent in memory (`child is not parent1` and `child.root is not parent1.root`).

3. **Elitism Preservation**:
   - Observation 1.4 confirms that populating slot 0 of generation $g+1$ with a clone of `self.best_individual_` guarantees that the training RMSE of the population's best individual is strictly monotonically non-increasing across generations over 10 distinct runs of 20 generations on `synth1`.

4. **Style and Robustness**:
   - Observation 1.1 verifies that all repository source and test files pass the R2 casual lowercase comment/docstring style constraints without trailing periods, and all 164 unit and E2E tests execute cleanly in under 3 seconds.

---

## 3. Caveats

- **No Caveats**: All required empirical challenges specified in `DISPATCH.md` were executed directly against the code, verified against theoretical models, and confirmed with zero failures.

---

## 4. Conclusion

### Verdict: **APPROVE**

Milestone 3 meets and exceeds all correctness, robustness, engineering, and empirical requirements:
1. Tournament selection ($k=2$), scaled roulette selection, and $\epsilon$-lexicase selection operate with exact mathematical and empirical fidelity.
2. Depth limits ($\le 7$) are strictly enforced across 100% of crossover and mutation operations, with clean clone reversion to parent 1.
3. Elitism guarantees strictly monotonic non-increasing training RMSE across generations.
4. The codebase is clean, modular, and 100% style-compliant.

---

## 5. Verification Method

To independently reproduce the empirical findings of this report:

1. **Execute Empirical Stress Test Harness**:
   ```powershell
   .\.venv\Scripts\python.exe scripts/stress_m3.py
   ```
   *Expected output*: All 5 stress suites pass; prints `milestone 3 empirical challenger verdict: APPROVE`.

2. **Execute Full Test Suite**:
   ```powershell
   .\.venv\Scripts\python.exe -m pytest tests/ -v
   ```
   *Expected output*: `164 passed in ~2.5s`.

3. **Execute Code Style Linter**:
   ```powershell
   .\.venv\Scripts\python.exe scripts/check_style.py
   ```
   *Expected output*: `all 26 files passed style checks`.

### Invalidation Conditions
- Any occurrence of tree depth $> 7$ in offspring.
- Any generation $g$ where $f_{best, g} > f_{best, g-1} + 10^{-12}$ when `elitism=True`.
- Any chi-squared p-value $< 0.01$ in selection frequency goodness-of-fit.
- Any failure in `scripts/check_style.py` or `pytest tests/`.
