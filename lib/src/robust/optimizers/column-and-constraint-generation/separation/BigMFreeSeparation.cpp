//
// Created by Henri on 21/01/2026.
//
#include "idol/robust/optimizers/column-and-constraint-generation/separation/BigMFreeSeparation.h"
#include "idol/bilevel/optimizers/KKT/KKT.h"
#include "idol/bilevel/optimizers/StrongDuality/StrongDuality.h"
#include "idol/robust/optimizers/column-and-constraint-generation/Formulation.h"
#include "idol/robust/optimizers/column-and-constraint-generation/Optimizers_ColumnAndConstraintGeneration.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"

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
      m_single_level_optimizer(t_src.m_single_level_optimizer ? t_src.m_single_level_optimizer->clone() : nullptr) {

}

void idol::Robust::CCG::BigMFreeSeparation::operator()() {

    auto [model, description] = build_separation_problem();
    const auto& ccg = get_parent();
    const auto& formulation = get_formulation();

    BoundProvider bound_provider(*this);
    auto kkt = idol::Bilevel::KKT(description);
    kkt.with_bound_provider(bound_provider);
    kkt.with_single_level_optimizer(*m_single_level_optimizer);
    //kkt.with_sos1_constraints(true);
    //kkt.with_kleinart_cuts(true);
    kkt.with_time_limit(ccg.get_remaining_time());

    model.use(kkt);

    model.optimize();
    model.write("model.lp"); std::cout << "Writing to file ..." << std::endl;

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
            if (const double value = model.get_var_primal(slack) ; !is_zero(value, Tolerance::Feasibility)) {
                is_feasible = false;
                break;
            }
        }

        if (is_feasible) {
            const double LB = get_parent().get_best_bound();
            const double UB = LB - model.get_best_obj();
            submit_upper_bound(UB);
        }
    }

    std::cout << save_primal(model) << std::endl;

    if (formulation.should_have_epigraph_and_epigraph_is_not_in_master() || model.get_best_obj() < -Tolerance::MIPAbsoluteGap) {

        auto scenario = save_primal(get_robust_description().uncertainty_set(), model);

        assert(scenario.size() <= 3); // todo; REMOVE THIS!!

        add_scenario(std::move(scenario));

        return;
    }

    std::cout << "WARNING: Separation did not produce any new scenario" << std::endl;

}

idol::Robust::CCG::BigMFreeSeparation& idol::Robust::CCG::BigMFreeSeparation::with_single_level_optimizer(const OptimizerFactory& t_optimizer) {

    if (m_single_level_optimizer) {
        throw Exception("A bilevel optimizer has already been set.");
    }

    m_single_level_optimizer.reset(t_optimizer.clone());

    return *this;
}

