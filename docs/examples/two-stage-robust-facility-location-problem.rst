.. _example_robust_flp_ccg:

Two-Stage Robust Facility Location Problem (CCG)
================================================



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

Here is one possible implementation of the problem with idol and solved with MibS.

.. literalinclude:: ../../examples/bilevel-optimization/bilevel.example.cpp
    :language: cpp
