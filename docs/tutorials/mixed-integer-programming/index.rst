.. _mip:

Mixed-Integer Optimization
==========================

Mixed-integer optimization deals with problems of the following form:

.. math::
    :label: eq:mip

    \begin{align}
        \min_{x} \quad & c^\top x + x^\top D x + c_0 \\
        \text{s.t.} \quad & a_{i\cdot}^\top x + x^\top Q^i x \le b_i, \quad i = 1, \ldots, m, \\
        & x_j \in \mathbb{Z}, \quad j \in I \subseteq \{ 1, \dotsc, n \}.
    \end{align}

Here, :math:`x` are the decision variables, while :math:`c`, :math:`D`, :math:`c_0`, :math:`a_i`, :math:`Q^i`, and :math:`b_i` are given data.
Some of the decision variables are required to be integer-valued and are indexed by :math:`I`.

We say that :math:numref:`eq:mip` is an integer problem if :math:`I = \{ 1, \dotsc, n \}`, i.e., if all variables are required to
be integer. It is said to be mixed-integer if :math:`I \neq \emptyset`. Otherwise, we say that :math:numref:`eq:mip` is a continuous problem.

An important class of problems is when :math:`D = 0` and :math:`Q^i = 0` for all :math:`i`, i.e., when the objective function and the constraints are linear.
In such a case, we say that :math:numref:`eq:mip` is a mixed-integer linear problem (MILP), or a linear problem (LP) if all variables are continuous.

**Table of Contents**

.. toctree::
    :maxdepth: 2
    :glob:

    modeling/index
    optimizers/index
    branch-and-bound/index
    dantzig-wolfe/index
