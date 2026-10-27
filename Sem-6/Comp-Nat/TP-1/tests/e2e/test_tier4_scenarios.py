# tier 4 tests for real world application scenarios on benchmark datasets
import os
import sys
import numpy as np
import pytest

# ensure project root is on sys path
repo_root = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
if repo_root not in sys.path:
    sys.path.insert(0, repo_root)

from src.data import load_dataset

# optional imports for features implemented in later milestones
try:
    from src.gp.node import Node
except ImportError:
    Node = None

try:
    from src.gp.engine import GPEngine
except ImportError:
    GPEngine = None


# scenario 1: synth1 symbolic regression end to end run

def test_scenario_synth1_regression_run():
    """test full genetic programming regression run on synth1 benchmark"""
    if GPEngine is None:
        pytest.skip("engine not implemented yet")
    x_tr, y_tr, x_te, y_te = load_dataset("synth1", base_dir=repo_root)
    engine = GPEngine(
        pop_size=30,
        generations=5,
        selection="tournament",
        pc=0.90,
        pm=0.05,
        elitism=True,
        seed=42,
    )
    res = engine.fit(x_tr, y_tr, x_te, y_te)
    assert "best" in res
    assert "history" in res
    history = res["history"]
    assert len(history) == 5
    # check that best train rmse is tracked and finite
    final_entry = history[-1]
    assert final_entry["train_rmse_best"] >= 0.0
    assert not np.isnan(final_entry["train_rmse_best"])
    assert final_entry["test_rmse_best"] >= 0.0


# scenario 2: synth1 analytical ground truth formula verification

def test_scenario_synth1_ground_truth_tree():
    """test manual syntax tree representing the ground truth polynomial on synth1"""
    if Node is None:
        pytest.skip("node not implemented yet")
    x_tr, y_tr, x_te, y_te = load_dataset("synth1", base_dir=repo_root)

    # ground truth formula: y = x0^4 - x0^3 + 0.5 * x1^2 - x1
    # build x0^2
    x0_a = Node("var", value=0)
    x0_b = Node("var", value=0)
    x0_sq = Node("*", children=[x0_a, x0_b])

    # build x0^4 = x0^2 * x0^2
    x0_sq_clone = Node("*", children=[Node("var", value=0), Node("var", value=0)])
    x0_pow4 = Node("*", children=[x0_sq, x0_sq_clone])

    # build x0^3 = x0 * x0^2
    x0_c = Node("var", value=0)
    x0_pow3 = Node("*", children=[x0_c, Node("*", children=[Node("var", value=0), Node("var", value=0)])])

    # build term1 = x0^4 - x0^3
    term1 = Node("-", children=[x0_pow4, x0_pow3])

    # build 0.5 * x1^2
    x1_a = Node("var", value=1)
    x1_b = Node("var", value=1)
    x1_sq = Node("*", children=[x1_a, x1_b])
    half = Node("const", value=0.5)
    term2 = Node("*", children=[half, x1_sq])

    # build term1 + term2
    poly_sum = Node("+", children=[term1, term2])

    # subtract x1: (poly_sum) - x1
    x1_c = Node("var", value=1)
    ground_truth_tree = Node("-", children=[poly_sum, x1_c])

    # evaluate on train set
    preds_tr = ground_truth_tree.eval(x_tr)
    rmse_tr = np.sqrt(np.mean((preds_tr - y_tr) ** 2))
    assert rmse_tr < 1e-10

    # evaluate on test set
    preds_te = ground_truth_tree.eval(x_te)
    rmse_te = np.sqrt(np.mean((preds_te - y_te) ** 2))
    assert rmse_te < 1e-10


# scenario 3: synth2 bivariate surface run

def test_scenario_synth2_regression_run():
    """test full genetic programming regression run on synth2 dataset"""
    if GPEngine is None:
        pytest.skip("engine not implemented yet")
    x_tr, y_tr, x_te, y_te = load_dataset("synth2", base_dir=repo_root)
    engine = GPEngine(
        pop_size=20,
        generations=3,
        selection="lexicase",
        pc=0.90,
        pm=0.05,
        elitism=True,
        seed=42,
    )
    res = engine.fit(x_tr, y_tr, x_te, y_te)
    best = res["best"]
    preds = best.eval(x_te) if hasattr(best, "eval") else best.root.eval(x_te)
    assert preds.shape == (1000,)
    assert not np.isnan(preds).any()
    assert not np.isinf(preds).any()


# scenario 4: concrete 8 dimensional regression run

def test_scenario_concrete_regression_run():
    """test full genetic programming regression run on 8 dimensional concrete dataset"""
    if GPEngine is None:
        pytest.skip("engine not implemented yet")
    x_tr, y_tr, x_te, y_te = load_dataset("concrete", base_dir=repo_root)
    engine = GPEngine(
        pop_size=20,
        generations=3,
        selection="roulette",
        pc=0.80,
        pm=0.15,
        elitism=True,
        seed=42,
    )
    res = engine.fit(x_tr, y_tr, x_te, y_te)
    history = res["history"]
    assert len(history) == 3
    final_entry = history[-1]
    # verify train and test rmse are positive finite floats
    assert final_entry["train_rmse_best"] > 0.0
    assert final_entry["test_rmse_best"] > 0.0


# scenario 5: baseline predictors calculation

def test_scenario_baseline_dummy_predictors():
    """test baseline mean predictor rmse matches expected dataset baselines"""
    # synth1 baseline
    _, y_tr1, _, y_te1 = load_dataset("synth1", base_dir=repo_root)
    mean_tr1 = np.mean(y_tr1)
    rmse_tr1 = np.sqrt(np.mean((y_tr1 - mean_tr1) ** 2))
    assert np.isclose(rmse_tr1, 18.6693, atol=1e-2)

    # synth2 baseline
    _, y_tr2, _, y_te2 = load_dataset("synth2", base_dir=repo_root)
    mean_tr2 = np.mean(y_tr2)
    rmse_tr2 = np.sqrt(np.mean((y_tr2 - mean_tr2) ** 2))
    assert np.isclose(rmse_tr2, 3.6799, atol=1e-2)

    # concrete baseline
    _, y_tr3, _, y_te3 = load_dataset("concrete", base_dir=repo_root)
    mean_tr3 = np.mean(y_tr3)
    rmse_tr3 = np.sqrt(np.mean((y_tr3 - mean_tr3) ** 2))
    assert np.isclose(rmse_tr3, 16.8021, atol=1e-2)


# scenario 6: metric history export schema verification

def test_scenario_history_schema_completeness():
    """test that history dictionary has all required metrics for plotting and tables"""
    if GPEngine is None:
        pytest.skip("engine not implemented yet")
    x_tr, y_tr, _, _ = load_dataset("synth1", base_dir=repo_root)
    engine = GPEngine(pop_size=15, generations=2, seed=42)
    res = engine.fit(x_tr, y_tr)
    history = res.get("history", [])
    assert len(history) >= 2
    required_cols = [
        "gen",
        "train_rmse_best",
        "train_rmse_worst",
        "train_rmse_mean",
        "duplicate_trees",
        "crossover_better",
        "crossover_worse",
        "crossover_total",
        "avg_nodes",
        "avg_depth",
        "best_nodes",
        "best_depth",
    ]
    for row in history:
        for col in required_cols:
            assert col in row
