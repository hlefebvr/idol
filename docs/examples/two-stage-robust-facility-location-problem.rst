.. _example_robust_flp_ccg:

Two-Stage Robust Facility Location Problem (CCG)
================================================

Problem Description
-------------------

We consider a robust version of the capacitated Facility Location Problem (FLP).
Given a set of potential facility locations :math:`V_1` and a set of customers :math:`V_2`, the goal is to select a subset of facility locations
to activate in order to serve all customers' demand, while minimizing the total cost.
This version introduces uncertainty in the customers' demands.

Note that there is also an example for the :ref:`deterministic version of the FLP using Column Generation <example_flp>`.

Each facility :math:`i\in V_1` has an opening cost :math:`f_i` and a maximum capacity :math:`q_i`.
Each customer :math:`j\in V_2` has a demand :math:`d_j`.
The unitary cost for serving customer :math:`j\in V_2` from facility :math:`i\in V_1` is :math:`t_{ij}`.
The uncertainty in customer demands is controlled by a parameter :math:`\Gamma`.

In this robust variant, we consider that the demands are uncertain and can be expressed as :math:`d_j(\xi) = d_j(1 + p\xi_j)`
with :math:`p` being the maximum increase in demand and :math:`\xi` being an unknown vector taken in the uncertainty set

.. math::

    \Xi := \left\{ \xi\in[ 0, 1 ]^{|V_2|} : \sum_{j\in V_2} \xi_j \le \Gamma \right\}.

We model the two-stage robust FLP as

.. math::

    \min_{x\in \{0,1\}^{|V_1|}} \ \left\{ \sum_{i\in V_1} f_i x_i + \max_{\xi\in \Xi} \ \min_{y\in Y(x,\xi)} \  \sum_{i\in V_1} \sum_{j\in V_2} t_{ij} y_{ij} \right\}

where :math:`Y(x,\xi)` is the set of feasible solutions for the second stage problem, given the first stage solution :math:`x` and the realization :math:`\xi` of the uncertain demand vector.
It is defined as the set of vectors :math:`y\in \mathbb{R}^{|V_1|\times|V_2|}` that satisfy the following constraints

.. math::

    \begin{align*}
        & \sum_{i\in V_1} y_{ij} = d_j(\xi) && j\in V_2, \\
        & \sum_{j\in V_2} y_{ij} \le q_i x_i && i\in V_1, \\
        & y_{ij} \ge 0 && i\in V_1, j\in V_2.
    \end{align*}

Implementation
--------------

.. literalinclude:: ../../examples/robust/robust_ccg.example.cpp
    :language: cpp
