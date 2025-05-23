\page example_mixed_integer_sos1_gurobi sos1-gurobi
\brief Models a simple continuous problem and adds an SOS1 constraint. Then, uses Gurobi to optimize it and print the solution.



\f[
    \begin{aligned}
        \min_{x} \quad & -2x_1 - x_2 - x_3 \\
        \text{s.t.} \quad & 0 \le x_j \le 1, \quad \text{for all } j=1,2,3, \\
        & \text{SOS1}(x_1,x_2,x_3).
    \end{aligned}
\f]

\include ../examples/mixed-integer/sos1-gurobi.example.cpp
