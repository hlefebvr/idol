//
// Created by henri on 07.02.24.
//
#include <iostream>
#include <Research/idol/lib/include/idol/modeling.h>
#include <OsiClpSolverInterface.hpp>
#include <OsiCpxSolverInterface.hpp>
#include "idol/optimizers/bilevel-optimization/wrappers/MibS/MibS.h"
#include "idol/modeling/bilevel-optimization/LowerLevelDescription.h"
#include "idol/optimizers/mixed-integer-optimization/wrappers/Gurobi/Gurobi.h"
#include "idol/optimizers/mixed-integer-optimization/callbacks/Callback.h"

using namespace idol;

class MyCallback : public CallbackFactory {
public:
    class Strategy : public Callback {
    protected:
        void operator()(CallbackEvent t_event) override {
            std::cout << t_event << '\t' << best_bound() << '\t' << best_obj() << std::endl;
            std::cout << primal_solution() << std::endl;

            const auto& model = original_model();

            const auto x = model.get_var_by_index(0);
            const auto y = model.get_var_by_index(1);

            Solution::Primal solution;
            solution.set_objective_value(-22);
            solution.set(x, 2);
            solution.set(y, 2);

            submit_heuristic_solution(solution);

        }
    };

    Callback *operator()() override {
        return new Strategy();
    }

    [[nodiscard]] CallbackFactory *clone() const override {
        return new MyCallback(*this);
    }
};

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
    description.make_follower_var(y);
    description.make_follower_ctr(follower_c1);
    description.make_follower_ctr(follower_c2);
    description.make_follower_ctr(follower_c3);
    description.make_follower_ctr(follower_c4);

    // Use coin-or/MibS as external solver
    high_point_relaxation.use(
                Bilevel::MibS(description)
                    .add_callback(MyCallback())
                    .with_logs(true)
    );

    // Optimize and print solution
    high_point_relaxation.optimize();

    std::cout << save_primal(high_point_relaxation) << std::endl;

    return 0;
}
