"""testes unitários para o gerador de relatórios, tabelas e modelo de relatório latex"""

import os
import shutil
import sys
import tempfile
import xml.etree.ElementTree as et
import numpy as np
import pandas as pd

# garante que a raiz do projeto esteja no caminho de importação
repo_root = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
if repo_root not in sys.path:
    sys.path.insert(0, repo_root)

from scripts.check_style import check_file
from src.reporting.plotter import (
    plot_bloat,
    plot_convergence,
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


def _make_sample_history_df() -> pd.DataFrame:
    """gera dataframe amostral de métricas geracionais para testes"""
    return pd.DataFrame(
        {
            "generation": [0, 1, 2, 3, 4],
            "best_train_rmse": [2.5, 1.8, 1.2, 0.9, 0.6],
            "mean_train_rmse": [3.2, 2.5, 2.0, 1.7, 1.4],
            "best_test_rmse": [2.7, 2.0, 1.4, 1.1, 0.8],
            "avg_nodes": [12.0, 15.0, 18.0, 22.0, 25.0],
            "avg_depth": [3.0, 3.5, 4.0, 4.2, 4.5],
            "crossover_better": [4, 8, 12, 15, 18],
            "crossover_worse": [16, 12, 8, 5, 2],
            "crossover_equal": [0, 0, 0, 0, 0],
            "unique_canonical_trees": [50, 48, 44, 40, 35],
        }
    )


def test_plot_convergence_svg():
    """testa se o gerador de convergência grava svg xml válido e png correspondente"""
    tmp_dir = tempfile.mkdtemp()
    try:
        sample_df = _make_sample_history_df()
        csv_file = os.path.join(tmp_dir, "run.csv")
        sample_df.to_csv(csv_file, index=False)

        out_svg = os.path.join(tmp_dir, "conv.svg")
        plot_convergence(csv_file, out_svg, title="convergence test")

        assert os.path.exists(out_svg)
        # verifica se o png correspondente foi criado
        out_png = os.path.join(tmp_dir, "conv.png")
        assert os.path.exists(out_png)

        # verifica estrutura xml válida do svg
        tree = et.parse(out_svg)
        root = tree.getroot()
        assert root.tag.endswith("svg")
        xml_text = open(out_svg, "r", encoding="utf-8").read()
        assert "<polyline" in xml_text
        assert "melhor rmse de treino" in xml_text
        assert "convergence test" in xml_text

        # verifica cabeçalho binário png válido
        with open(out_png, "rb") as f:
            header = f.read(8)
        assert header == b"\x89PNG\r\n\x1a\n"
    finally:
        shutil.rmtree(tmp_dir, ignore_errors=True)


def test_plot_convergence_png_direct():
    """testa se requisitar saída png diretamente cria png e svg correspondente"""
    tmp_dir = tempfile.mkdtemp()
    try:
        sample_df = _make_sample_history_df()
        csv_file = os.path.join(tmp_dir, "run.csv")
        sample_df.to_csv(csv_file, index=False)

        out_png = os.path.join(tmp_dir, "conv_direct.png")
        plot_convergence(csv_file, out_png)

        assert os.path.exists(out_png)
        assert os.path.exists(os.path.join(tmp_dir, "conv_direct.svg"))
        with open(out_png, "rb") as f:
            header = f.read(8)
        assert header == b"\x89PNG\r\n\x1a\n"
    finally:
        shutil.rmtree(tmp_dir, ignore_errors=True)


def test_plot_convergence_shaded_band_multiple_runs():
    """testa se plotar múltiplas sementes cria faixa sombreada de erro no svg"""
    tmp_dir = tempfile.mkdtemp()
    try:
        df1 = pd.DataFrame(
            {
                "generation": [0, 1, 2],
                "best_train_rmse": [2.0, 1.5, 1.0],
                "mean_train_rmse": [3.0, 2.5, 2.0],
                "best_test_rmse": [2.2, 1.7, 1.2],
            }
        )
        df2 = pd.DataFrame(
            {
                "generation": [0, 1, 2],
                "best_train_rmse": [2.4, 1.9, 1.4],
                "mean_train_rmse": [3.4, 2.9, 2.4],
                "best_test_rmse": [2.6, 2.1, 1.6],
            }
        )
        p1 = os.path.join(tmp_dir, "r1.csv")
        p2 = os.path.join(tmp_dir, "r2.csv")
        df1.to_csv(p1, index=False)
        df2.to_csv(p2, index=False)

        out_svg = os.path.join(tmp_dir, "multi.svg")
        plot_convergence([p1, p2], out_svg)

        xml_text = open(out_svg, "r", encoding="utf-8").read()
        # múltiplas sementes com variância produzem faixa poligonal sombreada
        assert "<polygon" in xml_text
    finally:
        shutil.rmtree(tmp_dir, ignore_errors=True)


def test_plot_bloat():
    """testa se o gráfico de bloat cria svg válido com curvas de nós e profundidade"""
    tmp_dir = tempfile.mkdtemp()
    try:
        sample_df = _make_sample_history_df()
        csv_file = os.path.join(tmp_dir, "bloat.csv")
        sample_df.to_csv(csv_file, index=False)

        out_svg = os.path.join(tmp_dir, "bloat.svg")
        plot_bloat(csv_file, out_svg, title="bloat test")

        assert os.path.exists(out_svg)
        tree = et.parse(out_svg)
        root = tree.getroot()
        assert root.tag.endswith("svg")
        xml_str = open(out_svg, "r", encoding="utf-8").read()
        assert "número médio de nós" in xml_str
        assert "profundidade média" in xml_str
    finally:
        shutil.rmtree(tmp_dir, ignore_errors=True)


def test_plot_operator_success():
    """testa o gráfico de sucesso do operador plotando contagens de cruzamento melhores e piores"""
    tmp_dir = tempfile.mkdtemp()
    try:
        sample_df = _make_sample_history_df()
        csv_file = os.path.join(tmp_dir, "ops.csv")
        sample_df.to_csv(csv_file, index=False)

        out_svg = os.path.join(tmp_dir, "ops.svg")
        plot_operator_success(csv_file, out_svg, title="operator success")

        assert os.path.exists(out_svg)
        tree = et.parse(out_svg)
        assert tree.getroot().tag.endswith("svg")
        xml_str = open(out_svg, "r", encoding="utf-8").read()
        assert "cruzamento melhor" in xml_str
        assert "cruzamento pior" in xml_str
    finally:
        shutil.rmtree(tmp_dir, ignore_errors=True)


def test_plot_selection_diversity():
    """testa o gráfico de comparação da diversidade de seleção com múltiplos métodos"""
    tmp_dir = tempfile.mkdtemp()
    try:
        sample_df = _make_sample_history_df()
        p1 = os.path.join(tmp_dir, "tourney.csv")
        p2 = os.path.join(tmp_dir, "roulette.csv")
        sample_df.to_csv(p1, index=False)
        sample_df.to_csv(p2, index=False)

        out_svg = os.path.join(tmp_dir, "div.svg")
        plot_selection_diversity({"tourney": p1, "roulette": p2}, out_svg)

        assert os.path.exists(out_svg)
        tree = et.parse(out_svg)
        assert tree.getroot().tag.endswith("svg")
        xml_str = open(out_svg, "r", encoding="utf-8").read()
        assert "tourney" in xml_str
        assert "roulette" in xml_str
    finally:
        shutil.rmtree(tmp_dir, ignore_errors=True)


def test_plot_parity():
    """testa o gráfico de dispersão de paridade com linha diagonal de referência ideal"""
    tmp_dir = tempfile.mkdtemp()
    try:
        yt = np.array([1.0, 2.0, 3.0, 4.0, 5.0])
        yp = np.array([1.1, 1.9, 3.2, 3.8, 5.1])

        out_svg = os.path.join(tmp_dir, "parity.svg")
        plot_parity(yt, yp, out_svg, title="parity validation")

        assert os.path.exists(out_svg)
        tree = et.parse(out_svg)
        assert tree.getroot().tag.endswith("svg")
        xml_text = open(out_svg, "r", encoding="utf-8").read()
        assert "<circle" in xml_text
        assert "ideal (y=x)" in xml_text
    finally:
        shutil.rmtree(tmp_dir, ignore_errors=True)


def test_compute_summary_stats_from_summary_df():
    """testa cálculo de estatísticas de resumo a partir de dataframe com múltiplas configurações"""
    df = pd.DataFrame(
        {
            "config_name": ["cfg_a", "cfg_a", "cfg_b", "cfg_b"],
            "dataset": ["synth1", "synth1", "synth1", "synth1"],
            "best_train_rmse": [0.1, 0.2, 0.5, 0.6],
            "final_test_rmse": [0.3, 0.4, 0.7, 0.8],
            "best_tree_nodes": [10, 14, 20, 22],
            "best_tree_depth": [4, 4, 5, 5],
        }
    )
    res = compute_summary_stats(df)
    assert len(res) == 2
    assert "cfg_a" in res["configuration"].values
    assert "cfg_b" in res["configuration"].values


def test_export_markdown_table():
    """testa se o exportador de tabela markdown gera tabela delimitada por barras válida"""
    tmp_dir = tempfile.mkdtemp()
    try:
        stats_df = pd.DataFrame(
            {
                "configuration": ["pop50_gen50", "pop100_gen100"],
                "dataset": ["synth1", "synth1"],
                "train_rmse": ["0.1234 \u00b1 0.0123", "0.0890 \u00b1 0.0050"],
                "test_rmse": ["0.2345 \u00b1 0.0234", "0.1560 \u00b1 0.0100"],
                "avg_nodes": ["15.20 \u00b1 2.10", "18.40 \u00b1 2.50"],
                "avg_depth": ["4.30 \u00b1 0.50", "4.80 \u00b1 0.60"],
            }
        )

        out_md = os.path.join(tmp_dir, "table.md")
        table_str = export_markdown_table(stats_df, out_md)

        assert os.path.exists(out_md)
        assert "| Configuration |" in table_str
        assert "| pop50_gen50 |" in table_str
        assert ":---:" in table_str
    finally:
        shutil.rmtree(tmp_dir, ignore_errors=True)


def test_export_latex_table_booktabs():
    """testa se o exportador de tabela latex produz comandos booktabs e escapa underscores"""
    tmp_dir = tempfile.mkdtemp()
    try:
        stats_df = pd.DataFrame(
            {
                "configuration": ["pop50_gen50", "pop100_gen100"],
                "dataset": ["synth1", "synth1"],
                "train_rmse": ["0.1234 \u00b1 0.0123", "0.0890 \u00b1 0.0050"],
                "test_rmse": ["0.2345 \u00b1 0.0234", "0.1560 \u00b1 0.0100"],
                "avg_nodes": ["15.20 \u00b1 2.10", "18.40 \u00b1 2.50"],
                "avg_depth": ["4.30 \u00b1 0.50", "4.80 \u00b1 0.60"],
            }
        )

        out_tex = os.path.join(tmp_dir, "table.tex")
        tex_str = export_latex_table(stats_df, out_tex, caption="test table", label="tab:test")

        assert os.path.exists(out_tex)
        assert "\\begin{table}[htbp]" in tex_str
        assert "\\toprule" in tex_str
        assert "\\midrule" in tex_str
        assert "\\bottomrule" in tex_str
        assert "\\caption{test table}" in tex_str
        assert "\\label{tab:test}" in tex_str
        # verifica escape de sublinhado nos nomes de configuração
        assert "pop50\\_gen50" in tex_str
        assert "$\\pm$" in tex_str
    finally:
        shutil.rmtree(tmp_dir, ignore_errors=True)


def test_generate_summary_tables():
    """testa varredura do diretório de resultados para gerar tabelas markdown e latex"""
    tmp_dir = tempfile.mkdtemp()
    try:
        res_dir = os.path.join(tmp_dir, "results")
        out_dir = os.path.join(tmp_dir, "tables_out")

        s1_dir = os.path.join(res_dir, "step1_pop_gen")
        os.makedirs(s1_dir, exist_ok=True)
        summary_csv = os.path.join(s1_dir, "summary.csv")
        summary_df = pd.DataFrame(
            {
                "config_name": ["pop50_gen50", "pop50_gen100"],
                "dataset": ["synth1", "synth1"],
                "best_train_rmse": [0.2, 0.15],
                "final_test_rmse": [0.3, 0.25],
                "best_tree_nodes": [12, 16],
                "best_tree_depth": [4, 5],
            }
        )
        summary_df.to_csv(summary_csv, index=False)

        generate_summary_tables(res_dir, out_dir)

        assert os.path.exists(os.path.join(out_dir, "step1_pop_gen_summary.md"))
        assert os.path.exists(os.path.join(out_dir, "step1_pop_gen_summary.tex"))
    finally:
        shutil.rmtree(tmp_dir, ignore_errors=True)


def test_relatorio_template_structure():
    """testa se relatorio_tp1.tex existe e contém todas as seções e marcadores necessários"""
    tex_path = os.path.join(repo_root, "relatorio_tp1.tex")
    assert os.path.exists(tex_path)

    content = open(tex_path, "r", encoding="utf-8").read()
    assert len(content) > 500

    # verifica 5 seções obrigatórias
    assert "\\section{Introdução}" in content
    assert "\\section{Implementação}" in content
    assert "\\section{Experimentos e Análise de Resultados}" in content
    assert "\\section{Conclusões}" in content
    assert "\\bibliography{referencias}" in content

    # verifica subseções obrigatórias para os passos 1 a 5
    assert "Passo 1" in content
    assert "Passo 2" in content
    assert "Passo 3" in content
    assert "Passo 4" in content
    assert "Passo 5" in content

    # verifica marcadores de figuras obrigatórios
    for fig_id in [
        "fig1_step1_convergence",
        "fig2_step1_bloat",
        "fig3_step2_selection_convergence",
        "fig4_step2_diversity",
        "fig5_step3_operator_success",
        "fig6_step4_elitism_comparison",
        "fig7_step5_synth2_concrete_convergence",
        "fig8_step5_generalization_scatter",
    ]:
        assert fig_id in content

    # verifica marcadores de tabelas obrigatórios
    for tab_id in [
        "step1_summary.tex",
        "step2_summary.tex",
        "step3_summary.tex",
        "step4_summary.tex",
        "step5_summary.tex",
    ]:
        assert tab_id in content

    # verifica pacotes obrigatórios
    assert "\\usepackage{booktabs}" in content
    assert "\\usepackage{graphicx}" in content
    assert "\\usepackage{geometry}" in content


def test_reporting_code_style():
    """testa se todos os arquivos de relatório passam nas verificações estritas de estilo r2"""
    targets = [
        os.path.join(repo_root, "src", "reporting", "__init__.py"),
        os.path.join(repo_root, "src", "reporting", "plotter.py"),
        os.path.join(repo_root, "src", "reporting", "tables.py"),
        os.path.join(repo_root, "tests", "test_reporting.py"),
    ]
    for target in targets:
        assert os.path.exists(target)
        errors = check_file(target)
        assert len(errors) == 0, f"style errors in {target}: {errors}"
