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
          m_separation_bilevel_description(t_parent.env()) {

    parse_variables();
    parse_objective();
    parse_constraints();
    copy_bilevel_description();

}

void idol::CCG::Formulation::parse_variables() {

    for (const auto& var : m_parent.vars()) {

        if (m_bilevel_description.is_lower(var) > 0) {
            m_second_stage_variables.emplace_back(var);
            continue;
        }

        const double lb = m_parent.get_var_lb(var);
        const double ub = m_parent.get_var_ub(var);
        const auto type = m_parent.get_var_type(var);
        const double obj = m_parent.get_var_obj(var);

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
            continue;
        }

        if (m_bilevel_description.is_lower(pair.second)) {
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
                m_linking_constraints.emplace_back(ctr);
            }
            m_second_stage_constraints.emplace_back(ctr);
            continue;
        }

        const auto type = m_parent.get_ctr_type(ctr);
        const double rhs = m_parent.get_ctr_rhs(ctr);

        m_master.add(ctr, TempCtr(LinExpr(row), type, rhs));
    }

}

void idol::CCG::Formulation::add_scenario_to_master(const idol::Point<idol::Var> &t_scenario) {

    std::cout << "Adding scenario\n" << t_scenario << std::endl;

    const auto& lower_level_annotation = m_bilevel_description.lower_level();

    std::vector<std::optional<Var>> new_vars;
    new_vars.resize(m_parent.vars().size());

    // Add Variables
    for (const auto& var : m_second_stage_variables) {

        const double lb = m_parent.get_var_lb(var);
        const double ub = m_parent.get_var_ub(var);
        const auto type = m_parent.get_var_type(var);
        const auto index = m_parent.get_var_index(var);

        new_vars[index] = m_master.add_var(lb, ub, type, 0, var.name() + "_" + std::to_string(m_n_added_scenario));
        new_vars[index]->set(lower_level_annotation, m_n_added_scenario);
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
            new_row += evaluate(coeff, t_scenario) * new_vars[m_parent.get_var_index(var)].value();
        }

        rhs += evaluate(m_robust_description.uncertain_rhs(ctr), t_scenario);

        const auto new_ctr = m_master.add_ctr(TempCtr(std::move(new_row), type, rhs), ctr.name() + "_" + std::to_string(m_n_added_scenario));
        new_ctr.set(lower_level_annotation, m_n_added_scenario);

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

    if (!m_bilevel_description.lower_level_obj().is_zero(Tolerance::Feasibility)) {
        throw Exception("Newly created lower-levels do not have an objective function. This has to be "
                        "implemented. Though they have the right annotation.");
    }

    ++m_n_added_scenario;
}

void idol::CCG::Formulation::add_separation_problem_constraints(idol::Model &t_model,
                                                                const idol::Point<idol::Var> &t_first_stage_decision) {

    // Add uncertainty variables
    for (const auto& var : m_robust_description.uncertainty_set().vars()) {

        const double lb = m_robust_description.uncertainty_set().get_var_lb(var);
        const double ub = m_robust_description.uncertainty_set().get_var_ub(var);
        const auto type = m_robust_description.uncertainty_set().get_var_type(var);

        t_model.add(var, TempVar(lb, ub, type, 0, LinExpr<Ctr>()));

    }

    // Add uncertainty constraints
    for (const auto& ctr : m_robust_description.uncertainty_set().ctrs()) {

        const auto& row = m_robust_description.uncertainty_set().get_ctr_row(ctr);
        const auto type = m_robust_description.uncertainty_set().get_ctr_type(ctr);
        double rhs = m_robust_description.uncertainty_set().get_ctr_rhs(ctr);

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

        t_model.add(ctr, TempCtr(std::move(new_row), type, rhs));

    }

    if (!m_robust_description.uncertain_obj().empty()) {
        throw Exception("Uncertain objectives not yet implemented");
    }

    if (m_robust_description.uncertain_mat_coeffs().size() > 0) {
        throw Exception("Uncertain matrix coefficients cannot be handled.");
    }
}

idol::Model idol::CCG::Formulation::build_optimality_separation_problem_for_adjustable_robust_problem(
        const idol::Point<idol::Var> &t_first_stage_decision, unsigned int t_coupling_constraint_index) {

    auto& env = m_master.env();
    Model result(env);

    add_separation_problem_constraints(result, t_first_stage_decision);

    // Compute objective
    QuadExpr objective;
    if (t_coupling_constraint_index == 0) {

        const auto& src_objective = m_parent.get_obj_expr();

        objective += src_objective.affine().constant();
        for (const auto& [var, coeff] : src_objective.affine().linear()) {
            if (m_bilevel_description.is_upper(var)) {
                objective += coeff * t_first_stage_decision.get(var);
                continue;
            }
            objective += coeff * var;
        }

        if (src_objective.has_quadratic()) {
            throw Exception("Quadratic objectives not yet implemented");
        }

    } else {
        throw Exception("Coupling constraints not yet implemented");
    }

    result.set_obj_expr(-1. * objective);
    m_separation_bilevel_description.set_lower_level_obj(std::move(objective));

    return result;
}

void idol::CCG::Formulation::copy_bilevel_description() {

    const auto& src_annotation = m_bilevel_description.lower_level();
    const auto& dest_annotation = m_separation_bilevel_description.lower_level();

    for (const auto& var : m_parent.vars()) {
        var.set(dest_annotation, var.get(src_annotation));
    }

    for (const auto& ctr : m_parent.ctrs()) {
        ctr.set(dest_annotation, ctr.get(src_annotation));
    }

}

idol::Model
idol::CCG::Formulation::build_feasibility_separation_problem(const idol::Point<idol::Var> &t_first_stage_decision) {

    auto& env = m_master.env();
    Model result(env);

    add_separation_problem_constraints(result, t_first_stage_decision);

    std::cout << result << std::endl;

    const auto compute_range = [&](const Ctr& t_ctr) {
        double bound = 0;
        const auto type = result.get_ctr_type(t_ctr);

        if (type == LessOrEqual) {
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
            std::cout << "Bound of " << t_ctr.name() << " is " << bound << std::endl;
        } else if (type == GreaterOrEqual) {
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
            std::cout << "Bound of " << t_ctr.name() << " is " << bound << std::endl;
        } else {
            throw Exception("Equal constraints not yet implemented");
        }

        if (is_inf(bound) || is_inf(bound)) {
            return Inf;
        }

        return std::max(0., bound);
    };

    const auto add_slack = [&](const Ctr& t_ctr, double t_coeff) {
        const double range = compute_range(t_ctr);
        LinExpr<Ctr> column = t_coeff * t_ctr;
        const auto s = result.add_var(0, range, Continuous, -1, column, "slack_" + t_ctr.name());
        m_separation_bilevel_description.make_lower_level(s);
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

    m_separation_bilevel_description.set_lower_level_obj(-1. * result.get_obj_expr());

    return std::move(result);
}
