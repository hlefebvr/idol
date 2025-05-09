.. _mosek_and_socp:

Why is Eigen Necessary to Solve QCQPs with Mosek?
=================================================

In Short
--------

The interface of idol is based on quadratic expressions like

.. math::

    \sum_{j=1}^n a_{ij}x_j + \sum_{j=1}^n\sum_{k=1}^n q_{jk}^ix_jx_k \le b_i.

The C++ interface of Mosek, instead, is based on "conic expressions" like

.. math::

    (x_0, \textbf{Fx}) \in \mathcal Q^n,

where :math:`\mathcal Q^n` denotes the second-order cone and :math:`F` is some matrix related to :math:`Q^i`.

In order to make the conversion between the Mosek interface and
the idol interface (for instance, computing the matrix :math:`F`),
one needs to compute an eigen value decomposition.
This is automatically done by idol using `Eigen <https://eigen.tuxfamily.org/index.php?title=Main_Page>`_.
This is why Eigen is necessary if one wants to use Mosek with idol to solve QPs or QCQPs.

Detailed Answer
---------------

Consider the quadratic expression

.. math::

    \sum_{j=1}^n a_{ij}x_j + \sum_{j=1}^n\sum_{k=1}^n q_{jk}^ix_jx_k \le b_i.

It can be written as

.. math::

    a_{(i)}^\top x + x^\top Q^i x \le b_i.

The first task is to compute an eigen value decomposition of :math:`Q^i`, i.e.,
to find matrices :math:`L` and :math:`D` such that :math:`Q^i = L D L^\top`
and :math:`D` is a diagonal matrix containing the eigen values of :math:`Q^i`.

If there are more than two negative eigen values, the constraint is not convex and an exeception is thrown.

Otherwise, we compute :math:`F` and :math:`N` so that :math:`x^\top Q^i x = \lVert Fx \rVert_2^2 + x^\top N x`.
This is done by setting :math:`F = \sqrt{D^+} L^\top` where :math:`D^+` is :math:`D` with the negative eigen values replaced by zero,
and :math:`N = L D^- L^\top`.

Hence, we have that the constraint is expressed as

.. math::

    \lVert Fx \rVert_2^2 + x^\top N x \le b_i - a_{(i)}^\top x.

If :math:`N = 0`, it can be written as

.. math::

    \begin{align}
        & \lVert Fx \rVert_2^2 \le b_i - a_{(i)}^\top x \\
        \iff & (.5, b_i - a_{(i)}^\top x, Fx) \in\mathcal Q^{n+2}_r,
    \end{align}

where :math:`\mathcal Q^{n+2}_r` denotes the rotated second-order cone.

Otherwise, if :math:`N` has one non-zero entry, say :math:`n_{ij}`.
Then, it must be that :math:`a_{(i)} = 0` and :math:`b_i \le 0` for the constraint to be
converted by idol. Under this assumption, the constraint can be written as

.. math::

    \begin{align}
        & \lVert Fx \rVert_2^2 + \sqrt{-b_i}^2 \le n_{ij} x_i x_j \\
        \iff & (.5 n_{i,j} x_i, x_j, Fx, \sqrt{-b_i}) \in\mathcal Q^{n+1}_r,
    \end{align}

where it is assumed that :math:`n_{i,j} x_{i}x_{j} \ge 0` holds
(for now, a warning is printed to enlight this expectation).