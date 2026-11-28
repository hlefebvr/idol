\page cli_reference Command Reference
\brief Complete reference for all idol_cl commands, options and exit codes.

\tableofcontents

`idol_cl` is the command-line interface for idol. It is organised as a
two-level hierarchy: a **problem class** (`milp`, `bilevel`, `robust`) followed
by a **subcommand** (`solve` or `list`).

```text
idol_cl [GLOBAL OPTIONS] <problem> <subcommand> [file] [OPTIONS]
```

---

\section cli_ref_global Global Options

These options are accepted before the problem class and apply to every invocation.

| Option | Description |
|:-------|:------------|
| `-h`, `--help` | Print help and exit. |
| `--help-all` | Print help for all subcommands and exit. |
| `-v`, `--version` | Print the version and detected solver versions. |
| `-c FILE`, `--config FILE` | Load options from an `.ini` configuration file. |

Any option that appears in a subcommand (e.g. `--method`, `--time-limit`) may also
be stored in the `.ini` file and loaded with `--config`. Command-line flags
override file values.

---

\section cli_ref_milp milp

Solves or lists methods for a mixed-integer linear program stored in an `.lp` or
`.mps` file.

\subsection cli_ref_milp_solve milp solve \<file\>

```shell
idol_cl milp solve model.lp [OPTIONS]
idol_cl milp solve model.mps [OPTIONS]
```

| Option | Type | Default | Description |
|:-------|:-----|:--------|:------------|
| `--method` | string | auto | Solver: `Gurobi`, `HiGHS`, `GLPK`, `Cplex`, `JuMP`. |
| `--time-limit` | seconds | none | Wall-clock time limit. |
| `--thread-limit` | integer | 1 | Maximum number of threads. |
| `--tol-feasibility` | float | solver default | Feasibility tolerance. |
| `--save-primal` | file | — | Write the incumbent to a `.sol` file. |
| `--csv-report` | flag | off | Print a one-line CSV summary after solving. |
| `--default-method` | string | — | MILP solver used for internal sub-problems (e.g. inside decompositions). |
| `--julia-using` | string | — | Comma-separated Julia modules to load (JuMP only). |
| `--jump-optimizer` | string | — | JuMP optimizer name (required with `--method JuMP`). |

\subsection cli_ref_milp_list milp list \<file\>

```shell
idol_cl milp list model.lp
```

Lists all methods that are applicable to the detected problem type, together with
their requirements and whether each requirement is currently satisfied.
Accepts the same file and common options as `solve` (except `--save-primal`).

---

\section cli_ref_bilevel bilevel

Solves or lists methods for a bilevel problem described by an `.lp`/`.mps` file
(the single-level relaxation) and an `.aux` file (the follower structure).

\subsection cli_ref_bilevel_solve bilevel solve \<file\>

```shell
idol_cl bilevel solve model.mps --aux follower.aux [OPTIONS]
```

| Option | Type | Default | Description |
|:-------|:-----|:--------|:------------|
| `--aux` | file | — | `.aux` file describing the lower-level structure. Mutually exclusive with `--ce`. |
| `--method` | string | auto | Bilevel method (see table below). |
| `--default-method` | string | — | MILP solver for internal sub-problems. |
| `--time-limit` | seconds | none | Wall-clock time limit. |
| `--thread-limit` | integer | 1 | Maximum number of threads. |
| `--save-primal` | file | — | Write the incumbent to a `.sol` file. |
| `--csv-report` | flag | off | Print a one-line CSV summary. |
| `--pessimistic` | flag | off | Solve the pessimistic variant (requires no coupling constraints). |
| `--no-kleinert-vi` | flag | off | Disable the Kleinert et al. (2020) valid inequality for KKT methods. |
| `--bound-provider` | file | — | Big-M bounds file for `KKT_BigM`. |

**Available methods:**

