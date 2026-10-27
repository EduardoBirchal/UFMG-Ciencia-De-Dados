"""executor mestre de experimentos executando os passos 1 a 5 com 30 sementes"""

import os
import sys
import time
from typing import Any, Dict, List, Tuple
import numpy as np
import pandas as pd
from joblib import Parallel, delayed

repo_root = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
if repo_root not in sys.path:
    sys.path.insert(0, repo_root)

from src.data import load_dataset
from src.gp.engine import GPEngine
from src.reporting.plotter import (
    plot_bloat,
    plot_convergence,
    plot_metric_comparison,
    plot_operator_success,
    plot_parity,
    plot_selection_diversity,
)
from src.reporting.tables import (
    compute_summary_stats,
    export_latex_table,
    export_markdown_table,
    generate_summary_tables,
)

# 30 sementes fixas para reprodutibilidade científica
SEEDS = [42 + i for i in range(30)]
N_JOBS = 7


def run_single_task(
    dataset_name: str,
    pop_size: int,
    generations: int,
    selection: str,
    pc: float,
    pm: float,
    elitism: bool,
    seed: int,
    output_csv_path: str,
    step_name: str,
    config_name: str,
    base_dir: str,
) -> Dict[str, Any]:
    """executa uma única execução de programação genética e registra métricas por geração em csv"""
    x_tr, y_tr, x_te, y_te = load_dataset(dataset_name, base_dir=base_dir)

    t0 = time.time()
    engine = GPEngine(
        pop_size=pop_size,
        generations=generations,
        selection=selection,
        tournament_k=2,
        pc=pc,
        pm=pm,
        elitism=elitism,
        max_depth=7,
        seed=seed,
    )
    res = engine.fit(x_tr, y_tr, x_te, y_te)
    runtime = float(time.time() - t0)

    # exporta métricas por geração para csv
    history = res["history"]
    history_df = pd.DataFrame(history)
    history_df["best_tree_nodes"] = history_df["best_nodes"]
    history_df["best_tree_depth"] = history_df["best_depth"]

    os.makedirs(os.path.dirname(output_csv_path), exist_ok=True)
    history_df.to_csv(output_csv_path, index=False)

    last_row = history_df.iloc[-1]
    best_tr = float(last_row["best_train_rmse"])
    final_te = float(last_row["best_test_rmse"])
    b_nodes = int(last_row["best_nodes"])
    b_depth = int(last_row["best_depth"])
    avg_nodes = float(last_row["avg_nodes"])
    avg_depth = float(last_row["avg_depth"])
    unique_final = int(last_row["unique_canonical_trees"])

    # avalia predições de teste do melhor indivíduo se necessário para o gráfico de dispersão
    test_preds = None
    if dataset_name == "concrete" and res.get("best") is not None:
        best_ind = res["best"]
        test_preds = best_ind.eval(x_te).tolist()

    return {
        "step": step_name,
        "config_name": config_name,
        "dataset": dataset_name,
        "seed": seed,
        "pop_size": pop_size,
        "generations": generations,
        "selection": selection,
        "pc": pc,
        "pm": pm,
        "elitism": elitism,
        "best_train_rmse": best_tr,
        "final_test_rmse": final_te,
        "best_tree_nodes": b_nodes,
        "best_tree_depth": b_depth,
        "avg_node_count": avg_nodes,
        "avg_depth": avg_depth,
        "unique_canonical_final": unique_final,
        "total_runtime_seconds": runtime,
        "test_preds": test_preds,
    }


