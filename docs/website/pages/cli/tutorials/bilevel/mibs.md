\page cli_bilevel_mibs Solving Bilevel Problems with MibS
\brief Describes how to use the MibS solver for mixed-integer bilevel problems.

\tableofcontents

\section cli_bilevel_mibs_intro When to Use MibS

The `MibS` method handles mixed-integer bilevel problems — where the follower's
variables are integer — by calling the
<a href="https://github.com/hlefebvr/MibS" target="_blank">customized MibS solver</a>
(idol branch).

\note MibS must be compiled into `idol_cl`. It is included automatically when
using the \ref automated_install "automated installation script" or a package
manager install. See \ref solver_config_mibs if you built from source.

The method requires:
- The follower's linking variables must be integer (not purely continuous).
- MibS must be available.

For problems with a continuous lower level, use a KKT-based method instead
(see \ref cli_bilevel_kkt).

\section cli_bilevel_mibs_example Example

We use the Moore-Bard example from \ref cli_bilevel_basics
(`moore-and-bard.mps` and `moore-and-bard.aux`). The follower variable \\(y\\)
is integer, so MibS applies.

```shell
idol_cl bilevel solve moore-and-bard.mps --aux moore-and-bard.aux --method MibS
```

**Expected output:**
```text
-- Problem type is bilevel problem.
...
Status: Optimal
Reason: Proved
Objective value: -22
x = 2
y = 2
```

\section cli_bilevel_mibs_options Options

The underlying MILP solver used by MibS internally (for the LP relaxations) can
be changed with `--default-method`:

```shell
idol_cl bilevel solve moore-and-bard.mps --aux moore-and-bard.aux \
    --method MibS --default-method HiGHS
```

A time limit applies to the full bilevel solve:

```shell
idol_cl bilevel solve moore-and-bard.mps --aux moore-and-bard.aux \
    --method MibS --time-limit 300
```

\section cli_bilevel_mibs_vs_kkt MibS vs. KKT Reformulations

| | MibS | KKT (SOS1 / BigM) |
|:--|:-----|:------------------|
| Follower variables | Integer | Continuous |
| Underlying solver | MibS (shipped) | Any MILP solver |
| Globally optimal | Yes | Yes (given valid Big-M) |
| Commercial solver required | No | No (GLPK, HiGHS work) |

For listing all applicable methods for a given instance:

```shell
idol_cl bilevel list moore-and-bard.mps --aux moore-and-bard.aux
```
