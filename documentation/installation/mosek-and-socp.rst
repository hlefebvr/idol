.. _mosek_and_socp:

Using Idol with Mosek for QPs and SOCPs
=======================================

The interface of idol
is based on functional expressions, e.g., like

.. math::

    \sum_{j=1}^n a_{ij}x_j + \sum_{j=1}^n\sum_{k=1}^n q_{jk}^ix_jx_k \le b_i.

The C++ Mosek interface, instead, is based on conic expressions, e.g., like

.. math::

    (x_0, \textbf{Fx}) \in \mathcal Q^n.

To make the conversion between the Mosek interface and
the idol interface, one needs to compute an eigen value decomposition.

This is automatically done by idol using the `Eigen <https://eigen.tuxfamily.org/index.php?title=Main_Page>`_ C++ library.

Thus, if you intend to use Idol to interface with Mosek and to use quadratic expressions, you need to set the `-DUSE_EIGEN=YES` CMake
option as well (note that Eigen is a header-only library and that no installation is needed).
