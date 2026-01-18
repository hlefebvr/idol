//
// Created by Henri on 18/01/2026.
//
#include <iostream>
#include "idol/modeling.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"

using namespace idol;

int main() {

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

    model.use(Gurobi());
    model.optimize();

    std::cout << "Objective value: " << model.get_best_obj() << std::endl;

    return 0;
}