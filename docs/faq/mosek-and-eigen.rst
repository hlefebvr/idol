Why does idol need Eigen when solving QPs and SOCPs with Mosek?
================================================================

The interface of idol is based on functional expressions like

.. math::

    \sum_{j=1}^n a_{ij}x_j + \sum_{j=1}^n\sum_{k=1}^n q_{jk}^ix_jx_k \le b_i.

The C++ Mosek interface, instead, is based on "conic expressions" like

.. math::

    (x_0, \textbf{Fx}) \in \mathcal Q^n,

where :math:`\mathcal Q^n` denotes the second-order cone and :math:`F` is some matrix related to :math:`Q^i`.

In order to compute :math:`F` and thus, to make the conversion between the Mosek interface and
the idol interface, one needs to compute an eigen value decomposition.

This is automatically done by idol using `Eigen <https://eigen.tuxfamily.org/index.php?title=Main_Page>`_.
