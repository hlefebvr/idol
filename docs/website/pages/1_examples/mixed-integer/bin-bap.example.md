\page example_mixed_integer_bin_bap bin-bap
\brief Solves the bin packing problem with idol's branch-and-price algorithm and extracts the active columns in the best-known solution.

Given a set of \\(n\\) items and a set of identical bins, the goal is to pack each item into the smallest number of bins possible.

The data for this problem is as follows:

- Each bin has capacity \\(C\\);
- Each item \\(j\\) with \\(j = 1, …, n\\) has size \\(s_j\\).

Natural Formulation
===================

We model the Bin Packing Problem with the following binary linear program.
Let \\(m\\) be an upper bound on the number of bins (e.g., \\(m = n\\)).

\f[

    \begin{align*}
        \min_{x,y} \quad & \sum_{i=1}^m y_i \\
        \text{s.t.} \quad 
        & \sum_{j=1}^n s_j x_{ij} \le C,
          \quad \text{for all } i=1,\dotsc,m, \\
        & \sum_{i=1}^m x_{ij} = 1,
          \quad \text{for all } j=1,\dotsc,n, \\
        & x_{ij} \le y_i,
          \quad \text{for all } i=1,\dotsc,m,\; j=1,\dotsc,n, \\
        & x_{ij} \in \{0,1\},
          \quad \text{for all } i=1,\dotsc,m,\; j=1,\dotsc,n, \\
        & y_i \in \{0,1\},
          \quad \text{for all } i=1,\dotsc,m.
    \end{align*}

\f]

Here, variable \\(x_{ij}\\) equals 1 if and only if item \\(j\\) is packed into bin \\(i\\), while variable \\(y_i\\) equals 1
if and only if bin \\(i\\) is used. The objective minimizes the total number of bins used while ensuring that all items
are packed and no bin exceeds its capacity.

Dantzig-Wolfe Reformulation
===========================

In this example, we decompose the problem by bin. 

Implementation in idol
======================

Here, we specifically show how to retrieve the active nodes in the best solution found. 
To achieve this, all you need to do is to use a column-generation-specific node class that will save this information for you.

\include ../examples/mixed-integer/bin-bap.example.cpp
