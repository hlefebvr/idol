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
#include "idol/mixed-integer/modeling/models/KKT.h"
#include "idol/mixed-integer/optimizers/wrappers/GLPK/GLPK.h"
#include "idol/mixed-integer/optimizers/wrappers/HiGHS/HiGHS.h"

int main(int t_argc, const char** t_argv) {

    using namespace idol;

    Env env;
    Model model(env);
    const auto x = model.add_var(0, Inf, Continuous, 0., "x");
    const auto y = model.add_var(0, Inf, Continuous, 0., "y");
    const auto delta = model.add_var(-Inf, Inf, Continuous, 0., "delta");

    model.set_obj_expr(delta * delta);

    auto c = model.add_qctr((1 + delta) * x + y - 1, LessOrEqual);
    auto coupling = model.add_ctr(y == 1);

    Bilevel::Description description(env);
    description.make_lower_level(x);
    description.make_lower_level(y);
    description.make_lower_level(c);
    description.set_lower_level_obj(-2 * x - y);

    Reformulators::KKT reformulator(model, description);

    Model single_level(env);
    reformulator.add_coupling_variables(single_level);
    reformulator.add_strong_duality_reformulation(single_level);
    reformulator.add_coupling_constraints(single_level);
    single_level.set_obj_expr(single_level.get_obj_expr());

    std::cout << model << std::endl;
    std::cout << single_level << std::endl;

    single_level.use(Gurobi());
    single_level.optimize();

    std::cout << save_primal(single_level) << std::endl;

    Annotation<unsigned int> decomposition(env, "sub_problem");
    for (const auto& var : single_level.vars()) {
        var.set(decomposition, var.id() == delta.id());
    }

    /*
     * All constraints will be penalized.
     * The initial penalty value is set to 1e2.
     * If you do not want to penalize a constraint, you can set the initial penalty value to < 0.
     */
    Annotation<double> initial_penalties(env, "initial_penalties", 1e2);

    Point<Var> initial_point;

    single_level.use(
            PADM(decomposition, initial_penalties)
                    .with_default_sub_problem_spec(
                            ADM::SubProblem()
                                            .with_optimizer(Gurobi())
                                            .with_initial_point(initial_point)
                    )
                    .with_penalty_update(PenaltyUpdates::Multiplicative(2))
                    .with_rescaling_threshold(1e4)
                    .with_logs(true)
    );

    single_level.optimize();

    std::cout << save_primal(single_level) << std::endl;

    return 0;
}