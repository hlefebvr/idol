//
// Created by henri on 11.12.24.
//
#include <idol/robust/optimizers/column-and-constraint-generation/Formulation.h>
#include <idol/mixed-integer/modeling/variables/TempVar.h>
#include <idol/mixed-integer/modeling/expressions/operations/operators.h>


idol::CCG::Formulation::Formulation(const idol::Model &t_parent, const idol::Robust::Description &t_robust_description,
                                    const idol::Bilevel::Description &t_bilevel_description)
        : m_parent(t_parent),
          m_robust_description(t_robust_description),
          m_bilevel_description(t_bilevel_description),
          m_master(t_parent.env()),
          m_bilevel_description_separation(t_parent.env()) {

    parse_variables();
    parse_objective();
    parse_constraints();
    copy_bilevel_description(t_bilevel_description, m_bilevel_description_separation);

    if (is_wait_and_see_follower()) {
        auto& env = m_parent.env();
        m_bilevel_description_master = std::make_optional<::idol::Bilevel::Description>(env);
        copy_bilevel_description(t_bilevel_description, *m_bilevel_description_master);
    }

}

void idol::CCG::Formulation::parse_variables() {

    for (const auto& var : m_parent.vars()) {

        const double obj = m_parent.get_var_obj(var);

        if (m_bilevel_description.is_lower(var)) {
            m_second_stage_variables.emplace_back(var);
            if (!is_zero(obj, Tolerance::Sparsity)) {
                m_has_second_stage_objective = true;
            }
            continue;
        }

        const double lb = m_parent.get_var_lb(var);
        const double ub = m_parent.get_var_ub(var);
        const auto type = m_parent.get_var_type(var);

        m_master.add(var, TempVar(lb, ub, type, obj, LinExpr<Ctr>()));

    }

}

void idol::CCG::Formulation::parse_objective() {

    const auto& objective = m_parent.get_obj_expr();

    if (!objective.has_quadratic()) {
        return; // Because the objective has been added while parsing the variables
    }

    auto master_objective = m_master.get_obj_expr();

    for (const auto& [pair, coeff] : objective) {

        if (m_bilevel_description.is_lower(pair.first)) {
            m_has_second_stage_objective = true;
            continue;
        }

        if (m_bilevel_description.is_lower(pair.second)) {
            m_has_second_stage_objective = true;
            continue;
        }

        master_objective += coeff * pair.first * pair.second;

    }

}

void idol::CCG::Formulation::parse_constraints() {

    for (const auto& ctr : m_parent.ctrs()) {

        const auto& row = m_parent.get_ctr_row(ctr);

        const bool has_first_stage = std::any_of(row.begin(), row.end(), [this](const auto& term) {
            return m_bilevel_description.is_upper(term.first);
        });
        const bool has_second_stage = std::any_of(row.begin(), row.end(), [this](const auto& term) {
            return m_bilevel_description.is_lower(term.first);
        });

        if (has_second_stage || !m_robust_description.uncertain_mat_coeffs(ctr).empty()) {

            if (has_first_stage) {

                // A coupling constraint is an upper-level constraint with a lower-level variable or with uncertainty
                if (m_bilevel_description.is_upper(ctr)) {
                    m_coupling_constraints.emplace_back(ctr);
                } else {
                    m_linking_constraints.emplace_back(ctr);
                }

            }

            m_second_stage_constraints.emplace_back(ctr);
            continue;
        }

        const auto type = m_parent.get_ctr_type(ctr);
        const double rhs = m_parent.get_ctr_rhs(ctr);

        m_master.add(ctr, TempCtr(LinExpr(row), type, rhs));
    }

}

