.. _example_bilevel_pessimistic:

Solving a pessimistic bilevel problem using an optimistic reformulation
=======================================================================

Problem Definition
------------------

This example is taken from :cite:`Zeng2020` (example 1) and is an LP-LP bilevel problem.

.. math::

       \begin{align}
           \min_{x} \quad & -8 x_1 - 6x_2 + \max_{ y\in S(x) }  - 25 y_1 - 30 y_2 + 2 y_3 + 16 y_4 \\
           \text{s.t.} \quad & x_1 + x_2 \le 10, \\
           & x_1, x_2 \ge 0,
       \end{align}

where :math:`S(x)` is defined as

.. math::
       \begin{align}
           S(x) := \underset{y}{\text{arg min}} \ & -10 y_1 - 10 y_2 - 10 y_3 - 10 y_4 \\
           \text{s.t.} \ & y_1 + y_2 + y_3 + y_4 \le 10 - x_1 - x_2, \\
           & -y_1 + y_4 \le 0.8 x_1 + 0.8 x_2, \\
           & y_2 + y_4 \le 4 x_2, \\
           & y_1, y_2, y_3, y_4 \ge 0.
       \end{align}


In this example, we will reformulate this pessimistic bilevel problem as an optimistic one. Then, we will solve it
using its KKT reformulation. The resulting problem will be solved by Gurobi as an MILP.

.. hint::

    Here, the optimistic reformulation reads as follows.

    .. math::

        \begin{align}
           \min_{x, \bar y, y} \quad & -8 x_1 - 6x_2  - 25 y_1 - 30 y_2 + 2 y_3 + 16 y_4 \\
           \text{s.t.} \quad & x_1 + x_2 \le 10, \\
           & x_1, x_2 \ge 0, \\
           & \bar y_1 + \bar y_2 + \bar y_3 + \bar y_4 \le 10 - x_1 - x_2, \\
           & -\bar y_1 + \bar y_4 \le 0.8 x_1 + 0.8 x_2, \\
           & \bar y_2 + \bar y_4 \le 4 x_2, \\
           & \bar y_1, \bar y_2, \bar y_3, \bar y_4 \ge 0, \\
           & y \in \tilde S(x, \bar y),
       \end{align}

    in which :math:`\tilde S(x)` is defined as

    .. math::
           \begin{align}
               S(x) := \underset{y}{\text{arg min}} \quad & 25 y_1 + 30 y_2 - 2 y_3 - 16 y_4 \\
               \text{s.t.} \quad & y_1 + y_2 + y_3 + y_4 \le 10 - x_1 - x_2, \\
               & -y_1 + y_4 \le 0.8 x_1 + 0.8 x_2, \\
               & y_2 + y_4 \le 4 x_2, \\
               & -10 y_1 - 10 y_2 - 10 y_3 - 10 y_4 \le -10 \bar y_1 - 10 \bar y_2 - 10 \bar y_3 - 10 \bar y_4, \\
               & y_1, y_2, y_3, y_4 \ge 0.
           \end{align}

Implementation
--------------

.. literalinclude:: ../../examples/bilevel/pessimistic.example.cpp
    :language: cpp
