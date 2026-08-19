\page lib_milp_branch_and_bound Creating a Branch-and-Bound Algorithm
\brief Describes how to create a branch-and-bound algorithm

\tableofcontents

The branch-and-bound algorithm is a very well-known approach to solve combinatorial problems. 
It is a divide and conquer type of algorithms. 

> For an introduction, please refer to the <a href="https://en.wikipedia.org/wiki/Branch_and_bound">Wikipedia page</a>
> or to the book <a href="https://doi.org/10.1007/978-3-319-11008-0">Integer Programming</a>.
> 
> Otherwise, we assume that you are familiar with the branch-and-bound algorithm.

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

Throughout this tutorial, bounds and objective values are described using the minimization convention implemented
by the current branch-and-bound optimizer. Thus, a node relaxation provides a lower bound, a smaller node bound is
better for node selection, and an incumbent provides an upper bound on the optimal objective value. Idol models are
minimization problems.

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
- \subpage lib_milp_bnb_callbacks
- \subpage lib_milp_bnb_presolve
- \subpage lib_milp_bnb_cutting_planes
- \subpage lib_milp_bnb_heuristics
- \subpage lib_milp_bnb_node_type
- \subpage lib_milp_bnb_options

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

Under the minimization convention stated above, the best bound strategy selects the active node with the smallest
relaxation bound.

Here is how to use it.

```cpp
branch_and_bound.with_node_selection_rule(BestBound());
```

\section lib_milp_bnb_node_selection_BestEstimate Best Estimate

The best estimate strategy combines the node relaxation value with its degree of integrality infeasibility.

To do so, a score is computed for each node and the one with the lowest score is selected.
The score is computed as follows.

\f[
    s_v = z_v + (z_0 - \bar z)\frac{I_v}{I_0}.
\f]

Here, \f$z_v\f$ and \f$I_v\f$ are respectively the relaxation objective value and the sum of integrality
infeasibilities at node \f$v\f$; \f$z_0\f$ and \f$I_0\f$ are their values at the root node; and \f$\bar z\f$ is the
objective value of the current incumbent. The implementation selects an active node minimizing \f$s_v\f$. Under
minimization, a smaller \f$z_v\f$ lowers the score. Since normally \f$z_0 \le \bar z\f$, the coefficient
\f$z_0-\bar z\f$ is non-positive, so the formula as implemented lowers the score when \f$I_v\f$ increases.

> If there is no incumbent solution yet, the strategy defaults to the node with the best objective value. 
> Thus, it acts like the best bound approach.

Here is how to use it.

This built-in rule works with the default `BranchAndBound` node-information type, `DefaultNodeInfo`.

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

A node-selection strategy derives from `NodeSelectionRule<NodeInfoT>` and returns an iterator into the
`NodeSet<Node<NodeInfoT>>` passed to `operator()`. The set exposes two ordered views:
`by_objective_value()` and `by_level()`. The returned iterator must belong to one of these views because the
branch-and-bound optimizer erases the selected node through that iterator.

The strategy is created by a matching `NodeSelectionRuleFactory<NodeInfoT>`. Factories are cloned when the
`BranchAndBound` factory is copied, so a custom factory must implement both `operator()` and `clone()`.
The following rule selects the deepest active node.

```cpp
template<class NodeInfoT>
class DeepestNodeSelection : public NodeSelectionRule<NodeInfoT> {
public:
    explicit DeepestNodeSelection(Optimizers::BranchAndBound<NodeInfoT>& parent)
        : NodeSelectionRule<NodeInfoT>(parent) {}

    typename NodeSet<Node<NodeInfoT>>::const_iterator
    operator()(const NodeSet<Node<NodeInfoT>>& active_nodes) override {
        auto result = active_nodes.by_level().end();
        --result;
        return result;
    }
};

template<class NodeInfoT>
class DeepestNodeSelectionFactory : public NodeSelectionRuleFactory<NodeInfoT> {
public:
    NodeSelectionRule<NodeInfoT>*
    operator()(Optimizers::BranchAndBound<NodeInfoT>& parent) const override {
        return new DeepestNodeSelection<NodeInfoT>(parent);
    }

    DeepestNodeSelectionFactory* clone() const override {
        return new DeepestNodeSelectionFactory(*this);
    }
};
```

