\mainpage Home

<div id="welcome">
    <span style="width: 58%">
        <span style="display:block;line-height: 3.5em">
            <span style="font-size:2.5em;color:#2f42ff;font-weight:700;">
                Solve optimization problems
            </span><br />
            <span style="font-size:2.5em;font-weight:500;padding-left: 30px;">
                and build custom algorithms
            </span>
        </span>
        <span style="display: block; padding-top: 30px; line-height:1.4; font-size: 1.1em;font-weight: 500">
            Solve <span class="highlight">bilevel</span> and <span class="highlight">two-stage robust optimization</span> problems with the command line interface, or build new custom algorithms
            with the C++ library.
        </span>
        <span style="display:block;padding-top: 30px;padding-bottom: 5px;">
            <a href="examples.html" class="my-button">
                Get started
            </a>
            <a href="examples.html" class="my-button-secondary">
                To the C++ API
            </a>
        </span>
        <span style="font-size:.7em;">Compatible with Gurobi • Cplex • HiGHS • GLPK • CBC • MibS • JuMP</span>
    </span>
    <span style="width: 38%">
        <span style="display: block; padding-right: 20px;margin-top:15%;" class="fragment" id="cli">
<!--
```cpp 
idol_cl solve model.mps \ 
    --bilevel follower.aux \
    --method KKT-SOS1 \
    --time-limit 300
```
-->
        </span>
    </span>
</div>

<span style="font-weight:500;">Browse per problem type</span>

<div id="cards">
    <div class="card" onclick="window.location = 'examples_mixed_integer.html';">
        <a href="examples_mixed_integer.html">
            Mixed-Integer Optimization
        </a>
        <div class="frame">
            <img src="milp.png"/>
        </div>
        Interface with solvers like Gurobi and HiGHS, or write your own branch-and-bound algorithm.
    </div>
    <div class="card" onclick="window.location = 'examples_bilevel.html';">
        <a href="examples_bilevel.html">
            Bilevel Optimization
        </a>
            <div class="frame">
                <img src="bilevel.png"/>
            </div>
            Solve bilevel problems by KKT reformulations or with bilevel solvers like MibS.
    </div>
    <div class="card" onclick="window.location = 'examples_robust.html';">
        <a href="examples_robust.html">
            Robust Optimization
        </a>
        <div class="frame">
            <img src="robust.png" />
        </div>
        Handle uncertain problems using dualization or column-and-constraint generation algorithms.
    </div>
    <div class="card" onclick="window.location = 'examples_mixed_integer.html#examples_mixed_integer_branch_and_price';">
        <a href="examples_mixed_integer.html#examples_mixed_integer_branch_and_price">
            Branch-and-Price
        </a>
        <div class="frame">
            <img src="column-generation.png"/>
        </div>
        Solve large-scale problems efficiently with column generation and branch-and-price techniques.
    </div>
</div>

\section quick_start Quick Start

<div class="tabbed">
<ul>
  <li>
    <b class="tab-title">Linux</b>
```cpp
echo "deb [arch=amd64 trusted=yes] https://henrilefebvre.com/apt stable main" | sudo tee /etc/apt/sources.list.d/idol.list
sudo apt-get update
sudo apt-get install idol
idol_cl --version
```
  </li>
  <li><b class="tab-title">Mac</b>
```cpp
brew tap hlefebvr/idol
brew install idol
idol_cl --version
```
  </li>
  <li><b class="tab-title">From Source</b>
```cpp
git clone https://github.com/hlefebvr/idol.git
mkdir -p idol/build && cd idol/build
cmake .. && cmake --build .
sudo cmake --install .
./idol_cl --version
```
</li>
</ul>
</div>

\section highlights Highlights

- <b>Unified CLI</b> Solve MILP, bilevel, and robust problems in one interface
- <b>Composable C++ framework</b> Build, combine, and nest optimization algorithms
- <b>Solver interoperability</b> Work with Gurobi, Cplex, HiGHS, and more
- <b>Flexible modeling</b> Use your own branch-and-bound, branch-and-price, or decomposition methods


idol is a C++ framework for **mathematical optimization** and complex decision-making problems.
It is designed to help you build new algorithms for solving complex optimization problems.
The main philosophy behind idol is interoperability and ease of use.
Hence, any algorithm can be seamlessly combined with any other algorithm to create a new one.
For instance, you can combine a branch-and-bound algorithm with a column generation algorithm to create a branch-and-price algorithm.

```cpp 
const auto branch_and_price = branch_and_bound + column_generation;
model.use(branch_and_price);
model.optimize();
```

idol offers a variety of tools to tackle many different kinds of optimization problems:



\section getting_started_with_idol Getting Started with idol

New to idol? Be sure to have a look at our [examples](/examples.html). If you want to get a fast hands-on
start, have a look at our [local installation guideline](/automatic-installation.htmt). It is the easiest installation process
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
> const std::vector<double> profit { 40., 50., 100., 95., 30., };
> const std::vector<double> weight { 2., 3.14, 1.98, 5., 3., };
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
> model.set_obj_expr(idol_Sum(j, Range(n_items), -profit[j] * x[j]));
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
* [JuMP](https://jump.dev/) (Julia framework),
* [coin-or/Osi](https://github.com/coin-or/Osi) (gives you acccess to any Osi-compatible solver).

It also integrates with the following solvers for mixed-integer bilevel optimization:

* [coin-or/MibS](https://github.com/coin-or/MibS) (the mixed-integer bilevel solver).

