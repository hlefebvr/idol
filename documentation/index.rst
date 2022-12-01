IDOL's documentation
====================

.. toctree::
    :maxdepth: 2
    :caption: Contents:

Idol is a C++ library for implementing complex decomposition algorithms
mathematical optimization.

Table of Contents
-----------------

.. toctree::
    :maxdepth: 1

    self
    basics/index
    api/index
    dev/index

* :ref:`genindex`

To do
----

- **Make buffer_size a parameter**
    So far, Model resizes when needed without control of the increased size. In Solver, m_buffer_size already exists.
    This should be commonly managed.
- **Public methods of algorithms**
    Check public methods for ColumnGenerationSP, some of them should be hidden.
- **Tests for solving an unbounded LP**
    So far, the test is commented because extreme_ray is not a method of Algorithm.
- **Contribute to add constraint in column generation / Local and Global cuts**
    So far, global cuts are not handled by generation procedures.
