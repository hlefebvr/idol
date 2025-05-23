\mainpage Home

<div>
    <h1 style="font-size:2.5em;border:none;margin-top:5px;margin-bottom:20px;">A C++ Framework for Optimization</h1>
    <h2 style="font-size:1.5em;color:gray;line-height:1.5em;border:none;margin-bottom:20px;">
        Start solving optimization problems now.
    </h2>
    <p>
        <b><a href="examples.html" class="my-button" id="get-started" >Get started with examples</a></b>
        <b></b><a href="installation.html" class="my-button">To installation guidelines</a></b>
    </p>
</div>

\section what_is_idol What is idol?

idol is a C++ framework for **mathematical optimization** and complex decision-making problems.
It is designed to help you build new algorithms for solving complex optimization problems.
The main philosophy behind idol is interoperability and ease of use.
Hence, any algorithm can be seamlessly combined with any other algorithm to create a new one.
For instance, you can combine a Branch-and-Bound algorithm with a Column Generation algorithm to create a Branch-and-Price algorithm.

```cpp 
const auto branch_and_price = branch_and_bound + column_generation;
model.use(branch_and_price);
model.optimize();
```

idol offers a variety of tools to tackle many different kinds of optimization problems:

<div id="cards">
    <div class="card" onclick="window.location = 'examples_mixed_integer.html';">
        <a href="examples_mixed_integer.html">
            Mixed-integer Optimization
        </a>
        <div class="frame">
            <img src="https://upload.wikimedia.org/wikipedia/commons/0/06/IP_polytope_with_LP_relaxation.svg">
        </div>
        Interface with solvers like Gurobi, Mosek, HiGHS and many others or implement your own branch-and-bound algorithm.
    </div>
    <div class="card" onclick="window.location = 'examples_mixed_integer.html#examples_mixed_integer_branch_and_price';">
        <a href="examples_mixed_integer.html#examples_mixed_integer_branch_and_price">
            Branch-and-Price
        </a>
        <div class="frame">
            <img src="https://media.springernature.com/full/springer-static/image/art%3A10.1007%2Fs00454-012-9421-9/MediaObjects/454_2012_9421_Fig1_HTML.gif">
        </div>
        Exploit your problem structure using branch-and-price and column generation. Solve large-scale problems with ease.
    </div>
    <div class="card" onclick="window.location = 'examples_bilevel.html';">
        <a href="examples_bilevel.html">
            Bilevel Optimization
        </a>
            <div class="frame">
                <img src="https://upload.wikimedia.org/wikipedia/commons/8/8c/Supply-demand-equilibrium.svg">
            </div>
            Tackle optimistic and pessimistic bilevel problems through KKT or strong duality reformulations,
            or use the MibS solver.
    </div>
    <div class="card" onclick="window.location = 'examples_robust.html';">
        <a href="examples_robust.html">
            Robust Optimization
        </a>
        <div class="frame">
            <img src="robust.png" />
        </div>
        Easily solve robust and adjustable robust optimization problems with standard techniques like dualization or
        with column-and-constraint generation.
    </div>
</div>

\section getting_started_with_idol Getting Started with idol

New to idol? Be sure to have a look at our [tutorials](/tutorials). If you want to get a fast hands-on
start, have a look at our [local installation guideline](/installation). It is the easiest installation process
one could think of: it automatically downloads the latest version of idol, and installs it locally in a subfolder of your CMake project.
It's really a matter of seconds before you can start using idol.

> Example
>
> Idol has a user-friendly interface which looks natural to people working in optimization. For instance,
> here is how you solve a knapsack problem instance with [Gurobi](https://www.gurobi.com/).
>
> ```cpp
> using namespace idol;
>
> const unsigned int n_items = 5;
> const double[] profit = { 40., 50., 100., 95., 30., };
> const double[] weight = { 2., 3.14, 1.98, 5., 3., };
> const double capacity = 10.;
>
> Env env;
>
> Model model(env);
>
> const auto x = model.add_vars(Dim<1>(n_items), 0., 1., Binary, 0., "x");
>
> model.add_ctr(idol_Sum(j, Range(n_items), weight[j] * x[j]) <= capacity);
>
> model.set_obj_expr(idol_Sum(j, Range(n_items), -profit[j] * x[j]);
>
> model.use(Gurobi());
>
> model.optimize();
>
> std::cout << "Objective value: " << model.get_best_obj() << std::endl;
> ```

\section is_this_a_mip_solver Is this a MIP Solver?

idol is not a MIP solver in itself. In fact, it typically needs to call external
solvers as a subroutine of more complex algorithms like, e.g., branch-and-cut-and-price.

The idea is to work hand in hand with existing well-engineered optimization
software to enhance their possibilities. Not to replace them!

Another advantage of using idol is that you can easily switch between different solvers.
Write your model once and try different solvers.

The following MIP solvers are currently supported by idol:

* [Cplex](https://www.ibm.com/products/ilog-cplex-optimization-studio),
* [GLPK](https://www.gnu.org/software/glpk/),
* [Gurobi](https://www.gurobi.com/),
* [HiGHS](https://highs.dev/),
* [JuMP](https://jump.dev/) (julia framework),
* [Mosek](https://www.mosek.com/),
* [coin-or/Osi](https://github.com/coin-or/Osi) (gives you acccess to any Osi-compatible solver).

It also integrates with the following solvers for mixed-integer bilevel optimization:

* [coin-or/MibS](https://github.com/coin-or/MibS) (the mixed-integer bilevel solver).

