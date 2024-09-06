//
// Created by henri on 30.08.24.
//
#include <iostream>
#include <Research/idol/lib/include/idol/modeling.h>
#include "idol/optimizers/bilevel-optimization/wrappers/MibS/MibS.h"
#include "idol/modeling/bilevel-optimization/LowerLevelDescription.h"
#include "idol/optimizers/mixed-integer-optimization/wrappers/Gurobi/Gurobi.h"
#include "idol/modeling/bilevel-optimization/read_from_file.h"
#include "KKT.h"

using namespace idol;

int main(int t_argc, const char** t_argv) {

    /*
        This example is taken from Kleinert, T. (2021). “Algorithms for Mixed-Integer Bilevel Problems with Convex Followers.” PhD thesis.
        min x + 6 y
        s.t.

        -x + 5 y <= 12.5,

        y in argmin { -y :
            2 x - y >= 0,
            -x - y >= 0,
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
    auto follower_c2 = high_point_relaxation.add_ctr(-x - y >= 0, "f2");
    auto follower_c3 = high_point_relaxation.add_ctr(-x + 6 * y >= -3, "f3");
    auto follower_c4 = high_point_relaxation.add_ctr(x + 3 * y >= 3, "f4");
    high_point_relaxation.add_ctr(-x + 5 * y <= 12.5);

    // Prepare bilevel description
    Bilevel::LowerLevelDescription description(env);
    description.set_follower_obj_expr(- y);
    description.set_follower_var(y);
    description.set_follower_ctr(follower_c1);
    description.set_follower_ctr(follower_c2);
    description.set_follower_ctr(follower_c3);
    description.set_follower_ctr(follower_c4);

    Reformulators::KKT kkt(high_point_relaxation,
                           [&](const Var& t_var) { return t_var.get(description.follower_vars()) != MasterId; },
                           [&](const Ctr& t_ctr) { return t_ctr.get(description.follower_ctrs()) != MasterId; });

    Model dual(env);
    dual.add(x);
    kkt.add_primal_variables(dual);
    kkt.add_primal_constraints(dual);
    kkt.add_dual_variables(dual);
    kkt.add_dual_constraints(dual);

    std::cout << dual << std::endl;

    return 0;
}
