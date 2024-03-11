Solving a Model and Accessing the Solution
==========================================

.. contents:: Table of Contents
    :local:
    :depth: 2

Solving a Model
^^^^^^^^^^^^^^^

This page shoes how to solve a model and access the solution.
The reader should be familiar with the concept of optimizer and optimizer factories. If this is not the case,
please refer to the page on :ref:`Optimizers <mip_optimizers>`.

Let us consider the following code.

.. code:: cpp

    using namespace idol;

    const unsigned int n_items = 5;
    const double[] profit = { 40., 50., 100., 95., 30., };
    const double[] weight = { 2., 3.14, 1.98, 5., 3., };
    const double capacity = 10.;

    Env env;
    Model model(env, Maximize);

    const auto x = model.add_vars(Dim<1>(n_items), 0., 1., Binary, "x");
    model.add(idol_Sum(j, Range(n_items), weight[j] * x[j] ) <= capacity);
    model.set_obj_expr(idol_Sum(j, Range(n_items), profit[i] * x[i]);

This code creates a model for the knapsack problem. As described in the page on :ref:`this page <mip_optimizers>`,
we now set up an optimizer and solve the model. We use GLPK.

.. code:: cpp

    model.use(GLPK());

Solving the model is done by a single class to the :code:`optimize` method.

.. code:: cpp

    model.optimize();

Then, idol provides several methods to access the solution.

First, :code:`get_status` and :code:`get_reason` return the status and the reason of the solution.
The status can be, for instance, :code:`Optimal` or :code:`Infeasible`. The reason provides more details about the status.
For instance, if the status is :code:`Infeasible`, the reason can be that the original problem is infeasible (:code:`Proved`),
or that the solver reached a time limit before finding a feasible solution (:code:`TimeLimit`).

The best objective value found can be accessed using :code:`get_best_obj`. This is the best objective value among feasible solutions considered during the execution of the algorithm.
The best objective value bound can be accessed using :code:`get_best_bound`. For instance, this can be a dual bound.
The methods :code:`get_relative_gap` and :code:`get_absolute_gap` return the relative and absolute optimality gaps, respectively.
See :ref:`this page <api_tolerances>` for more details about gaps and tolerances.

The methods :code:`get_var_primal` and :code:`get_var_ray` return the primal value and the (primal) ray value of a given variable, respectively.
Note that primal values are only accessible if the model has status :code:`Feasible`, :code:`SubOptimal` or :code:`Optimal`.
Similarly, a primal ray is only accessible if the model has status :code:`Unbounded`.

The methods :code:`get_ctr_dual` and :code:`get_ctr_farkas` return the dual value and the Farkas certificate value of a given constraint, respectively.
Note that dual values are only accessible if the model is continuous and has status :code:`Optimal` or :code:`Feasible`.
Similarly, a Farkas certificate is only accessible if the model has status :code:`Infeasible`.

Saving a Solution
^^^^^^^^^^^^^^^^^

Sometimes, you will find it useful to save a solution to access it later. idol provides the following functions to do so:
:code:`save_primal`, :code:`save_ray`, :code:`save_dual` and :code:`save_farkas`.
Each of these functions takes a model as argument and returns an object of the class :ref:`Solution::Primal <api_Solution_Primal>` or :ref:`Solution::Primal <api_Solution_Dual>` depending on the function.
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

This code will return an object of the class :ref:`Solution::Primal <api_Solution_Primal>` storing the results of corresponding calls to :code:`get_var_primal` methods on the higher dimensional model
for the original model variables.
