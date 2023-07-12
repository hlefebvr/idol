.. _api_model:

Model
=====

The Model class is probably the most central of idol. It is used to represent a mathematical optimization problem of the
following general form.

.. math::

    \begin{array}{lll}
        \textrm{minimize } & \displaystyle \sum_{j=1}^n a_{0j}x_j + \sum_{j=1}^n\sum_{k=1}^n q_{jk}^0x_jx_k + b_0\\
        \textrm{subject to } & \displaystyle \sum_{j=1}^n a_{ij}x_j + \sum_{j=1}^n\sum_{k=1}^n q^i_{jk}x_jx_k \le b_i & i=1,...,m \\
        & l_j \le x_j \le u_j & j=1,...,m \\
        & x_j\in\mathbb Z & j\in J_I,
    \end{array}

Here, :math:`x_1, ..., x_n` are the decision variables with bounds :math:`[l_1,u_1],...,[l_n,u_n]` (possibly,
:math:`l_j = -\infty` or :math:`u_j = +\infty`). Note that some variables have integrality requirements (i.e., those whose
indices are in :math:`J_I`). Other entities are input parameters of the problem and must typically have real values
(e.g., :math:`c_j, a_{ij}, q_{jk}^i` and :math:`b_i\in\mathbb R`). Alternatively, optimization problems can also be maximization
problems.

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
