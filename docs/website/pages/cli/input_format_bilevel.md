\page cli_input_format_bilevel Input Format for Bilevel Problems
\brief Describes the input file format for bilevel problems.

\tableofcontents

For bilevel problems, `idol_cl` uses the `.aux` file format from the <a href="https://github.com/coin-or/MibS" target="_blank">MibS solver</a>
coupled with an `.lp`/`.mps` file that stores the single-level relaxation.

\subsection example Example

We consider the bilevel problem

\f[
    \begin{align*}
        \min_{x, y} \quad & -x - 10y \\
        \text{s.t.} \quad
        & y \in \begin{array}[t]{rl}
        \displaystyle\arg\min_{z} \ & z \\
        \text{s.t.} \ & -25x + 20z \le 30, \\
        & x + 2z \le 10, \\
        & 2x - z \le 15, \\
        & 2x + 10z \ge 15, \\
        & z \ge 0, \\
        & z \in \mathbb{Z}.
        \end{array} \\
        & x \in \mathbb{Z}_{\ge 0}.
    \end{align*}
\f]

This example is taken from the paper
<a href="https://doi.org/10.1287/opre.38.5.911" target="_blank">"The Mixed Integer Linear Bilevel Programming Problem" by Moore and Bard (1990)</a>.

The `.mps` stores the single-level relaxation.

```text
NAME moore-and-bard
ROWS
 N  OBJ
 L  c_2
 L  c_3
 L  c_4
 G  c_5
COLUMNS
    __constant OBJ        1
    MARKER    'MARKER'                 'INTORG'
    x          OBJ        -1
    x          c_5        2
    x          c_4        2
    x          c_3        1
    x          c_2        -25
    y          OBJ        -10
    y          c_5        10
    y          c_4        -1
    y          c_3        2
    y          c_2        20
    MARKER    'MARKER'                 'INTEND'
RHS
    RHS       c_2        30
    RHS       c_3        10
    RHS       c_4        15
    RHS       c_5        15
BOUNDS
 FX BND       __constant 0
 LI BND       x          0
 LI BND       y          0
ENDATA
```

The `.aux` file describes what variables and constraints belong to the lower level and defines the lower-level objective coefficients.

```text
@NUMVARS
1
@NUMCONSTRS
4
@VARSBEGIN
y	1
@VARSEND
@CONSTRSBEGIN
c_2
c_3
c_4
c_5
@CONSTRSEND
@NAME
moore-and-bard
@MPS
moore-and-bard.mps
```

**Note**: in `idol_cl`, the field `@MPS` or `@LP` is typically ignored to avoid path corruption. 
For that reason, the `.mps`/`.lp` file must explicitly be given.

\subsection details Detailed Description

Bilevel problem instances are described by two files. First, an `.mps` or `.lp` file stores the single-level relaxation model. 
Then, the `.aux` file describes which variables and constraint belong to the lower-level problem.

For more details on the `.mps` and `.lp` file formats, see the page \ref cli_input_format_milp.

The `.aux` file is a plain-text file used to describe the structure of a bilevel optimization problem.
Specifically, it defines:
- The lower-level problem dimensions;
- Which variables belong to the lower-level problem;
- Which constraints belong to the lower-level problem;
- The lower-level objective coefficients;
- All variables and constraints not listed in the `.aux` file are implicitly part of the upper-level problem.

Without the `.aux` file, the model is interpreted as a standard single-level optimization problem.

\subsubsection sections Format of the `.aux` File

The `.aux` file is composed of tagged sections, each identified by a keyword starting with `@`. The following table lists all supported tags and their meaning.

| Tag             | Meanning                                                                                                      |
|-----------------|---------------------------------------------------------------------------------------------------------------|
| `@NUMVARS`      | The next line contains the number of lower-level variables                                                    |
| `@NUMCONSTRS`   | The next line contains the number of lower-level constraints                                                  |
| `@VARSBEGIN`    | Marks the beginning of the variables section                                                                  |
| `@VARSEND`      | Marks the end of the variables section                                                                        |
| `@CONSTRSBEGIN` | Marks the beginning of the constraints section                                                                |
| `@CONSTRSEND`   | Marks the end of the constraints section                                                                      |
| `@NAME`         | The next line contains the name of the instance (optional)                                                    |
| `@MPS`          | The next line contains the name of the `.mps` file with which this instance is associated (typically ignored) |
| `@LP`           | The next line contains the name of the `.lp` file with which this instance is associated (typically ignored)  |

The variables section is composed by the list of variable names that belong to the lower-level problem directly followed by their 
objective coefficient. The constraint section is composed by the list of constraint names that belong to the lower-level problem.

Note that bounds on lower-level variables are always assumed to be constraints of the lower-level problem. 

\section references References

- <a href="https://coin-or.github.io/MibS/input.html" target="_blank">MibS documentation</a>.
- <a href="https://bobilib.org/" target="_blank">BOBILib</a>, which is an instance library for mixed-integer bilevel optimization that uses this file format.