It is then configured like any built-in strategy.

```cpp
branch_and_bound.with_node_selection_rule(
    DeepestNodeSelectionFactory<DefaultNodeInfo>()
);
```

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
branch_and_bound.with_branching_rule(LeastInfeasible());
```

\section lib_milp_bnb_branching_rule_MostInfeasible Most Infeasible

The most infeasible strategy selects the variable whose value is farthest from an integer.

Here is how to use it.

```cpp
branch_and_bound.with_branching_rule(MostInfeasible());
```

\section lib_milp_bnb_branching_rule_PseudoCost Pseudo Cost Branching

The pseudo-cost strategy estimates, from earlier branching observations, how much each candidate variable may
increase the relaxation bound. For a candidate \f$x_j\f$ having value \f$x_j^*\f$ at the current node, the
implementation computes a separate score for each branching direction:

- the score of the upper-bound branch \f$x_j \le \lfloor x_j^* \rfloor\f$,
\f[
    s_j^{\mathrm{ub}}
    = (x_j^* - \lfloor x_j^* \rfloor)
      \frac{\sum_{k \in K_j^{\mathrm{ub}}} \Delta z_{kj}^{\mathrm{ub}}}
           {n_j^{\mathrm{ub}}},
\f]
- the score of the lower-bound branch \f$x_j \ge \lceil x_j^* \rceil\f$,
\f[
    s_j^{\mathrm{lb}}
    = (\lceil x_j^* \rceil - x_j^*)
      \frac{\sum_{k \in K_j^{\mathrm{lb}}} \Delta z_{kj}^{\mathrm{lb}}}
           {n_j^{\mathrm{lb}}}.
\f]

Here, \f$K_j^{\mathrm{ub}}\f$ and \f$K_j^{\mathrm{lb}}\f$ index the previous observations for the two directions;
\f$n_j^{\mathrm{ub}} = |K_j^{\mathrm{ub}}|\f$ and
\f$n_j^{\mathrm{lb}} = |K_j^{\mathrm{lb}}|\f$ are their separate sample counts; and
\f$\Delta z_{kj}^{\mathrm{ub}}\f$ and \f$\Delta z_{kj}^{\mathrm{lb}}\f$ are the observed relaxation-bound
increases per unit change in \f$x_j\f$. If a direction has no observation yet, the implementation substitutes the
average available directional score over the other candidates, or zero if no such observation exists.

Then, the two scores are combined using one of the two following formulas:

- a linear score with default parameter \f$\alpha = 1/16\f$,
\f[
    s_j = (1 - \alpha)\min\{s_j^{\mathrm{ub}},s_j^{\mathrm{lb}}\}
          + \alpha\max\{s_j^{\mathrm{ub}},s_j^{\mathrm{lb}}\}.
\f]
  This is the score used by `PseudoCost`.
- a product score with default parameter \f$\varepsilon = 10^{-5}\f$,
\f[
    s_j = \min\{\varepsilon,s_j^{\mathrm{ub}}\}
          \min\{\varepsilon,s_j^{\mathrm{lb}}\}.
\f]
  The implementation caps each directional score at \f$\varepsilon\f$ before multiplication. In particular, if both
  directional scores are at least \f$\varepsilon\f$, the combined score is \f$\varepsilon^2\f$; larger directional
  scores do not increase it further.

The branching candidate with the highest score is selected for branching.

Here is how to use it.

```cpp
branch_and_bound.with_branching_rule(PseudoCost());
```

The current `PseudoCost` factory does not expose a method for changing this combination: it always uses
`NodeScoreFunctions::Linear()` and therefore its default parameter \f$ \alpha = 1/16 \f$. The public score-function
configuration described below is currently available for `StrongBranching`, not for `PseudoCost`.

\section lib_milp_bnb_branching_rule_StrongBranching Strong Branching

Strong branching estimates the effect of branching by performing temporary trial solves before selecting a
variable. For each candidate \f$x_j\f$, the implementation constructs two temporary node-information objects and
solves the relaxations corresponding to the two candidate branches. These trials are not inserted into the active
branch-and-bound tree and are distinct from the actual children created after the variable is selected.

Let \f$z_v\f$ be the relaxation bound of the current parent node \f$v\f$. If
\f$z_j^{\mathrm{ub}}\f$ and \f$z_j^{\mathrm{lb}}\f$ are the bounds obtained by the upper-bound and lower-bound trial
solves, respectively, the directional gains are

\f[
    g_j^{\mathrm{ub}} = z_j^{\mathrm{ub}} - z_v
\f]
and
\f[
    g_j^{\mathrm{lb}} = z_j^{\mathrm{lb}} - z_v.
\f]

The upper-bound trial imposes \f$x_j \le \lfloor x_j^*\rfloor\f$, while the lower-bound trial imposes
\f$x_j \ge \lceil x_j^*\rceil\f$. The two gains are combined by the configured `NodeScoreFunction`, using the
linear or product formula described above.

The branching candidate with the highest score is selected for branching.

Several variants of strong branching exists, and are detailed next along with their implementation in `idol`.

\subsection lib_milp_bnb_branching_rule_StrongBranching_full Full Strong Branching

Full strong branching performs two trial relaxation solves for every branching candidate at each branching
decision. These extra solves can be computationally expensive, even though their temporary nodes are not inserted
into the search tree.

Here is how to use it.

```cpp
branch_and_bound.with_branching_rule(StrongBranching());
```

\subsection lib_milp_bnb_branching_rule_StrongBranching_restricted Restricted Strong Branching

Restricted strong branching is an attempt to reduce the computational burden of full strong branching. 
The idea is to consider only a maximum of \f$K\f$ branching candidates at each branching decision instead of the whole set of branching candidates.
At each node, we therefore build a "restricted branching candidate set" obtained by taking the \f$ K \f$ first variables selected by
another branching rule. Then, strong branching is applied on these \f$ K \f$ variables.

Here is how to use it, here with \f$ K = 10 \f$.

```cpp
branch_and_bound.with_branching_rule(StrongBranching().with_max_n_variables(10));
```

By default, the branching rule used to select the \f$ K \f$ candidates is the most infeasible branching rule.

\subsection lib_milp_bnb_branching_rule_StrongBranching_phases Strong Branching with Phases

Strong branching with phases is a combination of the above approaches which applies different schemes depending
on the level of the current node in the branch-and-bound tree. Additionally, it allows to solve each node only approximately
by, e.g., imposing a maximum number of iterations for the underlying optimizer.

Here is an example of strong branching with phases which, for nodes whose level is below or equal to 3, applies full
strong branching, then switches to restricted strong branching with \f$ K = 30 \f$ and solves nodes with an iteration
limit of 20. 

```cpp
auto strong_branching = StrongBranching();
strong_branching.add_phase(StrongBranchingPhases::WithNodeOptimizer(), std::numeric_limits<unsigned int>::max(), 3);
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
\f$ L \f$, and applies the full strong branching rule until the branching candidate does not change after \f$ L \f$
iterations. Then, the algorithm stops and the current branching candidate is returned.

