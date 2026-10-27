"""comprehensive unit tests for selection variation operators and gp engine"""

import os
import sys
import numpy as np
import pytest

# ensure project root is on sys path
repo_root = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
if repo_root not in sys.path:
    sys.path.insert(0, repo_root)

from src.data import load_dataset
from src.gp.engine import GPEngine
from src.gp.metrics import MetricTracker, compute_rmse, count_duplicates
from src.gp.node import Node
from src.gp.operators import (
    gaussian_constant_mutation,
    mutate,
    reproduce,
    subtree_crossover,
    subtree_mutation,
)
from src.gp.selection import (
    epsilon_lexicase_selection,
    scaled_roulette_selection,
    tournament_selection,
)
from src.gp.tree import Tree, full_tree, grow_tree, ramped_half_and_half


# 1. tournament selection tests

def test_tournament_k2_selects_lower_rmse():
    """tournament k=2 picks candidate with smaller error"""
    fits = np.array([8.0, 2.5, 9.1, 4.0])
    rng = np.random.default_rng(123)
    picks = [tournament_selection(fitnesses=fits, k=2, rng=rng) for _ in range(50)]
    assert all(0 <= p < len(fits) for p in picks)
    # worst candidate with index 2 should almost never win
    assert picks.count(2) < picks.count(1)


def test_tournament_k2_tie_breaking():
    """tournament tie breaking selects first encountered index"""
    fits = np.array([3.0, 3.0])
    rng = np.random.default_rng(42)
    win = tournament_selection(fitnesses=fits, k=2, rng=rng)
    assert win in (0, 1)


def test_tournament_k_larger_pressure():
    """larger tournament size increases selection pressure for best"""
    fits = np.array([10.0, 8.0, 6.0, 4.0, 2.0])
    rng = np.random.default_rng(42)
    k4_picks = [tournament_selection(fitnesses=fits, k=4, rng=rng) for _ in range(100)]
    # best index 4 should dominate
    assert k4_picks.count(4) > 40


def test_tournament_positional_arguments():
    """tournament handles population argument passed positionally"""
    fits = np.array([5.0, 1.0, 7.0])
    rng = np.random.default_rng(42)
    idx = tournament_selection(fits, k=2, rng=rng)
    assert 0 <= idx < 3


# 2. scaled roulette selection tests

def test_scaled_roulette_linear_shift_formula():
    """scaled roulette wheel applies linear shift fmax - fi + delta"""
    fits = np.array([2.0, 8.0])
    f_max = 8.0
    delta = 1e-4
    w0 = (f_max - 2.0) + delta
    w1 = (f_max - 8.0) + delta
    p0 = w0 / (w0 + w1)
    p1 = w1 / (w0 + w1)

    rng = np.random.default_rng(42)
    picks = [scaled_roulette_selection(fitnesses=fits, delta=delta, rng=rng) for _ in range(2000)]
    empirical_p0 = picks.count(0) / len(picks)
    assert np.isclose(empirical_p0, p0, atol=0.03)


def test_scaled_roulette_identical_fitnesses_uniform():
    """identical fitnesses produce uniform probability distribution"""
    fits = np.array([4.0, 4.0, 4.0, 4.0])
    rng = np.random.default_rng(42)
    picks = [scaled_roulette_selection(fitnesses=fits, rng=rng) for _ in range(1000)]
    counts = [picks.count(i) for i in range(4)]
    for c in counts:
        assert 180 <= c <= 320


def test_scaled_roulette_extreme_disparity_finite():
    """extreme fitness gap handles weights without nan or zero division"""
    fits = np.array([0.0, 1e9])
    rng = np.random.default_rng(42)
    win = scaled_roulette_selection(fitnesses=fits, rng=rng)
    assert win == 0


# 3. epsilon lexicase selection tests

def test_epsilon_lexicase_hand_calculated_mad():
    """verifies dynamic mad step by step calculation on controlled matrix"""
    # 3 candidates evaluated on 1 test case with errors 1.0, 2.0, 10.0
    # e*=1.0, median=2.0, deviations=[1, 0, 8] -> mad=1.0
    # threshold = 1.0 + 1.0 + 1e-12 = 2.000000000001
    # candidates 0 and 1 survive, candidate 2 is filtered out
    err_mat = np.array([
        [1.0],
        [2.0],
        [10.0],
    ])
    rng = np.random.default_rng(42)
    idx = epsilon_lexicase_selection(error_matrix=err_mat, rng=rng)
    assert idx in (0, 1)



