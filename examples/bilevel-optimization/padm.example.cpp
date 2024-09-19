//
// Created by henri on 18.09.24.
//
#include <iostream>
#include <idol/modeling.h>
#include <idol/modeling/bilevel-optimization/LowerLevelDescription.h>
#include <idol/modeling/models/KKT.h>
#include <idol/optimizers/mixed-integer-optimization/wrappers/Gurobi/Gurobi.h>
#include "idol/optimizers/mixed-integer-optimization/padm/PADM.h"
#include "idol/optimizers/mixed-integer-optimization/padm/SubProblem.h"
#include "idol/optimizers/mixed-integer-optimization/padm/PenaltyUpdates.h"

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

    Reformulators::KKT reformulator(model, description);

    Model single_level(env);
    reformulator.add_strong_duality_reformulation(single_level);

    Annotation<Var, unsigned int> decomposition(env, "sub_problem");
    for (const auto& var : single_level.vars()) {
        var.set(decomposition, var.id() == delta.id());
    }

    Annotation<Ctr, bool> penalized_constraints(env, "penalized_constraints");
    for (const auto& ctr : single_level.ctrs()) {
        ctr.set(penalized_constraints, true);
    }

    single_level.use(
            PADM(decomposition, penalized_constraints)
                .with_default_sub_problem_spec(ADM::SubProblem().with_optimizer(Gurobi()))
                .with_penalty_update(PenaltyUpdates::Multiplicative(2))
                .with_rescaling(true, 1e3)
                .with_logs(true)
    );

    single_level.optimize();

    std::cout << single_level.get_status() << std::endl;

    std::cout << save_primal(single_level) << std::endl;

    return 0;
}