Unfortunately, this approach is not yet implemented in `idol`.

\subsection lib_milp_bnb_branching_rule_scoring Changing the Scoring Function

The strong-branching score combiner is selected with `with_node_scoring_function`. The available implementations
are `NodeScoreFunctions::Linear`, whose constructor optionally receives \f$ \alpha \f$ and defaults to
\f$ \alpha = 1/16 \f$, and `NodeScoreFunctions::Product`, whose constructor optionally receives
\f$ \varepsilon \f$ and defaults to \f$ \varepsilon = 10^{-5} \f$. Strong branching uses `Product` by default.
For instance, the following selects the linear combiner with \f$ \alpha = 0.1 \f$.

`StrongBranching` works with the default `BranchAndBound` node-information type, `DefaultNodeInfo`.

```cpp
auto strong_branching = StrongBranching();
strong_branching.with_node_scoring_function(NodeScoreFunctions::Linear(0.1));
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

A branching strategy derives from `BranchingRule<NodeInfoT>`. It must decide whether a solved node is valid for
the original problem in `is_valid` and, for an invalid node, allocate the information objects for its children in
`create_child_nodes`. Ownership of the returned pointers is transferred to the branch-and-bound tree. The optional
hooks `initialize`, `on_node_solved`, and `on_nodes_have_been_created` support strategies that maintain state.

Most MILP rules can reuse `BranchingRules::VariableBranching<NodeInfoT>`. That class already checks integrality,
creates the two children imposing \f$ x_j \ge \lceil x_j^* \rceil \f$ and
\f$ x_j \le \lfloor x_j^* \rfloor \f$, and selects the variable having the largest score. A derived strategy only
needs to implement `scoring_function`. Its corresponding factory derives from `BranchingRuleFactory<NodeInfoT>`.
The following minimal rule scores a fractional variable \f$ x_j \f$ by its distance to the nearest integer.

```cpp
template<class NodeInfoT>
class FractionalityBranching
    : public BranchingRules::VariableBranching<NodeInfoT> {
public:
    FractionalityBranching(
        const Optimizers::BranchAndBound<NodeInfoT>& parent,
        std::list<Var> candidates
    ) : BranchingRules::VariableBranching<NodeInfoT>(parent,
                                                      std::move(candidates)) {}

    std::list<std::pair<Var, double>> scoring_function(
        const std::list<Var>& variables,
        const Node<NodeInfoT>& node
    ) override {
        std::list<std::pair<Var, double>> result;
        for (const auto& var : variables) {
            const double value = node.info().primal_solution().get(var);
            result.emplace_back(var, std::abs(value - std::round(value)));
        }
        return result;
    }
};

