\page cli_milp_basics Basics of the Command Line Interface for MILPs
\brief Describes the basic usage of `idol_cl` for MILPs, including the expected input file format.

\tableofcontents

\section cli_milp_basics_input Input File Format

For MILPs, `idol_cl` accepts `.mps` and `.lp` files.

These are standard formats used by solvers such as Gurobi, Cplex, GLPK, and HiGHS.

In most cases, the file parsing is performed by the underlying solver used by `idol`.

For more details, refer to <a href="https://www.ibm.com/docs/en/icos/22.1.2?topic=cplex-lp-file-format-algebraic-representation" target="_blank">the IBM page on the .lp format</a>
or the <a href="https://lpsolve.sourceforge.net/5.5/mps-format.htm" target="_blank">lpsolve page on the .mps format</a>.

\subsection cli_milp_example_lp Example (.lp and .mps)

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

\section cli_milp_basics_first Solving Your First MILP

To solve a given MILP stored in an `.lp` file, all you need to do is

```shell
idol_cl solve model.lp
```

If you want to set a time limit and use a specific method, run

```shell
idol_cl solve model.lp --time-limit 3600 --method GLK
```

To see which solution methods are available for a given problem, use the `list-methods` subcommand.

```shell
idol_cl list-methods model.lp
```

**Sampled Output**:
```text 
-- No configuration file loaded
-- The main input file is model.lp
-- Detected: problem type is MILP
-- Detected: applicable methods are [GUROBI, CPLEX, HIGHS, GLPK, JUMP]

Details (all methods):

 - HIGHS: High Performance Software for Linear Optimization [https://highs.dev/]
 - JUMP: Calls the Julia package JuMP [https://jump.dev/]
 - GLPK: GLPK (GNU Linear Programming Kit) [https://www.gnu.org/software/glpk/]
 - CPLEX: IBM ILOG CPLEX Optimization Studio [https://www.ibm.com/fr-fr/products/ilog-cplex-optimization-studio]
 - GUROBI: Gurobi Optimizer [https://www.gurobi.com/]
```

If no method is specified, idol_cl automatically selects a suitable solver
for the detected problem type.