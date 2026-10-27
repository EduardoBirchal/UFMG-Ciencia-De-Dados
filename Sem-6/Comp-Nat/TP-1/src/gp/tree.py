"""contêiner de árvore sintática e métodos de inicialização de população"""

from typing import Optional, List
import numpy as np

from src.gp.node import Node


BINARY_OPS = ["+", "-", "*", "/"]
UNARY_OPS = ["sin", "cos", "log", "exp"]
ALL_OPS = BINARY_OPS + UNARY_OPS


class Tree:
    """árvore individual encapsulando o nó raiz com avaliação e métricas"""

    def __init__(self, root: Node):
        self.root = root
        self._depth: Optional[int] = None
        self._node_count: Optional[int] = None
        self._str: Optional[str] = None

    def depth(self) -> int:
        """calcula a profundidade máxima da árvore"""
        if self._depth is None:
            self._depth = self.root.depth()
        return self._depth

    def node_count(self) -> int:
        """calcula a contagem total de nós na árvore"""
        if self._node_count is None:
            self._node_count = self.root.node_count()
        return self._node_count

    def copy(self) -> "Tree":
        """retorna um clone profundo da árvore"""
        clone = Tree(self.root.copy())
        clone._depth = self._depth
        clone._node_count = self._node_count
        clone._str = self._str
        return clone

    def to_string(self) -> str:
        """retorna a representação canônica serializada em string"""
        if self._str is None:
            self._str = self.root.to_string()
        return self._str

    def __str__(self) -> str:
        return self.to_string()

    def __repr__(self) -> str:
        return self.to_string()

    def eval(self, X: np.ndarray) -> np.ndarray:
        """avalia a árvore sobre a matriz de entrada retornando predições unidimensionais limitadas"""
        res = self.root.eval(X)
        return np.clip(res, -1e10, 1e10)


def _make_terminal(num_features: int, rng: np.random.Generator) -> Node:
    """amostra um nó folha terminal com 50% de variável e 50% de constante"""
    if rng.random() < 0.5:
        var_idx = int(rng.integers(0, num_features))
        return Node("var", value=var_idx)
    val = float(rng.uniform(-5.0, 5.0))
    return Node("const", value=round(val, 4))


def _build_full_node(cur_depth: int, max_depth: int, num_features: int, rng: np.random.Generator) -> Node:
    """constrói recursivamente o nó da árvore completa garantindo a profundidade exata"""
    if cur_depth >= max_depth:
        return _make_terminal(num_features, rng)
    op = str(rng.choice(ALL_OPS))
    if op in BINARY_OPS:
        left = _build_full_node(cur_depth + 1, max_depth, num_features, rng)
        right = _build_full_node(cur_depth + 1, max_depth, num_features, rng)
        return Node(op, children=[left, right])
    child = _build_full_node(cur_depth + 1, max_depth, num_features, rng)
    return Node(op, children=[child])


def full_tree(
    depth: int,
    num_features: int = 1,
    rng: Optional[np.random.Generator] = None,
) -> Tree:
    """gera árvore onde todas as folhas atingem a profundidade alvo exata"""
    generator = rng if rng is not None else np.random.default_rng()
    if depth <= 1:
        root = _make_terminal(num_features, generator)
    else:
        root = _build_full_node(1, depth, num_features, generator)
    return Tree(root)


def _build_grow_node(cur_depth: int, max_depth: int, num_features: int, rng: np.random.Generator) -> Node:
    """constrói recursivamente o nó da árvore grow permitindo terminação antecipada das folhas"""
    if cur_depth >= max_depth:
        return _make_terminal(num_features, rng)
    # em níveis intermediários escolhe terminal ou operador com igual probabilidade
    if cur_depth > 1 and rng.random() < 0.5:
        return _make_terminal(num_features, rng)
    op = str(rng.choice(ALL_OPS))
    if op in BINARY_OPS:
        left = _build_grow_node(cur_depth + 1, max_depth, num_features, rng)
        right = _build_grow_node(cur_depth + 1, max_depth, num_features, rng)
        return Node(op, children=[left, right])
    child = _build_grow_node(cur_depth + 1, max_depth, num_features, rng)
    return Node(op, children=[child])


def grow_tree(
    depth: int,
    num_features: int = 1,
    rng: Optional[np.random.Generator] = None,
) -> Tree:
    """gera árvore onde as folhas podem terminar antes da profundidade máxima"""
    generator = rng if rng is not None else np.random.default_rng()
    if depth <= 1:
        root = _make_terminal(num_features, generator)
    else:
        root = _build_grow_node(1, depth, num_features, generator)
    return Tree(root)


def ramped_half_and_half(
    pop_size: int,
    num_features: int = 1,
    min_depth: int = 2,
    max_depth: int = 5,
    rng: Optional[np.random.Generator] = None,
) -> List[Tree]:
    """gera população ao longo das profundidades usando divisão igual entre full e grow"""
    generator = rng if rng is not None else np.random.default_rng()
    depths = list(range(min_depth, max_depth + 1))
    k = len(depths)
    base_per_depth = pop_size // k
    rem = pop_size % k

    population = []
    for i, d in enumerate(depths):
        count = base_per_depth + (1 if i < rem else 0)
        n_full = count // 2
        n_grow = count - n_full
        for _ in range(n_full):
            population.append(full_tree(depth=d, num_features=num_features, rng=generator))
        for _ in range(n_grow):
            population.append(grow_tree(depth=d, num_features=num_features, rng=generator))

    return population

