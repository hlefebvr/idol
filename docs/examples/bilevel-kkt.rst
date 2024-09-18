.. _example_bilevel_kkt:

Bilevel LP-LP (KKT Reformulation)
=================================

Problem Definition
------------------

This example is taken from :cite:`Kleinert2021` and is an LP-LP bilevel problem.

The problem is formulated as follows:

.. math::

    \begin{align}
        \min_{x, y} \ & x + 6 y \\
        \text{s.t.} \ & -x + 5y \le 12.5 \\
        & x \ge 0 \\
        & y\in
            \begin{array}[t]{l}
                \displaystyle \underset{y}{\text{arg min}} \ & -y \\
                \text{s.t.} \ & 2 x - y \ge 0, \\
                & -x - y \ge -6, \\
                & -x + 6 y \ge -3, \\
                & x + 3 y \ge 3.
            \end{array}
    \end{align}

KKT Reformulation with idol
---------------------------

In this example, we show how to model this LP-LP bilevel problem and how to derive its KKT reformulation.

.. literalinclude:: ../../examples/bilevel-optimization/kkt.example.cpp
    :language: cpp
