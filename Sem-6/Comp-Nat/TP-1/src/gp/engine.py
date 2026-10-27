"""motor de evolução coordenando seleção, variação, elitismo e rastreamento de histórico"""

import time
from typing import Any, Dict, List, Optional
import numpy as np

from src.gp.metrics import MetricTracker, compute_rmse, count_duplicates
from src.gp.operators import mutate, reproduce, subtree_crossover
from src.gp.selection import (
    epsilon_lexicase_selection,
    scaled_roulette_selection,
    tournament_selection,
)
from src.gp.tree import Tree, ramped_half_and_half


class GPEngine:
    """motor geracional de regressão simbólica por programação genética"""

    def __init__(
        self,
        pop_size: int = 100,
        generations: int = 100,
        selection: str = "tournament",
        tournament_k: int = 2,
        pc: float = 0.9,
        pm: float = 0.05,
        elitism: bool = True,
        max_depth: int = 7,
        seed: Optional[int] = 42,
    ):
        self.pop_size = pop_size
        self.generations = generations
        self.selection = selection
        self.tournament_k = tournament_k
        self.pc = pc
        self.pm = pm
        self.elitism = elitism
        self.max_depth = max_depth
        self.seed = seed
        self.best_individual_: Optional[Tree] = None
        self.best_train_rmse_: float = float("inf")
        self.history_: List[Dict[str, Any]] = []

    def fit(
        self,
        X_train: np.ndarray,
        y_train: np.ndarray,
        X_test: Optional[np.ndarray] = None,
        y_test: Optional[np.ndarray] = None,
    ) -> Dict[str, Any]:
        """executa o laço evolutivo geracional de regressão"""
        start_time = time.time()
        rng = np.random.default_rng(self.seed)

        x_tr = np.asarray(X_train, dtype=np.float64)
        if x_tr.ndim == 1:
            x_tr = x_tr.reshape(-1, 1)
        y_tr = np.asarray(y_train, dtype=np.float64)
        n_features = x_tr.shape[1]

        has_test = X_test is not None and y_test is not None
        if has_test:
            x_te = np.asarray(X_test, dtype=np.float64)
            if x_te.ndim == 1:
                x_te = x_te.reshape(-1, 1)
            y_te = np.asarray(y_test, dtype=np.float64)
        else:
            x_te = None
            y_te = None

        # inicializa a população usando ramped half-and-half
        population = ramped_half_and_half(
            pop_size=self.pop_size,
            num_features=n_features,
            min_depth=2,
            max_depth=5,
            rng=rng,
        )

        tracker = MetricTracker()
        self.best_individual_ = None
        self.best_train_rmse_ = float("inf")
        cur_preds: Optional[np.ndarray] = None
        best_train_pred: Optional[np.ndarray] = None

        c_better = 0
        c_worse = 0
        c_equal = 0
        c_rej = 0
        c_tot = 0

        for gen in range(self.generations):
            # avalia predições da população no conjunto de treino
            if cur_preds is None:
                preds = np.array([ind.eval(x_tr) for ind in population])
            else:
                preds = cur_preds
            diffs = preds - y_tr
            fits = np.sqrt(np.mean(diffs ** 2, axis=1))
            fits = np.nan_to_num(fits, nan=1e9, posinf=1e9, neginf=1e9)

            cur_best_idx = int(np.argmin(fits))
            cur_best_fit = float(fits[cur_best_idx])
            cur_worst_fit = float(np.max(fits))
            cur_mean_fit = float(np.mean(fits))

            if self.elitism:
                if cur_best_fit < self.best_train_rmse_:
                    self.best_train_rmse_ = cur_best_fit
                    self.best_individual_ = population[cur_best_idx].copy()
                    best_train_pred = preds[cur_best_idx].copy()
                best_train_rmse = self.best_train_rmse_
            else:
                best_train_rmse = cur_best_fit
                if cur_best_fit < self.best_train_rmse_:
                    self.best_train_rmse_ = cur_best_fit
                    self.best_individual_ = population[cur_best_idx].copy()
                    best_train_pred = preds[cur_best_idx].copy()

            # calcula o rmse de teste para o melhor indivíduo
            if has_test and x_te is not None and y_te is not None:
                active_best = self.best_individual_ if self.elitism else population[cur_best_idx]
                test_preds = active_best.eval(x_te)
                test_rmse_best = float(compute_rmse(y_te, test_preds))
            else:
                test_rmse_best = float(best_train_rmse)

            # métricas de diversidade e complexidade da árvore
            dups = count_duplicates(population)
            unique_cnt = len(population) - dups
            node_counts = [ind.node_count() for ind in population]
            depths = [ind.depth() for ind in population]
            avg_nodes = float(np.mean(node_counts))
            avg_depth = float(np.mean(depths))

            rep_best = self.best_individual_ if (self.elitism and self.best_individual_ is not None) else population[cur_best_idx]
            best_nodes = rep_best.node_count()
            best_depth = rep_best.depth()

            elapsed = float(time.time() - start_time)

            tracker.log_generation(
                gen=gen,
                best_train_rmse=best_train_rmse,
                worst_train_rmse=cur_worst_fit,
                mean_train_rmse=cur_mean_fit,
                best_test_rmse=test_rmse_best,
                duplicate_trees=dups,
                unique_canonical_trees=unique_cnt,
                crossover_better=c_better,
                crossover_worse=c_worse,
                crossover_equal=c_equal,
                crossover_rejected=c_rej,
                crossover_total=c_tot,
                avg_nodes=avg_nodes,
                avg_depth=avg_depth,
                best_nodes=best_nodes,
                best_depth=best_depth,
                elapsed_seconds=elapsed,
            )

            # pula a etapa de reprodução se esta for a geração final
            if gen >= self.generations - 1:
                break

            # constrói a população de descendentes
            offspring: List[Tree] = []
            offspring_preds: List[np.ndarray] = []
            if self.elitism and self.best_individual_ is not None:
                # posição 0 recebe cópia exata do melhor atual
                offspring.append(self.best_individual_.copy())
                if best_train_pred is not None:
                    offspring_preds.append(best_train_pred.copy())
                else:
                    offspring_preds.append(self.best_individual_.eval(x_tr))
                slots_needed = self.pop_size - 1
            else:
                slots_needed = self.pop_size

            # configura o amostrador de seleção
            if self.selection == "tournament":
                def select_parent() -> int:
                    return tournament_selection(fitnesses=fits, k=self.tournament_k, rng=rng)
            elif self.selection == "roulette":
                def select_parent() -> int:
                    return scaled_roulette_selection(fitnesses=fits, rng=rng)
            elif self.selection == "lexicase":
                err_mat = np.abs(diffs)
                def select_parent() -> int:
                    return epsilon_lexicase_selection(error_matrix=err_mat, rng=rng)
            else:
                raise ValueError(f"unknown selection scheme: {self.selection}")

            # redefine contadores de operadores para a reprodução da próxima geração
            next_better = 0
            next_worse = 0
            next_equal = 0
            next_rej = 0
            next_tot = 0

            for _ in range(slots_needed):
                u = float(rng.random())
                if u < self.pc:
                    p1_idx = select_parent()
                    p2_idx = select_parent()
                    p1 = population[p1_idx]
                    p2 = population[p2_idx]
                    child, success = subtree_crossover(
                        p1,
                        p2,
                        max_depth=self.max_depth,
                        rng=rng,
                        return_success=True,
                    )
                    offspring.append(child)

                    # avalia a melhoria do descendente de cruzamento em relação aos pais
                    if not success:
                        next_rej += 1
                        child_pred = preds[p1_idx].copy()
                        child_fit = float(fits[p1_idx])
                    else:
                        child_pred = child.eval(x_tr)
                        diff_c = child_pred - y_tr
                        child_fit = float(np.sqrt(np.mean(diff_c ** 2)))
                    offspring_preds.append(child_pred)
                    parent_avg_fit = (fits[p1_idx] + fits[p2_idx]) / 2.0

                    if child_fit < parent_avg_fit - 1e-9:
                        next_better += 1
                    elif child_fit > parent_avg_fit + 1e-9:
                        next_worse += 1
                    else:
                        next_equal += 1
                    next_tot += 1

                elif u < self.pc + self.pm:
                    p_idx = select_parent()
                    child = mutate(
                        population[p_idx],
                        num_features=n_features,
                        max_depth=self.max_depth,
                        rng=rng,
                    )
                    offspring.append(child)
                    offspring_preds.append(child.eval(x_tr))

                else:
                    p_idx = select_parent()
                    child = reproduce(population[p_idx])
                    offspring.append(child)
                    offspring_preds.append(preds[p_idx].copy())

            population = offspring
            cur_preds = np.array(offspring_preds)
            c_better = next_better
            c_worse = next_worse
            c_equal = next_equal
            c_rej = next_rej
            c_tot = next_tot

        self.history_ = tracker.get_history()
        return {
            "best": self.best_individual_,
            "history": self.history_,
            "best_train_rmse": self.best_train_rmse_,
        }

    def predict(self, X: np.ndarray) -> np.ndarray:
        """avalia o melhor indivíduo na matriz de atributos fornecida"""
        if self.best_individual_ is None:
            raise RuntimeError("engine has not been fitted yet")
        return self.best_individual_.eval(X)
