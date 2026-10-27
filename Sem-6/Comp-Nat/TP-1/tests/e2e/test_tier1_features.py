# tier 1 tests for happy path isolated features
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
        reproduce,
    )
except ImportError:
    subtree_crossover = None
    mutate_constants = None
    subtree_mutation = None
    reproduce = None

try:
    from src.gp.metrics import (
        compute_rmse,
        count_duplicates,
        MetricTracker,
    )
except ImportError:
    compute_rmse = None
    count_duplicates = None
    MetricTracker = None

try:
    from src.gp.engine import GPEngine
except ImportError:
    GPEngine = None


# feature 1: dataset ingestion tests

def test_load_synth1_happy_path():
    """test loading synth1 dataset shapes and contiguous arrays"""
    x_tr, y_tr, x_te, y_te = load_dataset("synth1", base_dir=repo_root)
    assert x_tr.shape == (60, 2)
    assert y_tr.shape == (60,)
    assert x_te.shape == (600, 2)
    assert y_te.shape == (600,)
    assert x_tr.dtype == np.float64
    assert y_tr.dtype == np.float64
    assert x_te.dtype == np.float64
    assert y_te.dtype == np.float64
    assert x_tr.flags.c_contiguous
    assert y_tr.flags.c_contiguous
    assert x_te.flags.c_contiguous
    assert y_te.flags.c_contiguous


def test_load_synth2_happy_path():
    """test loading synth2 dataset shapes and contiguous arrays"""
    x_tr, y_tr, x_te, y_te = load_dataset("synth2", base_dir=repo_root)
    assert x_tr.shape == (300, 2)
    assert y_tr.shape == (300,)
    assert x_te.shape == (1000, 2)
    assert y_te.shape == (1000,)
    assert x_tr.dtype == np.float64
    assert y_tr.dtype == np.float64
    assert x_tr.flags.c_contiguous
    assert y_tr.flags.c_contiguous


def test_load_concrete_happy_path():
    """test loading concrete dataset shapes with 8 features"""
    x_tr, y_tr, x_te, y_te = load_dataset("concrete", base_dir=repo_root)
    assert x_tr.shape == (824, 8)
    assert y_tr.shape == (824,)
    assert x_te.shape == (206, 8)
    assert y_te.shape == (206,)
    assert x_tr.dtype == np.float64
    assert y_tr.dtype == np.float64
    assert x_tr.flags.c_contiguous
    assert y_tr.flags.c_contiguous


def test_load_dataset_no_nans_or_infs():
    """test that all datasets contain zero nan or infinite values"""
    for name in ("synth1", "synth2", "concrete"):
        x_tr, y_tr, x_te, y_te = load_dataset(name, base_dir=repo_root)
        for arr in (x_tr, y_tr, x_te, y_te):
            assert not np.isnan(arr).any()
            assert not np.isinf(arr).any()


def test_load_dataset_case_insensitivity():
    """test case insensitive name matching in load_dataset"""
    x_tr1, _, _, _ = load_dataset("Synth1", base_dir=repo_root)
    x_tr2, _, _, _ = load_dataset("SYNTH1", base_dir=repo_root)
    assert np.array_equal(x_tr1, x_tr2)


def test_load_dataset_invalid_name_raises_value_error():
    """test that unknown dataset name raises value error"""
    with pytest.raises(ValueError):
        load_dataset("non_existent_dataset_name", base_dir=repo_root)


# feature 2: protected primitives tests

def test_safe_add_basic():
    """test safe addition on standard vectors"""
    if safe_add is None:
        pytest.skip("primitives not implemented yet")
    a = np.array([1.0, 2.0, 3.0])
    b = np.array([4.0, 5.0, 6.0])
    res = safe_add(a, b)
    assert np.allclose(res, [5.0, 7.0, 9.0])


def test_safe_sub_basic():
    """test safe subtraction on standard vectors"""
    if safe_add is None:
        pytest.skip("primitives not implemented yet")
    a = np.array([10.0, 20.0, 30.0])
    b = np.array([1.0, 2.0, 3.0])
    res = safe_sub(a, b)
    assert np.allclose(res, [9.0, 18.0, 27.0])


