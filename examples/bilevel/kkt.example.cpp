//
// Created by henri on 30.08.24.
//
#include <iostream>
#include "idol/modeling.h"
#include "idol/bilevel/modeling/Description.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"
#include "idol/bilevel/modeling/read_from_file.h"
#include "idol/mixed-integer/modeling/models/KKT.h"
#include "idol/mixed-integer/optimizers/wrappers/GLPK/GLPK.h"
#include "idol/mixed-integer/modeling/models/Dualizer.h"

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

    auto x = high_point_relaxation.add_var(0, Inf, Continuous, 0., "x");
    auto y = high_point_relaxation.add_var(-Inf, Inf, Continuous, 0., "y");

    high_point_relaxation.set_obj_expr(x + 6 * y);
    auto follower_c1 = high_point_relaxation.add_ctr(2 * x - y >= 0, "f1");
    auto follower_c2 = high_point_relaxation.add_ctr(-x - y >= -6, "f2");
    auto follower_c3 = high_point_relaxation.add_ctr(-x + 6 * y >= -3, "f3");
    auto follower_c4 = high_point_relaxation.add_ctr(x + 3 * y >= 3, "f4");
    high_point_relaxation.add_ctr(-x + 5 * y <= 12.5);

    // Prepare bilevel description
    Bilevel::Description description(env);
    description.set_lower_objective(-y);
    description.make_lower_level(y);
    description.make_follower(follower_c1);
    description.make_follower(follower_c2);
    description.make_follower(follower_c3);
    description.make_follower(follower_c4);

    Reformulators::KKT reformulator(high_point_relaxation, description);

    Dualizer dualizer(high_point_relaxation,
                      description.follower_obj(),
                      [&](const Var& t_var) { return description.is_follower(t_var); },
                      [&](const Ctr& t_ctr) { return description.is_follower(t_ctr); },
                      [&](const QCtr& t_qctr) { return description.is_follower(t_qctr); }
                      );

    Model new_single_level(env);
    dualizer.add_kkt_reformulation(new_single_level);

    Model single_level(env);
    reformulator.add_kkt_reformulation(single_level);

    std::cout << high_point_relaxation << std::endl;
    std::cout << single_level << std::endl;
    std::cout << new_single_level << std::endl;

    single_level.use(Gurobi());
    new_single_level.use(Gurobi());

    single_level.optimize();
    new_single_level.optimize();

    single_level.write("kkt.lp");
    new_single_level.write("new_kkt.lp");

    std::cout << save_primal(single_level) << std::endl;
    std::cout << save_primal(new_single_level) << std::endl;

    return 0;
}
