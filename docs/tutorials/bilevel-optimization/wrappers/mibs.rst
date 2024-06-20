Calling the Coin-OR/MibS Solver
===============================

MibS is an optimization solver for mixed-integer bilevel problems.
Idol seemlessly integrates with MibS to solve bilevel problems.

We will see that solving bilevel problems with MibS is very similar to solving bilevel problems with any optimizer in idol.

We will assume that you have your bilevel problem modeled already in idol. In particular, we consider that you have
two variables:

1. :code:`high_point_relaxation` which is a :code:`Model` representing the high-point relaxation of your bilevel problem.

2. :code:`description` which is a :code:`Bilevel:Description` object representing the bilevel problem. If you do not know what this is or how to create it, please refer to the :ref:`previous tutorial <tutorial_optimistic_bilevel>`.

Then, you can solve your bilevel problem with MibS as follows:

.. code::

    high_point_relaxation.use(Bilevel::MibS(description));

    high_point_relaxation.optimize();

    std::cout << save_primal(high_point_relaxation) << std::endl;

Notice how the MibS solver is attached to the high-point relaxation model.

The rest of the code is the same as with any other solver.

.. warning::

    To use MibS, you need to have the MibS library installed on your system and idol linked to the executable.
    You can download MibS from `here <https://projects.coin-or.org/MibS>`_.

    Then, idol should be compiled with the options :code:`USE_MIBS=YES`, :code:`USE_CLP=YES`.