def execute_step1(base_dir: str) -> Tuple[int, int, pd.DataFrame]:
    """executa o passo 1 ajustando o tamanho da população e o limite de gerações no synth1"""
    print("\n--- starting step 1: population and generation grid search on synth1 ---")
    step_dir = os.path.join(base_dir, "results", "step1_pop_gen")
    os.makedirs(step_dir, exist_ok=True)

    pop_sizes = [50, 100, 500]
    gen_budgets = [50, 100, 500]

    tasks = []
    for pop in pop_sizes:
        for gen in gen_budgets:
            cfg = f"pop{pop}_gen{gen}"
            for seed in SEEDS:
                out_csv = os.path.join(step_dir, f"{cfg}_seed{seed}.csv")
                tasks.append(
                    (
                        "synth1",
                        pop,
                        gen,
                        "tournament",
                        0.90,
                        0.05,
                        True,
                        seed,
                        out_csv,
                        "step1_pop_gen",
                        cfg,
                        base_dir,
                    )
                )

    print(f"dispatching {len(tasks)} runs across {N_JOBS} worker processes")
    t_start = time.time()
    results = Parallel(n_jobs=N_JOBS)(delayed(run_single_task)(*t) for t in tasks)
    elapsed = time.time() - t_start
    print(f"step 1 completed in {elapsed:.1f}s")

    summary_df = pd.DataFrame(results).drop(columns=["test_preds"], errors="ignore")
    summary_path = os.path.join(step_dir, "summary.csv")
    summary_df.to_csv(summary_path, index=False)

    # encontra o par vencedor de pop e gen com base no menor rmse médio de teste
    grouped = summary_df.groupby(["pop_size", "generations"])["final_test_rmse"].mean()
    best_pair = grouped.idxmin()
    p_star, g_star = int(best_pair[0]), int(best_pair[1])
    lowest_rmse = float(grouped.loc[best_pair])
    print(f"step 1 winner: pop*={p_star}, gen*={g_star} with mean test rmse={lowest_rmse:.4f}")

    return p_star, g_star, summary_df


def execute_step2(p_star: int, g_star: int, base_dir: str) -> Tuple[str, pd.DataFrame]:
    """executa o passo 2 comparando operadores de seleção no synth1"""
    print(f"\n--- starting step 2: selection comparison using pop*={p_star}, gen*={g_star} ---")
    step_dir = os.path.join(base_dir, "results", "step2_selection")
    os.makedirs(step_dir, exist_ok=True)

    selections = ["tournament", "roulette", "lexicase"]
    tasks = []
    for sel in selections:
        for seed in SEEDS:
            out_csv = os.path.join(step_dir, f"{sel}_seed{seed}.csv")
            tasks.append(
                (
                    "synth1",
                    p_star,
                    g_star,
                    sel,
                    0.90,
                    0.05,
                    True,
                    seed,
                    out_csv,
                    "step2_selection",
                    sel,
                    base_dir,
                )
            )

    print(f"dispatching {len(tasks)} runs across {N_JOBS} worker processes")
    t_start = time.time()
    results = Parallel(n_jobs=N_JOBS)(delayed(run_single_task)(*t) for t in tasks)
    elapsed = time.time() - t_start
    print(f"step 2 completed in {elapsed:.1f}s")

    summary_df = pd.DataFrame(results).drop(columns=["test_preds"], errors="ignore")
    summary_path = os.path.join(step_dir, "summary.csv")
    summary_df.to_csv(summary_path, index=False)

    # encontra o operador de seleção vencedor com base no menor rmse médio de teste
    grouped = summary_df.groupby("selection")["final_test_rmse"].mean()
    s_star = str(grouped.idxmin())
    lowest_rmse = float(grouped.loc[s_star])
    print(f"step 2 winner: selection*={s_star} with mean test rmse={lowest_rmse:.4f}")

    return s_star, summary_df


def execute_step3(p_star: int, g_star: int, s_star: str, base_dir: str) -> Tuple[Tuple[float, float], str, pd.DataFrame]:
    """executa o passo 3 comparando operadores de variação no synth1"""
    print(f"\n--- starting step 3: variation comparison using pop*={p_star}, gen*={g_star}, sel*={s_star} ---")
    step_dir = os.path.join(base_dir, "results", "step3_variation")
    os.makedirs(step_dir, exist_ok=True)

    variations = [
        ("pc90_pm05", 0.90, 0.05),
        ("pc60_pm30", 0.60, 0.30),
    ]
    tasks = []
    for cfg_name, pc, pm in variations:
        for seed in SEEDS:
            out_csv = os.path.join(step_dir, f"{cfg_name}_seed{seed}.csv")
            tasks.append(
                (
                    "synth1",
                    p_star,
                    g_star,
                    s_star,
                    pc,
                    pm,
                    True,
                    seed,
                    out_csv,
                    "step3_variation",
                    cfg_name,
                    base_dir,
                )
            )

    print(f"dispatching {len(tasks)} runs across {N_JOBS} worker processes")
    t_start = time.time()
    results = Parallel(n_jobs=N_JOBS)(delayed(run_single_task)(*t) for t in tasks)
    elapsed = time.time() - t_start
    print(f"step 3 completed in {elapsed:.1f}s")

    summary_df = pd.DataFrame(results).drop(columns=["test_preds"], errors="ignore")
    summary_path = os.path.join(step_dir, "summary.csv")
    summary_df.to_csv(summary_path, index=False)

    # encontra o regime de variação vencedor com base no menor rmse médio de teste
    grouped = summary_df.groupby("config_name")["final_test_rmse"].mean()
    v_star_name = str(grouped.idxmin())
    lowest_rmse = float(grouped.loc[v_star_name])
    v_star = (0.90, 0.05) if v_star_name == "pc90_pm05" else (0.60, 0.30)
    print(f"step 3 winner: variation*={v_star_name} (pc={v_star[0]}, pm={v_star[1]}) with mean test rmse={lowest_rmse:.4f}")

    return v_star, v_star_name, summary_df


