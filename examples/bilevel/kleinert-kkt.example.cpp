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
#include "idol/mixed-integer/modeling/models/KKT.h"
#include "idol/bilevel/optimizers/KKT/KKT.h"
#include "idol/mixed-integer/optimizers/wrappers/Cplex/Cplex.h"

using namespace idol;

// This is a dummy example which returns 1e3 for all bounds.
class MyBoundProvider : public idol::Reformulators::KKT::BoundProvider {
public:
    double get_ctr_dual_lb(const Ctr &t_ctr) override { return -1e3; }
    double get_ctr_dual_ub(const Ctr &t_ctr) override { return 1e3; }
    double get_ctr_slack_lb(const Ctr &t_ctr) override { return -1e3; }
    double get_ctr_slack_ub(const Ctr &t_ctr) override { return 1e3; }
    double get_var_lb_dual_ub(const Var &t_var) override { return 1e3; }
    double get_var_ub_dual_lb(const Var &t_var) override { return -1e3; }
    [[nodiscard]] MyBoundProvider *clone() const override { return new MyBoundProvider(*this); }
};

int main(int t_argc, const char** t_argv) {

    /*
        This example is taken from Kleinert, T. (2021). “Algorithms for Mixed-Integer Bilevel Problems with Convex Followers.” PhD thesis; see Example 1.

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
    description.set_lower_level_obj(-y);
    description.make_lower_level(y);
    description.make_lower_level(follower_c1);
    description.make_lower_level(follower_c2);
    description.make_lower_level(follower_c3);
    description.make_lower_level(follower_c4);

    MyBoundProvider bound_provider;
    auto single_level = Bilevel::KKT::make_model(high_point_relaxation, description, true);
    // auto single_level = Bilevel::KKT::make_model(high_point_relaxation, description, bound_provider);
    single_level.use(Cplex());
    single_level.optimize();

    /**
     * Alternatively, you could also do
     *  high_point_relaxation.use(Bilevel::KKT().with_single_level_optimizer(Gurobi()));
     *  high_point_relaxation.optimize();
     * Or even,
     *  high_point_relaxation.use(Bilevel::KKT() + Gurobi());
     */

    single_level.write("kkt.lp");

    std::cout << save_primal(single_level) << std::endl;

    return 0;
}
