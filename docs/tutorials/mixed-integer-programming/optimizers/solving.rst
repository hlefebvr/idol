Solving a Model and Accessing the Solution
==========================================

This page shows how to solve a model and access the solution.
The reader should be familiar with the concept of optimizer and optimizer factories. If this is not the case,
please refer to the page on :ref:`Optimizers <mip_optimizers>`.

.. contents:: Table of Contents
    :local:
    :depth: 2

Solving a Model with GLPK
^^^^^^^^^^^^^^^^^^^^^^^^^

Let us consider the following code.

.. code:: cpp

    using namespace idol;

    const unsigned int n_items = 5;
    const double[] profit = { 40., 50., 100., 95., 30., };
    const double[] weight = { 2., 3.14, 1.98, 5., 3., };
    const double capacity = 10.;

    Env env;
    Model model(env, Maximize);

    auto x = model.add_vars(Dim<1>(n_items), 0., 1., Binary, 0, "x");
    model.add(idol_Sum(j, Range(n_items), weight[j] * x[j] ) <= capacity);
    model.set_obj_expr(idol_Sum(j, Range(n_items), profit[i] * x[i]);

This code creates a model for the knapsack problem.

As described in the page on :ref:`this page <mip_optimizers>`,
we will now set up an optimizer and solve the model. For this example, we use GLPK.

.. code:: cpp

    model.use(GLPK());

Solving the model is then done by calling the :code:`optimize` method.

.. code:: cpp

    model.optimize();

Accessing the Solution
^^^^^^^^^^^^^^^^^^^^^^

idol provides several methods to access the solution.

First, the status of the solution can be accessed using the :code:`get_status` method.
Here is a detailed list of possible statuses and reasons:

+-------------------+-------------------------------------------+
| Status            | Description                               |
+===================+===========================================+
| Loaded            | The model has been loaded but not solved. |
+-------------------+-------------------------------------------+
| Optimal           | An optimal solution has been found.       |
+-------------------+-------------------------------------------+
| Feasible          | A feasible solution has been found.       |
+-------------------+-------------------------------------------+
| Infeasible        | No feasible solution could be found.      |
+-------------------+-------------------------------------------+
| InfOrUnbnd        | The model is infeasible or unbounded.     |
+-------------------+-------------------------------------------+
| Unbounded         | The model is unbounded.                   |
+-------------------+-------------------------------------------+
| Fail              | The solver failed to solve the model.     |
+-------------------+-------------------------------------------+
| SubOptimal        | A suboptimal solution has been found.     |
+-------------------+-------------------------------------------+

Related to a status, the reason for the status can be accessed using the :code:`get_reason` method.

+-----------------+--------------------------------------------------------------------+
| Reason          | Description                                                        |
+=================+====================================================================+
| NotSpecified    | No specific reason is available.                                   |
+-----------------+--------------------------------------------------------------------+
| Proved          | The solver proved optimality.                                      |
+-----------------+--------------------------------------------------------------------+
| TimeLimit       | The solver reached the time limit.                                 |
+-----------------+--------------------------------------------------------------------+
| IterLimit       | The solver reached the iteration limit.                            |
+-----------------+--------------------------------------------------------------------+
| ObjLimit        | The solver reached the objective limit.                            |
+-----------------+--------------------------------------------------------------------+
| Numerical       | A numerical issue occurred during the solution process.            |
+-----------------+--------------------------------------------------------------------+
| MemoryLimit     | The solver ran out of memory.                                      |
+-----------------+--------------------------------------------------------------------+
| Cycling         | The solver encountered cycling (e.g., in simplex method).          |
+-----------------+--------------------------------------------------------------------+
| SolutionLimit   | The solver reached the solution limit (e.g., number of solutions). |
+-----------------+--------------------------------------------------------------------+

Then, the following methods can be used to access the solution:

- :code:`get_best_obj` returns the best known objective value (this always refers to feasible solutions),
- :code:`get_best_bound` returns the best known objective value bound,
- :code:`get_relative_gap` returns the relative optimality gap,
- :code:`get_absolute_gap` returns the absolute optimality gap; see :ref:`this page <api_tolerances>` for more details about gaps and tolerances.

Accessing the primal and dual values can be done with the following methods:

- :code:`get_var_primal` returns the primal value of a given variable (Feasible and Optimal status only),
- :code:`get_var_ray` returns the primal ray value of a given variable (Unbounded status only),
- :code:`get_ctr_dual` returns the dual value of a given constraint (Continuous models only),
- :code:`get_ctr_farkas` returns the Farkas certificate value of a given constraint (Continuous models and Infeasible status only).

Saving a Solution
^^^^^^^^^^^^^^^^^

Sometimes, you will find it useful to save a solution to access it later.

idol provides the following functions to do so:
:code:`save_primal`, :code:`save_ray`, :code:`save_dual` and :code:`save_farkas`.
Each of these functions takes a model as argument and returns an object of the class :ref:`Point<Var> <api_Point>` or :ref:`Point<Ctr> <api_Point>` depending on the function.
The returned object stores the results of corresponding calls to :code:`get_var_primal`, :code:`get_var_ray`, :code:`get_ctr_dual` or :code:`get_ctr_farkas` methods.

.. admonition:: Example

    This example shows how to solve a model using HiGHS and retrieves some piece of information about its solution.

    .. code-block::

        model.use(HiGHS());

        model.optimize();

        const auto status = model.get_status();

        if (status == Optimal) {

            std::cout << "Optimal solution found!" << std::endl;

            const auto primal_values = save_primal(model);

            std::cout << primal_values << std::endl;

        } else {

            std::cout << "An optimal solution could not be found." << std::endl;

            std::cout << "HiGHS returned status " << status << std::endl;

            std::cout << "The reason for this status is " << model.get_reason() << std::endl;

            if (status == Feasible) {

                std::cout << "The optimality gap is " << model.get_relative_gap() * 100 << " %" << std::endl;

            } else if (status == Unbounded) {

                std::cout << "An unbounded ray is" << std::endl;

                const auto primal_ray = save_ray(model);

                std::cout << primal_ray << std::endl;

            } else if (status == Infeasible) {

                std::cout << "A Farkas certificate is" << std::endl;

                const auto farkas = save_farkas(model);

                std::cout << farkas << std::endl;

            }

        }

Saving a Projected Solution
^^^^^^^^^^^^^^^^^^^^^^^^^^^

In a more advanced solution scheme, you may deal with extended formulations of an original model, and may want to save
the projected solution on the original problem space.
In such a case, you can use the functions :code:`save_*` with an additional argument to specify the original model.
For instance.

.. code:: cpp

    const auto primal_values = save_primal(original_model, higher_dimensional_model);

This code will return an object of the class :ref:`Point<Var> <api_Solution_Primal>` storing the results of corresponding calls to :code:`get_var_primal` methods on the higher dimensional model
for the original model variables.
