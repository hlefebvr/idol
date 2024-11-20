//
// Created by henri on 18.09.24.
//
#include <iostream>
#include <idol/modeling.h>
#include <idol/bilevel/modeling/Description.h>
#include <idol/mixed-integer/modeling/models/KKT.h>
#include <idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h>
#include "idol/mixed-integer/optimizers/padm/PADM.h"
#include "idol/mixed-integer/optimizers/padm/SubProblem.h"
#include "idol/mixed-integer/optimizers/padm/PenaltyUpdates.h"

int main(int t_argc, const char** t_argv) {

    using namespace idol;

    Env env;
    Model model(env);
    const auto x = model.add_var(0, Inf, Continuous, 0., "x");
    const auto y = model.add_var(0, Inf, Continuous, 0., "y");
    const auto delta = model.add_var(-Inf, Inf, Continuous, 0., "delta");

    model.set_obj_expr(delta * delta);

    auto c = model.add_qctr(x + delta * x + y - 1, LessOrEqual);
    model.add_ctr(y == 1);

    Bilevel::Description description(env);
    description.make_lower_level(x);
    description.make_lower_level(y);
    description.make_lower_level(c);
    description.set_lower_objective(-2 * x - y);

    Reformulators::KKT reformulator(model, description);

    Model single_level(env);
    reformulator.add_strong_duality_reformulation(single_level);

    Annotation<unsigned int> decomposition(env, "sub_problem");
    for (const auto& var : single_level.vars()) {
        var.set(decomposition, var.id() == delta.id());
    }

    Annotation<bool> penalized_constraints(env, "penalized_constraints");
    for (const auto& ctr : single_level.ctrs()) {
        ctr.set(penalized_constraints, true);
    }

    single_level.use(
            PADM(decomposition, penalized_constraints)
                .with_default_sub_problem_spec(ADM::SubProblem().with_optimizer(Gurobi()))
                .with_penalty_update(PenaltyUpdates::Multiplicative(2))
                .with_rescaling(true, 1e5)
                .with_initial_penalty_parameter(1e2)
                .with_logs(true)
    );

    single_level.optimize();

    std::cout << single_level.get_status() << std::endl;

    std::cout << save_primal(single_level) << std::endl;

    return 0;
}