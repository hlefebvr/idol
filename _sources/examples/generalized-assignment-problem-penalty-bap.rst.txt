Combining branch-and-price, column generation, and a penalty method for the generalized assignment problem
==========================================================================================================

Problem Definition
------------------

We consider the Generalized Assignment Problem (GAP) already studied in the :ref:`branch-and-price example <example_gap_bap>`.

In this example, we solve the continuous relaxation of the GAP using a penalty method and column generation.
Then possibly branch on the integer variables to solve the mixed-integer problem.

First, we consider the continuous relaxation of the GAP:

.. math::

    \begin{align*}
        \min_{x} \quad & \sum_{i=1}^m \sum_{j=1}^n c_{ij} x_{ij} \\
        \text{s.t.} \quad & \sum_{j=1}^n r_{ij} x_{ij}, \le C_i && i=1,\dotsc,m, \\
                    & \sum_{i=1}^m x_{ij} = 1, && j=1,\dotsc,n, \\
                    & x_{ij} \in [0,1], && i=1,\dotsc,m, j=1,\dotsc,n.
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

.. literalinclude:: ../../examples/mixed-integer/assignment-penalty-bap.example.cpp
    :language: cpp
