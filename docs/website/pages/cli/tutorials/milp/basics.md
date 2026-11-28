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
    MARKER    'MARKER'    'INTORG'
    x         obj         3
    x         c1          2
    x         c2          1
    y         obj         4
    y         c1          1
    y         c2          2
    MARKER    'MARKER'    'INTEND'
RHS
    rhs       c1          5
    rhs       c2          6
BOUNDS
 LO bnd       x           0
 LO bnd       y           0
ENDATA
```

The `MARKER 'INTORG'` / `MARKER 'INTEND'` lines delimit the integer variable block.
Without them all variables are treated as continuous.

\section cli_milp_basics_first Solving Your First MILP

To solve the example MILP above, save the `.lp` file as `model.lp` and run:

```shell
idol_cl milp solve model.lp
```

`idol_cl` automatically selects a suitable solver for the detected problem type.
To specify one explicitly and add a time limit:

```shell
idol_cl milp solve model.lp --time-limit 3600 --method GLPK
```

**Expected output** (solver may differ):
```text
╔════════════════════════════════════════╗
║               idol 0.10.3              ║
║    A C++ Framework for Optimization    ║
║         by Henri Lefebvre, 2026        ║
║     https://henrilefebvre.com/idol     ║
╚════════════════════════════════════════╝
-- No configuration file loaded.
-- No time limit was configured (use --time-limit to configure).
-- Using up to 1 threads (use --thread-limit to configure).
-- Problem type is MILP.
...
Status: Optimal
Reason: Proved
Objective value: 14
x = 2
y = 2
```

The unique optimal solution is \\( x = 2, y = 2 \\) with objective value \\( 14 \\).

To save the incumbent to a file, use `--save-primal`:

```shell
idol_cl milp solve model.lp --method GLPK --save-primal solution.sol
```

The `.sol` file contains the objective value as a comment followed by one
variable-value pair per line.

To see which solution methods are available for a given problem, run:

```shell
idol_cl milp list model.lp
```

**Sampled output:**
```text
Applicable Methods
Gurobi
Description: Gurobi Optimizer [https://www.gurobi.com/]
Required Assumptions:
 ✓ Requires Gurobi to be installed.

GLPK
Description: GLPK (GNU Linear Programming Kit) [https://www.gnu.org/software/glpk/]
Required Assumptions:
 ✓ Requires GLPK to be installed.
 ✓ Requires no SOS-type constraints.
 ✓ Requires no quadratic constraints.
 ✓ Requires a linear objective function.

HiGHS
Description: High Performance Software for Linear Optimization [https://highs.dev/]
Required Assumptions:
 ✓ Requires HiGHS to be installed.
 ✓ Requires no SOS-type constraints.
 ✓ Requires no quadratic constraints.
 ✓ Requires a linear objective function.
```

For solver configuration (environment variables, search paths), see \ref solver_configuration.

\section cli_milp_basics_options Common Options

The following options apply to all `idol_cl milp solve` commands.

| Option | Description | Example |
|:-------|:------------|:--------|
| `--method` | Solver to use (`Gurobi`, `HiGHS`, `GLPK`, `Cplex`, `JuMP`) | `--method HiGHS` |
| `--time-limit` | Wall-clock limit in seconds | `--time-limit 300` |
| `--thread-limit` | Maximum number of threads | `--thread-limit 4` |
| `--tol-feasibility` | Feasibility tolerance | `--tol-feasibility 1e-6` |
| `--save-primal` | Write incumbent to a `.sol` file | `--save-primal out.sol` |
| `--csv-report` | Also print a one-line CSV summary | `--csv-report` |

**Example — time-limited solve with four threads:**
```shell
idol_cl milp solve model.lp --method Gurobi --time-limit 300 --thread-limit 4
```

**Example — read settings from a configuration file:**

Options can also be stored in an `.ini` file and loaded with `--config`:

```ini
method = HiGHS
time-limit = 600
thread-limit = 2
```

```shell
idol_cl milp solve model.lp --config settings.ini
```
