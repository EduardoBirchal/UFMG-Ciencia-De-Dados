"""independent reproducibility smoke test for seed 42"""

import os
import sys
import numpy as np
import pandas as pd

base_dir = r"d:\Downloads\bases-TP1\datasets"
if base_dir not in sys.path:
    sys.path.insert(0, base_dir)

from src.data import load_dataset
from src.gp.engine import GPEngine

x_tr, y_tr, x_te, y_te = load_dataset("synth1", base_dir=base_dir)

# Run with seed=42
engine = GPEngine(
    pop_size=50,
    generations=10,
    selection="tournament",
    tournament_k=2,
    pc=0.90,
    pm=0.05,
    elitism=True,
    max_depth=7,
    seed=42,
)

res = engine.fit(x_tr, y_tr, x_te, y_te)
sim_df = pd.DataFrame(res["history"])

# Load the saved CSV from results/step1_pop_gen/pop50_gen50_seed42.csv
recorded_csv = os.path.join(base_dir, "results", "step1_pop_gen", "pop50_gen50_seed42.csv")
rec_df = pd.read_csv(recorded_csv).iloc[:10]

print("Comparing first 10 generations of seed 42 (simulated vs recorded):")
cols_to_compare = ["train_rmse_best", "train_rmse_worst", "train_rmse_mean", "test_rmse_best", "duplicate_trees", "avg_nodes"]

all_match = True
for col in cols_to_compare:
    sim_vals = sim_df[col].values
    rec_vals = rec_df[col].values
    diff = np.max(np.abs(sim_vals - rec_vals))
    print(f"  Col '{col}' max diff: {diff:.6e}")
    if diff > 1e-6:
        all_match = False

if all_match:
    print("PERFECT MATCH: The recorded CSV is bit-exact identical to genuine GPEngine execution!")
else:
    print("MISMATCH DETECTED!")
