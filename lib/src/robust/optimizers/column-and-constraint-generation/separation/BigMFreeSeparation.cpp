//
// Created by Henri on 21/01/2026.
//
#include "idol/robust/optimizers/column-and-constraint-generation/separation/BigMFreeSeparation.h"
#include "idol/bilevel/optimizers/KKT/KKT.h"
#include "idol/bilevel/optimizers/MinMaxDualize/MinMax_Dualize.h"
#include "idol/bilevel/optimizers/StrongDuality/StrongDuality.h"
#include "idol/robust/optimizers/column-and-constraint-generation/Formulation.h"
#include "idol/robust/optimizers/column-and-constraint-generation/Optimizers_ColumnAndConstraintGeneration.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"
#include "idol/mixed-integer/modeling/variables/TempVar.h"

double idol::overestimate(const AffExpr<Var>& t_expr,
                          const std::function<double(const Var&)>& t_get_var_lb,
                          const std::function<double(const Var&)>& t_get_var_ub) {

    double result = t_expr.constant();

    for (const auto& [var, coeff] : t_expr.linear()) {
        if (coeff > 0) {
            const double ub = t_get_var_ub(var);
            if (is_pos_inf(ub)) {
                throw Exception("infinite bound returned for " + var.name());
            }
            result += coeff * ub;
        } else {
            const double lb = t_get_var_lb(var);
            if (is_neg_inf(lb)) {
                throw Exception("infinite bound returned for " + var.name());
            }
            result += coeff * lb;
        }
    }

    return result;
}

double idol::underestimate(const AffExpr<Var>& t_expr,
        const std::function<double(const Var&)>& t_get_var_lb,
        const std::function<double(const Var&)>& t_get_var_ub) {

    double result = t_expr.constant();

    for (const auto& [var, coeff] : t_expr.linear()) {
        if (coeff > 0) {
            result += coeff * t_get_var_lb(var);
        } else {
            result += coeff * t_get_var_ub(var);
        }
    }

    return result;
}

idol::Robust::CCG::BigMFreeSeparation::BigMFreeSeparation(const BigMFreeSeparation& t_src)
    : Separation(t_src),
      m_single_level_optimizer(t_src.m_single_level_optimizer ? t_src.m_single_level_optimizer->clone() : nullptr),
      m_with_zero_one_uncertainty_set(t_src.m_with_zero_one_uncertainty_set) {

}

void idol::Robust::CCG::BigMFreeSeparation::operator()() {

    auto [model, description] = build_separation_problem();
    const auto& ccg = get_parent();
    const auto& formulation = get_formulation();

    BoundProvider bound_provider(*this);

    if (!m_with_zero_one_uncertainty_set.value_or(false)) {
        auto kkt = idol::Bilevel::KKT(description);
        kkt.with_bound_provider(bound_provider);
        kkt.with_single_level_optimizer(*m_single_level_optimizer);
        kkt.with_kleinart_cuts(true);
        kkt.with_time_limit(ccg.get_remaining_time());
        model.use(kkt);
    } else {
        auto dualize = idol::Bilevel::MinMax::Dualize(description);
        dualize.with_bound_provider(bound_provider);
        dualize.with_single_level_optimizer(*m_single_level_optimizer);
        model.use(dualize);
    }
    model.optimize();

    const auto status = model.get_status();
    const auto reason = model.get_reason();
    set_status(status, reason);

    if (status != Feasible && status != Optimal) {
        terminate();
        return;
    }

    if (status == Optimal && m_slack_for_objective) { // Check if we can update the UB

        bool is_feasible = true;
        for (const auto& slack : m_slack_for_constraints) {
            if (const double value = model.get_var_primal(slack) ; !is_zero(value, ccg.get_tol_feasibility())) {
                is_feasible = false;
                break;
            }
        }

        if (is_feasible) {
            const auto& master_solution = get_master_solution();
            const double LB = master_solution.objective_value();
            const double UB = LB - model.get_best_obj();
            submit_upper_bound(UB);
        }
    }

    if (formulation.should_have_epigraph_and_epigraph_is_not_in_master() || model.get_best_obj() < -Tolerance::Feasibility) {

        auto scenario = save_primal(get_robust_description().uncertainty_set(), model);
        add_scenario(std::move(scenario));

        return;
    }

}

idol::Robust::CCG::BigMFreeSeparation& idol::Robust::CCG::BigMFreeSeparation::with_single_level_optimizer(const OptimizerFactory& t_optimizer) {

    if (m_single_level_optimizer) {
        throw Exception("A bilevel optimizer has already been set.");
    }

    m_single_level_optimizer.reset(t_optimizer.clone());

    return *this;
}

idol::Robust::CCG::BigMFreeSeparation& idol::Robust::CCG::BigMFreeSeparation::with_zero_one_uncertainty_set(bool t_value) {

    if (m_with_zero_one_uncertainty_set) {
        throw Exception("Zero-one uncertainty set has already been configured.");
    }

    m_with_zero_one_uncertainty_set = t_value;

    return *this;
}

