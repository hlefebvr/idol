Knapsack Problem
================

Problem Definition
------------------

We consider the Knapsack Problem (KP).
Given a set of :math:`n` items, each of which having a weight and a profit, the goal is to
select of subset of items such that the total weight does not exceed a given capacity and the total profit is maximized.

For each item :math:`j\in\{1,\dotsc,n\}`, we denote its weight by :math:`w_j` and its profit by :math:`p_j`.
The maximum capacity of the knapsack is :math:`C`.

We model the KP with the following binary linear program:

.. math::

    \begin{align*}
        \max_{x} \ & \sum_{j=1}^n p_j x_j \\
        \text{s.t.} & \sum_{j=1}^n w_j x_j \le C \\
                    & x_j \in \{0,1\} && j=1,\dotsc,n.
    \end{align*}

Implementation with idol
------------------------

In this example, we show how to model the Knapsack Problem with idol and how to solve it using the HiGHS solver.

.. literalinclude:: ../../examples/mixed-integer-optimization/knapsack.example.cpp
    :language: cpp
