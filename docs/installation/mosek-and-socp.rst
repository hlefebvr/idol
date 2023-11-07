.. _mosek_and_socp:

Using Idol with Eigen
=====================

This page elaborates on some operations made by idol which require usage of the `Eigen <https://eigen.tuxfamily.org/index.php?title=Main_Page>`_
C++ library.

We first explain how to link idol with Eigen, then discuss several operations which require the use of Eigen.

.. warning::

    Calling functionalities which require Eigen without have linked idol with Eigen will result in an Exception being
    thrown.

"Linking" with Eigen
--------------------

Eigen is header-only C++ library. In that sense, no installation is required. One only needs to download Eigen's source
code in order to make their header files accessible.

Then, all you have to do in order to "link" with Eigen is to set the ``USE_EIGEN`` CMake
option to ``YES``.

.. admonition:: Example

    This example shows you have to create an install target for idol with Mosek and Eigen.

    .. code-block::

        cmake -DUSE_MOSEK=YES -DUSE_EIGEN=YES ..


Interfacing Mosek for Solving QPs and SOCPs
-------------------------------------------

The interface of idol
is based on functional expressions, e.g., like

.. math::

    \sum_{j=1}^n a_{ij}x_j + \sum_{j=1}^n\sum_{k=1}^n q_{jk}^ix_jx_k \le b_i.

The C++ Mosek interface, instead, is based on conic expressions, e.g., like

.. math::

    (x_0, \textbf{Fx}) \in \mathcal Q^n,

where :math:`\mathcal Q^n` denotes the second-order cone and :math:`F` is some matrix related to :math:`Q^i`.

In order to compute :math:`F` and thus, to make the conversion between the Mosek interface and
the idol interface, one needs to compute an eigen value decomposition.

This is automatically done by idol using `Eigen <https://eigen.tuxfamily.org/index.php?title=Main_Page>`_.

