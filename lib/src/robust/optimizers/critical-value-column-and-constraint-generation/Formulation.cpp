//
// Created by Henri on 17/04/2026.
//
#include "idol/robust/optimizers/critical-value-column-and-constraint-generation/Formulation.h"

#include <unistd.h>

#include "idol/robust/optimizers/critical-value-column-and-constraint-generation/Optimizers_CriticalValueColumnAndConstraintGeneration.h"
#include "idol/mixed-integer/modeling/variables/TempVar.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"

idol::CVCCG::Formulation::Formulation(const idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration& t_parent) :
    m_parent(t_parent),
    m_master(t_parent.parent().env()),
    m_sub_problem(t_parent.parent().env())
{

    check_assumptions();
    initialize_master();
    initialize_sub_problem();

}

void idol::CVCCG::Formulation::check_assumptions() {

    const auto& model = m_parent.parent();
    const auto& description = m_parent.description();
    const auto& uncertainty_set = description.uncertainty_set();

    // Check that all linking variables are binary
    // and checks if has linking variabes
    for (const auto& var : uncertainty_set.vars()) {

        const bool is_linking = model.has(var);

        if (!is_linking) {
            continue;
        }

        m_linking_variables.emplace_back(var);

        const auto type = model.get_var_type(var);
        const double lb = model.get_var_lb(var);
        const double ub = model.get_var_ub(var);

        if (type == Continuous) {
            throw Exception("All linking variables must be integer.");
        }

        if (type == Integer && (lb < -.5 || ub > 1.5)) {
            m_all_linking_variables_are_binary = false;
        }

    }

    // Check that all data in uncertainty set is integer
    for (const auto& ctr : uncertainty_set.ctrs()) {

        const auto& row = uncertainty_set.get_ctr_row(ctr);
        bool is_linking_constraint = false;
        for (const auto& [var, coeff] : row) {
            if (model.has(var)) {
                is_linking_constraint = true;
            }
            if (!is_integer(coeff, m_parent.get_tol_integer())) {
                m_all_data_in_linking_constraints_is_integer = false;
            }
        }
        if (is_linking_constraint) {
            m_linking_constraints.emplace_back(ctr);
        }

    }

}

void idol::CVCCG::Formulation::initialize_master() {

    const auto& model = m_parent.parent();
    const auto& description = m_parent.description();

    // Copy variables to master
    for (const auto& var : model.vars()) {
        const double lb = model.get_var_lb(var);
        const double ub = model.get_var_ub(var);
        const auto type = model.get_var_type(var);
        m_master.add(var, TempVar(lb, ub, type, 0., LinExpr<Ctr>()));
    }

    // Copy constraints to master or create subproblem
    for (const auto& ctr : model.ctrs()) {

        const bool is_uncertain = !description.uncertain_mat_coeffs(ctr).empty() || !description.uncertain_rhs(ctr).empty();

        if (!is_uncertain) {
            auto row = model.get_ctr_row(ctr);
            const double rhs = model.get_ctr_rhs(ctr);
            const auto type = model.get_ctr_type(ctr);
            m_master.add(ctr, TempCtr(std::move(row), type, rhs));
        } else {
            m_uncertainties.emplace_back(ctr);
        }
    }

    if (!description.uncertain_obj().empty()) {
        m_uncertainties.emplace_back();
        std::cerr << "Objective uncertainty is an experimental feature" << std::endl;
    } else {
        m_master.set_obj_expr(model.get_obj_expr());
    }

    // Create cover constraints
    if (m_use_cover_constraints) {
        for (auto& linking : m_linking_constraints) {
            linking.cover_constraint = m_master.add_ctr(LinExpr(), LessOrEqual, 1, "__cover_" + linking.ctr_in_uncertainty_set.name());
        }
    }

    // Set optimizer for master
    m_master.use(m_parent.get_master_optimizer_factory());

}

void idol::CVCCG::Formulation::initialize_sub_problem() {

    const auto& model = m_parent.parent();
    const auto& description = m_parent.description();
    const auto& uncertainty_set = description.uncertainty_set();

    // Copy variables to sub-problem
    for (const auto& var : uncertainty_set.vars()) {
        const bool is_linking = model.has(var);
        if (is_linking) {
            continue;
        }
        const double lb = uncertainty_set.get_var_lb(var);
        const double ub = uncertainty_set.get_var_ub(var);
        const auto type = uncertainty_set.get_var_type(var);
        m_sub_problem.add(var, TempVar(lb, ub, type, 0., LinExpr<Ctr>()));
    }

    // Copy constraints to sub-problem
    for (const auto& ctr : uncertainty_set.ctrs()) {
        const auto& row = uncertainty_set.get_ctr_row(ctr);
        const auto type = uncertainty_set.get_ctr_type(ctr);
        LinExpr<Var> fixed_row;
        for (const auto& [var, coeff] : row) {
            if (model.has(var)) {
                continue;
            }
            fixed_row += coeff * var;
        }
        m_sub_problem.add(ctr, TempCtr(std::move(fixed_row), type, 0.));
    }

    // Set optimizer for sub-problem
    m_sub_problem.use(m_parent.get_deterministic_optimizer_factory());

}

