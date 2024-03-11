.. _mip_optimizers:

.. role:: cpp(code)
   :language: cpp

Optimizers
==========

This page discusses the concept of optimizers and optimizer factories in idol.
We will discuss how to set up an optimizer for solving a model and how to access the solution of the model.

In idol, an optimizer is a class that is responsible for solving a given optimization problem.
It is created by an optimizer factory, which is a class that is responsible for assembling and creating optimizers.

We start with an example. Consider the following code, which models a simple knapsack problem:

.. code:: cpp

    Env env;
    Model model(env, Maximize);
    const auto x = model.add_vars(Dim<1>(3), 0, 1, Binary, "x");
    model.add_ctr(x[0] + 2 * x[1] + 3 * x[2] <= 5);
    model.set_obj_expr(x[0] + x[1] + x[2]);

The code creates a model with three binary variables, and adds a constraint and an objective function to it.
Currently, the model does not have an optimizer, so it cannot be solved.
Calling :code:`model.optimize()` will result in an exception being thrown.

To set up an optimizer, we need to call the :code:`Model::use` method, which takes an optimizer factory as an argument.
For instance, let us use the :ref:`GLPK <api_GLPK>` optimizer factory, which creates an optimizer using GLPK to solve our model:

.. code:: cpp

    model.use(GLPK());

Here, we assume that idol was linked with the GLPK library during installation.
Now, calling :code:`model.optimize()` will cause the optimizer factory to create an optimizer, which will solve the model.
Since we are using the :ref:`GLPK <api_GLPK>` optimizer factory, an instance of :ref:`Optimizers::GLPK <api_Optimizers_GLPK>`
will be created and used to solve the model.

Note that we can modify the model after setting up the optimizer. The optimizer will be updated accordingly. For instance.

.. code::

    model.add_ctr(x[0] + x[1] <= 1); // Add a new constraint
    model.optimize(); // The optimizer will solve the model with the new constraint

Idol provides several optimizer factories, each of which creates an optimizer using a different solvers or implementing different algorithms.
For instance, the :ref:`Gurobi <api_Gurobi>` optimizer factory creates an optimizer using the Gurobi solver,
while the :ref:`BranchAndBound <api_BranchAndBound>` optimizer factory creates an optimizer implementing a branch-and-bound algorithm.

Typically, users only work with optimizer factories, and do not need to create optimizers directly.
Passing arguments to an optimizer is always possible at creation time through the optimizer factory.
For instance, here is a small example of how to pass a time limit to the GLPK optimizer:

.. code::

    model.use(GLPK().with_time_limit(10)); // add time limit of 10 seconds

As it can be seen, arguments are given by calling the :code:`with_` method of the optimizer factory, followed by the name of the argument.


In some cases, it is useful to nest optimizers to, e.g., to use an optimizer to solve a sub-problem of another optimization problem.
The process of nesting optimizers is straightforward and follows the same pattern as setting up an optimizer for a model.

For instance, consider the following code, which implements a simple branch-and-bound algorithm to solve our knapsack problem:

.. code:: cpp

    model.use(
        /* The overall algorithm is a branch-and-bound */
        BranchAndBound()

            /* Each node is solved by the continuous relaxation of the problem, with GLPK */
            .with_node_solver(GLPK::ContinuousRelaxation())

            /* Variables are selected for branching using the most-infeasible rule */
            .with_branching_rule(MostInfeasible())

            /* Nodes are selected using the best-bound rule */
            .with_node_selection_rule(BestBound())

            /* Turn on logs */
            .with_logs(true)

            /* The algorithm will run with a time limit of 3600 */
            .with_time_limit(3600)
        )
    );

In this example, we create a branch-and-bound algorithm to solve the model, and we use GLPK to solve the continuous relaxation of the problem at each node.
Also note that we can nest optimizers as many times as we want with no restrictions.

To simplify the writing of complex optimizer factories, it may be useful to use temporary variables to store intermediate results.
For instance.

.. code:: cpp

    const auto continuous_relaxation = GLPK::ContinuousRelaxation().with_logs(true);
    const auto branching_rule = MostInfeasible();
    const auto node_selection_rule = BestBound();

    const auto branch_and_bound = BranchAndBound()
        .with_node_solver(continuous_relaxation)
        .with_branching_rule(branching_rule)
        .with_node_selection_rule(node_selection_rule)
        .with_logs(true)
        .with_time_limit(3600);

    model.use(branch_and_bound);

.. toctree::
    :maxdepth: 1
    :glob:

    solving
    access-optimizer
    universal-callbacks
    create-universal-callback
    create-optimizer
