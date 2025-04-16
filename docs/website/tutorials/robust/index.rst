.. _robust:

Robust Optimization
===================

Robust optimization is a field of mathematical optimization that explicitly accounts for uncertainty in optimization problems.
In many real-world applications, the input parameters—such as costs, demands, or resource availabilities—are not precisely known
at the time decisions must be made. This uncertainty can arise from estimation errors, forecasting inaccuracies,
or inherent variability in the system.
Robust optimization provides a framework for making decisions that remain feasible and perform well across a range of
possible realizations of the uncertain parameters.

A key concept in robust optimization is the *uncertainty set*, which defines the range of possible values that uncertain
parameters can take.
Instead of assuming that uncertain parameters follow a known probability distribution (as, e.g., in stochastic optimization),
robust optimization only requires that they belong to a predefined set. This set, typically denoted as :math:`U`, captures
the worst-case deviations within a specified level of conservatism.
By optimizing against the worst-case scenario within :math:`U`, robust optimization ensures that the solution remains
feasible and performs satisfactorily under all realizations within the uncertainty set.

There are mainly two types of problems that can be handled by idol: single-stage problems and two-stage problems.
If you are new to robust optimization, we strongly advise you to start with the single-stage problems.

.. toctree::
    :maxdepth: 2
    :glob:

    single-stage/index
    two-stage/index
