.. _using_an_external_solver:

.. role:: cpp(code)
   :language: cpp

Using an external solver
========================

In this tutorial, we will see how to use an external optimization solver like Gurobi or HiGHS to solve
a small combinatorial problem.
The considered problem is the Knapsack Problem; see the `Knapsack Problem wikipedia page <https://en.wikipedia.org/wiki/Knapsack_problem>`_.

Modeling
--------

The first step is to model our problem using idol. Recall the standard model for the knapsack problem:

.. math::

    \begin{array}{lll}
        \max\  & \displaystyle \sum_{j=1}^n p_jx_j \\
        \textrm{s.t. } & \displaystyle \sum_{j=1}^n w_jx_j \le W, \\
        & x_j \in \{ 0, 1 \} & j=1,...,n,
    \end{array}

in which :math:`n` denotes the number of items. For each item :math:`j\in\{1,...,n\}`, :math:`p_j` denotes the profit of
item while :math:`i` and :math:`w_j` denotes its weight. Finally, we let :math:`W` denote the knapsack capacity.

We will assume to have the following input data at hand.

.. code-block:: cpp

    const unsigned int n = 5; // 5 items
    const std::vector<double> p = { 40., 50., 100., 95., 30. }; // profits
    const std::vector<double> w = { 2., 3.14, 1.98, 5., 3. }; // weights
    const double W = 10; // capacity

Then, using idol's modeling API contained in :cpp:`#include <idol/modeling.h>`, we can create our model easily.
We will first present a naive implementation using only the knowledge introduced in the previous tutorial.
Then, we will show a more compact way of implementing it using the :code:`idol_Sum` macro.

A first approach
^^^^^^^^^^^^^^^^

A first approach to model our knapsack problem in idol is as follows.

.. code-block:: cpp

    // Create environemnt
    Env env;

    // Create a new model
    Model model(env);

    // Create a vector for storing the x variables
    std::vector<Var> x;
    x.reserve(n);

    // Create x variables as binary with p_j as objective coefficient
    for (unsigned int j = 0 ; j < n ; ++j) {
        Var x_j(env, 0., 1., Binary, Column(p[j]), "x_" + std::to_string(j));
        model.add(x_j);
        x.emplace_back( x_j );
    }

    // Create expression for the knapsack constraint
    Expr knapsack_constraint;
    for (unsigned int j = 0 ; j < n ; ++j) {
        knapsack_constraint += w[j] * x[j];
    }

    // Create the knapsack constraint
    Ctr c(knapsack_constraint <= W);
    model.add(c);

    // Define objective sense
    model.set_obj_sense(Maximize);

Here, we used the :cpp:`Model::set_obj_sense` method to set the objective sense of our model. By default, all models are
assumed to be minimization problems.

A more elegant approach
^^^^^^^^^^^^^^^^^^^^^^^

Though our first approach works well, its size can greatly be reduced while improving readability by using the
predefined macro :cpp:`idol_Sum` combined with clever calls to  methods of the :code:`Model` class.

