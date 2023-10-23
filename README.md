# idol

![GitHub](https://img.shields.io/github/license/hlefebvr/idol)
![GitHub tag (latest by date)](https://img.shields.io/github/v/tag/hlefebvr/idol?label=version)
![GitHub Workflow Status (branch)](https://img.shields.io/github/actions/workflow/status/hlefebvr/idol/tests.yml?branch=main)
![GitHub issues](https://img.shields.io/github/issues-raw/hlefebvr/idol)
![Repo status](https://www.repostatus.org/badges/latest/wip.svg)
[![codecov](https://codecov.io/github/hlefebvr/idol/branch/main/graph/badge.svg?token=BWMH5522QP)](https://app.codecov.io/gh/hlefebvr/idol)

Idol is a powerful and flexible library meticulously crafted for developing new mathematical optimization algorithms.
It is built to provide researchers with a versatile toolkit to construct, tweak, and experiment with state-of-the-art
methods. Whether you're exploring Branch-and-Price,
Benders decomposition, Column-and-Constraint generation for adjustable robust problems,
or any other cutting-edge method, idol is your trusted companion.

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
  - Stabilization by dual price smoothing
  - Heuristics: Integer Master

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
                                        .with_branching_rule(MostFractional())
                                        
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
        /* Variables are selected for branching using the most-infeasible rule */
        .with_branching_rule(StrongBranching())
        
        /* Nodes are selected using the best-bound rule */
        .with_node_selection_rule(BestBound()
        
        /* Only informational logs will be printed (in blue) */
        .with_log_level(Info, Blue)
        
        /* The algorithm will run with a time limit of 3600 */
        .with_time_limit(3600)
    )
    
model.use(nested_branch_and_price);

model.optimize();
```

## Miscellaneous

Versionning is compliant with [Semantic versionning 2.0.0](https://semver.org/).
