\page cli_bilevel_kkt Solving Bilevel Problems via its KKT-Based Single-Level Reformulation
\brief Describes how to solve a bilevel problem with continuous lower level via its KKT-based single-level reformulation

\tableofcontents

\section kkt_introduction Introduction

We consider an LP-LP bilevel problem given by
\f[
    \begin{align*}
        \min_{x,y} \quad & c^\top x + d^\top y \\
        \text{s.t.} \quad & Ax + By \ge a, \\
        & y \in S(x),
    \end{align*}
\f]
where \\( S(x) \\) denotes the set of solutions to the \\( x \\)-parameterized problem 
\f[
    \begin{align*}
    \min_{y} \quad & f^\top y \\
    \text{s.t.} \quad & Cx + Dy \ge b.
    \end{align*}
\f]

Because the lower-level problem is a linear problem, its Karush-Kuhn-Tucker (KKT) conditions are necessary and sufficient optimality conditions.
Thus, one can derive a single-level reformulation of the bilevel problem by replacing the bilevel constraint by the KKT
conditions of the lower level. This leads to

\f[
    \begin{align*}
    \min_{x,y,\lambda} \quad & c^\top x + d^\top y \\
    \text{s.t.} \quad & Ax + By \ge a, \\
    &  Cx + Dy \ge b, \\
    & D^\top\lambda = f, \\
    & \lambda \ge 0, \\
    & \lambda^\top (Cx + Dy - b) = 0.
    \end{align*}
\f]

This formulation is called the KKT-based single-level reformulation of the bilevel problem and different approaches
exist for solving it. This is detailed in the next sections.

\section kkt_nlp Using the NLP Formulation

Using the option `--method KKT-NLP` automatically formulates and solves the KKT reformulation as a nonlinear optimization problem. 
Obviously, the underlying solver must handle nonlinear constraints.

Simply run

```shell
idol_cl solve bilevel model.lp --aux follower.aux --method KKT-NLP
```

\section kkt_sos1 Using the SOS1 Formulation

The only nonlinearities in the KKT-based single-level reformulation are due to the complementarity constraints 

\f[
    \lambda^\top(Cx + Dy - b) = 0.
\f]

It is well-known that this constraint can be further reformulated as a set of disjunctive constraints of the form 

\f[
    ( \lambda_i = 0 ) \lor ( (Cx + Dy - b)_i = 0 ), \quad \text{for all } i = 1,\dots,m_y.
\f]

Thus, one can further reformulate the KKT-based single-level reformulation as 

\f[
\begin{align*}
    \min_{x,y,\lambda} \quad & c^\top x + d^\top y \\
    \text{s.t.} \quad & Ax + By \ge a, \\
    &  Cx + Dy - b = s, \\
    & D^\top\lambda = f, \\
    & \lambda \ge 0, \\
    & s \ge 0, \\
    & \text{SOS1}(\lambda_i, s_i), \quad\text{for all } i = 1,\dots,m_y.
    \end{align*}
\f]

Here, \\( \text{SOS1}(\lambda_i,s_i) \\) models the fact that \\( \\{ \lambda_i, s_i \\} \\) must be a special ordered set 
of type 1, i.e., at most one value in the set can be nonzero.

Using the option `--method KKT` together with `--sos1` automatically reformulates and solves the bilevel problem using this 
SOS1 approach.
Obviously, the underlying solver must handle SOS1 constraints for this approach to work.

Simply run

```shell
idol_cl solve bilevel model.lp --aux follower.aux --method KKT-SOS1
```

\section kkt_big_m Using the Big-M Reformulation 

Using the Big-M reformulation requires an additional file to provide bounds on the dual variables of the primal lower-level 
problem. The format for this file is detailed in the next section. Given such a file, say `bounds.txt`, the method can be 
triggered as follows.

```shell
idol_cl solve bilevel model.lp --aux follower.aux --method KKT-BIGM --bound-provider bounds.txt
```

\subsection kkt_big_m_file The File Format for Providing Bounds on the Dual Variables

Consider the linear problem 
\f[
    \begin{align*}
        \min_{y_1,y_2} \quad & y_1 + 2y_2 \\
        \text{s.t.} \quad & y_1 + y_2 \ge 1, & (\lambda\ge 0) \\
        & y_1\ge 0, & (\alpha\ge 0) \\
        & y_2\ge 0 & (\beta \ge 0).
    \end{align*}
\f]

For simplicity, it is not parameterized by any leader decision, this obviously does not affect the file format that we describe.

The `.lp` file reads 
```text
Minimize
 obj: y1 + 2 y2
Subject To
 c1: y1 + y2 >= 1
Bounds
 y1 >= 0
 y2 >= 0
End
```

