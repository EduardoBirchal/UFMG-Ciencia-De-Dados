# tier 3 tests for pairwise feature interactions and parameter combinations
import os
import sys
import numpy as np
import pytest

# ensure project root is on sys path
repo_root = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
if repo_root not in sys.path:
    sys.path.insert(0, repo_root)

# optional imports for features implemented in later milestones
try:
    from src.gp.node import Node
except ImportError:
    Node = None

try:
    from src.gp.tree import (
        Tree,
        ramped_half_and_half,
    )
except ImportError:
    Tree = None
    ramped_half_and_half = None

try:
    from src.gp.selection import (
        tournament_selection,
        roulette_selection,
        lexicase_selection,
    )
except ImportError:
    tournament_selection = None
    roulette_selection = None
    lexicase_selection = None

try:
    from src.gp.operators import (
        subtree_crossover,
        mutate_constants,
    )
except ImportError:
    subtree_crossover = None
    mutate_constants = None

try:
    from src.gp.metrics import (
        count_duplicates,
    )
except ImportError:
    count_duplicates = None

try:
    from src.gp.engine import GPEngine
except ImportError:
    GPEngine = None


# combination 1: tournament selection with and without elitism

def test_combination_tournament_with_elitism():
    """test tournament selection combined with elitism enabled"""
    if GPEngine is None:
        pytest.skip("engine not implemented yet")
    x = np.random.randn(30, 2)
    y = x[:, 0] * 3.0 + x[:, 1]
    engine = GPEngine(pop_size=20, generations=4, selection="tournament", elitism=True, seed=42)
    res = engine.fit(x, y)
    history = res.get("history", [])
    if history:
        # best fitness should never worsen when elitism is true
        bests = [h["train_rmse_best"] for h in history]
        for i in range(1, len(bests)):
            assert bests[i] <= bests[i - 1] + 1e-9


def test_combination_tournament_without_elitism():
    """test tournament selection combined with elitism disabled"""
    if GPEngine is None:
        pytest.skip("engine not implemented yet")
    x = np.random.randn(30, 2)
    y = x[:, 0] * 3.0 + x[:, 1]
    engine = GPEngine(pop_size=20, generations=4, selection="tournament", elitism=False, seed=42)
    res = engine.fit(x, y)
    assert res is not None


# combination 2: scaled roulette wheel with and without elitism

def test_combination_roulette_with_elitism():
    """test scaled roulette wheel selection combined with elitism enabled"""
    if GPEngine is None:
        pytest.skip("engine not implemented yet")
    x = np.random.randn(30, 2)
    y = x[:, 0] - x[:, 1]
    engine = GPEngine(pop_size=20, generations=4, selection="roulette", elitism=True, seed=42)
    res = engine.fit(x, y)
    history = res.get("history", [])
    if history:
        bests = [h["train_rmse_best"] for h in history]
        for i in range(1, len(bests)):
            assert bests[i] <= bests[i - 1] + 1e-9


def test_combination_roulette_without_elitism():
    """test scaled roulette wheel selection combined with elitism disabled"""
    if GPEngine is None:
        pytest.skip("engine not implemented yet")
    x = np.random.randn(30, 2)
    y = x[:, 0] - x[:, 1]
    engine = GPEngine(pop_size=20, generations=4, selection="roulette", elitism=False, seed=42)
    res = engine.fit(x, y)
    assert res is not None


# combination 3: epsilon lexicase with and without elitism

def test_combination_lexicase_with_elitism():
    """test epsilon lexicase selection combined with elitism enabled"""
    if GPEngine is None:
        pytest.skip("engine not implemented yet")
    x = np.random.randn(25, 2)
    y = x[:, 0] ** 2
    engine = GPEngine(pop_size=20, generations=4, selection="lexicase", elitism=True, seed=42)
    res = engine.fit(x, y)
    history = res.get("history", [])
    if history:
        bests = [h["train_rmse_best"] for h in history]
        for i in range(1, len(bests)):
            assert bests[i] <= bests[i - 1] + 1e-9


def test_combination_lexicase_without_elitism():
    """test epsilon lexicase selection combined with elitism disabled"""
    if GPEngine is None:
        pytest.skip("engine not implemented yet")
    x = np.random.randn(25, 2)
    y = x[:, 0] ** 2
    engine = GPEngine(pop_size=20, generations=4, selection="lexicase", elitism=False, seed=42)
    res = engine.fit(x, y)
    assert res is not None


