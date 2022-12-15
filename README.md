# Idol

![GitHub](https://img.shields.io/github/license/hlefebvr/idol)
![GitHub tag (latest by date)](https://img.shields.io/github/v/tag/hlefebvr/idol?label=version)
![GitHub Workflow Status (branch)](https://img.shields.io/github/workflow/status/hlefebvr/idol/CMake/main)
![GitHub issues](https://img.shields.io/github/issues-raw/hlefebvr/idol)

Idol is a C++ library for implementing complex decomposition algorithms for
mathematical optimization.

Its main goal is to facilitate prototyping new algorithms which
can exploit structure in optimization problems (e.g., Dantzig-Wolfe decomposition, Benders decomposition,
nested approaches, ...) or to design complex algorithmic schemes for solving $\Sigma_i^P$-hard problems
(e.g., bilevel problems, adjustable robust optimization, ...).

Visit our [online documentation](https://hlefebvr.github.io/idol/).

## Features

**IMPORTANT: This library is under active development and has no stable feature (nor interface) for the moment.
Use it with care and with precise release reference only.**

### Branch-and-bound

- Agnostic on solution method (e.g., LP, Dantzig-Wolfe, ...) ;
- Agnostic on node type and on branching type, yet the following are ready to be used:
  - Branching on variables by bound ;
- Agnostic on branching strategy, yet the following are ready to be used:
  - Most infeasible branching ;
  - *TODO Pseudo-cost branching* ;
  - *TODO Strong branching* ;
  - *TODO Reliability branching* ;
- Agnostic on node selection strategy, yet the following are ready to be used:
    - Depth first ;
    - Breadth first ;
    - Best bound first ;
    - Worst bound first ;
    - Hybrid strategy ;

### Dantzig-Wolfe decomposition
  - Automatic reformulation ;
  - Branching applied to master problem (default) or pricing problem ;
  - Restoring feasible columns from pool after branching ;
  - Cleaning up pool after user-defined threshold ;
  - Stabilization via smoothing of dual values ;
  - Dealing with infeasible initial master methods:
    - Artificial variables (default) ;
    - Farkas pricing ;

### Benders decomposition

*TODO*

### Cut generation

*TODO*