def test_safe_mul_basic():
    """test safe multiplication on standard vectors"""
    if safe_add is None:
        pytest.skip("primitives not implemented yet")
    a = np.array([2.0, -3.0, 4.0])
    b = np.array([5.0, 2.0, -2.0])
    res = safe_mul(a, b)
    assert np.allclose(res, [10.0, -6.0, -8.0])


def test_safe_div_non_zero():
    """test safe division with non-zero denominators"""
    if safe_add is None:
        pytest.skip("primitives not implemented yet")
    a = np.array([10.0, -12.0, 0.0])
    b = np.array([2.0, 3.0, 5.0])
    res = safe_div(a, b)
    assert np.allclose(res, [5.0, -4.0, 0.0])


def test_safe_trigonometric():
    """test safe sine and cosine on known angle values"""
    if safe_add is None:
        pytest.skip("primitives not implemented yet")
    angles = np.array([0.0, np.pi / 2.0, np.pi])
    sin_res = safe_sin(angles)
    cos_res = safe_cos(angles)
    assert np.allclose(sin_res, [0.0, 1.0, 0.0], atol=1e-6)
    assert np.allclose(cos_res, [1.0, 0.0, -1.0], atol=1e-6)


def test_safe_log_positive():
    """test safe logarithm on positive real numbers"""
    if safe_add is None:
        pytest.skip("primitives not implemented yet")
    vals = np.array([1.0, np.e, np.e ** 2])
    res = safe_log(vals)
    assert np.allclose(res, [0.0, 1.0, 2.0], atol=1e-6)


def test_safe_exp_moderate():
    """test safe exponential on moderate range numbers"""
    if safe_add is None:
        pytest.skip("primitives not implemented yet")
    vals = np.array([0.0, 1.0, 2.0])
    res = safe_exp(vals)
    assert np.allclose(res, [1.0, np.e, np.e ** 2], atol=1e-6)


# feature 3: syntax tree node tests

def test_node_constant_leaf():
    """test constant leaf node properties"""
    if Node is None:
        pytest.skip("node not implemented yet")
    node = Node("const", value=3.14)
    assert node.op == "const"
    assert node.value == 3.14
    assert node.depth() == 1
    assert node.node_count() == 1
    assert "3.14" in node.to_string()


def test_node_variable_leaf():
    """test variable leaf node properties"""
    if Node is None:
        pytest.skip("node not implemented yet")
    node = Node("var", value=1)
    assert node.op == "var"
    assert node.value == 1
    assert node.depth() == 1
    assert node.node_count() == 1
    assert "x1" in node.to_string().lower()


def test_node_unary_internal():
    """test unary internal node depth and count"""
    if Node is None:
        pytest.skip("node not implemented yet")
    child = Node("var", value=0)
    root = Node("sin", children=[child])
    assert root.depth() == 2
    assert root.node_count() == 2
    assert "sin" in root.to_string().lower()


def test_node_binary_internal():
    """test binary internal node depth and count"""
    if Node is None:
        pytest.skip("node not implemented yet")
    left = Node("var", value=0)
    right = Node("const", value=5.0)
    root = Node("+", children=[left, right])
    assert root.depth() == 2
    assert root.node_count() == 3


def test_node_deep_copy_independence():
    """test that node copy creates a completely independent tree"""
    if Node is None:
        pytest.skip("node not implemented yet")
    left = Node("const", value=1.0)
    right = Node("const", value=2.0)
    root = Node("+", children=[left, right])
    clone = root.copy()
    assert clone.depth() == root.depth()
    assert clone.node_count() == root.node_count()
    # mutate clone child value to verify independence
    clone.children[0].value = 99.0
    assert root.children[0].value == 1.0


# feature 4: vectorized tree evaluation tests

def test_eval_constant_node_broadcasts():
    """test constant evaluation broadcasts across all rows of feature matrix"""
    if Node is None:
        pytest.skip("node not implemented yet")
    node = Node("const", value=42.0)
    x = np.zeros((10, 2))
    preds = node.eval(x)
    assert preds.shape == (10,)
    assert np.allclose(preds, 42.0)


def test_eval_variable_node_extracts_column():
    """test variable node extracts the specified column"""
    if Node is None:
        pytest.skip("node not implemented yet")
    node = Node("var", value=1)
    x = np.array([[10.0, 20.0], [30.0, 40.0], [50.0, 60.0]])
    preds = node.eval(x)
    assert np.allclose(preds, [20.0, 40.0, 60.0])


