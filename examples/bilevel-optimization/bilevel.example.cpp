//
// Created by henri on 07.02.24.
//
#include <iostream>
#include <Research/idol/lib/include/idol/modeling.h>
#include "idol/optimizers/mixed-integer-optimization/wrappers/MibS/MibS.h"

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

    // Annotate follower variables
    // * If not annotated, the default value is MasterId, i.e., leader variables and constraints
    // * Otherwise, it indicates the id of the follower (here, we have only one follower)
    Annotation<Var> follower_variables(env, "follower_variable", MasterId);
    Annotation<Ctr> follower_constraints(env, "follower_constraints", MasterId);

    y.set(follower_variables, 0);
    follower_c1.set(follower_constraints, 0);
    follower_c2.set(follower_constraints, 0);

    // Use coin-or/MibS as external solver
    model.use(Bilevel::MibS(follower_variables,
                            follower_constraints,
                            follower_objective));

    // Optimize and print solution
    model.optimize();

    std::cout << save_primal(model) << std::endl;

    return 0;
}