Its KKT system reads

\f[
    \begin{align*}
        & \lambda + \alpha \le 1, \\
        & \lambda + \beta \le 2, \\
        & \lambda, \alpha, \beta \ge 0, \\
        & \lambda(y1 + y2 - 1) = 0, \\
        & \alpha y1 = 0, \\
        & \beta y2 = 0.
    \end{align*}
\f]

We have the following bounds on the dual variables:
\f[
    \begin{equation*}
        0 \le \lambda \le 1, \quad
        0 \le \alpha \le 1, \quad
        0 \le \beta \le 2. \\
    \end{equation*}
\f]

Similarly, we have the following bounds on the primal constraints:
\f[
    \begin{equation*}
        0 \le y_1 + y_2 - 1 \le 2, \quad
        0 \le y_1 \le 1, \quad
        0 \le y_2 \le 2. \\
    \end{equation*}
\f]

These bounds are provided through an additional file whose format is described next.

The associated file for this example reads

```text
@CTR_DUAL
c1 1
@CTR_PRIMAL
c1 2
@LB_DUAL
y1 1
y2 2
@LB_PRIMAL
@UB_DUAL
@UB_PRIMAL
y1 1
y2 2
```

The bounds file used by `idol_cl` is composed of tagged sections, each identified by a keyword starting with `@`.
Each section contains entries specifying bounds associated with different components of the primal–dual system used in the
KKT Big-M reformulation.

The following table lists all supported tags and their meaning.

| Tag           | Meaning                                                                                                                         |
|---------------|---------------------------------------------------------------------------------------------------------------------------------|
| `@CTR_DUAL`   | This section contains the bounds for the dual variables of the respective (primal) constraints                                  |
| `@CTR_PRIMAL` | This section contains the bounds on the activity (slack) of the respective (primal) constraints                                 |
| `@LB_DUAL`    | This section contains the bounds for the dual variables of the lower-bound constraint of the respective (primal) variable       |
| `@LB_PRIMAL`  | This section contains the lower bounds of the respective (primal) variables (not needed if these are already part of the model) |
| `@UB_DUAL`    | This section contains the bounds for the dual variables of the upper-bound constraint of the respective (primal) variable       |
| `@UB_PRIMAL`  | This section contains the upper bounds of the respective (primal) variables (not needed if these are already part of the model) |

Sections may be empty if no bounds of that type are provided.

Each non-empty line inside a section follows the format
```text 
<NAME> <VALUE>
```
where
- `<NAME>` is the name of a constraint or variable appearing in the deterministic optimization model,
- `<VALUE>` is a numeric value specifying the bound used in the Big-M reformulation.

The interpretation of `<NAME>` depends on the section:
- In `@CTR_*` sections, `<NAME>` must correspond to a constraint name;
- In `@LB_*` and `@UB_*` sections, `<NAME>` must correspond to a variable name.

For example,
```text 
@CTR_DUAL
c1 1
```
indicates that the dual variable associated with constraint *c1* has an upper bound of 1 in the KKT Big-M formulation.

\subsection kkt_big_m_def Reference for Dual Variable Bounds

The sign of the dual variables depend on the type of the constraint it is associated to. 
To be more formal, we consider an "extended" follower's problem with both equalities and inequalities.
It is given by

\f[
    \begin{align*}
        \min_y \quad & f^\top y \\
        \text{s.t.} \quad & C^=x + D^=y = b^=, & (\lambda^=\in\mathbb{R}^{m_=}) \\
        & C^\le x + D^\le y \le b^\le, & (\lambda^\le\in\mathbb{R}^{m_\le}_{\le 0}) \\
        & C^\ge x + D^\ge y \ge b^\ge, & (\lambda^\ge\in\mathbb{R}^{m_\ge}_{\ge 0}) \\
        & y \le y^\le, & (\pi^\le\in\mathbb{R}^{n}_{\le 0}) \\
        & y \ge y^\ge. & (\pi^\ge\in\mathbb{R}^{n}_{\ge 0})
    \end{align*}
\f]

The dual problem then reads

\f[
    \begin{align}
        \max_{ \lambda^=, \lambda^\ge, \lambda^\le, \pi^\le, \pi^\ge } \quad &
        (b^= - C^=x)^\top\lambda^=
        + (b^\le - C^\le x)^\top\lambda^\le
          + (b^\ge - C^\ge x)^\top\lambda^\ge
          +\sum_{j:y^\le_j < \infty} (y^\le)^\top\pi^\le
          +\sum_{j:y^\ge_j > -\infty} (y^\ge)^\top\pi^\ge \\
          \text{s.t.} \quad & (D^=)^\top\lambda^= + (D^\le)^\top\lambda^\le + (D^\ge)^\top\lambda^\ge + \pi^\le + \pi^\ge = d, \\
          & \lambda^\le \le 0, \ \lambda^\ge \ge 0, \ \pi^\le \le 0, \ \pi^\ge \ge 0.
    \end{align}
\f]

