\page lib_milp_branch_and_bound Creating a Branch-and-Bound Algorithm
\brief Describes how to create a branch-and-bound algorithm

\tableofcontents

The branch-and-bound algorithm is a very well-known approach to solve combinatorial problems. 
It is a divide and conquer type of algorithms. For an introduction, please refer to the <a href="https://en.wikipedia.org/wiki/Branch_and_bound">Wikipedia page</a>
or to the book <a href="https://doi.org/10.1007/978-3-319-11008-0">Integer Programming</a>.

\warning The rest of this tutorial assumes a basic understanding of branch-and-bound procedures.

\section lib_milp_bnb_basics Basics

Writing a branch-and-bound algorithm with `idol` can be done using the `BranchAndBound<NodeTypeT>` class.
This is a template class where the template argument, `NodeTypeT`, is the class that stores the information of each node
in the tree. If none is specified, the default one is picked, i.e., `DefaultNodeInfo`. This class is more than enough for 
classic LP-based branch-and-bounds.

Then, at least three ingredients need to be provided in order to set up the algorithm:

- An optimizer, for solving each node;
- A node selection rule, for choosing which node to be solved next;
- A branching rule, for choosing how to branch on a node whose solution is not feasible for the original problem.

> Clearly, the `BranchAndBound` class can be used to create any sort of branch-and-bound algorithm. To streamline the discussion, however, we will focus on 
> a branch-and-bound algorithm for solving an MILP, based on the LP relaxation. 

To start with, we give a minimal example for constructing such a branch-and-bound method. 
Assume to have an object called `model` of the class `Model`, storing your MILP. 
Here is how to define a branch-and-bound method to solve it.

```cpp
// Create the algorithm
auto branch_and_bound = BranchAndBound();

// Set the optimizer used to solve the nodes
branch_and_bound.with_node_optimizer(Gurobi::ContinuousRelaxation());

// Set the node selection rule
branch_and_bound.with_node_selection_rule(BestBound());

// Set the branching rule
branch_and_bound.with_branching_rule(MostInfeasible());

// Use the optimizer
model.use(branch_and_bound);

// Solve
model.optimize();
```

Pretty easy no? Beware, however, that we are using here the `Gurobi::ContinuousRelaxation()` optimizer, and not `Gurobi()`. 
This is because `Gurobi()` would not relax the integrality requirements by default and solve the node's problem directly as a MILP.
Here, we really want to solve each node by means of the continuous relaxation of the problem at that node.

\section lib_milp_bnb_sub_pages More Advanced Topics

In the following sub-pages, we give a more in-depth description for these basic ingredients as well as more advanced features
like callbacks, presolve, cutting planes, etc.

- \subpage lib_milp_bnb_node_selection
- \subpage lib_milp_bnb_branching_rule
- \subpage lib_milp_bnb_callbacks (TODO)
- \subpage lib_milp_bnb_presolve
- \subpage lib_milp_bnb_cutting_planes (TODO)
- \subpage lib_milp_bnb_heuristics (TODO)
- \subpage lib_milp_bnb_node_type (TODO)
- \subpage lib_milp_bnb_options (TODO)

\page lib_milp_bnb_node_selection Node Selection Rules

\tableofcontents

Node selection rules are responsible for picking the next node to be explored within the list of active nodes.
Different strategies are possible, and can be configured with the help of the `with_node_selection_rule` method.

Here is a quick sumary.

| Rule              | Purpose / Guideline                                                              |
|-------------------|----------------------------------------------------------------------------------|
| **Best Bound**    | Pick node with best bound; fast convergence.                                     |
| **Best Estimate** | Balance objective and feasibility; favors near-feasible nodes.                   |
| **Breadth First** | Explore level by level; uniform search.                                          |
| **Depth First**   | Explore deep branches first; memory-efficient, finds feasible solutions quickly. |
| **Worst Bound**   | Pick worst bound; mainly for testing.                                            |

\section lib_milp_bnb_node_selection_BestBound Best Bound

