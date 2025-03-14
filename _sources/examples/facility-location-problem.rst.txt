.. _example_flp:

Solving a facility location problem with a custom branch-and-bound
==================================================================

Problem Definition
------------------

We consider the capacitated Facility Location Problem (FLP).
Given a set of potential facility locations :math:`V_1` and a set of customers :math:`V_2`, the goal is to select
a subset of facility location to activate in order to serve all customers' demand, while minimizing the total cost.

Each facility :math:`i\in V_1` has an opening cost :math:`f_i` and a maximum capacity :math:`q_i`.
Each customer :math:`j\in V_2` has a demand :math:`d_j`.
The unitary cost for serving customer :math:`j\in V_2` from facility :math:`i\in V_1` is :math:`t_{ij}`.

We model the capacitated FLP with the MILP:

.. math::

    \begin{align*}
        \min_{x,y} \quad & \sum_{i\in V_1} f_i x_i + \sum_{i\in V_1} \sum_{j\in V_2} t_{ij} y_{ij} \\
        \text{s.t.} \quad & \sum_{j\in V_2} d_j y_{ij} \le q_i, && i\in V_1, \\
                          & \sum_{i\in V_1} y_{ij} = 1, && j\in V_2, \\
                          & y_{ij} \le x_i && i\in V_1, j\in V_2, \\
                          & x_i \in \{0,1\}, && i\in V_1, \\
                          & y_{ij} \in \{0,1\}, && i\in V_1, j\in V_2.
    \end{align*}

Instance
--------

We will use an instance stored in a file called `facility.data.txt`. This file reads

.. literalinclude:: ../../examples/mixed-integer/facility.data.txt

On the first line, the number of facilities and the numbers of customers are specified.
Then, for each facility, the opening cost and the capacity are given.
For each customer, the demand is given.
Finally, the cost of serving each customer from each facility is given.


Implementation
--------------

.. literalinclude:: ../../examples/mixed-integer/facility.example.cpp
    :language: cpp
