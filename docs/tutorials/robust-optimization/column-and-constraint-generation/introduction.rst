Introduction
============

The Column-and-Constraint Generation (CCG) algorithm is a classical method for solving two-stage robust optimization problems.
It was originally introduced by :cite:`Zeng2013`.

In idol, the implementation of CCG alows to solve a class of problems which is slightly more general than classical
two-stage robust optimization problems. This class of problems is defined in the following section.

.. contents:: Table of Contents
    :local:
    :depth: 2

Problem Definition and Notation
-------------------------------

The CCG algorithm can solve optimization problems of the form

.. math::
    :label: eq:original-problem

    \begin{align}
        \min_x \quad & f(x) \\
        \text{s.t.} \quad  & x\in X, \\
        & \forall \xi\in\Xi, \ \exists y\in Y(x,\xi), \ G(x,y,\xi) \le 0.
    \end{align}

in which :math:`f:\mathbb R^{n_x}\rightarrow\mathbb R` is a given function,
:math:`X\subseteq\mathbb R^{n_x}` is a given set, called the *first-stage feasible set*,
:math:`\Xi\subseteq\mathbb R^{n_\xi}` is a given set, called the *uncertainty set*, typically compact,
:math:`Y(x,\xi) \subseteq \mathbb R^{n_y}` is a set, defined for all :math:`x\in X` and :math:`\xi\in\Xi`, called the *second-stage feasible set*, and
:math:`G:\mathbb R^{n_x+n_y+n_\xi}\rightarrow\mathbb R^\ell` is a given vector of functions, defining the so-called *coupling constraints* between the uncertainty, the first- and the second-stage decisions.

.. admonition:: Regarding Coupling Constraints

    It is clear that coupling constraints are redundant since they can be incorporated into the definition of the second-stage feasible set.
    However, we will see that explicitly defining *coupling constraints* may lead to different implementations of the CCG algorithm.
    We will dive into this aspect after introducing the concept of *separators*.

.. hint::

    Consider the Min-Max-Min problem

    .. math::

        \min_{x\in X} \ \max_{\xi\in\Xi} \ \min_{y\in Y(x,\xi)} \ \psi(x,y,\xi).

    Clearly, this is a special case of :math:numref:`eq:original-problem` since it can be written as

    .. math::

        \begin{align}
            \min_{x_0,x} \quad & x_0 \\
            \text{s.t.} \quad  & (x_0,x) \in\mathbb R\times X, \\
            & \forall \xi\in\Xi, \ \exists y\in Y(x,\xi), \ \psi(x,y,\xi) \le x_0.
        \end{align}

    Here, there is only one coupling constraint, which is :math:`\psi(x,y,\xi) \le x_0`.


The Algorithm
-------------

Basic Idea
^^^^^^^^^^

The idea of the CCG algorithm is to consider a finite subset of m_n_points_bound in :math:`\Xi`, say :math:`\{ \xi^1, \dotsc, \xi^k \}`,
and to solve the following problem instead of :math:numref:`eq:original-problem`:

.. math::
    :label: eq:master-problem

    \begin{align}
        \min_{x_0,x,y^1,\dotsc,y^K} \quad & f(x) \\
        \text{s.t.} \quad & x\in X, \\
        & G(x,y^t,\xi^t) \le 0 \qquad k=1,\dotsc,K, \\
        & y^t\in Y(x,\xi^t) \qquad k=1,\dotsc,K.
    \end{align}

Here, a new variable :math:`y^t` has been introduced for each :math:`t=1,...,k`, enforcing that, indeed,
there exists :math:`y^t\in Y(x,\xi^t)` such that :math:`G(x,y^t,\xi^t) \le 0`.

Note that Problem :math:numref:`eq:master-problem` is a relaxation of Problem :math:numref:`eq:original-problem` since
any feasible point of :math:numref:`eq:original-problem` is also feasible for :math:numref:`eq:master-problem` (for some :math:`y^1,\dotsc,y^K`).

