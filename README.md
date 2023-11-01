# idol - The Mathematical Optimization Framework

![License](https://img.shields.io/github/license/hlefebvr/idol?color=blue)
![GitHub tag (latest by date)](https://img.shields.io/github/v/release/hlefebvr/idol?color=blue)
![GitHub Workflow Status (branch)](https://github.com/hlefebvr/idol/actions/workflows/tests.yml/badge.svg)
![GitHub issues](https://img.shields.io/github/issues-raw/hlefebvr/idol)
![Repo status](https://www.repostatus.org/badges/latest/wip.svg)
[![codecov](https://codecov.io/github/hlefebvr/idol/branch/main/graph/badge.svg?token=BWMH5522QP)](https://app.codecov.io/gh/hlefebvr/idol)
[![GitHub sponsors](https://img.shields.io/github/sponsors/hlefebvr)](https://github.com/sponsors/hlefebvr)

Idol is a powerful and flexible library meticulously crafted for developing new mathematical optimization algorithms.
It is built to provide researchers with a versatile toolkit to construct, tweak, and experiment with state-of-the-art
methods. Whether you're exploring Branch-and-Price,
Benders decomposition, Column-and-Constraint generation for adjustable robust problems,
or any other cutting-edge method, idol is your trusted companion.

![Making Branch-and-Price implementation as easy as Branch-and-Bound + Column-Generation](https://raw.githubusercontent.com/hlefebvr/idol/refacto_column_generation/docs/branch-and-price-implementation.png)

Visit our [documentation](https://hlefebvr.github.io/idol/) and [benchmark](https://hlefebvr.github.io/idol_benchmark/GAP.render.html).

## Features

- Branch-and-Bound
  - Node selection rules: Best Bound, Worst Bound, Depth First, Best Estimate, Breadth First.
  - Branching rules (for variable branching): Pseudo Cost, Strong Branching (with phases), Most Infeasible, Least Infeasible, First Found, Uniformly Random.
  - Subtree exploration
  - Heuristics (for variable branching): Simple Rounding, Relaxed Enforced Neighborhood, Local Branching
  - Callbacks: User Cuts, Lazy Cuts
- Column Generation and Branch-and-Price
  - Automatic Dantzig-Wolfe reformulation
  - Soft and hard branching available (i.e, branching on master or sub-problem)
  - Stabilization by dual price smoothing: [Wentges (1997)](https://doi.org/10.1016/S0969-6016(97)00001-4), [Neame (2000)](https://scholar.google.com/scholar?&q=Neame%2C%20P.J.%3A%20Nonsmooth%20Dual%20Methods%20in%20Integer%20Programming.%20PhD%20thesis%20%281999%29).
  - Heuristics: Integer Master

## Benchmark 

- **Branch-and-Price** for [Generalized Assignment Problem](https://hlefebvr.github.io/idol-benchmark-gap/GAP.render.html).
- **Branch-and-Bound** for [Knapsack Problem](https://hlefebvr.github.io/idol-benchmark-kp/KP.render.html).

![Performance profile](https://raw.githubusercontent.com/hlefebvr/idol-benchmark-gap/gh-pages/profile.png)

This is a performance profile computed according to *Dolan, E., Moré, J. Benchmarking optimization software with performance profiles. Math. Program. 91, 201–213 (2002)* [https://doi.org/10.1007/s101070100263](https://doi.org/10.1007/s101070100263).

## Examples

Here is an example of a complex algorithm built using idol. It is a nested Branch-and-Price algorithm with strong branching.

```cpp
auto nested_branch_and_price =
        
    /* The overall algorithm is a branch-and-bound */
    BranchAndBound()
    
        /* Each node is solved with a Dantzig-Wolfe decomposition algorithm */
        .with_node_optimizer(
                
            /* The annotation "decomposition1" is used to automatically decompose the problem */
            DantzigWolfeDecomposition(decomposition1)
            
                /* The master problem is solved using Gurobi */
                .with_master_optimizer(Gurobi::ContinuousRelaxation())
                
                /* Each pricing problem is solved by a (nested) branch-and-bound algorithm */
                .with_pricing_optimizer(
                        
                    BranchAndBound()
                    
                        /* Each node is solved by a dantzig-wolfe decomposition algorithm */
                        .with_nodes_solver(
                                
                            /* The annotation "decomposition2" is used to decompose the sub-problem again */
                            DantzigWolfeDecomposition(decomposition2)
                            
                                /* The master problem is solved using Mosek */
                                .with_master_optimizer(Mosek::ContinuousRelaxation())
                                
                                /* The sub-problem is solved by a (nested) branch-and-bound algorithm [we could have used, e.g., Gurobi instead] */
                                .with_pricing_optimizer(
                                        
                                    BranchAndBound()
                                    
                                        /* Each node is solved by GLPK */
                                        .with_node_optimizer(GLPK::ContinuousRelaxation())
                                        
                                        /* Variables are selected for branching using the most-infeasible rule */
                                        .with_branching_rule(MostInfeasible())
                                        
                                        /* Nodes are selected using the worst-bound rule */
                                        .with_node_selection_rule(WorstBound())
                                        
                                )
                                
                                /* Branching constraints are applied to the pricing problem */
                                .with_branching_on_master(false)
                        )
                        /* Variables are selected for branching using the most-infeasible rule */
                        .with_branching_rule(MostInfeasible())
                        
                        /* Nodes are selected using the depth-first rule */
                        .with_node_selection_rule(DepthFirst())
                )
                
                /* Column generation is stabilized by dual-price smoothing */
                .with_dual_price_smoothing_stabilization(.3)
                
                /* Infeasible master problem are dealt with using Farkas pricing */
                .with_farkas_pricing()
                
                /* Branching constraints are applied to the master problem */
                .with_branching_on_master()
        )
        /* Variables are selected for branching using the strong-branching rule */
        .with_branching_rule(StrongBranching())
        
        /* Nodes are selected using the best-bound rule */
        .with_node_selection_rule(BestBound())
        
        /* Only informational logs will be printed (in blue) */
        .with_log_level(Info, Blue)
        
        /* The algorithm will run with a time limit of 3600 */
        .with_time_limit(3600)
    );
    
model.use(nested_branch_and_price);

model.optimize();
```

## Miscellaneous

Versionning is compliant with [Semantic versionning 2.0.0](https://semver.org/).
