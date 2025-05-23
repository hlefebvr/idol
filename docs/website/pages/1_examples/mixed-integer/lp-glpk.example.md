\page example_mixed_integer_lp_glpk lp-glpk
\brief Models a simple linear problem and optimizes it with GLPK. If the is feasible, prints the primal and dual solution.
    If the model is infeasible, prints a Farkas certificate (dual ray). If the model is unbounded, prints a primal ray.

The LP model is taken from "Model building in Mathematical Programming" by H. Paul Williams and is
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

\include ../examples/mixed-integer/lp-glpk.example.cpp
