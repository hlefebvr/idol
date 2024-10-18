COIN-OR/Osi (Cplex, Xpress, Cbc, ...)
=====================================

Idol can be linked with the `coin-or/Osi <https://github.com/coin-or/Osi>`_ library (*Open Solver Interface*) to use any solver having an osi interface.

.. hint::

    At the moment, only Cplex has been properly tested and can be used as follows.

    .. code-block::

        model.use(OsiCplex());

.. hint::

    Note that Osi can also be used with *any* Osi object by directly passing the interface as an argument.

    .. code-block::

        model.use(Osi(OsiCpxSolverInterface())); // same as: model.use(OsiCplex());

.. warning::

    Though idol has dedicated optimizers for Symphony, Cbc and Clp, it seems that the osi interface for these solvers do not
    fully fulfill the interface requirement to have a stable usage.

.. toctree::
    :maxdepth: 1
    :glob:

    *

.. doxygenclass:: idol::Osi

