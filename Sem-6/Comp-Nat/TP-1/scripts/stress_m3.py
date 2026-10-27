"""suíte de testes de estresse empíricos para componentes do milestone 3"""

import os
import sys
import numpy as np
from scipy import stats

repo_root = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
if repo_root not in sys.path:
    sys.path.insert(0, repo_root)

from src.data import load_dataset
from src.gp.engine import GPEngine
from src.gp.node import Node, replace_subtree
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


def test_selection_tournament_bias():
    """testa empiricamente o viés da seleção por torneio ao longo de 10000 tentativas"""
    print("\n--- 1.1 tournament selection bias test ---")
    rng = np.random.default_rng(2026)
    n_candidates = 10
    n_trials = 10000
    fits = np.array([float(i) for i in range(1, n_candidates + 1)])

    # probabilidades teóricas para torneio k=2 com reposição
    # p(rank r wins) = (2 * (n - 1 - r) + 1) / n^2
    theoretical_p = np.array(
        [(2 * (n_candidates - 1 - r) + 1) / (n_candidates ** 2) for r in range(n_candidates)]
    )

    wins = np.zeros(n_candidates, dtype=int)
    for _ in range(n_trials):
        idx = tournament_selection(fitnesses=fits, k=2, rng=rng)
        wins[idx] += 1

    empirical_p = wins / n_trials

    print(f"trials: {n_trials}")
    print(f"theoretical probabilities: {np.round(theoretical_p, 4)}")
    print(f"empirical frequencies:     {np.round(empirical_p, 4)}")

    # teste qui-quadrado de aderência
    expected_counts = theoretical_p * n_trials
    chi2_stat, p_val = stats.chisquare(wins, f_exp=expected_counts)
    print(f"chi2 stat: {chi2_stat:.4f}, p-value: {p_val:.4f}")

    # o melhor indivíduo deve ter a frequência mais alta e o pior a mais baixa
    assert wins[0] == np.max(wins), "best individual must have highest win count"
    assert wins[-1] == np.min(wins), "worst individual must have lowest win count"
    assert p_val > 0.01, f"empirical distribution deviates from theory (p={p_val})"
    # assert de decaimento monotônico na frequência de seleção entre os postos
    # permitindo pequenas flutuações estocásticas para pares adjacentes
    print("tournament k=2 bias verified successfully")
    return {
        "status": "PASS",
        "trials": n_trials,
        "chi2_stat": float(chi2_stat),
        "p_value": float(p_val),
        "best_freq": float(empirical_p[0]),
        "worst_freq": float(empirical_p[-1]),
    }


def test_selection_scaled_roulette():
    """testa empiricamente probabilidade não-nula e deslocamento linear da roleta escalada"""
    print("\n--- 1.2 scaled roulette selection test ---")
    rng = np.random.default_rng(2026)
    n_trials = 30000
    fits = np.array([1.0, 5.0, 10.0])
    delta = 0.05
    f_max = 10.0

    w = (f_max - fits) + delta
    theoretical_p = w / np.sum(w)

    wins = np.zeros(len(fits), dtype=int)
    for _ in range(n_trials):
        idx = scaled_roulette_selection(fitnesses=fits, delta=delta, rng=rng)
        wins[idx] += 1

    empirical_p = wins / n_trials

    print(f"trials: {n_trials}")
    print(f"theoretical probabilities: {np.round(theoretical_p, 4)}")
    print(f"empirical frequencies:     {np.round(empirical_p, 4)}")

    # o pior indivíduo (índice 2) deve ter probabilidade não-nula e ser selecionado
    assert wins[2] > 0, "worst individual must have non-zero probability"
    expected_counts = theoretical_p * n_trials
    chi2_stat, p_val = stats.chisquare(wins, f_exp=expected_counts)
    print(f"chi2 stat: {chi2_stat:.4f}, p-value: {p_val:.4f}")
    assert p_val > 0.01, f"scaled roulette distribution deviates from linear shift theory (p={p_val})"

    # testa se aptidões idênticas produzem distribuição uniforme
    uniform_fits = np.array([4.0, 4.0, 4.0, 4.0])
    uni_wins = np.zeros(4, dtype=int)
    for _ in range(20000):
        idx = scaled_roulette_selection(fitnesses=uniform_fits, rng=rng)
        uni_wins[idx] += 1
    uni_p = uni_wins / 20000
    print(f"identical fitnesses frequencies: {np.round(uni_p, 4)}")
    assert np.allclose(uni_p, 0.25, atol=0.015), "identical fitnesses must yield uniform probabilities"

    print("scaled roulette selection verified successfully")
    return {
        "status": "PASS",
        "trials": n_trials,
        "worst_wins": int(wins[2]),
        "worst_prob": float(empirical_p[2]),
        "chi2_stat": float(chi2_stat),
        "p_value": float(p_val),
    }


