Welcome to idol!
================

.. image:: https://img.shields.io/github/license/hlefebvr/idol
.. image:: https://img.shields.io/github/v/tag/hlefebvr/idol?label=version
.. image:: https://img.shields.io/github/actions/workflow/status/hlefebvr/idol/tests.yml?branch=main
.. image:: https://img.shields.io/github/issues-raw/hlefebvr/idol
.. image:: https://www.repostatus.org/badges/latest/wip.svg
.. image:: https://codecov.io/github/hlefebvr/idol/branch/main/graph/badge.svg?token=BWMH5522QP)

.. image:: https://raw.githubusercontent.com/hlefebvr/idol/main/docs/branch-and-price-implementation.png

.. toctree::
    :maxdepth: 3
    :caption: Contents:

What Is Idol?
-------------

Idol is a powerful and flexible library meticulously crafted for developing new mathematical optimization algorithms.
It is built to provide researchers with a versatile toolkit to construct, tweak, and experiment with state-of-the-art
methods. Whether you're exploring Branch-and-Price,
Benders decomposition, Column-and-Constraint generation for adjustable robust problems,
or any other cutting-edge method, idol is your trusted companion.

Why Choose Idol?
----------------

* **Versatility:** idol is all about freedom and flexibility. You can easily tailor, refine, and combine standard optimization algorithms to create novel solutions that perfectly fit your problem.
* **State-of-the-art:** idol offers a solid foundation of well-established mathematical optimization methods, with the ability to break free from conventions, enabling you to customize and adapt these methods to your specific needs.
* **Modularity:** idol encourages modularity. Whether you need to fine-tune a parameter or replace a component entirely, idol empowers you to do so seamlessly.
* **Collaboration:** Mathematical optimization is a collaborative field. With idol, you can easily share your custom algorithms with other researchers, fostering a spirit of innovation and exploration.
* **Open-source:** As an open-source library, idol is not only a tool for your research but also a community-driven project where you can contribute, share your enhancements, and collaborate with others.

Getting Started With Idol
-------------------------

If you are new to idol, I advise you to visit our :ref:`installation guideline <installation>`.
In particular, have a look at the :ref:`local installation guideline <installation_local>`.
It is the easiest installation process one could think of since it automatically downloads the latest version of idol and
installs it in a local folder. It's really a mater of seconds before you can start using idol.

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

        auto x = model.add_vars(Dim<1>(n_items), 0., 1., Binary, "x");

        model.add_ctr(idol_Sum(j, Range(n_items), weight[j] * x[j]) <= capacity);

        model.set_obj_expr(idol_Sum(j, Range(n_items), -profit[j] * x[j]);

        model.use(Gurobi());

        model.optimize();

If you want to learn more about idol's modeling interface, be sure to visit :ref:`our introductory tutorials <tutorials_basics>`.

If you are looking for a Branch-and-Price tutorial, directly got to :ref:`our Dantzig-Wolfe Decomposition tutorials <tutorials_dantzig_wolfe>`.

Is This a MIP Solver?
---------------------

The idol library is not a MIP solver in itself. Indeed, it typically needs to call external
solvers (e.g., `GLPK <https://www.gnu.org/software/glpk/>`_ or `Gurobi <https://www.gurobi.com/>`_)
as a sub-routine of more complex algorithmic schemes (e.g., Column Generation).
The idea is to work hand in hand with existing fine-tuned and well-engineered optimization
software to enhance their possibilities to solve, e.g., larger problems or problems which
cannot directly be modeled as reasonable MIPs (e.g., :math:`\Sigma_i^P`-hard problems).

Typically, each subproblem is then solved by an external and dedicated solver.
Currently, the following external solvers can be interfaced with through idol:

* `Gurobi <https://www.gurobi.com/>`_
* `Mosek <https://www.mosek.com/>`_
* `GLPK <https://www.gnu.org/software/glpk/>`_
* `HiGHS <https://highs.dev/>`_

Current Features
----------------

.. warning::

    Handling quadratic expressions is an experimental feature and extreme care should be taken if used.

Interfacing External Optimization Solvers
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The idol library can be used to interface external solvers like `GLPK <https://www.gnu.org/software/glpk/>`_
or `Gurobi <https://www.gurobi.com/>`_ to solve standalone LPs, QPs, MILPs, MIQPs, and MIQCQPs.

It offers a unified and common
interface to every solver including standard parameters and branch-and-bound callbacks.
Thus, you can write your code once and test it with different solvers!

It also provides classical callbacks one would need to implement when solving hard problems like generating user cuts
or lazy constraints. Simply give idol the separation model and let it handle the rest.

Branch-and-Bound (B&B) Algorithms
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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

**Features to come/desired features**

* *Parallelization of the algorithm*.
* *New branching rules*: reliability branching.
* *Default cutting planes*.

Column Generation (CG) and Dantzig-Wolfe Decomposition
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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

**Features to come/desired features**

* *Handling identical sub-systems*.

Benchmark
---------

* A benchmark for the **Branch-and-Price** implementation is available for the `Generalized Assignment Problem <https://hlefebvr.github.io/idol-benchmark-gap/GAP.render.html>`_.
* A benchmark for the **Branch-and-Bound** implementation is available for the `Knapsack Problem <https://hlefebvr.github.io/idol-benchmark-kp/KP.render.html>`_

.. image:: https://raw.githubusercontent.com/hlefebvr/idol-benchmark-gap/gh-pages/profile.png

This is a performance profile computed according to :cite:`Dolan2002`.

Table of contents
-----------------

.. toctree::
    :maxdepth: 2

    self
    installation/index
    tutorials/index
    api/index
    examples/index
    developer_guide/index
    references
