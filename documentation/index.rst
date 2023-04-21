Welcome to idol!
================

.. image:: https://img.shields.io/github/license/hlefebvr/idol
.. image:: https://img.shields.io/github/v/tag/hlefebvr/idol?label=version
.. image:: https://img.shields.io/github/actions/workflow/status/hlefebvr/idol/cmake.yml?branch=main
.. image:: https://img.shields.io/github/issues-raw/hlefebvr/idol
.. image:: https://www.repostatus.org/badges/latest/wip.svg
.. image:: https://codecov.io/github/hlefebvr/idol/branch/main/graph/badge.svg?token=BWMH5522QP)](https://app.codecov.io/gh/hlefebvr/idol

.. toctree::
    :maxdepth: 2
    :caption: Contents:

Idol is a C++ library for implementing complex decomposition algorithms for
mathematical optimization.

Its main goal is to ease the development of new mathematical optimization techniques to
solve complex optimization problems. It offers a user-friendly interface to build new algorithms
on top of state-of-the-art methods like Dantzig-Wolfe decomposition, Benders decomposition or
column-and-constraint generation.

New to idol?
------------

If you are new to idol, I advise you to visit our :ref:`installation guideline <installation>`.
In particular, have a look at the :ref:`local installation guideline <installation_local>`.
It is the easiest installation process one could think of since it automatically downloads the latest version of idol and
installs it in a local folder. It's really a mater of seconds before you can start using idol.

.. admonition:: Example

    Idol has a user-friendly interface which should look natural to people working in optimization. For instance,
    here is how one solves a Knapsack Problem using the `Gurobi <https://www.gurobi.com/>`_ solver.

    .. code:: cpp

        const unsigned int n_items = 5;
        const double[] profit = { 40., 50., 100., 95., 30., };
        const double[] weight = { 2., 3.14, 1.98, 5., 3., };
        const double capacity = 10.;

        Env env;

        Model model(env);

        auto x = model.add_vars(Dim<1>(n_items), 0., 1., Binary, "x");

        model.add_ctr(idol_Sum(j, Range(n_items), weight[j] * x[j]) <= capacity);

        model.set_obj_expr(idol_Sum(j, Range(n_items), -profits[j] * x[j]);

        model.use(Gurobi());

        model.optimize();

If you want to learn more about idol's modeling interface, be sure to visit :ref:`beginner tutorials <basics>`.

If you are more experienced, you may want to have a look at our :ref:`decomposition methods tutorials <decomposition>`.

Is this a MIP solver?
---------------------

The idol library is not a MIP solver in itself. Indeed, it typically needs to call external
solvers (e.g., `GLPK <https://www.gnu.org/software/glpk/>`_ or `Gurobi <https://www.gurobi.com/>`_)
as a sub-routine of more complex algorithmic schemes (e.g., column generation).
The idea is to work hand in hand with existing fine-tuned and well-engineered optimization
softwares to enhance their possibilities to solve e.g., larger problems or problems which
cannot directly be modeled as MIPs (e.g., :math:`\Sigma_i^P`-hard problems).

Each subproblem is then solved by an external and dedicated solver.
Currently, the following external solvers can be interfaced:

* `Gurobi <https://www.gurobi.com/>`_;
* `Mosek <https://www.mosek.com/>`_;
* `GLPK <https://www.gnu.org/software/glpk/>`_.

Current features
----------------

Interfacing external optimization solvers
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The idol library can be used to interface external solvers like `GLPK <https://www.gnu.org/software/glpk/>`_
or `Gurobi <https://www.gurobi.com/>`_ to solve standalone MI(N)LPs. It offers a unified and common
interface to every solver including standard parameters and branch-and-bound callbacks.
Thus, you can write your code once and test it with different solvers!

Generic Branch-and-Bound algorithms
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Idol allows its user to create custom branch-and-bound scheme very easily where almost everything can be tweaked.

* *Node agnostic implementation*. The branch-and-bound algorithm can work with any node type as long as the user properly
  defines how nodes must be updated. A default node type is already implemented for classical variable branching. This
  node type can also be inherited, e.g., to enrich the information carried out from each parent node to its children.
* *Customizable branching rule*. The default branching rules include: most-infeasible rule.
* *Customizable node selection rule*. The default node selection rules include: best-bound, worst-bound, depth-first, breadth-first.
* *Callbacks*. The user can specify its own callback to locally or globally modify a node's problem, submit heuristic solutions,
  or influence the execution of the overall algorithm.
* *Branch-and-bound with phases*. Independently on any of the above settings, the B&B algorithm supports sub-trees exploration
  to reach valid solutions to the original problem as quick as possible.

**Features to come/desired features**

* *Parallelization of the algorithm*.
* *New branching rules*: pseudo-cost, strong-branching and reliability branching.
* *Default cutting planes*.

Generic Column Generation and Dantzig-Wolfe decomposition
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Idol can be used to build and custom column-generation algorithms with the following features.

* *Infeasible master problem procedures*. Idol can handle an infeasible master problem by resorting to Farkas pricing or
  by adding artificial variables to the original problem (note that if this fails, idol will then switch to pure phase I
  so as to exactly prove infeasibility).
* *Automatic pool clean up*. If the master problem contains too many variables, they can be automatically removed. This can be
  all controlled by user parameters.
* *Stabilization via dual price smoothing*. The column generation procedure can be stabilized using dual price smoothing
  controlled by user inputs.
* *Parallel pricing*. Each pricing problem can be solved in parallel by means of a single parameter.
* *Nested column generation*. A key idea of idol is that every optimizer is seen as a black-box to solve a given optimization model.
  As such, a column-generation algorithm can easily be "plugged in" any algorithmic phase. For instance, solving the pricing of
  a column-generation algorithm can be done by yet another column-generation algorithm.

**Features to come/desired features**

* *Handling identical sub-systems*.
* *Ryan-and-Foster* branching rule.

Table of contents
-----------------

.. toctree::
    :maxdepth: 2

    self
    installation/index
    tutorials/index
    examples/index
    api/index
    developer_guide/index

* :ref:`genindex`
