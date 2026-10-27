"""gerador de tabelas de resumo exportando para markdown e latex booktabs"""

import os
import numpy as np
import pandas as pd


def _clean_latex_text(text: str) -> str:
    """escapa caracteres reservados do latex na string"""
    s = str(text)
    s = s.replace("\\", "\\textbackslash ")
    s = s.replace("_", "\\_")
    s = s.replace("%", "\\%")
    s = s.replace("&", "\\&")
    s = s.replace("#", "\\#")
    s = s.replace("±", "$\\pm$")
    s = s.replace("+/-", "$\\pm$")
    return s


def compute_summary_stats(df: pd.DataFrame) -> pd.DataFrame:
    """calcula a média e o desvio padrão das métricas entre as sementes por configuração"""
    rows = []
    group_cols = ["config_name"]
    if "dataset" in df.columns and df["dataset"].nunique() > 1:
        group_cols.append("dataset")

    for g_keys, group_data in df.groupby(group_cols):
        if isinstance(g_keys, tuple):
            cfg_val = str(g_keys[0])
            ds_val = str(g_keys[1]) if len(g_keys) > 1 else ""
        else:
            cfg_val = str(g_keys)
            ds_val = str(group_data["dataset"].iloc[0]) if "dataset" in group_data.columns else ""
        rows.append(_extract_metrics_stats(group_data, config_name=cfg_val, dataset_name=ds_val))

    return pd.DataFrame(rows)


def _extract_metrics_stats(df: pd.DataFrame, config_name: str = "", dataset_name: str = "") -> dict:
    """calcula média e desvio padrão entre as linhas para as métricas necessárias"""
    nodes_col = "best_tree_nodes" if "best_tree_nodes" in df.columns else "avg_nodes"
    depth_col = "best_tree_depth" if "best_tree_depth" in df.columns else "avg_depth"

    def calc(col_name: str) -> tuple[float, float]:
        vals = df[col_name].dropna().to_numpy(dtype=np.float64)
        mean_v = float(np.mean(vals))
        std_v = float(np.std(vals, ddof=1)) if len(vals) > 1 else 0.0
        return mean_v, std_v

    tr_mean, tr_std = calc("best_train_rmse")
    te_mean, te_std = calc("final_test_rmse")
    nodes_mean, nodes_std = calc(nodes_col)
    depth_mean, depth_std = calc(depth_col)

    return {
        "configuration": config_name,
        "dataset": dataset_name,
        "train_rmse_mean": tr_mean,
        "train_rmse_std": tr_std,
        "train_rmse": f"{tr_mean:.4f} \u00b1 {tr_std:.4f}",
        "test_rmse_mean": te_mean,
        "test_rmse_std": te_std,
        "test_rmse": f"{te_mean:.4f} \u00b1 {te_std:.4f}",
        "avg_nodes_mean": nodes_mean,
        "avg_nodes_std": nodes_std,
        "avg_nodes": f"{nodes_mean:.2f} \u00b1 {nodes_std:.2f}",
        "avg_depth_mean": depth_mean,
        "avg_depth_std": depth_std,
        "avg_depth": f"{depth_mean:.2f} \u00b1 {depth_std:.2f}",
    }



def export_markdown_table(df: pd.DataFrame, output_path: str | None = None) -> str:
    """formata o dataframe em tabela limpa no formato github flavored markdown"""
    if df.empty:
        return ""

    # seleciona e renomeia colunas de exibição
    cols_to_display = []
    headers = []
    alignments = []

    if "configuration" in df.columns:
        cols_to_display.append("configuration")
        headers.append("Configuration")
        alignments.append(":---")

    if "dataset" in df.columns and df["dataset"].astype(str).str.strip().any():
        cols_to_display.append("dataset")
        headers.append("Dataset")
        alignments.append(":---")

    if "train_rmse" in df.columns:
        cols_to_display.append("train_rmse")
        headers.append("Train RMSE (Mean \u00b1 Std)")
        alignments.append(":---:")
    elif "train_rmse_mean" in df.columns:
        cols_to_display.append("train_rmse_mean")
        headers.append("Train RMSE")
        alignments.append(":---:")

    if "test_rmse" in df.columns:
        cols_to_display.append("test_rmse")
        headers.append("Test RMSE (Mean \u00b1 Std)")
        alignments.append(":---:")
    elif "test_rmse_mean" in df.columns:
        cols_to_display.append("test_rmse_mean")
        headers.append("Test RMSE")
        alignments.append(":---:")

    if "avg_nodes" in df.columns:
        cols_to_display.append("avg_nodes")
        headers.append("Avg Nodes")
        alignments.append(":---:")

    if "avg_depth" in df.columns:
        cols_to_display.append("avg_depth")
        headers.append("Avg Depth")
        alignments.append(":---:")

    # recurso alternativo para todas as colunas se nenhuma coincidir
    if not cols_to_display:
        cols_to_display = list(df.columns)
        headers = [str(c).title() for c in cols_to_display]
        alignments = [":---:" for _ in cols_to_display]

    lines = [
        "| " + " | ".join(headers) + " |",
        "| " + " | ".join(alignments) + " |",
    ]

    for _, row in df.iterrows():
        row_vals = [str(row[c]) if pd.notna(row[c]) else "" for c in cols_to_display]
        lines.append("| " + " | ".join(row_vals) + " |")

    table_str = "\n".join(lines) + "\n"

    if output_path is not None:
        out_dir = os.path.dirname(os.path.abspath(output_path))
        if out_dir:
            os.makedirs(out_dir, exist_ok=True)
        with open(output_path, "w", encoding="utf-8") as f:
            f.write(table_str)

    return table_str


