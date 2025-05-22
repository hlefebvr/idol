.. title:: idol, a C++ Framework for Optimization

.. raw:: html

    <div style="padding-bottom:25px;margin-bottom:25px;border-bottom:1px solid #eeebee;">

        <h1 style="font-size:2.5em;">A C++ Framework for Optimization</h1>

        <h2 style="font-size:1.5em;color:gray;line-height:1.5em;border-bottom:none;">
            Start solving optimization problems now.
        </h2>

        <span style="font-size:1.2em;">
            <a href="tutorials/index.html" class="my-button" id="get-started" > Get started now </a>
            &nbsp;
            <a href="installation/index.html" class="my-button"> To installation guidelines </a>
        </span>

    </div>

What is idol?
-------------

idol is a C++ framework for **mathematical optimization** and complex decision-making problems.
It is designed to help you build new algorithms for solving complex optimization problems.
The main philosophy behind idol is interoperability and ease of use.
Hence, any algorithm can be seamlessly combined with any other algorithm to create a new one.
For instance, you can combine a Branch-and-Bound algorithm with a Column Generation algorithm to create a Branch-and-Price algorithm.

.. code::

    const auto branch_and_price = branch_and_bound + column_generation;
    model.use(branch_and_price);
    model.optimize();

idol offers a variety of tools to tackle many different kinds of optimization problems:

.. raw:: html

    <div id="cards">
        <a href="tutorials/mixed-integer-programming/index.html">
            <span>Mixed-Integer Optimization</span>
            <div>
                <img src="https://upload.wikimedia.org/wikipedia/commons/0/06/IP_polytope_with_LP_relaxation.svg">
            </div>
            <p>
                Interface with solvers like Gurobi, Mosek, HiGHS and many others or implement your own branch-and-bound algorithm.
            </p>
        </a>
        <a href="tutorials/mixed-integer-programming/dantzig-wolfe/index.html">
            <span>Branch-and-Price</span>
            <div>
                <img src="https://media.springernature.com/full/springer-static/image/art%3A10.1007%2Fs00454-012-9421-9/MediaObjects/454_2012_9421_Fig1_HTML.gif">
            </div>
            <p>
                Exploit your problem structure using branch-and-price and column generation. Solve large-scale problems with ease.
            </p>
        </a>
        <a href="tutorials/bilevel-optimization/index.html">
            <span>Bilevel Optimization</span>
            <div>
                <img src="https://upload.wikimedia.org/wikipedia/commons/8/8c/Supply-demand-equilibrium.svg">
            </div>
            <p>
                Tackle optimistic and pessimistic bilevel problems through KKT or strong duality reformulations,
                or use the MibS solver.
            </p>
        </a>
        <a href="tutorials/robust-optimization/index.html">
            <span>Robust Optimization</span>
            <div>
                <img src="_static/robust.png" />
            </div>
            <p>
                Easily solve robust and adjustable robust optimization problems with standard techniques like dualization or
                with column-and-constraint generation.
            </p>
        </a>
        <a href="tutorials/index.html" style="background-color:#efefef;color:black;">
            <span>And More...</span>
            <div>
                <img src="https://upload.wikimedia.org/wikipedia/commons/7/72/Max_paraboloid.svg" />
            </div>
            <p>
                See all our tutorials...
            </p>
        </a>
    </div>

Getting Started with idol
-------------------------

New to idol? Be sure to have a look at our :ref:`tutorials <tutorials>`. If you want to get a fast hands-on
start, have a look at our :ref:`local installation guideline <installation_local>`. It is the easiest installation process
one could think of: it automatically downloads the latest version of idol, and installs it locally in a sub-folder of your CMake project.
It's really a mater of seconds before you can start using idol.

.. admonition:: Example

    Idol has a user-friendly interface which looks natural to people working in optimization. For instance,
    here is how you solve a knapsack problem instance with `Gurobi <https://www.gurobi.com/>`_.

    .. code:: cpp

        using namespace idol;

        const unsigned int n_items = 5;
        const double[] profit = { 40., 50., 100., 95., 30., };
        const double[] weight = { 2., 3.14, 1.98, 5., 3., };
        const double capacity = 10.;

        Env env;

        Model model(env);

        const auto x = model.add_vars(Dim<1>(n_items), 0., 1., Binary, 0., "x");

        model.add_ctr(idol_Sum(j, Range(n_items), weight[j] * x[j]) <= capacity);

        model.set_obj_expr(idol_Sum(j, Range(n_items), -profit[j] * x[j]);

        model.use(Gurobi());

        model.optimize();

        std::cout << "Objective value: " << model.get_best_obj() << std::endl;

Is this a MIP Solver?
---------------------

idol is not a MIP solver in itself. In fact, it typically needs to call external
solvers as a sub-routine of more complex algorithms like, e.g., branch-and-cut-and-price.

The idea is to work hand in hand with existing well-engineered optimization
software to enhance their possibilities. Not to replace them!

Another advantage of using idol is that you can easily switch between different solvers.
Write your model once, and try different solvers.

The following MIP solvers are currently supported by idol:

* `Cplex <https://www.ibm.com/products/ilog-cplex-optimization-studio>`_,
* `GLPK <https://www.gnu.org/software/glpk/>`_,
* `Gurobi <https://www.gurobi.com/>`_,
* `HiGHS <https://highs.dev/>`_,
* `JuMP <https://jump.dev/>`_ (julia framework),
* `Mosek <https://www.mosek.com/>`_,
* `coin-or/Osi <https://github.com/coin-or/Osi>`_ (gives you acccess to any Osi-compatible solver)

It also integrates with the following solvers for mixed-integer bilevel optimization:

* `coin-or/MibS <https://github.com/coin-or/MibS>`_ (the mixed-integer bilevel solver).

Table of Contents
-----------------

.. toctree::
    :maxdepth: 1

    installation/index
    tutorials/index
    examples/index
    tests/index
    benchmark/index

    api/index
    faq/index
    developer_guide/index
    references