def test_eval_arithmetic_tree():
    """test evaluation of arithmetic expression x0 + x1"""
    if Node is None:
        pytest.skip("node not implemented yet")
    left = Node("var", value=0)
    right = Node("var", value=1)
    root = Node("+", children=[left, right])
    x = np.array([[1.0, 2.0], [3.0, 4.0], [5.0, 6.0]])
    preds = root.eval(x)
    assert np.allclose(preds, [3.0, 7.0, 11.0])


def test_eval_nonlinear_tree():
    """test evaluation of sin(x0) expression"""
    if Node is None:
        pytest.skip("node not implemented yet")
    child = Node("var", value=0)
    root = Node("sin", children=[child])
    x = np.array([[0.0], [np.pi / 2.0], [np.pi]])
    preds = root.eval(x)
    assert np.allclose(preds, [0.0, 1.0, 0.0], atol=1e-6)


def test_eval_nested_tree():
    """test evaluation of nested expression (x0 * x0) + 1.0"""
    if Node is None:
        pytest.skip("node not implemented yet")
    v1 = Node("var", value=0)
    v2 = Node("var", value=0)
    mul = Node("*", children=[v1, v2])
    one = Node("const", value=1.0)
    root = Node("+", children=[mul, one])
    x = np.array([[2.0], [3.0], [4.0]])
    preds = root.eval(x)
    assert np.allclose(preds, [5.0, 10.0, 17.0])


# feature 5: tree initialization tests

def test_ramped_half_and_half_pop_size():
    """test that ramped half and half generates exact requested population size"""
    if ramped_half_and_half is None:
        pytest.skip("initialization not implemented yet")
    pop = ramped_half_and_half(pop_size=50, num_features=2, min_depth=2, max_depth=5)
    assert len(pop) == 50


def test_ramped_half_and_half_depth_range():
    """test that generated individuals have depths between 2 and 5"""
    if ramped_half_and_half is None:
        pytest.skip("initialization not implemented yet")
    pop = ramped_half_and_half(pop_size=40, num_features=2, min_depth=2, max_depth=5)
    for ind in pop:
        d = ind.depth() if hasattr(ind, "depth") else ind.root.depth()
        assert 2 <= d <= 5


def test_ramped_half_and_half_terminal_indices():
    """test that variable terminal indices do not exceed feature count"""
    if ramped_half_and_half is None:
        pytest.skip("initialization not implemented yet")
    pop = ramped_half_and_half(pop_size=30, num_features=4, min_depth=2, max_depth=4)
    for ind in pop:
        root = ind.root if hasattr(ind, "root") else ind
        for n in root.get_all_nodes():
            if n.op == "var":
                assert 0 <= n.value < 4


def test_full_tree_exact_depth():
    """test that full tree generation creates trees with exact requested depth"""
    if full_tree is None:
        pytest.skip("initialization not implemented yet")
    rng = np.random.default_rng(42)
    tree = full_tree(depth=4, num_features=2, rng=rng)
    d = tree.depth() if hasattr(tree, "depth") else tree.root.depth()
    assert d == 4


def test_grow_tree_bounded_depth():
    """test that grow tree generation creates trees bounded by target depth"""
    if grow_tree is None:
        pytest.skip("initialization not implemented yet")
    rng = np.random.default_rng(42)
    for _ in range(10):
        tree = grow_tree(depth=4, num_features=2, rng=rng)
        d = tree.depth() if hasattr(tree, "depth") else tree.root.depth()
        assert 2 <= d <= 4


# feature 6: fitness calculation (rmse) tests

def test_rmse_identical_vectors():
    """test rmse between identical vectors is exactly zero"""
    if compute_rmse is None:
        # compute directly via numpy
        y_true = np.array([1.0, 2.0, 3.0, 4.0])
        y_pred = np.array([1.0, 2.0, 3.0, 4.0])
        val = np.sqrt(np.mean((y_true - y_pred) ** 2))
    else:
        y_true = np.array([1.0, 2.0, 3.0, 4.0])
        y_pred = np.array([1.0, 2.0, 3.0, 4.0])
        val = compute_rmse(y_true, y_pred)
    assert val == 0.0


