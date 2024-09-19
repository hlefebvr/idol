Generalized Assignment Problem (Column Generation + Penalty Method)
===================================================================

Problem Definition
------------------

We consider the Generalized Assignment Problem (GAP) already studied in the :ref:`Branch-and-Price example <example_gap_bap>`.

In this example, we solve the continuous relaxation of the GAP using a penalty method and column generation.

First, we consider the continuous relaxation of the GAP:

.. math::

    \begin{align*}
        \min_{x} \ & \sum_{i=1}^m \sum_{j=1}^n c_{ij} x_{ij} \\
        \text{s.t.} & \sum_{j=1}^n r_{ij} x_{ij} \le C_i && i=1,\dotsc,m \\
                    & \sum_{i=1}^m x_{ij} = 1 && j=1,\dotsc,n \\
                    & x_{ij} \in [0,1] && i=1,\dotsc,m, j=1,\dotsc,n.
    \end{align*}

Then, we introduce penalty parameters :math:`\rho_j` to penalize the constraints :math:`\sum_{i=1}^m x_{ij} = 1` in the objective function.

.. math::

    \begin{align*}
        \min_{x} \ & \sum_{i=1}^m \sum_{j=1}^n c_{ij} x_{ij} + \sum_{j=1}^n \rho_j \left| \sum_{i=1}^m x_{ij} - 1 \right| \\
        \text{s.t.} & \sum_{j=1}^n r_{ij} x_{ij} \le C_i && i=1,\dotsc,m \\
                    & x_{ij} \in [0,1] && i=1,\dotsc,m, j=1,\dotsc,n.
    \end{align*}

Throughout the optimization process, we update the penalty parameters :math:`\rho_j` iteratively to enforce the feasibility of the solution.

Finally, we solve the continuous relaxation of the GAP using column generation to generate new columns (variables) and improve the objective function.
That is, we iteratively solve the master problem and the subproblems to generate new columns and update the master problem.


Implementation with idol
------------------------

In this example, we show how to model the Generalized Assignment Problem with idol and how to solve it using a
Dantzig-Wolfe decomposition within a branch-and-bound framework, i.e., a branch-and-price algorithm.

.. literalinclude:: ../../examples/mixed-integer-optimization/assignment-penalty-bap.example.cpp
    :language: cpp
