"""comprehensive unit tests for core gp primitives node representation and tree evaluation"""

import os
import sys
import numpy as np
import pytest

# ensure project root is in sys path
repo_root = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
if repo_root not in sys.path:
    sys.path.insert(0, repo_root)

from src.data import load_dataset
from src.gp.metrics import compute_rmse
from src.gp.primitives import (
    safe_div,
    safe_log,
    safe_exp,
)
from src.gp.node import (
    Node,
    replace_subtree,
)
from src.gp.tree import (
    Tree,
    full_tree,
    grow_tree,
    ramped_half_and_half,
)


def test_node_attributes_and_leaf():
    """verifies node initialization attributes and leaf metrics"""
    n_const = Node("const", value=2.718)
    assert n_const.op == "const"
    assert n_const.value == 2.718
    assert n_const.depth() == 1
    assert n_const.node_count() == 1
    assert "2.718" in n_const.to_string()

    n_var = Node("var", value=0)
    assert n_var.op == "var"
    assert n_var.value == 0
    assert n_var.depth() == 1
    assert n_var.node_count() == 1
    assert "x0" in n_var.to_string()


def test_node_deep_copy():
    """verifies deep copy produces independent trees without aliasing"""
    left = Node("var", value=0)
    right = Node("const", value=3.0)
    root = Node("+", children=[left, right])
    clone = root.copy()

    assert clone.depth() == root.depth()
    assert clone.node_count() == root.node_count()
    assert clone.to_string() == root.to_string()

    # modify clone to ensure original remains intact
    clone.children[0].value = 5
    clone.children[1].value = 99.0
    assert root.children[0].value == 0
    assert root.children[1].value == 3.0


def test_node_to_string_canonical():
    """verifies canonical string serialization across operator types"""
    v0 = Node("var", value=0)
    v1 = Node("var", value=1)
    c1 = Node("const", value=1.5)

    u_sin = Node("sin", children=[v0])
    assert u_sin.to_string() == "(sin x0)"

    b_add = Node("+", children=[v0, v1])
    assert b_add.to_string() == "(+ x0 x1)"

    b_nested = Node("*", children=[b_add, c1])
    assert b_nested.to_string() == "(* (+ x0 x1) 1.5000)"


def test_node_get_all_nodes_prefix_order():
    """verifies prefix traversal order of all nodes in tree"""
    # tree: (+ (* x0 x1) (sin 2.0))
    n_x0 = Node("var", value=0)
    n_x1 = Node("var", value=1)
    n_mul = Node("*", children=[n_x0, n_x1])
    n_c2 = Node("const", value=2.0)
    n_sin = Node("sin", children=[n_c2])
    root = Node("+", children=[n_mul, n_sin])

    all_nodes = root.get_all_nodes()
    assert len(all_nodes) == 6
    assert [n.op for n in all_nodes] == ["+", "*", "var", "var", "sin", "const"]


def test_node_get_constants():
    """verifies extraction of constant nodes"""
    n_c1 = Node("const", value=1.0)
    n_c2 = Node("const", value=2.0)
    n_v0 = Node("var", value=0)
    root = Node("+", children=[Node("*", children=[n_c1, n_v0]), n_c2])

    constants = root.get_constants()
    assert len(constants) == 2
    assert [c.value for c in constants] == [1.0, 2.0]


def test_replace_subtree_functional():
    """verifies immutable subtree replacement at different indices"""
    # tree: (+ x0 x1), indices: 0: +, 1: x0, 2: x1
    x0 = Node("var", value=0)
    x1 = Node("var", value=1)
    root = Node("+", children=[x0, x1])

    new_sub = Node("const", value=42.0)

    # replace index 1 (x0) with 42.0
    replaced = replace_subtree(root, 1, new_sub)
    assert replaced.to_string() == "(+ 42.0000 x1)"
    # verify original root is unchanged
    assert root.to_string() == "(+ x0 x1)"

    # replace index 0 (root) with new_sub
    replaced_root = replace_subtree(root, 0, new_sub)
    assert replaced_root.to_string() == "42.0000"

    # replace index 2 (x1) with (sin x0)
    sin_x0 = Node("sin", children=[Node("var", value=0)])
    replaced_right = replace_subtree(root, 2, sin_x0)
    assert replaced_right.to_string() == "(+ x0 (sin x0))"
    assert root.children[1].op == "var"


def test_safe_primitives_division_edge_cases():
    """verifies protected division returns 1 on near-zero and zero denominators"""
    num = np.array([10.0, -5.0, 0.0, 1e30])
    denom = np.array([0.0, 1e-7, -1e-8, 0.0])

    res = safe_div(num, denom)
    assert np.allclose(res, [1.0, 1.0, 1.0, 1.0])
    assert not np.isnan(res).any()
    assert not np.isinf(res).any()


def test_safe_primitives_log_edge_cases():
    """verifies protected log returns 0 on near-zero inputs and handles negatives"""
    vals = np.array([0.0, 1e-7, -1e-8, -1.0, np.e, -(np.e ** 3)])
    res = safe_log(vals)
    expected = [0.0, 0.0, 0.0, 0.0, 1.0, 3.0]
    assert np.allclose(res, expected, atol=1e-6)
    assert not np.isnan(res).any()
    assert not np.isinf(res).any()


def test_safe_primitives_exp_clamping():
    """verifies protected exp clamps inputs to prevent overflow and underflow"""
    vals = np.array([32.0, 50.0, 500.0, -32.0, -50.0, -500.0])
    res = safe_exp(vals)
    expected = [
        np.exp(32.0),
        np.exp(32.0),
        np.exp(32.0),
        np.exp(-32.0),
        np.exp(-32.0),
        np.exp(-32.0),
    ]
    assert np.allclose(res, expected)
    assert not np.isnan(res).any()
    assert not np.isinf(res).any()


