Introduction to the Column-and-Constraint Generation
====================================================

.. contents:: Table of Contents
    :local:
    :depth: 2

Introduction
------------

Robust Bilevel Optimization with Wait-and-See Lower Level
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Robust bilevel optimization problems with wait-and-see lower level is the highest class of problems that can be solved
by the current implementation of the column-and-constraint generation algorithm.
This class of problems can be expressed as

.. math::
    :label: bilevel_wait_and_see

    \begin{align*}
        \min_{x\in X} \ \max_{\xi\in\Xi} \ \min_{y} \ \left\{ F(x,y,\xi) \ : \ y\in S(x,\xi), \ G(x,y,\xi) \ge 0 \right\}.
    \end{align*}

Here, :math:`S(x,\xi)` denotes the set of optimal solutions to the lower-level problem and is given by

.. math::

    \begin{align*}
        S(x,\xi) := \text{arg}\min_y \quad & f(x,y,\xi) \quad \text{s.t.} \quad  g(x,y,\xi) \ge 0, \ y\in Y.
    \end{align*}

Constraints :math:`G(x,y,\xi) \ge 0` are the coupling constraints between the upper and lower levels.
The set :math:`\{ y\in Y : g(x,y,\xi) \ge 0 \}` is the feasible set of the lower-level problem.
The set :math:`\Xi` is the uncertainty set while :math:`X` is the feasible set of the upper-level problem.

Two-stage Robust Optimization and Adjustable Robust Optimization
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Originally, the column-and-constraint generation algorithm was developed for two-stage robust optimization problems :cite:`Zeng2013`.
Note that robust bilevel problems with wait-and-see lower level are a generalization of two-stage robust optimization problems.
In fact, :math:numref:`bilevel_wait_and_see` can be reduced to a two-stage robust problem, or adjustable robust problem,
by simply considering :math:`f(x,y,\xi) = 0`, i.e., :math:`S(x,\xi) = \{ y\in Y : g(x,y,\xi) \ge 0 \}`. By doing so,
one obtains the following two-stage robust optimization problem:

.. math::
    :label: aro

    \begin{align*}
        \min_{x\in X} \ \max_{\xi\in\Xi} \ \min_{y} \ \left\{ F(x,y,\xi) \ : \ y\in Y, \ g(x,y,\xi) \ge 0, \ G(x,y,\xi) \ge 0 \right\}.
    \end{align*}

Note that we keep the notion of "coupling constraints" here which refer to the constraints :math:`G(x,y,\xi) \ge 0`.
Algorithmically, coupling constraints are treated differently from the lower-level constraints :math:`y\in Y` and :math:`g(x,y,\xi) \ge 0`.
However, it is clear that the coupling constraints :math:`G` are "redundant" in the sense that they can be incorporated
into the lower-level constraints :math:`g`. Nevertheless, the algorithm which is applied is different depending on whether
the coupling constraints are present or not.

Assumptions
^^^^^^^^^^^

The current implementation of the column-and-constraint generation algorithm requires some assumptions to be satisfied.
If these assumptions are not satisfied, the algorithm may not converge to the optimal solution, or have unspecified
behavior. We now state these assumptions.

1. :math:`Y(x,\xi) \neq \emptyset \implies S(x,\xi) \neq \emptyset`.
2. :math:`\Xi` is explicitly bounded, i.e., :math:`\Xi\subseteq[\underline\xi, \overline\xi]`.
3. One of the two following assumptions hold:

* It holds :math:`Y(x,\xi) = \emptyset` for all :math:`x\in X` and :math:`\xi\in\Xi`.
* All variables in :math:`Y(x,\xi)` are explicitly bounded, i.e., :math:`Y(x,\xi)\subseteq[\underline y, \bar y]`.

The Adversarial Problem
-----------------------

Let :math:`\hat x` be given.

Checking Feasibility of the Lower-level
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The adversarial problem for checking feasibility is given by:

.. math::

    \begin{align*}
        \max_{\xi\in\Xi} \quad \min_{y,s} \quad & e^\top s \\
        \text{s.t.} \quad & g(\hat x,y,\xi) + s \ge 0, \\
         & s \ge 0, \\
         & y\in Y.
    \end{align*}

.. hint::

    Internally, idol calls a bilevel solver and asks to solve the following equivalent min-min formulation:

    .. math::

        \begin{align*}
            \min_{\xi,y} \quad & -e^\top s \\
            \text{s.t.} \quad & \xi\in\Xi, \\
            & 0 \le s \le \bar s, \\
            & y\in \begin{array}[t]{rl}
                \displaystyle \text{arg}\min_{y,s} & e^\top s \\
                \text{s.t.} & g(\hat x,y,\xi) + s \ge 0, \\
                & s \ge 0, \\
                & y\in Y.
            \end{array}
        \end{align*}

Checking Feasibility of the Coupling Constraints
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Recall that we checked :math:`Y(\hat x,\xi) \neq \emptyset` for all :math:`\xi\in\Xi` by solving the feasibility problem,
or that it is known that :math:`Y(x,\xi) = \emptyset` for all :math:`x\in X` and :math:`\xi\in\Xi`.
Since we further assumed that :math:`Y(x,\xi) \neq \emptyset \implies S(x,\xi) \neq \emptyset`, the following problem
is feasible and can be used to check optimality:

.. math::

    \begin{equation*}
        \max_{\xi\in\Xi} \quad \min_{y\in S(x,\xi)} \quad  G_k(x,y,\xi).
    \end{equation*}


Checking Optimality
^^^^^^^^^^^^^^^^^^^

.. math::

    \begin{equation*}
        \max_{\xi\in\Xi} \quad \min_{y\in S(x,\xi)} \quad  F(x,y,\xi).
    \end{equation*}


Pseudo-Code
-----------

.. raw:: html

    <pre class="pseudocode">
        \begin{algorithm}
        \caption{The Column-and-constraint Generation Algorithm}
        \begin{algorithmic}
        \REQUIRE An initial radius \( \rho\in\{ 1,\dotsc, n_x \} \) and an initial point \( \bar x\in X \).
        \STATE Initialize \( k \gets 0 \), \( UB \gets +\infty \), \( LB \gets -\infty \), \( R \gets \emptyset \)
        \STATE Solve the restricted master problem (RMP)
        \IF{the RMP is infeasible}
            \IF{ \( \rho \ge n_x \) }
                \STATE STOP, \( UB \) is the optimal value.
            \ENDIF
            \STATE Add a reversed local branching constraint, \( R \gets R \cup (\bar x, \rho) \)
            \STATE Increase \( \rho \)
        \ELSE
            \STATE Let \( x^k \) be the solution of the RMP and \( v^k \) be its value
            \STATE Solve the separation problem, let \( \xi^k \) be the solution and \( s^k \) be its value
            \IF{ \( s^k \le \varepsilon_\text{feas} \) }
                \STATE \( UB \gets \min\{ UB, v^k \} \)
                \STATE Solve the RMP without the stabilization constraints, let $\underline v^k$ be its value, set \( LB \gets \underline v^k \)
                \IF{ \( UB - LB \le \varepsilon \) }
                    \STATE STOP, \( UB \) is the optimal value.
                \ENDIF
                \STATE Add a reversed local branching constraint, \( R \gets R \cup (x^k, \rho) \)
                \STATE Update the stability center \( \bar x \gets x^k \)
                \STATE Optionally, reset \( \rho \gets 1 \)
            \ENDIF
            \STATE \( k \gets k + 1 \)
            \STATE Go back to step 2
        \ENDIF
        \end{algorithmic}
        \end{algorithm}
    </pre>
    <script>
        pseudocode.renderClass("pseudocode", { lineNumber: true });
    </script>
