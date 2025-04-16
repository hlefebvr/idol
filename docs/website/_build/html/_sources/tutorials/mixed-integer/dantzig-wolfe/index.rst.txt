.. _tutorial_DantzigWolfe:

Column Generation and Branch-and-Price Algorithm
================================================

**Column generation** is an algorithm used to solve large scale optimization problems which involve a large number of variables.
Instead of solving the problem in its entirety, column generation begins with a restricted version of the problem containing only a subset of variables. It then iteratively introduces new variables (or columns) that have the potential to improve the solution, based on the problem's structure and constraints.

In practice, column generation is particularly effective for problems with block-angular structures, such as cutting stock, vehicle routing, and generalized assignment problems. The method splits the problem into a master problem, which handles the selection of variables, and a pricing problem, which identifies the most promising variables to add to the master problem. These subproblems are solved iteratively until no further improvements are possible.

.. figure:: https://upload.wikimedia.org/wikipedia/commons/e/ec/DW_Block_Angular_Matrix.jpg
    :align: center
    :target: https://en.wikipedia.org/wiki/Dantzig%E2%80%93Wolfe_decomposition

    Block-angular matrix structure particularly suited for column generation.

The **Branch-and-Price** algorithm extends column generation to integer programming. While column generation operates on the linear relaxation of the problem, Branch-and-Price integrates it into a Branch-and-Bound framework to handle integer constraints. This combination allows efficient exploration of the solution space while exploiting the power of decomposition techniques. Each node of the Branch-and-Bound tree employs column generation to solve the relaxed problem, providing tight bounds for pruning and accelerating convergence.

In this series of tutorials, we will see how column generation and, more precisely, **Dantzig-Wolfe decomposition** can be implemented in idol.

.. hint::

    For more details on column generation, we refer to the book :cite:`uchoa2024optimizing`.

.. toctree::
    :maxdepth: 1
    :glob:

    dantzig-wolfe
    writing-branch-and-price
    strong-branching
