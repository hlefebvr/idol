.. _tutorial_optimistic_bilevel:

Modeling a Bilevel Problem
==========================

In this tutorial, we will see how to model a bilevel problem in idol.

To follow this tutorial, you should be familiar with bilevel optimization and modeling optimization problems in idol.
If this is not the case, we recommend you to read the tutorial on :ref:`MIP modeling <mip_modeling>`.

.. contents:: Table of Contents
    :local:
    :depth: 2

Problem Definition and Main Steps
----------------------------------

We consider the optimistic bilevel problem

.. math::

    \begin{align}
        \min_{x, y} \quad & -x + -10 y \\
        \text{s.t.} \quad & x \in \mathbb Z_{\ge 0} \\
        & y\in
            \begin{array}[t]{rl}
                \displaystyle \underset{y}{\text{arg min}} \quad & y \\
                \text{s.t.} \quad & -25 x + 20 y \leq 30, \\
                & x + 2 y \leq 10, \\
                & 2 x - y \leq 15, \\
                & 2 x + 10 y \geq 15, \\
                & y \geq 0, \\
                & y \in \mathbb Z_{\ge 0}.
            \end{array}
    \end{align}

This is an ILP-ILP bilevel problem which is taken from :cite:`Moore1990` (Example 1).

To model this problem in idol, there are three main steps:

1. Define the high-point relaxation (HPR) model,

2. Describe which variables and constraints are part of the lower-level problem,

3. Define the lower-level objective function.

Modeling the High-Point Relaxation
----------------------------------

The HPR can be modeled in the same way as a classical optimization problem.
If you are not familiar with modeling optimization problems in idol,
we recommend you to read the tutorial on :ref:`MIP modeling <mip_modeling>`.

In our example, the HPR reads

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

Here is the code to model the HPR of the bilevel problem.

.. code::

    Env env;
    Model high_point_relaxation(env);

    auto x = high_point_relaxation.add_var(0, Inf, Integer, 0, "x");
    auto y = high_point_relaxation.add_var(0, Inf, Integer, 0, "y");

    high_point_relaxation.set_obj_expr(-x - 10 * y);
    auto lower_c1 = high_point_relaxation.add_ctr(-25*x + 20*y <= 30);
    auto lower_c2 = high_point_relaxation.add_ctr(x + 2 * y <= 10);
    auto lower_c3 = high_point_relaxation.add_ctr(2 * x - y <= 15);
    auto lower_c4 = high_point_relaxation.add_ctr(2 * x + 10 * y >= 15);

Describing the Lower-Level Problem
----------------------------------

To describe the lower-level problem, we need to specify which variables and constraints are part of it.
This is done by creating an object of type :code:`Bilevel::Description` and calling the methods :code:`make_lower_level`.

.. code::

    Bilevel::Description description(env);
    description.make_lower_level(y);
    description.make_lower_level(lower_c1);
    description.make_lower_level(lower_c2);
    description.make_lower_level(lower_c3);
    description.make_lower_level(lower_c4);

Note that this does nothing more but to create a new :code:`Annotation<unsigned int>` to indicate variables and constraints that are part of the lower-level problem.
These annotations are used by the bilevel solvers to identify the lower-level problem.
In particular, all variables and constraints that are not annotated as lower-level variables or constraints are considered as upper-level variables or constraints, respectively.
Upper-level variables and constraints have an annotation which is set to :code:`MasterId` by default.

Also note that it is possible to create and use your own annotation. For instance, the following code is equivalent to the previous one.


.. code::

    Annotation<unsigned int> lower_level(env, MasterId, "lower_level");

    y.set(lower_level, 0);
    lower_c1.set(lower_level, 0);
    lower_c2.set(lower_level, 0);
    lower_c3.set(lower_level, 0);
    lower_c4.set(lower_level, 0);


    Bilevel::Description description(lower_level);


Defining the Lower-Level Objective Function
-------------------------------------------

Finally, we need to define the lower-level objective function.
This is done by calling the method :code:`set_lower_level_obj` on the object of type :code:`Bilevel::Description`.
A :code:`QuadExpr` object is passed as argument to this method.

.. code::

    description.set_lower_level_obj(y);

Complete Example
----------------

A complete example is available :ref:`here <example_mibs>` and uses the MibS solver.
