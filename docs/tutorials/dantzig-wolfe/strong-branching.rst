.. _tutorials_dantzig_wolfe_strong_branching:

.. role:: cpp(code)
   :language: cpp

Strong Branching
================

Hey, you've implemented your first Branch-and-Price algorithm and are curious about how to implement strong branching in
idol? Then you are at the right place! If not, make sure to check the
`Generalized Assignment Problem tutorial <_tutorials_dantzig_wolfe>`_.

Introduction
------------

If you've landed here, you should already know what strong branching is. To be sure though, let's give a small introduction.

First of all, *Strong Branching* is variable-branching rule. Thus, it is used inside a branch-and-bound algorithm to
select a variable for branching. In particular, among a set of branching candidates whose value must be integer, one must
decide which variable with fractional value in the current relaxation to choose for creating
the child nodes, i.e., for branching.

The most common rule is the so-called *Most-Infeasible* rule, which selects a variable whose fractional
part is closest to `0.5`. Unfortunately, this rule performs badly in practice. Most importantly, if solving a node is
computationally hard, it makes sense to spend some time in carefully choosing the variable to branch. This is typically
the case when nodes are solved using Column Generation, and that's why we are here.

The idea of Strong Branching is to evaluate the effect of branching on a variable before branching actually happens.

To be more clear, let :math:`C` be a set of indices for branching candidates at a given node, i.e., for each :math:`j\in C`,
:math:`x_j` is an integer variable in the original problem but with :math:`x_j^*\notin\mathbb Z` at the current node.
Strong Branching tries to evaluate the effect of applying branching constraints :math:`x_j \le \lfloor x_j^* \rfloor` and
:math:`x_j \ge \lceil x_j^* \rceil` to the current node. To do so, it solves, before branching happens, and for each :math:`j\in C`,
both the left and right child node. Let :math:`z_i^\le` and :math:`z_i^\ge` denote the (optimal) value of the
left and right nodes if branching is performed on :math:`x_j`. The "effect of branching on :math:`x_j`" is then estimated by
computing a score, noted :math:`\text{Score}(j)`, based on :math:`z_j^\le` and :math:`z_j^\ge`. Then Strong Branching
selects the variable with an index :math:`j^*` such that (for minimization problems)

.. math::

    j^* \in \text{argmax}\{ \text{Score}(j) : j\in C \}.

Empirically, Strong Branching is known to produce substantially smaller Branch-and-Bound trees compared to other
branching rules. Unfortunately, computing :math:`\text{Score}(j)` typically requires a lot of time. To avoid this,
several techniques have been designed such as *Restricted Strong Branching* and *Strong Branching with Phases* (see below).

Scoring Functions
^^^^^^^^^^^^^^^^^

Two common scoring functions :math:`\text{Score}(j)` are found in the literature. The *linear* formula

.. math::

    \text{LinearScore}(j) := (1 - \mu) \min(\Delta_j^\le, \Delta_j^\ge) + \mu \max(\Delta_j^\le, \Delta_j^\ge),

and the *product* formula

.. math::

    \text{ProductScore}(j) := \max(\Delta_j^\le, \varepsilon) \max(\Delta_j^\ge, \varepsilon),

in which :math:`\Delta_j^\le := z_j^\le - z^*` and :math:`\Delta_j^\ge := z_j^\ge - z^*` with :math:`z^*` denoting the
(optimal) value of the current node. Parameters :math:`\mu\in[0,1]` and :math:`\varepsilon > 0` are given.
In idol, :math:`\mu = 1/6` and :math:`\varepsilon = 10^{-6}`.

Full Strong Branching
^^^^^^^^^^^^^^^^^^^^^

*Full Strong Branching* denotes the standard Strong Branching rule which solves all :math:`2|C|` nodes at each branching
decision. The drawback of this approach is that it may take a lot of time to solve all these sub-problems before branching
actually happens.

Restricted Strong Branching
^^^^^^^^^^^^^^^^^^^^^^^^^^^

*Restricted Strong Branching* is an attempt to reduce the computational burden of Full Strong Branching. The idea is to
consider only a maximum of :math:`K` branching candidates at each branching decision instead of the whole set :math:`C`.
Thus, :math:`C` is replaced by a smaller set :math:`R\subseteq C` such that :math:`|R| = K` with :math:`K` fixed.
The "restricted branching candidate set" :math:`R` is created by taking the :math:`K` first variables selected by, yet
another, branching rule, e.g., the most-infeasible rule.

