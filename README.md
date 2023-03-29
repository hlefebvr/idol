# Idol

![GitHub](https://img.shields.io/github/license/hlefebvr/idol)
![GitHub tag (latest by date)](https://img.shields.io/github/v/tag/hlefebvr/idol?label=version)
![GitHub Workflow Status (branch)](https://img.shields.io/github/actions/workflow/status/hlefebvr/idol/cmake.yml?branch=main)
![GitHub issues](https://img.shields.io/github/issues-raw/hlefebvr/idol)
![Repo status](https://www.repostatus.org/badges/latest/wip.svg)
[![codecov](https://codecov.io/github/hlefebvr/idol/branch/main/graph/badge.svg?token=BWMH5522QP)](https://app.codecov.io/gh/hlefebvr/idol)

Idol is a C++ library for implementing complex decomposition algorithms for
mathematical optimization.

Visit our [online documentation](https://hlefebvr.github.io/idol/) and [online benchmark](https://hlefebvr.github.io/idol_benchmark/GAP.render.html).

## Examples

Here is an example of a complex algorithm built using idol. It is a nested Branch-and-Price algorithm.

```cpp
auto nested_branch_and_price =
        
    /* The overall algorithm is a branch-and-bound */
    BranchAndBound()
    
        /* Each node is solved with a Dantzig-Wolfe decomposition algorithm */
        .with_node_solver(
                
            /* The annotation "decomposition1" is used to automatically decompose the problem */
            DantzigWolfeDecomposition(decomposition1)
            
                /* The master problem is solved using Gurobi */
                .with_master_solver(Gurobi::ContinuousRelaxation())
                
                /* Each pricing problem is solved by a (nested) branch-and-bound algorithm */
                .with_pricing_solver(
                        
                    BranchAndBound()
                    
                        /* Each node is solved by a dantzig-wolfe decomposition algorithm */
                        .with_nodes_solver(
                                
                            /* The annotation "decomposition2" is used to decompose the sub-problem again */
                            DantzigWolfeDecomposition(decomposition2)
                            
                                /* The master problem is solved using Mosek */
                                .with_master_solver(Mosek::ContinuousRelaxation())
                                
                                /* The sub-problem is solved by a (nested) branch-and-bound algorithm [we could have used, e.g., Gurobi instead] */
                                .with_pricing_solver(
                                        
                                    BranchAndBound()
                                    
                                        /* Each node is solved by GLPK */
                                        .with_node_solver(GLPK::ContinuousRelaxation())
                                        
                                        /* Variables are selected for branching using the most-infeasible rule */
                                        .with_branching_rule(MostInfeasible())
                                        
                                        /* Nodes are selected using the worst-bound rule */
                                        .with_node_selection_rule(WorstBound())
                                        
                                )
                                
                                /* Branching constraints are applied to the pricing problem */
                                .with_branching_on_pricing()
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
        .with_branching_rule(MostInfeasible())
        
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
