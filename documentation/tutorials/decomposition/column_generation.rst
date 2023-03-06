.. _basics_colgen:

.. role:: cpp(code)
   :language: cpp

Using the Branch-and-Price solver
=================================

In this section, we will show how to use the Branch-and-Price solver to solve the *Generalized Assignment Problem* (GAP)
using an external solver to solve each subproblem.

This tutorial regards the "advanced topic" of Column Generation and Dantzig-Wolfe decomposition, some prerequisites are needed:

- Readers who are not familiar with Column Generation or Branch-and-Price may refer to the `Column Generation wikipedia page <https://en.wikipedia.org/wiki/Column_generation>`_
- Readers who are not familiar with Dantzig-Wolfe decomposition may refer to the `Dantzig-Wolfe decomposition wikipedia page <https://en.wikipedia.org/wiki/Dantzig%E2%80%93Wolfe_decomposition>`_
- Readers who are not familiar with GAP may refer to the `Generalized Assignment Problem Wikipedia page <https://en.wikipedia.org/wiki/Generalized_assignment_problem>`_.

Readers interested in computational aspects regarding idol may refer to the `Benchmark on Generalized Assignemnt Problem <https://hlefebvr.github.io/idol_benchmark/GAP.render.html>`_.
This includes a comparison with `Coluna.jl <https://github.com/atoptima/Coluna.jl>`_.

Mathematical models
-------------------

In this section, we assume that the reader is familiar with GAP.
Let :math:`m` be a given of agents and let :math:`n` be a set of tasks to perform. Let :math:`c_{ij}` be the cost for
assigning task :math:`j` to agent :math:`i`, :math:`w_{ij}` be the resource consumption of task :math:`j` when performed
by agent :math:`i` and let :math:`t_i` be the resource capacity of agent :math:`i`.

Direct model
^^^^^^^^^^^^

The Generalized Assignment Problem (GAP) can be modeled as

.. math::

    \min \ & \sum_{i=1}^m\sum_{j=1}^n c_{ij} x_{ij} \\
    \textrm{s.t. } & \sum_{j=1}^n w_{ij} x_{ij} \le t_i & i=1,...,m \\
    & \sum_{i=1}^m x_{ij} = 1 & j = 1,...,n \\
    & x_{ij}\in\{0,1\} & i=1,...,m, j=1,...,n

Here, variable :math:`x_{ij}` encodes the assignment decision and equals 1 if and only if task :math:`j` is assigned to
agent :math:`i`.

Dantzig-Wolfe reformulation
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Let us enumerate the list of all feasible assignments, i.e., let

.. math::

    \{\bar x^e_{ij} \}_{e\in E} = \{ x \in \{ 0,1 \}^{mn} : \sum_{j=1}^n w_{ij}x_{ij} \le t_i \quad i=1,...,m \},

where :math:`E` denotes a list for their indices. The Dantzig-Wolfe reformulation of GAP reads

.. math::

    \min \ & \sum_{e\in E} \lambda_e\left( \sum_{i=1}^m\sum_{j=1}^n c_{ij}\bar x_{ij}^e \right) \\
    \textrm{s.t. } & \sum_{e\in E} \lambda_e \left( \sum_{i=1}^m \bar x_{ij}^e \right) = 1 & j=1,...,n \\
    & \sum_{e\in E} \lambda_e = 1 \\
    & \lambda_e \in \{ 0, 1 \} & \forall e\in E

Though this model contains an exponential number of variables (i.e., columns) it can be solved efficiently using
Column Generation and Branch-and-price. In such case, the pricing problem is a Knapsack Problem.

Automatic reformulation in Idol
-------------------------------

The simplest way to solve a problem using column generation and idol is through its automatic reformulation feature.
To use this, one simply needs to give the *original space* formulation of the problem (here, the *so-called* direct model)
and to indicate which constraints should be moved to the pricing problem (here, the knapsack constraints).

Implementing the direct model with Idol
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To read an instance for GAP, we first need to include the header file located in :code:`"idol/problems/generalized-assignment-problem/GAP_Instance.h"`.
This will allow us to use Idol's instance parser for GAP.
Assuming that the instance file is named :code:`instance.txt`, it can be loaded as follows.

.. code-block:: cpp

    auto instance = Problems::GAP::read_instance("instance.txt");

We are now ready to model our problem (for more details, refer to :ref:`this tutorial on modeling <basics_first_model>`)

.. code-block:: cpp

    // Create optimization environment
    Env env;

    // Create model
    Model model(env);

    // Create assignment variables (x_ij binaries)
    auto x = Var::array(env, Dim<2>(n_agents, n_jobs), 0., 1., Binary, "x");

    // Add variables to the model
    model.add_array<Var, 2>(x);

    // Create knapsack constraints (i.e., capacity constraints)
    for (unsigned int i = 0 ; i < n_agents ; ++i) {
        Ctr capacity(env, idol_Sum(j, Range(n_jobs), instance.resource_consumption(i, j) * x[i][j]) <= instance.capacity(i), "capacity_" + std::to_string(i));
        model.add(capacity);
    }

    // Create assignment constraints
    for (unsigned int j = 0 ; j < n_jobs ; ++j) {
        Ctr assignment(env, idol_Sum(i, Range(n_agents), x[i][j]) == 1, "assignment_" + std::to_string(j));
        model.add(assignment);
    }

    // Set the objective function
    model.set(Attr::Obj::Expr, idol_Sum(i, Range(n_agents), idol_Sum(j, Range(n_jobs), instance.cost(i, j) * x[i][j])));

