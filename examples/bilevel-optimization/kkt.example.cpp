//
// Created by henri on 30.08.24.
//
#include <iostream>
#include <Research/idol/lib/include/idol/modeling.h>
#include "idol/optimizers/bilevel-optimization/wrappers/MibS/MibS.h"
#include "idol/modeling/bilevel-optimization/LowerLevelDescription.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"
#include "idol/modeling/bilevel-optimization/read_from_file.h"
#include "idol/modeling/models/KKT.h"

using namespace idol;

int main(int t_argc, const char** t_argv) {

    /*
        This example is taken from Kleinert, T. (2021). “Algorithms for Mixed-Integer Bilevel Problems with Convex Followers.” PhD thesis.
        min x + 6 y
        s.t.

        -x + 5 y <= 12.5,

        y in argmin { -y :
            2 x - y >= 0,
            -x - y >= -6,
            -x + 6 y >= -3,
            x + 3 y >= 3.
        }

        x >= 0.

     */

    Env env;

    // Define High Point Relaxation
    Model high_point_relaxation(env);

    auto x = high_point_relaxation.add_var(0, Inf, Continuous, "x");
    auto y = high_point_relaxation.add_var(-Inf, Inf, Continuous, "y");

    high_point_relaxation.set_obj_expr(x + 6 * y);
    auto follower_c1 = high_point_relaxation.add_ctr(2 * x - y >= 0, "f1");
    auto follower_c2 = high_point_relaxation.add_ctr(-x - y >= -6, "f2");
    auto follower_c3 = high_point_relaxation.add_ctr(-x + 6 * y >= -3, "f3");
    auto follower_c4 = high_point_relaxation.add_ctr(x + 3 * y >= 3, "f4");
    high_point_relaxation.add_ctr(-x + 5 * y <= 12.5);

    // Prepare bilevel description
    Bilevel::LowerLevelDescription description(env);
    description.set_follower_obj_expr(- y);
    description.make_follower_var(y);
    description.make_follower_ctr(follower_c1);
    description.make_follower_ctr(follower_c2);
    description.make_follower_ctr(follower_c3);
    description.make_follower_ctr(follower_c4);

    Reformulators::KKT reformulator(high_point_relaxation, description);

    Model single_level(env);
    reformulator.add_kkt_reformulation(single_level);

    std::cout << high_point_relaxation << std::endl;
    std::cout << single_level << std::endl;

    single_level.use(Gurobi());

    single_level.optimize();

    std::cout << save_primal(single_level) << std::endl;

    return 0;
}
