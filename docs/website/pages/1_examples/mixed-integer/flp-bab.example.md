\page example_mixed_integer_flp_bab flp-bab
\brief Models a capacitated facility location problem and solves it with idol's branch-and-bound algorithm.
    Each node is solved by GLPK. Reduced cost fixing is used.

We consider the capacitated Facility Location Problem (FLP).
Given a set of potential facility locations \\(V_1\\) and a set of customers \\(V_2\\), the goal is to select
a subset of facility location to activate so as to serve the customers' demand. In doing so, costs should be minimized.

The data of this problem reads:
- Each facility \\(i\in V_1\\) has an opening cost \\(f_i\\) and a maximum capacity \\(q_i\\);
- Each customer \\(j\in V_2\\) has a demand \\(d_j\\);
- The unitary cost for serving customer \\(j\in V_2\\) from facility \\(i\in V_1\\) is \\(c_{ij}\\).

We model the capacitated FLP as the MILP

\f[
    \begin{aligned}
        \min_{x,y} \quad & \sum_{i\in V_1} f_ix_i + \sum_{i\in V_1} \sum_{j\in V_2} c_{ij} d_j y_{ij} \\
        \text{s.t.} \quad & \sum_{j\in V_2} d_jy_{ij} \le q_i x_i, \quad \text{for all } i\in V_1, \\
        & \sum_{i\in V_1} y_{ij} = 1, \quad \text{for all } j\in V_2, \\
        & x_i \in \{0,1\}, \quad \text{for all } i\in V_1, \\
        & y_{ij} \in \{0,1\}, \quad \text{for all } i\in V_1, j\in V_2.
    \end{aligned}
\f]

In the following code, we show how this problem can be solved using idol's branch-and-bound algorithm.

\include ../examples/mixed-integer/flp-bab.example.cpp
