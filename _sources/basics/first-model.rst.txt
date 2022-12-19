.. _basics_first_model:

.. role:: cpp(code)
   :language: cpp

Introduction to the modeling interface
================

On this page, we discuss the very basic ideas of idol's modeling API.
All classes which are used for modeling an optimization problem can be accessed by including :code:`#include <idol/modeling.h>`.
Let's get started.

Model
-----

The :cpp:`Model` class is used to represent an optimization model of the following form.

.. math::

    \begin{array}{lll}
        \textrm{minimize } & \displaystyle \sum_{j=1}^n c_jx_j + \sum_{j=1}^n\sum_{k=1}^n d_{jk}x_jx_k \\
        \textrm{subject to } & \displaystyle \sum_{j=1}^n a_{ij}x_j + \sum_{j=1}^n\sum_{k=1}^n q^i_{jk}x_jx_k \le b_i & i=1,...,m \\
        & l_j \le x_j \le u_j & j=1,...,m \\
        & x_j\in\mathbb Z & j\in J_I
    \end{array}

Here, :math:`x_j` are the variables of this optimization problem while :math:`c_j, d_{jk}, a_{ij}` and :math:`q_{jk}^i` are given
input parameters for this model. Optionally, :math:`J_I\subseteq\{1,...,n\}` is a set of variable indices
whose value must be integral.

The default constructor of :cpp:`Model` can be used to create a new optimization model in idol.

.. code-block:: cpp

    int main(int t_argc, const char** t_argv) {

        Model model;

        // The model has been created!

        return 0;
    }

Now, we can define optimization variables and constraints.

Variables
---------

Any variable in idol is associated to at least one :cpp:`Model` object. Typically, exactly one.
We can easily create a new variable by using the :cpp:`Model::add_var` method. For instance, the following piece of code
will create a new variable, called "x", which is continuous, non-negative and has an objective coefficient of 2.

.. code-block:: cpp

    auto x = model.add_var(0., Inf, Continuous, 2, "x");

    std::cout << x << std::endl; // "x"

Note that it is also possible to create multiple variables at once by calling :cpp:`Model::add_vars`. This method requires
an extra parameter specifying the dimension of the new variable. For instance, the following code creates variables :math:`y_{ij}`
with :math:`i=1,...,K` and :math:`j=1,...,T`.

.. code-block:: cpp

    auto y = model.add_vars(Dim<2>(K, T), 0., Inf, Continuous, 0., "y);

    std::cout << y[0][0] << std::endl; // "y_0_0"

Another possible way to add a variable is to add it "by column", i.e., by specifying the matrix coefficients of the new variable.
This is done by first defining a :cpp:`Column` object and to give it to the :cpp:`Model::add_var` method. We will not discuss
it here for brevity.

Constraints
-----------

Similar to variables, any constraint in idol is associated to at least one :cpp:`Model` object and, typically, exactly one.

It is created by calling the :cpp:`Model::add_ctr` method. See for instance.

.. code-block:: cpp

    Model model;
    auto x = model.add_vars(Dim<1>(2), 0., Inf, Continuous, 1, "x");
    auto constraint = model.add_ctr(x[0] + x[1] >= 1);

As you can see, a constraint is created using the pattern :code:`{expression} {sign} {expression}` where :code:`{sign}` is one of
:code:`<=`, :code:`>=` and :code:`==`, and where :code:`{expression}` is an instance of :code:`Expr`.

The class :code:`Expr` is used to represent a mathematical expression in idol. An expression is composed of three parts:

* :code:`Expr::linear` will give you access to the linear part of the expression ;
* :code:`Expr::quadratic` will give you access to the quadratic part of the expression ;
* :code:`Expr::constant` will return the constant (or offset) of the expression.

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

Without diving into too much detail, we should here precise that each constant multiplying a variable in an :cpp:`Expr`
can actually be composite (See the tutorial on column generation for details, or see the :cpp:`InParam` and :cpp:`Constant` classes).
Accessing the actual :cpp:`double` which represents the constant can be done by calling :cpp:`Constant::numerical`.

In the following example, we make use of the :cpp:`LinExpr::get` function to retrieve the coefficient of a variable inside
the linear part of an :cpp:`Expr`. Then, we access the :cpp:`double` representing the constant.

.. code-block:: cpp

    Constant coefficient = expr.linear().get(x[0]);
    double num = coefficient.numerical();

    std::cout << num << std::endl; // "4"

In the following tutorial, we will dive into a more practical way to build expressions by considering the simple example
of the combinatorial Knapsack problem. We will also see how to call an external solver like Gurobi or GLPK using idol.
See you there!