//
// Created by henri on 06/04/23.
//
#include <iostream>
#include "idol/mixed-integer/problems/knapsack-problem/KP_Instance.h"
#include "idol/modeling.h"
#include "idol/mixed-integer/optimizers/wrappers/HiGHS/HiGHS.h"
#include "idol/mixed-integer/optimizers/wrappers/GLPK/GLPK.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"
#include "idol/mixed-integer/optimizers/callbacks/cutting-planes/UserCutCallback.h"
#include "idol/mixed-integer/optimizers/callbacks/cutting-planes/LazyCutCallback.h"

using namespace idol;

int main(int t_argc, const char** t_argv) {

    const auto instance = Problems::KP::read_instance("knapsack-cover.data.txt");

    const auto n_items = instance.n_items();

    Env env;

    // Create model
    Model model(env);
    auto x = model.add_vars(Dim<1>(n_items), 0, 1, Binary, 0., "x");
    model.add_ctr(idol_Sum(j, Range(n_items), instance.weight(j) * x[j]) <= instance.capacity());
    model.set_obj_expr(idol_Sum(j, Range(n_items), -instance.profit(j) * x[j]));

    // Create cut
    Model cover(env);
    const auto z = cover.add_vars(Dim<1>(n_items), 0, 1, Binary, 0, "z");
    cover.add_ctr(idol_Sum(i, Range(n_items), instance.weight(i) * z[i]) >= instance.capacity() + 1);
    const auto cover_cut = idol_Sum(i, Range(n_items), !z[i] * x[i]) - idol_Sum(i, Range(n_items), 1 - !z[i]);

    // Set optimizer
    auto gurobi = Gurobi();

    // Solve
    model.use(gurobi);
    model.optimize();

    std::cout << "Objective value = " << model.get_best_obj() << std::endl;
    std::cout << "Solution:\n";
    std::cout << save_primal(model) << std::endl;

    // Add callback
    auto cb = UserCutCallback(cover, cover_cut, LessOrEqual);
    cb.with_separation_optimizer(Gurobi());
    gurobi.add_callback(cb);
    gurobi.with_presolve(false);
    gurobi.with_external_param(GRB_IntParam_Cuts, 0);
    gurobi.with_logs(true);

    // Solve
    model.use(gurobi);
    model.optimize();

    std::cout << "Objective value = " << model.get_best_obj() << std::endl;
    std::cout << "Solution:\n";
    std::cout << save_primal(model) << std::endl;

    return 0;
}