void idol::CCG::Formulation::add_scenario_to_master(const idol::Point<idol::Var> &t_scenario, bool t_add_annotation) {

    const auto& lower_level_annotation = m_bilevel_description_master.has_value() ? m_bilevel_description_master->lower_level() : m_bilevel_description.lower_level();

    std::vector<std::optional<Var>> new_vars;
    new_vars.resize(m_parent.vars().size());

    // Add Variables
    for (const auto& var : m_second_stage_variables) {

        const double lb = m_parent.get_var_lb(var);
        const double ub = m_parent.get_var_ub(var);
        const auto type = m_parent.get_var_type(var);
        const auto index = m_parent.get_var_index(var);

        new_vars[index] = m_master.add_var(lb, ub, type, 0, var.name() + "_" + std::to_string(m_n_added_scenario));
        if (t_add_annotation) {
            new_vars[index]->set(lower_level_annotation, m_n_added_scenario);
        }
    }

    m_master.update();

    // Add Constraints
    for (const auto& ctr : m_second_stage_constraints) {

        const auto& row = m_parent.get_ctr_row(ctr);
        const auto type = m_parent.get_ctr_type(ctr);
        double rhs = m_parent.get_ctr_rhs(ctr);

        LinExpr<Var> new_row;
        for (const auto& [var, coeff] : row) {

            if (m_bilevel_description.is_upper(var)) {
                new_row += coeff * var;
                continue;
            }

            new_row += coeff * new_vars[m_parent.get_var_index(var)].value();
        }

        const auto uncertainties = m_robust_description.uncertain_mat_coeffs(ctr);
        for (const auto& [var, coeff] : uncertainties) {
            const double coeff_value = evaluate(coeff, t_scenario);
            if (m_bilevel_description.is_upper(var)) {
                new_row += coeff_value * var;
            } else {
                new_row += coeff_value * new_vars[m_parent.get_var_index(var)].value();
            }
        }

        rhs += evaluate(m_robust_description.uncertain_rhs(ctr), t_scenario);

        const auto new_ctr = m_master.add_ctr(TempCtr(std::move(new_row), type, rhs), ctr.name() + "_" + std::to_string(m_n_added_scenario));
        if (t_add_annotation) {
            new_ctr.set(lower_level_annotation, m_n_added_scenario);
        }

    }

    if (!m_second_stage_epigraph) {
        m_second_stage_epigraph = m_master.add_var(-Inf, Inf, Continuous, 1, "second_stage_epigraph");
        m_master.update();
    }

    // Add Objective
    LinExpr<Var> objective;
    for (const auto& var : m_second_stage_variables) {
        objective += m_parent.get_var_obj(var) * new_vars[m_parent.get_var_index(var)].value();
    }
    for (const auto& [var, coeff] : m_robust_description.uncertain_obj()) {
        objective += evaluate(coeff, t_scenario) * new_vars[m_parent.get_var_index(var)].value();
    }

    m_master.add_ctr(*m_second_stage_epigraph >= std::move(objective));

    if (is_wait_and_see_follower() && t_add_annotation) {

        auto lower_objective = m_bilevel_description_master->lower_level_obj();

        for (const auto& [var, coeff] : m_bilevel_description.lower_level_obj().affine().linear()) {
            if (m_bilevel_description.is_upper(var)) {
                continue;
            }
            lower_objective += coeff * new_vars[m_parent.get_var_index(var)].value();
        }

        m_bilevel_description_master->set_lower_level_obj(std::move(lower_objective));
        m_master.update();

    }

    ++m_n_added_scenario;
}

void idol::CCG::Formulation::add_separation_problem_constraints(idol::Model &t_model,
                                                                const idol::Point<idol::Var> &t_first_stage_decision) {

    // Add uncertainty variables
    const auto& uncertainty_set = m_robust_description.uncertainty_set();
    for (const auto& var : uncertainty_set.vars()) {

        const double lb = uncertainty_set.get_var_lb(var);
        const double ub = uncertainty_set.get_var_ub(var);
        const auto type = uncertainty_set.get_var_type(var);

        t_model.add(var, TempVar(lb, ub, type, 0, LinExpr<Ctr>()));

    }

    // Add uncertainty constraints
    for (const auto& ctr : uncertainty_set.ctrs()) {

        const auto& row = uncertainty_set.get_ctr_row(ctr);
        const auto type = uncertainty_set.get_ctr_type(ctr);
        double rhs = uncertainty_set.get_ctr_rhs(ctr);

        LinExpr<Var> new_row;
        for (const auto& [var, coeff] : row) {
            new_row += coeff * var;
        }

        t_model.add(ctr, TempCtr(std::move(new_row), type, rhs));

    }

    // Add second-stage variables
    for (const auto& var : m_second_stage_variables) {

        const double lb = m_parent.get_var_lb(var);
        const double ub = m_parent.get_var_ub(var);
        const auto type = m_parent.get_var_type(var);

        t_model.add(var, TempVar(lb, ub, type, 0, LinExpr<Ctr>()));

    }

    // Add remaining constraints
    for (const auto& ctr : m_second_stage_constraints) {

        const auto& row = m_parent.get_ctr_row(ctr);
        const auto type = m_parent.get_ctr_type(ctr);
        double rhs = m_parent.get_ctr_rhs(ctr);

        LinExpr<Var> new_row;
        for (const auto& [var, coeff] : row) {
            if (m_bilevel_description.is_upper(var)) {
                rhs -= coeff * t_first_stage_decision.get(var);
                continue;
            }
            new_row += coeff * var;
        }

        for (const auto& [var, coeff] : m_robust_description.uncertain_rhs(ctr)) {
            new_row -= coeff * var;
        }

        for (const auto& [var, coeff] : m_robust_description.uncertain_mat_coeffs(ctr)) {
            if (m_bilevel_description.is_lower(var)) {
                throw Exception("Uncertain matrix coefficients cannot be handled.");
            }
            new_row += t_first_stage_decision.get(var) * coeff;
        }

        t_model.add(ctr, TempCtr(std::move(new_row), type, rhs));

    }

    if (!m_robust_description.uncertain_obj().empty()) {
        throw Exception("Uncertain objectives not yet implemented");
    }

}