template<class NodeInfoT>
class FractionalityBranchingFactory
    : public BranchingRuleFactory<NodeInfoT> {
public:
    BranchingRule<NodeInfoT>* operator()(
        const Optimizers::BranchAndBound<NodeInfoT>& parent
    ) const override {
        std::list<Var> candidates;
        for (const auto& var : parent.parent().vars()) {
            if (parent.parent().get_var_type(var) != Continuous) {
                candidates.emplace_back(var);
            }
        }
        return new FractionalityBranching<NodeInfoT>(
            parent,
            std::move(candidates)
        );
    }

    FractionalityBranchingFactory* clone() const override {
        return new FractionalityBranchingFactory(*this);
    }
};
```

The custom factory is supplied directly to the algorithm.

```cpp
branch_and_bound.with_branching_rule(
    FractionalityBranchingFactory<DefaultNodeInfo>()
);
```

\page lib_milp_bnb_callbacks Callbacks

\tableofcontents

Callbacks observe the search and may modify it at specific points. Add as many callback factories as needed with
`BranchAndBound::add_callback`; the algorithm creates and owns the callback strategies when it is attached to a
model.

The current branch-and-bound implementation emits three events after solving and analyzing a node relaxation:

- `IncumbentSolution`, after the relaxation point satisfies the branching rule and improves the incumbent bound,
  but before idol accepts it as the new incumbent;
- `InvalidSolution`, after a promising relaxation point fails the branching rule, and before cuts are recycled or
  children are created;
- `PrunedSolution`, after idol determines that the node will not be branched because it is infeasible, reached an
  objective limit, or cannot improve the incumbent.

`NodeLoaded` is part of the generic `CallbackEvent` enumeration, but the current `BranchAndBound` implementation
does not emit it. Callback code should therefore not rely on receiving that event from this optimizer.

\section lib_milp_bnb_callbacks_specific Writing a Branch-and-Bound-Specific Callback

A branch-and-bound-specific callback derives from `BranchAndBoundCallback<NodeInfoT>`. Its `operator()` receives
the event and can inspect `node()`, `original_model()`, the elapsed `time()`, `best_bound()`, `best_obj()`, and
`node_count()`. During callbacks associated with a solved tree node, `relaxation()` gives read-only access to the
current node relaxation. The callback may add user or lazy cuts, add a local variable-branching decision, submit a
candidate node-information object, submit a proven bound, or call `terminate()`.

The callback is paired with a `BranchAndBoundCallbackFactory<NodeInfoT>`, which implements `operator()` and
`clone()`. The following callback stops after a chosen number of solved nodes.

```cpp
template<class NodeInfoT>
class NodeLimitCallback : public BranchAndBoundCallback<NodeInfoT> {
    unsigned int m_limit;
public:
    explicit NodeLimitCallback(unsigned int limit) : m_limit(limit) {}

protected:
    void operator()(CallbackEvent) override {
        if (this->node_count() >= m_limit) {
            this->terminate();
        }
    }
};

