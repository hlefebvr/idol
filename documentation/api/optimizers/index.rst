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
the "real" optimizer in this case will be an instance of ``Optimizers::Gurobi`` which will be created when necessary.

This mechanism is what allows idol to create complex algorithms by combining optimizers together. Here is a second example
which creates a nested branch-and-price algorithm.

.. code-block:: cpp

    auto nested_branch_and_price =

        /* The overall algorithm is a branch-and-bound */
        BranchAndBound()

            /* Each node is solved with a Dantzig-Wolfe decomposition algorithm */
            .with_node_solver(

                /* The annotation "decomposition1" is used to automatically decompose the problem */
                DantzigWolfeDecomposition(decomposition1)

                    /* The master problem is solved using Gurobi */
                    .with_master_solver(Gurobi::ContinuousRelaxation())

                    /* Each pricing problem is solved by a (nested) branch-and-bound algorithm */
                    .with_pricing_solver(

                        BranchAndBound()

                            /* Each node is solved by a dantzig-wolfe decomposition algorithm */
                            .with_nodes_solver(

                                /* The annotation "decomposition2" is used to decompose the sub-problem again */
                                DantzigWolfeDecomposition(decomposition2)

                                    /* The master problem is solved using Mosek */
                                    .with_master_solver(Mosek::ContinuousRelaxation())

                                    /* The sub-problem is solved by a (nested) branch-and-bound algorithm [we could have used, e.g., Gurobi instead] */
                                    .with_pricing_solver(

                                        BranchAndBound()

                                            /* Each node is solved by GLPK */
                                            .with_node_solver(GLPK::ContinuousRelaxation())

                                            /* Variables are selected for branching using the most-infeasible rule */
                                            .with_branching_rule(MostInfeasible())

                                            /* Nodes are selected using the worst-bound rule */
                                            .with_node_selection_rule(WorstBound())

                                    )

                                    /* Branching constraints are applied to the pricing problem */
                                    .with_branching_on_master(false)
                            )
                            /* Variables are selected for branching using the most-infeasible rule */
                            .with_branching_rule(MostInfeasible())

                            /* Nodes are selected using the depth-first rule */
                            .with_node_selection_rule(DepthFirst())
                    )

                    /* Column generation is stabilized by dual-price smoothing */
                    .with_dual_price_smoothing_stabilization(.3)

                    /* Infeasible master problem are dealt with using Farkas pricing */
                    .with_farkas_pricing()

                    /* Branching constraints are applied to the master problem */
                    .with_branching_on_master()
            )
            /* Variables are selected for branching using the most-infeasible rule */
            .with_branching_rule(MostInfeasible())

            /* Nodes are selected using the best-bound rule */
            .with_node_selection_rule(BestBound()

            /* Only informational logs will be printed (in blue) */
            .with_log_level(Info, Blue)

            /* The algorithm will run with a time limit of 3600 */
            .with_time_limit(3600)
        )

    my_model.use(nested_branch_and_price);

Once an optimizer, or rather, an optimizer factory, has been given, the ``Model::optimize`` method can be called to actually
solve the model.

.. code-block::

    my_model.optimize();

Optimizer factories
-------------------

Typically, a user mostly works with optimizer factories rather than with optimizers. Optimizer factories are like
"building plans" for creating actual optimizers. They usually can be recognized by their ``with_*`` functions allowing
the user to customize this plan. For instance, the ``BranchAndBound`` optimizer factory has a method ``with_node_solver``
allowing the user to pass yet another optimizer factory which will be used to create optimizers for the branch-and-bound
nodes.

Table of contents
-----------------

.. toctree::
    :maxdepth: 4
    :glob:

    branch-and-bound/index
    column-generation/index
    glpk/index
    gurobi/index
    mosek/index
    callbacks/index
