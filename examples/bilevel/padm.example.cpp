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
#include "idol/bilevel/optimizers/KKT/KKT.h"
#include "idol/bilevel/optimizers/StrongDuality/StrongDuality.h"

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

    Annotation<unsigned int> decomposition(env, "sub_problem", 0);
    delta.set(decomposition, 1);

    /*
     * All constraints will be penalized.
     * The initial penalty value is set to 1e2.
     * If you do not want to penalize a constraint, you can set the initial penalty value to < 0.
     */
    Annotation<double> initial_penalties(env, "initial_penalties", 1e2);

    const auto strong_duality = Bilevel::StrongDuality(description);

    const auto padm = PADM(decomposition, initial_penalties)
            .with_default_sub_problem_spec(
                    ADM::SubProblem().with_optimizer(Gurobi())
            )
            .with_penalty_update(PenaltyUpdates::Multiplicative(2))
            .with_rescaling_threshold(1e4)
            .with_logs(true);

    model.use(strong_duality + padm);

    model.optimize();

    std::cout << save_primal(model) << std::endl;

    /**
     * Alternatively, you can use:
     * const Model strong_duality = Bilevel::StrongDuality::make_model(description);
     * strong_duality.use(padm);
     * strong_duality.optimize();
     */

    return 0;
}