template<class NodeInfoT>
class NodeLimitCallbackFactory
    : public BranchAndBoundCallbackFactory<NodeInfoT> {
    unsigned int m_limit;
public:
    explicit NodeLimitCallbackFactory(unsigned int limit) : m_limit(limit) {}

    BranchAndBoundCallback<NodeInfoT>* operator()() override {
        return new NodeLimitCallback<NodeInfoT>(m_limit);
    }

    NodeLimitCallbackFactory* clone() const override {
        return new NodeLimitCallbackFactory(*this);
    }
};

void add_node_limit_callback(BranchAndBound<DefaultNodeInfo>& branch_and_bound) {
    branch_and_bound.add_callback(
        NodeLimitCallbackFactory<DefaultNodeInfo>(1000)
    );
}
```

\section lib_milp_bnb_callbacks_universal Writing a Universal Callback

A universal callback derives from `Callback` and is created by a `CallbackFactory`. It uses the same
`CallbackEvent` values but does not expose the branch-and-bound-specific `Node<NodeInfoT>` or `relaxation()`.
Instead, it provides the current `primal_solution()`, the original model, bounds, elapsed time, node count, cut and
heuristic-solution submission, and termination. This makes the callback usable with any optimizer that implements
the universal callback interface.

Passing a `CallbackFactory` to `BranchAndBound::add_callback` automatically wraps it in
`CallbackAsBranchAndBoundCallback<NodeInfoT>`. Built-in factories such as `ReducedCostFixing`, `UserCutCallback`,
`LazyCutCallback`, and `Heuristics::SimpleRounding` use this interface.

\page lib_milp_bnb_cutting_planes Cutting Planes

\tableofcontents

Cuts are submitted from callbacks as temporary linear constraints. A user cut is intended to strengthen a node
relaxation at an `InvalidSolution` event. The caller is responsible for ensuring that it does not remove solutions
that should remain feasible; the callback API does not prove this property. A lazy cut is intended to reject a
candidate encountered at an `IncumbentSolution` event because it violates a constraint omitted from the initial
formulation. These two roles correspond to `add_user_cut` and `add_lazy_cut` on both callback interfaces.

\section lib_milp_bnb_cutting_planes_cut_pool The Cut Pool

User cuts pass through the branch-and-bound `CutPool`. The pool rejects duplicate or nearly parallel cuts, keeps
accepted cuts available for later nodes, recycles violated cuts before branching, and periodically removes inactive
cuts from the current relaxation. An accepted user cut or a recycled cut causes the current node relaxation to be
reoptimized before branching. The pool is owned by the branch-and-bound optimizer and is shared across its node
processing rather than being stored as metadata on one tree node.

Lazy cuts do not currently use this pool. The implementation adds them directly to the first relaxation. This is a
current capability boundary and differs from the user-cut lifecycle.

\section lib_milp_bnb_cutting_planes_cgl Using the Coin-OR Cut Generation Library (Cgl)

When idol is built with Cgl support, `CglCutCallback<NodeInfoT>` separates standard Cgl cut families at
`InvalidSolution` events and submits accepted inequalities as user cuts. The callback is added directly to the
branch-and-bound factory.

```cpp
branch_and_bound.add_callback(CglCutCallback());
```

The current callback includes cover, flow-cover, zero-half, mixed-integer-rounding, and residual-capacity
generators. If idol was not linked with Cgl, initialization of this callback throws an exception.

\section lib_milp_bnb_cutting_planes_user_cuts Adding Your Own User Cuts

A custom callback should separate user cuts at `InvalidSolution`, where the current relaxation point is available.
After one or more accepted cuts are added, the node is solved again. For example, the following universal callback
adds the valid inequality \f$ x + y \le 1 \f$ whenever the current relaxation violates it.

```cpp
class MyUserCutCallback : public Callback {
    Var m_x;
    Var m_y;
public:
    MyUserCutCallback(Var x, Var y) : m_x(x), m_y(y) {}

protected:
    void operator()(CallbackEvent event) override {
        if (event != InvalidSolution) {
            return;
        }
        const auto point = primal_solution();
        if (point.get(m_x) + point.get(m_y) > 1) {
            add_user_cut(m_x + m_y <= 1);
        }
    }
};
```

As with every `Callback`, this strategy must be returned by a `CallbackFactory` before being passed to
`BranchAndBound::add_callback`. For separation expressed as an optimization model, the built-in
`UserCutCallback` factory performs this wiring and requires a separation optimizer configured through
`with_separation_optimizer`.

\section lib_milp_bnb_cutting_planes_lazy_cuts Adding Your Own Lazy Cuts

Lazy cuts are normally separated at `IncumbentSolution`. If the callback adds a lazy cut, the candidate is not
accepted as incumbent and its node is reoptimized. A universal callback can inspect `primal_solution()` and call
`add_lazy_cut` exactly as in the user-cut example, but with `IncumbentSolution` as its triggering event.

The built-in `LazyCutCallback` expresses separation as an auxiliary optimization model. It is configured with the
separation model, a `GenerationPattern<Ctr>`, an optional constraint type, and an optimizer through
`with_separation_optimizer`. Its strategy is triggered only by `IncumbentSolution`.

\page lib_milp_bnb_heuristics Heuristics

\tableofcontents

The callback interface is also the extension point for primal heuristics. A universal `Callback` submits a
`PrimalPoint` with `submit_heuristic_solution`; a `BranchAndBoundCallback<NodeInfoT>` instead transfers ownership
of a `NodeInfoT*` whose primal solution has been filled. Submission only proposes a candidate. Under minimization,
the incumbent is the accepted candidate with the smallest objective value found so far, and its value is the current
upper bound. The optimizer first rejects a submitted candidate whose stored objective value is worse than the
incumbent value. It then invokes `IncumbentSolution` callbacks, rejects the candidate if those callbacks add a lazy
cut, and otherwise stores it as the new incumbent. The current submission path does not independently check the
candidate against the model constraints or call the branching rule's validity test; the submitting heuristic is
responsible for constructing a feasible candidate.

For the default node type, a branch-and-bound-specific callback can submit a point as follows.

```cpp
auto* candidate = new DefaultNodeInfo();
candidate->set_primal_solution(std::move(point));
submit_heuristic_solution(candidate); // ownership is transferred
```

Submission does not itself construct or repair a feasible point; the callback is responsible for doing so. The
current public design therefore does not require a separate heuristic base class. Built-in heuristic factories,
including `Heuristics::SimpleRounding`, `Heuristics::LocalMIP`, and `Heuristics::RENS`, are callbacks and are added
with `BranchAndBound::add_callback`.

```cpp
branch_and_bound.add_callback(Heuristics::LocalMIP());
```

\page lib_milp_bnb_presolve Presolve

\tableofcontents

Presolve techniques analyze the problem before starting the branch-and-bound search to simplify it and reduce the search space. 
For instance, it may tighten variable bounds, remove redundancies, or strengthen constraints.
Presolve can significantly speed up the solution process.

Typically, a presolve operation is added using the `.with_presolver` method. For instance, here is how to add the 
`Presolvers::OneRowBoundTightening` presolver detailed below.

```cpp
branch_and_bound.add_presolver(Presolvers::OneRowBoundTightening());
```

For more details, please refer to <a href="https://doi.org/10.1287/ijoc.2018.0857">Achterberg et al. (2019)</a>.

\section lib_milp_bnb_presolve_BoundRounding Bound Rounding

This presolve simply rounds down or up the variable bounds associated to integer or binary variables.
This is typically used in combination with other presolvers that may affect variable bounds.

Here is how to add it.

```cpp
branch_and_bound.add_presolver(Presolvers::BoundRounding());
```

\section lib_milp_bnb_presolve_OneRowBoundTightening One Row Bound Tightening

One-row bound strengthening tightens variable bounds by analyzing each constraint individually. Consider a linear inequality

\f[
    A_{iS} x_S + a_{ik} x_k \le b_i,
\f]

where \f$ S = \text{supp}(A_i\cdot) \setminus \\{k\\} \f$ and \f$ a_{ik} \neq 0 \f$. First, a lower bound on the sum of the other variables is computed

\f[
    \ell_{iS} = \inf \{ A_{iS} x_S \}.
\f]

Then, depending on the sign of \f$a_{ik}\f$, the bound of \f$x_k\f$ is updated as follows:

- If \f$a_{ik} > 0\f$, update the upper bound:

\f[
    u_k := \min \Big\{ u_k, \frac{b_i - \ell_{iS}}{a_{ik}} \Big\}.
\f]

- If \f$a_{ik} < 0\f$, update the lower bound:

\f[
    \ell_k := \max \Big\{ \ell_k, \frac{b_i - \ell_{iS}}{a_{ik}} \Big\}.
\f]

This procedure is applied iteratively across all constraints, with safeguards to prevent infinite sequences of tiny reductions. 
More specifically, 
- a change is ignored if the improvement is smaller than \f$10^3 \cdot \varepsilon\f$, where \f$\varepsilon\f$ is the feasibility tolerance,
- bounds with absolute values exceeding \f$10^8\f$ are also ignored.

Only one round per constraint is applied per presolve pass. 

> Note that this presolver does not round the bounds for integer variables. Use it in combination with the `Presolvers::BoundRounding` presolver when bound rounding is desired.

\page lib_milp_bnb_node_type Using Your Own Node Type

\tableofcontents

The template parameter `NodeTypeT` in `BranchAndBound<NodeTypeT>` is algorithmic metadata attached to every
`Node<NodeTypeT>`. It does not by itself add a variable or constraint to the mathematical model, nor does merely
storing data in it change a node relaxation. The default, `DefaultNodeInfo`, records the result of solving a node:
status and termination reason, primal relaxation point, objective value and bound, sum of integrality
infeasibilities, and the variable and constraint branching decisions used by `DefaultNodeUpdator` to reconstruct
that node's relaxation.

Using a template parameter lets an advanced algorithm preserve additional per-node state—for example, information
needed by a specialized branching rule or node updater—while reusing the branch-and-bound engine. A custom type is
most conveniently derived from `DefaultNodeInfo`. It must be
default-constructible unless a root instance is supplied with `with_root_node_info`; it must provide `clone()`,
`create_child()`, and the static `create_updator(const Model&, Model&)` function expected by the optimizer. Methods
used by the chosen branching, selection, logging, and callback components must also remain available. Deriving from
`DefaultNodeInfo` preserves the interface required by the standard components.

```cpp
class MyNodeInfo : public DefaultNodeInfo {
    unsigned int m_tag = 0;
public:
    MyNodeInfo() = default;

