.. _tutorial_bilevel_kkt:

Deriving Single-level Reformulations
====================================

A standard technique for solving LP-LP bilevel problems is to reformulate the problem as a single-level problem.
This can be done in several ways. In this tutorial, we will show how we can use idol to derive the
Karush-Kuhn-Tucker (KKT) reformulation and the strong-duality reformulation of a bilevel problem.

Let's get started!

.. contents:: Table of Contents
    :local:
    :depth: 2

Problem Formulation
-------------------

Consider the following bilevel optimization problem:

.. math::

    \begin{align}
        \min_{x,y} \quad & c^\top x + d^\top y \\
        \text{s.t.} \quad & Ax + By \ge a, \\
        & y \in \text{arg min} \{ f^\top y : Dy \ge b - Cx \}.
    \end{align}

In order to solve problem :math:numref:`bilevel` with a standard optimization solver, we need to reformulate it as a single-level optimization problem.
This can be automatically done by idol using the KKT reformulation or the strong-duality reformulation.

For this tutorial, we will work on the following code snippet:

.. code::

    Env env;

    Model high_point_relaxation(env);

    auto x = high_point_relaxation.add_var(0, Inf, Continuous, 0., "x");
    auto y = high_point_relaxation.add_var(-Inf, Inf, Continuous, 0., "y");

    high_point_relaxation.set_obj_expr(x + 6 * y);
    auto follower_c1 = high_point_relaxation.add_ctr(2 * x - y >= 0);
    auto follower_c2 = high_point_relaxation.add_ctr(-x - y >= -6);
    auto follower_c3 = high_point_relaxation.add_ctr(-x + 6 * y >= -3);
    auto follower_c4 = high_point_relaxation.add_ctr(x + 3 * y >= 3);
    high_point_relaxation.add_ctr(-x + 5 * y <= 12.5);

    Bilevel::Description description(env);
    description.set_lower_level_obj(-y);
    description.make_lower_level(y);
    description.make_lower_level(follower_c1);
    description.make_lower_level(follower_c2);
    description.make_lower_level(follower_c3);
    description.make_lower_level(follower_c4);

Here, a bilevel problem is modeled with idol. The high point relaxation is stored in :code:`high_point_relaxation`, while
the lower-level problem is described using the :code:`Bilevel::Description` object :code:`description`.

The KKT Reformulation
---------------------

The KKT reformulation of a bilevel problem is obtained by replacing the lower-level problem with its KKT optimality conditions.
The KKT reformulation of the bilevel problem is as follows:

.. math::
    :label: kkt

    \begin{align}
        \min_{x,y,\lambda} \quad & c^\top x + d^\top y \\
        \text{s.t.} \quad & Ax + By \ge a, \\
        & Dy \ge b - Cx, \\
        & D^\top \lambda = f, \;  \lambda \ge 0, \\
        & \lambda^\top (b - Cx - Dy) = 0.
    \end{align}

Getting the Reformulated Model
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

With idol, it is possible to generate the KKT reformulation of the bilevel problem using the :code:`Bilevel::KKT::make_model` function.

For instance, one could do the following.

.. code::

    auto single_level_model = Bilevel::KKT::make_model(
        high_point_relaxation, description
        );

Then, one can solve the single-level model using a standard optimization solver. Here, we use Gurobi.

.. code::

    single_level.use(Gurobi());
    single_level.optimize();

Note that the KKT reformulation :math:numref:`kkt`, and thus :code:`single_level`, is a nonlinear problem because of the complementarity constraint.
Hence, we use the nonlinear solving features of Gurobi here.

A common approach to handle the complementarity constraints is to linearize them by introducing binary variables.
This can also be done in idol. To do this, we will need to provide some valid bounds on the dual variables.
This is done by annotating the lower-level problem constraints. The constraints associated with the variables
bounds can also have their big-M values by annotating the corresponding variable.

In the following code, we will simply use a default value for the big-M values of :math:`10^{4}`.

.. code::

    Annotation<double> big_M(env, "big_M", 1e4);
    auto single_level_model = Bilevel::KKT::make_model(
            high_point_relaxation, description, big_M
            );

The returned model is now a mixed-integer linear program that can be solved using a standard optimization solver.

Now, say that constraint :code:`follower_c1` is known to have a dual variable bounded by, say, 10. One could do the following.

.. code::

    Annotation<double> big_M(env, "big_M", 1e4);
    follower_c1.set(big_M, 10);

    auto single_level_model = Bilevel::KKT::make_model(
            high_point_relaxation, description, big_M
            );

Solving the KKT Reformulation directly
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In the previous section, we showed how to generate the KKT reformulation of a bilevel problem.
However, idol also provides a function to solve the KKT reformulation directly.
This can be done by using the :code:`Bilevel::KKT` optimizer.

Here is a code snippet that shows how to solve the KKT reformulation directly.

.. code::

    high_point_relaxation.use(Bilevel::KKT(description) + Gurobi());

Note that big-M values can also be provided to the :code:`Bilevel::KKT` optimizer. Here is an example.

.. code::

    high_point_relaxation.use(
        Bilevel::KKT(description)
            .with_single_level_optimizer(Gurobi())
            .with_big_M(big_M)
    );

Calling the :code:`optimize` method on the model will solve the KKT reformulation of the bilevel problem.

The Strong-duality Reformulation
--------------------------------

Another single-level reformulation is the so-called strong-duality reformulation which reads

.. math::

    \begin{align}
        \min_{x,y,\lambda} \quad & c^\top x + d^\top y \\
        \text{s.t.} \quad & Ax + By \ge a, \\
        & Dy \ge b - Cx, \\
        & D^\top \lambda = f, \;  \lambda \ge 0, \\
        & f^\top y \le (b - Cx)^\top \lambda.
    \end{align}


Getting the Reformulated Model
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The strong-duality reformulation can be obtained using the :code:`Bilevel::StrongDuality::make_model` function.
This function works similarly to the :code:`Bilevel::KKT::make_model` function.
Here is an example.

.. code::

    auto single_level_model = Bilevel::StrongDuality::make_model(
        high_point_relaxation, description
        );
    single_level_model.use(Gurobi());
    single_level_model.optimize();

Note that here, the strong-duality reformulation is a nonlinear problem because of the complementarity constraint.
Hence, we use the nonlinear solving features of Gurobi here.

Solving the Strong-duality Reformulation directly
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Similarly to the KKT reformulation, idol also provides a function to solve the strong-duality reformulation directly.
This can be done by using the :code:`Bilevel::StrongDuality` optimizer as follows.

.. code::

    high_point_relaxation.use(
        Bilevel::StrongDuality(description) + Gurobi()
    );
    high_point_relaxation.optimize();
