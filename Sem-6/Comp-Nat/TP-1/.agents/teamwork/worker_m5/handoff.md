# Handoff Report: Milestone 5 (Experimental Automation & 30-Seed Execution)

**Agent**: `worker_m5`  
**Working Directory**: `d:\Downloads\bases-TP1\datasets\.agents\teamwork\worker_m5`  
**Date**: 2026-10-27T00:15:00Z  
**Target Recipient**: Project Orchestrator (`0fbe2801-fce8-49f2-8814-84768519d3f0`)

---

## 1. Observation

### 1.1 Experimental Execution Results
The master runner `scripts/run_experiments.py` was executed across 7 parallel CPU workers using fixed seeds `SEEDS = [42 + i for i in range(30)]` (seeds 42 to 71). All 540 genuine GP evolutionary runs completed successfully without failures:

1. **Step 1 (Population & Generations Tuning on `synth1`)**:
   - 9 configurations $\times$ 30 seeds = 270 runs.
   - Evaluated configurations: `pop50_gen50`, `pop50_gen100`, `pop50_gen500`, `pop100_gen50`, `pop100_gen100`, `pop100_gen500`, `pop500_gen50`, `pop500_gen100`, `pop500_gen500`.
   - Elapsed wall-clock time: 335.3 seconds.
   - Winner: $(P^*, G^*) = (500, 500)$ with mean test RMSE = $2.4171 \pm 2.2293$ (train RMSE = $1.4166 \pm 0.6404$, avg nodes = $40.33 \pm 10.92$, avg depth = $6.97 \pm 0.18$).

2. **Step 2 (Selection Comparison on `synth1`)**:
   - 3 configurations $\times$ 30 seeds = 90 runs using $(P^*, G^*) = (500, 500)$, $p_c=0.9, p_m=0.05$, elitism=True.
   - Evaluated selections: `tournament` ($k=2$), `roulette` (scaled), `lexicase` ($\epsilon$-lexicase).
   - Elapsed wall-clock time: 2071.9 seconds.
   - Results:
     - `lexicase`: Train RMSE = $0.2609 \pm 0.3722$, Test RMSE = $0.4927 \pm 0.9241$, Avg Nodes = $36.27 \pm 9.10$.
     - `tournament`: Train RMSE = $1.4166 \pm 0.6404$, Test RMSE = $2.4171 \pm 2.2293$, Avg Nodes = $40.33 \pm 10.92$.
     - `roulette`: Train RMSE = $4.0158 \pm 2.1606$, Test RMSE = $6.1381 \pm 4.1907$, Avg Nodes = $13.43 \pm 4.26$.
   - Winner: $S^* = \text{lexicase}$ with lowest mean test RMSE ($0.4927$).

3. **Step 3 (Variation Operator Comparison on `synth1`)**:
   - 2 configurations $\times$ 30 seeds = 60 runs using $(P^*, G^*, S^*)$ and elitism=True.
   - Evaluated variations: `pc90_pm05` ($p_c=0.90, p_m=0.05$) vs `pc60_pm30` ($p_c=0.60, p_m=0.30$).
   - Elapsed wall-clock time: 2802.9 seconds.
   - Results:
     - `pc90_pm05`: Train RMSE = $0.2609 \pm 0.3722$, Test RMSE = $0.4927 \pm 0.9241$, Avg Nodes = $36.27 \pm 9.10$.
     - `pc60_pm30`: Train RMSE = $0.4487 \pm 0.4988$, Test RMSE = $0.8934 \pm 1.3282$, Avg Nodes = $32.40 \pm 5.97$.
   - Winner: $V^* = \text{pc90\_pm05}$ ($p_c=0.90, p_m=0.05$).

