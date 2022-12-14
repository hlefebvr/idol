IDOL's documentation
====================

.. toctree::
    :maxdepth: 2
    :caption: Contents:

Idol is a C++ library for implementing complex decomposition algorithms for
mathematical optimization.

Its main goal is to facilitate prototyping new algorithms which
can exploit structure in optimization problems (e.g., Dantzig-Wolfe decomposition, Benders decomposition,
nested approaches, ...) or to design complex algorithmic schemes for solving :math:`\Sigma_i^P`-hard problems
(e.g., bilevel problems, adjustable robust optimization, ...).

General approach
----------------

Based on the current state of the art, idol aims at solving complex problems by iteratively
solving a set of so-called subproblems of the following form.

.. math::

    \begin{array}{lll}
        \textrm{minimize } & \displaystyle \sum_{j=1}^n c_jx_j + \sum_{j=1}^n\sum_{k=1}^n d_{jk}x_jx_k \\
        \textrm{subject to } & \displaystyle \sum_{j=1}^n a_{ij}x_j + \sum_{j=1}^n\sum_{k=1}^n q^i_{jk}x_jx_k \le b_i & i=1,...,m \\
        & l_j \le x_j \le u_j & j=1,...,m \\
        & x_j\in\mathbb Z & j\in J_I
    \end{array}

Here, :math:`x_j` are the variables of this optimization problem while :math:`c_j, d_{jk}, a_{ij}` and :math:`q_{jk}^i` are given
input parameters for this model. Optionally, :math:`J_I\subseteq\{1,...,n\}` is a set of variable indices
whose value must be integral.

Idol provides a structured and rigorous way to write custom or new algorithms based on - but not limited to - the current
state-of-the-art algorithms (e.g., Branch-and-price, Benders decomposition with local cuts, ...) by combining and interconnecting
each algorithm together.

An Algorithm in idol is viewed as a "black-box" which can be chained with any other algorithm in any relevant way.

External solvers
----------------

Each subproblem is then solved
by an external and dedicated solver. Currently, the following external solvers can be interfaced:

* Gurobi (see https://www.gurobi.com/ ) ;
* GLPK (see https://www.gnu.org/software/glpk/ ).

Table of Contents
-----------------

.. toctree::
    :maxdepth: 2

    self
    basics/index
    api/index
    dev/index

* :ref:`genindex`