def test_tree_wrapper_methods():
    """verifies tree wrapper class delegation and string methods"""
    root = Node("+", children=[Node("var", value=0), Node("const", value=1.0)])
    tree = Tree(root)

    assert tree.depth() == 2
    assert tree.node_count() == 3
    assert tree.to_string() == "(+ x0 1.0000)"
    assert str(tree) == "(+ x0 1.0000)"
    assert repr(tree) == "(+ x0 1.0000)"

    clone = tree.copy()
    assert clone.depth() == 2
    assert clone.node_count() == 3
    clone.root.children[1].value = 10.0
    assert tree.root.children[1].value == 1.0


def test_tree_eval_vectorized_broadcasting():
    """verifies vectorized evaluation broadcasts scalars and extracts features"""
    # tree: (+ (* x0 x0) (* 2.0 x1))
    x0_a = Node("var", value=0)
    x0_b = Node("var", value=0)
    term1 = Node("*", children=[x0_a, x0_b])
    c2 = Node("const", value=2.0)
    x1 = Node("var", value=1)
    term2 = Node("*", children=[c2, x1])
    root = Node("+", children=[term1, term2])
    tree = Tree(root)

    x = np.array([
        [1.0, 3.0],
        [2.0, 4.0],
        [3.0, 5.0],
        [0.0, 0.0],
    ])
    # expected: x0^2 + 2*x1
    # row 0: 1^2 + 2*3 = 7
    # row 1: 2^2 + 2*4 = 12
    # row 2: 3^2 + 2*5 = 19
    # row 3: 0^2 + 2*0 = 0
    preds = tree.eval(x)
    assert preds.shape == (4,)
    assert preds.dtype == np.float64
    assert np.allclose(preds, [7.0, 12.0, 19.0, 0.0])


def test_tree_eval_clamping_and_nan_safety():
    """verifies tree output is clamped to valid range and replaces nans"""
    # constant node with value exceeding 1e10
    huge_const = Node("const", value=1e15)
    tree = Tree(huge_const)
    x = np.zeros((5, 2))
    preds = tree.eval(x)
    assert preds.shape == (5,)
    assert np.allclose(preds, 1e10)


def test_full_tree_exact_depth():
    """verifies that full tree generation strictly matches requested depth"""
    rng = np.random.default_rng(123)
    for target_d in [2, 3, 4, 5]:
        for _ in range(5):
            t = full_tree(depth=target_d, num_features=3, rng=rng)
            assert t.depth() == target_d


def test_grow_tree_bounded_depth():
    """verifies that grow tree generation does not exceed target depth"""
    rng = np.random.default_rng(456)
    for max_d in [2, 3, 4, 5]:
        for _ in range(10):
            t = grow_tree(depth=max_d, num_features=3, rng=rng)
            assert 2 <= t.depth() <= max_d


def test_ramped_half_and_half_structure():
    """verifies population size depth bounds and feature index constraints"""
    rng = np.random.default_rng(789)
    pop = ramped_half_and_half(pop_size=60, num_features=4, min_depth=2, max_depth=5, rng=rng)
    assert len(pop) == 60

    depths_observed = set()
    for ind in pop:
        d = ind.depth()
        assert 2 <= d <= 5
        depths_observed.add(d)
        for node in ind.root.get_all_nodes():
            if node.op == "var":
                assert 0 <= node.value < 4

    # all requested depths should be present in population
    assert depths_observed == {2, 3, 4, 5}


def test_compute_rmse():
    """verifies root mean squared error metric computation"""
    y_true = np.array([1.0, 2.0, 3.0, 4.0])
    y_pred = np.array([1.0, 2.0, 3.0, 4.0])
    assert compute_rmse(y_true, y_pred) == 0.0

    y_off = np.array([3.0, 4.0, 5.0, 6.0])
    assert np.isclose(compute_rmse(y_true, y_off), 2.0)
    assert np.isclose(compute_rmse(y_off, y_true), 2.0)

    # non-empty random check
    rnd_a = np.random.randn(100)
    rnd_b = np.random.randn(100)
    assert compute_rmse(rnd_a, rnd_b) >= 0.0


def test_synth1_ground_truth_rmse():
    """verifies synth1 ground truth polynomial achieves rmse below 1e-12"""
    x_tr, y_tr, x_te, y_te = load_dataset("synth1", base_dir=repo_root)

    # formula: y = x0^4 - x0^3 + 0.5 * x1^2 - x1
    # x0^2
    x0_sq = Node("*", children=[Node("var", value=0), Node("var", value=0)])
    # x0^4 = x0^2 * x0^2
    x0_pow4 = Node("*", children=[x0_sq, x0_sq.copy()])
    # x0^3 = x0 * x0^2
    x0_pow3 = Node("*", children=[Node("var", value=0), x0_sq.copy()])
    # term1 = x0^4 - x0^3
    term1 = Node("-", children=[x0_pow4, x0_pow3])

    # 0.5 * x1^2
    x1_sq = Node("*", children=[Node("var", value=1), Node("var", value=1)])
    term2 = Node("*", children=[Node("const", value=0.5), x1_sq])

    # poly_sum = term1 + term2
    poly_sum = Node("+", children=[term1, term2])

    # ground truth root = poly_sum - x1
    gt_root = Node("-", children=[poly_sum, Node("var", value=1)])
    gt_tree = Tree(gt_root)

    # train evaluation
    preds_tr = gt_tree.eval(x_tr)
    rmse_tr = compute_rmse(y_tr, preds_tr)
    assert rmse_tr < 1e-12

    # test evaluation
    preds_te = gt_tree.eval(x_te)
    rmse_te = compute_rmse(y_te, preds_te)
    assert rmse_te < 1e-12