.. code-block:: cpp

    // Create environemnt
    Env env;

    // Create a new model
    Model model(env);

    // Create x variables
    model.add_vars(Dim<1>(n), 0., 1., Binary, "x");

    // Create the knapsack constraint
    model.add(idol_Sum(j, Range(n), w[j] * x[j] ) <= W);

    // Define objective sense
    model.set_obj_sense(Maximize);
    model.set_obj_expr(idol_Sum(j, Range(n), p[i] * x[i]);

Here, we directly add :math:`n` variables with the right types and bounds.

Then, we use the :cpp:`idol_Sum` macro to create the knapsack constraint.
This macro is used as follows :cpp:`idol_Sum({name}, {iteratable}, {expression})` where :cpp:`{name}` will be the name of an index
taking value in the :cpp:`{iteratable}` (here, :cpp:`Range(n)`) while :cpp:`{expression}` is the piece of expression which will be accumulated.

.. admonition:: About Range

    In the above example, :cpp:`Range` is used to define an iterable ranging from :math:`0` to :math:`n` (note that it is also possible to range from :math:`l` to :math:`n` for :math:`l < n`
    by calling :cpp:`Range(l, n)`).

The objective function is created similarly.

Solving the problem using an external solver
--------------------------------------------

The idol library offers different ways for solving optimization problems.
To select the desired approach for a given model, one must call the :cpp:`Model::use` method and specify the
"optimizer" to be used.

For instance, the following will set the optimizer to HiGHS for solving our model.

.. code-block:: cpp

    model.use(HiGHS());

.. hint::

    Here, HiGHS is actually an ``OptimizerFactory`` which will eventually create an optimizer to solve our model.
    Indeed, the “real” optimizer in this case will be an instance of ``Optimizers::HiGHS`` which will be created just in
    time, when necessary.

    If you want to learn more about optimizers and optimizer factories, please refer to :ref:`this page <api_optimizers>`.

Now that the optimizer has been configure, we can solve our problem by calling the ``Model::optimize`` method.

.. code-block::

    model.optimize();

Accessing the solution
----------------------

Finally, you may access pieces of information regarding the solution by using one of the following methods:

* ``get_status`` returns the current solution status (e.g., ``Optimal``, ``Infeasible``, ``Unbounded``, ...).
* ``get_reason`` returns the reason for the solution status (e.g., the solution reports ``Infeasible`` because of
  ``TimeLimit``).
* ``get_best_obj`` returns the best objective value found (i.e., the best objective cost among feasible solutions
  considered during the execution of the algorithm).
* ``get_best_bound`` returns the best objective value bound (e.g., a dual bound).
* ``get_relative_gap`` returns the relative optimality gap computed as

.. math::

        \textrm{relative_gap} = \left| \frac{ \textrm{best_obj} - \textrm{best_bound} }{ 1e^{-10} + \textrm{best_obj} } \right|.

* ``get_absolute_gap`` returns the absolute optimality gap computed as

.. math::

        \textrm{absolute_gap} = | \textrm{best_obj} - \textrm{best_bound} |.

* ``get_var_primal`` returns the primal value (when status is ``Optimal`` or ``Feasible``) of a given variable.
* ``get_var_ray`` returns the ray value (when status is ``Unbounded``) of a given variable.
* ``get_ctr_dual`` returns the dual value (when status is ``Optimal`` or ``Feasible`` and for continuous problems) of a given
  constraint.
* ``get_ctr_farkas`` returns the Farkas certificate value (when status is ``Infeasible`` and for continuous problems) of a given
  constraint.

Additionally, functions ``save_primal``, ``save_ray``, ``save_dual`` and ``save_farkas`` will create objects of the class
``Solution::Primal`` and ``Solution::Dual`` storing the results of corresponding calls to ``get_var_*`` and ``get_ctr_*``
methods.

.. admonition:: Example

    This example shows how to solve a model using HiGHS and retrieves some pieces of information about its solution.

    .. code-block::

        model.use(HiGHS());

        model.optimize();

        const SolutionStatus status = model.get_status();

        if (status == Optimal) {

            std::cout << "Optimal solution found!" << std::endl;

            Solution::Primal primal_values = save_primal(model);

            std::cout << primal_values << std::endl;

        } else {

            std::cout << "An optimal solution could not be found" << std::endl;

            std::cout << "HiGHS returned status " << status << std::endl;

            std::cout << "The reason for this status is " << model.get_reason() << std::endl;

            if (status == Feasible) {

                std::cout << "The optimality gap is " << model.get_relative_gap() * 100 << " %" << std::endl;

            } else if (status == Unbounded) {

                std::cout << "An unbounded ray is" << std::endl;

                std::cout << save_ray(model) << std::endl;

            } else if (status == Infeasible) {

                std::cout << "A Farkas certificate is" << std::endl;

                std::cout << save_farkas(model) << std::endl;

            }

        }