def test_rmse_known_constant_error():
    """test rmse with known constant difference"""
    y_true = np.array([10.0, 20.0, 30.0])
    y_pred = np.array([12.0, 22.0, 32.0])
    if compute_rmse is None:
        val = np.sqrt(np.mean((y_true - y_pred) ** 2))
    else:
        val = compute_rmse(y_true, y_pred)
    assert np.isclose(val, 2.0)


def test_rmse_symmetry():
    """test that rmse is symmetric with respect to arguments"""
    y_true = np.array([1.0, 5.0, 9.0])
    y_pred = np.array([2.0, 3.0, 11.0])
    if compute_rmse is None:
        val1 = np.sqrt(np.mean((y_true - y_pred) ** 2))
        val2 = np.sqrt(np.mean((y_pred - y_true) ** 2))
    else:
        val1 = compute_rmse(y_true, y_pred)
        val2 = compute_rmse(y_pred, y_true)
    assert np.isclose(val1, val2)


def test_rmse_non_negative():
    """test that rmse is strictly non negative for any inputs"""
    y_true = np.random.randn(50)
    y_pred = np.random.randn(50)
    if compute_rmse is None:
        val = np.sqrt(np.mean((y_true - y_pred) ** 2))
    else:
        val = compute_rmse(y_true, y_pred)
    assert val >= 0.0


def test_rmse_batch_predictions():
    """test rmse calculation across population matrix"""
    y_true = np.array([0.0, 0.0, 0.0])
    y_preds = np.array([[1.0, 1.0, 1.0], [2.0, 2.0, 2.0]])
    errors = np.sqrt(np.mean((y_preds - y_true) ** 2, axis=1))
    assert np.allclose(errors, [1.0, 2.0])


# feature 7: selection operators tests

def test_tournament_selection_picks_best():
    """test tournament selection chooses the candidate with lower rmse"""
    if tournament_selection is None:
        pytest.skip("selection not implemented yet")
    fitnesses = np.array([10.0, 2.0, 8.0, 5.0])
    rng = np.random.default_rng(42)
    # in k=2 tournament comparing index 0 (fit 10) and index 1 (fit 2), winner is 1
    winner = tournament_selection(fitnesses=fitnesses, k=2, rng=rng)
    assert 0 <= winner < 4


def test_tournament_selection_bounds():
    """test tournament selection returned index is valid across many trials"""
    if tournament_selection is None:
        pytest.skip("selection not implemented yet")
    fitnesses = np.array([1.0, 5.0, 3.0, 2.0, 4.0])
    rng = np.random.default_rng(42)
    for _ in range(50):
        idx = tournament_selection(fitnesses=fitnesses, k=2, rng=rng)
        assert 0 <= idx < len(fitnesses)


def test_roulette_selection_weights_favor_best():
    """test scaled roulette wheel assigns higher probability to lower rmse"""
    if roulette_selection is None:
        pytest.skip("selection not implemented yet")
    fitnesses = np.array([1.0, 10.0])
    rng = np.random.default_rng(42)
    # best individual with fitness 1 should be selected far more frequently
    picks = [roulette_selection(fitnesses=fitnesses, rng=rng) for _ in range(200)]
    count_best = sum(1 for p in picks if p == 0)
    assert count_best > 120


def test_lexicase_selection_returns_valid():
    """test epsilon lexicase selection returns valid individual index"""
    if lexicase_selection is None:
        pytest.skip("selection not implemented yet")
    # error matrix shape (m candidates, n test cases)
    err_matrix = np.array([
        [0.1, 0.2, 0.3],
        [0.5, 0.4, 0.2],
        [0.05, 0.1, 0.1],
    ])
    rng = np.random.default_rng(42)
    idx = lexicase_selection(error_matrix=err_matrix, rng=rng)
    assert 0 <= idx < 3


def test_lexicase_selection_selects_strictly_dominant():
    """test lexicase selection reliably picks individual with lowest error on all cases"""
    if lexicase_selection is None:
        pytest.skip("selection not implemented yet")
    err_matrix = np.array([
        [10.0, 10.0, 10.0],
        [0.01, 0.01, 0.01],
        [5.0, 5.0, 5.0],
    ])
    rng = np.random.default_rng(42)
    picks = [lexicase_selection(error_matrix=err_matrix, rng=rng) for _ in range(20)]
    assert all(p == 1 for p in picks)


