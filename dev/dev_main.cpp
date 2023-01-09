#include <iostream>
#include <utility>
#include "modeling.h"
#include "algorithms.h"
#include "reformulations/Reformulations_Benders.h"
#include "algorithms/benders/Benders.h"

int main(int t_argc, char** t_argv) {

    Model model;

    auto flag = model.add_user_attr<unsigned int>(0, "subproblem");

    auto x = model.add_vars(Dim<1>(2), 0., Inf, Continuous, 0., "x");
    auto y = model.add_var(0., Inf, Integer, 1., "y");
    model.add_ctr(x[0] + 2 * x[1] + y >= 3.);
    model.add_ctr(2 * x[0] - x[1] + 3 * y >= 4);

    model.set<unsigned int>(flag, x[0], 1);
    model.set<unsigned int>(flag, x[1], 1);

    model.set(Attr::Obj::Expr, 2 * x[0] + 3 * x[1] + 2 * y);

    Benders solver(model, flag);

    solver.set_master_solution_strategy<Solvers::Gurobi>();
    solver.subproblem(1).set_exact_solution_strategy<Solvers::Gurobi>();

    solver.solve();

    std::cout << solver.primal_solution() << std::endl;

    solver.master_solution_strategy().write("model");


    return 0;
}
