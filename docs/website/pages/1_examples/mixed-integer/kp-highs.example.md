\page example_mixed_integer_kp_highs kp-highs
\brief Models a simple binary knapsack problem and optimizes it with HiGHS. If the model is feasible, prints all solutions
    in the solution pool.

The knapsack problem instance is modeled as the mixed-integer optimization problem 

\f[
    \begin{align*}
        \min_{x} \quad & -40x_1 - 50x_2 - 100x_3 - 95x_4 - 30x_5 \\
        \text{s.t.} \quad & 2x_1 + 3.14x_2 + 1.98x_3 + 5x_4 + 3x_5 \le 10, \\
        & x_i\in \{ 0,1 \}, \quad \text{for all } i=1,\dotsc,5.
    \end{align*}
\f]

\include ../examples/mixed-integer/kp-highs.example.cpp