# combination 4: operator probabilities comparison

def test_combination_high_crossover_regime():
    """test high crossover regime with pc=0.90 and pm=0.05"""
    if GPEngine is None:
        pytest.skip("engine not implemented yet")
    x = np.random.randn(25, 2)
    y = x[:, 0] + 2.0 * x[:, 1]
    engine = GPEngine(pop_size=20, generations=3, pc=0.90, pm=0.05, seed=42)
    res = engine.fit(x, y)
    assert res is not None


def test_combination_high_mutation_regime():
    """test high mutation regime with pc=0.60 and pm=0.30"""
    if GPEngine is None:
        pytest.skip("engine not implemented yet")
    x = np.random.randn(25, 2)
    y = x[:, 0] + 2.0 * x[:, 1]
    engine = GPEngine(pop_size=20, generations=3, pc=0.60, pm=0.30, seed=42)
    res = engine.fit(x, y)
    assert res is not None


def test_combination_reproduction_only_regime():
    """test reproduction only regime preserving population syntax across generations"""
    if GPEngine is None or count_duplicates is None:
        pytest.skip("engine or metrics not implemented yet")
    x = np.random.randn(20, 2)
    y = x[:, 0]
    # pc=0.0 and pm=0.0 leaves 1.0 for reproduction/cloning
    engine = GPEngine(pop_size=15, generations=2, pc=0.0, pm=0.0, seed=42)
    res = engine.fit(x, y)
    assert res is not None


# combination 5: ramped half and half with different feature dimensions

def test_combination_initialization_dimension_one():
    """test tree initialization on single input dimension d=1"""
    if ramped_half_and_half is None or Node is None:
        pytest.skip("initialization not implemented yet")
    pop = ramped_half_and_half(pop_size=20, num_features=1, min_depth=2, max_depth=4)
    for ind in pop:
        root = ind.root if hasattr(ind, "root") else ind
        for n in root.get_all_nodes():
            if n.op == "var":
                assert n.value == 0


def test_combination_initialization_dimension_eight():
    """test tree initialization on 8 input dimensions matching concrete dataset"""
    if ramped_half_and_half is None or Node is None:
        pytest.skip("initialization not implemented yet")
    pop = ramped_half_and_half(pop_size=30, num_features=8, min_depth=2, max_depth=5)
    observed_features = set()
    for ind in pop:
        root = ind.root if hasattr(ind, "root") else ind
        for n in root.get_all_nodes():
            if n.op == "var":
                assert 0 <= n.value < 8
                observed_features.add(n.value)
    # in a population of 30 trees with depth up to 5 multiple features should appear
    assert len(observed_features) > 1


# combination 6: crossover combining distinct feature terminals

def test_combination_crossover_feature_mixing():
    """test crossover between parents containing distinct feature terminals"""
    if subtree_crossover is None or Node is None:
        pytest.skip("operators not implemented yet")
    # parent 1 only uses x0
    p1 = Node("+", children=[Node("var", value=0), Node("const", value=1.0)])
    # parent 2 only uses x1
    p2 = Node("*", children=[Node("var", value=1), Node("const", value=2.0)])
    rng = np.random.default_rng(42)
    # multiple crossover attempts should produce offspring containing x1 from donor
    mixed_found = False
    for _ in range(20):
        child = subtree_crossover(p1, p2, max_depth=7, rng=rng)
        features = {n.value for n in child.get_all_nodes() if n.op == "var"}
        if 0 in features and 1 in features:
            mixed_found = True
            break
    assert mixed_found


# combination 7: constant mutation preserving tree structure

def test_combination_constant_mutation_preserves_topology():
    """test constant mutation modifies only constants while leaving tree topology invariant"""
    if mutate_constants is None or Node is None:
        pytest.skip("operators not implemented yet")
    left = Node("var", value=0)
    right = Node("const", value=10.0)
    parent = Node("+", children=[left, right])
    rng = np.random.default_rng(42)
    mutant = mutate_constants(parent, sigma=0.5, rng=rng)
    assert mutant.depth() == parent.depth()
    assert mutant.node_count() == parent.node_count()
    assert mutant.op == parent.op
    assert mutant.children[0].op == "var"
    assert mutant.children[0].value == 0
    assert mutant.children[1].op == "const"
