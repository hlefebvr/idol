.. _api_ro_cccg:

Column-and-Constraint Generation
================================

The Column-and-Constraint Generation (CCG) algorithm is a classical method for solving two-stage robust optimization problems.
It was originally introduced by :cite:`Zeng2013`.

In idol, the implementation of CCG alows to solve a class of problems which is slightly more general than classical two-stage robust optimization problems.
This class of problems is defined in the following sub-section.

Problem Class
-------------

The CCG algorithm can solve optimization problems of the following form:

.. math::
    :label: eq:original-problem

    \begin{align}
        \min_x \quad & f(x) \\
        \text{s.t.} \quad  & x\in X, \\
        & \forall \xi\in\Xi, \ \exists y\in Y(x,\xi), \ G(x,y,\xi) \le 0.
    \end{align}

in which

- :math:`f` is a given function,
- :math:`X` is a given set, called the *first-stage feasible set*,
- :math:`\Xi` is a given set, called the *uncertainty set*, typically compact,
- :math:`Y(x,\xi)` is a set, defined for all :math:`x\in X` and :math:`\xi\in\Xi`, called the *second-stage feasible set*, and
- :math:`G` is a given vector of functions, defining the so-called *coupling constraints* between the uncertainty, the first- and the second-stage decisions.

.. admonition:: Regarding Coupling Constraints

    It is clear that coupling constraints are redundant since they can be incorporated into the definition of the second-stage feasible set.
    However, we will see that explicitly defining *coupling constraints* may lead to different implementations of the CCG algorithm.
    We will dive into this aspect after introducing the concept of *separators*.

Min-Max-Min Problems
--------------------

Consider the following min-max-min problem:

.. math::

    \min_{x\in X} \ \max_{\xi\in\Xi} \ \min_{y\in Y(x,\xi)} \ \psi(x,y,\xi).

This class of problems is a special case of :math:numref:`eq:original-problem` since it can be written as

.. math::

    \begin{align}
        \min_{x_0,x} \quad & x_0 \\
        \text{s.t.} \quad  & (x_0,x) \in\mathbb R\times X, \\
        & \forall \xi\in\Xi, \ \exists y\in Y(x,\xi), \ \psi(x,y,\xi) \le x_0.
    \end{align}

Here, there is only one coupling constraint, which is :math:`\psi(x,y,\xi) \le x_0`.

Separators
----------

Basic Idea
^^^^^^^^^^

Separators should return a solution of

.. math::

        \max_{\xi\in \Xi} \ \max_{\ell=1,...,L} \left\{ \ \min_{ y\in Y(\hat x,\xi) } \ G_\ell(\hat x,y,\xi) \right\}.

Then, a scenario :math:`\xi^*` is added if, and only if,

.. math::

    \max_{\ell=1,...,L} \left\{ \ \min_{ y\in Y(\hat x,\xi^*) } \ G_\ell(\hat x,y,\xi^*) \right\} > 0.

Note that we use the convention :math:`\max \emptyset = -\infty` and :math:`\min \emptyset = +\infty`.

On the Impact of Coupling Constraints
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

We now discuss the impact of the definition of the coupling constraints on the implementation of the CCG algorithm.

Indeed, let :math:`\hat x\in X` be a given point. In order to check whether :math:`\hat x` is feasible for :math:numref:`eq:original-problem`,
one needs to solve the following problem:

.. math::

    \max_{\xi\in \Xi} \ \max_{\ell=1,...,L} \left\{ \ \min_{ y\in Y(x,\xi) } \ G_\ell(\hat x,y,\xi) \right\},

and check whether the optimal value is non-positive (here, we assume that :math:`G` is an :math:`L`-dimensional vector).

Arguably, one obtains an equivalent problem to :math:numref:`eq:original-problem` by defining the second-stage feasible set as

.. math::

    \tilde Y(x,\xi) = \{ y\in Y(x,\xi) \ | \ G(x,y,\xi) \le 0 \},

and by considering the following problem:

.. math::

    \begin{align}
        \min_x \quad & f(x) \\
        \text{s.t.} \quad  & x\in X, \\
        & \forall \xi\in\Xi, \ \exists y\in \tilde Y(\hat x,\xi).
    \end{align}

Then, the separation problem becomes

.. math::

    \max_{\xi\in\Xi} \ \min_{ y\in Y'(\hat x,\xi) } \ 0^\top y,

which is a feasibility problem. See, e.g., :cite:`Ayoub2016`.

.. toctree::
    :maxdepth: 2
    :glob:

    ColumnAndConstraintGeneration.doxygen.rst
    separators/index
