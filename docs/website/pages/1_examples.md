\page examples Examples
\brief Describes a collection of examples to get started with idol.

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


Examples are organized according to the type of optimization problem they solve. The following sections
describe the examples in more detail. Each example can be found in the idol repository, in the `examples/` folder.

- \subpage examples_mixed_integer
- \subpage examples_bilevel
- \subpage examples_robust

