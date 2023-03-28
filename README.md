# Idol

![GitHub](https://img.shields.io/github/license/hlefebvr/idol)
![GitHub tag (latest by date)](https://img.shields.io/github/v/tag/hlefebvr/idol?label=version)
![GitHub Workflow Status (branch)](https://img.shields.io/github/actions/workflow/status/hlefebvr/idol/cmake.yml?branch=main)
![GitHub issues](https://img.shields.io/github/issues-raw/hlefebvr/idol)
![Repo status](https://www.repostatus.org/badges/latest/wip.svg)
![Coverage](https://img.shields.io/codecov/c/github/hlefebvr/idol/branch_and_bound?token=BWMH5522QP)

Idol is a C++ library for implementing complex decomposition algorithms for
mathematical optimization.

Visit our [online documentation](https://hlefebvr.github.io/idol/) and [online benchmark](https://hlefebvr.github.io/idol_benchmark/GAP.render.html).

## Examples

Here is an example of a complex algorithm built using idol. It is a nested Branch-and-Price algorithm.

```cpp
auto nested_branch_and_price = 
            
    /* The overall algorithm is a branch-and-bound */
    BranchAndBoundOptimizer(
        
        /* Each node is solved with a Dantzig-Wolfe decomposition algorithm */
        DantzigWolfeOptimizer(
    
                /* This annotation is used to automatically decompose the problem */
                decomposition1,
    
                /* The master problem is solved using Gurobi */
                GurobiOptimizer::ContinuousRelaxation(), 
                
                /* Each sub-problem is solved by a (nested) branch-and-bound algorithm */
                BranchAndBoundOptimizer(
                        
                        /* Each node is solved by a dantzig-wolfe decomposition algorithm */
                        DantzigWolfeOptimizer(
                                
                                /* This annotation is used to decompose the sub-problem again */
                                decomposition2,
                                
                                /* The master problem is solved using Gurobi */
                                GurobiOptimizer::ContinuousRelaxation(),
    
                                /* The sub-problem is solved by a (nested) branch-and-bound algorithm [we could have used GurobiOptimizer instead] */
                                BranchAndBoundOptimizer(
                                        
                                        /* Each node is solved by GLPK */
                                        GLPKOptimizer::ContinuousRelaxation(),
    
                                        /* Variables are selected for branching using the most-infeasible rule */
                                        MostInfeasible(),
                                        
                                        /* Nodes are selected using the worst-bound rule */
                                        WorstBound()
                                )
    
                        ),
                       
                       /* Variables are selected for branching using the most-infeasible rule */
                       MostInfeasible(),
                       
                       /* Nodes are selected using the depth-first rule */
                       DepthFirst()
                )
        ).with_log_level(Info, Magenta), /* Here, we set some logging parameters */
    
        /* Variables are selected for branching using the most-infeasible rule */
        MostInfeasible(),
    
        /* Nodes are selected using the best-bound rule */
        BestBound()
        
    ).with_log_level(Info, Blue); /* Here, we set some logging parameters */
    
model.use(nested_branch_and_price);

model.optimize();
```