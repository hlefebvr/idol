//
// Created by henri on 17.12.24.
//
#include <iostream>
#include "idol/modeling.h"
#include "idol/bilevel/optimizers/wrappers/MibS/MibS.h"
#include "idol/bilevel/modeling/Description.h"
#include "idol/bilevel/optimizers/PessimisticAsOptimistic/PessimisticAsOptimistic.h"

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

    auto x = high_point_relaxation.add_var(0, Inf, Integer, -1, "x");
    auto y = high_point_relaxation.add_var(0, Inf, Integer, -10, "y");

    auto follower_c1 = high_point_relaxation.add_ctr(-25 * x + 20 * y <= 30);
    auto follower_c2 = high_point_relaxation.add_ctr(x + 2 * y <= 10);
    auto follower_c3 = high_point_relaxation.add_ctr(2 * x - y <= 15);
    auto follower_c4 = high_point_relaxation.add_ctr(2 * x + 10 * y >= 15);

    // Prepare bilevel description
    Bilevel::Description description(env);
    description.set_lower_level_obj(y);
    description.make_lower_level(y);
    description.make_lower_level(follower_c1);
    description.make_lower_level(follower_c2);
    description.make_lower_level(follower_c3);
    description.make_lower_level(follower_c4);

    auto [opt_model, opt_description] = Bilevel::PessimisticAsOptimistic::make_model(high_point_relaxation, description);

    std::cout << opt_model << std::endl;

    // Use coin-or/MibS as external solver
    opt_model.use(
            Bilevel::MibS(description)
                    .with_logs(true)
    );

    // Optimize and print solution
    opt_model.optimize();

    std::cout << save_primal(opt_model) << std::endl;

    return 0;
}
