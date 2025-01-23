.. _api_constraints:

Constraints: :code:`Ctr` and :code:`QCtr`
=========================================

Constraints are mathematical expressions that must be satisfied by the decision variables of an optimization problem.
They are used to model relationships between variables, and to restrict the feasible region of the problem.

There are mainly two types of constraints:

- **Linear Constraints**: constraints that are linear in the decision variables.
- **Quadratic Constraints**: constraints that are non-linear in the decision variables.

.. contents:: Table of Contents
    :local:
    :depth: 2

Linear Constraints
------------------

Creating Constraints
^^^^^^^^^^^^^^^^^^^^

Similarly to variables, constraints are easily created and added to a given ``Model``.

Constraints can be created by calling the constructor of the :ref:`Ctr <api_Ctr>` class and added to a model by means of
:cpp:`Model::add` or by calling the :cpp:`Model::add_ctr` method.

Using the :cpp:`Ctr` class directly creates a variable that is not associated to any model. We will then need to add it
explicitly to a model. This is done by calling the :cpp:`Model::add` method. See for instance the following code.

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

    const auto x = model.add_vars(Dim<1>(2), 0., Inf, Continuous, 0, "x");
    const auto constraint = model.add_ctr(x[0] + x[1] >= 1);

Temporary Constraints
^^^^^^^^^^^^^^^^^^^^^

As you can see, a constraint is created using the pattern :code:`{expression} {sign} {expression}` where

* :code:`{sign}` is one of :code:`<=`, :code:`>=` and :code:`==`;
* :code:`{expression}` is an expression, i.e., an instance of :code:`AffExpr`.

Actually, the pattern :code:`{expression} {sign} {expression}` only creates a "temporary" constraint, i.e., a constraint
which is not associated to any model. This is why we need to add it to a model by calling the :code:`Model::add_ctr` method.
Temporary constraints are objects of the class :ref:`TempCtr <api_TempCtr>`. An equivalent code would be.

.. code:: cpp

    Env env;
    Model model(env);

    const auto x = model.add_vars(Dim<1>(2), 0., Inf, Continuous, "x");
    const auto temporary_constraint = TempCtr(x[0] + x[1], GreaterOrEqual, 1);

    model.add_ctr(temporary_constraint);

Here, the sign of the constraint is
specified by the second argument of the constructor of the :ref:`TempCtr <api_TempCtr>` class, and can take values
:code:`LessOrEqual`, :code:`GreaterOrEqual` and :code:`EqualTo`.

Accessing Constraints
^^^^^^^^^^^^^^^^^^^^^

Information about a given constraint in a model can be accessed by calling the corresponding methods of the model.
For instance, one can access the right-hand side of a constraint by calling the :cpp:`Model::get_ctr_rhs` method.
Here is a list of the most common methods to access information about a constraint.

- :code:`Model::get_ctr_rhs`: get the right-hand side of a constraint,
- :code:`Model::get_ctr_type`: get the type of a constraint,
- :code:`Model::get_ctr_row`: get the row of a constraint,
- :code:`Model::get_ctr_index`: get the index of a constraint.

When available, the values associated to the constraint in a given solution can be accessed in a similar way.
Here is a list of the most common methods to access information about a constraint in a solution.

- :code:`Model::get_ctr_dual`: get the dual value of a constraint,
- :code:`Model::get_ctr_farkas`: get the Farkas certificate of a constraint (for infeasible systems).

The :cpp:`Model::has` method can be used to check if a given constraint is in the model.

For more details, see the :ref:`Model <api_Model>` class.

Modifying Constraints
^^^^^^^^^^^^^^^^^^^^^

Similarly to accessing constraints, the attributes of a constraint can be modified by calling the corresponding methods of the model.
Here is a list of the most common methods to modify a constraint.

- :code:`Model::set_ctr_rhs`: set the right-hand side of a constraint,
- :code:`Model::set_ctr_type`: set the type of a constraint,
- :code:`Model::set_ctr_row`: set the row of a constraint.

The row of a constraint in the model can be modified by calling the :cpp:`Ctr::set_row` method.
For instance.

.. code:: cpp

    LinExpr row;
    row.set(x, coefficient_for_x_in_constraint);
    row.set(y, coefficient_for_y_in_constraint);

    model.set_ctr_row(constraint, row);

For more details, see the :ref:`Model <api_Model>` class.

Removing Constraints
^^^^^^^^^^^^^^^^^^^^

A constraint can be removed from a model by calling the :cpp:`Model::remove` method.

Quadratic Constraints
---------------------

Creating, accessing and modifying quadratic constraints is similar to linear constraints. The main difference is that
quadratic constraints are created using the :ref:`QCtr <api_QCtr>` class. Moreover, the corresponding methods in :code:`Model`
use :code:`qctr` instead of :code:`ctr`. For instance, the following code accesses the type of a quadratic constraint.

.. code::

    model.get_qctr_type(quadratic_constraint);

