\page cli_robust_basics Basics of the Command Line Interface for Robust Problems
\brief Describes the input file format for robust problems.

\tableofcontents

\section cli_robust_basics_static Static Robust Optimization

\subsection cli_robust_basics_intro Introduction

A (classic) static robust optimization considers problems of the form 

\f[
\begin{align*}
    \min_{x\in X} \quad & c^\top x \\
    \text{s.t.} \quad & \sum_{j=1}^n (\bar{a}_j + \hat{a}_ju_j)x_j \ge b, \quad\text{for all } u\in U.
\end{align*}
\f]

Here \\(U\\) models the uncertainty set which contains a (possibly infinite) set of possible scenarios for the parameters \\(u\\).

\warning TODO, add decision-dependent problems

\subsection cli_robust_basics_input Input File Format

\subsubsection cli_robust_basics_input_example A First Example

We consider the robust problem 

\f[
\begin{align*}
\min_{x,y} \quad & -x - 2y \\
\text{s.t.} \quad & (1 + u_1) x + (1 + u_2) y \le 2, \quad\text{for all } u\in U, \\
& x,y \in\{ 0,1 \},
\end{align*}
\f]
where \f( U := \left\{ u\in\{0;1\}^2 : u_1 + u_2 \le 1 \right\} \f).

The `.mps` file for the deterministic problem is given by

```text
NAME instance
ROWS
 N  OBJ
 L  c
COLUMNS
    __constant OBJ        1
    MARKER    'MARKER'                 'INTORG'
    x          OBJ        -1
    x          c          1
    y          OBJ        -2
    y          c          1
    MARKER    'MARKER'                 'INTEND'
RHS
    RHS       c          2
BOUNDS
 FX BND       __constant 0
 LI BND       x          0
 UI BND       x          1
 LI BND       y          0
 UI BND       y          1
ENDATA
```

The `.mps` file for the uncertainty set is given by

```text
NAME instance-unc
ROWS
 N  OBJ
 L  budget
COLUMNS
    __constant OBJ        1
    MARKER    'MARKER'                 'INTORG'
    u_1        OBJ        0
    u_1        budget     1
    u_2        OBJ        0
    u_2        budget     1
    MARKER    'MARKER'                 'INTEND'
RHS
    RHS       budget     2
BOUNDS
 FX BND       __constant 0
 LI BND       u_1        0
 UI BND       u_1        1
 LI BND       u_2        0
 UI BND       u_2        1
ENDATA
```

Finally, an additional file, with extension `.par`, is used to indicate the uncertainty parameterization in the deterministic model

```text
@RHS
@OBJ
@MAT
c	y	u_2	1
c	x	u_1	1
```

Here, the uncertain parameters enter the constraint matrix. 
For instance, in the constraint with name "c", the variable with name "x" has an uncertain coefficient of one associated to the parameter "u_1".

\subsubsection cli_robust_basics_input_detailed Detailed Description

In `idol` a static robust instance is made of three files:
- A first `.mps`/`.lp` stores the model for the deterministic problem, i.e., here,
  \f[
  \begin{align*}
  \min_{x\in X} \quad & c^\top x \\
  \text{s.t.} \quad & \sum_{j=1}^n \bar{a}_jx_j \ge b.
  \end{align*}
  \f]
- A second `.mps`/`.lp` stores the uncertainty set \\( U \\).
- A `.par` file indicates where the uncertain parameters enter in the deterministic problem. Here, it should indicate that
  each variable \\(x_j\\) has an uncertain coefficient \\( \hat{a}_ju_j \\) in the respective constraint.

In case multiple uncertainty sets must be used, they are all represented in a single `.mps`/`.lp`.

For more details on the `.mps` and `.lp` file formats, see the page \ref cli_input_format_milp.

The `.par` file is composed of tagged sections, each identified by a keyword starting with `@`. The following table lists all supported tags and their meaning. 


| Tag    | Meaning                                                                                    |
|--------|--------------------------------------------------------------------------------------------|
| `@RHS` | This section contains the uncertain parameterization of the constraints right-hand side    |
| `@OBJ` | This section contains the uncertain parameterization of the objective function coefficients |
| `@MAT` | This section contains the uncertain parameterization of the constraint matrix              |

Sections may be empty if no uncertainty of that type exists in the problem.

\paragraph cli_robust_rhs_file Right-hand Side Uncertainty

The `@RHS` section is composed by a list of entries with the format `<CTR> <UNC_VAR> <COEFF>`, where `<CTR>` is a constraint name, 
`<UNC_VAR>` is an uncertainty parameter name (which must be defined in the uncertainty set), and `<COEFF>` is a number.
Each entry specifies that the right-hand side of the constraint `<CTR>` has an uncertain contribution equal to `<COEFF>` times `<UNC_VAR>`.

\paragraph cli_robust_obj_file Objective Uncertainty

The `@OBJ` section is composed of a list of entries with the format `<VAR> <UNC_VAR> <COEFF>`, where `<VAR>` is a variable name,
`<UNC_VAR>` is an uncertainty parameter name (which must be defined in the uncertainty set), and `<COEFF>` is a number.
Each entry specifies that the objective coefficient of the variable `<VAR>` has an uncertain contribution equal to `<COEFF>` times `<UNC_VAR>`.