def export_latex_table(
    df: pd.DataFrame,
    output_path: str | None = None,
    caption: str = "",
    label: str = "",
) -> str:
    """formata o dataframe em tabela latex com qualidade de publicação usando booktabs"""
    if df.empty:
        return ""

    cols_to_display = []
    headers = []
    col_types = []

    if "configuration" in df.columns:
        cols_to_display.append("configuration")
        headers.append("Configuration")
        col_types.append("l")

    if "dataset" in df.columns and df["dataset"].astype(str).str.strip().any():
        cols_to_display.append("dataset")
        headers.append("Dataset")
        col_types.append("l")

    if "train_rmse" in df.columns:
        cols_to_display.append("train_rmse")
        headers.append("Train RMSE")
        col_types.append("c")
    elif "train_rmse_mean" in df.columns:
        cols_to_display.append("train_rmse_mean")
        headers.append("Train RMSE")
        col_types.append("c")

    if "test_rmse" in df.columns:
        cols_to_display.append("test_rmse")
        headers.append("Test RMSE")
        col_types.append("c")
    elif "test_rmse_mean" in df.columns:
        cols_to_display.append("test_rmse_mean")
        headers.append("Test RMSE")
        col_types.append("c")

    if "avg_nodes" in df.columns:
        cols_to_display.append("avg_nodes")
        headers.append("Avg Nodes")
        col_types.append("c")

    if "avg_depth" in df.columns:
        cols_to_display.append("avg_depth")
        headers.append("Avg Depth")
        col_types.append("c")

    if not cols_to_display:
        cols_to_display = list(df.columns)
        headers = [str(c).title() for c in cols_to_display]
        col_types = ["c" for _ in cols_to_display]

    col_spec = "".join(col_types)
    header_str = " & ".join(headers) + " \\\\"

    body_rows = []
    for _, row in df.iterrows():
        formatted_vals = []
        for c in cols_to_display:
            val = row[c]
            val_str = str(val) if pd.notna(val) else ""
            formatted_vals.append(_clean_latex_text(val_str))
        body_rows.append(" & ".join(formatted_vals) + " \\\\")

    table_lines = [
        "\\begin{table}[htbp]",
        "  \\centering",
    ]
    if caption:
        table_lines.append(f"  \\caption{{{_clean_latex_text(caption)}}}")
    if label:
        table_lines.append(f"  \\label{{{label}}}")

    table_lines.extend(
        [
            f"  \\begin{{tabular}}{{{col_spec}}}",
            "    \\toprule",
            f"    {header_str}",
            "    \\midrule",
        ]
    )
    for b_row in body_rows:
        table_lines.append(f"    {b_row}")
    table_lines.extend(
        [
            "    \\bottomrule",
            "  \\end{tabular}",
            "\\end{table}",
            "",
        ]
    )

    latex_str = "\n".join(table_lines)

    if output_path is not None:
        out_dir = os.path.dirname(os.path.abspath(output_path))
        if out_dir:
            os.makedirs(out_dir, exist_ok=True)
        with open(output_path, "w", encoding="utf-8") as f:
            f.write(latex_str)

    return latex_str


def generate_summary_tables(results_dir: str, output_dir: str) -> None:
    """percorre os diretórios de resultados experimentais e gera tabelas de resumo em markdown e latex"""
    os.makedirs(output_dir, exist_ok=True)

    step_folders = [
        "step1_pop_gen",
        "step2_selection",
        "step3_variation",
        "step4_elitism",
        "step5_validation",
    ]

    for step_name in step_folders:
        summary_file = os.path.join(results_dir, step_name, "summary.csv")
        if not os.path.exists(summary_file):
            continue

        summary_data = pd.read_csv(summary_file)
        stats_df = compute_summary_stats(summary_data)

        if not stats_df.empty:
            md_path = os.path.join(output_dir, f"{step_name}_summary.md")
            tex_path = os.path.join(output_dir, f"{step_name}_summary.tex")
            export_markdown_table(stats_df, md_path)
            step_num = step_name.split("_")[0].replace("step", "Passo ")
            export_latex_table(
                stats_df,
                tex_path,
                caption=f"Resultados resumidos do {step_num}",
                label=f"tab:{step_name}",
            )