idol::Model idol::CCG::Formulation::build_optimality_separation_problem_for_adjustable_robust_problem(
        const idol::Point<idol::Var> &t_first_stage_decision, unsigned int t_coupling_constraint_index) {

    auto& env = m_master.env();
    Model result(env);

    add_separation_problem_constraints(result, t_first_stage_decision);

    // Compute objective
    QuadExpr objective = compute_second_stage_objective(t_first_stage_decision);
    if (t_coupling_constraint_index > 0) {
        throw Exception("Coupling constraints have no meaning for two-stage robust problems");
    }

    result.set_obj_expr(-1. * objective);

    if (is_adjustable_robust_problem()) {
        m_bilevel_description_separation.set_lower_level_obj(std::move(objective));
    } else {
        // std::cerr << "Assuming pessimistic separator" << std::endl;
        m_bilevel_description_separation.set_lower_level_obj(m_bilevel_description.lower_level_obj());
    }

    return result;
}

void idol::CCG::Formulation::copy_bilevel_description(const ::idol::Bilevel::Description& t_src, const ::idol::Bilevel::Description& t_dest) const {

    const auto& src_annotation = t_src.lower_level();
    const auto& dest_annotation = t_dest.lower_level();

    for (const auto& var : m_parent.vars()) {
        var.set(dest_annotation, var.get(src_annotation));
    }

    for (const auto& ctr : m_parent.ctrs()) {
        ctr.set(dest_annotation, ctr.get(src_annotation));
    }

}

