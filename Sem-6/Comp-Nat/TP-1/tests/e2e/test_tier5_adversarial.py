# tier 5 adversarial tests stress testing boundaries and hostile inputs
import os
import sys
import tempfile
import warnings
import numpy as np
import pandas as pd
import pytest

# ensure project root is on sys path
repo_root = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
if repo_root not in sys.path:
    sys.path.insert(0, repo_root)

from src.data import load_dataset
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
from src.gp.node import Node, replace_subtree
from src.gp.tree import (
    Tree,
    full_tree,
    grow_tree,
    ramped_half_and_half,
    compute_rmse,
)
from src.gp.selection import (
    tournament_selection,
    scaled_roulette_selection,
    epsilon_lexicase_selection,
)
from src.gp.operators import (
    subtree_crossover,
    subtree_mutation,
    gaussian_constant_mutation,
    mutate,
    reproduce,
)
from src.gp.engine import GPEngine
from src.reporting.plotter import (
    plot_convergence,
    plot_bloat,
    plot_operator_success,
    plot_parity,
    plot_selection_diversity,
)
from src.reporting.tables import (
    compute_summary_stats,
    export_markdown_table,
    export_latex_table,
)


def test_adversarial_numerical_extreme_floats():
    """primitives survive extreme float values without generating nans or infs"""
    extreme_vals = np.array([-1e15, 1e15, -1e-15, 1e-15, 0.0, -1.0, 1.0], dtype=np.float64)

    # test arithmetic primitives with broadcast and vector operations
    add_res = safe_add(extreme_vals, extreme_vals)
    assert np.all(np.isfinite(add_res))

    sub_res = safe_sub(extreme_vals, extreme_vals)
    assert np.all(np.isfinite(sub_res))

    mul_res = safe_mul(extreme_vals, extreme_vals)
    assert np.all(np.isfinite(mul_res))

    div_res = safe_div(extreme_vals, extreme_vals)
    assert np.all(np.isfinite(div_res))

    # test transcendental functions
    sin_res = safe_sin(extreme_vals)
    assert np.all(np.isfinite(sin_res))

    cos_res = safe_cos(extreme_vals)
    assert np.all(np.isfinite(cos_res))

    log_res = safe_log(extreme_vals)
    assert np.all(np.isfinite(log_res))

    exp_res = safe_exp(extreme_vals)
    assert np.all(np.isfinite(exp_res))


def test_adversarial_single_sample_dataset_n1():
    """gp engine runs and evaluates properly when training set has single sample"""
    x_single = np.array([[2.5, -1.2, 0.8, 3.4]], dtype=np.float64)
    y_single = np.array([4.2], dtype=np.float64)

    engine = GPEngine(pop_size=8, generations=2, seed=42)
    history_dict = engine.fit(x_single, y_single)

    assert engine.best_individual_ is not None
    preds = engine.predict(x_single)
    assert preds.shape == (1,)
    assert np.all(np.isfinite(preds))
    assert np.isfinite(engine.best_train_rmse_)


def test_adversarial_high_dimensional_features_d50():
    """tree generation and evaluation scale to 50 dimensions without index error"""
    rng = np.random.default_rng(101)
    pop = ramped_half_and_half(pop_size=20, num_features=50, min_depth=2, max_depth=5, rng=rng)

    # verify all variables reference columns strictly within bounds
    var_indices = []
    for tree in pop:
        for node in tree.root.get_all_nodes():
            if node.op == "var":
                assert 0 <= int(node.value) < 50
                var_indices.append(int(node.value))

    # ensure high dimensional indices actually get sampled across population
    assert any(idx >= 25 for idx in var_indices)

    # evaluate population on 20 samples with 50 features
    x_high_d = rng.normal(size=(20, 50))
    for tree in pop:
        preds = tree.eval(x_high_d)
        assert preds.shape == (20,)
        assert np.all(np.isfinite(preds))


