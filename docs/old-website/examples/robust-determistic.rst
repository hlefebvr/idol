Solving a robust facility location problem by its deterministic equivalent
===========================================================================

Problem Definition
------------------

We consider a facility location problem with uncertain demand.
Given a set of potential facility locations :math:`V_1` and a set of customers :math:`V_2`, the goal is to select a subset of facility locations
to activate in order to serve all customers' demand, while minimizing the total cost.
This version introduces uncertainty in the customers' demands.

Note that there is also an example for the :ref:`deterministic version of the FLP using Column Generation <example_flp>`.

Each facility :math:`i\in V_1` has an opening cost :math:`f_i` and a maximum capacity :math:`q_i`.
Each customer :math:`j\in V_2` has a demand :math:`d_j`.
The unitary cost for serving customer :math:`j\in V_2` from facility :math:`i\in V_1` is :math:`t_{ij}`.
The uncertainty in customer demands is controlled by a parameter :math:`\Gamma`.

In this robust variant, we consider that the demands are uncertain and can be expressed as :math:`d_j(\xi) = d_j(1 + 0.2 \xi_j)`
with :math:`\xi` being an unknown vector taken in the uncertainty set

.. math::

    \Xi := \left\{ \xi\in[ 0, 1 ]^{|V_2|} : \sum_{j\in V_2} \xi_j \le \Gamma \right\}.

The goal is to minimize the total cost of opening facilities and serving customers, considering the worst-case demand scenario.

The robust version can be formulated as

.. math::

    \begin{align*}
        \min_{x,y} \quad & \sum_{i\in V_1} f_i x_i + \sum_{i\in V_1} \sum_{j\in V_2} t_{ij} y_{ij} \\
        \text{s.t.} \quad & \sum_{j\in V_2} d_j(\xi) y_{ij} \le q_i, && i\in V_1, \quad \text{for all }\xi\in\Xi, \\
                          & \sum_{i\in V_1} y_{ij} = 1, && j\in V_2, \\
                          & y_{ij} \le x_i && i\in V_1, j\in V_2, \\
                          & x_i \in \{0,1\}, && i\in V_1, \\
                          & y_{ij} \in \{0,1\}, && i\in V_1, j\in V_2.
    \end{align*}

In this example, we will reformulate this robust facility location problem as a deterministic one. Then, we will solve it using Gurobi.

.. hint::

    Here, the deterministic reformulation reads as follows.

    .. math::

        \begin{align*}
            \min_{x,y,u.v} \quad & \sum_{i\in V_1} f_i x_i + \sum_{i\in V_1} \sum_{j\in V_2} t_{ij} y_{ij} \\
            \text{s.t.} \quad & \sum_{j\in V_2} d_j y_{ij} + \Gamma u + \sum_{j\in V_2} v_j \le q_i, && i\in V_1, \\
                              & u + v_j \ge 0.2 d_j y_j, && j\in V_2, \\
                              & \sum_{i\in V_1} y_{ij} = 1, && j\in V_2, \\
                              & u, v_j \ge 0, && j\in V_2, \\
                              & y_{ij} \le x_i && i\in V_1, j\in V_2, \\
                              & x_i \in \{0,1\}, && i\in V_1, \\
                              & y_{ij} \in \{0,1\}, && i\in V_1, j\in V_2.
        \end{align*}


Implementation
--------------

.. literalinclude:: ../../../examples/robust/robust-facility-location.example.cpp
    :language: cpp
