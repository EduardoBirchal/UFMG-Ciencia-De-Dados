"""carregador de conjuntos de dados para benchmarks de regressão simbólica"""

import os
import numpy as np
import pandas as pd


def load_dataset(dataset_name: str, base_dir: str = ".") -> tuple[np.ndarray, np.ndarray, np.ndarray, np.ndarray]:
    """carrega o conjunto de dados retornando x_train, y_train, x_test, y_test como arrays float64 contíguos"""
    train_path = os.path.join(base_dir, dataset_name, f"{dataset_name}-train.csv")
    test_path = os.path.join(base_dir, dataset_name, f"{dataset_name}-test.csv")

    # carrega tabelas brutas sem cabeçalhos
    train_df = pd.read_csv(train_path, header=None)
    test_df = pd.read_csv(test_path, header=None)

    train_data = train_df.to_numpy(dtype=np.float64)
    test_data = test_df.to_numpy(dtype=np.float64)

    # separa atributos e coluna alvo
    x_train = np.ascontiguousarray(train_data[:, :-1], dtype=np.float64)
    y_train = np.ascontiguousarray(train_data[:, -1], dtype=np.float64)
    x_test = np.ascontiguousarray(test_data[:, :-1], dtype=np.float64)
    y_test = np.ascontiguousarray(test_data[:, -1], dtype=np.float64)

    return x_train, y_train, x_test, y_test

