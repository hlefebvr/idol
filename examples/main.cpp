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
    std::cout << "Ray\n" << solver.unbounded_ray().normalize(Inf) << std::endl;
}

int main() {

    Env env;
    Model model(env);

    auto x = model.add_variable(0., Inf, Continuous, -3, "x");
    auto y = model.add_variable(0., Inf, Continuous, -2, "y");
    auto c1 = model.add_constraint(x + -2 * y <= 1);
    auto c2 = model.add_constraint(-2 * x + y <= 1);
    auto c3 = model.add_constraint(x + y >= 2);

    solve_with<Gurobi>(model);
    solve_with<Lpsolve>(model);

    return 0;
}

