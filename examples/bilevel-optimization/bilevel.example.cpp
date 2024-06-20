//
// Created by henri on 07.02.24.
//
#include <iostream>
#include <Research/idol/lib/include/idol/modeling.h>
#include "idol/optimizers/bilevel-optimization/wrappers/MibS/MibS.h"
#include "idol/modeling/bilevel-optimization/Description.h"
#include "idol/optimizers/mixed-integer-optimization/wrappers/Gurobi/Gurobi.h"
#include "idol/modeling/bilevel-optimization/write_to_file.h"
#include "idol/modeling/bilevel-optimization/read_from_file.h"

using namespace idol;

int main(int t_argc, const char** t_argv) {

    /**
     * min  -x − 7y
     * s.t. −3x + 2y ≤ 12
             x + 2y ≤ 20
             x ≤ 10
             y ∈ arg min {z : 2x - z ≤ 7,
                              -2x + 4z ≤ 16,
                              z ≤ 5}
     */

    Env env;

    // Define High Point Relaxation
    Model model(env);

    auto x = model.add_var(0, 10, Integer, "x");
    auto y = model.add_var(0, 5, Integer, "y");

    model.set_obj_expr(-x - 7 * y);
    model.add_ctr(-3 * x + 2 * y <= 12);
    model.add_ctr(x + 2 * y <= 20);
    auto follower_objective = model.add_ctr(y == 0);
    auto follower_c1 = model.add_ctr(2 * x - y <= 7);
    auto follower_c2 = model.add_ctr(-2 * x + 4 * y <= 16);

    // Prepare bilevel description
    Bilevel::Description description(env);
    description.make_follower_objective(follower_objective);
    description.make_follower_var(y);
    description.make_follower_ctr(follower_c1);
    description.make_follower_ctr(follower_c2);

    // Use coin-or/MibS as external solver
    //model.use(Bilevel::MibS(description));

    Bilevel::write_to_file(model, description, "instance");

    Bilevel::read_from_file<Gurobi>(env, "instance.aux");

    // Optimize and print solution
    //model.optimize();

    //std::cout << save_primal(model) << std::endl;

    return 0;
}
