#include <iostream>
#include "modeling.h"
#include "solvers/gurobi/Gurobi.h"
#include "solvers/lpsolve/Lpsolve.h"
#include "algorithms/column-generation/ColGenModel.h"
#include "algorithms/column-generation/ColGenerator.h"
#include "solvers/solutions/Solution.h"

int main() {

    Env env;
    Model model(env);

    auto tau = model.add_variable(0, Inf, Continuous, -1, "tau");
    auto xi = model.add_variable(0., 1., Binary, 0., "xi");
    auto c1 = model.add_constraint(tau + xi <= 2.);
    auto c2 = model.add_constraint(tau + -1 * xi <= 1.);
    model.add_constraint(tau >= 1.1);

    Gurobi gurobi(model);
    gurobi.set_presolve(false);
    gurobi.set_infeasible_or_unbounded_info(true);
    gurobi.solve();
    gurobi.write("model.lp");

    std::cout << gurobi.primal_solution() << std::endl;
    std::cout << gurobi.iis() << std::endl;


    return 0;
}

