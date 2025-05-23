\page example_mixed_integer_lp_dual lp-dual
\brief Models a simple LP and generates its dual. Solves the dual problem with HiGHS and prints, for each primal constraint, 
    its associated dual variable with its value.

The primal LP is taken from "Model building in Mathematical Programming" by H. Paul Williams and is
given by

\f[
    \begin{align*}
    \min_{x,y} \quad & 3x + 2y \\
    \text{s.t.} \quad & x + y \le 4, \\
    & 2x + y \le 5, \\
    & -x + 4y \ge 2, \\
    & x,y \ge 0.
    \end{align*}
\f]

The dual problem reads 

\f[
    \begin{align*}
    \max_{\alpha,\beta,\gamma,\delta,\epsilon} \quad & 4\alpha + 5\beta + 2\gamma \\
    \text{s.t.} \quad & \alpha + 2\beta - \gamma + \delta = 3, \\
    & \alpha + \beta + 4\gamma + \epsilon = 2, \\
    & \alpha, \beta \le 0, \\
    & \gamma, \delta, \epsilon \ge 0.
    \end{align*}
\f]

\include ../examples/mixed-integer/lp-dual.example.cpp
