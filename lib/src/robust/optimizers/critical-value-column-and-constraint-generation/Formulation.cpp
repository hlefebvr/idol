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

        m_has_linking_variables = true;

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
    std::cerr << "Skipped checking all data is integer since working with no-good cuts for now" << std::endl;
    for (const auto& ctr : uncertainty_set.ctrs()) {
        // TODO
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

    if (!m_has_linking_variables) {
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

void idol::CVCCG::Formulation::add_scenario(const PrimalPoint& t_scenario) {
    std::cout << "todo: add scenario\n" << t_scenario << std::endl;
}