Similarly, the KKT conditions are stated by

\f[
    \begin{align*}
        \text{Primal feasibility} \quad &
        \begin{cases}
            C^=x + D^=y = b^=, \\
            C^\le x + D^\le y \le b^\le, \\
            C^\ge x + D^\ge y \ge b^\ge, \\
            y \le y^\le, \\
            y \ge y^\ge,
        \end{cases} \\
        \text{Dual feasibility} \quad &
        \begin{cases}
            \lambda^\le \le 0, \\
            \lambda^\ge \ge 0, \\
            \pi^\le \le 0, \\
            \pi^\ge \ge 0,
        \end{cases} \\
        \text{Stationarity} \quad &
        \begin{cases}
            (D^=)^\top\lambda^= + (D^\le)^\top\lambda^\le + (D^\ge)^\top\lambda^\ge + \pi^\le + \pi^\ge = d,
        \end{cases} \\
        \text{Complementarity} \quad &
        \begin{cases}
            (C^\le x + D^\le y - b^\le)^\top \lambda^\le = 0, \\
            (C^\ge x + D^\ge y - b^\ge)^\top \lambda^\ge = 0, \\
            (y - y^\le)^\top \pi^\le = 0, \\
            (y - y^\ge)^\top \pi^\ge = 0.
        \end{cases}
    \end{align*}
\f]

Hence, the linearized complementarity conditions are given as follows.

\f[
    \begin{align*}
        & M_i^\le u^\le_i \le \lambda^\le_i \le 0,
        \quad N_i^\le(1 - u_i^\le) \le (C^\le x + D^\le y - b^\le)_i \le 0,  \quad \text{for all } i=1,\dotsc,m_\le,\\
        & M_i^\ge u^\ge_i \ge \lambda^\ge_i \ge 0,
        \quad N_i^\ge(1 - u_i^\ge) \ge (C^\ge x + D^\ge y - b^\ge \ge 0)_i,  \quad \text{for all } i=1,\dotsc,m_\ge,\\
        & O_j^\le v_j^\le \le \pi^\le_i \le 0,
        \quad P_i^\le(1 - v_j^\le) \le (y - y^\le)_i \le 0, \quad \text{for all }j=1,\dotsc,n, \\
        & O_j^\ge v_j^\ge \ge \pi^\ge \ge 0,
        \quad P_i^\le(1 - v_j^\le) \ge (y - y^\ge)_i \ge 0, \quad \text{for all }j=1,\dotsc,n, \\
        & u^\le\in\{0,1\}^{m_\le}, \quad u^\ge\in\{0,1\}^{m_\ge}, \quad
        v^\le\in\{0,1\}^{n}, \quad v^\ge\in\{0,1\}^{n}.
    \end{align*}
\f]

Therefore, it is clear that every inequality constraint needs only one bound for its dual variable as well as only one bound for its slack.
For less-or-equal-to constraints, the big-M values must be non-positive. For greater-or-equal-to, they must be non-negative.

\section kkt_other_milp_solver Changing the Underlying MILP Solver

By default, `idol_cl` will choose a MILP solver automatically for solving the obtained single-level relaxation. 

To change the underlying solver, use the `--default-sub-milp-method` option as follows.

```shell
idol_cl solve bilevel model.lp --aux follower.aux --method KKT-BIGM --bound-provider bounds.txt --default-sub-milp-method GLPK
```

Here, `GLPK` will now be used to solve the single-level reformulation of the bilevel problem.

\section kkt_kleinert_vi Deactivating Kleinart et al. (2020) Valid Inequalities

If all upper-level variables are bounded, every KKT single-level reformulation incorporates the primal-dual valid inequality
described in <a href="https://doi.org/10.1007/s11590-020-01660-6" target="_blank">Kleinart et al. (2020)</a>.

This inequality is given as

\f[
    f^\top y \le \lambda^\top( b - C^- ),
\f]
where \\( C^-_i \ge \min \\{ C_i x \\} \\{ x\in \\{ x^\ge, x^\le \\} \\} \\) for all \\( i \\).

This is done automatically. If, however, you want to deactivate this functionality, please use the `--no-kleinert-vi` option.

Simply run

```shell
idol_cl solve bilevel model.lp --aux follower.aux --method KKT-SOS1 --no-kleinert-vi
```