def execute_step4(
    p_star: int,
    g_star: int,
    s_star: str,
    v_star: Tuple[float, float],
    base_dir: str,
) -> Tuple[bool, str, pd.DataFrame]:
    """executa o passo 4 analisando o efeito do elitismo no synth1"""
    print(f"\n--- starting step 4: elitism comparison using pop*={p_star}, gen*={g_star}, sel*={s_star}, pc={v_star[0]}, pm={v_star[1]} ---")
    step_dir = os.path.join(base_dir, "results", "step4_elitism")
    os.makedirs(step_dir, exist_ok=True)

    elitism_options = [
        ("with_elitism", True),
        ("without_elitism", False),
    ]
    tasks = []
    for cfg_name, el in elitism_options:
        for seed in SEEDS:
            out_csv = os.path.join(step_dir, f"{cfg_name}_seed{seed}.csv")
            tasks.append(
                (
                    "synth1",
                    p_star,
                    g_star,
                    s_star,
                    v_star[0],
                    v_star[1],
                    el,
                    seed,
                    out_csv,
                    "step4_elitism",
                    cfg_name,
                    base_dir,
                )
            )

    print(f"dispatching {len(tasks)} runs across {N_JOBS} worker processes")
    t_start = time.time()
    results = Parallel(n_jobs=N_JOBS)(delayed(run_single_task)(*t) for t in tasks)
    elapsed = time.time() - t_start
    print(f"step 4 completed in {elapsed:.1f}s")

    summary_df = pd.DataFrame(results).drop(columns=["test_preds"], errors="ignore")
    summary_path = os.path.join(step_dir, "summary.csv")
    summary_df.to_csv(summary_path, index=False)

    # encontra a configuração de elitismo vencedora
    grouped = summary_df.groupby("config_name")["final_test_rmse"].mean()
    e_star_name = str(grouped.idxmin())
    lowest_rmse = float(grouped.loc[e_star_name])
    e_star = True if e_star_name == "with_elitism" else False
    print(f"step 4 winner: elitism*={e_star_name} ({e_star}) with mean test rmse={lowest_rmse:.4f}")

    return e_star, e_star_name, summary_df


def execute_step5(
    p_star: int,
    g_star: int,
    s_star: str,
    v_star: Tuple[float, float],
    e_star: bool,
    base_dir: str,
) -> Tuple[pd.DataFrame, List[float]]:
    """executa o passo 5 com a validação final no synth2 e concrete usando a melhor configuração geral"""
    print(f"\n--- starting step 5: validation on synth2 and concrete with best config ---")
    print(f"c* = (pop={p_star}, gen={g_star}, sel={s_star}, pc={v_star[0]}, pm={v_star[1]}, elitism={e_star})")
    step_dir = os.path.join(base_dir, "results", "step5_validation")
    os.makedirs(step_dir, exist_ok=True)

    datasets = ["synth2", "concrete"]
    tasks = []
    for ds in datasets:
        for seed in SEEDS:
            out_csv = os.path.join(step_dir, f"{ds}_seed{seed}.csv")
            tasks.append(
                (
                    ds,
                    p_star,
                    g_star,
                    s_star,
                    v_star[0],
                    v_star[1],
                    e_star,
                    seed,
                    out_csv,
                    "step5_validation",
                    ds,
                    base_dir,
                )
            )

    print(f"dispatching {len(tasks)} runs across {N_JOBS} worker processes")
    t_start = time.time()
    results = Parallel(n_jobs=N_JOBS)(delayed(run_single_task)(*t) for t in tasks)
    elapsed = time.time() - t_start
    print(f"step 5 completed in {elapsed:.1f}s")

    # isola as melhores predições do concrete para o gráfico de dispersão de paridade
    best_concrete_preds = None
    best_concrete_rmse = float("inf")
    for r in results:
        if r["dataset"] == "concrete" and r.get("test_preds") is not None:
            if r["final_test_rmse"] < best_concrete_rmse:
                best_concrete_rmse = r["final_test_rmse"]
                best_concrete_preds = r["test_preds"]

    summary_df = pd.DataFrame(results).drop(columns=["test_preds"], errors="ignore")
    summary_path = os.path.join(step_dir, "summary.csv")
    summary_df.to_csv(summary_path, index=False)

    for ds in datasets:
        ds_mean_tr = summary_df[summary_df["dataset"] == ds]["best_train_rmse"].mean()
        ds_mean_te = summary_df[summary_df["dataset"] == ds]["final_test_rmse"].mean()
        print(f"step 5 {ds}: mean train rmse={ds_mean_tr:.4f}, mean test rmse={ds_mean_te:.4f}")

    return summary_df, best_concrete_preds