| `--method` | Lower level | Requirements |
|:-----------|:------------|:-------------|
| `MibS` | Integer | MibS installed |
| `KKT_SOS1` | Continuous LP | Solver with SOS1 (Gurobi, CPLEX) |
| `KKT_BigM` | Continuous LP | `--bound-provider` file |
| `KKT_NLP` | Continuous LP | Nonlinear-capable solver |
| `StrongDuality` | Continuous LP | — |

\subsection cli_ref_bilevel_list bilevel list \<file\>

```shell
idol_cl bilevel list model.mps --aux follower.aux
```

Lists all applicable methods for the problem, with requirement checks.

---

\section cli_ref_robust robust

Solves or lists methods for a static or two-stage robust problem.
Problem type is inferred from the presence of `--aux`:

- **No `--aux`** → static (single-stage) robust problem.
- **With `--aux`** → two-stage robust problem.

\subsection cli_ref_robust_solve robust solve \<file\>

```shell
# Static robust
idol_cl robust solve model.mps --par params.par --unc uncertainty.mps [OPTIONS]

# Two-stage robust
idol_cl robust solve model.mps --aux stages.aux --par params.par --unc uncertainty.mps [OPTIONS]
```

| Option | Type | Default | Description |
|:-------|:-----|:--------|:------------|
| `--aux` | file | — | `.aux` file marking second-stage variables and constraints. |
| `--par`, `--uncertainty-param` | file | — | `.par` file describing where uncertainty enters the model. Required together with `--unc`. |
| `--unc`, `--uncertainty-set` | file | — | `.mps`/`.lp` file for the uncertainty set. Required together with `--par`. |
| `--method` | string | auto | Separation method (see table below). |
| `--default-method` | string | — | MILP solver for internal sub-problems. |
| `--time-limit` | seconds | none | Wall-clock time limit. |
| `--thread-limit` | integer | 1 | Maximum number of threads. |
| `--complete-recourse` | flag | off | Signal that the second stage always has a feasible solution (required by `CCG_MibS`). |
| `--n-policies` | integer | — | Number of policies for K-adaptability approaches. |
| `--save-primal` | file | — | Write the incumbent to a `.sol` file. |
| `--csv-report` | flag | off | Print a one-line CSV summary. |
| `--bound-provider` | file | — | Big-M bounds file for KKT-based separation. |

**Available methods:**

| `--method` | Problem | Uncertainty | 2nd stage | Requirements |
|:-----------|:--------|:------------|:----------|:-------------|
| `CG_Indicator` | Static | Any | — | — |
| `CCG_KKT_SOS1` | Two-stage | Any integer/continuous | Continuous | Solver with SOS1 |
| `CCG_Farkas` | Two-stage | Binary / {0,1} polytope | Continuous | — |
| `CCG_MibS` | Two-stage | General integer | Any | MibS, `--complete-recourse` |
| `Convexification` | Two-stage | Continuous, objective only | Continuous | — |

\subsection cli_ref_robust_list robust list \<file\>

```shell
idol_cl robust list model.mps --par params.par --unc uncertainty.mps
idol_cl robust list model.mps --aux stages.aux --par params.par --unc uncertainty.mps
```

---

\section cli_ref_formats File Formats

| Extension | Used for | Reference |
|:----------|:---------|:----------|
| `.lp` / `.mps` | MILP model (single-level relaxation for bilevel/robust) | \ref cli_milp_basics |
| `.aux` | Bilevel follower structure; two-stage second-stage assignments | \ref cli_bilevel_basics |
| `.par` | Uncertainty parameterization (where \\(u\\) enters the model) | \ref cli_robust_basics |
| `.sol` | Primal solution output (Gurobi SOL format, written by `--save-primal`) | — |
| `.ini` | Configuration file for `--config` | — |

---

\section cli_ref_exit Exit Codes

| Code | Meaning |
|:-----|:--------|
| `0` | Solve completed (status may be `Optimal`, `Feasible`, `Infeasible`, `Unknown`, etc.) |
| `1` | Fatal error (bad arguments, missing file, solver exception) |

A non-optimal or time-limited result exits with code `0`; code `1` is reserved
for failures that prevented the solver from running at all.
