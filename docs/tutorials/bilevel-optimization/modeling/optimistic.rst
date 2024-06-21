.. _tutorial_optimistic_bilevel:

Modeling an Optimistic Bilevel Problem
======================================

Introduction
------------

In this tutorial, we will see how to model an optimistic bilevel problem in idol.
To this end, we consider the bilevel problem

.. math::

    \begin{align}
        \min_{x, y} \ & -x + -10 y \\
        \text{s.t.} \ & x \in \mathbb Z_+ \\
        & y\in
            \begin{array}[t]{l}
                \displaystyle \underset{y}{\text{arg min}} \ & y \\
                \text{s.t.} \ & -25 x + 20 y \leq 30, \\
                & x + 2 y \leq 10, \\
                & 2 x - y \leq 15, \\
                & 2 x + 10 y \geq 15, \\
                & y \geq 0, \\
                & y \in \mathbb Z_+.
            \end{array}
    \end{align}

This MILP-MILP bilevel problem is taken from :cite:`Moore1990`.

To model this problem in idol, there are three main steps:

1. Define the high-point relaxation (HPR) model, i.e., in our example,

    .. math::

        \begin{align}
            \min_{x, y} \ & -x + -10 y \\
            \text{s.t.} \ & x \in \mathbb Z_+ \\
            & -25 x + 20 y \leq 30, \\
            & x + 2 y \leq 10, \\
            & 2 x - y \leq 15, \\
            & 2 x + 10 y \geq 15, \\
            & y \geq 0, \\
            & y \in \mathbb Z_+.
        \end{align}

    This model defines all constraints and variables of the bilevel problem.
    The feasible region is classically called the shared-constraint set.

2. Describe which variables and constraints are part of the lower-level problem.

3. Define the lower-level objective function.

Modeling the High-Point Relaxation
----------------------------------

The HPR can be modeled in the same way as a classical optimization problem.
If you are not familiar with modeling optimization problems in idol,
we recommend you to read the tutorial on :ref:`MIP modeling <mip_modeling>`.

Here is the code to model the HPR of the bilevel problem.

.. code::

    Env env;
    Model high_point_relaxation(env);

    auto x = high_point_relaxation.add_var(0, Inf, Integer, "x");
    auto y = high_point_relaxation.add_var(0, Inf, Integer, "y");

    high_point_relaxation.set_obj_expr(-x - 10 * y);
    auto follower_c1 = high_point_relaxation.add_ctr(-25 * x + 20 * y <= 30);
    auto follower_c2 = high_point_relaxation.add_ctr(x + 2 * y <= 10);
    auto follower_c3 = high_point_relaxation.add_ctr(2 * x - y <= 15);
    auto follower_c4 = high_point_relaxation.add_ctr(2 * x + 10 * y >= 15);

Describing the Lower-Level Problem
----------------------------------

To describe the lower-level problem, we need to specify which variables and constraints are part of the lower-level problem.
This done by creating an object of type :code:`Bilevel::LowerLevelDescription` and calling the methods :code:`set_follower_var`
and :code:`set_follower_ctr`.

.. code::

    Bilevel::LowerLevelDescription description(env);
    description.set_follower_var(y);
    description.set_follower_ctr(follower_c1);
    description.set_follower_ctr(follower_c2);
    description.set_follower_ctr(follower_c3);
    description.set_follower_ctr(follower_c4);

Note that this does nothing more but to create two new :code:`Annotation<Var|Ctr>` to indicate variables and constraints that are part of the lower-level problem.
These annotations are used by the bilevel solver to identify the lower-level problem.
In particular, all variables and constraints that are not annotated as follower variables or constraints are considered as leader variables and constraints
and have an annotation which is equal to :code:`MasterId`.

Also note that it is possible to create and use your own annotation. For instance, the following code is equivalent to the previous one.


.. code::

    Annotation<Var> follower_vars(env, MasterId, "follower_variables");
    y.set(follower_vars, 0);

    Annotation<Ctr> follower_ctrs(env, MasterId, "follower_constraints");
    follower_c1.set(follower_ctrs, 0);
    follower_c2.set(follower_ctrs, 0);
    follower_c3.set(follower_ctrs, 0);
    follower_c4.set(follower_ctrs, 0);


    Bilevel::LowerLevelDescription description(follower_vars, follower_ctrs);


Defining the Lower-Level Objective Function
-------------------------------------------

Finally, we need to define the lower-level objective function.
This is done by calling the method :code:`set_follower_obj_expr` on the object of type :code:`Bilevel::LowerLevelDescription`.
An :code:`Expr` object is passed as argument to this method.

.. code::

    description.set_follower_obj_expr(y);

Complete Example
----------------

A complete example is available :ref:`here <example_mibs>`.
