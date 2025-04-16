# idol, A C++ Framework for Optimization 

![License](https://img.shields.io/github/license/hlefebvr/idol?color=blue)
![GitHub tag (latest by date)](https://img.shields.io/github/v/release/hlefebvr/idol?color=blue)
![GitHub Workflow Status (branch)](https://github.com/hlefebvr/idol/actions/workflows/tests.yml/badge.svg)
![GitHub issues](https://img.shields.io/github/issues-raw/hlefebvr/idol)
![Repo status](https://www.repostatus.org/badges/latest/wip.svg)
[![codecov](https://codecov.io/github/hlefebvr/idol/branch/main/graph/badge.svg?token=BWMH5522QP)](https://app.codecov.io/gh/hlefebvr/idol)
[![GitHub sponsors](https://img.shields.io/github/sponsors/hlefebvr)](https://github.com/sponsors/hlefebvr)

![Making Branch-and-Price implementation as easy as Branch-and-Bound + Column-Generation](https://raw.githubusercontent.com/hlefebvr/idol/main/docs/website/branch-and-price-implementation.png)

idol is a C++ framework for mathematical optimization and complex decision-making problems. 

It is designed to help you build new algorithms for solving complex optimization problems. The main philosophy behind idol is interoperability and ease of use. Hence, any algorithm can be seamlessly combined with any other algorithm to create a new one. For instance, you can combine a Branch-and-Bound algorithm with a Column Generation algorithm to create a Branch-and-Price algorithm.

- [Documentation](#Documentation)
- [Using idol for Research?](#using-idol-for-research)
- [Examples](#Examples)
  - [Branch-and-Price](#branch-and-price)
  - [Bilevel Optimization (using coin-or/Mibs)](#bilevel-optimization-using-mibs)

## Documentation

Visit our [online documentation](https://hlefebvr.github.io/idol/).

## Using idol for Research?

If you are opting for idol in one of your research projects and encounter some issues, please contact me at lefebvre(at)uni-trier.de.

## Examples

### Branch-and-Price

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

### Bilevel Optimization (using MibS)

Here, idol uses the external solver [coin-or/MibS](https://github.com/coin-or/MibS) to solve a bilevel optimization problem with integer lower level.

```cpp

/*
 This example is taken from "The Mixed Integer Linear Bilevel Programming Problem" (Moore and Bard, 1990).

    min -1 x + -10 y
    s.t.

    y in argmin { y :
        -25 x + 20 y <= 30,
        1 x + 2 y <= 10,
        2 x + -1 y <= 15,
        2 x + 10 y >= 15,
        y >= 0 and integer.
    }
    x >= 0 and integer.

 */

Env env;

// Define High Point Relaxation
Model high_point_relaxation(env);

auto x = high_point_relaxation.add_var(0, Inf, Integer, "x");
auto y = high_point_relaxation.add_var(0, Inf, Integer, "y");

high_point_relaxation.set_obj_expr(-x - 10 * y);
auto follower_c1 = high_point_relaxation.add_ctr(-25 * x + 20 * y <= 30);
auto follower_c2 = high_point_relaxation.add_ctr(x + 2 * y <= 10);
auto follower_c3 = high_point_relaxation.add_ctr(2 * x - y <= 15);
auto follower_c4 = high_point_relaxation.add_ctr(2 * x + 10 * y >= 15);

// Prepare bilevel description
Bilevel::Description description(env);
description.set_lower_level_obj(y);
description.set_follower_var(y);
description.set_follower_ctr(follower_c1);
description.set_follower_ctr(follower_c2);
description.set_follower_ctr(follower_c3);
description.set_follower_ctr(follower_c4);

// Use coin-or/MibS as external solver
high_point_relaxation.use(Bilevel::MibS(description));

// Optimize and print solution
high_point_relaxation.optimize();

std::cout << high_point_relaxation.get_status() << std::endl;
std::cout << high_point_relaxation.get_reason() << std::endl;
std::cout << save_primal(high_point_relaxation) << std::endl;
```

Idol can also be interfaced with [ROOT](https://root.cern/) to monitor the progress of your algorithm.
For instance, here is a screenshot of the monitoring of MibS for a bilevel instance.

![Monitor Any Branch-and-Bound Method](https://raw.githubusercontent.com/hlefebvr/idol/main/docs/monitor-mibs.png)
