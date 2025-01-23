.. _bilevel:

Bilevel Optimization
====================

Bilevel optimization is a field of mathematical optimization in which two optimization problems are intertwined: the
upper-level and the lower-level problem. The upper-level problem minimizes a given objective function taking into account
the solution to the lower-level problem, which is parameterized by the upper-level’s decision. Such problems have many
applications in, e.g., economics where it is used to model non-cooperative games.

A classic model for a bilevel problem is as follows.

.. math::
    :label: bilevel

    \begin{align}
        \text{''}\min_{x}\text{''} \quad & F(x,y) \\
        \text{s.t.} \quad & G(x,y) \ge 0, \\
        & x\in X, \\
        & y\in S(x),
    \end{align}

in which :math:`S(x)` denotes the solution set of the lower-level problem, which is parameterized by the upper-level decision
:math:`x`. The lower-level problem is defined as

.. math::

    \begin{align}
        \min_{y} \quad & f(x,y) \\
        \text{s.t.} \quad & g(x,y) \ge 0, \\
        & y\in Y.
    \end{align}

Note that the quotes around the :math:`\min` operator in :math:numref:`bilevel` is here to highlight that the problem is
ill-defined in its current form. Indeed, in case multiple solutions to the lower-level problem exist, the upper-level problem
has to somehow "choose" one of them. To circumvent this, we typically consider the optimistic setting, where the lower-level
is assumed to pick the solution in favor of the upper-level problem, in order to break ties.

Optimistic bilevel problems can be modeled as:

.. math::

    \begin{align}
        \min_{x,y} \quad & F(x,y) \\
        \text{s.t.} \quad & G(x,y) \ge 0, \\
        & x\in X, \\
        & y\in S(x),
    \end{align}

Note that there exists other notions such as pessimistic bilevel problems. There, the lower-level problem is assumed to pick
the worst solution for the upper-level problem. This can be modeled as follows.

.. math::

    \begin{align}
        \min_{x} \quad & \max_{ y\in S(x) } \ F(x,y) \\
        \text{s.t.} \quad & G(x,\bar y) \ge 0, \quad \text{for all } \bar y\in S(x) \\
        & x\in X, \\
        & S(x) \neq \emptyset.
     \end{align}

Pessimisitc bilevel problems are less studied in the literature.

.. toctree::
    :maxdepth: 1
    :glob:

    modeling
    mibs
    pessimistic
