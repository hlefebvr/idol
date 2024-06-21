//
// Created by henri on 07.02.24.
//
#include <iostream>
#include <Research/idol/lib/include/idol/modeling.h>
#include "idol/optimizers/bilevel-optimization/wrappers/MibS/MibS.h"
#include "idol/modeling/bilevel-optimization/LowerLevelDescription.h"
#include "idol/optimizers/mixed-integer-optimization/wrappers/Gurobi/Gurobi.h"
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
    Model high_point_relaxation(env);

    auto x = high_point_relaxation.add_var(0, Inf, Integer, "x");
    auto y = high_point_relaxation.add_var(0, Inf, Integer, "y");

    high_point_relaxation.set_obj_expr(-x - 10 * y);
    auto follower_c1 = high_point_relaxation.add_ctr(-25 * x + 20 * y <= 30);
    auto follower_c2 = high_point_relaxation.add_ctr(x + 2 * y <= 10);
    auto follower_c3 = high_point_relaxation.add_ctr(2 * x - y <= 15);
    auto follower_c4 = high_point_relaxation.add_ctr(2 * x + 10 * y >= 15);

    // Prepare bilevel description
    Bilevel::LowerLevelDescription description(env);
    description.set_follower_obj_expr(y);
    description.set_follower_var(y);
    description.set_follower_ctr(follower_c1);
    description.set_follower_ctr(follower_c2);
    description.set_follower_ctr(follower_c3);
    description.set_follower_ctr(follower_c4);

    // Use coin-or/MibS as external solver
    high_point_relaxation.use(Bilevel::MibS(description));

    // Optimize and print solution
    high_point_relaxation.optimize();

    std::cout << high_point_relaxation.get_status() << std::endl;
    std::cout << high_point_relaxation.get_reason() << std::endl;
    std::cout << save_primal(high_point_relaxation) << std::endl;

    return 0;
}