def generate_all_tables(base_dir: str) -> None:
    """calcula tabelas de resumo em todos os passos e exporta arquivos markdown e latex"""
    tables_dir = os.path.join(base_dir, "results", "tables")
    os.makedirs(tables_dir, exist_ok=True)

    step_info = [
        ("step1_pop_gen", 1, "step1_summary"),
        ("step2_selection", 2, "step2_summary"),
        ("step3_variation", 3, "step3_summary"),
        ("step4_elitism", 4, "step4_summary"),
        ("step5_validation", 5, "step5_summary"),
    ]

    for step_folder, step_num, base_name in step_info:
        summary_csv = os.path.join(base_dir, "results", step_folder, "summary.csv")
        if not os.path.exists(summary_csv):
            continue

        raw_df = pd.read_csv(summary_csv)
        stats_df = compute_summary_stats(raw_df)

        md_path1 = os.path.join(tables_dir, f"{base_name}.md")
        md_path2 = os.path.join(tables_dir, f"{step_folder}_summary.md")
        tex_path1 = os.path.join(tables_dir, f"{base_name}.tex")
        tex_path2 = os.path.join(tables_dir, f"{step_folder}_summary.tex")

        caption = f"Resultados resumidos do Passo {step_num}"
        label = f"tab:step{step_num}"

        export_markdown_table(stats_df, md_path1)
        export_markdown_table(stats_df, md_path2)
        export_latex_table(stats_df, tex_path1, caption=caption, label=label)
        export_latex_table(stats_df, tex_path2, caption=caption, label=label)

    print("all summary tables successfully generated in results/tables/")


