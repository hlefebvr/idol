.. _decomposition_dantzig_wolfe:

.. role:: cpp(code)
   :language: cpp

Dantzig-Wolfe decomposition
===========================

In this section, we will show how to use the Branch-and-Price solver to solve the *Generalized Assignment Problem* (GAP)
using an external solver to solve each sub-problem.

.. hint::

    This tutorial regards the `advanced topic` of Column Generation and Dantzig-Wolfe decomposition.
    Rudimentary notions in the following subjects are recommended:

    - `Column Generation and Branch-and-Price algorithmd <https://en.wikipedia.org/wiki/Column_generation>`_
    - `Dantzig-Wolfe decomposition <https://en.wikipedia.org/wiki/Dantzig%E2%80%93Wolfe_decomposition>`_
    - `Generalized Assignment Problem <https://en.wikipedia.org/wiki/Generalized_assignment_problem>`_.

.. seealso::

    A `Benchmark on Generalized Assignemnt Problem <https://hlefebvr.github.io/idol_benchmark/GAP.render.html>`_ is
    available.
    This includes a comparison with `Coluna.jl <https://github.com/atoptima/Coluna.jl>`_.

Mathematical models
-------------------

In this section, we assume that the reader is familiar with GAP.
Let :math:`m` be a given of agents and let :math:`n` be a set of tasks to perform. Let :math:`c_{ij}` be the cost for
assigning task :math:`j` to agent :math:`i`, :math:`w_{ij}` be the resource consumption of task :math:`j` when performed
by agent :math:`i` and let :math:`t_i` be the resource capacity of agent :math:`i`.

Direct model
^^^^^^^^^^^^

The Generalized Assignment Problem (GAP) can be modeled as

.. math::

    \min \ & \sum_{i=1}^m\sum_{j=1}^n c_{ij} x_{ij} \\
    \textrm{s.t. } & \sum_{j=1}^n w_{ij} x_{ij} \le t_i & i=1,...,m \\
    & \sum_{i=1}^m x_{ij} = 1 & j = 1,...,n \\
    & x_{ij}\in\{0,1\} & i=1,...,m, j=1,...,n

Here, variable :math:`x_{ij}` encodes the assignment decision and equals 1 if and only if task :math:`j` is assigned to
agent :math:`i`.

Dantzig-Wolfe reformulation
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Let us enumerate the list of all feasible assignments, i.e., let

.. math::

    \{\bar x^e_{ij} \}_{e\in E} = \{ x \in \{ 0,1 \}^{mn} : \sum_{j=1}^n w_{ij}x_{ij} \le t_i \quad i=1,...,m \},

where :math:`E` denotes a list for their indices. The Dantzig-Wolfe reformulation of GAP reads

.. math::

    \min \ & \sum_{e\in E} \lambda_e\left( \sum_{i=1}^m\sum_{j=1}^n c_{ij}\bar x_{ij}^e \right) \\
    \textrm{s.t. } & \sum_{e\in E} \lambda_e \left( \sum_{i=1}^m \bar x_{ij}^e \right) = 1 & j=1,...,n \\
    & \sum_{e\in E} \lambda_e = 1 \\
    & \lambda_e \in \{ 0, 1 \} & \forall e\in E

Though this model contains an exponential number of variables (i.e., columns) it can be solved efficiently using
Column Generation and Branch-and-price. In such case, the pricing problem is a Knapsack Problem.

Automatic reformulation in idol
-------------------------------

The simplest way to solve a problem using column generation and idol is through its automatic reformulation feature.
To use this, one simply needs to give the *original space* formulation of the problem (here, the *so-called* direct model)
and to indicate which constraints should be moved to the pricing problem (here, the knapsack constraints).

Implementing the direct model with idol
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To read an instance for GAP, we first need to include the header file located in :code:`"idol/problems/generalized-assignment-problem/GAP_Instance.h"`.
This will allow us to use idol's instance parser for GAP.
Assuming that the instance file is named :code:`instance.txt`, it can be loaded as follows.

