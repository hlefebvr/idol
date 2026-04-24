//
// Created by Henri on 21/01/2026.
//
#include "idol/robust/optimizers/column-and-constraint-generation/separation/FeasibilitySeparation.h"
#include "idol/bilevel/optimizers/BilevelOptimizerInterface.h"
#include "idol/bilevel/optimizers/PessimisticAsOptimistic/PessimisticAsOptimistic.h"
#include "idol/robust/optimizers/column-and-constraint-generation/Optimizers_ColumnAndConstraintGeneration.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"

idol::Robust::CCG::FeasibilitySeparation::FeasibilitySeparation(const FeasibilitySeparation& t_src)
    : Separation(t_src),
      m_bilevel_optimizer(t_src.m_bilevel_optimizer ? t_src.m_bilevel_optimizer->clone() : nullptr),
      m_with_integer_slack_variables(t_src.m_with_integer_slack_variables) {

}

idol::Robust::CCG::FeasibilitySeparation& idol::Robust::CCG::FeasibilitySeparation::with_bilevel_optimizer(const OptimizerFactory& t_optimizer) {

    if (m_bilevel_optimizer) {
        throw Exception("A bilevel optimizer has already been set.");
    }

    m_bilevel_optimizer.reset(t_optimizer.clone());

    if (!dynamic_cast<idol::Bilevel::OptimizerInterface*>(m_bilevel_optimizer.get())) {
        std::cout << "WARNING: the bilevel optimizer you have provided does not implement the bilevel interface. "
                     "We are assuming you know what you are doing. If this is not intended, "
                     "you most likely are doing something wrong."
        << std::endl;
    }

    return *this;
}

idol::Robust::CCG::FeasibilitySeparation& idol::Robust::CCG::FeasibilitySeparation::with_integer_slack_variables(bool t_value) {

    if (m_with_integer_slack_variables) {
        throw Exception("Integer slack setting has already been configured.");
    }

    m_with_integer_slack_variables = t_value;

    return *this;
}

void idol::Robust::CCG::FeasibilitySeparation::operator()() {

    auto [model, description] = build_separation_problem();

    // Set optimizer
    m_bilevel_optimizer->as<Bilevel::OptimizerInterface>().set_bilevel_description(description);
    model.use(*m_bilevel_optimizer);

    // Set optimizer parameters
    const auto& parent = get_parent();
    model.optimizer().set_param_time_limit(parent.get_remaining_time());

    // Optimize
    model.optimize();

    const auto status = model.get_status();
    const auto reason = model.get_reason();
    set_status(status, reason);

    if (status != Feasible && status != Optimal) {
        terminate();
        return;
    }

    if (status != Optimal || model.get_best_obj() >= -parent.get_tol_feasibility()) {
        return;
    }

    auto scenario = save_primal(get_robust_description().uncertainty_set(), model);
    add_scenario(std::move(scenario));
}

std::pair<idol::Model, idol::Bilevel::Description> idol::Robust::CCG::FeasibilitySeparation::build_separation_problem() {

    auto [result_model, result_description] = Separation::build_separation_problem();

    result_model.set_obj_expr(0);

    const auto add_slack = [&](const Ctr& t_ctr, double t_coeff) {

        const double rhs = result_model.get_ctr_rhs(t_ctr);
        const auto& row = result_model.get_ctr_row(t_ctr);
        double L = 0.;
        double U = 0.;
        for (const auto& [var, coeff] : row) {
            const double lb = result_model.get_var_lb(var);
            const double ub = result_model.get_var_ub(var);
            if (is_pos_inf(ub) || is_neg_inf(lb)) {
                throw Exception("Variable " + var.name() + " has infinite bounds." );
            }
            if (coeff > 0) {
                U += coeff * ub;
                L += coeff * lb;
            } else {
                U += coeff * lb;
                L += coeff * ub;
            }
        }

        double slack_ub = t_coeff < 0 ? std::max(0., U - rhs) : std::max(0., rhs - L);

        LinExpr<Ctr> column = t_coeff * t_ctr;
        auto name = "__slack_" + t_ctr.name() + (t_coeff < 0 ? "_le" : "_ge");
        const auto type = m_with_integer_slack_variables.value_or(false) ? Integer : Continuous;
        const auto slack = result_model.add_var(0, slack_ub, type, -1, std::move(column), std::move(name));
        result_description.make_lower_level(slack);
    };

    for (const auto& ctr : result_model.ctrs()) {

        if (result_description.is_upper(ctr)) {
            continue;
        }

        const auto type = result_model.get_ctr_type(ctr);

        switch (type) {
        case GreaterOrEqual: add_slack(ctr, 1); break;
        case LessOrEqual: add_slack(ctr, -1); break;
        case Equal: { add_slack(ctr, 1); add_slack(ctr, -1); } break;
        }

    }

    result_description.set_lower_level_obj(-result_model.get_obj_expr());

    return {
        std::move(result_model),
        std::move(result_description)
    };
}
