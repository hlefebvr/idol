.. _modeling_two_stage_robust_problem:

Modeling a Two-Stage Robust Problem
===================================

In this tutorial, we will see how to model a two-stage robust problem in idol.

To follow this tutorial, you should be familiar with two-stage robust optimization and modeling optimization problems in idol.
If this is not the case, we recommend you to read the tutorial on :ref:`MIP modeling <mip_modeling>`.

.. contents:: Table of Contents
    :local:
    :depth: 2

A Two-Stage Robust Facility Location Problem
--------------------------------------------

We consider a two-stage robust facility location problem (FLP) where demands are uncertain.

Given a set of potential facility locations :math:`V_1` and a set of customers :math:`V_2`, the goal is to select a subset of facility locations
to activate in order to serve all customers' demand, while minimizing the total cost.
This version introduces uncertainty in the customers' demands.

Note that there is also an example for the :ref:`deterministic version of the FLP using Column Generation <example_flp>`.

Each facility :math:`i\in V_1` has an opening cost :math:`f_i` and a maximum capacity :math:`q_i`.
Each customer :math:`j\in V_2` has a demand :math:`d_j`.
The unitary cost for serving customer :math:`j\in V_2` from facility :math:`i\in V_1` is :math:`t_{ij}`.
The uncertainty in customer demands is controlled by a parameter :math:`\Gamma`.

In this robust variant, we consider that the demands are uncertain and can be expressed as :math:`d_j(\xi) = d_j(1 + p\xi_j)`
with :math:`p` being the maximum increase in demand and :math:`\xi` being an unknown vector taken in the uncertainty set

.. math::

    \Xi := \left\{ \xi\in[ 0, 1 ]^{|V_2|} : \sum_{j\in V_2} \xi_j \le \Gamma \right\}.

We model the two-stage robust FLP as

.. math::

    \min_{x\in \{0,1\}^{|V_1|}} \ \left\{ \sum_{i\in V_1} f_i x_i + \max_{\xi\in \Xi} \ \min_{y\in Y(x,\xi)} \  \sum_{i\in V_1} \sum_{j\in V_2} t_{ij} y_{ij} \right\}

where :math:`Y(x,\xi)` is the set of feasible solutions for the second stage problem, given the first stage solution :math:`x` and the realization :math:`\xi` of the uncertain demand vector.
It is defined as the set of vectors :math:`y\in \mathbb{R}^{|V_1|\times|V_2|}` that satisfy the following constraints

.. math::

    \begin{align*}
        & \sum_{i\in V_1} y_{ij} = d_j(\xi) && j\in V_2, \\
        & \sum_{j\in V_2} y_{ij} \le q_i x_i && i\in V_1, \\
        & y_{ij} \ge 0 && i\in V_1, j\in V_2.
    \end{align*}

In what follows, we will assume that we have a variable :code:`instance` of type :code:`idol::Problems::FLP::Instance`
that contains the data of the problem. Most typically, you will want to read the instance from a file. This is done as follows.

.. code::

    // Read instance
    const auto instance = Problems::FLP::read_instance_1991_Cornuejols_eal("robust_ccg.data.txt");

Additionally, we define an optimization environment :code:`env` and some parameters.

.. code::

    Env env;

    const double Gamma = 3;
    const double percentage_increase = .2;

Modeling Steps
--------------

To model a two-stage robust problem, one needs to perform the following steps:

1. Define an uncertainty set :math:`\Xi`.
2. Define the deterministic model in which :math:`\xi` is a parameter.
3. Assign a stage to each variable and constraint.

Let's see how these steps are done in idol.

Defining the Uncertainty Set
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Modeling the uncertainty set :math:`\Xi` is done in the same way as modeling any classical optimization problem.
For instance, one can do as follows.

.. code::

    const unsigned int n_customers = instance.n_customers();

    Model uncertainty_set(env);

    auto xi = uncertainty_set.add_vars(Dim<1>(n_customers), 0., 1, Binary, "xi");
    uncertainty_set.add_ctr(idol_Sum(j, Range(n_customers), xi[j]) <= Gamma);

Note that defining an objective function is not necessary since the uncertainty set is not optimized over.
If an objective function is defined, it will be ignored by the optimizer.

Defining the Deterministic Model
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The deterministic model underlying the two-stage robust FLP is the same as the classical FLP, except that the demand is seen as a parameter.

