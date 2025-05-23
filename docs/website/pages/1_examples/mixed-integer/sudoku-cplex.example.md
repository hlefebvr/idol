\page example_mixed_integer_sudoku_cplex sudoku-cplex
\brief Models and solves a sudoku using Cplex. Shows how to manipulate multi-dimensional variables.

This example solves a sudoku puzzle using Cplex. It demonstrates how to manipulate multi-dimensional variables in Cplex.
Indeed, we use the following three-dimensional variable to model our problem:

\f[
    x_{ijk} = \begin{cases}
    1, & \text{if number $k$ is entered in row $i$ and column $j$}, \\
    0, & \text{otherwise}.
    \end{cases}
\f]

The rules of sudoku are as follows:

- Each cell must contain exactly one number:
    \f[
        \sum_{k=1}^9 x_{ijk} = 1 \quad \text{for all } i=1,\dots,9,\, j=1,\dots,9.
    \f]

- Each row must contain the numbers 1 to 9, exactly once:
    \f[
        \sum_{j=1}^9 x_{ijk} = 1 \quad \text{for all } i=1,\dots,9,\, k=1,\dots,9.
    \f]

- Each column must contain the numbers 1 to 9, exactly once:
    \f[
        \sum_{i=1}^9 x_{ijk} = 1 \quad \text{for all } j=1,\dots,9,\, k=1,\dots,9.
    \f]

- Each 3x3 block must contain the numbers 1 to 9, exactly once:
    \f[
        \sum_{i=3p-2}^{3p}\ \sum_{j=3q-2}^{3q} x_{ijk} = 1 \quad \text{for all }  p=1,2,3,\, q=1,2,3,\, k=1,\dots,9
    \f]

\include ../examples/mixed-integer/sudoku-cplex.example.cpp