.. code-block:: cpp

    const auto instance = Problems::GAP::read_instance("instance.txt");

    const unsigned int n_agents = instance.n_agents();
    const unsigned int n_jobs = instance.n_jobs();

We are now ready to model our problem (for more details, refer to :ref:`this tutorial on modeling <modeling_optimization_problems>`)

.. code-block:: cpp

    // Create optimization environment
    Env env;

    // Create model
    Model model(env);

    // Create assignment variables (x_ij binaries)
    auto x = model.add_vars(Dim<2>(n_agents, n_jobs), 0., 1., Binary, "x");

    // Create knapsack constraints (i.e., capacity constraints)
    for (unsigned int i = 0 ; i < n_agents ; ++i) {
        model.add_ctr(idol_Sum(j, Range(n_jobs), instance.resource_consumption(i, j) * x[i][j]) <= instance.capacity(i), "capacity_" + std::to_string(i));
    }

    // Create assignment constraints
    for (unsigned int j = 0 ; j < n_jobs ; ++j) {
        model.add(idol_Sum(i, Range(n_agents), x[i][j]) == 1, "assignment_" + std::to_string(j));
    }

    // Set the objective function
    model.set_obj_expr(idol_Sum(i, Range(n_agents), idol_Sum(j, Range(n_jobs), instance.cost(i, j) * x[i][j])));

Giving decomposition instructions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

We are now at the crucial step of indicating which constraint should be moved to the pricing problem. In idol, this is done by using
*annotations*. Annotations are additional information associated to an optimization object (e.g., a constraint or a variable).
Note that annotations are global, i.e., they do not relate to a given optimization model.

Every annotation is formed with two template arguments: an optimization object type and a value type. Here, we want to add
annotations to constraints, thus, the optimization object type must be :code:`Ctr`. The value type can typically be any desired
type. Here, however, the Dantzig-Wolfe decomposition needs an annotation corresponding to an :code:`unsigned int`. Thus, we create
the annotation as follows.

.. code-block:: cpp

     Annotation<Ctr, unsigned int> decomposition(env, "decomposition", MasterId);

Here, we pass three arguments to the constructor of :code:`Annotation<Ctr, unsigned int>`. First, we pass the optimization
environment which will store the annotation. Then, a name (mandatory) is given to the annotation, here, "decomposition".
Finally, a default value is given and equals "MasterId". This will tell idol that constraints which have not been annotated
should remain in the master problem.

Now, observe how the annotation is applied to the capacity constraints.

.. code:: cpp

    for (unsigned int i = 0 ; i < n_agents ; ++i) {
        Ctr capacity = model.add_ctr(idol_Sum(j, Range(n_jobs), instance.resource_consumption(i, j) * x[i][j]) <= instance.capacity(i), "capacity_" + std::to_string(i));
        capacity.set(decomposition, i); // <-- Annotating the capacity constraint
    }

Here, the first capacity constraint is moved to the first pricing problem (id: 0),
the second constraint to the second pricing problem (id: 1), and so on.

Note that another decomposition would be materialized as follows.


.. code:: cpp

    for (unsigned int i = 0 ; i < n_agents ; ++i) {
        Ctr capacity = model.add_ctr(idol_Sum(j, Range(n_jobs), instance.resource_consumption(i, j) * x[i][j]) <= instance.capacity(i), "capacity_" + std::to_string(i));
        capacity.set(decomposition, 0); // <-- Annotating the capacity constraint
    }

Here, all the knapsack constraints are moved to the same pricing problem (id: 0).

Decomposing and solving the model
---------------------------------

Now that the desired decomposition has been specified, we can specify the desired optimizer to solve our model.
Here, we want to solve our problem using a branch-and-price algorithm, i.e., a branch-and-bound algorithm where each relaxation
in the branch-and-bound tree is solved by a Dantzig-Wolfe decomposition. This is done as follows.

