.. _api_constraints:

Constraints
-----------

.. contents:: Table of Contents
    :local:
    :depth: 2

Creating Constraints
^^^^^^^^^^^^^^^^^^^^

Similarly to variables, constraints are easily created and added to a given ``Model``.

Constraints can be created by calling the constructor of the :ref:`Ctr <api_Ctr>` class and added to a model by means of
:cpp:`Model::add` or by calling the :cpp:`Model::add_ctr` method. See for instance.

.. code::

    Env env;
    Model model(env);

    Var x_0(env, 0., Inf, Continuous, "x_0");
    Var x_1(env, 0., Inf, Continuous, "x_1");
    Ctr constraint(env, x_0 + x_1 >= 1);

    model.add(x_0);
    model.add(x_1);
    model.add(constraint);

A more compact version of this code is obtained by making use of the ``Model::add_vars`` and ``Model::add_ctr`` methods.

.. code-block:: cpp

    Env env;
    Model model(env);

    const auto x = model.add_vars(Dim<1>(2), 0., Inf, Continuous, "x");
    const auto constraint = model.add_ctr(x[0] + x[1] >= 1);

As you can see, a constraint is created using the pattern :code:`{expression} {sign} {expression}` where

* :code:`{sign}` is one of :code:`<=`, :code:`>=` and :code:`==`;
* :code:`{expression}` is an expression, i.e., an instance of :code:`Expr`.

Actually, the pattern :code:`{expression} {sign} {expression}` only creates a "temporary" constraint, i.e., a constraint
which is not associated to any model. This is why we need to add it to a model by calling the :code:`Model::add_ctr` method.
Temporary constraints are objects of the class :ref:`TempCtr <api_TempCtr>`. An equivalent code would be.

.. code:: cpp

    Env env;
    Model model(env);

    const auto x = model.add_vars(Dim<1>(2), 0., Inf, Continuous, "x");
    const auto temporary_constraint = TempCtr(Row(x[0] + x[1], 1), GreaterThan);

    model.add_ctr(temporary_constraint);

Here, we used the class :ref:`Row <api_Row>` to create the row associated to the constraint. The sign of the constraint is
specified by the second argument of the constructor of the :ref:`TempCtr <api_TempCtr>` class, and can take values
:code:`LessThan`, :code:`GreaterThan` and :code:`EqualTo`.

Accessing Constraints
^^^^^^^^^^^^^^^^^^^^^

Information about a given constraint in a model can be accessed by calling the corresponding methods of the model.
The type of a constraint can be accessed by calling the :cpp:`Model::get_ctr_type` method. The row of a constraint can be
accessed by calling the :cpp:`Model::get_ctr_row` method.

If a given model has been solved and feasibility could be proved (or better, optimality), the dual value of a constraint
can be accessed by calling the :cpp:`Model::get_ctr_dual` method.
For infeasible models, a Farkas certificate (dual ray) can be accessed by calling the :cpp:`Model::get_ctr_farkas` method.

The :cpp:`Model::has` method can be used to check if a given constraint is in the model.

The current index of a constraint in the model can be accessed by calling the :cpp:`Model::get_ctr_index` method.
Beware, however, that the index may change if the model is modified.

For more details, see the :ref:`Model <api_Model>` class.

Modifying Constraints
^^^^^^^^^^^^^^^^^^^^^

The type of a constraint can be modified by calling the :cpp:`Ctr::set_ctr_type` method.
The value of the right-hand side of a constraint can be modified by calling the :cpp:`Ctr::set_rhs` method.

The row of a constraint in the model can be modified by calling the :cpp:`Ctr::set_row` method.
For instance.

.. code:: cpp

    Row row;
    row.set_rhs(2);
    row.linear().set(x, coefficient_for_x_in_constraint);
    row.linear().set(y, coefficient_for_y_in_constraint);

    constraint.set_ctr_row(constraint, row);

For more details, see the :ref:`Model <api_Model>` class.

Removing Constraints
^^^^^^^^^^^^^^^^^^^^

A constraint can be removed from a model by calling the :cpp:`Model::remove` method.

