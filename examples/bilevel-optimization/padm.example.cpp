//
// Created by henri on 18.09.24.
//
#include <iostream>
#include <idol/modeling.h>
#include <idol/modeling/bilevel-optimization/LowerLevelDescription.h>
#include <idol/modeling/models/KKT.h>
#include <idol/optimizers/mixed-integer-optimization/wrappers/Gurobi/Gurobi.h>

int main(int t_argc, const char** t_argv) {


    using namespace idol;

    Env env;
    Model model(env);
    const auto x = model.add_var(0, Inf, Continuous, "x");
    const auto y = model.add_var(0, Inf, Continuous, "y");
    const auto delta = model.add_var(-Inf, Inf, Continuous, "delta");

    model.set_obj_expr(delta * delta);

    auto c = model.add_ctr(x + delta * x + y <= 1);
    model.add_ctr(y == 1);

    Bilevel::LowerLevelDescription description(env);
    description.make_follower_var(x);
    description.make_follower_var(y);
    description.make_follower_ctr(c);
    description.set_follower_obj_expr(-2 * x - y);

    Model single_level(env);
    Reformulators::KKT(model, description).add_strong_duality_reformulation(single_level);

    std::cout << model << std::endl;
    std::cout << single_level << std::endl;

    Annotation<Ctr> sub_problem(env, "sub_problem");

    single_level.use(Gurobi());

    single_level.optimize();

    std::cout << save_primal(single_level) << std::endl;

    return 0;
}