void idol::CVCCG::Formulation::update_sub_problem_constraints(const PrimalPoint& t_master_solution) {

    if (m_linking_variables.empty()) {
        return;
    }

    const auto& model = m_parent.parent();
    const auto& description = m_parent.description();
    const auto& uncertainty_set = description.uncertainty_set();

    for (const auto& ctr : uncertainty_set.ctrs()) {

        const auto& row = uncertainty_set.get_ctr_row(ctr);

        double fixed_rhs = uncertainty_set.get_ctr_rhs(ctr);
        for (const auto& [var, coeff] : row) {
            if (!model.has(var)) {
                continue;
            }
            fixed_rhs -= coeff * t_master_solution.get(var);
        }

        m_sub_problem.set_ctr_rhs(ctr, fixed_rhs);

    }

}

void idol::CVCCG::Formulation::
update_sub_problem_objective(const PrimalPoint& t_master_solution, const Uncertainty& t_uncertainty) {

    if (!t_uncertainty.is_constraint()) {
        throw Exception("Not implemented.");
    }

    const auto& model = m_parent.parent();
    const auto& description = m_parent.description();
    const auto& ctr = t_uncertainty.ctr();
    const auto type = model.get_ctr_type(ctr);
    const auto& row = model.get_ctr_row(ctr);

    double fixed_rhs = model.get_ctr_rhs(ctr);
    LinExpr fixed_row;

    for (const auto& [var, coeff] : row) {
        if (model.has(var)) {
            fixed_rhs -= coeff * t_master_solution.get(var);
        } else {
            fixed_row += coeff * var;
        }
    }
    for (const auto& [var, unc_coeff] : description.uncertain_mat_coeffs(ctr)) {
        fixed_row += t_master_solution.get(var) * unc_coeff;
    }
    std::cout << description.uncertain_rhs(ctr) << std::endl;
    for (const auto& [unc_param, coeff] : description.uncertain_rhs(ctr)) {
        fixed_row += -coeff * unc_param;
    }

    std::cout << fixed_row << std::endl;

    if (type == LessOrEqual) {
        m_sub_problem.set_obj_expr(fixed_rhs - fixed_row);
    } else if (type == GreaterOrEqual) {
        m_sub_problem.set_obj_expr(fixed_row - fixed_rhs);
    } else {
        throw Exception("Uncertain equality constraints are not supported.");
    }

}

void idol::CVCCG::Formulation::add_scenario_to_master(const std::list<GeneratedScenario>::iterator& t_iterator_in_pool) {

    for (auto& uncertainty : m_uncertainties) {
        add_scenario_to_master(t_iterator_in_pool, uncertainty);
    }

}

double idol::CVCCG::Formulation::get_scenario_var_primal(const Var& t_var) const {

    double value = 0.;
    unsigned int n_scenarios = 0;
    for (const auto& uncertainty : m_uncertainties) {
        for (const auto& cut : uncertainty.currently_present_cuts()) {
            value += cut.scenario->scenario.get(t_var);
            n_scenarios++;
        }
    }

    return value / n_scenarios;
}

std::list<idol::CVCCG::Formulation::GeneratedScenario>::iterator idol::CVCCG::Formulation::add_scenario_to_pool(PrimalPoint&& t_scenario, PrimalPoint&& t_master_scenario) {

    create_critical_value_variable_if_needed(t_scenario);

    m_scenario_pool.emplace_back(std::move(t_scenario), std::move(t_master_scenario));

    return --m_scenario_pool.end();
}

double idol::CVCCG::Formulation::compute_critical_value(const Ctr& t_ctr, const PrimalPoint& t_scenario) const {

    const auto& model = m_parent.parent();
    const auto& uncertainty_set = m_parent.description().uncertainty_set();

    const auto& row = uncertainty_set.get_ctr_row(t_ctr);
    const auto type = uncertainty_set.get_ctr_type(t_ctr);
    std::cout << type << std::endl;
    assert(type == LessOrEqual);

    double result = uncertainty_set.get_ctr_rhs(t_ctr) + 1;
    for (const auto& [var, coeff] : row) {
        if (!model.has(var)) {
            result -= coeff * t_scenario.get(var);
        }
    }
    assert(is_integer(result, m_parent.get_tol_integer()));

    return result;
}

