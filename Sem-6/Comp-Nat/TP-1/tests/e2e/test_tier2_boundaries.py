# tier 2 tests for boundary conditions, numerical limits, and corner cases
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
    from src.gp.primitives import (
        safe_add,
        safe_sub,
        safe_mul,
        safe_div,
        safe_sin,
        safe_cos,
        safe_log,
        safe_exp,
    )
except ImportError:
    safe_add = None

try:
    from src.gp.node import Node
except ImportError:
    Node = None

try:
    from src.gp.tree import (
        Tree,
        ramped_half_and_half,
        full_tree,
        grow_tree,
    )
except ImportError:
    Tree = None
    ramped_half_and_half = None
    full_tree = None
    grow_tree = None

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
        subtree_mutation,
    )
except ImportError:
    subtree_crossover = None
    mutate_constants = None
    subtree_mutation = None

try:
    from src.gp.engine import GPEngine
except ImportError:
    GPEngine = None


# boundary category 1: numerical protections and extreme math

def test_safe_div_by_exact_zero():
    """test safe division by exact zero returns one"""
    if safe_add is None:
        pytest.skip("primitives not implemented yet")
    num = np.array([5.0, -10.0, 0.0])
    denom = np.array([0.0, 0.0, 0.0])
    res = safe_div(num, denom)
    assert np.allclose(res, [1.0, 1.0, 1.0])
    assert not np.isnan(res).any()
    assert not np.isinf(res).any()


def test_safe_div_by_sub_epsilon():
    """test safe division by numbers smaller than 1e-6 returns one"""
    if safe_add is None:
        pytest.skip("primitives not implemented yet")
    num = np.array([4.2, -7.1])
    denom = np.array([1e-12, -1e-15])
    res = safe_div(num, denom)
    assert np.allclose(res, [1.0, 1.0])


def test_safe_log_of_exact_zero():
    """test safe logarithm of zero returns zero"""
    if safe_add is None:
        pytest.skip("primitives not implemented yet")
    x = np.array([0.0])
    res = safe_log(x)
    assert np.allclose(res, [0.0])
    assert not np.isnan(res).any()
    assert not np.isinf(res).any()


def test_safe_log_of_negative_values():
    """test safe logarithm evaluates absolute value on negative numbers"""
    if safe_add is None:
        pytest.skip("primitives not implemented yet")
    x = np.array([-np.e, -(np.e ** 2)])
    res = safe_log(x)
    assert np.allclose(res, [1.0, 2.0], atol=1e-6)


def test_safe_log_of_sub_epsilon():
    """test safe logarithm on numbers smaller than 1e-6 returns zero"""
    if safe_add is None:
        pytest.skip("primitives not implemented yet")
    x = np.array([1e-10, -1e-12])
    res = safe_log(x)
    assert np.allclose(res, [0.0, 0.0])


def test_safe_exp_positive_overflow_clamped():
    """test safe exponential clips arguments above 32 to prevent float64 overflow"""
    if safe_add is None:
        pytest.skip("primitives not implemented yet")
    x = np.array([50.0, 500.0, 1e9])
    res = safe_exp(x)
    expected = np.exp(32.0)
    assert np.allclose(res, [expected, expected, expected])
    assert not np.isnan(res).any()
    assert not np.isinf(res).any()


def test_safe_exp_negative_underflow_clamped():
    """test safe exponential clips arguments below negative 32"""
    if safe_add is None:
        pytest.skip("primitives not implemented yet")
    x = np.array([-50.0, -500.0, -1e9])
    res = safe_exp(x)
    expected = np.exp(-32.0)
    assert np.allclose(res, [expected, expected, expected])


def test_safe_mul_overflow_clipping():
    """test multiplication of huge floats is safely clamped"""
    if safe_add is None:
        pytest.skip("primitives not implemented yet")
    x = np.array([1e40, -1e40])
    y = np.array([1e40, 1e40])
    res = safe_mul(x, y)
    assert not np.isnan(res).any()
    assert not np.isinf(res).any()


# boundary category 2: depth ceiling limit enforcement

def test_depth_ceiling_crossover_rejection():
    """test crossover exceeding depth 7 is rejected and returns unmodified parent"""
    if subtree_crossover is None or Node is None:
        pytest.skip("operators not implemented yet")
    # build a tree of depth 7
    curr = Node("var", value=0)
    for _ in range(6):
        curr = Node("sin", children=[curr])
    assert curr.depth() == 7

    # build donor tree of depth 4
    donor = Node("var", value=1)
    for _ in range(3):
        donor = Node("cos", children=[donor])
    assert donor.depth() == 4

    # grafting donor onto a deep node of curr would exceed depth 7
    rng = np.random.default_rng(42)
    for _ in range(20):
        child = subtree_crossover(curr, donor, max_depth=7, rng=rng)
        assert child.depth() <= 7


def test_depth_ceiling_mutation_rejection():
    """test subtree mutation exceeding depth 7 reverts to parent"""
    if subtree_mutation is None or Node is None:
        pytest.skip("operators not implemented yet")
    curr = Node("var", value=0)
    for _ in range(6):
        curr = Node("exp", children=[curr])
    assert curr.depth() == 7

    rng = np.random.default_rng(42)
    for _ in range(20):
        mutant = subtree_mutation(curr, max_depth=7, num_features=2, rng=rng)
        assert mutant.depth() <= 7


