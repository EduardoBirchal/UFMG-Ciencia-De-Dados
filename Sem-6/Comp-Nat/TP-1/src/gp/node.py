"""representação e operações do nó da árvore sintática"""

from typing import Optional, List, Union
import numpy as np

from src.gp.primitives import (
    safe_div,
    safe_log,
    safe_exp,
)


class Node:
    """nó da árvore sintática representando um operador, variável ou constante"""

    def __init__(
        self,
        op: str,
        children: Optional[List["Node"]] = None,
        value: Optional[Union[float, int]] = None,
    ):
        self.op = op
        self.children = children if children is not None else []
        self.value = value

    def depth(self) -> int:
        """calcula a profundidade da árvore a partir deste nó onde a folha tem profundidade um"""
        if not self.children:
            return 1
        if len(self.children) == 1:
            return 1 + self.children[0].depth()
        d0 = self.children[0].depth()
        d1 = self.children[1].depth()
        return 1 + (d0 if d0 >= d1 else d1)

    def node_count(self) -> int:
        """conta o total de nós na subárvore enraizada aqui"""
        if not self.children:
            return 1
        if len(self.children) == 1:
            return 1 + self.children[0].node_count()
        return 1 + self.children[0].node_count() + self.children[1].node_count()

    def copy(self) -> "Node":
        """retorna um clone profundo da subárvore"""
        return Node(
            op=self.op,
            children=[c.copy() for c in self.children],
            value=self.value,
        )

    def to_string(self) -> str:
        """serialização canônica em string para detecção de duplicatas"""
        if self.op == "const":
            return f"{float(self.value):.4f}"
        if self.op == "var":
            return f"x{self.value}"
        if len(self.children) == 1:
            return f"({self.op} {self.children[0].to_string()})"
        return f"({self.op} {self.children[0].to_string()} {self.children[1].to_string()})"

    def __str__(self) -> str:
        return self.to_string()

    def __repr__(self) -> str:
        return self.to_string()

    def get_all_nodes(self) -> List["Node"]:
        """retorna todos os nós em ordem prefixa"""
        nodes: List["Node"] = []

        def _collect(n: "Node") -> None:
            nodes.append(n)
            for c in n.children:
                _collect(c)

        _collect(self)
        return nodes

    def get_constants(self) -> List["Node"]:
        """retorna todos os nós constantes em ordem prefixa"""
        return [n for n in self.get_all_nodes() if n.op == "const"]

    def eval(self, X: np.ndarray) -> np.ndarray:
        """avaliação vetorizada na matriz de atributos"""
        if self.op == "const":
            return np.full(len(X), float(self.value), dtype=np.float64)

        if self.op == "var":
            return X[:, int(self.value)]

        if len(self.children) == 1:
            c0 = self.children[0].eval(X)
            if self.op == "sin":
                return np.sin(c0)
            elif self.op == "cos":
                return np.cos(c0)
            elif self.op == "log":
                return safe_log(c0)
            elif self.op == "exp":
                return safe_exp(c0)
            else:
                raise ValueError(f"unknown op: {self.op}")

        c0 = self.children[0].eval(X)
        c1 = self.children[1].eval(X)

        if self.op == "+":
            return np.add(c0, c1)
        elif self.op == "-":
            return np.subtract(c0, c1)
        elif self.op == "*":
            return np.multiply(c0, c1)
        elif self.op == "/":
            return safe_div(c0, c1)
        else:
            raise ValueError(f"unknown op: {self.op}")


def replace_subtree(root: Node, target_idx: int, new_subtree: Node) -> Node:
    """substituição funcional imutável de subárvore no índice em ordem prefixa"""
    curr = 0

    def helper(node: Node) -> Node:
        nonlocal curr
        this_idx = curr
        curr += 1
        if this_idx == target_idx:
            return new_subtree.copy()
        return Node(node.op, [helper(c) for c in node.children], node.value)

    return helper(root)

