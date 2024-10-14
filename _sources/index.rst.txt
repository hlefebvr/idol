.. raw:: html

    <div style="padding-bottom:25px;margin-bottom:25px;border-bottom:1px solid #eeebee;">

        <h1 style="font-size:2.5em;">A C++ framework for optimization</h1>

        <h2 style="font-size:1.5em;color:gray;">
            Idol is a powerful and flexible library designed to help you build new mathematical optimization algorithms and solve more and more challenging problems efficiently.
        </h2>

        <span style="font-size:1.2em;">
            <a href="tutorials/index.html" class="my-button" id="get-started" > Get started now </a>
            &nbsp;
            <a href="installation/index.html" class="my-button"> To installation guidelines </a>
        </span>

    </div>

What Is Idol?
-------------

Idol is a C++ library for **mathematical optimization** and complex decision making. It is designed to help you build new
algorithms easily for solving more and more challenging problems. It is a versatile and powerful tool that can be
used to solve a wide range of optimization problems, including mixed-integer linear programming (MILP), quadratically constrained
problems (MIQCQP and MIQP), bilevel problems (BO), robust optimization problems (RO and ARO) and many more.

It is designed to be easy to use and to integrate with other software, so that you can use it
to build new algorithms and solve new problems without having to start from scratch. For instance, here is a piece of
code which create a Branch-and-Price algorithm.

.. code::

    const auto branch_and_price = branch_and_bound + column_generation;

Here, `branch_and_bound` and `column_generation` are two objects which are
combined to create a new algorithm, `branch_and_price`, which can be instantiated and executed.

Getting Started With Idol
-------------------------

If you are new to idol, be sure to have a look at our :ref:`tutorials <tutorials>`. If you want to get a fast hands-on
start, have a look at our :ref:`local installation guideline <installation_local>`. It is the easiest installation process
one could think of since it automatically downloads the latest version of idol, and installs it locally in a sub-folder.
It's really a mater of seconds before you can start using idol.

