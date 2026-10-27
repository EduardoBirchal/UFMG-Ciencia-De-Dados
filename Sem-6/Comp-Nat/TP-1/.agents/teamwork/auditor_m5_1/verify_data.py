"""independent forensic verification script for milestone 5 artifacts"""

import os
import hashlib
import numpy as np
import pandas as pd

base_dir = r"d:\Downloads\bases-TP1\datasets"
results_dir = os.path.join(base_dir, "results")
tables_dir = os.path.join(results_dir, "tables")

step_dirs = [
    "step1_pop_gen",
    "step2_selection",
    "step3_variation",
    "step4_elitism",
    "step5_validation",
]

print("=== 1. Cross-checking summary.csv rows against individual run CSVs ===")
total_checked = 0
mismatches = []

for sd in step_dirs:
    summary_path = os.path.join(results_dir, sd, "summary.csv")
    sum_df = pd.read_csv(summary_path)
    for idx, row in sum_df.iterrows():
        total_checked += 1
        cfg = row["config_name"]
        seed = int(row["seed"])
        run_csv_path = os.path.join(results_dir, sd, f"{cfg}_seed{seed}.csv")
        if not os.path.exists(run_csv_path):
            mismatches.append(f"Missing run CSV: {run_csv_path}")
            continue
        run_df = pd.read_csv(run_csv_path)
        last_row = run_df.iloc[-1]

        r_tr = float(row["best_train_rmse"])
        l_tr = float(last_row["best_train_rmse"])
        if not np.isclose(r_tr, l_tr, atol=1e-5):
            mismatches.append(f"{run_csv_path}: train rmse mismatch {r_tr} vs {l_tr}")

        r_te = float(row["final_test_rmse"])
        l_te = float(last_row["best_test_rmse"])
        if not np.isclose(r_te, l_te, atol=1e-5):
            mismatches.append(f"{run_csv_path}: test rmse mismatch {r_te} vs {l_te}")

        r_nd = float(row["avg_node_count"])
        l_nd = float(last_row["avg_node_count"])
        if not np.isclose(r_nd, l_nd, atol=1e-5):
            mismatches.append(f"{run_csv_path}: avg nodes mismatch {r_nd} vs {l_nd}")

print(f"Summary entries verified: {total_checked}. Mismatches: {len(mismatches)}")
assert len(mismatches) == 0, f"Found mismatches: {mismatches[:5]}"

print("\n=== 2. Cross-checking results/tables against summary.csv data ===")
# Check step1 to step5 summary tables in markdown and tex
table_errors = []
for sd in step_dirs:
    summary_path = os.path.join(results_dir, sd, "summary.csv")
    sum_df = pd.read_csv(summary_path)

    # Calculate actual stats from summary.csv
    group_col = "config_name"
    for cfg, gdf in sum_df.groupby(group_col):
        mean_tr = gdf["best_train_rmse"].mean()
        std_tr = gdf["best_train_rmse"].std()
        mean_te = gdf["final_test_rmse"].mean()
        std_te = gdf["final_test_rmse"].std()

        # Check that this configuration appears in the corresponding markdown table
        # with matching numbers
        step_prefix = sd.split("_")[0] # e.g. step1
        md_file = os.path.join(tables_dir, f"{step_prefix}_summary.md")
        if not os.path.exists(md_file):
            table_errors.append(f"Missing table: {md_file}")
            continue

        with open(md_file, "r", encoding="utf-8") as f:
            content = f.read()

        # format formatted string e.g. f"{mean_te:.4f} ± {std_te:.4f}"
        tr_str = f"{mean_tr:.4f} ± {std_tr:.4f}"
        te_str = f"{mean_te:.4f} ± {std_te:.4f}"
        if tr_str not in content:
            table_errors.append(f"{md_file}: expected train str '{tr_str}' not found for cfg '{cfg}'")
        if te_str not in content:
            table_errors.append(f"{md_file}: expected test str '{te_str}' not found for cfg '{cfg}'")

print(f"Table verification errors: {len(table_errors)}")
assert len(table_errors) == 0, f"Found table errors: {table_errors[:5]}"

print("\n=== 3. Checking plots existence and sizes ===")
plots_dir = os.path.join(results_dir, "plots")
expected_plots = [
    "fig1_step1_convergence",
    "fig2_step1_bloat",
    "fig3_step2_selection_convergence",
    "fig4_step2_diversity",
    "fig5_step3_operator_success",
    "fig6_step4_elitism_comparison",
    "fig7_step5_synth2_concrete_convergence",
    "fig8_step5_generalization_scatter",
]

plot_errors = []
for p in expected_plots:
    png_path = os.path.join(plots_dir, f"{p}.png")
    svg_path = os.path.join(plots_dir, f"{p}.svg")
    if not os.path.exists(png_path) or os.path.getsize(png_path) < 100:
        plot_errors.append(f"Missing or tiny PNG: {png_path}")
    if not os.path.exists(svg_path) or os.path.getsize(svg_path) < 100:
        plot_errors.append(f"Missing or tiny SVG: {svg_path}")

print(f"Plots verified: {len(expected_plots)*2} files. Errors: {len(plot_errors)}")
assert len(plot_errors) == 0, f"Found plot errors: {plot_errors}"

print("\nAll artifact consistency checks PASSED perfectly!")