The best bound strategy selects the node with the best (i.e., the lowest) bound among all active nodes.

Here is how to use it.

```cpp
branch_and_bound.with_node_selection_rule(BestBound());
```

\section lib_milp_bnb_node_selection_BestEstimate Best Estimate

The best estimate strategy selects the node that is expected to lead to the best final solution by combining objective value and degree of infeasibility.

To do so, a score is computed for each node and the one with the lowest score is selected.
The score is computed as follows.

\f[
    \begin{equation}
        \text{score} = \text{objective\_value} + \frac{
            (\text{root\_node\_objective} - \text{incumbent\_objective}) \times \text{sum\_of\_infeasibilities}
        }{
            \text{root\_node\_sum\_of\_infeasibilities}
        }
    \end{equation}
\f]

where 

- objective_value is the objective value at the current node;
- sum_of_infeasibilities is the sum of violation of integralities constraints at the node;
- incumbent_objective is the objective value of the best known feasible point so far;
- root_onode_bjective and root_node_sum_of_infeasibilities are the values stored at the root node of "objective_value" and "sum_of_infeasibilities".

Intuitively, this balances two factors: nodes with high objective values are promising, but nodes that are less infeasible (closer to satisfying integrality constraints) are more likely to lead to feasible solutions. 

> If there is no incumbent solution yet, the strategy defaults to the node with the best objective value. 
> Thus, it acts like the best bound approach.

Here is how to use it.

```cpp
branch_and_bound.with_node_selection_rule(BestEstimate());
```

This node selection rule is due to <a href="https://doi.org/10.1287/mnsc.20.5.736">J. J. H. Forrest et al. (1974)</a> under the name "Best Projection".

\section lib_milp_bnb_node_selection_BreadthFirst Breadth First

The breadth first strategy explores nodes level by level, starting from the root and moving across all nodes at a given depth before descending further.

Here is how to use it.

```cpp
branch_and_bound.with_node_selection_rule(BreadthFirst());
```

\section lib_milp_bnb_node_selection_DepthFirst Depth First

The depth first strategy explores a branch as far as possible before backtracking.
It can quickly find feasible solutions deep in the tree, which helps prune other branches early.
However, it may waste time exploring suboptimal branches if the initial path is not promising.

Here is how to use it.

```cpp
branch_and_bound.with_node_selection_rule(DepthFirst());
```

\section lib_milp_bnb_node_selection_WorstBound Worst Bound

The worst bound strategy selects the node with the worst (i.e., the highest) bound among all active nodes.

Here is how to use it.

```cpp
branch_and_bound.with_node_selection_rule(WorstBound());
```

\section lib_milp_bnb_node_selection_writing Writing Your Own Node Selection Rule

\warning TODO write this section

\page lib_milp_bnb_branching_rule Branching Rules

\tableofcontents

Given a node selected for exploration, the next step is to decide how to split the node into new nodes. 
Here again, several strategies are at hand.

| Branching Rule             | Purpose / Guideline                                                                                    |
|----------------------------|--------------------------------------------------------------------------------------------------------|
| **First Infeasible Found** | Branch on the first infeasible variable found; simple and fast.                                        |
| **Least Infeasible**       | Branch on variable closest to feasibility.                                                             |
| **Most Infeasible**        | Branch on variable farthest from feasibility; may reduce infeasibilities faster.                       |
| **Pseudo Cost**            | Branch on variable with highest historical impact on objective; balances efficiency and effectiveness. |
| **Strong Branching**       | Test candidate branches before choosing; often gives best node reduction but more expensive.           |
| **Uniformly Random**       | Pick a variable randomly; mainly for testing.                                                          |

\section lib_milp_bnb_branching_rule_FirstInfeasibleFound First Infeasible Found

The first infeasible found strategy selects the first variable that violates its integrality constraint.

Here is how to use it. 

```cpp
branch_and_bound.with_branching_rule(FirstInfeasibleFound());
```