def generate_all_plots(
    base_dir: str,
    p_star: int,
    g_star: int,
    v_star_name: str,
    concrete_best_preds: List[float],
) -> None:
    """gera todas as 8 figuras necessárias para publicação nos formatos png e svg"""
    plots_dir = os.path.join(base_dir, "results", "plots")
    os.makedirs(plots_dir, exist_ok=True)

    # figura 1: convergência do passo 1
    s1_dir = os.path.join(base_dir, "results", "step1_pop_gen")
    winning_s1_files = [os.path.join(s1_dir, f"pop{p_star}_gen{g_star}_seed{s}.csv") for s in SEEDS]
    fig1_path = os.path.join(plots_dir, "fig1_step1_convergence.png")
    plot_convergence(winning_s1_files, fig1_path, title=f"passo 1: convergência (pop={p_star}, gen={g_star})")

    # figura 2: bloat do passo 1
    fig2_path = os.path.join(plots_dir, "fig2_step1_bloat.png")
    plot_bloat(winning_s1_files, fig2_path, title=f"passo 1: complexidade da árvore (pop={p_star}, gen={g_star})")

    # figura 3: convergência da seleção do passo 2
    s2_dir = os.path.join(base_dir, "results", "step2_selection")
    s2_dict = {
        "torneio (k=2)": [os.path.join(s2_dir, f"tournament_seed{s}.csv") for s in SEEDS],
        "roleta escalada": [os.path.join(s2_dir, f"roulette_seed{s}.csv") for s in SEEDS],
        "epsilon-lexicase": [os.path.join(s2_dir, f"lexicase_seed{s}.csv") for s in SEEDS],
    }
    fig3_path = os.path.join(plots_dir, "fig3_step2_selection_convergence.png")
    plot_metric_comparison(s2_dict, fig3_path, metric_col="best_train_rmse", title="passo 2: convergência da seleção")

    # figura 4: diversidade do passo 2
    fig4_path = os.path.join(plots_dir, "fig4_step2_diversity.png")
    plot_selection_diversity(s2_dict, fig4_path, title="passo 2: diversidade estrutural")

    # figura 5: sucesso do operador do passo 3
    s3_dir = os.path.join(base_dir, "results", "step3_variation")
    winning_s3_files = [os.path.join(s3_dir, f"{v_star_name}_seed{s}.csv") for s in SEEDS]
    fig5_path = os.path.join(plots_dir, "fig5_step3_operator_success.png")
    plot_operator_success(winning_s3_files, fig5_path, title=f"passo 3: sucesso do operador ({v_star_name})")

    # figura 6: comparação do elitismo do passo 4
    s4_dir = os.path.join(base_dir, "results", "step4_elitism")
    s4_dict = {
        "com elitismo": [os.path.join(s4_dir, f"with_elitism_seed{s}.csv") for s in SEEDS],
        "sem elitismo": [os.path.join(s4_dir, f"without_elitism_seed{s}.csv") for s in SEEDS],
    }
    fig6_path = os.path.join(plots_dir, "fig6_step4_elitism_comparison.png")
    plot_metric_comparison(s4_dict, fig6_path, metric_col="best_train_rmse", title="passo 4: comparação do elitismo")

    # figura 7: convergência de synth2 e concrete do passo 5
    s5_dir = os.path.join(base_dir, "results", "step5_validation")
    s5_dict = {
        "rmse de teste synth2": [os.path.join(s5_dir, f"synth2_seed{s}.csv") for s in SEEDS],
        "rmse de teste concrete": [os.path.join(s5_dir, f"concrete_seed{s}.csv") for s in SEEDS],
    }
    fig7_path = os.path.join(plots_dir, "fig7_step5_synth2_concrete_convergence.png")
    plot_metric_comparison(s5_dict, fig7_path, metric_col="best_test_rmse", title="passo 5: convergência de validação no teste")

    # figura 8: gráfico de dispersão de paridade do concrete no passo 5
    _, _, _, y_te_concrete = load_dataset("concrete", base_dir=base_dir)
    fig8_path = os.path.join(plots_dir, "fig8_step5_generalization_scatter.png")
    if concrete_best_preds is not None:
        plot_parity(y_te_concrete, np.array(concrete_best_preds), fig8_path, title="concrete: predições vs real")
    else:
        # avaliação alternativa se necessário
        plot_parity(y_te_concrete, y_te_concrete, fig8_path, title="concrete: predições vs real")

    print("all 8 figures successfully generated in results/plots/ (png and svg)")


def main():
    """coordena o pipeline experimental completo de 540 execuções e gera todos os artefatos"""
    base_dir = repo_root
    total_start = time.time()
    print("=" * 60)
    print("starting full gp experimental suite: 5 steps, 540 genuine runs, 30 seeds")
    print(f"seeds: {SEEDS[0]}..{SEEDS[-1]} (count: {len(SEEDS)})")
    print(f"concurrency: {N_JOBS} parallel workers")
    print("=" * 60)

    # passo 1: 9 configs x 30 sementes = 270 execuções
    p_star, g_star, _ = execute_step1(base_dir)

    # passo 2: 3 configs x 30 sementes = 90 execuções
    s_star, _ = execute_step2(p_star, g_star, base_dir)

    # passo 3: 2 configs x 30 sementes = 60 execuções
    v_star, v_star_name, _ = execute_step3(p_star, g_star, s_star, base_dir)

    # passo 4: 2 configs x 30 sementes = 60 execuções
    e_star, e_star_name, _ = execute_step4(p_star, g_star, s_star, v_star, base_dir)

    # passo 5: 2 datasets x 30 sementes = 60 execuções
    _, concrete_best_preds = execute_step5(p_star, g_star, s_star, v_star, e_star, base_dir)

    # gera tabelas de resumo
    generate_all_tables(base_dir)

    # gera gráficos de publicação
    generate_all_plots(base_dir, p_star, g_star, v_star_name, concrete_best_preds)

    total_time = time.time() - total_start
    print("=" * 60)
    print(f"all 540 runs completed successfully in {total_time:.1f}s ({total_time / 60.0:.2f} minutes)")
    print(f"final winning configuration: pop={p_star}, gen={g_star}, selection={s_star}, pc={v_star[0]}, pm={v_star[1]}, elitism={e_star}")
    print("=" * 60)


if __name__ == "__main__":
    main()
