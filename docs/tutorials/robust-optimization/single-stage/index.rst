.. _robust_single:

Single-stage Robust Optimization
================================

A classic robust optimization model can be formulated as follows:

.. math::

    \min_{x} \quad & c^\top x \\
    \text{s.t.} \quad & \sum_{i=1}^j (\bar a_{ij} + \hat a_{ij}u_j)x_j \ge b_i \qquad \text{for all }u\in U_i, \qquad i=1,\dotsc,m,  \\
     & x\in X.

Here:

- :math:`x` represents the decision variables.
- :math:`c^\top x` is the objective function to be minimized.
- The constraints involve uncertain coefficients :math:`a_{ij}`, which are modeled as :math:`\bar a_{ij} + \hat a_{ij} u_j`, where :math:`\bar a_{ij}` is the nominal value, and :math:`\hat a_{ij} u_j` represents the uncertainty.
- :math:`U_i` is the uncertainty set, specifying the range of possible values for the uncertain parameters :math:`u_j`.
- The goal is to find a solution :math:`x` that satisfies the constraints for all possible values of :math:`u` within :math:`U_i` for row :math:`i`.

This formulation ensures that the solution is robust against the worst-case realizations of the uncertain parameters within the given uncertainty set.
The choice of :math:`U` significantly impacts the conservatism of the solution: a larger uncertainty set leads to a more
robust but potentially more conservative solution, while a smaller set provides less protection against extreme variations.


.. toctree::
    :maxdepth: 3
    :glob:


