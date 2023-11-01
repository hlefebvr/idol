.. _api_optimizers:

Optimizers
==========

Introduction
------------

Optimizers denote the algorithms which are being used by idol to solve a given ``Model``.
They are set by using the ``Model::use`` method, as done in the following example.

.. code-block:: cpp

    Env env;

    Model my_model(env);

    my_model.use( Gurobi() ); // setting the optimizer of my_model to Gurobi()

Here, ``Gurobi`` is actually an ``OptimizerFactory`` which will eventually create an optimizer to solve our model. Indeed,
the "real" optimizer in this case will be an instance of ``Optimizers::Gurobi`` which will be created just in time,
when necessary.

Typically, a user mostly works with optimizer factories rather than with optimizers. Optimizer factories are like
"building plans" for creating actual optimizers. They usually can be recognized by their ``with_*`` functions allowing
the user to customize this plan. For instance, the ``BranchAndBound`` optimizer factory has a method ``with_node_optimizer``
allowing the user to pass yet another optimizer factory which will be used to create optimizers for the branch-and-bound
nodes.

This mechanism is what allows idol to create complex algorithms by combining optimizers together. Here is a second example
which creates a branch-and-bound algorithm in which each node is solved by Gurobi.

.. code-block:: cpp

    auto branch_and_bound =

        /* The overall algorithm is a branch-and-bound */
        BranchAndBound()

            /* Each node is solved with a Dantzig-Wolfe decomposition algorithm */
            .with_node_solver(Gurobi::ContinuousRelaxation())

            /* Variables are selected for branching using the most-infeasible rule */
            .with_branching_rule(MostInfeasible())

            /* Nodes are selected using the best-bound rule */
            .with_node_selection_rule(BestBound())

            /* Only informational logs will be printed (in blue) */
            .with_log_level(Info, Blue)

            /* The algorithm will run with a time limit of 3600 */
            .with_time_limit(3600)
        )

    my_model.use(branch_and_bound);

Once an optimizer, or rather, an optimizer factory, has been given, the ``Model::optimize`` method can be called to actually
solve the model.

.. code-block::

    my_model.optimize();

Table of contents
-----------------

.. toctree::
    :maxdepth: 4
    :glob:

    branch-and-bound/index
    column-generation/index
    wrappers/index
    callbacks/index
