\page example_mixed_integer_tsp_gurobi tsp-gurobi
\brief Solves the traveling salesman problem using a callback for separating sub-tour elimination constraints

The traveling salesman problem consists of finding a minimum-cost tour that
visits each city exactly once and returns to the starting city.

Let \f( V = \{1,\dots,n\} \f) denote the set of cities, and let \f( c_{ij} \f) be
the cost of traveling directly from city \f( i \f) to city \f( j \f).

An Exponential-Size Model
=========================

Decision Variables
------------------

We introduce binary variables

\f[
x_{ij} =
\begin{cases}
1 & \text{if the tour goes directly from city } i \text{ to city } j, \\
0 & \text{otherwise},
\end{cases}
\qquad \text{for all } i,j \in V,\; i \neq j.
\f]

Objective Function
------------------

The objective is to minimize the total travel cost of the tour:

\f[
\min \sum_{i \in V} \sum_{j \in V \setminus \{i\}} c_{ij} x_{ij}.
\f]

Degree Constraints
------------------

To ensure that each city is entered and left exactly once, we impose the
following constraints:

\f[
\sum_{j \in V \setminus \{i\}} x_{ij} = 1 \quad \text{for all } i \in V,
\f]

\f[
\sum_{i \in V \setminus \{j\}} x_{ij} = 1 \quad \text{for all } j \in V.
\f]

These constraints guarantee that every city has exactly one outgoing arc and one
incoming arc. However, they still allow solutions consisting of multiple
disconnected cycles (sub-tours).

Sub-Tour Elimination Constraints
--------------------------------

To prevent sub-tours, we add the following family of constraints:

\f[
\sum_{i \in S} \sum_{j \in S \setminus \{i\}} x_{ij}
\le |S| - 1
\quad
\text{for all } S \subset V,\; 2 \le |S| \le n-1.
\f]

Each constraint enforces that no proper subset \f( S \f) of cities can form a
closed cycle. Together, these constraints ensure that the solution corresponds
to a single Hamiltonian tour.

Since the number of subsets \f( S \f) grows exponentially with \f( n \f), these
constraints are not added explicitly to the model. Instead, they are generated
dynamically during the solution process.

Integrality Constraints
-----------------------

Finally, we require:

\f[
x_{ij} \in \{0,1\} \quad \text{for all } i \neq j.
\f]

The Full Model
--------------

Finally, the full model is given by

\f[
\begin{aligned}
\min_x \quad
& \sum_{i \in V} \sum_{j \in V \setminus \{i\}} c_{ij} x_{ij} \\

    \text{s.t.}\quad
    & \sum_{j \in V \setminus \{i\}} x_{ij} = 1,
      \quad \text{for all } i \in V, \\

    & \sum_{i \in V \setminus \{j\}} x_{ij} = 1,
      \quad \text{for all } j \in V, \\

    & \sum_{i \in S} \sum_{j \in S \setminus \{i\}} x_{ij}
      \le |S| - 1,
      \quad \text{for all } S \subset V,\; 2 \le |S| \le n-1, \\

    & x_{ij} \in \{0,1\},
      \quad \text{for all } i \neq j.
    \end{aligned}
\f]

Implementation in idol
======================

This example demonstrates how to solve the TSP using **lazy constraints** in
Gurobi via *idol*. The model is initialized with the objective function and the
degree constraints only. During the branch-and-bound process, a callback is used
to inspect incumbent integer solutions. If a solution contains one or more
sub-tours, the corresponding sub-tour elimination constraints are generated and
added lazily.

\include ../examples/mixed-integer/tsp-lazy.example.cpp
