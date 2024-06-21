Using Trust Region Stabilization [TODO]
=======================================

.. warning::

   This is a work in progress. Thank you for your understanding.


In this section, we discuss stabilization of the CCG algorithm **under the specific setting of binary first-stage decisions**, i.e., :math:`X \subseteq \{0,1\}^{n_x}`.
This procedure is inspried by the work of :cite:`Baena2020` on Benders decomposition.

Let :math:`\bar x` be a given stability center. The following constraint is called a *local branching constraint*:

.. math::

    \Delta(x,\bar x) := \sum_{i:\bar x_i = 1} (1 - y_i) + \sum_{i:\bar x_i = 0} y_i \le \rho,

with :math:`\rho` a given radius. Additionally, we call *reversed local branching constraint* the following constraint:

.. math::

    \Delta(x,\bar x) \ge \rho + 1.

At each iteration, the stabilized CCG algorithm solves the following problem instead of :math:numref:`eq:master-problem`:

.. math::

    \begin{align}
        \min_{x_0,x} \quad & f(x) \\
        \text{s.t.} \quad & x\in X, \\
        & \left. \begin{array}{l}
            G(x,y^t,\xi^t) \le 0, \\
            y^t\in Y(x,\xi^t)
          \end{array} \right\} \quad \forall t=1,...,k, \\
        & \Delta(x,\bar x) \le \rho, \\
        & \Delta(x,\bar x') \ge \rho' + 1 \quad \forall (\bar x', \rho') \in R,
    \end{align}

for some set :math:`R\subseteq X\times\mathbb N` of reversed local branching constraints.

We now state the algorithm.

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
