.. _tutorial_ccg:

Writing a Column-and-Constraint-Generation Algorithm
====================================================

In this tutorial, we will see how to write a column-and-constraint-generation to solve a two-stage robust problem.

To this end, we will assume that you have your two-stage robust problem modeled already in idol. In particular,
we consider that you have

1. :code:`(idol::Model) model` which is the deterministic model of the problem in which the uncertain data are seen as parameters.

2. :code:`(idol::Model) uncertainty_set` which is the uncertainty set of the robust problem.

3. :code:`(idol::Robust::StageDescription) stages` which stores the assignments of variables and constraints to each stage.

If you do not know what these are, please refer to the tutorial on :ref:`how to model a two-stage robust problem <modeling_two_stage_robust_problem>`.

Then, you can solve this two-stage robust problem using a column-and-constraint-generation algorithm as follows:

.. code::

     model.use(
            Robust::ColumnAndConstraintGeneration(stages, uncertainty_set)
                    .with_master_optimizer(Gurobi())
                    .with_separator(Robust::CCGSeparators::Bilevel())
                    .with_logs(true)
    );

    model.optimize();

    std::cout << save_primal(model) << std::endl;

Notice that the optimizer is attached to the deterministic model and that both the uncertainty set and the stages are passed as arguments.
An optimizer to solve the master problem is necessary and will be called when needed. Here, we use the Gurobi optimizer.

Most importantly, it is also necessary to specify a separator. The separator is a sub-routine of the CCG algorithm
which solves the separation problem, i.e., it finds the worst-case scenario for a given solution to the master problem.
In this example, we use the Bilevel separator which calls the MibS bilevel optimization solver at each iteration.

Finally, the method :code:`optimize` is called to solve the problem and the solution is printed.

.. admonition::

    Here, we have assumed that the problem does not satisfy the complete recourse assumption, i.e., it is not known if
    :math:`\forall x\in X, \forall\xi\in\Xi, \exists y\in Y(x,\xi)`. If, to the contrary, this assumption holds,
    it can be communicated to the solver by calling the method :code:`with_complete_recourse(true)`. By doing this,
    the CCG algorithms will not solve the feasibility separation problem which is most likely to result in a faster convergence.

