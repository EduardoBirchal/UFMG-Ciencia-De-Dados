"""cálculo de métricas e rastreador de registro geracional"""

from typing import Any, Dict, List
import numpy as np

from src.gp.tree import Tree


def compute_rmse(y_true: np.ndarray, y_pred: np.ndarray) -> float:
    """calcula a raiz do erro quadrático médio entre os arrays reais e preditos"""
    diff = np.asarray(y_true, dtype=np.float64) - np.asarray(y_pred, dtype=np.float64)
    res = float(np.sqrt(np.mean(diff ** 2)))
    return res if np.isfinite(res) else 1e9


def count_duplicates(population: List[Tree]) -> int:
    """conta indivíduos sintaticamente duplicados por string canônica"""
    strings = [ind.to_string() for ind in population]
    return len(population) - len(set(strings))


class MetricTracker:
    """registra estatísticas evolutivas por geração"""

    def __init__(self):
        self.history: List[Dict[str, Any]] = []

    def log_generation(
        self,
        gen: int,
        best_train_rmse: float,
        worst_train_rmse: float,
        mean_train_rmse: float,
        best_test_rmse: float,
        duplicate_trees: int,
        unique_canonical_trees: int,
        crossover_better: int,
        crossover_worse: int,
        crossover_equal: int,
        crossover_rejected: int,
        crossover_total: int,
        avg_nodes: float,
        avg_depth: float,
        best_nodes: int,
        best_depth: int,
        elapsed_seconds: float,
    ) -> Dict[str, Any]:
        """registra uma entrada de estatísticas da geração atual"""
        entry: Dict[str, Any] = {
            "generation": int(gen),
            "best_train_rmse": float(best_train_rmse),
            "worst_train_rmse": float(worst_train_rmse),
            "mean_train_rmse": float(mean_train_rmse),
            "best_test_rmse": float(best_test_rmse),
            "duplicate_trees": int(duplicate_trees),
            "unique_canonical_trees": int(unique_canonical_trees),
            "crossover_better": int(crossover_better),
            "crossover_worse": int(crossover_worse),
            "crossover_equal": int(crossover_equal),
            "crossover_rejected": int(crossover_rejected),
            "crossover_total": int(crossover_total),
            "avg_nodes": float(avg_nodes),
            "avg_depth": float(avg_depth),
            "best_nodes": int(best_nodes),
            "best_depth": int(best_depth),
            "elapsed_seconds": float(elapsed_seconds),
        }
        self.history.append(entry)
        return entry

    def get_history(self) -> List[Dict[str, Any]]:
        """retorna o histórico completo de registros geracionais"""
        return list(self.history)

