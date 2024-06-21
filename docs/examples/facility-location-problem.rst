.. _example_flp:

Facility Location Problem
=========================

Problem Definition
------------------

We consider the capacitated Facility Location Problem (FLP).
Given a set of potential facility locations :math:`V_1` and a set of customers :math:`V_2`, the goal is to select
a subset of facility location to activate in order to serve all customers' demand, while minimizing the total cost.

Each facility :math:`i\in V_1` has an opening cost :math:`f_i` and a maximum capacity :math:`q_i`.
Each customer :math:`j\in V_2` has a demand :math:`d_j`.
The unitary cost for serving customer :math:`j\in V_2` from facility :math:`i\in V_1` is :math:`t_{ij}`.

We model the capacitated FLP with the MILP

.. math::

    \begin{align*}
        \min_{x,y} \ & \sum_{i\in V_1} f_i x_i + \sum_{i\in V_1} \sum_{j\in V_2} t_{ij} y_{ij} \\
        \text{s.t.} & \sum_{j\in V_2} d_j y_{ij} \le q_i && i\in V_1 \\
                    & \sum_{i\in V_1} y_{ij} = 1 && j\in V_2 \\
                    & y_{ij} \le x_i && i\in V_1, j\in V_2 \\
                    & x_i \in \{0,1\} && i\in V_1 \\
                    & y_{ij} \in \{0,1\} && i\in V_1, j\in V_2.
    \end{align*}

Implementation with idol
------------------------

In this example, we show how to model the capacitated FLP with idol and how to solve it using a "hand-crafted"
branch-and-bound algorithm.

.. literalinclude:: ../../examples/mixed-integer-optimization/facility.example.cpp
    :language: cpp