.. code:: cpp

    model.use(
        /* The overall algorithm is a branch-and-bound */
        BranchAndBound()

            /* Each node is solved with a Dantzig-Wolfe decomposition algorithm */
            .with_node_optimizer(

                /* The annotation "decomposition" is used to automatically decompose the problem */
                DantzigWolfeDecomposition(decomposition1)

                    /* The master problem is solved using Gurobi */
                    .with_master_optimizer(Gurobi::ContinuousRelaxation())

                    /* Each pricing problem is solved by Gurobi as well */
                    .with_pricing_optimizer(Gurobi())

            )

            /* Variables are selected for branching using the most-infeasible rule */
            .with_branching_rule(MostInfeasible())

            /* Nodes are selected using the best-bound rule */
            .with_node_selection_rule(BestBound()

            /* The algorithm will run with a time limit of 3600 */
            .with_time_limit(3600)

    );

Then, one can simply call the :code:`Model::optimize` method as follows.

.. code:: cpp

    model.optimize();


That's it! The problem is being solved by column generation, and possibly branching on fractional variables.


.. hint::

    Note that it is possible to obtain logs using the `with_log_level` method on the desired optimizer.
    For instance, one may want to have
    logs for the branch-and-bound optimizer. Then, one should do as follows.

    .. code:: cpp

        model.use(
            BranchAndBound()

                /* ... omitting identical details */

                .with_log_level(Info, Blue)
        );

The rest remains unchanged and one can use :code:`Model::optimize` to solve the problem and retrieve the solution
through methods like :code:`Model::get_status` and :code:`Model::get_var_primal`.

.. admonition:: Example

    Here, we can solve our model using a Dantzig-Wolfe decomposition.

    .. code-block::

        model.optimize();

        std::cout << save_primal(model) << std::endl;

    This will produce the following output (e.g.).

    .. code-block:: text

        [2023-04-07 13:45:44]   [info]  [dantzig-wolfe] <Type=Master> <Iter=0> <TimT=0.00> <TimI=0.00> <Stat=Optimal> <Reas=Proved> <ObjVal=110000> <NGen=0> <BestBnd=-inf> <BestObj=110000.00> <RGap=90909090909090928.00 %> <AGap=+inf>
        [2023-04-07 13:45:44]   [info]  [dantzig-wolfe] <Type=Pricing> <Iter=0> <TimT=0.00> <TimI=0.00> <Stat=Optimal> <Reas=Proved> <Obj=-50093.00000> <NGen=0> <BestBnd=-inf> <BestObj=110000.00000> <RGap=90909090909090928.00000 %> <AGap=+inf>
        [2023-04-07 13:45:44]   [info]  [dantzig-wolfe] <Type=Pricing> <Iter=0> <TimT=0.00> <TimI=0.00> <Stat=Optimal> <Reas=Proved> <Obj=-40096.00000> <NGen=0> <BestBnd=-inf> <BestObj=110000.00000> <RGap=90909090909090928.00000 %> <AGap=+inf>
        [2023-04-07 13:45:44]   [info]  [dantzig-wolfe] <Type=Pricing> <Iter=0> <TimT=0.00> <TimI=0.00> <Stat=Optimal> <Reas=Proved> <Obj=-30068.00000> <NGen=0> <BestBnd=-inf> <BestObj=110000.00000> <RGap=90909090909090928.00000 %> <AGap=+inf>
        [2023-04-07 13:45:44]   [info]  [dantzig-wolfe] <Type=Master> <Iter=10> <TimT=0.01> <TimI=0.00> <Stat=Optimal> <Reas=Proved> <ObjVal=-242.5> <NGen=1> <BestBnd=-277.86> <BestObj=-242.50> <RGap=14.58 %> <AGap=35.36>
        [2023-04-07 13:45:44]   [info]  [dantzig-wolfe] <Type=Pricing> <Iter=10> <TimT=0.01> <TimI=0.00> <Stat=Optimal> <Reas=Proved> <Obj=-7.90269> <NGen=1> <BestBnd=-277.85653> <BestObj=-242.50000> <RGap=14.58001 %> <AGap=35.35653>
        [2023-04-07 13:45:44]   [info]  [dantzig-wolfe] <Type=Pricing> <Iter=10> <TimT=0.01> <TimI=0.00> <Stat=Optimal> <Reas=Proved> <Obj=-14.49817> <NGen=1> <BestBnd=-277.85653> <BestObj=-242.50000> <RGap=14.58001 %> <AGap=35.35653>
        [2023-04-07 13:45:44]   [info]  [dantzig-wolfe] <Type=Pricing> <Iter=10> <TimT=0.01> <TimI=0.00> <Stat=Optimal> <Reas=Proved> <Obj=-1.36131> <NGen=1> <BestBnd=-277.85653> <BestObj=-242.50000> <RGap=14.58001 %> <AGap=35.35653>
        [2023-04-07 13:45:44]   [info]  [dantzig-wolfe] <Type=Master> <Iter=16> <TimT=0.02> <TimI=0.00> <Stat=Optimal> <Reas=Proved> <ObjVal=-242.5> <NGen=0> <BestBnd=-242.51> <BestObj=-242.50> <RGap=0.00 %> <AGap=0.01>
        [2023-04-07 13:45:44]   [info]  [branch-and-bound]      <Node=   0 > <TimT=   0.02> <TimI=   0.02> <Levl=  0> <Stat=1> <Reas=0> <ObjVal=  -242.50> <BestBnd=     -inf> <BestObj=     +inf> <RelGap=200.00> <AbsGap= +inf>
        [2023-04-07 13:45:44]   [info]  [branch-and-bound]      <Node=   H-> <TimT=   0.02> <TimI=   0.02> <Levl=  0> <Stat=1> <Reas=1> <ObjVal=  -233.00> <BestBnd=     -inf> <BestObj=  -233.00> <RelGap=42918454935603896320.00> <AbsGap= +inf>
        [2023-04-07 13:45:44]   [info]  [branch-and-bound]      <Node=   0+> <TimT=   0.02> <TimI=   0.02> <Levl=  0> <Stat=1> <Reas=0> <ObjVal=  -242.50> <BestBnd=  -242.50> <BestObj=  -233.00> <RelGap= 4.08> <AbsGap= 9.50>
        [2023-04-07 13:45:44]   [info]  [dantzig-wolfe] <Type=Master> <Iter=0> <TimT=0.00> <TimI=0.00> <Stat=Optimal> <Reas=Proved> <ObjVal=-233> <NGen=0> <BestBnd=-inf> <BestObj=-233.00> <RGap=42918454935603896320.00 %> <AGap=+inf>
        [2023-04-07 13:45:44]   [info]  [dantzig-wolfe] <Type=Pricing> <Iter=0> <TimT=0.00> <TimI=0.00> <Stat=Optimal> <Reas=Proved> <Obj=-22579.50000> <NGen=0> <BestBnd=-inf> <BestObj=-233.00000> <RGap=42918454935603896320.00000 %> <AGap=+inf>
        [2023-04-07 13:45:44]   [info]  [dantzig-wolfe] <Type=Pricing> <Iter=0> <TimT=0.00> <TimI=0.00> <Stat=Optimal> <Reas=Proved> <Obj=-17547.50000> <NGen=0> <BestBnd=-inf> <BestObj=-233.00000> <RGap=42918454935603896320.00000 %> <AGap=+inf>
        [2023-04-07 13:45:44]   [info]  [dantzig-wolfe] <Type=Pricing> <Iter=0> <TimT=0.00> <TimI=0.00> <Stat=Optimal> <Reas=Proved> <Obj=-17552.50000> <NGen=0> <BestBnd=-inf> <BestObj=-233.00000> <RGap=42918454935603896320.00000 %> <AGap=+inf>
        [2023-04-07 13:45:44]   [info]  [dantzig-wolfe] <Type=Master> <Iter=10> <TimT=0.01> <TimI=0.00> <Stat=Optimal> <Reas=Proved> <ObjVal=-233> <NGen=2> <BestBnd=-235.81> <BestObj=-233.00> <RGap=1.21 %> <AGap=2.81>
        [2023-04-07 13:45:44]   [info]  [dantzig-wolfe] <Type=Pricing> <Iter=10> <TimT=0.01> <TimI=0.00> <Stat=Optimal> <Reas=Proved> <Obj=-0.25760> <NGen=2> <BestBnd=-235.80933> <BestObj=-233.00000> <RGap=1.20572 %> <AGap=2.80933>
        [2023-04-07 13:45:44]   [info]  [dantzig-wolfe] <Type=Pricing> <Iter=10> <TimT=0.01> <TimI=0.00> <Stat=Optimal> <Reas=Proved> <Obj=-0.22397> <NGen=2> <BestBnd=-235.80933> <BestObj=-233.00000> <RGap=1.20572 %> <AGap=2.80933>
        [2023-04-07 13:45:44]   [info]  [dantzig-wolfe] <Type=Pricing> <Iter=10> <TimT=0.01> <TimI=0.00> <Stat=Optimal> <Reas=Proved> <Obj=-0.36123> <NGen=2> <BestBnd=-235.80933> <BestObj=-233.00000> <RGap=1.20572 %> <AGap=2.80933>
        [2023-04-07 13:45:44]   [info]  [dantzig-wolfe] <Type=Master> <Iter=13> <TimT=0.01> <TimI=0.00> <Stat=Optimal> <Reas=Proved> <ObjVal=-233> <NGen=2> <BestBnd=-233.02> <BestObj=-233.00> <RGap=0.01 %> <AGap=0.02>
        [2023-04-07 13:45:44]   [info]  [dantzig-wolfe] <Type=Master> <Iter=0> <TimT=0.00> <TimI=0.00> <Stat=Optimal> <Reas=Proved> <ObjVal=4747> <NGen=0> <BestBnd=-inf> <BestObj=4747.00> <RGap=2106593638087168512.00 %> <AGap=+inf>
        [2023-04-07 13:45:44]   [info]  [dantzig-wolfe] <Type=Pricing> <Iter=0> <TimT=0.00> <TimI=0.00> <Stat=Optimal> <Reas=Proved> <Obj=-12451.00000> <NGen=0> <BestBnd=-inf> <BestObj=4747.00000> <RGap=2106593638087168512.00000 %> <AGap=+inf>
        [2023-04-07 13:45:44]   [info]  [dantzig-wolfe] <Type=Pricing> <Iter=0> <TimT=0.00> <TimI=0.00> <Stat=Optimal> <Reas=Proved> <Obj=-7492.00000> <NGen=0> <BestBnd=-inf> <BestObj=4747.00000> <RGap=2106593638087168512.00000 %> <AGap=+inf>
        [2023-04-07 13:45:44]   [info]  [dantzig-wolfe] <Type=Pricing> <Iter=0> <TimT=0.00> <TimI=0.00> <Stat=Optimal> <Reas=Proved> <Obj=0.00000> <NGen=0> <BestBnd=-inf> <BestObj=4747.00000> <RGap=2106593638087168512.00000 %> <AGap=+inf>
        [2023-04-07 13:45:44]   [info]  [dantzig-wolfe] <Type=Master> <Iter=8> <TimT=0.01> <TimI=0.00> <Stat=Optimal> <Reas=Proved> <ObjVal=-218> <NGen=1> <BestBnd=-224.13> <BestObj=-218.00> <RGap=2.81 %> <AGap=6.13>

        +-----------------------
        | Status: Optimal
        | Reason: Proved
        | ObjVal: -233.00
        | Values:
        | 	x_1_7 = 1.00
        | 	x_2_0 = 1.00
        | 	x_2_1 = 1.00
        | 	x_1_4 = 1.00
        | 	x_1_5 = 1.00
        | 	x_0_6 = 1.00
        | 	x_0_2 = 1.00
        | 	x_0_3 = 1.00
        +-----------------------
