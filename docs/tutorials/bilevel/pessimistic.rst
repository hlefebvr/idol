.. _tutorial_pessimistic_bilevel:

From Pessimistic Bilevel Optimization to Optimistic Bilevel Optimization
========================================================================

Most of the litereature on bilevel optimization focuses on the optimistic setting, where the lower-level problem is assumed to
pick the solution in favor of the upper-level problem.
However, there exists other notions such as pessimistic bilevel problems.

A pessimistic problem reads as follows:

.. math::
    :label: pessimistic

    \begin{align}
        \min_{x} \quad & \max_{ y\in S(x) } \ F(x,y) \\
        \text{s.t.} \quad & x\in X, \\
        & S(x) \neq \emptyset,
     \end{align}

with :math:`S(x)` the solution set of

.. math::

    \begin{align}
        \min_{y} \quad & f(x,y) \\
        \text{s.t.} \quad & g(x,y) \ge 0, \\
        & y\in Y.
    \end{align}


In this tutorial, we will show how a pessimistic bilevel problem can be automatically transformed into an optimistic bilevel problem.
This transformation is due to :cite:`Zeng2020`.

.. hint::

    Here, :math:numref:`pessimistic` does not have coupling constraints for simplicity.
    However, the transformation can be extended to bilevel problems with coupling constraints.

.. contents:: Table of Contents
    :local:
    :depth: 2

The Equivalent Optimistic Problem
---------------------------------

In :cite:`Zeng2020`, the authors show that the pessimistic bilevel problem :math:numref:`pessimistic` is equivalent to the following optimistic bilevel problem:

.. math::
    :label: optimistic

    \begin{align}
        \min_{x,\bar y} \quad & F(x,y) \\
        \text{s.t.} \quad & x\in X, \ \bar y\in Y, \\
        & g(x,\bar y) \ge 0, \\
        & y\in \tilde S(x, \bar y),
    \end{align}

in which :math:`\tilde S(x, \bar y)` is the solution set of

.. math::

    \begin{align}
        \min_y \quad & -F(x,y) \\
        \text{s.t.} \quad & g(x,y) \ge 0, \\
        & y\in Y, \\
        & f(x,y) \le f(x, \bar y).
    \end{align}

Note that :math:numref:`optimistic` is an optimistic bilevel problem.

Implementation
--------------

Deriving the equivalent optimistic bilevel problem from a pessimistic bilevel problem can be done easily in idol.

To this end, let us assume that you have your bilevel problem already modeled in idol. In particular, let us consider that you have
two variables:

1. :code:`high_point_relaxation` which is a :code:`Model` representing the high-point relaxation of your bilevel problem.

2. :code:`description` which is a :code:`Bilevel:Description` object representing the bilevel problem. If you do not know what this is or how to create it, please refer to the :ref:`previous tutorial <tutorial_optimistic_bilevel>`.

Then, you can derive the equivalent optimistic bilevel problem as follows:

.. code::

    auto [opt_model, opt_description] =
        Bilevel::PessimisticAsOptimistic::make_model(
                                            high_point_relaxation,
                                            description
                                    );

Here, :code:`opt_model` is the high-point relaxation of :math:numref:`optimistic` and :code:`opt_description` is the bilevel description of :math:numref:`optimistic`.

The rest of the code is the same as with any other solver. For instance, you can solve the optimistic bilevel problem with MibS as follows:

.. code::

    opt_model.use(Bilevel::MibS(opt_description));

    opt_model.optimize();

    std::cout << save_primal(opt_model) << std::endl;