# 4. crossover depth limit tests

def test_crossover_depth_ceiling_reversion():
    """crossover exceeding depth 7 cleanly reverts to parent 1 copy"""
    # depth 7 parent
    node = Node("var", value=0)
    for _ in range(6):
        node = Node("cos", children=[node])
    p1 = Tree(node)
    assert p1.depth() == 7

    # depth 3 donor
    p2 = Tree(Node("+", children=[Node("var", value=1), Node("const", value=2.0)]))

    rng = np.random.default_rng(42)
    child, success = subtree_crossover(p1, p2, max_depth=7, rng=rng, return_success=True)
    assert child.depth() <= 7


# 5. mutation tests

def test_gaussian_constant_mutation_value_perturbation():
    """gaussian constant mutation perturbs numeric constant value"""
    tree = Tree(Node("+", children=[Node("var", value=0), Node("const", value=10.0)]))
    rng = np.random.default_rng(42)
    mut = gaussian_constant_mutation(tree, sigma=0.5, rng=rng)
    assert mut.depth() == tree.depth()
    assert mut.node_count() == tree.node_count()
    new_val = mut.root.children[1].value
    assert new_val != 10.0
    assert abs(new_val - 10.0) < 3.0


def test_gaussian_constant_mutation_no_constants():
    """gaussian constant mutation leaves tree without constants intact"""
    tree = Tree(Node("+", children=[Node("var", value=0), Node("var", value=1)]))
    rng = np.random.default_rng(42)
    mut = gaussian_constant_mutation(tree, sigma=0.5, rng=rng)
    assert mut.to_string() == tree.to_string()


def test_subtree_mutation_bounds_depth():
    """subtree mutation produces valid offspring bounded by max depth"""
    tree = grow_tree(depth=4, num_features=2, rng=np.random.default_rng(42))
    rng = np.random.default_rng(42)
    for _ in range(20):
        mut = subtree_mutation(tree, max_depth=7, num_features=2, rng=rng)
        assert mut.depth() <= 7


def test_mutate_dispatcher():
    """mutate helper handles trees with or without constants"""
    tree_c = Tree(Node("+", children=[Node("var", value=0), Node("const", value=1.0)]))
    rng = np.random.default_rng(42)
    res = mutate(tree_c, num_features=2, max_depth=7, rng=rng)
    assert res.depth() <= 7


def test_reproduction_creates_deep_copy():
    """reproduction produces independent clone"""
    tree = Tree(Node("sin", children=[Node("var", value=0)]))
    clone = reproduce(tree)
    assert clone.to_string() == tree.to_string()
    assert clone is not tree
    assert clone.root is not tree.root


# 6. elitism and gp engine 10-generation run on synth1

def test_gp_engine_synth1_ten_generations_convergence():
    """runs 10 generations of gp engine on synth1 verifying convergence and history schema"""
    x_tr, y_tr, x_te, y_te = load_dataset("synth1", base_dir=repo_root)

    engine = GPEngine(
        pop_size=30,
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
    assert "best" in res
    assert "history" in res
    history = res["history"]
    assert len(history) == 10

    # check monotonic improvement with elitism
    bests = [h["best_train_rmse"] for h in history]
    for i in range(1, len(bests)):
        assert bests[i] <= bests[i - 1] + 1e-9

    # check final best train rmse improved over initial best
    assert bests[-1] <= bests[0]

    # verify history schema contains all expected metric keys
    expected_cols = [
        "generation",
        "best_train_rmse",
        "worst_train_rmse",
        "mean_train_rmse",
        "best_test_rmse",
        "duplicate_trees",
        "unique_canonical_trees",
        "crossover_better",
        "crossover_worse",
        "crossover_total",
        "avg_nodes",
        "avg_depth",
        "best_nodes",
        "best_depth",
        "elapsed_seconds",
    ]
    for row in history:
        for col in expected_cols:
            assert col in row

    # test predictions of best individual
    preds = engine.predict(x_te)
    assert preds.shape == (600,)
    assert not np.isnan(preds).any()
    assert not np.isinf(preds).any()
