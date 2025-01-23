Getting Access to The Underlying Optimizer
==========================================

In some advanced situations, you may want to access the underlying optimizer of an optimization model.
For instance, this could be the case if you want to get optimizer-specific information which are not exposed by the :code:`Model` interface.

In this case, you can use the :code:`Model::optimizer` method. Note that this method
will return a reference of type :code:`Optimizer&`.

To access optimizer-specific routines, you will need to cast the reference to the appropriate optimizer type.
This is made easier  by the :code:`Optimizer::as` and :code:`Optimizer::is` methods, which allow you to cast the optimizer to a specific type or check if it is of a specific type.

For instance, the following code shows how to access the underlying Gurobi optimizer of model.

.. code::

    Env env;
    Model model(env);
    auto x = model.add_var(0.0, 1.0, Continuous, -1, "x");

    model.use(Gurobi());
    model.optimize();

    if (!model.optimizer().is<Optimizers::Gurobi>()) {
        throw Exception("The optimizer is not Gurobi");
    }

    auto& gurobi_optimizer = model.optimizer().as<Optimizers::Gurobi>();

    // Access the optimizer-specific routines
    GRBVar& var = gurobi_optimizer[x];

.. warning::

    A common mistake is to try to cast the optimizer to the wrong type. In particular, to cast optimizer to its
    optimizer factory type (e.g., :code:`Gurobi` instead of :code:`Optimizers::Gurobi`).
    If so, this will result in a runtime error.