\paragraph cli_robust_mat_file  Constraint Matrix Uncertainty

The `@MAT` section is composed of a list of entries with the format `<CTR> <VAR> <UNC_VAR> <COEFF>`, where `<CTR>` is a constraint name,
`<VAR>` is a variable name, `<UNC_VAR>` is an uncertainty parameter name (which must be defined in the uncertainty set), and `<COEFF>` is a number. 
Each entry specifies that the coefficient of the variable `<VAR>` in the constraint `<CTR>` has an uncertain contribution equal to `<COEFF>` times `<UNC_VAR>`.

\warning TODO, add decision-dependent problems

\section cli_robust_basics_aro Two-Stage Robust Optimization

\subsection cli_robust_basics_aro_introduction Introduction

Two-stage robust optimization problems are problems of the form

\f[
    \begin{align*}
        \min_{x\in X} \ c_x^\top x + \max_{u\in U} \ \min_{y\in Y(x,u) } \ c_y^\top y,
    \end{align*}
\f]
where \\( U \\) denotes the uncertainty set, \\( X \\) denotes the first-stage decision feasible region and \\( Y(x,u) \\)
denotes the second-stage decision feasible region.

\subsection cli_robust_basics_aro_input Input File Format

\subsubsection cli_robust_basics_aro_input_example A First Example

We consider the two-stage robust problem 

\f[
    \begin{align*}
    \min_{x\in \mathbb{Z}\cap[0,5]} \ 3x + \max_{u\in [0,3]} \ \min_{y} \ 10y \quad \text{s.t.} \quad x + y \ge 5 + u, \ y\ge 0.
    \end{align*}
\f]

The `.mps` file for the deterministic model reads 

```text
NAME instance
ROWS
 N  OBJ
 G  demand
COLUMNS
    __constant OBJ        1
    x          OBJ        3
    x          demand     1
    y          OBJ        10
    y          demand     1
RHS
    RHS       demand     5
BOUNDS
 FX BND       __constant 0
 LI BND       x          0
 UI BND       x          5
 LO BND       y          0
ENDATA
```

The `.mps` file for the uncertainty set reads 

```text 
NAME instance-unc
ROWS
 N  OBJ
COLUMNS
    __constant OBJ        1
    u          OBJ        0
RHS
BOUNDS
 FX BND       __constant 0
 LO BND       u          0
 UP BND       u          3
ENDATA
```

The `.par` file for the uncertainty parameterization reads 

```text
@RHS
demand u 1
@OBJ
@MAT
```

The `aux` file for indicating the second-stage variables and constraints reads 

```text 
@NUMVARS
1
@NUMCONSTRS
1
@VARSBEGIN
y    0
@VARSEND
@CONSTRSBEGIN
demand
@CONSTRSEND
@NAME
instance
@MPS
instance.mps
```

\subsubsection cli_robust_basics_aro_input_detailed Detailed Description

The format used to describe two-stage robust problems is based on how static robust problems are described but with an additional file
indicating which variable and constraint belong to the second stage.

Thus, four files are necessary to define a two-stage robust problem. 

- A first `.mps`/`.lp` stores the model for the deterministic problem, i.e., here,
  \f[
    \min_{x\in X, y\in Y(x,0) } \quad c^\top x
  \f]
- A second `.mps`/`.lp` stores the uncertainty set \\( U \\).
- A `.par` file indicates where the uncertain parameters enter in the deterministic problem. 
- A `.aux` file indicates which variable and constraint belong to the second-stage problem.

For more details on the `.mps` and `.lp` file formats, see the page \ref cli_input_format_milp.

For more details on the `.par` file format, see the section on \ref cli_robust_static.

For more details on the `.aux` file format, see the page \ref cli_input_format_bilevel. 

<div class="info">
    <span>
        Note that **second-stage objective coefficients should be defined in the respective deterministic model.**
        More specifically, they must be set to zero in the `.aux` file.  
        <br />
        If lower-level coefficients are given in the `.aux` file, say \\(d\\), then the problem will be considered as a robust bilevel optimization problem with a wait-and-see follower, i.e.,
        problems of the form
        \f[
            \begin{align*}
                \min_{x\in X} \ c_x^\top x + \max_{u\in U} \ \min_{y\in S(x,u) } \ c_y^\top y,
            \end{align*}
        \f]
        where \\( S(x,u) \\) denotes the set of solutions to the \\((x,u)\\)-parameterized problem 
        \f[
            \min_{y'} \ d^\top y \quad\text{s.t.}\quad y'\in Y(x,u).
        \f]
    </span>
</div>

\subsection cli_robust_basics_aro_solving Solving a Two-Stage Robust Problem

Given a two-stage robust problem, you can solve it by running 

```shell
idol_cl solve robust model.lp --aux stages.aux --par parameters.par --unc uncertainty-set.mps
```

Here, `model.lp` is the deterministic model, `stages.aux` is the auxiliary file indicating which variable and constraints
belong to the second-stage and `uncertainty-set.mps` denotes the uncertainty set.

Here again, to list the available methods for your problem class, you may use

```shell
idol_cl list robust model.lp --aux stages.aux --par parameters.par --unc uncertainty-set.mps
```