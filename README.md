# idol - The Mathematical Optimization Framework

![License](https://img.shields.io/github/license/hlefebvr/idol?color=blue)
![GitHub tag (latest by date)](https://img.shields.io/github/v/release/hlefebvr/idol?color=blue)
![GitHub Workflow Status (branch)](https://github.com/hlefebvr/idol/actions/workflows/tests.yml/badge.svg)
![GitHub issues](https://img.shields.io/github/issues-raw/hlefebvr/idol)
![Repo status](https://www.repostatus.org/badges/latest/wip.svg)
[![codecov](https://codecov.io/github/hlefebvr/idol/branch/main/graph/badge.svg?token=BWMH5522QP)](https://app.codecov.io/gh/hlefebvr/idol)
[![GitHub sponsors](https://img.shields.io/github/sponsors/hlefebvr)](https://github.com/sponsors/hlefebvr)

![Making Branch-and-Price implementation as easy as Branch-and-Bound + Column-Generation](https://raw.githubusercontent.com/hlefebvr/idol/refacto_column_generation/docs/branch-and-price-implementation.png)

Idol is a powerful and flexible library meticulously crafted for developing new mathematical optimization algorithms.
It is built to provide researchers with a versatile toolkit to construct, tweak, and experiment with state-of-the-art
methods. Whether you're exploring Branch-and-Price,
Benders decomposition, Column-and-Constraint generation for adjustable robust problems,
or any other cutting-edge method, idol is your trusted companion.

- [Documentation](#Documentation)
- [Examples](#Examples)
- [Implemented features](#Implemented-Features)
  - [Branch-and-Bound](#Branch-and-Bound)
  - [Column Generation and Branch-and-Price](#Column-Generation-and-Branch-and-Price)

## Documentation

Visit our [online documentation](https://hlefebvr.github.io/idol/).

## Examples

Look at how easy it is to implement a Branch-and-Price algorithm using idol.

```cpp
const auto [model, decomposition] = create_model(); // Creates the model with an annotation for automatic decomposition

const auto sub_problem_specifications = 
        DantzigWolfe::SubProblem()
          .add_optimizer(Gurobi()); // Each sub-problem will be solved by Gurobi

const auto column_generation = 
        DantzigWolfeDecomposition(decomposition)
          .with_master_optimizer(Gurobi::ContinuousRelaxation()) // The master problem will be solved by Gurobi
          .with_default_sub_problem_spec(sub_problem_specifications);

const auto branch_and_bound =
        BranchAndBound()
          .with_node_selection_rule(BestBound()) // Nodes will be selected by the "best-bound" rule
          .with_branching_rule(MostInfeasible()) // Variables will be selected by the "most-fractional" rule
          .with_log_level(Info, Blue);

const auto branch_and_price = branch_and_bound + column_generation; // Embed the column generation in the Branch-and-Bound algorithm

model.use(branch_and_price);

model.optimize();
```

## Implemented Features

### Branch-and-Bound

- Node selection rules: Best Bound, Worst Bound, Depth First, Best Estimate, Breadth First.
- Branching rules (for variable branching): Pseudo Cost, Strong Branching (with phases), Most Infeasible, Least Infeasible, First Found, Uniformly Random.
- Subtree exploration
- Heuristics (for variable branching): Simple Rounding, Relaxed Enforced Neighborhood, Local Branching
- Callbacks: User Cuts, Lazy Cuts

### Column Generation and Branch-and-Price

- Automatic Dantzig-Wolfe reformulation
- Soft and hard branching available (i.e, branching on master or sub-problem)
- Stabilization by dual price smoothing: [Wentges (1997)](https://doi.org/10.1016/S0969-6016(97)00001-4), [Neame (2000)](https://scholar.google.com/scholar?&q=Neame%2C%20P.J.%3A%20Nonsmooth%20Dual%20Methods%20in%20Integer%20Programming.%20PhD%20thesis%20%281999%29)
- Can solve sub-problems in parallel
- Supports pricing heuristics
- Heuristics: Integer Master

## Benchmark 

- A benchmark for the **Branch-and-Price** implementation is available for the [Generalized Assignment Problem](https://hlefebvr.github.io/idol-benchmark-gap/GAP.render.html).
- A benchmark for the **Branch-and-Bound** implementation is available for the [Knapsack Problem](https://hlefebvr.github.io/idol-benchmark-kp/KP.render.html).

![Performance profile](https://raw.githubusercontent.com/hlefebvr/idol-benchmark-gap/gh-pages/profile.png)

This is a performance profile computed according to *Dolan, E., Moré, J. Benchmarking optimization software with performance profiles. Math. Program. 91, 201–213 (2002)* [https://doi.org/10.1007/s101070100263](https://doi.org/10.1007/s101070100263).

## Miscellaneous

Versionning is compliant with [Semantic versionning 2.0.0](https://semver.org/).
