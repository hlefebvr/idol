Writing A Branch-and-Price Algorithm
====================================

This tutorial is a follow-up to the `Dantzig-Wolfe decomposition tutorial <tutorial_column_generation>`_.
In this tutorial, we will see how to implement a Branch-and-Price algorithm in idol to, not only solve
the continuous relaxation of the Generalized Assignment Problem (GAP), but also handle the integrality constraints.

.. contents:: Table of Contents
    :local:
    :depth: 2

Prerequisites
-------------

We will assume that you have already defined your model as well as the column generation optimizer factory.
If you haven't done so, we recommend you to read the `Dantzig-Wolfe decomposition tutorial <tutorial_column_generation>`_.

Hence, you should already have something like this:

.. code::


    const auto sub_problem_specifications = DantzigWolfe::SubProblem()
                                                .add_optimizer(Gurobi());

    const auto column_generation = DantzigWolfeDecomposition(decomposition)
            .with_master_optimizer(Gurobi::ContinuousRelaxation())
            .with_default_sub_problem_spec(sub_problem_specifications);

Moreover, we will assume that your model is called :code:`model` and that the decomposition annotation is called :code:`decomposition`.

Creating the Branch-and-Price Algorithm
---------------------------------------

Our remaining task is to embed our column generation routine inside of a Branch-and-Bound algorithm.

Hence, we create a branch-and-bound algorithm as follows.

.. code:: cpp

    const auto branch_and_bound = BranchAndBound()

        /* Variables are selected for branching using
           the most-infeasible rule */
        .with_branching_rule(MostInfeasible())

        /* Nodes are selected using the best-bound rule */
        .with_node_selection_rule(BestBound())

        /* The algorithm will run with a time limit of 3600 */
        .with_time_limit(3600)

    );

Here, variables are selected for branching using the most-infeasible rule, and nodes are selected using the best-bound rule.
We also set a time limit of one hour.

The column generation algorithm can easily be combined with the branch-and-bound algorithm using the :code:`BranchAndBound::with_node_optimizer`
method, or by simply adding the two algorithms together.

For instance, the following line creates a branch-and-price algorithm.

.. code:: cpp

    model.use(branch_and_bound + column_generation);

Solving
-------

As usual, one can simply call the :code:`Model::optimize` method to solve the problem.

.. code:: cpp

    model.optimize();


That's it! The problem is being now solved by a branch-and-price and the integrality constraints are handled automatically!

The rest remains unchanged and one can retrieve the solution
through the usual methods such as :code:`Model::get_status` and :code:`Model::get_var_primal`.