\section lib_milp_bnb_branching_rule_LeastInfeasible Least Infeasible

The least infeasible strategy selects the variable whose value is closest to an integer.

Here is how to use it.

```cpp
branch_and_bound.with_branching_rule(LeastInfeasibleFound());
```

\section lib_milp_bnb_branching_rule_MostInfeasible Most Infeasible

The most infeasible strategy selects the variable whose value is farthest from an integer.

Here is how to use it.

```cpp
branch_and_bound.with_branching_rule(MostInfeasibleFound());
```

\section lib_milp_bnb_branching_rule_PseudoCost Pseudo Cost Branching

The pseudo cost strategy selects the variable with the highest estimated impact on the objective value, based on past branching decisions.
To do so, for each branching candidate \\( x_j \\), two scores are first computed:

- an upper bounding score (for the branch \\( x_j \le \lfloor x_j^* \rfloor \\))
\f[
    \text{score}^+_j = (x_j^* - \lfloor x_j^* \rfloor) \cdot \frac{ \sum_{k} \Delta\text{obj}_{kj}^+ }{ n_j }
\f]
- a lower bounding score (for the branch \\( x_j \ge \lceil x_j^* \rceil \\))
\f[
    \text{score}^-_j = (\lceil x_j^* \rceil - x_j^*) \cdot \frac{ \sum_{k} \Delta\text{obj}_{kj}^- }{ n_j }
\f]

Here, 
- \\( x^* \\) denotes the current node solution,
- \\( n_j \\) denote the number of times \\( x_j \\) has previously been selected for branching, 
- \\( \Delta\text{obj}_{kj}^+ \\) (resp., \\( \Delta\text{obj}_{kj}^- \\)) denotes the objective change that occurred at node \\( k \\) after having branched on 
\\( x_j \\) in the left (resp., right) child node w.r.t. the parent node.

Then, the two scores are combined using one of the two following formulas:

- a linear one using (with default value \\( \alpha = 1/16 \\))
\f[
    \text{score}_j = (1 - \alpha) \times \min\{ \text{score}^+_j, \text{score}^-_j \} + \alpha\times \max\{ \text{score}^+_j, \text{score}^-_j \}.
\f]
  This slightly favors the larger score while keeping a base weight on the smaller one.
  It is the default setting.
- a quadratic one using (with default value \\( \varepsilon = 10^{-6} \\))
\f[
  \text{score}_j = \min\{ \varepsilon, \text{score}^+_j \} \times \min\{ \varepsilon, \text{score}^-_j \}
\f]
  This method heavily penalizes variables where one score is very small, favoring variables that are strong in both directions.

The branching candidate with the highest score is selected for branching.

Here is how to use it.

```cpp
branch_and_bound.with_branching_rule(PseudoCost());
```

\warning TODO add how to change node scoring function and their parameters 

\section lib_milp_bnb_branching_rule_StrongBranching Strong Branching

Strong branching is a sophisticated variable selection strategy that estimates 
the impact of branching by temporarily solving child nodes before making the final branching decision.

More specifically, for each branching candidate, strong branching requires to solves, before branching happens,
the left and right child node. Thus, two scores are computed: 

- an upper bounding score (for the branch \\( x_j \le \lfloor x_j^* \rfloor \\))
  \f[
  \text{score}^+_j = \text{objective\_value}^+ - \text{parent\_objective\_value}
  \f]
- a lower bounding score (for the branch \\( x_j \ge \lceil x_j^* \rceil \\))
  \f[
  \text{score}^-_j = \text{objective\_value}^- - \text{parent\_objective\_value}
  \f]

Here,
- \\( \text{parent\\_objective\\_value} \\) denotes the optimal objective function value of the parent node,
- \\( \text{objective\\_value}^+ \\) (resp., \\( \text{objective\\_value}^- \\)) denotes the objective value of the left (resp., right) node.

Then, the two scores are combined using one of the two formulas that are detailed in the pseudo cost section.

The branching candidate with the highest score is selected for branching.

