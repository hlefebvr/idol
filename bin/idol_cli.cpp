//
// Created by Henri on 18/01/2026.
//
#include <iostream>
#include "idol/modeling.h"
#include "idol/mixed-integer/optimizers/wrappers/GLPK/GLPK.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"
#include "idol/mixed-integer/optimizers/wrappers/HiGHS/HiGHS.h"

using namespace idol;

int main(int t_argc, const char ** t_argv) {

    if (t_argc != 2) {
        throw Exception("Expected one argument.");
    }

    std::cout << "Welcome to idol version " << IDOL_VERSION << std::endl;

    const unsigned int n_items = 5;
    const std::vector<double> profit { 40., 50., 100., 95., 30., };
    const std::vector<double> weight { 2., 3.14, 1.98, 5., 3., };
    const double capacity = 10.;

    Env env;
    Model model(env);

    const auto x = model.add_vars(Dim<1>(n_items), 0., 1., Binary, 0., "x");
    model.add_ctr(idol_Sum(j, Range(n_items), weight[j] * x[j]) <= capacity);
    model.set_obj_expr(idol_Sum(j, Range(n_items), -profit[j] * x[j]));

    const std::string solver = t_argv[1];
    if (solver == "--gurobi") {
        model.use(Gurobi());
    } else if (solver == "--glpk") {
        model.use(GLPK());
    } else if (solver == "--highs") {
        model.use(HiGHS());
    } else {
        throw Exception("Unknown solver.");
    }

    model.optimizer().set_param_logs(true);

    model.optimize();

    std::cout << "Objective value: " << model.get_best_obj() << std::endl;

    return 0;
}