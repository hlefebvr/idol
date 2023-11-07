.. _modeling_optimization_problems:

.. role:: cpp(code)
   :language: cpp

Modeling Optimization Problems
==============================

On this page, we discuss the very basic ideas of idol's modeling API.
All classes which are used for modeling an optimization problem can be accessed by including :code:`#include <idol/modeling.h>`.
Let's get started.

.. warning::

    The idol library can be found inside the :code:`idol` namespace, thus, be sure to add :code:`using namespace idol;`
    where desired or to explicitly refer to classes using this namespace, e.g., by using :code:`idol::Model` instead of :code:`Model`.

Env
---

Every optimization object (like variables and constraints) are managed by an "optimization environment". Essentially,
it is the environment that controls the death and lives of such objects. It is through the environment that idol
manages the different versions each optimization object may have during the execution of your program.

Typically, only one environment should be used by your code, though it is possible to instantiate many environment (not advised).

Environments are objects of the ``Env`` class and can be created as follows.

.. code-block::

    Env env; // Creates a new optimization environment.

Model
-----

The :cpp:`Model` class is used to represent an optimization model of the following form.

.. math::

    \begin{array}{lll}
        \min\  & \displaystyle \sum_{j=1}^n c_jx_j + \sum_{j=1}^n\sum_{k=1}^n d_{jk}x_jx_k \\
        \textrm{s.t. } & \displaystyle \sum_{j=1}^n a_{ij}x_j + \sum_{j=1}^n\sum_{k=1}^n q^i_{jk}x_jx_k \le b_i & i=1,...,m \\
        & l_j \le x_j \le u_j & j=1,...,m \\
        & x_j\in\mathbb Z & j\in J_I
    \end{array}

Here, :math:`x_j` are the variables of this optimization problem while :math:`c_j, d_{jk}, a_{ij}` and :math:`q_{jk}^i` are given
input parameters for this model. Optionally, :math:`J_I\subseteq\{1,...,n\}` is a set of variable indices
whose value must be integral.

Constructors of :cpp:`Model` can be used to create a new optimization model in idol. The most common takes a single argument
which is the optimization environment to be used.

.. code-block:: cpp

    int main(int t_argc, const char** t_argv) {

        Env env;

        Model model(env);

        // The model has been created!

        return 0;
    }

Now, we can define optimization variables and constraints.

Variables
---------

Any variable in idol is associated to at least one :cpp:`Model` object. Typically, exactly one.
We can easily create a new variable by using the constructor of the :cpp:`Var` class and add it to a model by calling the :cpp:`Model::add` method.
For instance, the following piece of code
will create a new variable, called "x", which is continuous and non-negative.

.. code-block:: cpp

    Var x(env, 0., Inf, Continuous, "x");

    model.add(x); // Variable x is added to the model

A more straightforward way of creating variables is by directly calling the ``add_var`` method of the ``Model`` class.
For instance, the following piece of code is equivalent to the previous one.

.. code-block:: cpp

    model.add_var(0, Inf, Continuous, "x");

Note that we did not need to repeat the environment since the model's environment is used by default.

The idol library also offers simple ways for creating multiple variables at once.
For instance, one can call the :cpp:`Var::make_vector` function. This function requires
an extra parameter specifying the dimension of the new variable. For instance, the following code creates variables :math:`y_{ij}`
with :math:`i=1,...,K` and :math:`j=1,...,T`.

.. code-block:: cpp

    auto y = Var::array(env, Dim<2>(K, T), 0., Inf, Continuous, "y");

    model.add_array<Var, 2>(y); // Variables y_ij are added to the model

    std::cout << y[0][0] << std::endl; // "y_0_0"

Alternatively, one can use the ``Model::add_vars`` method.

.. code-block:: cpp

    auto y= model.add_vars(Dim<2>(K, T), 0., Inf, Continuous, "y");

    std::cout << y[0][0] << std::endl; // "y_0_0"

Constraints
-----------

Similarly to variables, constraints are easily created and added to a given ``Model``.

Constraints can be created by calling the constructor of the :cpp:`Ctr` class and added to a model by means of :cpp:`Model::add`.
See for instance.

.. code-block:: cpp

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

    auto x = model.add_vars(Dim<1>(2), 0., Inf, Continuous, "x");
    auto constraint = model.add_ctr(x + y >= 1);

As you can see, a constraint is created using the pattern :code:`{expression} {sign} {expression}` where

* :code:`{sign}` is one of :code:`<=`, :code:`>=` and :code:`==`;
* :code:`{expression}` is an instance of :code:`Expr`.

The :code:`Expr` class is used to represent a mathematical expression in idol. An expression is composed of three parts:

* :code:`Expr::linear` will give you access to the linear part of the expression (it is an instance of ``LinExpr``);
* :code:`Expr::quadratic` will give you access to the quadratic part of the expression (it is an instance of ``QuadExpr``);
* :code:`Expr::constant` will return the constant (or offset) of the expression (it is an instance of ``Constant``).

Typically, however, you will not really care about what composes an expression since an :code:`Expr` object can be created
quite naturally. See for instance the following code.

.. code-block:: cpp

    Expr expr = 1 + 3 * x[0];
    expr += x[1];
    expr += x[0] + 2 * x[0] * x[1];

    std::cout << expr << std::endl; // "1 + 4 * x[0] + 1 * x[1] + 2 * x[0] * x[1]"

As mentioned, you can access parts of an expression as follows.

.. code-block:: cpp

    for (const auto& [var, coefficient] : expr.linear()) {
        std::cout << var << " is multiplied by " << coefficient << std::endl;
    }

    /*
        "x_0 is multiplied by 4"
        "x_1 is multiplied by 1"
    */

.. admonition:: About constants in expressions

    Without diving into too much detail, we should here precise that each constant multiplying a variable in an :cpp:`Expr`
    can actually be composite (e.g., a coefficient in the expression may refer to external optimization variables whose
    value is considered fixed in the current model).

    Fixing variables in an optimization problem can be achieved thanks to the ``!`` symbol.

    For instance, here is an expression where ``xi_0`` and ``xi_1`` are seen as parameters.

    .. code-block::

        Expr expr = (1 + 2 * !xi_0) * x + 3 * !xi_1 * y;

    Here, ``1 + 2 * !xi_0`` is an instance of the ``Constant`` object and can be used as follows.

    .. code-block::

        Constant constant = 1 + 2 * !xi_0;

        std::cout << constant.numerical() << std::endl; // output: 1

        for (const auto& [param, coeff] : constant) {
            std::cout << coeff << " * " << param << std::endl; // output: 2 * !xi_0
        }

Objective function
------------------

The objective function of an optimization model can specified by the ``Model::set_obj_expr`` method.

Here is an example which sets the objective function to :math:`-x_0 + 2 x_1`;

.. code-block::

    model.set_obj_expr(-x_0 + 2 * x_1);

What then?
----------

In the following tutorial, we will dive into even more practical ways of building expressions, considering a simple
example based on the combinatorial Knapsack problem.
We will also see how to call an external solver like Gurobi or HiGHS using idol.
See you there!
