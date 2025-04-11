.. _api_variables:

Variables
=========

Variables are the decision variables of an optimization problem. They are the objects that we want to find the best
values for, with respect to some objective function given the specified constraints.

.. contents:: Table of Contents
    :local:
    :depth: 2

Creating Variables
------------------

There are mainly two ways to create variables in idol. The first one is to use the :ref:`Var <api_Var>` class directly, and the
second one is to use the :cpp:`Model::add_var` method.

Using the :code:`Var` class and :code:`Model::add`
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Using the :cpp:`Var` class directly creates a variable that is not associated to any model. We will then need to add it
explicitly to a model. This is done by calling the :cpp:`Model::add` method. See for instance the following code which
creates a continuous variable :math:`x`, with bounds :math:`[0,\infty)`, and adds it to the model.

.. code:: cpp

    Var x(env, 0., Inf, Continuous, 0, "x");

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

Also, it is possible to add a variable to a model and set its attributes (bounds and type) at the same time. For instance,
the following code is equivalent to the previous one.

.. code:: cpp

    Var x(env, 0., Inf, Continuous, "x");

    model.add(x);

    Model model2(env);
    model2.add(x, TempVar(0, 10, Continuous, 0));

Here, we used the class :ref:`TempVar <api_mip_variables_TempVar>` to create a temporary variable. A temporary variable is a variable
which has not been instantiate yet. It is only a "template" of a variable specifying its bounds, type an objective coefficient. Here, we therefore
add the variable :math:`x` to the second model and directly set its attributes (bounds and type) accordingly.

Using :code:`Model::add_var`
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

A more straightforward way to create variables is to call the ``add_var`` method of the ``Model`` class.
For instance, one can do:

.. code-block:: cpp

    const auto x = model.add_var(0, Inf, Continuous, 0, "x");

Here, we do not need to repeat the environment since the model's environment is automatically used. Moreover, we do not need to
explicitly add the variable to the model since it is done by the ``add_var`` method.

Again, a variable may belong to several models at the same time. And we can do as follows.

.. code:: cpp

    const auto x = model.add_var(0, Inf, Continuous, "x");

    Model model2(env);
    model2.add(x);
    model2.set_var_ub(x, 10.);

Note that an even more compact version of the previous code is obtained by using the ``Model::add`` method with additional
parameters. For instance, the following code is equivalent to the previous one.

.. code:: cpp

    const auto x = model.add_var(0, Inf, Continuous, "x");

    Model model2(env);
    model2.add(x, TempVar(0, 10, Continuous, 0));

Creating Several Variables at Once
----------------------------------

Sometimes, you will find it convenient to create several variables at once. This can be done by calling the ``Var::make_vector``
function, or the ``Model::add_vars`` method. These functions require
an extra parameter specifying the dimension of the new variable.

Using :code:`Var::make_vector` and :code:`Model::add_vector`
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The following code creates variables :math:`y_{ij}` with :math:`i=1,...,K` and :math:`j=1,...,T`.

.. code:: cpp

    const auto y = Var::make_vector(env, Dim<2>(K, T), 0., Inf, Continuous, "y");

    model.add_vector<Var, 2>(y);

    std::cout << y[0][0] << std::endl; // "y_0_0"

Here, we use the :ref:`Dim <api_Dim>` class to specify the dimension of the new variable. The :ref:`Dim <api_Dim>` class is a template class
that takes an integer as a template parameter. The integer specifies the dimension of the new variable. In this case, we use
2 to specify that we want to create a two-dimensional variable. Then, we specify the size of each dimension by passing the
appropriate arguments to the constructor of the :ref:`Dim <api_Dim>` class. Here, we specify that the first dimension has size :math:`K` and the
second dimension has size :math:`T`.

Once the variables are created, we add them to the model by calling the :cpp:`Model::add_vector` method.

Using :code:`Model::add_vars`
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

One alternative is to use the more direct ``Model::add_vars`` method.

.. code:: cpp

    const auto y = model.add_vars(Dim<2>(K, T), 0., Inf, Continuous, "y");

    std::cout << y[0][0] << std::endl; // "y_0_0"

Note that the :ref:`Dim <api_Dim>` class is used in the same way as in the previous example.

Variables Types
---------------

Variables can be of different types. In idol, three types of variables are available:

- :code:`Continuous`: continuous variables,
- :code:`Binary` binary variables,
- :code:`Integer`: integer variables.

Accessing Variables
-------------------

Information about a given variable in a model can be accessed by calling the corresponding methods of the model. For instance,
one can access the lower bound of a variable by calling the :cpp:`Model::get_var_lb` method. Here is a list of the most common
methods to access information about a variable.

- :code:`Model::get_var_lb`: returns the lower bound of a variable,
- :code:`Model::get_var_ub`: returns the upper bound of a variable,
- :code:`Model::get_var_type`: returns the type of a variable,
- :code:`Model::get_var_column`: returns the column of a variable,
- :code:`Model::get_var_obj`: returns the objective coefficient of a variable,
- :code:`Model::get_var_index`: returns the index of a variable.

When available, the value of the variable in a given solution can be accessed in a similar way.
Here is a list of the most common methods to access information about a variable in a solution.

- :code:`Model::get_var_primal`: returns the value of a variable in a solution,
- :code:`Model::get_var_reduced_cost`: returns the reduced cost of a variable in a solution,
- :code:`Model::get_var_ray`: returns the primal ray of a variable in a solution.

The :cpp:`Model::has` method can be used to check if a given variable is in the model.

For more details, see the :ref:`Model <api_Model>` class.

Modifying Variables
-------------------

Similarly to accessing variables, the attributes of a variable can be modified by calling the corresponding methods of the model.
Here is a list of the most common methods to modify a variable.

- :code:`Model::set_var_lb`: sets the lower bound of a variable,
- :code:`Model::set_var_ub`: sets the upper bound of a variable,
- :code:`Model::set_var_type`: sets the type of a variable,
- :code:`Model::set_var_obj`: sets the objective coefficient of a variable,
- :code:`Model::set_var_column`: sets the column of a variable.

For instance, the following code updates the column of a given variable.

.. code:: cpp

    LinExpr<Ctr> column(objective_coefficient_for_x);
    column.set(constraint1, constraint1_coefficient_for_x);
    column.set(constraint2, constraint2_coefficient_for_x);

    model.set_var_column(x, column);

Here, we use the :cpp:`LinExpr<Ctr>` class to represent the column of the variable :math:`x`.

For more details, see the :ref:`Model <api_Model>` class.

Removing Variables
------------------

A variable can be removed from a model by calling the :cpp:`Model::remove` method.
