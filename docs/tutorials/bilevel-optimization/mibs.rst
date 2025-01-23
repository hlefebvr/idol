Solving Mixed-Integer Bilevel Problems with coin-or/MibS
========================================================

MibS is an optimization solver for mixed-integer bilevel problems; for more information, please refer to the `MibS website <https://projects.coin-or.org/MibS>`_.
Idol seamlessly integrates with MibS to solve bilevel problems.

We will see that solving bilevel problems with MibS is very similar to solving any optimization problem in idol.

.. contents:: Table of Contents
    :local:
    :depth: 2

Setup
-----

We will assume that you have your bilevel problem already modeled in idol. In particular, we consider that you have
two variables:

1. :code:`high_point_relaxation` which is a :code:`Model` representing the high-point relaxation of your bilevel problem.

2. :code:`description` which is a :code:`Bilevel:Description` object representing the bilevel problem. If you do not know what this is or how to create it, please refer to the :ref:`previous tutorial <tutorial_optimistic_bilevel>`.

Using MibS
----------

To solve your bilevel problem, you can use the :code:`MibS` optimizer factory as follows:

.. code::

    high_point_relaxation.use(Bilevel::MibS(description));

    high_point_relaxation.optimize();

    std::cout << save_primal(high_point_relaxation) << std::endl;

Notice how the MibS optimizer factory is attached to the high-point relaxation model and that the bilevel description
is passed as an argument.

The rest of the code is the same as with any other solver.

.. hint::

    To use MibS, you need to have the MibS library installed on your system and idol linked to the executable.
    You can download MibS from `here <https://projects.coin-or.org/MibS>`_.

    Then, idol should be compiled with the options :code:`USE_MIBS=YES`, :code:`USE_CLP=YES`.

Using CPLEX for Feasibility Check
---------------------------------

Note that it is also possible to use MibS in combination with CPLEX for the feasibility check. This can be done as follows:

.. code::

    const auto mibs = Bilevel::MibS(description)
                        .with_cplex_for_feasibility(true)
                        .with_time_limit(3600)
                        .with_logs(true);

    high_point_relaxation.use(mibs);

    high_point_relaxation.optimize();

    std::cout << save_primal(high_point_relaxation) << std::endl;

Off course, MibS must have been installed with CPLEX for this to work.
