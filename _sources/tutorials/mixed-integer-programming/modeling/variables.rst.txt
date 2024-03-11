.. _api_variables:

Variables
---------

Variables are the decision variables of an optimization problem. They are the objects that we want to find the best
values for, with respect to some objective function given the specified constraints.

.. contents:: Table of Contents
    :local:
    :depth: 2

Creating Variables
^^^^^^^^^^^^^^^^^^^

There are mainly two ways to create variables in idol. The first one is to use the :ref:`Var <api_Var>` class directly, and the
second one is to use the :cpp:`Model::add_var` method.

Using the :cpp:`Var` class directly creates a variable that is not associated to any model. We will there need to add it
explicitly to a model. This is done by calling the :cpp:`Model::add` method. See for instance the following code which
creates a continuous variable :math:`x`, with bounds :math:`[0,\infty)`, and adds it to the model.

.. code:: cpp

    Var x(env, 0., Inf, Continuous, "x");

    model.add(x); // Variable x is added to the model

Note that a variable may belong to several models at the same time. Each version of the variable in each model is then independent.
For instance, one could do the following.

.. code:: cpp

    Var x(env, 0., Inf, Continuous, "x");

    model.add(x);

    Model model2(env);
    model2.add(x);
    model2.set_var_ub(x, 10.);

In this example, the variable :math:`x` is added to two different models. The upper bound of :math:`x` in the second model
is then set to 10. The upper bound of :math:`x` in the first model is not affected by this change.

A more straightforward way to create variables is to call the ``add_var`` method of the ``Model`` class.
For instance, one can do:

.. code-block:: cpp

    const auto x = model.add_var(0, Inf, Continuous, "x");

Here, we do not need to repeat the environment since the model's environment is used by default. Moreover, we do not need to
explicitly add the variable to the model since it is done automatically by the ``add_var`` method.

Again, a variable may belong to several models at the same time. And we can do as follows.

.. code:: cpp

    const auto x = model.add_var(0, Inf, Continuous, "x");

    Model model2(env);
    model2.add(x);
    model2.set_var_ub(x, 10.);

Note that an even more compact version of the previous code is obtained by using the ``Model::add`` method with additional
parameters. For instance, the following code is equivalent to the previous one.

.. code:: cpp

    const auto x = model.add(0, Inf, Continuous, "x");

    Model model2(env);
    model2.add(x, TempVar(0, 10, Continuous));

Here, we used the class :ref:`TempVar <api_TempVar>` to create a temporary variable. A temporary variable is a variable
which has not been instantiate yet. It is only a "template" of a variable specifying its bounds and type. Here, we therefore
add the variable :math:`x` to the second model and directly set its attributes (bounds and type) accordingly.

Creating Several Variables at Once
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Sometimes, you will find it convenient to create several variables at once. This can be done by calling the ``Var::make_vector``
function, or the ``Model::add_vars`` method. These functions require
an extra parameter specifying the dimension of the new variable.

For instance, the following code creates variables :math:`y_{ij}` with :math:`i=1,...,K` and :math:`j=1,...,T`.

.. code:: cpp

    const auto y = Var::make_vector(env, Dim<2>(K, T), 0., Inf, Continuous, "y");

    model.add_vector<Var, 2>(y);

    std::cout << y[0][0] << std::endl; // "y_0_0"

Similarly, one could use the more direct ``Model::add_vars`` method.

.. code:: cpp

    const auto y = model.add_vars(Dim<2>(K, T), 0., Inf, Continuous, "y");

    std::cout << y[0][0] << std::endl; // "y_0_0"

Integer and binary variables can be created by setting the type of the variable to :cpp:`Integer` and :cpp:`Binary` respectively.

Accessing Variables
^^^^^^^^^^^^^^^^^^^

Information about a given variable in a model can be accessed by calling the corresponding methods of the model. For instance,
one can access the lower bound of a variable by calling the :cpp:`Model::get_var_lb` method. Similarly, the upper bound of a
variable can be accessed by calling the :cpp:`Model::get_var_ub` method. The type of a variable can be accessed by calling the
:cpp:`Model::get_var_type` method. The column of a variable in the model can be accessed by calling the :cpp:`Model::get_var_column`
method.

If a given model has been solved and feasibility could be proved (or better, optimality), the value of a variable
can be accessed by calling the :cpp:`Model::get_var_primal` method.
When applicable, its reduced cost can be accessed by calling the :cpp:`Model::get_var_reduced_cost` method.
Similarly, for unbounded models, a primal ray can be accessed by calling the :cpp:`Model::get_var_ray` method.

The :cpp:`Model::has` method can be used to check if a given variable is in the model.

The current index of a variable in the model can be accessed by calling the :cpp:`Model::get_var_index` method.
Beware, however, that the index may change if the model is modified.

For more details, see the :ref:`Model <api_Model>` class.

Modifying Variables
^^^^^^^^^^^^^^^^^^^

The bounds of a variable can be modified by calling the :cpp:`Model::set_var_lb` and :cpp:`Model::set_var_ub` methods.
The type of a variable can be modified by calling the :cpp:`Model::set_var_type` method.

The column of a variable in the model can be modified by calling the :cpp:`Model::set_var_column` method.
For instance.

.. code:: cpp

    Column column(objective_coefficient_for_x);
    column.linear().set(constraint1, constraint1_coefficient_for_x);
    column.linear().set(constraint2, constraint2_coefficient_for_x);

    model.set_var_column(x, column);

For more details, see the :ref:`Model <api_Model>` class.

Removing Variables
^^^^^^^^^^^^^^^^^^

A variable can be removed from a model by calling the :cpp:`Model::remove` method.
