\page cli_bilevel_mibs Solving Bilevel Problems with MibS
\brief Describes how to use the MibS solver for mixed-integer bilevel problems.

\tableofcontents

\section cli_bilevel_mibs_intro When to Use MibS

Use `--method MibS` when the follower's variables are integer.
For a continuous lower level, use a KKT-based method instead (see \ref cli_bilevel_kkt).

\note MibS must be compiled into `idol_cl`. It is included automatically with the
\ref automated_install "automated installation script" and package manager installs.
See \ref solver_config_mibs if you built from source.

\section cli_bilevel_mibs_example Example

We use the Moore-Bard example from \ref cli_bilevel_basics
(`moore-and-bard.mps`, `moore-and-bard.aux`). The follower variable \\(y\\) is
integer, so MibS applies. The figure below shows the follower's integer best
response \\(y^*(x)\\) for each leader choice \\(x\\), and the leader's optimum at
\\((x^*,y^*)=(2,2)\\).

<div style="text-align:center;margin:1.5em 0;">
<img src="bilevel_integer.png" alt="Integer bilevel best-response" style="max-width:380px;">
</div>

```shell
idol_cl bilevel solve moore-and-bard.mps --aux moore-and-bard.aux --method MibS
```

**Expected output:**
```text
Status: Optimal
Reason: Proved
Objective value: -22
x = 2
y = 2
```

\section cli_bilevel_mibs_vs_kkt MibS vs. KKT Methods

| | MibS | KKT (SOS1 / BigM) |
|:--|:-----|:------------------|
| Follower variables | **Integer** | Continuous |
| Underlying solver | MibS (shipped) | Any MILP solver |
| Globally optimal | Yes | Yes (given valid Big-M) |

To confirm which methods apply to a given instance:

```shell
idol_cl bilevel list moore-and-bard.mps --aux moore-and-bard.aux
```