Giving decomposition instructions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

We are now at the crucial step of indicating which constraint should be moved to the pricing problem. In Idol, this is done by using
*annotations*. Annotations are additional informations associated to an optimization object (e.g., a constraint or a variable).
Note that annotations are global, i.e., they do not relate to a given optimization model.

Every annotation is formed with two template arguments: an optimization object type and a value type. Here, we want to add
annotations to constraints, thus, the optimization object type must be :code:`Ctr`. The value type can typically be any desired
type. Here, however, the Dantzig-Wolfe decomposition needs an annotation corresponding to an :code:`unsigned int`. Thus, we create
the annotation as follows.

.. code-block:: cpp

     Annotation<Ctr, unsigned int> decomposition(env, "decomposition", MasterId);

Here, we pass three arguments to the constructor of :code:`Annotation<Ctr, unsigned int>`. First, we pass the optimization
environment which will store the annotation. Then, a name (mandatory) is given to the annotation, here, "decomposition".
Finally, a default value is given and equals "MasterId". This will tell Idol that constraints which have not been annotated
should remain in the master problem.

Now, observe how the annotation is applied to the capacity constraints.

.. code:: cpp

    for (unsigned int i = 0 ; i < n_agents ; ++i) {
        Ctr capacity(env, idol_Sum(j, Range(n_jobs), instance.resource_consumption(i, j) * x[i][j]) <= instance.capacity(i), "capacity_" + std::to_string(i));
        capacity.set(decomposition, i); // <-- Annotating the capacity constraint
        model.add(capacity);
    }

Here, the first capacity constraint is moved to the first pricing problem (id: 0),
the second constraint to the second pricing problem (id: 1), and so on.

Note that another decomposition would be materialized as follows.


.. code:: cpp

    for (unsigned int i = 0 ; i < n_agents ; ++i) {
        Ctr capacity(env, idol_Sum(j, Range(n_jobs), instance.resource_consumption(i, j) * x[i][j]) <= instance.capacity(i), "capacity_" + std::to_string(i));
        capacity.set(decomposition, 0); // <-- Annotating the capacity constraint
        model.add(capacity);
    }

Here, all the knapsack constraints are moved to the same pricing problem (id: 0).

Decomposing and solving the model
---------------------------------

Now that the desired decomposition has been specified, we can define the optimizer for our model and request its decomposition.
This is done in the exact same way as classically done with any other solver, using the :code:`set_optimizer` function.

.. code:: cpp

    Idol::set_optimizer<BranchAndPrice<GLPK>>(model, decomposition);

Here, we pass the direct model as argument with the desired decomposition. Then, one can simply call the :code:`optimize` method as follows.

.. code:: cpp

    model.optimize();

That's it! The problem is being solved by column generation, and possibly branching on fractional variables. Note that it
is possible to obtain logs as follows.

.. code:: cpp

    Logs::set_level<BranchAndBound>(Debug); // Set debug log level for BranchAndBound algorithms
    Logs::set_color<BranchAndBound>(Blue); // Set output color to blue for BranchAndBound algorithms

    Logs::set_level<ColumnGeneration>(Debug); // Set debug log level for ColumnGeneration algorithms
    Logs::set_color<ColumnGeneration>(Yellow); // Set output color to blue for ColumnGeneration algorithms

Note that the rest remains unchanged and one can use :code:`model.get(Attr::Solution::Status)` to get the optimization status
or use :code:`save(model, Attr::Solution::Primal)` to save the primal solution of the problem.

Parameters
----------

We end this tutorial by discussing some parameters which can be used to tweak the execution of the branch-and-price algorithm.

- :code:`(int) Param::ColumnGeneration::LogFrequency` controls the frequency for log outputs, e.g., when set to 10, a log
  regarding the solution of the master and pricing problems is written every 10 iterations;
- :code:`(bool) Param::ColumnGeneration::BranchingOnMaster` controls where the branching is applied, e.g., when set to 1,
  branching is applied to the master problem (0 applies it to the pricing problems);
- :code:`(bool) Param::ColumnGeneration::FarkasPricing` controls how infeasible master problems are handled, e.g., when set to
  1, a farkas certificate is used to generate new columns or proving infeasibility of the original problem (0 introduces artificial
  variables with high costs similar to Phase I Simplex);
- :code:`(double) Param::ColumnGeneration::ArtificialVarCost` controls the value for the artificial variables (when :code:`Param::ColumnGeneration::FarkasPricing`
  is set to 0);
- :code:`(int) Param::ColumnGeneration::CleanUpThreshold` controls the maximum number of columns present in the column pool
  before the pool is cleaned up;
- :code:`(double) Param::ColumnGeneration::CleanUpRatio` controls the amount of columns which are removed from the pool during
  clean up (note that the oldest columns are removed first), e.g., when set to .75, 25% of the columns are removed from the pool;
- :code:`(double) Param::ColumnGeneration::SmoothingFactor` controls the stabilization factor for dual price smoothing;
- :code:`(bool) Param::BranchAndPrice::IntegerMasterHeuristic` controls the activation of a primal heuristic for branch-and-price where
  integrality requirements are imposed on Dantzig-Wolfe coefficients when the solution is fractional.

For example, we may set the stabilization factor controlling dual price smoothing as follwos:

.. code:: cpp

    model.set(Param::ColumnGeneration::SmoothingFactor, .3); // (must be between 0 and 1)

