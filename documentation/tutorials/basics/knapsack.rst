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
        \max\  & \sum_{j=1}^n p_jx_j \\
        \textrm{s.t. } & \sum_{j=1}^n w_jx_j \le W \\
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

    // Create environemnt
    Env env;

    // Create a new model
    Model model(env);

    // Create a vector for storing the x variables
    std::vector<Var> x;
    x.reserve(n);

    // Create x variables as binary with p_j as objective coefficient
    for (unsigned int j = 0 ; j < n ; ++j) {
        Var x_j(env, 0., 1., Binary, p[j], "x_" + std::to_string(j));
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
    model.set(Attr::Obj::Sense, Maximize);

Here, we used the :cpp:`Model::set` method to set the objective sense of our model. By default, all models are
assumed to be minimization problems.

More elegant approach
^^^^^^^^^^^^^^^^^^^^^

Though our naive approach works well, its size can be greatly reduced by using the predefined macro :cpp:`idol_Sum`
and further calls to the :cpp:`Model::set` method.

.. code-block:: cpp

    // Create environemnt
    Env env;

    // Create a new model
    Model model(env);

    // Create x variables
    auto x = Var::array(env, Dim<1>(n), 0., 1., Binary, "x");
    model.add_array<Var, 1>(x);

    // Create the knapsack constraint
    Ctr c(env, idol_Sum(j, Range(n), w[j] * x[j] ) <= W);
    model.add(c);

    // Define objective sense
    model.set(Attr::Obj::Sense, Maximize);
    model.set(Attr::Obj::Expr, idol_Sum(j, Range(n), p[i] * x[i]);

Here, we directly add :math:`n` variables with the right types and bounds.
Then, we use the :cpp:`idol_Sum` macro to create the knapsack constraint.
This macro is used as follows :cpp:`idol_Sum({name}, {iteratable}, {expression})` where :cpp:`{name}` will be the name of an index
taking value in the :cpp:`{iteratable}` (here, :cpp:`Range(n)`) while :cpp:`{expression}` is the piece of expression which will be accumulated.
Here, :cpp:`Range` is used to define an iteratable ranging from :math:`0` to :math:`n` (note that it is also possible to range from :math:`l` to :math:`n` for :math:`l < n`
by calling :cpp:`Range(l, n)`). The objective function is created similarly.

Solving the model using an external solver
-----------------------------------------

We can now solve our optimization model.
Idol offers different approaches for solving optimization problems. To select the desired approach for a given model,
one must call the :cpp:`Idol::set_optimizer<>` method and set its template argument as the desired "backend solver".
For instance, the following will set the optimizer to Gurobi for solving our model.

.. code-block:: cpp

    Idol::set_optimizer<Gurobi>(model);

    model.optimize();

Once done, informations regarding the executation and solution can be accessed through the :cpp:`model` variable itself. For instance,
we may ask for the solution status, the primal values and the execution time as follows.

.. code-block:: cpp

    std::cout << "Solution: " << (SolutionStatus) model.get(Attr::Solution::Status) << std::endl;
    std::cout << "Value of x_0: " << model.get(Attr::Solution::Primal, x[O]) << std::endl;
    std::cout << "Time: " << solver.time().count(Seconds) << " s" << std::endl;

Note that a more compact way of saving primal values is available and is as follows.

.. code-block:: cpp

    auto primals = save(model, Attr::Solution::Primal);

    std::cout << primals << std::endl;