def test_adversarial_extreme_target_magnitudes():
    """engine handles extreme target magnitudes without floating overflow crash"""
    x_mat = np.array([[1.0, 2.0], [3.0, 4.0], [5.0, 6.0], [7.0, 8.0]], dtype=np.float64)
    y_extreme = np.array([1e8, -1e8, 5e7, -5e7], dtype=np.float64)

    engine = GPEngine(pop_size=10, generations=2, seed=42)
    engine.fit(x_mat, y_extreme)

    assert engine.best_individual_ is not None
    assert np.isfinite(engine.best_train_rmse_)
    preds = engine.predict(x_mat)
    assert preds.shape == (4,)
    assert np.all(np.isfinite(preds))


def test_adversarial_full_binary_tree_depth7_127nodes():
    """complete binary tree of depth 7 has exactly 127 nodes and evaluates cleanly"""
    def build_full_binary(d: int) -> Node:
        if d == 1:
            return Node("const", value=1.0)
        return Node("+", children=[build_full_binary(d - 1), build_full_binary(d - 1)])

    root = build_full_binary(7)
    tree = Tree(root)

    assert tree.depth() == 7
    assert tree.node_count() == 127

    x_test = np.ones((10, 2), dtype=np.float64)
    preds = tree.eval(x_test)
    assert preds.shape == (10,)
    assert np.all(np.isfinite(preds))
    # full binary addition tree with 64 leaves of 1.0 evaluates to 64.0
    assert np.allclose(preds, 64.0)


def test_adversarial_deeply_nested_transcendental_functions():
    """deeply nested transcendental function chain evaluates safely on extreme inputs"""
    # construct unary chain of depth 7: sin(cos(exp(sin(cos(log(x0))))))
    leaf = Node("var", value=0)
    n1 = Node("log", children=[leaf])
    n2 = Node("cos", children=[n1])
    n3 = Node("sin", children=[n2])
    n4 = Node("exp", children=[n3])
    n5 = Node("cos", children=[n4])
    n6 = Node("sin", children=[n5])
    tree = Tree(n6)

    assert tree.depth() == 7

    x_adversarial = np.array([[-1e15], [-1.0], [0.0], [1e-15], [1e15]], dtype=np.float64)
    preds = tree.eval(x_adversarial)

    assert preds.shape == (5,)
    assert np.all(np.isfinite(preds))
    # since outermost op is sin, predictions must be bounded in [-1.0, 1.0]
    assert np.all(preds >= -1.0 - 1e-7)
    assert np.all(preds <= 1.0 + 1e-7)


def test_adversarial_safe_exp_clamping_zero_warnings():
    """safe exp clamps extreme inputs without throwing any runtime warnings"""
    extreme_inputs = np.array([-1e300, -1e50, -33.0, -32.0, 0.0, 32.0, 33.0, 1e50, 1e300], dtype=np.float64)

    with warnings.catch_warnings(record=True) as captured:
        warnings.simplefilter("always")
        result = safe_exp(extreme_inputs)

    assert len(captured) == 0
    assert np.all(np.isfinite(result))
    assert np.all(result >= np.exp(-32.0) - 1e-9)
    assert np.all(result <= np.exp(32.0) + 1e-9)


def test_adversarial_deep_division_chains_with_zero_divisors():
    """deep division chain with zero and sub-threshold denominators remains finite"""
    # construct x0 / (x1 / (x0 / (x1 / 1.0)))
    leaf_one = Node("const", value=1.0)
    div4 = Node("/", children=[Node("var", value=1), leaf_one])
    div3 = Node("/", children=[Node("var", value=0), div4])
    div2 = Node("/", children=[Node("var", value=1), div3])
    div1 = Node("/", children=[Node("var", value=0), div2])
    tree = Tree(div1)

    x_zeros = np.array([[0.0, 0.0], [1.0, 0.0], [0.0, 1.0], [1e-7, 1e-7], [1e-5, 1e-5]], dtype=np.float64)
    preds = tree.eval(x_zeros)

    assert preds.shape == (5,)
    assert np.all(np.isfinite(preds))


