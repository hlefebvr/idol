//
// Created by Henri on 17/04/2026.
//
#include "idol/robust/optimizers/critical-value-column-and-constraint-generation/Formulation.h"
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
update_sub_problem_objective(const PrimalPoint& t_master_solution, unsigned int t_index) {

    const auto& uncertainty = m_uncertainties[t_index];

    if (!uncertainty.is_constraint()) {
        throw Exception("Not implemented.");
    }

    const auto& model = m_parent.parent();
    const auto& description = m_parent.description();
    const auto& ctr = uncertainty.ctr();
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
    for (const auto& [unc_param, coeff] : description.uncertain_rhs(ctr)) {
        fixed_row += coeff * unc_param;
    }

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

std::list<idol::CVCCG::Formulation::GeneratedScenario>::iterator idol::CVCCG::Formulation::add_scenario_to_pool(PrimalPoint&& t_scenario, PrimalPoint&& t_master_scenario) {

    m_scenario_pool.emplace_back(std::move(t_scenario), std::move(t_master_scenario));

    return --m_scenario_pool.end();
}

void idol::CVCCG::Formulation::add_scenario_to_master(const std::list<GeneratedScenario>::iterator& t_iterator_in_pool, Uncertainty& t_uncertainty) {

    if (!t_uncertainty.is_constraint()) {
        throw Exception("Not implemented.");
    }

    const auto& ctr = t_uncertainty.ctr();
    const auto& model = m_parent.parent();
    const auto& description = m_parent.description();
    const auto& uncertainty_set = description.uncertainty_set();
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

    const double penalty = (type == LessOrEqual ? -1. : 1.) * 1e4;
    std::cerr << "Warning: Penalty parameter is arbitrary... " << penalty << std::endl;

    if (m_linking_constraints.empty() || m_use_indicator) { // add indicator

        for (const auto& var : m_linking_variables) {
            const double val = master_solution.get(var);
            lhs += penalty * ( var - 2 * val * var );
            rhs -= penalty * val;
        }

        m_master.add_ctr(std::move(lhs), type, rhs);

    } else { // Add critical value

        LinExpr sum_of_activations;
        for (auto& linking : m_linking_constraints) {

            const auto& row = uncertainty_set.get_ctr_row(linking.ctr_in_uncertainty_set);
            const auto type = uncertainty_set.get_ctr_type(linking.ctr_in_uncertainty_set);
            assert(type == LessOrEqual);

            double critical_value = uncertainty_set.get_ctr_rhs(linking.ctr_in_uncertainty_set) + 1;
            LinExpr parameterized_part;
            for (const auto& [var, coeff] : row) {
                if (model.has(var)) {
                    parameterized_part += coeff * var;
                } else {
                    critical_value -= coeff * scenario.get(var);
                }
            }
            const auto activation = m_master.add_var(0, 1, Binary, 0);
            m_master.add_ctr(critical_value * activation <= parameterized_part);

            assert(is_integer(critical_value, m_parent.get_tol_integer()));

            const auto [it, success] = linking.critical_values.emplace((long int) critical_value, activation);
            if (!success) {
                std::cout << "Could recycle critical value " << critical_value << std::endl;
            }

            lhs += penalty * activation;
            sum_of_activations += activation;

        }

        m_master.add_ctr(std::move(lhs), type, rhs);
        m_master.add_ctr(sum_of_activations <= 1);

    }

}