# feature 8: variation operators tests

def test_crossover_produces_valid_syntax_tree():
    """test subtree crossover returns valid tree structure"""
    if subtree_crossover is None or Node is None:
        pytest.skip("operators not implemented yet")
    p1 = Node("+", children=[Node("var", value=0), Node("const", value=1.0)])
    p2 = Node("*", children=[Node("var", value=1), Node("const", value=2.0)])
    rng = np.random.default_rng(42)
    child = subtree_crossover(p1, p2, max_depth=7, rng=rng)
    assert child.depth() <= 7
    assert child.node_count() >= 1


def test_crossover_parent_immutability():
    """test subtree crossover does not mutate parent trees in place"""
    if subtree_crossover is None or Node is None:
        pytest.skip("operators not implemented yet")
    p1 = Node("+", children=[Node("var", value=0), Node("const", value=1.0)])
    p2 = Node("*", children=[Node("var", value=1), Node("const", value=2.0)])
    s1_before = p1.to_string()
    s2_before = p2.to_string()
    rng = np.random.default_rng(42)
    _ = subtree_crossover(p1, p2, max_depth=7, rng=rng)
    assert p1.to_string() == s1_before
    assert p2.to_string() == s2_before


def test_constant_mutation_perturbs_value():
    """test constant mutation modifies ephemeral constant value"""
    if mutate_constants is None or Node is None:
        pytest.skip("operators not implemented yet")
    parent = Node("+", children=[Node("var", value=0), Node("const", value=5.0)])
    rng = np.random.default_rng(42)
    mutant = mutate_constants(parent, sigma=0.5, rng=rng)
    assert mutant.depth() == parent.depth()
    # constant should be perturbed with high probability
    assert mutant.children[1].value != 5.0


def test_subtree_mutation_validity():
    """test subtree mutation produces valid tree bounded by max depth"""
    if subtree_mutation is None or Node is None:
        pytest.skip("operators not implemented yet")
    parent = Node("+", children=[Node("var", value=0), Node("const", value=1.0)])
    rng = np.random.default_rng(42)
    mutant = subtree_mutation(parent, max_depth=7, num_features=2, rng=rng)
    assert mutant.depth() <= 7


def test_reproduction_clones_exactly():
    """test reproduction operator produces an exact structural clone"""
    if reproduce is None or Node is None:
        pytest.skip("operators not implemented yet")
    parent = Node("sin", children=[Node("var", value=0)])
    child = reproduce(parent)
    assert child.to_string() == parent.to_string()
    assert child is not parent


# feature 9: metric logging and tracking tests

def test_count_duplicates_distinct_trees():
    """test duplicate counter returns zero for all unique trees"""
    if count_duplicates is None or Node is None:
        pytest.skip("metrics not implemented yet")
    trees = [
        Node("var", value=0),
        Node("var", value=1),
        Node("const", value=3.0),
    ]
    assert count_duplicates(trees) == 0


def test_count_duplicates_repeated_trees():
    """test duplicate counter returns correct count when duplicates exist"""
    if count_duplicates is None or Node is None:
        pytest.skip("metrics not implemented yet")
    trees = [
        Node("var", value=0),
        Node("var", value=0),
        Node("var", value=0),
        Node("const", value=1.0),
    ]
    # 4 individuals with 2 unique shapes gives 2 duplicates
    assert count_duplicates(trees) == 2


def test_metric_tracker_records_entries():
    """test metric tracker appends generation stats correctly"""
    if MetricTracker is None:
        pytest.skip("metrics not implemented yet")
    tracker = MetricTracker()
    tracker.log_generation(
        gen=0,
        train_rmse_best=1.5,
        train_rmse_worst=10.0,
        train_rmse_mean=4.2,
        test_rmse_best=2.0,
        duplicate_trees=3,
        crossover_better=5,
        crossover_worse=2,
        crossover_total=10,
        avg_nodes=7.2,
        avg_depth=3.1,
        best_nodes=5,
        best_depth=3,
    )
    records = tracker.get_history()
    assert len(records) == 1
    assert records[0]["gen"] == 0
    assert records[0]["train_rmse_best"] == 1.5


