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

    /*
     This example is taken from "The Mixed Integer Linear Bilevel Programming Problem" (Moore and Bard, 1990).

        min -1 x + -10 y
        s.t.

        y in argmin { y :
            -25 x + 20 y <= 30,
            1 x + 2 y <= 10,
            2 x + -1 y <= 15,
            2 x + 10 y >= 15,
            y >= 0 and integer.
        }
        x >= 0 and integer.

     */

    Env env;

    // Define High Point Relaxation
    Model model(env);

    auto x = model.add_var(0, Inf, Integer, "x");
    auto y = model.add_var(0, Inf, Integer, "y");

    model.set_obj_expr(-x - 10 * y);
    auto follower_objective = model.add_ctr(y == 0);
    auto follower_c1 = model.add_ctr(-25 * x + 20 * y <= 30);
    auto follower_c2 = model.add_ctr(x + 2 * y <= 10);
    auto follower_c3 = model.add_ctr(2 * x - y <= 15);
    auto follower_c4 = model.add_ctr(2 * x + 10 * y >= 15);

    // Prepare bilevel description
    Bilevel::Description description(env);
    description.make_follower_objective(follower_objective);
    description.make_follower_var(y);
    description.make_follower_ctr(follower_c1);
    description.make_follower_ctr(follower_c2);
    description.make_follower_ctr(follower_c3);
    description.make_follower_ctr(follower_c4);

    std::cout << model << std::endl;

    // Use coin-or/MibS as external solver
    model.use(Bilevel::MibS(description).with_logs(true));

    // Optimize and print solution
    model.optimize();

    std::cout << model.get_status() << std::endl;
    std::cout << model.get_reason() << std::endl;
    std::cout << save_primal(model) << std::endl;

    return 0;
}