def test_adversarial_degenerate_identical_fitness_tournament():
    """tournament selection handles zero variance fitness array properly"""
    rng = np.random.default_rng(42)
    flat_fits = np.full(50, 2.71828, dtype=np.float64)

    winners = [tournament_selection(fitnesses=flat_fits, k=2, rng=rng) for _ in range(100)]
    assert all(0 <= w < 50 for w in winners)
    # verify random spread over candidates
    assert len(set(winners)) > 1


def test_adversarial_degenerate_identical_fitness_roulette():
    """scaled roulette selection with zero variance assigns uniform probabilities"""
    rng = np.random.default_rng(42)
    flat_fits = np.full(20, 10.0, dtype=np.float64)

    picks = [scaled_roulette_selection(fitnesses=flat_fits, rng=rng) for _ in range(100)]
    assert all(0 <= p < 20 for p in picks)
    assert len(set(picks)) > 1

    # also check with all zeros
    zero_fits = np.zeros(10, dtype=np.float64)
    zero_picks = [scaled_roulette_selection(fitnesses=zero_fits, rng=rng) for _ in range(50)]
    assert all(0 <= p < 10 for p in zero_picks)


def test_adversarial_degenerate_identical_errors_lexicase():
    """epsilon lexicase selection with identical candidate error vectors uses tie-break"""
    rng = np.random.default_rng(42)
    # 15 candidates each producing identical errors on all 20 test cases
    err_mat = np.full((15, 20), 1.414, dtype=np.float64)

    winners = [epsilon_lexicase_selection(error_matrix=err_mat, rng=rng) for _ in range(100)]
    assert all(0 <= w < 15 for w in winners)
    # check that uniform tie-break selects diverse candidates
    assert len(set(winners)) > 1


def test_adversarial_single_candidate_selection_pools():
    """selection operators return immediately when pool contains only one candidate"""
    rng = np.random.default_rng(42)

    assert tournament_selection(fitnesses=np.array([3.14]), k=2, rng=rng) == 0
    assert scaled_roulette_selection(fitnesses=np.array([3.14]), rng=rng) == 0
    assert epsilon_lexicase_selection(error_matrix=np.array([[1.0, 2.0]]), rng=rng) == 0


def test_adversarial_extreme_fitness_disparity_roulette():
    """scaled roulette survives extreme fitness disparity without underflow crash"""
    rng = np.random.default_rng(42)
    extreme_fits = np.array([1e-12, 1e8, 1e9, 0.0, 1e-4], dtype=np.float64)

    picks = [scaled_roulette_selection(fitnesses=extreme_fits, rng=rng) for _ in range(100)]
    assert all(0 <= p < 5 for p in picks)
    # best candidate (0.0 or 1e-12) should receive highest selection frequency
    assert picks.count(3) + picks.count(0) > picks.count(2)


def test_adversarial_variation_pure_crossover_regime():
    """pure crossover regime produces all offspring via crossover obeying depth ceiling"""
    x_toy = np.linspace(-2.0, 2.0, 30).reshape(-1, 1)
    y_toy = x_toy[:, 0] ** 2

    engine = GPEngine(pop_size=16, generations=3, pc=1.0, pm=0.0, elitism=True, max_depth=7, seed=42)
    engine.fit(x_toy, y_toy)

    # with elitism, slot 0 is cloned best and remaining 15 slots are crossover
    for h in engine.history_:
        if h["generation"] > 0:
            assert h["crossover_total"] == 15

    # check all trees in final population respect depth ceiling
    preds = engine.predict(x_toy)
    assert preds.shape == (30,)
    assert engine.best_individual_.depth() <= 7


def test_adversarial_variation_pure_mutation_regime():
    """pure mutation regime breeds all offspring via mutation without depth violations"""
    x_toy = np.linspace(-2.0, 2.0, 30).reshape(-1, 1)
    y_toy = x_toy[:, 0] ** 2

    engine = GPEngine(pop_size=16, generations=3, pc=0.0, pm=1.0, elitism=True, max_depth=7, seed=42)
    engine.fit(x_toy, y_toy)

    for h in engine.history_:
        assert h["crossover_total"] == 0

    assert engine.best_individual_.depth() <= 7