    MyNodeInfo* clone() const override {
        return new MyNodeInfo(*this);
    }

    MyNodeInfo* create_child() const override {
        auto* result = new MyNodeInfo();
        result->m_tag = m_tag;
        return result;
    }

    static DefaultNodeUpdator<MyNodeInfo>* create_updator(
        const Model& source,
        Model& relaxation
    ) {
        return new DefaultNodeUpdator<MyNodeInfo>(source, relaxation);
    }
};
```

All node-dependent factories must use the same type.

```cpp
auto branch_and_bound = BranchAndBound<MyNodeInfo>()
    .with_node_optimizer(Gurobi::ContinuousRelaxation())
    .with_node_selection_rule(BestBound())
    .with_branching_rule(MostInfeasible());
```

Override `save` when extra data must be extracted from the solved relaxation, and provide a custom
`NodeUpdator<MyNodeInfo>` when preparing and clearing a node requires more than applying the default branching
decisions. `NodeWithCGInfo` is a current example: it extends `DefaultNodeInfo` to save active column-generation
columns while continuing to use `DefaultNodeUpdator`.

\page lib_milp_bnb_options Other Options

\tableofcontents

In addition to the required node optimizer, node-selection rule, and branching rule, the `BranchAndBound` factory
exposes callbacks, presolvers, root-node information, subtree exploration, and logging. General optimizer
parameters such as time limits, optimality gaps, tolerances, and whether logs are enabled come from
`OptimizerFactoryWithDefaultParameters` and use the usual `with_*` methods.

\section lib_milp_bnb_options_solution_pool Solution Pool Size

The runtime optimizer currently stores up to ten incumbents internally and exposes them through the standard model
solution-pool interface, such as `Model::get_n_solutions` and `Model::set_solution_index`. The value ten is an
implementation default; the `BranchAndBound` factory currently has no public option for changing it.

\section lib_milp_bnb_options_sub_tree Sub-Trees

`with_subtree_depth(depth)` controls how far each selected child's subtree is explored before its remaining active
nodes are merged back into the main search. The default depth is zero, meaning that each child root is solved before
control returns to the main node-selection rule. A depth of one also explores one further branching level, and so
on.

```cpp
branch_and_bound.with_subtree_depth(1);
```

The current factory does not expose a thread-count option; branch-and-bound execution therefore uses its internal
single-thread default.

\section lib_milp_bnb_options_logs Logging

Logging is enabled or disabled with the general `with_logs` parameter. Unless another logger is selected,
`BranchAndBound` uses `Logs::BranchAndBound::Info`, which prints the root relaxation and periodic search progress.
Its default progress interval is five seconds and node-optimizer logs are disabled after the root node.

Use `with_logger` to configure this logger explicitly. `with_frequency_in_seconds` changes the progress interval,
and `with_node_logs(true)` keeps the node optimizer's logs enabled after the root.

```cpp
branch_and_bound
    .with_logs(true)
    .with_logger(
        Logs::BranchAndBound::Info()
            .with_frequency_in_seconds(1)
            .with_node_logs(false)
    );
```

Each of `with_node_optimizer`, `with_branching_rule`, `with_node_selection_rule`, `with_logger`, and
`with_root_node_info` may be configured only once. In contrast, `add_callback` and `add_presolver` may be called
repeatedly to compose several callbacks or presolve operations.
