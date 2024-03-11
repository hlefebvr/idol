Expressions
-----------

An expression generically refers to any mathematical expression involving variables or constraints. In idol, expressions
are represented by the :ref:`Expr <api_Expr>` class.

An expression can be created by adding, subtracting or multiplying variables with constants or other expressions. For instance,
the following code creates the mathematical expression :math:`1 + 3 x_0 + x_1 + x_0 + 2 x_0 x_1`.

.. code:: cpp

    const Expr expr = 1 + 3 * x[0] + x[1] + x[0] + 2 * x[0] * x[1];

    std::cout << expr << std::endl; // "1 + 4 * x[0] + 1 * x[1] + 2 * x[0] * x[1]"

Expressions are composed of three parts:

* A constant (or offset) which is an instance of the :ref:`Constant <api_Constant>` class;
* A linear part which is an instance of the :ref:`LinExpr <api_LinExpr>` class;
* A quadratic part which is an instance of the :ref:`QuadExpr <api_QuadExpr>` class.

Each of these parts can be accessed using the methods :cpp:`Expr::constant`, :cpp:`Expr::linear` and :cpp:`Expr::quadratic`, respectively.
For instance, consider the following code.

.. code-block:: cpp

    for (const auto& [var, coefficient] : expr.linear()) {
        std::cout << var << " is multiplied by " << coefficient << std::endl;
    }

This code iterates over the linear part of the expression and prints the variables and their coefficients.


.. admonition:: About constants in expressions

    Without going into too much detail, we should here precise that each constant multiplying a variable in an :cpp:`Expr`,
    as well as the offset constant, can actually be composite. For instance, this is the case when a variable is multiplied by
    a parameter which is considered fixed in the current model but cannot be evaluated at the time the expression is created.

    Creating such "parameters" can be done by prepending a variable with the ``!`` symbol. This will automatically
    create an instance of the :ref:`Param <api_Param>` class.
    For instance, the following code creates
    a constant which involves the variables :math:`\xi_0` and :math:`\xi_1`, viewed as parameters.

    .. code-block::

        Env env;
        const Model model(env);
        const auto x = model.add_vars(Dim<1>(2), 0., Inf, Continuous, "x");

        const Model model2(env);
        const auto xi = model2.add_vars(Dim<1>(2), 0., 1., Continuous, "xi");

        const Expr expr = (1 + 2 * !xi[0]) * x[0] + 3 * !xi[1] * x[1];

    Here, ``1 + 2 * !xi_0`` is an instance of the ``Constant`` object and can be used as follows.

    .. code-block::

        Constant constant = 1 + 2 * !xi_0;

        std::cout << constant.numerical() << std::endl; // output: 1

        for (const auto& [param, coeff] : constant) {
            std::cout << coeff << " * " << param << std::endl; // output: 2 * !xi_0
        }

    Note that a parameter can be turned back into a variable by calling the :cpp:`Param::as<T>` template method.
    See, for instance.

    .. code-block:: cpp

        const auto param = !xi[0];

        if (param.is<Var>()) {
            const auto var = param.as<Var>();
            // do somthing with the variable
        }

    Parameters can be variables or constraints.
