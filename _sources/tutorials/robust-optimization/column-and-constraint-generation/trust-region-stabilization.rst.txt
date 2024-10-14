Using Trust Region Stabilization
================================

This tutorial describes how to use the trust region stabilization in the CCG algorithm.
This stabilization technique is inspired by the work of :cite:`Baena2020` on Benders decomposition.

.. admonition:: Assumption

    This feature is only applicable if the first-stage decisions are binary, i.e., :math:`X \subseteq \{0,1\}^{n_x}`.

.. contents:: Table of Contents
    :local:
    :depth: 2

Introduction
------------

In this section, we discuss stabilization of the CCG algorithm if the first-stage decisions are binary, i.e., :math:`X \subseteq \{0,1\}^{n_x}`.

Let :math:`\bar x` be a given stability center, i.e., a point that is thought to be a "good" solution to the problem.
The following constraint is called a *local branching constraint*:

.. math::

    \Delta(x,\bar x) := \sum_{i:\bar x_i = 1} (1 - x_i) + \sum_{i:\bar x_i = 0} x_i \le \rho,

with :math:`\rho` a given radius. Additionally, we call *reversed local branching constraint* the following constraint:

.. math::

    \Delta(x,\bar x) \ge \rho + 1.

At each iteration, the stabilized CCG algorithm solves the following problem instead of :math:numref:`eq:master-problem`:

.. math::

    \begin{align}
        \min_{x_0,x} \quad & f(x) \\
        \text{s.t.} \quad & x\in X, \\
        & G(x,y^t,\xi^t) \le 0 \quad t=1,\dotsc,k, \\
        & y^t\in Y(x,\xi^t) \quad t=1,\dotsc,k, \\
        & \Delta(x,\bar x) \le \rho, \\
        & \Delta(x,\bar x') \ge \rho' + 1 \quad \forall (\bar x', \rho') \in R,
    \end{align}

for some set :math:`R\subseteq X\times\mathbb N` of reversed local branching constraints.

The complete stabilized CCG algorithm is described with the following pseudocode.

.. raw:: html

    <pre class="pseudocode">
        \begin{algorithm}
        \caption{Stabilized CCG Algorithm}
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

Note that if :math:`\rho \ge n_x`, the stabilized CCG is exactly the CCG algorithm.

Implementation in idol
----------------------

Activating the trust region stabilization in idol is done through the method :code:`with_stabilization` of the :code:`idol::Robust::ColumnAndConstraintGeneration` class.
An object of the class :code:`Robust::CCGStabilizers::TrustRegion` must be passed as an argument to the method.

The following code shows how to use the trust region stabilization in idol.

.. code::

    model.use(
            Robust::ColumnAndConstraintGeneration(stages, uncertainty_set)
                    .with_master_optimizer(Gurobi())
                    .with_separator(Robust::CCGSeparators::Bilevel())
                    .with_stabilization(Robust::CCGStabilizers::TrustRegion())
                    .with_logs(true)
    );

    model.optimize();

Note that the radius :math:`\rho` is set to

.. math::

    \rho \gets \lceil \mu_i n_x \rceil

where :math:`\mu_i` is a parameter controlling the size of the trust region and :math:`n_x` is the number of (binary) first-stage variables. By default, :math:`\mu_i` takes value in
::math:`\lbrace .01, .02, .5 \rbrace`.
You can set the values of :math:`\mu_i` by calling the method :code:`with_trust_factors` of the class :code:`Robust::CCGStabilizers::TrustRegion`.
For instance, the following code sets the trust factors to :math:`\lbrace .02, .5 \rbrace`.

.. code::

    model.use(
            Robust::ColumnAndConstraintGeneration(stages, uncertainty_set)
                    .with_master_optimizer(Gurobi())
                    .with_separator(Robust::CCGSeparators::Bilevel())
                    .with_stabilization(
                        Robust::CCGStabilizers::TrustRegion()
                            .with_trust_factors({.02, .5})
                    )
                    .with_logs(true)
    );
