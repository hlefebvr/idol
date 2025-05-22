.. _example_mibs:

Solving an optimistic MILP-MILP bilevel problem with coin-or/MibS
=================================================================

Problem Definition
------------------

This example is taken from :cite:`Moore1990` and is a bilevel problem where the upper level is a mixed-integer linear program (MILP) and the lower level is a mixed-integer linear program (MILP).

The problem is formulated as follows:

.. math::

    \begin{align}
        \min_{x, y} \quad & -x + -10 y \\
        \text{s.t.} \quad & x \in \mathbb Z_{\ge 0 },\\
        & y\in
            \begin{array}[t]{rl}
                \displaystyle \underset{y}{\text{arg min}} \quad & y \\
                \text{s.t.} \quad & -25 x + 20 y \leq 30, \\
                & x + 2 y \leq 10, \\
                & 2 x - y \leq 15, \\
                & 2 x + 10 y \geq 15, \\
                & y \geq 0, \\
                & y \in \mathbb Z_{\ge 0}.
            \end{array}
    \end{align}

Implementation
--------------

.. literalinclude:: ../../../examples/bilevel/mibs.example.cpp
    :language: cpp