.. admonition:: Example

    Idol has a user-friendly interface which should look natural to people working in optimization. For instance,
    here is how one solves a Knapsack Problem using the `Gurobi <https://www.gurobi.com/>`_ solver.

    .. code:: cpp

        using namespace idol;

        const unsigned int n_items = 5;
        const double[] profit = { 40., 50., 100., 95., 30., };
        const double[] weight = { 2., 3.14, 1.98, 5., 3., };
        const double capacity = 10.;

        Env env;

        Model model(env);

        const auto x = model.add_vars(Dim<1>(n_items), 0., 1., Binary, "x");

        model.add_ctr(idol_Sum(j, Range(n_items), weight[j] * x[j]) <= capacity);

        model.set_obj_expr(idol_Sum(j, Range(n_items), -profit[j] * x[j]);

        model.use(Gurobi());

        model.optimize();

Is This a MIP Solver?
---------------------

The idol library is not a MIP solver in itself. Indeed, it typically needs to call external
solvers (e.g., `GLPK <https://www.gnu.org/software/glpk/>`_, `Gurobi <https://www.gurobi.com/>`_ or many others)
as a sub-routine of more complex algorithmic schemes (e.g., within a Branch-and-Price scheme).

The idea is to work hand in hand with existing fine-tuned and well-engineered optimization
software to enhance their possibilities. By doing so, we aim at solving larger and more complex problems than what is currently possible,
or even problems for which theoretical evidence indicate that no MIP of reasonable size can model them completely (e.g., :math:`\Sigma_i^P`-hard problems).

Even though idol is not a MIP solver, it can be used to solve standalone LPs, QPs, MILPs, MIQPs, and MIQCQPs through the
many external solvers it interfaces with.
A clear advantage of using idol to solve these problems is that it offers a unified and common interface to every solver.
Thus, you can write your code once and test it with different solvers!
The following solvers are currently supported by idol

* `Gurobi <https://www.gurobi.com/>`_
* `Mosek <https://www.mosek.com/>`_
* `GLPK <https://www.gnu.org/software/glpk/>`_
* `HiGHS <https://highs.dev/>`_
* `COIN-OR/Osi <https://github.com/coin-or/Osi>`_ which then gives you access to CPLEX, Symphony or any Osi compatible solver.

Current Features
----------------

Mixed-Integer Optimization
^^^^^^^^^^^^^^^^^^^^^^^^^^

Interfacing External Optimization Solvers
"""""""""""""""""""""""""""""""""""""""""

The idol library can be used to interface external solvers like `GLPK <https://www.gnu.org/software/glpk/>`_
or `Gurobi <https://www.gurobi.com/>`_ to solve standalone LPs, QPs, MILPs, MIQPs, and MIQCQPs.

It also provides classical callbacks one would need to implement when solving hard problems like generating user cuts
or lazy constraints. Simply give idol the separation model and let it handle the rest.

Branch-and-Bound Algorithms
"""""""""""""""""""""""""""

Idol can be used to create a custom branch-and-bound scheme very easily in which virtually anything can be tweaked.

* **Node-agnostic:** The B&B algorithm can work with any node type as long as the user properly
  defines how nodes must be updated. A default node type is already implemented for classical variable branching. This
  node type can also be inherited, e.g., to enrich the information carried out from each parent node to its children.
* **Customizable branching rules:** The default branching rules include: most infeasible, least infeasible, first
  infeasible found, uniformly random, strong branching (with phases) and pseudo-cost branching. The user can create its
  own branching rule if needed.
* **Customizable node selection rules:** The default node selection rules include: best estimate, best bound, worst bound,
  depth first, breadth first. The user can create its own node selection rule if needed.
* **Callbacks:** The user can specify its own callback to locally or globally modify a node's problem, submit heuristic
  solutions, or influence the execution of the overall tree search.
* **Sub-tree exploration**. The B&B algorithm supports sub-trees exploration to reach valid solutions to the original
  problem as quick as possible.

Column Generation and Dantzig-Wolfe Decomposition
"""""""""""""""""""""""""""""""""""""""""""""""""

Idol can be used to build and custom CG algorithms with the following features.

* **Infeasible master problem procedures:** Idol can handle infeasible master problems by resorting to Farkas pricing or
  by adding artificial variables to the original problem (if this fails, idol will automatically switch to pure phase I
  so as to exactly prove infeasibility).
* **Automatic pool clean up:** If the master problem contains too many variables, they can be automatically removed.
  This behavior is controlled by user parameters.
* **Stabilization via dual price smoothing:** The CG procedure can be stabilized using dual price smoothing controlled
  by user inputs. Default implementation include `Wentges (1997) <https://doi.org/10.1016/S0969-6016(97)00001-4>`_ and
  `Neame (2000) <https://scholar.google.com/scholar?&q=Neame%2C%20P.J.%3A%20Nonsmooth%20Dual%20Methods%20in%20Integer%20Programming.%20PhD%20thesis%20%281999%29>`_.
* **Parallel pricing:** Each pricing problem can be solved in parallel.
* **Nested column generation:** A key idea of idol is that every optimizer is seen as a black-box to solve a given optimization model.
  As such, a CG algorithm can easily be "plugged in" any algorithmic phase. For instance, solving the pricing of
  a CG algorithm can be done by yet another CG algorithm.

Bilevel Optimization
^^^^^^^^^^^^^^^^^^^^

Interfacing External Optimization Solvers
"""""""""""""""""""""""""""""""""""""""""

Currently, idol can interface with the mixed-integer bilevel solver `coin-or/MibS <https://github.com/coin-or/MibS>`_.

Robust Optimization
^^^^^^^^^^^^^^^^^^^^

Column-and-Constraint Generation
""""""""""""""""""""""""""""""""

Idol can be used to build and custom CCG algorithms with the following features.

* **Separation Sub-Routine**: Different separation strategies are implemented in idol to solve the separation problem in
  the CCG algorithm. Most importantly, any bilevel solver can be used to solve the max-min separation problem. If needed, the user can also easily implement its own separation strategy.
* **Trust Region Stabilization**: The CCG algorithm can be stabilized using trust region if the first-stage decisions are binary.

Table of Contents
-----------------

.. toctree::
    :maxdepth: 1

    installation/index
    tutorials/index
    examples/index
    api/index
    faq/index
    benchmark/index
    developer_guide/index
    references
