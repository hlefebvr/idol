.. _basics_knapsack:

.. role:: cpp(code)
   :language: cpp

Using an external solver
================

In this tutorial, we will see how to use an external optimization solver like Gurobi or GLPK to solve
a small combinatorial problem. The considered problem is the knapsack problem (see `<https://en.wikipedia.org/wiki/Knapsack_problem>`).

Modeling
--------

The first step is to model our problem using idol. Recall the standard model for the knapsack problem:

.. math::

    \begin{array}{lll}
        \textrm{maximize } & \sum_{j=1}^n p_jx_j \\
        \textrm{subject to } & \sum_{j=1}^n w_jx_j \le W \\
        & x_j = 0 \textrm{ or } 1 & j=1,...,n
    \end{array}

where :math:`n` denotes the number of items and for each item :math:`j\in\{1,...,n\}`, :math:`p_j` denotes its profit while
:math:`w_j` denotes its weight. Finally, we let :math:`W` denote the knapsack capacity.

We will assume to have the following input data at hand.

.. code-block:: cpp

    const unsigned int n = 5; // 5 items
    const std::vector<double> p = { 40, 50, 100, 95, 30 }; // profits
    const std::vector<double> w = { 2, 3.14, 1.98, 5, 3 }; // weights
    const double W = 10; // capacity

Then, using idol's modeling API contained in :cpp:`#include <api/modeling.h>`, we can create our model easily.
We will first present a naive implementation using only the knowledge introduced in the previous tutorial.
Then, we will show a more compact way of implementing it.

Naive approach
^^^^^^^^^^^^^^

A direct way to model our knapsack problem is as follows.

.. code-block:: cpp

    // Create a new model
    Model model;

    // Create a vector for storing the x variables
    std::vector<Var> x;
    x.reserve(n);

    // Create x variables as binary with p_j as objective coefficient
    for (unsigned int j = 0 ; j < n ; ++j) {
        auto x_j = model.add_var(0., 1., Binary, p[j], "x_" + std::to_string(j));
        x.emplace_back( x_j );
    }

    // Create expression for the knapsack constraint
    Expr knapsack_constraint;
    for (unsigned int j = 0 ; j < n ; ++j) {
        knapsack_constraint += w[j] * x[j];
    }

    // Create the knapsack constraint
    model.add_ctr(knapsack_constraint <= W);

    // Define objective sense
    model.set(Attr::Obj::Sense, Maximize);

Here, we used the :cpp:`Model::set` method to set the objective sense of our model. By default, all models are
assumed to be minimization problems.

More elegant approach
^^^^^^^^^^^^^^^^^^^^^

Though our naive approach works well, its size can be greatly reduced by using the predefined macro :cpp:`idol_Sum`
and further calls to the :cpp:`Model::set` method.

.. code-block:: cpp

    // Create a new model
    Model model;

    // Create x variables
    auto x = model.add_vars(Dim<1>(n), 0., 1., Binary, 0., "x");

    // Define objective function
    for (unsigned int j = 0 ; j < n ; ++j) {
        model.set(Attr::Var::Obj, p[i]);
    }

    // Create the knapsack constraint
    model.add_ctr(idol_Sum(j, Range(n), w[j] * x[j] ) <= W);

    // Define objective sense
    model.set(Attr::Obj::Sense, Maximize);

Here, we directly add :math:`n` variables with a no objective coefficient. Then, we set the :cpp:`Attr::Var::Obj`
variable attribute to :math:`p_j` for each variable :math:`j`.

Then, we use the :cpp:`idol_Sum` macro to create the knapsack constraint.
This macro is used as follows :cpp:`idol_Sum({name}, {iteratable}, {expression})` where :cpp:`{name}` will be the name of an index
taking value in the :cpp:`{iteratable}` (here, :cpp:`Range(n)`) while :cpp:`{expression}` is the piece of expression which will be accumulated.
Here, :cpp:`Range` is used to define an iteratable ranging from :math:`0` to :math:`n` (note that it is also possible to range from :math:`l` to :math:`n` for :math:`l < n`
by calling :cpp:`Range(l, n)`).

Solving the model using an external solver
-----------------------------------------

We can now call an external solver like Gurobi or GLPK to solve our model. Every available solvers can be found under the
:cpp:`Solvers` namespace. For instance, the following will call Gurobi to solve our model.

.. code-block:: cpp

    Solvers::Gurobi solver(model);
    solver.solve();

Once done, informations regarding the executaion and solution can be accessed through the :cpp:`solver` variable. For instance,
we may ask for the solution status, the primal values and the execution time as follows.

.. code-block:: cpp

    std::cout << "Time: " << solver.time().count(Seconds) << " s" << std::endl;
    std::cout << "Solution: " << solver.primal_solution() << std::endl;