void idol::CVCCG::Formulation::create_critical_value_variable_if_needed(const PrimalPoint& t_scenario) {

    for (auto& linking : m_linking_constraints) {

        const double critical_value = compute_critical_value(linking.ctr_in_uncertainty_set, t_scenario);

        const auto it = linking.critical_values.find((long int) critical_value);
        if (it != linking.critical_values.end()) {
            continue;
        }

        create_critical_value_variable(t_scenario, linking);

    }

}

void idol::CVCCG::Formulation::create_critical_value_variable(const PrimalPoint& t_scenario, LinkingConstraint& t_linking) {

    const double critical_value = compute_critical_value(t_linking.ctr_in_uncertainty_set, t_scenario);
    const auto& model = m_parent.parent();
    const auto& uncertainty_set = m_parent.description().uncertainty_set();
    const auto& row = uncertainty_set.get_ctr_row(t_linking.ctr_in_uncertainty_set);
    const auto type = uncertainty_set.get_ctr_type(t_linking.ctr_in_uncertainty_set);

    assert(type == LessOrEqual);

    LinExpr<Ctr> column;
    for (const auto& uncertainty : m_uncertainties) {
        for (const auto& cut : uncertainty.currently_present_cuts()) {
            const double local_critical_value = compute_critical_value(t_linking.ctr_in_uncertainty_set, cut.scenario->scenario);
            if ((long int) critical_value <= (long int) local_critical_value) {
                column.set(cut.cut, cut.penalty);
            }
        }
    }

    if (t_linking.cover_constraint) {
        column.set(*t_linking.cover_constraint, 1);
    }

    const auto& activation_var = m_master.add_var(0, 1, Binary, 0, std::move(column), "__" + t_linking.ctr_in_uncertainty_set.name() + "_cv_" + std::to_string((long int)critical_value));

    LinExpr<Var> parameterized_part;
    for (const auto& [var, coeff] : row) {
        if (model.has(var)) {
            parameterized_part += coeff * var;
        }
    }

    const auto& activation_ctr = m_master.add_ctr(critical_value * activation_var <= parameterized_part);

    const auto [it, success] = t_linking.critical_values.emplace((long int)critical_value, std::make_pair(activation_var, activation_ctr));
    assert(success);

    m_n_critical_values += 1;

}

void idol::CVCCG::Formulation::add_scenario_to_master(const std::list<GeneratedScenario>::iterator& t_iterator_in_pool, Uncertainty& t_uncertainty) {

    if (!t_uncertainty.is_constraint()) {
        throw Exception("Not implemented.");
    }

    const auto& ctr = t_uncertainty.ctr();
    const auto& model = m_parent.parent();
    const auto& description = m_parent.description();
    const auto& [scenario, master_solution] = *t_iterator_in_pool;

    const auto type = model.get_ctr_type(ctr);
    auto lhs = model.get_ctr_row(ctr);
    double rhs = model.get_ctr_rhs(ctr);

    for (const auto& [var, unc_coeff] : description.uncertain_mat_coeffs(ctr)) {
        lhs += evaluate(unc_coeff, scenario) * var;
    }
    for (const auto& [unc_param, coeff] : description.uncertain_rhs(ctr)) {
        rhs += coeff * scenario.get(unc_param);
    }

    double penalty = -rhs;
    for (const auto& [var, coeff] : lhs) {
        if (coeff > 0) {
            const double ub = model.get_var_ub(var);
            if (is_pos_inf(ub)) {
                throw Exception("Found variable with infinite bound during big-M computation.");
            }
            penalty += coeff * ub;
        } else {
            const double lb = model.get_var_lb(var);
            if (is_neg_inf(lb)) {
                throw Exception("Found variable with infinite bound during big-M computation.");
            }
            penalty += coeff * lb;
        }
    }

    if (type == LessOrEqual) {
        penalty *= -1.;
    }

    if (m_linking_variables.empty() || m_parent.use_indicator()) { // Add indicator

        for (const auto& var : m_linking_variables) {
            const double val = master_solution.get(var);
            lhs += penalty * ( var - 2 * val * var );
            rhs -= penalty * val;
        }

        m_master.add_ctr(std::move(lhs), type, rhs);

    } else { // Add critical value

        for (auto& linking : m_linking_constraints) {

            const double critical_value = compute_critical_value(linking.ctr_in_uncertainty_set, scenario);

            for (const auto& [local_critical_value, pair] : linking.critical_values) {
                if ((long int) critical_value <= (long int) local_critical_value) {
                    lhs += penalty * pair.first;
                }
            }

        }

        const auto cut = m_master.add_ctr(std::move(lhs), type, rhs);
        t_uncertainty.add_currently_present_cut(cut, t_iterator_in_pool, penalty);

    }

}