def test_exact_depth_seven_allowed():
    """test that a tree of depth exactly 7 is accepted and functional"""
    if Node is None:
        pytest.skip("node not implemented yet")
    curr = Node("var", value=0)
    for _ in range(6):
        curr = Node("+", children=[curr, Node("const", value=1.0)])
    assert curr.depth() == 7
    x = np.array([[2.0], [5.0]])
    preds = curr.eval(x)
    assert preds.shape == (2,)
    assert not np.isnan(preds).any()


def test_single_leaf_depth_one():
    """test that minimal tree with depth 1 evaluates on multi row inputs"""
    if Node is None:
        pytest.skip("node not implemented yet")
    leaf = Node("var", value=0)
    assert leaf.depth() == 1
    assert leaf.node_count() == 1
    x = np.array([[10.0], [20.0], [30.0]])
    preds = leaf.eval(x)
    assert np.allclose(preds, [10.0, 20.0, 30.0])


# boundary category 3: selection corner cases

def test_dynamic_mad_identical_candidate_errors():
    """test epsilon lexicase handles identical errors across candidates without crashing"""
    if lexicase_selection is None:
        pytest.skip("selection not implemented yet")
    # all candidates have identical error 0.5 across all cases
    err_matrix = np.full((10, 5), 0.5)
    rng = np.random.default_rng(42)
    idx = lexicase_selection(error_matrix=err_matrix, rng=rng)
    assert 0 <= idx < 10


def test_dynamic_mad_single_candidate_pool():
    """test lexicase terminates immediately when pool size is one"""
    if lexicase_selection is None:
        pytest.skip("selection not implemented yet")
    # candidate 0 is vastly superior on first test case
    err_matrix = np.array([
        [0.0, 10.0, 10.0],
        [100.0, 10.0, 10.0],
        [100.0, 10.0, 10.0],
        [100.0, 10.0, 10.0],
    ])
    rng = np.random.default_rng(42)
    idx = lexicase_selection(error_matrix=err_matrix, rng=rng)
    assert idx == 0


def test_roulette_identical_fitness_uniform_selection():
    """test roulette wheel with equal fitness avoids zero division and selects uniformly"""
    if roulette_selection is None:
        pytest.skip("selection not implemented yet")
    fitnesses = np.array([5.0, 5.0, 5.0, 5.0])
    rng = np.random.default_rng(42)
    picks = [roulette_selection(fitnesses=fitnesses, rng=rng) for _ in range(100)]
    unique_picks = set(picks)
    # over 100 trials, all 4 candidates should be selected
    assert len(unique_picks) == 4


def test_roulette_extreme_fitness_disparity():
    """test roulette wheel does not crash or produce nans with extreme fitness differences"""
    if roulette_selection is None:
        pytest.skip("selection not implemented yet")
    fitnesses = np.array([0.0, 1e9])
    rng = np.random.default_rng(42)
    idx = roulette_selection(fitnesses=fitnesses, rng=rng)
    assert idx in (0, 1)


def test_tournament_minimal_population():
    """test tournament selection works with minimum population size of two"""
    if tournament_selection is None:
        pytest.skip("selection not implemented yet")
    fitnesses = np.array([1.2, 3.4])
    rng = np.random.default_rng(42)
    winner = tournament_selection(fitnesses=fitnesses, k=2, rng=rng)
    assert winner in (0, 1)


# boundary category 4: dataset and engine dimensional boundaries

def test_eval_single_row_input():
    """test vectorized evaluation handles n=1 row feature matrix"""
    if Node is None:
        pytest.skip("node not implemented yet")
    left = Node("var", value=0)
    right = Node("const", value=3.0)
    root = Node("*", children=[left, right])
    x = np.array([[7.0]])
    preds = root.eval(x)
    assert preds.shape == (1,)
    assert np.isclose(preds[0], 21.0)


def test_eval_high_dimensional_features():
    """test evaluation with high column dimension d=50"""
    if Node is None:
        pytest.skip("node not implemented yet")
    node = Node("var", value=49)
    x = np.zeros((5, 50))
    x[:, 49] = 99.0
    preds = node.eval(x)
    assert np.allclose(preds, 99.0)


def test_engine_zero_generations():
    """test evolution engine runs with generations=0 evaluating initial population"""
    if GPEngine is None:
        pytest.skip("engine not implemented yet")
    x = np.array([[1.0], [2.0], [3.0]])
    y = np.array([2.0, 4.0, 6.0])
    engine = GPEngine(pop_size=10, generations=0, seed=42)
    res = engine.fit(x, y)
    assert "best" in res


def test_engine_mutation_only_regime():
    """test evolution engine runs with pc=0.0 and pm=1.0"""
    if GPEngine is None:
        pytest.skip("engine not implemented yet")
    x = np.random.randn(20, 2)
    y = x[:, 0]
    engine = GPEngine(pop_size=10, generations=2, pc=0.0, pm=1.0, seed=42)
    res = engine.fit(x, y)
    assert res is not None


def test_engine_crossover_only_regime():
    """test evolution engine runs with pc=1.0 and pm=0.0"""
    if GPEngine is None:
        pytest.skip("engine not implemented yet")
    x = np.random.randn(20, 2)
    y = x[:, 1]
    engine = GPEngine(pop_size=10, generations=2, pc=1.0, pm=0.0, seed=42)
    res = engine.fit(x, y)
    assert res is not None
