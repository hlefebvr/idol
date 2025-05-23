.. _example_gap_bap:

Solving a generalized assignment problem by column generation and branch-and-price
==================================================================================

Problem Definition
------------------

Given a set of :math:`m` agents and :math:`n` jobs, the goal is to assign each job to exactly one agent in such a
way that the total cost is minimized, while respecting the capacity constraints of each agent.

Each agent :math:`i\in\{1,\dotsc,m\}` has a capacity :math:`C_i`.
Each job :math:`j\in\{1,\dotsc,n\}` has a resource consumption :math:`r_{ij}` and a cost :math:`c_{ij}` when assigned to agent :math:`i`.

We model the GAP with the following binary linear program:

.. math::

    \begin{align*}
        \min_{x} \quad & \sum_{i=1}^m \sum_{j=1}^n c_{ij} x_{ij} \\
        \text{s.t.}  \quad & \sum_{j=1}^n r_{ij} x_{ij} \le C_i, && i=1,\dotsc,m, \\
                    & \sum_{i=1}^m x_{ij} = 1, && j=1,\dotsc,n, \\
                    & x_{ij} \in \{0,1\}, && i=1,\dotsc,m, j=1,\dotsc,n.
    \end{align*}

Decomposition
-------------

In this example, we use a Dantzig-Wolfe decomposition to break down the problem into a master problem and as many subproblems as agents.
The master problem coordinates the assignment of jobs to agents, while the subproblems handle the capacity constraints for each agent individually.

1. **Master Problem:** The master problem is responsible for ensuring that each job is assigned to exactly one agent. It maintains the overall objective of minimizing the total cost.

2. **Subproblems:** Each subproblem corresponds to an agent and ensures that the agent's capacity constraints are respected. The subproblems are solved independently and their solutions are used to update the master problem.

Implementation
--------------

.. literalinclude:: ../../../examples/mixed-integer/assignment-bap.example.cpp
    :language: cpp