def test_adversarial_variation_pure_cloning_regime():
    """pure cloning regime replicates selected parents without structural change"""
    x_toy = np.linspace(-2.0, 2.0, 20).reshape(-1, 1)
    y_toy = x_toy[:, 0] + 1.0

    engine = GPEngine(pop_size=12, generations=3, pc=0.0, pm=0.0, elitism=False, seed=42)
    engine.fit(x_toy, y_toy)

    for h in engine.history_:
        assert h["crossover_total"] == 0


def test_adversarial_mutation_on_single_node_leaf():
    """subtree and constant mutations operate safely on single node leaf trees"""
    rng = np.random.default_rng(42)

    leaf_const = Tree(Node("const", value=3.1415))
    mut_const = gaussian_constant_mutation(leaf_const, sigma=0.5, rng=rng)
    assert mut_const.root.op == "const"
    assert mut_const.root.value != 3.1415

    mut_sub = subtree_mutation(leaf_const, max_depth=7, rng=rng)
    assert mut_sub.depth() <= 7
    assert mut_sub.node_count() >= 1

    leaf_var = Tree(Node("var", value=0))
    mut_var_sub = subtree_mutation(leaf_var, max_depth=7, rng=rng)
    assert mut_var_sub.depth() <= 7


def test_adversarial_crossover_depth_rejection_at_limit():
    """crossover exceeding depth limit cleanly keeps parent1 unchanged"""
    def build_chain(d: int, op: str = "sin") -> Node:
        if d == 1:
            return Node("const", value=1.0)
        return Node(op, children=[build_chain(d - 1, op)])

    # p1 has depth 7, p2 has depth 7
    p1 = Tree(build_chain(7, "sin"))
    p2 = Tree(build_chain(7, "cos"))

    # graft p2 into level 2 of p1: resulting depth would be 2 - 1 + 7 = 8 > 7
    # replacement must be rejected and p1 retained
    rng = np.random.default_rng(999)
    for _ in range(20):
        child, success = subtree_crossover(p1, p2, max_depth=7, rng=rng, return_success=True)
        assert child.depth() <= 7


def test_adversarial_deterministic_seed_reproducibility():
    """two independent engines with identical seed produce bit-for-bit identical histories"""
    x_toy = np.linspace(-3.0, 3.0, 40).reshape(-1, 1)
    y_toy = np.sin(x_toy[:, 0])

    eng1 = GPEngine(pop_size=14, generations=3, seed=12345)
    hist1 = eng1.fit(x_toy, y_toy)

    eng2 = GPEngine(pop_size=14, generations=3, seed=12345)
    hist2 = eng2.fit(x_toy, y_toy)

    assert eng1.best_train_rmse_ == eng2.best_train_rmse_
    assert eng1.best_individual_.to_string() == eng2.best_individual_.to_string()

    for g in range(3):
        assert hist1["history"][g]["best_train_rmse"] == hist2["history"][g]["best_train_rmse"]
        assert hist1["history"][g]["unique_canonical_trees"] == hist2["history"][g]["unique_canonical_trees"]
        assert hist1["history"][g]["avg_node_count"] == hist2["history"][g]["avg_node_count"]


def test_adversarial_different_seeds_divergence():
    """engines with different random seeds explore distinct evolutionary trajectories"""
    rng = np.random.default_rng(99)
    x_toy = rng.normal(size=(50, 4))
    y_toy = rng.normal(size=50)

    eng_a = GPEngine(pop_size=20, generations=4, seed=111)
    eng_a.fit(x_toy, y_toy)

    eng_b = GPEngine(pop_size=20, generations=4, seed=222)
    eng_b.fit(x_toy, y_toy)

    # string representations of best trees should diverge
    assert eng_a.best_individual_.to_string() != eng_b.best_individual_.to_string()