std::pair<idol::Model, std::vector<idol::Var>>
idol::CCG::Formulation::build_feasibility_separation_problem(const idol::Point<idol::Var> &t_first_stage_decision) {

    auto& env = m_master.env();
    Model result(env);
    std::vector<Var> slack_variables;

    add_separation_problem_constraints(result, t_first_stage_decision);

    const auto compute_range = [&](const Ctr& t_ctr, CtrType t_type) {
        double bound = 0;

        if (t_type == LessOrEqual) {
            bound -= result.get_ctr_rhs(t_ctr);
            for (const auto& [var, coeff] : m_robust_description.uncertain_rhs(t_ctr)) {
                if (coeff > 0 ) {
                    bound -= coeff * result.get_var_lb(var);
                } else {
                    bound -= coeff * result.get_var_ub(var);
                }
            }
            for (const auto& [var, coeff] : result.get_ctr_row(t_ctr)) {
                if (coeff > 0) {
                    bound += coeff * result.get_var_ub(var);
                } else {
                    bound += coeff * result.get_var_lb(var);
                }
            }
        } else if (t_type == GreaterOrEqual) {
            bound += result.get_ctr_rhs(t_ctr);
            for (const auto& [var, coeff] : m_robust_description.uncertain_rhs(t_ctr)) {
                if (coeff > 0 ) {
                    bound += coeff * result.get_var_ub(var);
                } else {
                    bound += coeff * result.get_var_lb(var);
                }
            }
            for (const auto& [var, coeff] : result.get_ctr_row(t_ctr)) {
                if (coeff > 0) {
                    bound -= coeff * result.get_var_ub(var);
                } else {
                    bound -= coeff * result.get_var_lb(var);
                }
            }
            bound *= -1;
        } else {
            throw Exception("Equal constraints not yet implemented");
        }

        if (is_inf(bound) || is_inf(bound)) {
            return Inf;
        }

        return std::max(0., bound);
    };

    const auto add_slack = [&](const Ctr& t_ctr, double t_coeff) {
        const double range = compute_range(t_ctr, t_coeff < 0 ? LessOrEqual : GreaterOrEqual);
        LinExpr<Ctr> column = t_coeff * t_ctr;
        auto name = "__slack_" + t_ctr.name() + "_" + (t_coeff < 0 ? "lower" : "upper");
        const auto s = result.add_var(0, range, Continuous, -1, column, std::move(name));
        m_bilevel_description_separation.make_lower_level(s);
        slack_variables.emplace_back(s);
    };

    for (const auto& ctr : m_second_stage_constraints) {

        const auto& type = m_parent.get_ctr_type(ctr);

        switch (type) {
            case LessOrEqual:
                add_slack(ctr, -1);
                break;
            case GreaterOrEqual:
                add_slack(ctr, 1);
                break;
            case Equal:
                add_slack(ctr, 1);
                add_slack(ctr, -1);
                break;
        }

    }

    m_bilevel_description_separation.set_lower_level_obj(-1. * result.get_obj_expr());

    return {
        std::move(result),
        std::move(slack_variables)
    };
}

bool idol::CCG::Formulation::is_adjustable_robust_problem() const {
    return m_bilevel_description.lower_level_obj().is_zero(Tolerance::Feasibility);
}

std::pair<idol::Model, std::vector<idol::Var>>
idol::CCG::Formulation::build_joint_separation_problem(const idol::Point<idol::Var> &t_first_stage_decision) {

    auto [model, slack_variables] = build_feasibility_separation_problem(t_first_stage_decision);

    if (!m_second_stage_epigraph) {
        return {
            std::move(model),
            std::move(slack_variables)
        };
    }

    const auto s = model.add_var(0, Inf, Continuous, -1, LinExpr<Ctr>(), "__slack_objective");
    m_bilevel_description_separation.make_lower_level(s);
    m_bilevel_description_separation.set_lower_level_obj(-1. * model.get_obj_expr());

    auto objective = compute_second_stage_objective(t_first_stage_decision);
    if (objective.has_quadratic()) {
        throw Exception("Quadratic objectives in second stage are not yet implemented");
    }

    // Try to bound s
    double ub = objective.affine().constant();
    for (const auto& [var, coeff] : objective.affine().linear()) {
        if (coeff < 0) {
            ub += coeff * model.get_var_ub(var);
        } else {
            ub += coeff * model.get_var_lb(var);
        }
    }
    if (!is_pos_inf(ub)) {
        model.set_var_ub(s, std::max(0., ub));
    }

    const auto c = model.add_ctr(std::move(objective.affine().linear()) <= m_master.get_var_primal(*m_second_stage_epigraph) + s, "__epigraph_constraint");
    m_bilevel_description_separation.make_lower_level(c);

    slack_variables.emplace_back(s);

    return {
            std::move(model),
            std::move(slack_variables)
    };
}

idol::QuadExpr<idol::Var>
idol::CCG::Formulation::compute_second_stage_objective(const idol::Point<idol::Var> &t_first_stage_decision) const {

    QuadExpr<Var> result;

    const auto& src_objective = m_parent.get_obj_expr();

    result += src_objective.affine().constant();
    for (const auto& [var, coeff] : src_objective.affine().linear()) {
        if (m_bilevel_description.is_upper(var)) {
            result += coeff * t_first_stage_decision.get(var);
            continue;
        }
        result += coeff * var;
    }

    if (src_objective.has_quadratic()) {
        throw Exception("Quadratic objectives not yet implemented");
    }

    return result;
}

bool idol::CCG::Formulation::should_have_epigraph_and_epigraph_is_not_in_master() const {
    return m_has_second_stage_objective && (!m_second_stage_epigraph || !m_master.has(*m_second_stage_epigraph));
}
