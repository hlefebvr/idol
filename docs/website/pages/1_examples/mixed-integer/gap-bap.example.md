\page example_mixed_integer_gap_bap gap-bap
\brief Solves the generalized assignment problem with idol's branch-and-price algorithm on its Dantzig-Wolfe reformulation.

Given a set of \\(m\\) machines and \\(n\\) jobs, the goal is to assign each job to exactly one machine in such a
way that the total cost is minimized, while respecting the capacity constraints of each machine.

The data for this problem is as follows:
- Each machine \\(i\\) with \\(i=1,\dotsc,m\\) has a capacity \\(C_i\\);
- Each job \\(j\\) with \\(j=1,\dotsc,n\\) consumes \\(r_{ij}\\) resources if scheduled on machine \\(i\\);
- Each job \\(j\\) has a cost of \\(c_{ij}\\) if scheduled on machine \\(i\\).

We model the GAP with the following binary linear program:

\f[
    \begin{align*}
        \min_{x} \quad & \sum_{i=1}^m \sum_{j=1}^n c_{ij} x_{ij} \\
        \text{s.t.} \quad & \sum_{j=1}^n r_{ij} x_{ij} \le C_i, \quad \text{for all } i=1,\dotsc,m, \\
                    & \sum_{i=1}^m x_{ij} = 1, \quad \text{for all } j=1,\dotsc,n, \\
                    & x_{ij} \in \{0,1\}, \quad \text{for all } i=1,\dotsc,m, j=1,\dotsc,n.
    \end{align*}
\f]

Here, variable \\(x_{ij}\\) encodes the assignment decision and equals 1 if and only if job \\(j\\) is assigned to
machine \\(i\\).

We perform a Dantzig-Wolfe reformulation of the problem by enumerating, for each machine \\(i\\), the set of feasible
assignments \\(\mathcal{F}_i\\), i.e., we let

\f[
    \mathcal{F}_i = \left\{ a \in \{0,1\}^n : \sum_{j=1}^n r_{ij} a_{j} \le C_i \right\}.
\f]

The Dantzig-Wolfe reformulation of the GAP is then given by 

\f[
    \begin{align*}
        \min_{y} \quad & \sum_{i=1}^m \sum_{j=1}^n c_{ij} y_{ij} \\
        \text{s.t.} \quad & \sum_{a\in \mathcal{F}_i} a_j\lambda_{a} = 1, \quad \text{for all } i=1,\dotsc,m, \\
                    & \sum_{a\in\mathcal{F}_i} \lambda_{a} = 1,  \quad \text{for all } i=1,\dotsc,m \\
                    & \lambda_{a} \in \{ 0,1 \}, \quad \text{for all } a\in\mathcal{F}_i, \\
    \end{align*}
\f]

The data for this example is taken from [this book (Example 7.3)](http://www.or.deis.unibo.it/kp/Chapter7.pdf).

\include ../examples/mixed-integer/gap-bap.example.cpp
