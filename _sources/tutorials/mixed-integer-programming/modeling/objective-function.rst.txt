.. _api_objective_functions:

Objective Functions
===================

.. contents:: Table of Contents
    :local:
    :depth: 2

Adding an Objective Function
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The objective function of an optimization model can specified by the ``Model::set_obj_expr`` method. The objective function is a linear expression of the decision variables.

Here is an example which sets the objective function to :math:`-x_0 + 2 x_1`;

.. code-block::

    model.set_obj_expr(-x_0 + 2 * x_1);

Accessing the Objective Function
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The objective function can be accessed using the ``Model::get_obj_expr`` method.
The objective sense can be accessed using the ``Model::get_obj_sense`` method.

If a given model has been solved, the best objective function value can be accessed using the ``Model::get_best_obj`` method.
The best bound can be accessed using the ``Model::get_best_bound`` method.

Changing the Optimization Sense
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To change the optimization sense, use the ``Model::set_obj_sense`` method.
The optimization sense can be either ``Minimize`` or ``Maximize``. For instance,

.. code:: cpp

    model.set_obj_sense(Maximize);

Alternatively, one can use the constructor of the ``Model`` class to set the optimization sense.

.. code:: cpp

    Env env;
    Model model(env, Maximize); // Creates a model for maximization
