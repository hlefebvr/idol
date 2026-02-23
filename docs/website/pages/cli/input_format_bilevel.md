\page cli_input_format_bilevel Input Format for Bilevel Problems
\brief Describes the input file format for bilevel problems.

\tableofcontents

For bilevel problems, `idol_cl` uses the `.aux` file format from the <a href="https://github.com/coin-or/MibS" target="_blank">MibS solver</a>
coupled with an `.lp`/`.mps` file that stores the single-level relaxation.

For more details, you may also refer to the <a href="https://coin-or.github.io/MibS/input.html" target="_blank">MibS documentation</a>.
This file format is also the one in use by the <a href="https://bobilib.org/" target="_blank">BOBILib</a> instance library. 

\subsection example Example

Consider the bilevel optimization problem

\f[
    \begin{align*}
        \min_{x, y} \quad & -x - 7y \\
        \text{s.t.} \quad
        & -3x + 2y \le 12, \\
        & x + 2y \le 20, \\
        & x \le 10, \\
        & y \in \begin{array}[t]{rl}
            \displaystyle\arg\min_{z} \ & z \\
            \text{s.t.} \ & 2x - z \le 7, \\
            & -2x + 4z \le 16, \\
            & z \le 5.
        \end{array}
    \end{align*}
\f]

The `.mps` stores the single-level relaxation.

```
NAME generalExample
ROWS
 L  R0
 L  R1
 L  R2
 L  R3
 N  R4
COLUMNS
    INT1 'MARKER' 'INTORG'
    C0   R0       -3
    C0   R1       1
    C0   R2       2
    C0   R3       -2
    C0   R4       -1
    C1   R0       2
    C1   R1       2
    C1   R2       -1
    C1   R3       4
    C1   R4       -7
    INT1END 'MARKER' 'INTEND'
RHS
    B    R0       12
    B    R1       20
    B    R2       7
    B    R3       16
BOUNDS
 UP BOUND C0      10
 UP BOUND C1      5
ENDATA
```

The `.aux` file describes what variables and constraints belong to the lower level and defines the lower-level objective coefficients.

```
@NUMVARS
1
@NUMCONSTRS
2
@VARSBEGIN
C1 1.
@VARSEND
@CONSTRSBEGIN
R2
R3
@CONSTRSEND
@NAME
General Example
@MPS
generalExample.mps
```

**Note**: in `idol_cl`, the field `@MPS` is typically ignored to avoid path corruption. 
For that reason, the `.mps` file must explicitly be given.

\subsection details Detailed Description