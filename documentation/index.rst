IDOL's documentation
====================

.. toctree::
    :maxdepth: 2
    :caption: Contents:

Idol is a C++ library for implementing decomposition algorithms for (two-stage) robust optimization.

.. math::

    \min_{\boldsymbol x\in X}
    \max_{\boldsymbol\xi\in\Xi}
    \min_{\boldsymbol y\in Y(\boldsymbol x,\boldsymbol\xi)} \varphi(\boldsymbol x, \boldsymbol y ; \boldsymbol \xi)

Table of Contents
^^^^^^^^^^^^^^^^^

.. toctree::
    :maxdepth: 1

    self
    basics/index
    api/index
    dev/index

* :ref:`genindex`