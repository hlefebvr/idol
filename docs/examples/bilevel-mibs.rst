.. _example_mibs:

Bilevel MILP-MILP (MibS)
========================

Problem Definition
------------------

This example is taken from :cite:`Moore1990` and is a bilevel problem where the upper level is a mixed-integer linear program (MILP) and the lower level is a mixed-integer linear program (MILP).

The problem is formulated as follows:

.. math::

    \begin{align}
        \min_{x, y} \ & -x + -10 y \\
        \text{s.t.} \ & x \in \mathbb Z_+ \\
        & y\in
            \begin{array}[t]{l}
                \displaystyle \underset{y}{\text{arg min}} \ & y \\
                \text{s.t.} \ & -25 x + 20 y \leq 30, \\
                & x + 2 y \leq 10, \\
                & 2 x - y \leq 15, \\
                & 2 x + 10 y \geq 15, \\
                & y \geq 0, \\
                & y \in \mathbb Z_+.
            \end{array}
    \end{align}

Implementation with idol
------------------------

In this example, we show how to model this MILP-MILP bilevel problem and how to solve it using the MibS solver.

.. literalinclude:: ../../examples/bilevel-optimization/mibs.example.cpp
    :language: cpp