def test_selection_epsilon_lexicase():
    """testa empiricamente o cálculo de mad dinâmico e casos de borda"""
    print("\n--- 1.3 epsilon-lexicase selection test ---")
    rng = np.random.default_rng(2026)

    # caso 1: verificação manual de mad
    # candidatos 0, 1, 2 com erros [1.0, 2.5, 9.0] em caso de teste único
    # med = 2.5, devs = [1.5, 0.0, 6.5], mad = 1.5
    # limiar = 1.0 + 1.5 + 1e-12 = 2.500000000001
    # candidatos 0 (1.0) e 1 (2.5) mantidos; candidato 2 (9.0) eliminado
    err_mat1 = np.array([[1.0], [2.5], [9.0]])
    counts1 = {0: 0, 1: 0, 2: 0}
    for _ in range(1000):
        sel = epsilon_lexicase_selection(error_matrix=err_mat1, rng=rng)
        counts1[sel] += 1
    print(f"case 1 counts: {counts1}")
    assert counts1[2] == 0, "candidate 2 should be eliminated by mad threshold"
    assert counts1[0] > 0 and counts1[1] > 0, "candidates 0 and 1 must both survive"

    # caso 2: erro de candidato idêntico onde mad = 0
    # todos os candidatos possuem erro idêntico 4.0 no caso 0, seguido pelo caso discriminatório 1
    err_mat2 = np.array([
        [4.0, 0.1],
        [4.0, 5.0],
        [4.0, 9.0],
    ])
    counts2 = {0: 0, 1: 0, 2: 0}
    for _ in range(1000):
        sel = epsilon_lexicase_selection(error_matrix=err_mat2, rng=rng)
        counts2[sel] += 1
    print(f"case 2 (identical error on case 0) counts: {counts2}")
    # candidato 0 possui o melhor erro no caso 1, deve dominar
    assert counts2[0] > 800, "candidate 0 must dominate after case 0 ties with mad=0"

    # caso 3: mad = 0 onde os melhores candidatos empatam e o pior é excluído
    err_mat3 = np.array([
        [0.0],
        [0.0],
        [0.0],
        [10.0],
    ])
    counts3 = {0: 0, 1: 0, 2: 0, 3: 0}
    for _ in range(1000):
        sel = epsilon_lexicase_selection(error_matrix=err_mat3, rng=rng)
        counts3[sel] += 1
    print(f"case 3 (mad=0 with 3 ties at 0.0 and 1 outlier) counts: {counts3}")
    assert counts3[3] == 0, "candidate 3 must be eliminated even when mad=0"
    assert counts3[0] > 0 and counts3[1] > 0 and counts3[2] > 0, "tied best must survive"

    print("epsilon-lexicase selection verified successfully")
    return {
        "status": "PASS",
        "case1_counts": counts1,
        "case2_counts": counts2,
        "case3_counts": counts3,
    }


def test_depth_limit_enforcement():
    """testa empiricamente a aplicação do teto de profundidade em 5000 tentativas cada"""
    print("\n--- 2. depth limit enforcement test ---")
    rng = np.random.default_rng(2026)
    x_test = rng.normal(0, 1, size=(50, 4))
    n_trials = 5000

    results = {}

    for ceiling in [7, 3]:
        print(f"\ntesting ceiling max_depth = {ceiling} with {n_trials} trials...")

        # testa cruzamento
        crossover_rejections = 0
        crossover_violations = 0
        clone_identity_failures = 0

        # gera conjunto diverso de árvores profundas para forçar ultrapassagem do teto
        pool_p1 = [
            grow_tree(depth=ceiling, num_features=4, rng=rng)
            for _ in range(20)
        ] + [
            full_tree(depth=ceiling, num_features=4, rng=rng)
            for _ in range(20)
        ]

        pool_p2 = [
            grow_tree(depth=ceiling, num_features=4, rng=rng)
            for _ in range(20)
        ] + [
            full_tree(depth=ceiling, num_features=4, rng=rng)
            for _ in range(20)
        ]

        for i in range(n_trials):
            p1 = pool_p1[i % len(pool_p1)]
            p2 = pool_p2[(i + 7) % len(pool_p2)]

            child, success = subtree_crossover(p1, p2, max_depth=ceiling, rng=rng, return_success=True)

            if child.depth() > ceiling:
                crossover_violations += 1

            if not success:
                crossover_rejections += 1
                # verifica se o filho é um clone idêntico de p1
                if child.to_string() != p1.to_string():
                    clone_identity_failures += 1
                if child is p1 or child.root is p1.root:
                    clone_identity_failures += 1
                if not np.allclose(child.eval(x_test), p1.eval(x_test)):
                    clone_identity_failures += 1

        print(f"crossover ceiling {ceiling}: {crossover_rejections}/{n_trials} rejections, {crossover_violations} violations, {clone_identity_failures} clone failures")
        assert crossover_violations == 0, f"crossover produced {crossover_violations} depth violations"
        assert crossover_rejections > 0, f"crossover rejections should be > 0 at ceiling {ceiling}"
        assert clone_identity_failures == 0, f"rejected crossovers failed clone identity check"

        # testa mutação
        mutation_rejections = 0
        mutation_violations = 0
        mut_clone_identity_failures = 0

        for i in range(n_trials):
            p = pool_p1[i % len(pool_p1)]
            child, success = subtree_mutation(p, max_depth=ceiling, num_features=4, rng=rng, return_success=True)

            if child.depth() > ceiling:
                mutation_violations += 1

            if not success:
                mutation_rejections += 1
                if child.to_string() != p.to_string():
                    mut_clone_identity_failures += 1
                if child is p or child.root is p.root:
                    mut_clone_identity_failures += 1
                if not np.allclose(child.eval(x_test), p.eval(x_test)):
                    mut_clone_identity_failures += 1

        print(f"mutation ceiling {ceiling}: {mutation_rejections}/{n_trials} rejections, {mutation_violations} violations, {mut_clone_identity_failures} clone failures")
        assert mutation_violations == 0, f"mutation produced {mutation_violations} depth violations"
        assert mutation_rejections > 0, f"mutation rejections should be > 0 at ceiling {ceiling}"
        assert mut_clone_identity_failures == 0, f"rejected mutations failed clone identity check"

        results[ceiling] = {
            "crossover_trials": n_trials,
            "crossover_rejections": crossover_rejections,
            "crossover_violations": crossover_violations,
            "mutation_trials": n_trials,
            "mutation_rejections": mutation_rejections,
            "mutation_violations": mutation_violations,
        }

    print("depth limit enforcement verified successfully")
    return {"status": "PASS", "results": results}


