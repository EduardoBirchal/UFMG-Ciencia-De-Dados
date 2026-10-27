"""primitivas numéricas protegidas para regressão simbólica"""

import numpy as np


def safe_div(x: np.ndarray, y: np.ndarray) -> np.ndarray:
    """divisão protegida retornando 1 para denominador próximo de zero"""
    near_zero = np.abs(y) < 1e-6
    denom = np.where(near_zero, 1.0, y)
    res = np.where(near_zero, 1.0, np.divide(x, denom))
    return np.clip(res, -1e50, 1e50)


def safe_log(x: np.ndarray) -> np.ndarray:
    """logaritmo natural protegido retornando 0 para argumentos próximos de zero"""
    abs_x = np.abs(x)
    near_zero = abs_x < 1e-6
    safe_x = np.where(near_zero, 1.0, abs_x)
    res = np.where(near_zero, 0.0, np.log(safe_x))
    return np.clip(res, -1e50, 1e50)


def safe_exp(x: np.ndarray) -> np.ndarray:
    """exponencial protegida limitada para evitar overflow"""
    clipped = np.clip(x, -32.0, 32.0)
    return np.exp(clipped)

