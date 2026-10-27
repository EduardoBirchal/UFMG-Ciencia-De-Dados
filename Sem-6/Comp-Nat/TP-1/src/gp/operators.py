"""operadores de variação incluindo cruzamento, mutação e reprodução"""

from typing import Optional, Tuple, Union
import numpy as np

from src.gp.node import Node, replace_subtree
from src.gp.tree import Tree, grow_tree


def subtree_crossover(
    parent1: Tree,
    parent2: Tree,
    max_depth: int = 7,
    rng: Optional[np.random.Generator] = None,
    return_success: bool = False,
) -> Union[Tree, Tuple[Tree, bool]]:
    """substitui uma subárvore aleatória no parent1 por uma subárvore aleatória do parent2"""
    generator = rng if rng is not None else np.random.default_rng()

    root1 = parent1.root
    root2 = parent2.root

    nodes1: list = []

    def _collect1(n: Node, lvl: int) -> None:
        nodes1.append((n, lvl))
        for c in n.children:
            _collect1(c, lvl + 1)

    _collect1(root1, 1)
    nodes2 = root2.get_all_nodes()

    idx1 = int(generator.integers(0, len(nodes1)))
    idx2 = int(generator.integers(0, len(nodes2)))

    _, lvl1 = nodes1[idx1]
    donor = nodes2[idx2]

    if (lvl1 - 1 + donor.depth()) > max_depth:
        # rejeita o cruzamento quando a profundidade excede o teto e mantém parent1
        res_root = root1.copy()
        success = False
    else:
        res_root = replace_subtree(root1, idx1, donor.copy())
        success = True

    res = Tree(res_root)
    if return_success:
        return (res, success)
    return res


def subtree_mutation(
    parent: Tree,
    max_depth: int = 7,
    num_features: int = 1,
    rng: Optional[np.random.Generator] = None,
    return_success: bool = False,
) -> Union[Tree, Tuple[Tree, bool]]:
    """substitui um nó aleatório por uma subárvore grow recém-gerada"""
    generator = rng if rng is not None else np.random.default_rng()

    root = parent.root
    nodes: list = []

    def _collect_m(n: Node, lvl: int) -> None:
        nodes.append((n, lvl))
        for c in n.children:
            _collect_m(c, lvl + 1)

    _collect_m(root, 1)
    idx = int(generator.integers(0, len(nodes)))
    _, lvl_m = nodes[idx]

    # escolhe profundidade grow aleatória entre 1 e 3
    grow_depth = int(generator.integers(1, 4))
    donor_tree = grow_tree(depth=grow_depth, num_features=num_features, rng=generator)
    donor = donor_tree.root

    if (lvl_m - 1 + donor.depth()) > max_depth:
        res_root = root.copy()
        success = False
    else:
        res_root = replace_subtree(root, idx, donor)
        success = True

    res = Tree(res_root)
    if return_success:
        return (res, success)
    return res


def gaussian_constant_mutation(
    parent: Tree,
    sigma: float = 0.5,
    rng: Optional[np.random.Generator] = None,
) -> Tree:
    """perturba o valor de um nó constante aleatório com ruído gaussiano"""
    generator = rng if rng is not None else np.random.default_rng()

    clone = parent.copy()
    constants = clone.root.get_constants()
    if not constants:
        return clone

    target = generator.choice(constants)
    noise = float(generator.normal(0.0, sigma))
    target.value = round(float(target.value) + noise, 4)

    return clone


def mutate(
    parent: Tree,
    num_features: int = 1,
    max_depth: int = 7,
    rng: Optional[np.random.Generator] = None,
) -> Tree:
    """despacha entre mutação gaussiana de constante e mutação de subárvore"""
    generator = rng if rng is not None else np.random.default_rng()

    constants = parent.root.get_constants()
    if constants and generator.random() < 0.5:
        return gaussian_constant_mutation(parent, sigma=0.5, rng=generator)

    return subtree_mutation(
        parent,
        max_depth=max_depth,
        num_features=num_features,
        rng=generator,
    )


def reproduce(parent: Tree) -> Tree:
    """clona o indivíduo pai sem modificação"""
    return parent.copy()

