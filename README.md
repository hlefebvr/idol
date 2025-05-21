# A C++ Framework for Optimization 

![License](https://img.shields.io/github/license/hlefebvr/idol?color=blue)
![GitHub tag (latest by date)](https://img.shields.io/github/v/release/hlefebvr/idol?color=blue)
![GitHub Workflow Status (branch)](https://github.com/hlefebvr/idol/actions/workflows/tests.yml/badge.svg)
![GitHub issues](https://img.shields.io/github/issues-raw/hlefebvr/idol)
![Repo status](https://www.repostatus.org/badges/latest/wip.svg)
[![codecov](https://codecov.io/github/hlefebvr/idol/branch/main/graph/badge.svg?token=BWMH5522QP)](https://app.codecov.io/gh/hlefebvr/idol)
[![GitHub sponsors](https://img.shields.io/github/sponsors/hlefebvr)](https://github.com/sponsors/hlefebvr)

## What is idol?
idol is a C++ framework for mathematical optimization and complex decision-making problems. It is designed to help you build new algorithms for solving complex optimization problems. The main philosophy behind idol is interoperability and ease of use. Hence, any algorithm can be seamlessly combined with any other algorithm to create a new one. For instance, you can combine a branch-and-bound algorithm with a column generation algorithm to create a branch-and-price algorithm.

```cpp
const auto branch_and_price = branch_and_bound + column_generation;
model.use(branch_and_price);
model.optimize();
```

With idol, you can also interface with your favorite solver:

- mixed-integer optimization (Gurobi, Cplex, Mosek, HiGHS, GLPK, coin-or/Osi and julia's framework JuMP);
- bilevel optimization (single-level reformulations, heuristics and coin-or/MibS);
- robust optimization (column and constraint generation and more);

## Documentation

Visit our [online documentation](https://hlefebvr.github.io/idol/).

## Using idol for Research?

If you are opting for idol in one of your research projects and encounter some issues, please contact me at lefebvre(at)uni-trier.de.
