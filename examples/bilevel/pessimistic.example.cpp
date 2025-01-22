//
// Created by henri on 17.12.24.
//
#include <iostream>
#include "idol/modeling.h"
#include "idol/bilevel/optimizers/wrappers/MibS/MibS.h"
#include "idol/bilevel/modeling/Description.h"
#include "idol/bilevel/optimizers/PessimisticAsOptimistic/PessimisticAsOptimistic.h"
#include "idol/bilevel/optimizers/KKT/KKT.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"

using namespace idol;

int main(int t_argc, const char** t_argv) {

    /*
     This example is taken from "A Practical Scheme to Compute Pessimistic Bilevel Optimization Problem" (Bo Zeng, 2025).
     See https://optimization-online.org/wp-content/uploads/2015/09/5090.pdf

        min -8 x_1 - 6x_2 - 25 y_1 - 30 y_2 + 2 y_3 + 16 y_4
        s.t.

        x_1 + x_2 <= 10,
        x_1, x_2 >= 0,

        y in argmin { -10 y_1 - 10 y_2 - 10 y_3 - 10 y_4 :
            y_1 + y_2 + y_3 + y_4 <= 10 - x_1 - x_2,
            -y_1 + y_4 <= 0.8 x_1 + 0.8 x_2,
            y_2 + y_4 <= 4 x_2,
            y_1, y_2, y_3, y_4 >= 0
        }

     */

    Env env;

    // Define High Point Relaxation
    Model high_point_relaxation(env);

    auto x = high_point_relaxation.add_vars(Dim<1>(3), 0, Inf, Continuous, 0, "x");
    auto y = high_point_relaxation.add_vars(Dim<1>(4), 0, Inf, Continuous, 0, "y");

    auto leader_c = high_point_relaxation.add_ctr(x[0] + x[1] <= 10, "leader_c");
    auto follower_c1 = high_point_relaxation.add_ctr(y[0] + y[1] + y[2] + y[3] <= 10 - x[0] - x[1], "follower_c1");
    auto follower_c2 = high_point_relaxation.add_ctr(-y[0] + y[3] <= 0.8 * x[0] + 0.8 * x[1], "follower_c2");
    auto follower_c3 = high_point_relaxation.add_ctr(y[1] + y[3] <= 4 * x[1], "follower_c3");

    high_point_relaxation.set_obj_expr(-8 * x[0] - 6 * x[1] - 25 * y[0] - 30 * y[1] + 2 * y[2] + 16 * y[3]);

    // Prepare bilevel description
    Bilevel::Description description(env);
    description.set_lower_level_obj(-10 * y[0] - 10 * y[1] - 10 * y[2] - 10 * y[3]);
    description.make_lower_level<1>(y);
    description.make_lower_level(follower_c1);
    description.make_lower_level(follower_c2);
    description.make_lower_level(follower_c3);

    auto [opt_model, opt_description] = Bilevel::PessimisticAsOptimistic::make_model(high_point_relaxation, description);

    std::cout << opt_model << std::endl;

    // Use coin-or/MibS as external solver
    opt_model.use(
            Bilevel::KKT(opt_description) + Gurobi()
    );

    // Optimize and print solution
    opt_model.optimize();

    std::cout << save_primal(opt_model) << std::endl;

    return 0;
}
