.. _tutorial_column_generation:

Solving A Dantzig-Wolfe Reformulation
=====================================

In this tutorial, we will see how to implement a column generation algorithm to solve a continuous relaxation of the Generalized Assignment Problem (GAP).
This relaxation is obtained by relaxing the Dantzig-Wolfe reformulation of the GAP.

.. hint::

    This tutorial regards the `advanced topic` of Column Generation and Dantzig-Wolfe decomposition.
    Rudimentary notions in the following subjects are recommended:

    - `Column Generation algorithms <https://en.wikipedia.org/wiki/Column_generation>`_
    - `Dantzig-Wolfe decomposition <https://en.wikipedia.org/wiki/Dantzig%E2%80%93Wolfe_decomposition>`_
    - `Generalized Assignment Problem <https://en.wikipedia.org/wiki/Generalized_assignment_problem>`_.

A complete example which includes branching is available :ref:`here <example_gap_bap>`.

.. contents:: Table of Contents
    :local:
    :depth: 2

Problem Definition
------------------

Given a set of :math:`m` agents and :math:`n` jobs, the goal is to assign each job to exactly one agent in such a
way that the total cost is minimized, while respecting the capacity constraints of each agent.

Each agent :math:`i\in\{1,\dotsc,m\}` has a capacity :math:`C_i`.
Each job :math:`j\in\{1,\dotsc,n\}` has a resource consumption :math:`r_{ij}` and a cost :math:`c_{ij}` when assigned to agent :math:`i`.

The Natural Formulation
^^^^^^^^^^^^^^^^^^^^^^^

We model the GAP with the following binary linear program:

.. math::

    \begin{align*}
        \min_{x} \quad & \sum_{i=1}^m \sum_{j=1}^n c_{ij} x_{ij} \\
        \text{s.t.}  \quad & \sum_{j=1}^n r_{ij} x_{ij} \le C_i, && i=1,\dotsc,m, \\
                    & \sum_{i=1}^m x_{ij} = 1, && j=1,\dotsc,n, \\
                    & x_{ij} \in \{0,1\}, && i=1,\dotsc,m, j=1,\dotsc,n.
    \end{align*}

Here, variable :math:`x_{ij}` encodes the assignment decision and equals 1 if and only if task :math:`j` is assigned to
agent :math:`i`.

The Dantzig-Wolfe Reformulation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Let us enumerate the list of all feasible assignments, i.e., let

.. math::

    \{\bar x^e_{ij} \}_{e\in E} = \left\{ x \in \{ 0,1 \}^{mn} : \sum_{j=1}^n r_{ij}x_{ij} \le C_i \quad i=1,...,m \right\},

in which :math:`E` denotes a list for their indices.

The Dantzig-Wolfe reformulation of GAP reads

.. math::
    :label: eq-dantzig-wolfe-reformulation

    \min_{\lambda} \quad & \sum_{e\in E} \lambda_e\left( \sum_{i=1}^m\sum_{j=1}^n c_{ij}\bar x_{ij}^e \right) \\
    \textrm{s.t.} \quad & \sum_{e\in E} \lambda_e \left( \sum_{i=1}^m \bar x_{ij}^e \right) = 1 \quad j=1,...,n, \\
    & \sum_{e\in E} \lambda_e = 1, \\
    & \lambda_e \in \{ 0, 1 \} & \text{for all } e\in E.

Though this model contains an exponential number of variables, it can be solved efficiently using
Column Generation and Branch-and-price. In such a case, the pricing problem is a Knapsack Problem.

In this tutorial, we focus on the continuous relaxation of :math:numref:`eq-dantzig-wolfe-reformulation`.

Automatic Reformulation
-----------------------

The simplest way to solve a problem using Column Generation and idol is through its automatic reformulation feature.
To use this, one simply needs to give the *original space formulation* of the problem
and to indicate which constraints should be moved to the pricing problem (here, the knapsack constraints).

We will start by modeling the problem in its natural form and then indicate which constraints should be moved to the pricing problem.

The Natural Formulation
^^^^^^^^^^^^^^^^^^^^^^^

Before we start, we will use a GAP instance stored in a file. This file reads as follows.

.. literalinclude:: ../../../../examples/mixed-integer/assignment-bap.data.txt

To read an instance of the GAP, we need to include the header file located in :code:`idol/mixed-integer/problems/generalized-assignment-problem/GAP_Instance.h`.

Then, we can use the :code:`Problems::GAP::read_instance` function to read the instance file.

.. code-block:: cpp

    const auto instance =
            Problems::GAP::read_instance("assignment-bap.data.txt");

    const unsigned int n_agents = instance.n_agents();
    const unsigned int n_jobs = instance.n_jobs();

We are now ready to model our problem (for more details, refer to :ref:`this tutorial on modeling <mip_modeling>`)

.. code-block:: cpp

    // Create optimization environment
    Env env;

    // Create model
    Model model(env);

    // Create assignment variables (x_ij binaries)
    auto x = model.add_vars(Dim<2>(n_agents, n_jobs), 0., 1., Binary, "x");

    // Create knapsack constraints (i.e., capacity constraints)
    for (unsigned int i = 0 ; i < n_agents ; ++i) {
        model.add_ctr(idol_Sum(j, Range(n_jobs), instance.resource_consumption(i, j) * x[i][j]) <= instance.capacity(i), "capacity_" + std::to_string(i));
    }

    // Create assignment constraints
    for (unsigned int j = 0 ; j < n_jobs ; ++j) {
        model.add(idol_Sum(i, Range(n_agents), x[i][j]) == 1, "assignment_" + std::to_string(j));
    }

    // Set the objective function
    model.set_obj_expr(idol_Sum(i, Range(n_agents), idol_Sum(j, Range(n_jobs), instance.cost(i, j) * x[i][j])));

Giving Decomposition Instructions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

We are now at the crucial step of indicating which constraint should be moved to the pricing problem. In idol, this is done by using
*annotations*. Annotations are additional information associated to an optimization object (e.g., a constraint or a variable).
Note that annotations are global, i.e., they do not relate to a given optimization model.

Every annotation is formed with a template argument which is the value type of the annotation. Here, the Dantzig-Wolfe
decomposition expects an annotation with a value of type :code:`unsigned int` and which corresponds
to the sub-problem index to which the constraint will be moved to.

We create the annotation as follows.

.. code-block:: cpp

     Annotation<unsigned int> decomposition(env, "decomposition", MasterId);

Here, we pass three arguments to the constructor of :code:`Annotation<unsigned int>`. First, we pass the optimization
environment which will store the annotation. Then, a name is given to the annotation: here, "decomposition".
Finally, a default value is given and is set to :code:`MasterId`. This will tell idol that constraints which have not been annotated
should remain in the master problem.

Now, observe how the annotation is applied to the capacity constraints.

.. code:: cpp

    for (unsigned int i = 0 ; i < n_agents ; ++i) {
        auto capacity = model.add_ctr(idol_Sum(j, Range(n_jobs), instance.resource_consumption(i, j) * x[i][j]) <= instance.capacity(i), "capacity_" + std::to_string(i));
        capacity.set(decomposition, i); // <== Annotating the capacity constraint
    }

Here, the first capacity constraint is moved to the first pricing problem (id: 0),
the second constraint to the second pricing problem (id: 1), and so on.

Note that another decomposition would be materialized as follows.

.. code:: cpp

    for (unsigned int i = 0 ; i < n_agents ; ++i) {
        Ctr capacity = model.add_ctr(idol_Sum(j, Range(n_jobs), instance.resource_consumption(i, j) * x[i][j]) <= instance.capacity(i), "capacity_" + std::to_string(i));
        capacity.set(decomposition, 0); // <== Annotating the capacity constraint
    }

Here, all the knapsack constraints would be moved to the same pricing problem (id: 0).

Creating the Column Generation Algorithm
----------------------------------------

Now that the desired decomposition has been specified, we can set the desired optimizer to solve our model.
Here, we want to solve our the continuous relaxation of the Dantzig-Wolfe reformulation using column generation.

To begin with, we need to give some instructions about how each sub-problem will be solved. In other words, we need
to specify the optimizer(s) used for pricing during the column generation process. This is done by first creating a
:code:`DantzigWolfe::SubProblem` object.

.. code:: cpp

    const auto sub_problem_specifications = DantzigWolfe::SubProblem()
                                                .add_optimizer(Gurobi());

Then, we can create the column generation algorithm (factory) in the following way.

.. code:: cpp

    const auto column_generation = DantzigWolfeDecomposition(decomposition)
            .with_master_optimizer(Gurobi::ContinuousRelaxation())
            .with_default_sub_problem_spec(sub_problem_specifications);

We can now tell idol to use this algorithm for solving our model by using the :code:`Model::use` method.

.. code:: cpp

    model.use(column_generation);

Solving
-------

As usual, one can simply call the :code:`Model::optimize` method to solve the problem.

.. code:: cpp

    model.optimize();


That's it! The problem is being solved by column generation, and possibly branching on fractional variables.

The rest remains unchanged and one can retrieve the solution
through the usual methods such as :code:`Model::get_status` and :code:`Model::get_var_primal`.
