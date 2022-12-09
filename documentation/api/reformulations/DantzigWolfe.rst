.. _api_DantzigWolfe:

Dantzig-Wolfe
=============

For a given original formulation, modeled as follows:

.. math::

  \begin{array}{ll}
      \textrm{minimize } & c^Tx \\
      \textrm{subject to } & Ax \ge b \\
      & Bx \ge d \\
      & x \ge 0,
  \end{array}

its Dantzig-Wolfe reformulation reads:

.. math::

    \begin{array}{ll}
      \textrm{minimize } & \sum_{e\in E}\alpha_e c^T\bar{x}^e \\
      \textrm{subject to } & \sum_{e\in E}\alpha_e A\bar{x}^e \ge b \\
      & \sum_{e\in E} \alpha_e = 1 \\
      & \alpha_e \ge 0 \qquad \forall e\in E.
    \end{array}

Here, constraints :math:`Bx \ge d` are seen as complicating while :math:`E` is a list of indices
for the extreme points of :math:`\{ x\ge 0 : Bx \ge d \}`.

.. doxygenclass:: Reformulations::DantzigWolfe
