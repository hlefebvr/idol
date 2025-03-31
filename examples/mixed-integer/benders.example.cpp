//
// Created by henri on 31.03.25.
//
#include <iostream>
#include "idol/modeling.h"
#include "idol/mixed-integer/optimizers/wrappers/GLPK/GLPK.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"
#include "idol/mixed-integer/optimizers/callbacks/cutting-planes/LazyCutCallback.h"

using namespace idol;

int main(int t_argc, const char** t_argv) {

    Env env;

    // Create model
    Model model(env);
    const auto y = model.add_var(0, Inf, Integer, 2, "y");
    const auto z = model.add_var(0, Inf, Continuous, 1, "z");

    // Create separation problem
    Model separation(env);
    const auto lambda = separation.add_vars(Dim<1>(2), 0, Inf, Continuous, 0, "lambda");
    separation.add_ctr(lambda[0] + 2 * lambda[1] <= 2);
    separation.add_ctr(2 * lambda[0] - lambda[1] <= 3);
    const auto benders_cut = z - (!lambda[0] * 3 - !lambda[0] * y + !lambda[1] * 4 - !lambda[1] * 3 * y);

    auto cb = LazyCutCallback(separation, benders_cut, GreaterOrEqual);
    cb.with_separation_optimizer(Gurobi());

    auto gurobi = Gurobi();
    gurobi.add_callback(cb);
    gurobi.with_lazy_cut(true);
    gurobi.with_logs(true);

    model.use(gurobi);
    model.optimize();

    std::cout << "Objective value = " << model.get_best_obj() << std::endl;
    std::cout << "Solution:\n";
    std::cout << save_primal(model) << std::endl;

    return 0;
}
