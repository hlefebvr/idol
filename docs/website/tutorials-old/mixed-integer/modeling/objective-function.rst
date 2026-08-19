.. _api_objective_functions:

Objective Functions
===================

The objective function of an optimization model is a quadratic expression of the decision variables.
It is used to guide the optimization process by defining the quantity to be minimized or maximized.

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

Here is a list of methods to access the objective function of a given model:

- ``Model::get_obj_expr``: Returns the objective function of the model.
- ``Model::get_obj_sense``: Returns the optimization sense of the model.

If a given model has been solved, one can also query the best objective function value and the best bound.

- ``Model::get_best_obj``: Returns the best objective function value.
- ``Model::get_best_bound``: Returns the best bound.

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

.. warning::

    idol offers minimal support for maximization problems.
    We recommend to use minimization problems whenever possible.