std::pair<idol::Model, idol::Bilevel::Description>
idol::Robust::CCG::BigMFreeSeparation::build_separation_problem() {

    m_slack_for_objective.reset();
    m_slack_for_constraints.clear();

    auto [model, description] = Separation::build_separation_problem();
    const auto& formulation = get_formulation();
    const auto& ccg = get_parent();
    const auto& original_model = ccg.parent();

    if (formulation.is_wait_and_see_follower()) {
        throw Exception("Big-M free separation for robust bilevel problems with wait-and-see follower is not available.");
    }

    // Get and save objective
    auto second_stage_objective = -model.get_obj_expr().affine();
    model.set_obj_expr(0);


    const auto add_slack = [&](const Ctr& t_ctr, double t_obj, bool t_is_slack_for_objective = false) {
        std::optional<Var> s;
        const auto type = model.get_ctr_type(t_ctr);

        switch (type) {
        case LessOrEqual:
            s = model.add_var(0., Inf, Continuous, t_obj, -t_ctr, "__slack_" + t_ctr.name());
            break;
        case GreaterOrEqual:
            s = model.add_var(0., Inf, Continuous, t_obj, t_ctr, "__slack_" + t_ctr.name());
            break;
        case Equal: {
            s = model.add_var(0, Inf, Continuous, t_obj, t_ctr,"__slack_" + t_ctr.name() + "_l");
            const auto s2 = model.add_var(0, Inf, Continuous, t_obj, -t_ctr,"__slack_" + t_ctr.name() + "_u");
            description.make_lower_level(s2);
            m_slack_for_constraints.emplace_back(s2);
            break;
        }
        default: throw Exception("Unexpected constraint type.");
        }

        description.make_lower_level(*s);
        if (t_is_slack_for_objective) {
            assert(!m_slack_for_objective);
            m_slack_for_objective = s;
        } else {
            m_slack_for_constraints.push_back(*s);
        }
    };

    // Add slacks for objective constraint
    if (formulation.has_second_stage_epigraph()) {
        const auto x_0 = get_master_solution().get(formulation.second_stage_epigraph());
        const auto c = model.add_ctr(second_stage_objective.linear() <= x_0, "__objective");
        add_slack(c, -1, true);
        description.make_lower_level(c);
        m_M = std::max(1., std::abs(x_0)) + 1; // TODO
    }

    // Add slacks for original constraints
    for (const auto& ctr : formulation.second_stage_constraints()) {
        add_slack(ctr, -m_M);
    }

    // Add slacks for the variable bounds
    for (const auto& var : formulation.second_stage_variables()) {
        const double lb = model.get_var_lb(var);
        const double ub = model.get_var_ub(var);
        if (!is_neg_inf(lb)) {
            model.set_var_lb(var, -Inf);
            const auto c = model.add_ctr(var >= lb, "__var_lb_" + var.name());
            add_slack(c, -m_M);
            description.make_lower_level(c);
        }
        if (!is_pos_inf(ub)) {
            model.set_var_ub(var, Inf);
            const auto c = model.add_ctr(var <= ub, "__var_ub_" + var.name());
            add_slack(c, -m_M);
            description.make_lower_level(c);
        }
    }

    // Set lower level objective function
    description.set_lower_level_obj(-model.get_obj_expr());

    compute_kappa(model, description);

    return {
        std::move(model),
        std::move(description)
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
    for (const auto& ctr : formulation.second_stage_constraints()) {

        const auto& lhs = t_separation_model.get_ctr_row(ctr);
        const auto rhs = t_separation_model.get_ctr_rhs(ctr);
        const auto type = original_model.get_ctr_type(ctr);

        AffExpr<Var> ctr_expr_without_second_stage_variables = rhs;
        for (const auto& [var, coeff] : lhs) {
            if (uncertainty_set.has(var)) {
                ctr_expr_without_second_stage_variables -= coeff * var;
                continue;
            }
            assert(t_separation_bilevel_description.is_lower(var));
        }

        double worst_case = 0.;
        switch (type) {
        case LessOrEqual: {
            worst_case += std::max(0., -underestimate(ctr_expr_without_second_stage_variables, get_var_lb, get_var_ub));
            break;
        }
        case GreaterOrEqual: {
            worst_case += std::max(0., overestimate(ctr_expr_without_second_stage_variables, get_var_lb, get_var_ub));
            break;
        }
        case Equal: {
                worst_case += std::abs(std::max(
                    underestimate(ctr_expr_without_second_stage_variables, get_var_lb, get_var_ub),
                    overestimate(ctr_expr_without_second_stage_variables, get_var_lb, get_var_ub)
                ));
            break;
        }
        default: throw Exception("Unknown constraint type.");
        }

        m_kappa += m_M * worst_case;
    }

    // Variable bounds
    for (const auto& var : formulation.second_stage_variables()) {
        const double lb = original_model.get_var_lb(var);
        const double ub = original_model.get_var_ub(var);
        m_kappa += m_M * std::max(0., lb);
        m_kappa += m_M * std::max(0., -ub);
    }

    // Objective constraint
    if (formulation.has_second_stage_epigraph()) {
        m_kappa += std::max(0., -master_solution.get(formulation.second_stage_epigraph()));
    }

    std::cout << "Kappa = " << m_kappa << std::endl;

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
    if (t_var.name().starts_with("__slack_")) {
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
    const auto& model = this->model();
    const double rhs = model.get_ctr_rhs(t_ctr);
    assert(model.get_ctr_type(t_ctr) == LessOrEqual);
    auto row = model.get_ctr_row(t_ctr);
    double result = overestimate(row - rhs,
        [this](const Var& t_x) { return get_var_lb(t_x); },
        [this](const Var& t_x) { return get_var_ub(t_x); }
        );
    return -result;
}

double idol::Robust::CCG::BigMFreeSeparation::BoundProvider::get_ctr_slack_ub(const Ctr& t_ctr) {
    const auto& model = this->model();
    const double rhs = model.get_ctr_rhs(t_ctr);
    assert(model.get_ctr_type(t_ctr) == GreaterOrEqual);
    auto row = model.get_ctr_row(t_ctr);
    double result = overestimate(row - rhs,
        [this](const Var& t_x) { return get_var_lb(t_x); },
        [this](const Var& t_x) { return get_var_ub(t_x); }
        );
    return result;
}

double idol::Robust::CCG::BigMFreeSeparation::BoundProvider::get_var_ub_dual_lb(const Var& t_var) {
    assert(false);
}

double idol::Robust::CCG::BigMFreeSeparation::BoundProvider::get_var_lb_dual_ub(const Var& t_var) {
    assert(t_var.name().starts_with("__slack_"));
    if (t_var.name() == "__slack___objective") {
        return 1;
    }
    return m_parent.m_M;
}
