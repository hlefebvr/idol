.. _example_bilevel_kkt:

Solving an optimistic LP-LP bilevel problem with KKT reformulation
==================================================================

Problem Definition
------------------

This example is taken from :cite:`Kleinert2021` and is an LP-LP bilevel problem.

The problem is formulated as follows:

.. math::

    \begin{align}
        \min_{x, y} \quad & x + 6 y \\
        \text{s.t.} \quad & -x + 5y \le 12.5, \\
        & x \ge 0, \\
        & y\in
            \begin{array}[t]{rl}
                \displaystyle \underset{y}{\text{arg min}} \quad & -y \\
                \text{s.t.} \quad & 2 x - y \ge 0, \\
                & -x - y \ge -6, \\
                & -x + 6 y \ge -3, \\
                & x + 3 y \ge 3.
            \end{array}
    \end{align}

In this example, we will reformulate the bilevel problem as a single-level problem using the KKT conditions.
The resulting problem will be solved by Gurobi as an NLP. Note that it is also possible to provide valid big-Ms to
reformulate this problem as an MILP. Check out our tutorials to learn more.

Implementation
--------------

.. literalinclude:: ../../examples/bilevel/kkt.example.cpp
    :language: cpp
