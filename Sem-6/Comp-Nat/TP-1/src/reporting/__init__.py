"""pacote de relatórios com utilitários de plotagem e tabelas"""

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

__all__ = [
    "plot_convergence",
    "plot_bloat",
    "plot_operator_success",
    "plot_selection_diversity",
    "plot_metric_comparison",
    "plot_parity",
    "compute_summary_stats",
    "export_markdown_table",
    "export_latex_table",
    "generate_summary_tables",
]

