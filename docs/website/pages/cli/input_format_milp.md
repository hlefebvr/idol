\page cli_input_format_milp Input Format for MILPs
\brief Describes the input file format for MILPs.

\tableofcontents

For MILPs, `idol_cl` accepts `.mps` and `.lp` files.

These are standard formats used by solvers such as Gurobi, Cplex, GLPK, and HiGHS.

In most cases, the file parsing is performed by the underlying solver used by `idol`.

For more details, refer to <a href="https://www.ibm.com/docs/en/icos/22.1.2?topic=cplex-lp-file-format-algebraic-representation" target="_blank">the IBM page on the .lp format</a>
or the <a href="https://lpsolve.sourceforge.net/5.5/mps-format.htm" target="_blank">lpsolve page on the .mps format</a>.

\subsection milp_example_lp Example (.lp and .mps)

Example of a simple MILP:

\f[
\begin{align}
\min_{x,y} \quad & 3x + 4y \\
\text{s.t.} \quad
& 2x + y \ge 5 \\
& x + 2y \ge 6 \\
& x, y \in \mathbb{Z}_{\ge 0}
\end{align}
\f]

Corresponding `.lp` file:

```text
Minimize
 obj: 3 x + 4 y

Subject To
 c1: 2 x + y >= 5
 c2: x + 2 y >= 6

Bounds
 x >= 0
 y >= 0

Generals
 x
 y

End
```

Equivalent `.mps` file:

```text
NAME          example
ROWS
 N  obj
 G  c1
 G  c2
COLUMNS
    x       obj     3
    x       c1      2
    x       c2      1
    y       obj     4
    y       c1      1
    y       c2      2
RHS
    rhs     c1      5
    rhs     c2      6
BOUNDS
 LO bnd     x       0
 LO bnd     y       0
ENDATA
```
