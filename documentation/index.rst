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

TODO
----

Refacto Objective and RHS
^^^^^^^^^^^^^^^^^^^^^^^^^

So far, Objective and RHS are not handle in a sparse way.
For instance, we need to enumerate every variable when an objective changes or the objective.

Public attributes in NodeByBound
^^^^^^^^^^^^^^^^^^^^^^^^^

So far, NodeByBound exposes all its attributes.

Public methods of algorithms
^^^^^^^^^^^^^^^^^^^^^^^^^

Check public methods for ColumnGenerationSP, some of them should be hidden.

Throw when a parameter is given to a Solver
^^^^^^^^^^^^^^^^^^^^^^^^^

So far, no check is done in solvers and Constant::numerical is always used as value().

Tests for solving an unbounded LP
^^^^^^^^^^^^^^^^^^^^^^^^^

So far, the test is commented because extreme_ray is not a method of Algorithm.

Contribute to add constraint in column generation / Local and Global cuts
^^^^^^^^^^^^^^^^^^^^^^^^^

So far, global cuts are not handled by generation procedures.

Table of Contents
-----------------

.. toctree::
    :maxdepth: 1

    self
    basics/index
    api/index
    dev/index

* :ref:`genindex`