Now, given a solution :math:`\hat x\in X` to the relaxed problem :math:numref:`eq:master-problem`,
one needs to check whether :math:`\hat x` is feasible for Problem :math:numref:`eq:original-problem`.
Thus, one seeks a scenario :math:`\xi^*\in\Xi` such that, either :math:`Y(\hat x, \xi^*)` is empty, or :math:`G(\hat x,y,\xi^*) > 0` for all :math:`y\in Y(\hat x, \xi^*)`.
If no such scenario exists, then :math:`\hat x` is feasible for :math:numref:`eq:original-problem`. Otherwise, the new scenario :math:`\xi^*` is added to the set of considered scenarios and the process is repeated.

Identifying a missing scenario is called *separation*, and can be done by solving the following problem:

.. math::
    :name: eq:separation-problem

        \max_{\xi\in \Xi} \ \max_{\ell=1,...,L} \left\{ \ \min_{ y\in Y(\hat x,\xi) } \ G_\ell(\hat x,y,\xi) \right\}.

If the optimal value of the separation problem is non-positive, then :math:`\hat x` is feasible for :math:numref:`eq:original-problem`.
Otherwise, the solution to the separation problem gives a new scenario :math:`\xi^*` to be added to the set of scenarios.

Note that we use the convention :math:`\max \emptyset = -\infty` and :math:`\min \emptyset = +\infty`.

Separators
^^^^^^^^^^

Clearly, the separation problem :math:numref:`eq:separation-problem` can be solved in many different ways. In idol,
it is therefore possible to give a user-defined functor, called a *separator*, which solves the separation problem.
Note that the most common ways to solve the separation problem are already implemented in idol.
Yet, if you wish to implement your own separator, you should refer to :ref:`this tutorial <tutorial_write_ccg_separator>`.

Shortly put, the separator solves problems of the form

.. math::
    :label: eq:single-separation-problem

        \max_{\xi\in \Xi} \ \min_{ y\in Y(\hat x,\xi) } \ G_\ell(\hat x,y,\xi),

for a given :math:`G_\ell` (:math:`\ell\in\{1,...,L\}`).

Note that it is ensured that the separator always solves a problem which is feasible.
Indeed, in case Problem :math:numref:`eq:separation-problem`
is not known to satisfy the *complete recourse assumption* (i.e., it is not known whether :math:`\forall x\in X, \forall\xi\in\Xi, \exists y\in Y(x,\xi)` holds),
the CCG algorithm will first solve a feasibility version of the separation problem to check whether
:math:`\hat x` is such that for all :math:`\xi\in\Xi` there exists :math:`y\in Y(\hat x,\xi)`.
Fortunately, it is also possible to specify that the complete recourse assumption holds, in which case the feasibility version of the separation problem is not solved.

Let :math:`\xi^{\ell}` denote the solution to the separation problem :math:numref:`eq:single-separation-problem` for a given :math:`\ell\in\{1,...,L\}`.
Then, a scenario :math:`\xi^{\ell^*}` is added to Problem :math:numref:`eq:master-problem` if and only if

.. math::

    \ell^* \in \underset{\ell=1,...,L}{\text{argmax}} \ \min_{ y\in Y(\hat x,\xi^\ell) } \ G_\ell(\hat x,y,\xi^\ell) > \varepsilon_\text{feas}.

See :ref:`the dedicated page <tutorial_write_ccg_separator>` for more details.

On the Impact of Coupling Constraints
-------------------------------------

We now discuss the impact of the definition of the coupling constraints :math:`G` on the implementation of the CCG algorithm.
Clearly, one obtains an equivalent problem to :math:numref:`eq:original-problem` by defining the second-stage feasible set as

.. math::

    \tilde Y(x,\xi) = \{ y\in Y(x,\xi) \ | \ G(x,y,\xi) \le 0 \},

and by considering the problem

.. math::

    \begin{align}
        \min_x \quad & f(x) \\
        \text{s.t.} \quad  & x\in X, \\
        & \forall \xi\in\Xi, \ \exists y\in \tilde Y(\hat x,\xi).
    \end{align}

In this case, the separation problem becomes

.. math::

    \max_{\xi\in\Xi} \ \min_{ y\in \tilde Y(\hat x,\xi) } \ 0,

which is a feasibility problem.

Though the two approaches are equivalent, in the sense that they will both lead to a solution to :math:numref:`eq:original-problem`,
they may lead to different computational performances. An interested reader may refer to, e.g., :cite:`Ayoub2016` for more details.
