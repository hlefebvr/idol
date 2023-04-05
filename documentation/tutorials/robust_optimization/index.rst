.. _robust:

Robust Optimization
===================

Single-stage robust optimization
--------------------------------

.. math::

    \begin{align}
        \min \ & c^\top x \\
        \textrm{s.t. } & A(\xi)x \ge b & \forall \xi\in\Xi \\
        & x\in X
    \end{align}

Two-stage robust optimization
-----------------------------

.. math::

    \begin{align}
        \min \ & c^\top x + x_0 \\
        \textrm{s.t. } & \forall \xi\in\Xi, \exists y\in Y,
         \begin{cases}
            x_0 \ge d^\top y \\
            T(\xi)x + W(\xi) \ge h(\xi)
         \end{cases} \\
        & (x_0,X) \in \mathbb R\times X
    \end{align}

.. math::

    \begin{align}
        \min_{x\in X}\left\{ c^\top x + \max_{\xi\in\Xi}\min_{y\in Y(x,\xi)} d^\top y \right\}
    \end{align}

.. toctree::
   :maxdepth: 1
   :glob:

   dualizing