4. **Step 4 (Elitism Analysis on `synth1`)**:
   - 2 configurations $\times$ 30 seeds = 60 runs using $(P^*, G^*, S^*, V^*)$.
   - Evaluated elitism: `with_elitism` (elitism=True) vs `without_elitism` (elitism=False).
   - Elapsed wall-clock time: 3217.5 seconds.
   - Results:
     - `with_elitism`: Train RMSE = $0.2609 \pm 0.3722$, Test RMSE = $0.4927 \pm 0.9241$, Avg Nodes = $36.27 \pm 9.10$.
     - `without_elitism`: Train RMSE = $0.4902 \pm 0.5729$, Test RMSE = $0.9206 \pm 1.2375$, Avg Nodes = $38.63 \pm 9.68$.
   - Winner: $E^* = \text{with\_elitism}$ (elitism=True).

5. **Step 5 (Final Validation on `synth2` and `concrete`)**:
   - 2 datasets $\times$ 30 seeds = 60 runs using overall best configuration $C^* = (P=500, G=500, S=\text{lexicase}, p_c=0.9, p_m=0.05, \text{elitism}=\text{True})$.
   - Elapsed wall-clock time: 7524.7 seconds.
   - Results:
     - `synth2` ($N_{\text{train}}=300, N_{\text{test}}=1000$): Train RMSE = $0.5726 \pm 0.5876$, Test RMSE = $0.6501 \pm 0.6953$, Avg Nodes = $35.23 \pm 7.81$, Avg Depth = $6.93 \pm 0.25$.
     - `concrete` ($N_{\text{train}}=824, N_{\text{test}}=206$): Train RMSE = $7.9342 \pm 0.3954$, Test RMSE = $8.0103 \pm 0.5213$, Avg Nodes = $42.50 \pm 6.99$, Avg Depth = $7.00 \pm 0.00$.

### 1.2 Generated Artifacts Inventory
- **CSVs**: Exactly 545 CSV files in `results/` (540 per-run CSVs across steps 1-5 + 5 step `summary.csv` files). 0 empty files.
  - `results/step1_pop_gen/`: 270 run CSVs + `summary.csv`
  - `results/step2_selection/`: 90 run CSVs + `summary.csv`
  - `results/step3_variation/`: 60 run CSVs + `summary.csv`
  - `results/step4_elitism/`: 60 run CSVs + `summary.csv`
  - `results/step5_validation/`: 60 run CSVs + `summary.csv`
- **Tables** (in `results/tables/`):
  - `step1_summary.md` and `step1_summary.tex`
  - `step2_summary.md` and `step2_summary.tex`
  - `step3_summary.md` and `step3_summary.tex`
  - `step4_summary.md` and `step4_summary.tex`
  - `step5_summary.md` and `step5_summary.tex`
  (Plus companion aliases `step1_pop_gen_summary.*`, `step2_selection_summary.*`, etc.).
- **Figures** (in `results/plots/`):
  - `fig1_step1_convergence.png` and `fig1_step1_convergence.svg`
  - `fig2_step1_bloat.png` and `fig2_step1_bloat.svg`
  - `fig3_step2_selection_convergence.png` and `fig3_step2_selection_convergence.svg`
  - `fig4_step2_diversity.png` and `fig4_step2_diversity.svg`
  - `fig5_step3_operator_success.png` and `fig5_step3_operator_success.svg`
  - `fig6_step4_elitism_comparison.png` and `fig6_step4_elitism_comparison.svg`
  - `fig7_step5_synth2_concrete_convergence.png` and `fig7_step5_synth2_concrete_convergence.svg`
  - `fig8_step5_generalization_scatter.png` and `fig8_step5_generalization_scatter.svg`

### 1.3 Verification Command Outputs
- `scripts/check_style.py`:
  ```
  all 27 files passed style checks
  ```
  (0 uppercase comments/docstrings, 0 trailing periods).
- `pytest`:
  ```
  === 164 passed in 2.18s ===
  ```

---

## 2. Logic Chain

1. **Protocol Compliance**:
   - `ORIGINAL_REQUEST.md` and `DISPATCH.md` required executing Steps 1 to 5 sequentially across 30 distinct seeds (`SEEDS = [42 + i for i in range(30)]`).
   - `scripts/run_experiments.py` organized the execution into sequential steps, where the empirical winner of Step 1 fed Step 2, Step 2 fed Step 3, Step 3 fed Step 4, and the complete optimal tuple $C^*$ was evaluated in Step 5.
2. **Empirical Decision Path**:
   - In Step 1, `pop500_gen500` achieved the lowest mean test RMSE ($2.4171$), beating all smaller budgets (e.g. `pop50_gen50` had $14.49$, `pop100_gen100` had $5.10$). Thus, $(P^*, G^*) = (500, 500)$ was selected.
   - In Step 2, $\epsilon$-Lexicase reduced test RMSE by nearly $80\%$ compared to Tournament ($0.4927$ vs $2.4171$) and outperformed Roulette ($6.1381$) while preserving high structural diversity. Thus, $S^* = \text{lexicase}$ was selected.
   - In Step 3, the canonical crossover-dominant regime ($p_c=0.90, p_m=0.05$) outperformed high-mutation ($0.4927$ vs $0.8934$). Thus, $V^* = \text{pc90\_pm05}$ was selected.
   - In Step 4, enabling elitism improved both train RMSE ($0.2609$ vs $0.4902$) and test RMSE ($0.4927$ vs $0.9206$), preventing destructive loss of optimal fitness during replacement. Thus, $E^* = \text{with\_elitism}$ was selected.
   - In Step 5, the optimal pipeline achieved a test RMSE of $0.6501$ on `synth2` (down from baseline $3.68$) and $8.0103$ on the 8-feature `concrete` benchmark (down from baseline $16.80$).
3. **Artifact Integrity**:
   - All plots were rendered through the pure Python vector SVG and bitmap PNG engine in `src/reporting/plotter.py` without external plotting dependencies (`matplotlib` is not installed).
   - All tables were formatted in GitHub Markdown and LaTeX `booktabs` with escaped characters and $\pm$ standard error columns.
   - Strict R2 code style (casual lowercase comments/docstrings without trailing periods) was maintained and verified by `scripts/check_style.py`.

---

## 3. Caveats

- No caveats. All 540 runs completed genuinely from their explicit random seeds; all metrics and files were written to disk and verified.

---

## 4. Conclusion

Milestone 5 is completely accomplished:
- `scripts/run_experiments.py` is implemented, fully parallelized, and passes all R2 style checks.
- All 540 experimental runs across the 30 fixed seeds are completed and stored under `results/`.
- All 8 required figures are saved in both PNG and SVG formats under `results/plots/`.
- All 5 summary tables are generated in both Markdown and LaTeX `booktabs` under `results/tables/`.
- `PROJECT.md` Milestone 5 status has been updated to `DONE`.

---

## 5. Verification Method

To independently verify the outputs of Milestone 5, execute the following commands:

1. **Verify Style Checker**:
   ```powershell
   & "d:\Downloads\bases-TP1\datasets\.venv\Scripts\python.exe" scripts/check_style.py
   ```
   *Expected output*: `all 27 files passed style checks`

2. **Verify Full Test Suite**:
   ```powershell
   & "d:\Downloads\bases-TP1\datasets\.venv\Scripts\python.exe" -m pytest
   ```
   *Expected output*: `164 passed`

3. **Verify Generated Files**:
   ```powershell
   & "d:\Downloads\bases-TP1\datasets\.venv\Scripts\python.exe" -c "import os; csvs = [os.path.join(r, f) for r, _, fs in os.walk('results') for f in fs if f.endswith('.csv')]; print('total csvs:', len(csvs)); print('empty csvs:', len([p for p in csvs if os.path.getsize(p) == 0])); plots = os.listdir('results/plots'); print('plots count:', len(plots)); tables = os.listdir('results/tables'); print('tables count:', len(tables))"
   ```
   *Expected output*: `total csvs: 545`, `empty csvs: 0`, `plots count: 16` (8 png + 8 svg), `tables count: 20` (10 md + 10 tex).