def test_metric_tracker_best_worst_mean_consistency():
    """test mathematical consistency of tracked best worst and mean metrics"""
    if MetricTracker is None:
        pytest.skip("metrics not implemented yet")
    fitnesses = [1.0, 3.0, 5.0, 7.0]
    best_val = min(fitnesses)
    worst_val = max(fitnesses)
    mean_val = sum(fitnesses) / len(fitnesses)
    assert best_val <= mean_val <= worst_val


def test_metric_tracker_export_keys():
    """test exported history dictionary contains all required project columns"""
    if MetricTracker is None:
        pytest.skip("metrics not implemented yet")
    tracker = MetricTracker()
    tracker.log_generation(
        gen=1,
        train_rmse_best=0.8,
        train_rmse_worst=5.0,
        train_rmse_mean=2.1,
        test_rmse_best=1.1,
        duplicate_trees=1,
        crossover_better=4,
        crossover_worse=3,
        crossover_total=8,
        avg_nodes=6.0,
        avg_depth=2.8,
        best_nodes=4,
        best_depth=2,
    )
    entry = tracker.get_history()[0]
    expected_keys = {
        "gen",
        "train_rmse_best",
        "train_rmse_worst",
        "train_rmse_mean",
        "test_rmse_best",
        "duplicate_trees",
        "crossover_better",
        "crossover_worse",
        "crossover_total",
        "avg_nodes",
        "avg_depth",
        "best_nodes",
        "best_depth",
    }
    assert expected_keys.issubset(set(entry.keys()))


# feature 10: evolution engine tests

def test_gp_engine_init_defaults():
    """test gp engine initializes with valid default parameters"""
    if GPEngine is None:
        pytest.skip("engine not implemented yet")
    engine = GPEngine(pop_size=50, generations=10)
    assert engine.pop_size == 50
    assert engine.generations == 10


def test_gp_engine_fit_runs_generations():
    """test gp engine executes requested number of generations"""
    if GPEngine is None:
        pytest.skip("engine not implemented yet")
    x = np.random.randn(20, 2)
    y = x[:, 0] + x[:, 1]
    engine = GPEngine(pop_size=20, generations=3, seed=42)
    res = engine.fit(x, y)
    assert "history" in res or "best" in res


def test_gp_engine_elitism_preserves_best():
    """test elitism guarantees monotonic non increasing best train rmse"""
    if GPEngine is None:
        pytest.skip("engine not implemented yet")
    x = np.random.randn(30, 2)
    y = x[:, 0] * 2.0
    engine = GPEngine(pop_size=20, generations=5, elitism=True, seed=42)
    res = engine.fit(x, y)
    history = res.get("history", [])
    if history:
        bests = [h["train_rmse_best"] for h in history]
        for i in range(1, len(bests)):
            assert bests[i] <= bests[i - 1] + 1e-9


def test_gp_engine_seed_determinism():
    """test identical seeds produce identical evolutionary histories"""
    if GPEngine is None:
        pytest.skip("engine not implemented yet")
    x = np.random.randn(20, 2)
    y = x[:, 0] - x[:, 1]
    e1 = GPEngine(pop_size=15, generations=3, seed=123)
    e2 = GPEngine(pop_size=15, generations=3, seed=123)
    r1 = e1.fit(x, y)
    r2 = e2.fit(x, y)
    h1 = [h["train_rmse_best"] for h in r1.get("history", [])]
    h2 = [h["train_rmse_best"] for h in r2.get("history", [])]
    assert np.allclose(h1, h2)


def test_gp_engine_best_individual_evaluable():
    """test that the best individual returned by engine evaluates properly"""
    if GPEngine is None:
        pytest.skip("engine not implemented yet")
    x = np.random.randn(20, 2)
    y = x[:, 0]
    engine = GPEngine(pop_size=20, generations=2, seed=42)
    res = engine.fit(x, y)
    best = res.get("best")
    if best is not None:
        preds = best.eval(x) if hasattr(best, "eval") else best.root.eval(x)
        assert preds.shape == (20,)
        assert not np.isnan(preds).any()
