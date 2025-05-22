\page example_bilevel_moore_bard_mibs moore-bard-mibs
\brief Models the well-known Moore and Bard (1990) example of a mixed-integer bilevel problem and solves it with the external 
    solver MibS.

This example is taken from "The Mixed Integer Linear Bilevel Programming Problem" (Moore and Bard, 1990).

\f[
    \begin{align*}
        \min_{x,y} \quad & -x -10 y \\
        \text{s.t.} \quad & x\in\mathbb{Z}_{\ge 0}, \\
        & y\in S(x),
    \end{align*}
\f]
with \\( S(x) \\) being the set of optimal points to the lower level problem
\f[
    \begin{align*}
        \min_y \quad & y \\
        \text{s.t.} \quad & -25 x + 20 y \le 30, \\
        &   x + 2 y \le 10, \\
        & 2 x - y \le 15, \\
        & 2 x + 10 y \ge 15, \\
        & y\in\mathbb{Z}_{\ge 0} \\
    \end{align*}
\f]

\include ../examples/bilevel/moore-bard-mibs.example.cpp