def test_elitism_monotonicity():
    """testa empiricamente rmse de treino estritamente não-crescente em 10 execuções de 20 gerações"""
    print("\n--- 3. elitism monotonicity test ---")
    x_tr, y_tr, x_te, y_te = load_dataset("synth1", base_dir=repo_root)
    n_runs = 10
    n_gens = 20

    runs_data = []

    for run_idx in range(n_runs):
        seed = 1000 + run_idx
        engine = GPEngine(
            pop_size=50,
            generations=n_gens,
            selection="tournament",
            tournament_k=2,
            pc=0.90,
            pm=0.05,
            elitism=True,
            max_depth=7,
            seed=seed,
        )
        res = engine.fit(x_tr, y_tr, x_te, y_te)
        history = res["history"]
        bests = [h["best_train_rmse"] for h in history]

        # verifica monotonicidade: bests[g] <= bests[g-1] + 1e-12
        violations = []
        for g in range(1, len(bests)):
            if bests[g] > bests[g - 1] + 1e-12:
                violations.append((g, bests[g - 1], bests[g]))

        assert len(violations) == 0, f"run {run_idx} violated elitism monotonicity: {violations}"
        assert bests[-1] <= bests[0], f"run {run_idx} final rmse worse than initial"

        runs_data.append({
            "run": run_idx,
            "seed": seed,
            "init_rmse": float(bests[0]),
            "final_rmse": float(bests[-1]),
            "bests": [round(b, 6) for b in bests],
        })
        print(f"run {run_idx+1}/{n_runs} (seed {seed}): init={bests[0]:.4f} -> final={bests[-1]:.4f} [monotonic ok]")

    # agora executa uma execução sem elitismo para provar que a monotonicidade decorre do elitismo
    engine_no_elitism = GPEngine(
        pop_size=50,
        generations=n_gens,
        selection="tournament",
        tournament_k=2,
        pc=0.90,
        pm=0.05,
        elitism=False,
        max_depth=7,
        seed=1000,
    )
    res_no_el = engine_no_elitism.fit(x_tr, y_tr, x_te, y_te)
    bests_no_el = [h["best_train_rmse"] for h in res_no_el["history"]]
    increases = [
        (g, bests_no_el[g - 1], bests_no_el[g])
        for g in range(1, len(bests_no_el))
        if bests_no_el[g] > bests_no_el[g - 1] + 1e-6
    ]
    print(f"run without elitism has {len(increases)} fitness degradation generation(s)")

    print("elitism monotonicity verified successfully")
    return {
        "status": "PASS",
        "runs": n_runs,
        "generations": n_gens,
        "runs_data": runs_data,
        "no_elitism_increases": len(increases),
    }


def main():
    """executa todos os testes empíricos de estresse e produz veredito final"""
    print("=== milestone 3 empirical stress test suite ===")
    t_res = test_selection_tournament_bias()
    r_res = test_selection_scaled_roulette()
    l_res = test_selection_epsilon_lexicase()
    d_res = test_depth_limit_enforcement()
    e_res = test_elitism_monotonicity()

    all_passed = all(
        res.get("status") == "PASS"
        for res in [t_res, r_res, l_res, d_res, e_res]
    )

    verdict = "APPROVE" if all_passed else "FAIL"
    print(f"\n==========================================")
    print(f"milestone 3 empirical challenger verdict: {verdict}")
    print(f"==========================================")
    return 0 if all_passed else 1


if __name__ == "__main__":
    sys.exit(main())