std::pair<idol::Model, idol::Bilevel::Description>
idol::Robust::CCG::BigMFreeSeparation::build_separation_problem() {

    m_slack_for_objective.reset();
    m_slack_for_constraints.clear();

    auto& original_model = get_parent().parent();
    const auto& formulation = get_formulation();
    const auto& robust_description = get_robust_description();
    const auto& bilevel_description = get_bilevel_description();
    const auto& uncertainty_set = robust_description.uncertainty_set();
    const auto& master_solution = get_master_solution();

    Bilevel::Description result_description(uncertainty_set.env());
    Model result_model = uncertainty_set.copy();
    result_model.set_obj_expr(QuadExpr<>());

    if (formulation.has_second_stage_epigraph()) {
        const double x_0 = master_solution.get(formulation.second_stage_epigraph());
        m_M = std::max(1., std::abs(x_0));
    }

    // Add second-stage variables
    for (const auto& var : formulation.second_stage_variables()) {

        const double lb = original_model.get_var_lb(var);
        const double ub = original_model.get_var_ub(var);
        const auto type = original_model.get_var_type(var);

        assert(type == Continuous);

        result_model.add(var, TempVar(-Inf, Inf, Continuous, 0, LinExpr<Ctr>()));
        result_description.make_lower_level(var);

        if (!is_neg_inf(lb)) {
            const auto slack = result_model.add_var(0, Inf, Continuous, -m_M, "__slack__lb_" + var.name());
            const auto c = result_model.add_ctr(var + slack >= lb, "__lb_" + var.name());
            result_description.make_lower_level(slack);
            result_description.make_lower_level(c);
            m_slack_for_constraints.emplace_back(slack);
        }

        if (!is_pos_inf(ub)) {
            const auto slack = result_model.add_var(0, Inf, Continuous, -m_M, "__slack__ub_" + var.name());
            const auto c = result_model.add_ctr(-var + slack >= -ub, "__ub_" + var.name());
            result_description.make_lower_level(slack);
            result_description.make_lower_level(c);
            m_slack_for_constraints.emplace_back(slack);
        }

    }

    // Add second-stage constraints
    for (const auto& ctr : formulation.second_stage_constraints()) {

        const auto type = original_model.get_ctr_type(ctr);
        const auto& row = original_model.get_ctr_row(ctr);
        double rhs = original_model.get_ctr_rhs(ctr);

        LinExpr<Var> new_row;
        for (const auto& [var, coeff] : row) {
            if (bilevel_description.is_upper(var)) {
                rhs -= coeff * master_solution.get(var);
                continue;
            }
            new_row += coeff * var;
        }

        for (const auto& [var, coeff] : robust_description.uncertain_rhs(ctr)) {
            new_row -= coeff * var;
        }

        for (const auto& [var, coeff] : robust_description.uncertain_mat_coeffs(ctr)) {
            if (bilevel_description.is_lower(var)) {
                throw Exception("Uncertain matrix coefficients cannot be handled.");
            }
            new_row += master_solution.get(var) * coeff;
        }

        if (type == GreaterOrEqual || type == Equal) {
            const auto slack = result_model.add_var(0, Inf, Continuous, -m_M, "__slack_" + ctr.name() + "_ge");
            const auto c = result_model.add_ctr(new_row + slack >= rhs, ctr.name() + "_ge");
            result_description.make_lower_level(c);
            result_description.make_lower_level(slack);
            m_slack_for_constraints.emplace_back(slack);
        }

        if (type == LessOrEqual || type == Equal) {
            const auto slack = result_model.add_var(0, Inf, Continuous, -m_M, "__slack_" + ctr.name() + "_le");
            const auto c = result_model.add_ctr(-new_row + slack >= -rhs, ctr.name() + "_le");
            result_description.make_lower_level(c);
            result_description.make_lower_level(slack);
            m_slack_for_constraints.emplace_back(slack);
        }

    }

    if (formulation.has_second_stage_epigraph()) {
        LinExpr<Var> objective;
        for (const auto& [var, coeff] : original_model.get_obj_expr().affine().linear()) {
            if (bilevel_description.is_lower(var)) {
                objective += coeff * var;
            }
        }
        const double x_0 = master_solution.get(formulation.second_stage_epigraph());
        const auto slack = result_model.add_var(0, Inf, Continuous, -1, "__slack_objective");
        const auto c = result_model.add_ctr(-objective + slack >= -x_0, "__objective");
        result_description.make_lower_level(c);
        result_description.make_lower_level(slack);
        m_slack_for_objective = slack;
    }

    result_description.set_lower_level_obj(-result_model.get_obj_expr());

    compute_kappa(result_model, result_description);

    return {
        std::move(result_model),
        std::move(result_description)
    };
}

