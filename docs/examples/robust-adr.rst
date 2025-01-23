Using affine decision rules to solve a two-stage facility location problem with facility disruption
===================================================================================================

Problem Definition
------------------


We consider a facility location problem with facility disruption.
Given a set of potential facility locations :math:`V_1` and a set of customers :math:`V_2`, the goal is to select a subset of facility locations
to activate in order to serve all customers' demand, while minimizing the total cost.
This version considers random disruptions of the opened facilities making them unusable.

Note that there is also an example for the :ref:`deterministic version of the FLP using Column Generation <example_flp>`.

Each facility :math:`i\in V_1` has an opening cost :math:`f_i` and a maximum capacity :math:`q_i`.
Each customer :math:`j\in V_2` has a demand :math:`d_j`.
The unitary cost for serving customer :math:`j\in V_2` from facility :math:`i\in V_1` is :math:`t_{ij}`.
The maximum number of facilities that can be disrupted is noted :math:`\Gamma`.

The two-stage robust facility location problem can be formulated as

.. math::

    \begin{align}
        \min_{ x\in \{0,1\}^{|V_1|} } \quad & \left\{ \sum_{i\in V_1} f_i x_i + \max_{\xi\in \Xi} \min_{y\in Y(x,\xi)} \quad \sum_{i\in V_1} \sum_{j\in V_2} t_{ij} y_{ij} \right\},
    \end{align}
in which :math:`\Xi := \left\{ \xi\in\{0,1\}^{|V_1|} : e^\top\xi \le \Gamma \right\}` and :math:`Y(x,\xi)` is the feasible set of the second-stage problem defined as

.. math::

    \begin{align}
        & \sum_{j\in V_2} d_j y_{ij} \le q_i, && i\in V_1,\\
        & \sum_{i\in V_1} y_{ij} \ge 1, && j\in V_2, \\
        & y_{ij} \le x_i && i\in V_1, j\in V_2, \\
        & y_{ij} \le 1 - \xi_i && i\in V_1, j\in V_2, \\
        & x_i \in \{0,1\}, && i\in V_1, \\
        & y_{ij} \in \{0,1\}, && i\in V_1, j\in V_2.
    \end{align}

In this example, we will approximate the robust problem using affine decision rules and solve it using Gurobi.
To this end, we will restrict the adjustable decisions to be affine functions of the uncertain parameters, i.e., :math:`y(\xi) = \bar y + Y\xi`.

Implementation
--------------

.. literalinclude:: ../../examples/robust/adr-facility-location.example.cpp
    :language: cpp
