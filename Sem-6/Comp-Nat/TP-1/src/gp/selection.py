"""mecanismos de seleção incluindo torneio, roleta e epsilon lexicase"""

from typing import Optional
import numpy as np


def tournament_selection(
    fitnesses: np.ndarray,
    k: int = 2,
    rng: Optional[np.random.Generator] = None,
) -> int:
    """seleciona o índice vencedor a partir de k candidatos aleatórios pelo menor fitness rmse"""
    generator = rng if rng is not None else np.random.default_rng()
    fits = np.asarray(fitnesses, dtype=np.float64)
    n_candidates = len(fits)

    if k == 2:
        i1 = int(generator.integers(0, n_candidates))
        i2 = int(generator.integers(0, n_candidates))
        return i1 if fits[i1] <= fits[i2] else i2

    # amostra k candidatos com reposição
    k_eff = max(1, min(k, n_candidates))
    selected = generator.integers(0, n_candidates, size=k_eff)
    selected_fits = fits[selected]
    best_sub_idx = int(np.argmin(selected_fits))
    return int(selected[best_sub_idx])


def scaled_roulette_selection(
    fitnesses: np.ndarray,
    delta: float = 1e-4,
    rng: Optional[np.random.Generator] = None,
) -> int:
    """seleção por roleta escalada com deslocamento linear para minimização"""
    generator = rng if rng is not None else np.random.default_rng()
    fits = np.asarray(fitnesses, dtype=np.float64)
    n = len(fits)

    f_max = float(np.max(fits))
    # deslocamento linear: o melhor indivíduo recebe maior peso, o pior recebe delta
    weights = (f_max - fits) + delta
    weights = np.maximum(weights, delta)

    total_w = float(np.sum(weights))
    if not np.isfinite(total_w) or total_w <= 0.0:
        probs = np.full(n, 1.0 / n, dtype=np.float64)
    else:
        probs = weights / total_w
        # garante que a precisão numérica some um
        probs = probs / np.sum(probs)

    return int(generator.choice(n, p=probs))


def _fast_median(x: np.ndarray) -> float:
    """calcula a mediana usando partição quickselect"""
    n = len(x)
    if n == 1:
        return float(x[0])
    if n == 2:
        return float((x[0] + x[1]) * 0.5)
    half = n // 2
    if n % 2 == 1:
        return float(np.partition(x, half)[half])
    part = np.partition(x, (half - 1, half))
    return float((part[half - 1] + part[half]) * 0.5)


def epsilon_lexicase_selection(
    error_matrix: np.ndarray,
    rng: Optional[np.random.Generator] = None,
) -> int:
    """seleção epsilon lexicase vetorizada com limiar mad dinâmico"""
    generator = rng if rng is not None else np.random.default_rng()
    errors = np.asarray(error_matrix, dtype=np.float64)
    n_candidates, n_cases = errors.shape

    candidates = np.arange(n_candidates)
    case_order = generator.permutation(n_cases)

    for case_idx in case_order:
        if len(candidates) <= 1:
            break

        cand_errs = errors[candidates, case_idx]
        e_star = float(np.min(cand_errs))
        med = _fast_median(cand_errs)
        mad = _fast_median(np.abs(cand_errs - med))

        # filtra candidatos dentro da margem de mad
        threshold = e_star + mad + 1e-12
        candidates = candidates[cand_errs <= threshold]

    if len(candidates) == 1:
        return int(candidates[0])
    return int(generator.choice(candidates))