Several variants of strong branching exists, and are detailed next along with their implementation in `idol`.

\subsection lib_milp_bnb_branching_rule_StrongBranching_full Full Strong Branching

Full strong branching denotes the standard strong branching rule which solves twice as many nodes as there are branching candidates at each node selected for branching. 
Thus, a clear drawback is that it may take a lot of time to solve all these sub-problems.

Here is how to use it.

```cpp
branch_and_bound.with_branching_rule(StrongBranching());
```

\subsection lib_milp_bnb_branching_rule_StrongBranching_restricted Restricted Strong Branching

Restricted strong branching is an attempt to reduce the computational burden of full strong branching. 
The idea is to consider only a maximum of \\(K\\) branching candidates at each branching decision instead of the whole set of branching candidates.
At each node, we therefore build a "restricted branching candidate set" obtained by taking the \\( K \\) first variables selected by
another branching rule. Then, strong branching is applied on these \\( K \\) variables. 

Here is how to use it, here with \\( K = 10 \\).

```cpp
branch_and_bound.with_branching_rule(StrongBranching().with_max_n_variables(10));
```

By default, the branching rule used to select the \\( K \\) candidates is the most infeasible branching rule.

\subsection lib_milp_bnb_branching_rule_StrongBranching_phases Strong Branching with Phases

Strong branching with phases is a combination of the above approaches which applies different schemes depending
on the level of the current node in the branch-and-bound tree. Additionally, it allows to solve each node only approximately
by, e.g., imposing a maximum number of iterations for the underlying optimizer.

Here is an example of strong branching with phases which, for nodes whose level is below or equal to 3, applies full
strong branching, then switches to restricted strong branching with \\( K = 30 \\) and solves nodes with an iteration
limit of 20. 

```cpp
auto strong_branching = StrongBranching();
strong_branching.add_phase(StrongBranchingPhases::WithNodeOptimizer(), std::numeric_limits<unsigned int>::max(), 3)
strong_branching.add_phase(StrongBranchingPhases::WithIterationLimit(20), 30, std::numeric_limits<unsigned int>::max());

branch_and_bound.with_branching_rule(strong_branching);
```

Observe how we used :code:`std::numeric_limits<unsigned int>::max()` to remove restrictions on the number of
considered variables and on the maximum depth for the final phase. Note that, by default, if no phase is triggered for a
given depth, e.g., because it was not specified, full strong branching is applied. 
Here, however, we make sure that the second phase is always triggered.

\subsection lib_milp_bnb_branching_rule_StrongBranching_look_ahead Strong Branching with Look Ahead

Strong branchign with look ahead is similar to restricted strong branching yet differs from it by not specifying a
fixed size for the "restricted branching candidate set". Instead, it considers a look ahead parameter, noted
\\( L \\), and applies the full strong branching rule until the branching candidate does not change after \\( L \\)
iterations. Then, the algorithm stops and the current branching candidate is returned.

Unfortunately, this approach is not yet implemented in `idol`.

\subsection lib_milp_bnb_branching_rule_scoring Changing the Scoring Function

The scoring function can be changed using the `.with_scoring_function` method.
For instance, here is how to use the quadratic scoring function.

```cpp
auto strong_branching = StrongBranching();
strong_branching.with_scoring_function(NodeScoreFunctions::Product()); 
```

\section lib_milp_bnb_branching_rule_UniformlyRandom Uniformly Random Branching

The uniformly random strategy selects a branching variable randomly among all candidates.

Here is how to use it. 

```cpp
branch_and_bound.with_branching_rule(UniformlyRandom());
```

\section lib_milp_bnb_branching_rule_BranchingWithPriority Branching with Priorities

Branching with priorities groups variables into batches according to user-defined priorities.
Within each batch, a standard branching rule (e.g., most infeasible) is applied to select the next variable.
Higher-priority batches are always considered before lower-priority ones, allowing you to control the order in which variables are branched on.

Here is an example. 

