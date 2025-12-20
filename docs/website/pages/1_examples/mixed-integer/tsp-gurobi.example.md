\page example_mixed_integer_tsp_gurobi tsp-gurobi
\brief Solves the traveling salesman problem using a callback for separating sub-tour elimination constraints

Sub-tour Elimination Formulation
================================

Let \\(V = \\{1,\dots,n\\}\\) and let \\(c_{ij}\\) be the travel cost.

Decision variables:
\\(x_{ij} \in \\{0,1\\}\\) for all \\(i \neq j\\).

Model:
\f[
    \begin{aligned}
        \min_x \quad
        & \sum_{i \in V} \sum_{j \in V \setminus \{i\}} c_{ij} x_{ij} \\
        
        \text{s.t.}\quad
        & \sum_{j \in V \setminus \{i\}} x_{ij} = 1
        && \forall i \in V \\
        
        & \sum_{i \in V \setminus \{j\}} x_{ij} = 1
        && \forall j \in V \\
        
        & \sum_{i \in S} \sum_{j \in S \setminus \{i\}} x_{ij}
        \le |S| - 1
        && \forall S \subset V,\; 2 \le |S| \le n-1 \\
        
        & x_{ij} \in \{0,1\}
        && \forall i \neq j
    \end{aligned}
\f]

\\(S\\) is any proper subset of cities.


Implementation in idol
======================

\include ../examples/mixed-integer/tsp-lazy.example.cpp