Strong Branching with Look Ahead
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

*Strong Branching with Look Ahead* is similar to *Restricted Strong Branching* yet differs from it by not specifying a
fixed size for the "restricted branching candidate set" :math:`R`. Instead, it considers a look ahead parameter, noted
:math:`L`, and applies the Full Strong Branching rule. However, if the branching candidate does not change after :math:`L`
iterations, the algorithm stops and the current branching candidate is returned.

Strong Branching with Phases
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

*Strong Branching with Phases* is a combination of the above three approaches which applies different schemes depending
on the level of the current node in the Branch-and-Bound tree. Additionally, it allows to solve each node only approximately
by, e.g., imposing a maximum number of iterations for the underlying Column Generation algorithm.

Implementation
--------------

Full Strong Branching
^^^^^^^^^^^^^^^^^^^^^

Enough talking, let's implement strong branching. To do so, we will assume that you followed the previous tutorial
on Generalized Assignment Problem and Branch-and-Price. More specifically, we will assume that you have a variable
:code:`model` of type :code:`Model` which has a decomposable structure specified by the annotation :code:`(Annotation<Ctr, unsigned int>) decomposition`.

Recall that the Branch-and-Price algorithm is created by the following code.

.. code:: cpp

    const auto column_generation =
        DantzigWolfeDecomposition(decomposition)
            .with_master_optimizer(Gurobi::ContinuousRelaxation())
            .with_default_sub_problem_spec(
                DantzigWolfe::SubProblem()
                    .add_optimizer(Gurobi())
            );

You should be already familiar with this. What is new is the way in which we create our Branch-and-Bound algorithm. In
particular, we will use the :code:`StrongBranching` class to define our branching rule. We can, for instance, simply
declare

.. code:: cpp

    const auto branching_rule =
        StrongBranching();

which will create a new Full Strong Branching rule. Just like any other branching rule, it can be used by calling the
:code:`BranchAndBound::with_branching_rule` method.

.. code:: cpp

    const auto branch_and_bound =
        BranchAndBound()
            .with_branching_rule(branching_rule)
            .with_node_selection_strategy(BestBound());

Then, we can build a Branch-and-Price algorithm and solve our problem as follows.

.. code:: cpp

    const auto branch_and_price = branch_and_bound + column_generation;

    model.use(branch_and_price);

    model.optimize();

Pretty easy no?

But wait! Here, we only implemented Full Strong Branching which, as we saw, is not computationally convenient...
Let's see how to implemented Restricted Strong Branching!

Restricted Strong Branching
^^^^^^^^^^^^^^^^^^^^^^^^^^^

To implement Restricted Branching, one simply needs to call the :code:`StrongBranching::with_max_n_variables` method.
This is done as follows.

.. code:: cpp

    const auto branching_rule =
            StrongBranching()
                .with_max_n_variables(50);

Here, we set the maximum number of considered variables equal to :math:`K = 50`.

Phases
^^^^^^

In this section, we will discuss how to implement phases with the strong branching rule. This is done using the
:code:`StrongBranching::add_phase` method. This method takes three arguments: a phase type, which is used to indicate
how each node should be solved, e.g., with some iteration limit, a maximum number of variables to consider, for restricted
strong branching, and a maximum depth, used to trigger the phase based on the level of the current node in the Branch-and-Bound
tree.

Here is an instance of strong branching with phases which, for nodes whose level is below or equal to 3, applies Full
Strong Branching, then switches to Restricted Strong Branching with :math:`K = 30` and which solves nodes with an iteration
limit of 20.

.. code:: cpp

    const auto branching_rule =
                StrongBranching()
                    .add_phase(StrongBranchingPhases::WithNodeOptimizer(), std::numeric_limits<unsigned int>::max(), 3)
                    .add_phase(StrongBranchingPhases::WithIterationLimit(20), 30, std::numeric_limits<unsigned int>::max());

Observe how we used :code:`std::numeric_limits<unsigned int>::max()` to remove restrictions on the number of
considered variables and on the maximum depth for the final phase. Note that, by default, if no phase is triggered for a
given depth, e.g., because it was not specified, Full Strong Branching is applied. Here, however, we make sure that the
second phase is always triggered.

Conclusion
----------

Strong Branching is a possible approach to try to reduce the computation time of a Branch-and-Price algorithm.
Another approach, which can be complementary, is stabilization. This is the topic of the next tutorial, see you there!