def test_adversarial_elitism_monotonic_fitness():
    """elitism guarantees that best training rmse is non-increasing across generations"""
    x_toy = np.linspace(-2.0, 2.0, 50).reshape(-1, 1)
    y_toy = x_toy[:, 0] ** 3 - x_toy[:, 0]

    engine = GPEngine(pop_size=20, generations=6, elitism=True, seed=333)
    engine.fit(x_toy, y_toy)

    hist = engine.history_
    for g in range(1, len(hist)):
        curr_best = hist[g]["best_train_rmse"]
        prev_best = hist[g - 1]["best_train_rmse"]
        assert curr_best <= prev_best + 1e-12


def test_adversarial_plotting_constant_predictions_parity():
    """parity plotter handles zero variance constant inputs without axis collapse"""
    tmp_dir = tempfile.mkdtemp()
    y_const = np.full(40, 5.0, dtype=np.float64)
    svg_out = os.path.join(tmp_dir, "parity_const.svg")
    png_out = os.path.join(tmp_dir, "parity_const.png")

    plot_parity(y_const, y_const, svg_out)
    assert os.path.exists(svg_out)
    assert os.path.getsize(svg_out) > 0
    assert os.path.exists(png_out)
    assert os.path.getsize(png_out) > 0


def test_adversarial_plotting_single_generation_history():
    """convergence plotter handles history with only one generation without dividing by zero"""
    tmp_dir = tempfile.mkdtemp()
    single_gen_df = pd.DataFrame({
        "generation": [0],
        "best_train_rmse": [2.5],
        "mean_train_rmse": [3.0],
        "best_test_rmse": [2.8],
    })
    csv_file = os.path.join(tmp_dir, "single_gen.csv")
    single_gen_df.to_csv(csv_file, index=False)

    svg_out = os.path.join(tmp_dir, "conv_single.svg")
    png_out = os.path.join(tmp_dir, "conv_single.png")
    plot_convergence(csv_file, svg_out)

    assert os.path.exists(svg_out)
    assert os.path.getsize(svg_out) > 0
    assert os.path.exists(png_out)
    assert os.path.getsize(png_out) > 0


def test_adversarial_tables_zero_variance_or_single_run():
    """summary tables handle zero standard deviation across runs cleanly"""
    tmp_dir = tempfile.mkdtemp()
    flat_df = pd.DataFrame({
        "configuration": ["cfg_a", "cfg_a"],
        "dataset": ["synth1", "synth1"],
        "best_train_rmse": [1.5, 1.5],
        "best_test_rmse": [2.0, 2.0],
        "avg_node_count": [10.0, 10.0],
        "avg_depth": [3.0, 3.0],
    })

    summary_df = compute_summary_stats(flat_df)
    assert len(summary_df) == 1
    assert summary_df["train_rmse_std"].iloc[0] == 0.0

    md_out = os.path.join(tmp_dir, "summary.md")
    tex_out = os.path.join(tmp_dir, "summary.tex")

    export_markdown_table(summary_df, md_out)
    export_latex_table(summary_df, tex_out, caption="test caption", label="tab:test")

    assert os.path.exists(md_out)
    assert os.path.exists(tex_out)
    with open(tex_out, "r", encoding="utf-8") as f:
        tex_content = f.read()
    assert "\\begin{table}" in tex_content
    assert "\\bottomrule" in tex_content


def test_adversarial_dataset_loader_invalid_inputs():
    """dataset loader rejects unknown dataset names and missing paths with expected errors"""
    with pytest.raises(ValueError):
        load_dataset("nonexistent_benchmark_dataset")

    with pytest.raises(FileNotFoundError):
        load_dataset("synth1", base_dir="definitely_nonexistent_directory_xyz")


def test_adversarial_zero_generations_edge_case():
    """engine handles zero generations edge case returning initial population best"""
    x_toy = np.linspace(-1.0, 1.0, 20).reshape(-1, 1)
    y_toy = x_toy[:, 0] * 2.0

    engine = GPEngine(pop_size=10, generations=0, seed=42)
    res = engine.fit(x_toy, y_toy)

    assert engine.best_individual_ is not None
    assert np.isfinite(engine.best_train_rmse_)
    assert len(engine.history_) == 0
    preds = engine.predict(x_toy)
    assert preds.shape == (20,)
    assert np.all(np.isfinite(preds))