```cpp
std::vector<Var> high_priority = { x[0], x[1], x[3] };
std::vector<Var> low_priority = { x[4], x[5], x[6] };

auto branching_with_priority = BranchingWithPriority();
branching_with_priority.add_branching_rule(StrongBranching(high_priority.begin(), high_priority.end()));
branching_with_priority.add_branching_rule(MostInfeasible(low_priority.begin(), low_priority.end()));
```

Here, the branching strategy is as follows. First, it considers variables in the high-priority batch and uses (full) strong branching.
Once all the variables in this batch have integer values, it considers the low-priority batch and uses the most infeasible rule.

\section lib_milp_bnb_branching_rule_writing Writing Your Own Branching Rule

\warning TODO write this section

\page lib_milp_bnb_callbacks Callbacks

\tableofcontents

\section lib_milp_bnb_callbacks_specific Writing a Branch-and-Bound-Specific Callback

\section lib_milp_bnb_callbacks_universal Writing a Universal Callback

\page lib_milp_bnb_cutting_planes Cutting Planes

\tableofcontents

\page lib_milp_bnb_heuristics Heuristics

\tableofcontents

\page lib_milp_bnb_presolve Presolve

\tableofcontents

Presolve techniques analyze the problem before starting the branch-and-bound search to simplify it and reduce the search space. 
For instance, it may tighten variable bounds, remove redundancies, or strengthen constraints.
Presolve can significantly speed up the solution process.

Typically, a presolve operation is added using the `.with_presolver` method. For instance, here is how to add the 
`OneRowBoundStrengthening` presolver detailed bellow. 

```cpp
branch_and_bound.add_presolver(OneRowBoundStrengthening());
```

For more details, please refer to <a href="https://doi.org/10.1287/ijoc.2018.0857">Achterberg et al. (2019)</a>.

\section lib_milp_bnb_presolve_BoundRounding Bound Rounding

This presolve simply rounds down or up the variable bounds associated to integer or binary variables.
This is typically used in combination with other presolvers that may affect variable bounds.

Here is how to add it.

```cpp
branch_and_bound.add_presolver(BoundRounding());
```

\section lib_milp_bnb_presolve_OneRowBoundStrengthening One Row Bound Strengthening

One-row bound strengthening tightens variable bounds by analyzing each constraint individually. Consider a linear inequality

\f[
    A_{iS} x_S + a_{ik} x_k \le b_i,
\f]

where \\( S = \text{supp}(A_i\cdot) \setminus \\{k\\} \\) and \\( a_{ik} \neq 0 \\). First, a lower bound on the sum of the other variables is computed

\f[
    \ell_{iS} = \inf \{ A_{iS} x_S \}.
\f]

Then, depending on the sign of \\(a_{ik}\\), the bound of \\(x_k\\) is updated as follows:

- If \\(a_{ik} > 0\\), update the upper bound:

\f[
    u_k := \min \Big\{ u_k, \frac{b_i - \ell_{iS}}{a_{ik}} \Big\}.
\f]

- If \\(a_{ik} < 0\\), update the lower bound:

\f[
    \ell_k := \max \Big\{ \ell_k, \frac{b_i - \ell_{iS}}{a_{ik}} \Big\}.
\f]

This procedure is applied iteratively across all constraints, with safeguards to prevent infinite sequences of tiny reductions. 
More specifically, 
- a change is ignored if the improvement is smaller than \\(10^3 \cdot \varepsilon\\), where \\(\varepsilon\\) is the feasibility tolerance,
- bounds with absolute values exceeding \\(10^8\\) are also ignored.  

Only one round per constraint is applied per presolve pass. 

> Note that this presolver does not round the bounds for integer variables. Please, use this in combination with the `BoundRounding` presolver for better performance.

\page lib_milp_bnb_node_type Using Your Own Node Type

\tableofcontents

\page lib_milp_bnb_options Other Options

\tableofcontents

\section lib_milp_bnb_options_solution_pool Solution Pool Size

\section lib_milp_bnb_options_logs Logging
