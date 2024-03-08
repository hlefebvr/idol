.. _api_Model:

Model
=====

.. admonition:: Example:

    Here is a short example which builds and solves the following optimization problem.

    .. math::

        \begin{array}{ll}
            \textrm{minimize } & -x_0 - 1.5 x_1 + 3 \\
            \textrm{subject to } & x_0 + 2 x_1 \le 1.5 \\
            & 0 \le x_0 \le 1 \\
            & 0 \le x_1 \le 1
        \end{array}

    .. code-block:: cpp

        Env env;

        Model model(env);

        auto x = model.add_vars(Dim<1>(2), 0, 1, Continuous, "x");

        auto c = model.add_ctr(x[0] + 2 * x[1] <= 1.5);

        model.set_obj_expr(-x[0] - 1.5 * x[1] + 3);

        model.use(Gurobi());

        model.optimize();

        std::cout << "Best obj = " << model.get_best_obj() << std::endl;

.. doxygenclass:: idol::Model
