\page cli_basics Basics
\brief Describes the basic usage of the `idol_cl` tool.

\tableofcontents

\section cli_what_is What is idol_cl? 

`idol_cl` is a command-line tool for solving mathematical optimization problems, including:
- Mixed-Integer Linear Programs (MILPs);
- Bilevel optimization problems;
- Robust and two-stage robust optimization problems.

It allows you to model, solve, and analyze optimization problems directly from the command line.

To install it, please refer to the \ref cli_installation page.

\section cli_first_milp Solving Your First MILP

To solve a given MILP stored in an `.lp` file, all you need to do is

```shell
idol_cl solve model.lp
```

If you want to set a time limit and use a specific method, run

```shell
idol_cl solve model.lp --time-limit 3600 --method GLK
```

To see which solution methods are available for a given problem, use the `list-methods` subcommand.

```shell
idol_cl list-methods model.lp
```

**Sampled Output**:
```text 
-- No configuration file loaded
-- The main input file is model.lp
-- Detected: problem type is MILP
-- Detected: applicable methods are [GUROBI, CPLEX, HIGHS, GLPK, JUMP]

Details (all methods):

 - HIGHS: High Performance Software for Linear Optimization [https://highs.dev/]
 - JUMP: Calls the Julia package JuMP [https://jump.dev/]
 - GLPK: GLPK (GNU Linear Programming Kit) [https://www.gnu.org/software/glpk/]
 - CPLEX: IBM ILOG CPLEX Optimization Studio [https://www.ibm.com/fr-fr/products/ilog-cplex-optimization-studio]
 - GUROBI: Gurobi Optimizer [https://www.gurobi.com/]
```

If no method is specified, idol_cl automatically selects a suitable solver
for the detected problem type.

\section cli_basics_bilevel Bilevel Problems

Bilevel problems are represented using a pair of files:
- an `.mps` file describing the main model,
- an `.aux` file describing the follower problem.

For more information, see the dedicated page on \ref cli_input_format_bilevel.

Given a bilevel problem, you can solve it by running 

```shell 
idol_cl solve model.mps --bilevel follower.aux
```

Some commands require more parameters than others. For instance, here is how to solve an LP-LP bilevel problem
using its KKT reformulation using big-M values. 

```shell
idol_cl solve model.mps --bilevel follower.aux --method KKT-BIGM --bound-provider bounds.txt
```

For more information, see our tutorial on \ref cli_bilevel_kkt.

Here again, to list the available methods for your problem class, you may use 

```shell
idol_cl list-methods model.mps --bilevel follower.aux
```

Finally, if the bilevel problem does not have coupling constraints, it is possible to address its pessimistic variant by 
appending the `--pessimistic` flag. If so, the relaxation-and-correction scheme from <a href="https://doi.org/10.1287/ijoc.2019.0927" target="_blank">Zeng (2025)</a>
will be applied.

```shell
idol_cl solve model.lp --bilevel follower.aux --method KKT-SOS1 --pessimistic
```

\section cli_basics_aro Two-Stage Robust Problems

<div class="warning">
    This section is still a work in progress.
</div>