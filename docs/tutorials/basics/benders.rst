.. _decomposition_benders:

.. role:: cpp(code)
   :language: cpp

Lazy Cuts and Benders Decomposition
===================================

In this section, we will show how to use the LazyCutCallback callback to implement a simple Benders Decomposition
algorithm.

.. hint::

    This tutorial regards the `advanced topic` of Benders Decomposition.
    Rudimentary notions in the following subjects are recommended:

    - `Benders Decomposition <https://en.wikipedia.org/wiki/Benders_decomposition>`_
    - `Linear Programming duality <https://en.wikipedia.org/wiki/Linear_programming#Duality>`_

Mathematical Model
------------------

Original Formulation
^^^^^^^^^^^^^^^^^^^^

We will base our example on the following model taken from `Blanco, V., (2016), Benders Decomposition, MINLP School: Theory
and Applications <http://metodoscuantitativos.ugr.es/pages/web/vblanco/minlp16/slotv2/!>`_.

.. math::

    \begin{align}
        \min_{x,y} \ & 2 x_0 + 3x_1 + 2y \\
        \text{s.t.} \ & x_0 + 2x_1 + y \ge 3, \\
        & 2x_0 - x_1 + 3y \ge 4, \\
        & x,y\ge 0.
    \end{align}

Benders Reformulation
^^^^^^^^^^^^^^^^^^^^^

We apply a Benders reformulation to this problem by considering :math:`y` as the complicating variable.
The Benders reformulation reads:

.. math::

    \begin{align}
        \min_{y,z} \ & 2y + z \\
        \text{s.t.} \ & z \ge \lambda_1 ( 3 - y ) + \lambda_2(4 - 3y) \qquad \forall \lambda \in \Lambda, \\
        & z \ge 0,
    \end{align}

with :math:`\Lambda` defined as the set of all :math:`\lambda\in\mathbb R^2_+` such that

.. math::

    \begin{align}
        & \lambda_0 + 2 \lambda_1 \le 2, \\
        & 2\lambda_0 - \lambda_1 \le 3.
    \end{align}

Implementation
--------------

We are now ready to implement our decomposition method. We will need to define three different things:

- the master problem;
- the dual space :math:`\Lambda`;
- the  shape of the cuts to be added.

The Master Problem
^^^^^^^^^^^^^^^^^^

The master problem is created like any optimization model; see our :ref:`Modeling tutorial <modeling_optimization_problems>`.

.. code::

    Env env;

    Model master(env);

    auto y = master.add_var(0, Inf, Continuous, "y");
    auto z = master.add_var(0, Inf, Continuous, "z");

    master.set_obj_expr(2 * y + z);

The Dual Space :math:`\Lambda`
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To provide the description of the dual space :math:`\Lambda`, we use another Model object which will contain the variables
and constraints defining :math:`\Lambda`. The objective function is not used and can be left to zero.

.. code::

    Model dual_space(env);

    auto lambda = dual_space.add_vars(Dim<1>(2), 0, Inf, Continuous, "lambda");

    dual_space.add_ctr(lambda[0] + 2 + lambda[1] <= 2);
    dual_space.add_ctr(2 * lambda[0] - lambda[1] <= 3);

The Cuts to be Added
^^^^^^^^^^^^^^^^^^^^

Finally, we need to define the cuts to be added to the master problem for a given dual variable :math:`\lambda`.
The cuts are always expressed in the "master space". What we mean by this is that, here, :math:`y` should be a *variable*
in the constraint while :math:`\lambda` should be a *constant*.

This is done as follows.

.. code::

    auto benders_cut = z >= !lambda[0] * (3 - y) + !lambda[1] * (4 - 3 * y);

See how the lambda variables are "turned into" constants by prepending them with an "!" symbol.

Solving the Model
-----------------

In this section, we solve our model using GLPK and the LazyCutCallback from idol.

This is done as follows.

.. code::

    master.use(
        GLPK()
            .add_callback(
                LazyCutCallback(dual_space, benders_cut)
                    .with_separation_optimizer(GLPK())
            )
    );

    master.optimize();

    std::cout << save_primal(master) << std::endl;

See how we specified also an optimizer for solving the separation problem. Here, we use GLPK.

.. warning::

    If you are using Gurobi with the LazyCutCallback, make sure to call the :code:`Gurobi::with_lazy_cuts` method.
    This is necessary to turn off some parameters of Gurobi which would otherwise lead to wrong solutions.

    .. code::

        master.use(
                Gurobi()
                    .with_lazy_cuts(true)
                    .add_callback(
                        LazyCutCallback(dual_space, benders_cut)
                            .with_separation_optimizer(Gurobi())
                    )
            );
