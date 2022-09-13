#include <iostream>
#include "modeling.h"
#include "solvers/gurobi/Gurobi.h"
#include "solvers/lpsolve/Lpsolve.h"
#include "algorithms/column-generation/ColGenModel.h"
#include "algorithms/column-generation/ColGenerator.h"
#include "solvers/solutions/Solution.h"

template<class SolverT>
void solve_with(Model& t_model) {
    SolverT solver(t_model);
    solver.set_infeasible_or_unbounded_info(true);
    solver.solve();
    std::cout << "Primal\n" << solver.primal_solution() << std::endl;
    std::cout << "Dual\n" << solver.dual_solution() << std::endl;
    std::cout << "Farkas\n" << solver.dual_farkas() << std::endl;
}

int main() {

    Env env;
    Model model(env);

    auto u = model.add_variable(0., Inf, Continuous, 1, "u");
    auto v = model.add_variable(0., Inf, Continuous, 1, "v");
    auto w = model.add_variable(0., Inf, Continuous, -2, "w");
    auto c1 = model.add_constraint(u + -2 * v + -1 * w >= 3);
    auto c2 = model.add_constraint(-2 * u + v + -1 * w >= 2);

    solve_with<Gurobi>(model);
    solve_with<Lpsolve>(model);

    return 0;
}