Recall that a variable, e.g., :code:`xi[0]`, can be turned into a parameter by prepending it with :code:`!`.

Hence,
we can define the deterministic model as follows.

.. code::

    const unsigned int n_facilities = instance.n_facilities();

    Model model(env);

    const auto x = model.add_vars(Dim<1>(n_facilities), 0., 1., Binary, "x");
    const auto y = model.add_vars(Dim<2>(n_facilities, n_customers), 0., Inf, Continuous, "y");

    // Capacity constraints
    for (unsigned int i = 0 ; i < n_facilities ; ++i) {
        model.add_ctr(idol_Sum(j, Range(n_customers), y[i][j]) <= instance.capacity(i) * x[i]);
    }

    // Demand satisfaction constraints
    for (unsigned int j = 0 ; j < n_customers ; ++j) {
        // IMPORTANT: here we use the parameter "!xi[j]" instead of the variable "xi[j]"
        model.add_ctr(idol_Sum(i, Range(n_facilities), y[i][j]) == instance.demand(j) * (1 + percentage_increase * !xi[j]));
    }

    // Objective function
    model.seobj_expr(idol_Sum(i, Range(n_facilities),
                                instance.fixed_cost(i) * x[i]
                                + idol_Sum(j, Range(n_customers),
                                           instance.per_unit_transportation_cost(i, j) * y[i][j]
                                )
                       )
    );

Assigning Stages
^^^^^^^^^^^^^^^^

The last step is to assign a stage to each variable and constraint. Here, variables :math:`x` are first-stage variables
and variables :math:`y` are second-stage variables, i.e., they depend on the realization of the uncertain demand.
Similarly, all constraints are second-stage constraints since they are part of the second-stage feasible region.

Assigning stages is done by creating a new object of type :code:`idol::Robust::StageDescription`.
Under the hood, this object does nothing more but defining new annotations for variables and constraints storing
the assigned stage of each variable and constraint. It is created as follows.

.. code::

    Robust::StageDescription stages(env);

By default, all variables and constraints are assigned to the first stage.
To assign a variable or constraint to the second stage, one can use the method :code:`set_stage` of the object :code:`stages`.
For instance, one can do as follows.

.. code::

    for (const auto& var : model.vars()) {
        if (var.name().front() != 'x') {
            stages.set_stage(var, 2);
        }
    }

Similarly, since all constraints are second-stage constraints, one can do as follows.

.. code::

    for (const auto& ctr : model.ctrs()) {
        stages.set_stage(ctr, 2);
    }

.. admonition:: About stage annotations

    Note that it is also possible to define your own annotations to assign variables and constraints to stages.
    This is a rather advanced feature and it is your responsability to ensure that the annotations are consistent with the model.

    The annotations are based on the following conventions: all first-stage variables and constraints have the annotation evaluating to :code:`MasterId`.
    All second-stage variables and constraints have the annotation evaluating to :code:`0`.

    For instance, the following code is equivalent to the previous one.

    .. code::

        Annotation<Var, unsigned int> stage_vars(model, "stage_vars", MasterId); // By default, all variables are first-stage variables
        Annotation<Ctr, unsigned int> stage_ctrs(model, "stage_ctrs", MasterId); // By default, all constraints are first-stage constraints

        for (const auto& var : model.vars()) {
            if (var.name().front() != 'x') {
                var.set(stage_vars, 0); // Assign variable to the second stage
            }
        }

        for (const auto& ctr : model.ctrs()) {
            ctr.set(stage_ctrs, 0); // Assign constraint to the second stage
        }

        idol::Robust::StageDescription stages(stage_vars, stage_ctrs);

    By doing so, a call to :code:`stages.stage(var)` will return "1" for all first-stage variables and "2" for all second-stage variables.
    The underlying annotation can be obtained using

    .. code::

        Annotation<Var, unsigned int> stage_vars = stages.stage_vars()

    Finally, also note the method :code:`stages.stage_index(var)` that will return "0" for all first-stage variables and "1" for all second-stage variables.


That's it! We have now modeled a two-stage robust FLP in idol. Note that you will now need
to attach an optimizer to the model to solve it.
To this end, be sure to check the tutorials on optimizers for two-stage robust problems, e.g., :ref:`the column-and-constraint generation tutorial <tutorial_ccg>`.

Complete Example
----------------

A complete example is given :ref:`here <example_robust_flp_ccg>`