void idol::Robust::CCG::BigMFreeSeparation::compute_kappa(const Model& t_separation_model, const Bilevel::Description& t_separation_bilevel_description) {

    const auto& original_model = get_parent().parent();
    const auto& master_solution = get_master_solution();
    const auto& formulation = get_formulation();
    //const auto& bilevel_description = get_bilevel_description();
    const auto& robust_description = get_robust_description();
    const auto& uncertainty_set = robust_description.uncertainty_set();

    const auto get_var_lb = [&](const Var& t_var) { return uncertainty_set.get_var_lb(t_var); };
    const auto get_var_ub = [&](const Var& t_var) { return uncertainty_set.get_var_ub(t_var); };

    m_kappa = 0;

    // Constraints
    for (const auto& ctr : t_separation_model.ctrs()) {

        if (uncertainty_set.has(ctr)) {
            continue;
        }

        const auto& lhs = t_separation_model.get_ctr_row(ctr);
        const auto rhs = t_separation_model.get_ctr_rhs(ctr);

        assert(t_separation_model.get_ctr_type(ctr) == GreaterOrEqual);

        AffExpr<Var> ctr_expr_without_second_stage_variables = rhs;
        for (const auto& [var, coeff] : lhs) {
            if (uncertainty_set.has(var)) {
                ctr_expr_without_second_stage_variables -= coeff * var;
                continue;
            }
            assert(t_separation_bilevel_description.is_lower(var));
        }

        m_kappa += std::max(0., overestimate(ctr_expr_without_second_stage_variables, get_var_lb, get_var_ub));
    }

    // Variable bounds
    for (const auto& var : formulation.second_stage_variables()) {
        const double lb = original_model.get_var_lb(var);
        const double ub = original_model.get_var_ub(var);
        m_kappa += std::max(0., lb);
        m_kappa += std::max(0., -ub);
    }

    m_kappa *= m_M;

    // Objective constraint
    if (formulation.has_second_stage_epigraph()) {
        m_kappa += std::max(0., -master_solution.get(formulation.second_stage_epigraph()));
    }

}

idol::Robust::CCG::BigMFreeSeparation::BoundProvider::BoundProvider(const BigMFreeSeparation& t_parent) : m_parent(t_parent) {

}

double idol::Robust::CCG::BigMFreeSeparation::BoundProvider::get_var_lb(const Var& t_var) {
    const auto& original_model = m_parent.get_parent().parent();
    if (original_model.has(t_var)) {
        return original_model.get_var_lb(t_var) - m_parent.m_kappa / m_parent.m_M;
    }
    return Reformulators::KKT::BoundProvider::get_var_lb(t_var);
}

double idol::Robust::CCG::BigMFreeSeparation::BoundProvider::get_var_ub(const Var& t_var) {
    const auto& original_model = m_parent.get_parent().parent();
    if (original_model.has(t_var)) {
        return original_model.get_var_ub(t_var) + m_parent.m_kappa / m_parent.m_M;
    }
    if (t_var.name() == "__slack___objective") {
        return m_parent.m_kappa;
    }
    if (t_var.name().rfind("__slack_", 0) == 0) {
        return m_parent.m_kappa / m_parent.m_M;
    }
    return Reformulators::KKT::BoundProvider::get_var_ub(t_var);
}

double idol::Robust::CCG::BigMFreeSeparation::BoundProvider::get_ctr_dual_lb(const Ctr& t_ctr) {
    return -m_parent.m_M;
}

double idol::Robust::CCG::BigMFreeSeparation::BoundProvider::get_ctr_dual_ub(const Ctr& t_ctr) {
    return m_parent.m_M;
}

double idol::Robust::CCG::BigMFreeSeparation::BoundProvider::get_ctr_slack_lb(const Ctr& t_ctr) {
    assert(false);
}

double idol::Robust::CCG::BigMFreeSeparation::BoundProvider::get_ctr_slack_ub(const Ctr& t_ctr) {
    const auto& model = this->model();
    const double rhs = model.get_ctr_rhs(t_ctr);
    assert(model.get_ctr_type(t_ctr) == GreaterOrEqual);
    auto row = model.get_ctr_row(t_ctr);
    double result = overestimate(row - rhs,
        [this](const Var& t_x) { return this->get_var_lb(t_x); },
        [this](const Var& t_x) { return this->get_var_ub(t_x); }
        );
    return result;
}

double idol::Robust::CCG::BigMFreeSeparation::BoundProvider::get_var_ub_dual_lb(const Var& t_var) {
    assert(false);
}

double idol::Robust::CCG::BigMFreeSeparation::BoundProvider::get_var_lb_dual_ub(const Var& t_var) {
    assert(t_var.name().rfind("__slack_") == 0);
    if (t_var.name() == "__slack___objective") {
        return 1;
    }
    return m_parent.m_M;
}
