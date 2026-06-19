//
// Created by Henri on 17/04/2026.
//
#include "idol/robust/optimizers/critical-value-column-and-constraint-generation/Formulation.h"
#include "idol/robust/optimizers/critical-value-column-and-constraint-generation/Optimizers_CriticalValueColumnAndConstraintGeneration.h"
#include "idol/mixed-integer/modeling/variables/TempVar.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"
#include "idol/mixed-integer/optimizers/callbacks/cutting-planes/CglCutCallback.h"

idol::CVCCG::Formulation::Formulation(const idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration& t_parent) :
    m_parent(t_parent),
    m_master(t_parent.parent().env()),
    m_sub_problem(t_parent.parent().env())
{

    check_assumptions();
    initialize_master();
    initialize_sub_problem();
    m_global_lower_bound = compute_trivial_lower_bound();

}

void idol::CVCCG::Formulation::check_assumptions() {

    const auto& model = m_parent.parent();
    const auto& description = m_parent.description();
    const auto& uncertainty_set = description.uncertainty_set();

    // Check that all linking variables are binary
    // and checks if has linking variables
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

        if (type != Continuous) {
            m_master_is_continuous = false;
        }
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
    } else {
        m_master.set_obj_expr(model.get_obj_expr());
    }

    // Create cover constraints
    if (!m_parent.use_indicator() && m_use_cover_constraints) {
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
        const double rhs = uncertainty_set.get_ctr_rhs(ctr);
        m_sub_problem.add(ctr, TempCtr(std::move(fixed_row), type, rhs));
    }

    // Set optimizer for sub-problem
    m_sub_problem.use(m_parent.get_deterministic_optimizer_factory());

}

void idol::CVCCG::Formulation::update_sub_problem_rhs(const PrimalPoint& t_master_solution) {

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

    const auto& model = m_parent.parent();
    const auto& description = m_parent.description();

    const LinExpr<Var>* row = nullptr;
    const LinExpr<Var, LinExpr<Var>>* unc_coeffs = nullptr;
    const LinExpr<Var>* unc_constant = nullptr;
    CtrType type = LessOrEqual;

    double fixed_constant = 0.;
    LinExpr fixed_row;

    if (t_uncertainty.is_constraint()) {
        const auto& ctr = t_uncertainty.ctr();

        row = &model.get_ctr_row(ctr);
        type = model.get_ctr_type(ctr);
        fixed_constant = model.get_ctr_rhs(ctr);

        unc_coeffs = &description.uncertain_mat_coeffs(ctr);
        unc_constant = &description.uncertain_rhs(ctr);
    } else {
        row = &model.get_obj_expr().affine().linear();
        unc_coeffs = &description.uncertain_objs();
        fixed_constant = -model.get_obj_expr().affine().constant();
        if (m_epigraph_variable) {
            fixed_constant += t_master_solution.get(*m_epigraph_variable);
        }
    }

    for (const auto& [var, coeff] : *row) {
        fixed_constant -= coeff * t_master_solution.get(var);
    }
    // Can we use evaluate(*unc_coeffs, t_master_solution); instead ?
    for (const auto& [var, unc_coeff] : *unc_coeffs) {
        const double var_val = t_master_solution.get(var);
        if (is_zero(var_val, Tolerance::Sparsity)) {
            continue;
        }
        fixed_row += var_val * unc_coeff;
    }
    if (unc_constant) {
        fixed_row -= *unc_constant;
    }

    if (type == LessOrEqual) {
        m_sub_problem.set_obj_expr(fixed_constant - fixed_row);
    } else if (type == GreaterOrEqual) {
        m_sub_problem.set_obj_expr(fixed_row - fixed_constant);
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

    if (m_master_is_continuous) {

        double value = 0.;
        double norm = 0.;

        for (const auto& uncertainty : m_uncertainties) {
            for (const auto& cut : uncertainty.currently_present_cuts()) {
                const double dual_val = m_master.get_ctr_dual(cut.cut);
                if (is_zero(dual_val, Tolerance::Sparsity)) {
                    continue;
                }
                value += dual_val * cut.scenario->scenario.get(t_var);
                norm += dual_val;
            }
        }

        return value / norm;
    }

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

bool idol::CVCCG::Formulation::master_provides_a_valid_bound() const {
    return m_parent.description().uncertain_obj().empty() || m_epigraph_variable;
}

void idol::CVCCG::Formulation::remove_cut_if(Uncertainty& t_uncertainty, const std::function<bool(const Ctr&, const PrimalPoint&)>& t_indicator) {
    const auto cuts = t_uncertainty.currently_present_cuts();
    for (auto it = cuts.begin() ; it != cuts.end() ; ) {
        if (t_indicator(it->cut, it->scenario->scenario)) {
            m_master.remove(it->cut);
            it = t_uncertainty.remove_from_currently_present_cuts(it);
        } else {
            ++it;
        }
    }
}

void idol::CVCCG::Formulation::set_unc_var_lb(const Var& t_var, double t_lb) {
    m_sub_problem.set_var_lb(t_var, t_lb);
    const double tol_feasibility = m_parent.get_tol_feasibility();
    for (auto& uncertainty : m_uncertainties) {
        remove_cut_if(uncertainty, [&](const Ctr &t_object, const PrimalPoint &t_generator)-> bool {
            return true;
            const double value = t_generator.get(t_var);
            return !is(value, GreaterOrEqual, t_lb, tol_feasibility);
        });
    }
    //m_scenario_pool.clear();
}

void idol::CVCCG::Formulation::set_unc_var_ub(const Var& t_var, double t_ub) {
    m_sub_problem.set_var_ub(t_var, t_ub);
    const double tol_feasibility = m_parent.get_tol_feasibility();
    for (auto& uncertainty : m_uncertainties) {
        remove_cut_if(uncertainty, [&](const Ctr &t_object, const PrimalPoint &t_generator)-> bool {
            return true;
            const double value = t_generator.get(t_var);
            return !is(value, LessOrEqual, t_ub, tol_feasibility);
        });
    }
}

void idol::CVCCG::Formulation::load_cut_from_pool() {

    const auto tol_feasibility = m_parent.get_tol_feasibility();
    for (auto it = m_scenario_pool.begin(), end = m_scenario_pool.end(); it != end; ++it) {
        bool is_feasible = true;
        for (const auto& var : m_parent.branching_candidates()) {
            const double value = it->scenario.get(var);
            const double lb = m_sub_problem.get_var_lb(var);
            const double ub = m_sub_problem.get_var_ub(var);
            if (
                !is(value, GreaterOrEqual, lb, tol_feasibility)
                ||
                !is(value, LessOrEqual, ub, tol_feasibility)
            ) {
                is_feasible = false;
                break;
            }
        }
        if (is_feasible) {
            for (auto& uncertainty : m_uncertainties) {
                add_scenario_to_master(it, uncertainty);
            }
        }
    }

    if (m_epigraph_variable && m_uncertainties.back().currently_present_cuts().size() == 0) {
        m_master.remove(*m_epigraph_variable);
        m_epigraph_variable.reset();
    }

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
    assert(type != Equal);

    double result = uncertainty_set.get_ctr_rhs(t_ctr);
    for (const auto& [var, coeff] : row) {
        if (!model.has(var)) {
            result -= coeff * t_scenario.get(var);
        }
    }
    if (type == GreaterOrEqual) {
        result *= -1.;
    }
    assert(is_integer(result, m_parent.get_tol_integer()));

    return result + 1;
}

double idol::CVCCG::Formulation::compute_trivial_lower_bound() const {

    const auto& model = m_parent.parent();
    const auto& uncertainty_set = m_parent.description().uncertainty_set();
    const auto& objective = m_parent.parent().get_obj_expr();

    assert(!objective.has_quadratic());

    double result = objective.affine().constant();

    for (const auto& [var, coeff] : objective.affine().linear()) {
        if (coeff > 0) {
            const double lb = model.get_var_lb(var);
            assert(!is_neg_inf(lb));
            result += coeff * lb;
        } else {
            const double ub = model.get_var_ub(var);
            assert(!is_pos_inf(ub));
            result += coeff * ub;
        }
    }

    for (const auto& [var, unc_coefff] : m_parent.description().uncertain_objs()) {
        for (const auto& [unc_var, coeff] : unc_coefff) {

            // Underestimate the following
            const double lb_var = model.get_var_lb(var);
            const double ub_var = model.get_var_ub(var);
            const double lb_unc_var = uncertainty_set.get_var_lb(unc_var);
            const double ub_unc_var = uncertainty_set.get_var_ub(unc_var);

            const double v1 = lb_var * lb_unc_var;
            const double v2 = lb_var * ub_unc_var;
            const double v3 = ub_var * lb_unc_var;
            const double v4 = ub_var * ub_unc_var;

            const double min_prod = std::min({v1, v2, v3, v4});
            const double max_prod = std::max({v1, v2, v3, v4});

            if (coeff >= 0) {
                result += coeff * min_prod;
            } else {
                result += coeff * max_prod;
            }

        }
    }

    return result;
}

double idol::CVCCG::Formulation::compute_penalty(const LinExpr<Var, double>& t_row, CtrType t_type, double t_rhs) const {

    // a^\top x \le b => M = max(0, (max_{x\in [l,u]} a^\top x ) - b) ==> a^\top x \le b + Mz
    // a^T x >= b => M = max(0, b - (min_{x in [l,u]} a^T x)) ==> a^\top x >= b -Mz

    const auto& model = m_parent.parent();

    if (t_type == LessOrEqual) {

        double max_activity = 0;

        for (const auto& [var, coeff] : t_row) {
            if (coeff > 0) {
                const double ub = model.get_var_ub(var);
                assert(!is_pos_inf(ub));
                max_activity += coeff * ub;
            } else {
                const double lb = m_epigraph_variable && m_epigraph_variable->id() == var.id() ? m_master.get_var_lb(var) : model.get_var_lb(var);
                assert(!is_neg_inf(lb));
                max_activity += coeff * lb;
            }
        }

        return std::max(0., max_activity - t_rhs);
    }

    double min_activity = 0;

    for (const auto& [var, coeff] : t_row) {
        if (coeff > 0) {
            const double lb = model.get_var_lb(var);
            assert(!is_neg_inf(lb));
            min_activity += coeff * lb;
        } else {
            const double ub = model.get_var_ub(var);
            assert(!is_pos_inf(ub));
            min_activity += coeff * ub;
        }
    }

    return std::max(0., t_rhs - min_activity);

}

void idol::CVCCG::Formulation::create_critical_value_variable_if_needed(const PrimalPoint& t_scenario) {

    if (m_parent.use_indicator()) {
        return;
    }

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

    assert(type != Equal);

    LinExpr<Ctr> column;
    for (const auto& uncertainty : m_uncertainties) {
        for (const auto& cut : uncertainty.currently_present_cuts()) {
            const double local_critical_value = compute_critical_value(t_linking.ctr_in_uncertainty_set, cut.scenario->scenario);
            if ( (type == LessOrEqual && (long int) critical_value <= (long int) local_critical_value)
                || (type == GreaterOrEqual && (long int) critical_value >= (long int) local_critical_value)
            ) {
                column.set(cut.cut, cut.penalty);
            }
        }
    }

    if (t_linking.cover_constraint) {
        column.set(*t_linking.cover_constraint, 1);
    }

    const auto& activation_var = m_master.add_var(0, 1, Binary, 0, std::move(column), "__" + t_linking.ctr_in_uncertainty_set.name() + "_cv_" + std::to_string((long int)critical_value));

    LinExpr<Var> parameterized_part;
    double min_activity = 0.;
    double max_activity = 0.;
    for (const auto& [var, coeff] : row) {
        if (model.has(var)) {
            parameterized_part += coeff * var;

            const double ub = model.get_var_ub(var);
            const double lb = model.get_var_lb(var);
            assert(!is_pos_inf(ub));
            assert(!is_neg_inf(lb));
            if (coeff < 0) {
                min_activity += coeff * ub;
                max_activity += coeff * lb;
            } else {
                assert(!is_neg_inf(lb));
                min_activity += coeff * lb;
                max_activity += coeff * ub;
            }
        }
    }

    double M = min_activity;
    if (type == GreaterOrEqual) {
        parameterized_part *= -1.;
        M = max_activity;
    }

    const auto& activation_ctr = m_master.add_ctr(critical_value * activation_var + M * (1 - activation_var) <= parameterized_part, "__cv_" + std::to_string((long int)critical_value) + "_" + t_linking.ctr_in_uncertainty_set.name());

    const auto [it, success] = t_linking.critical_values.emplace((long int)critical_value, std::make_pair(activation_var, activation_ctr));
    assert(success);

    m_n_critical_values += 1;

}

void idol::CVCCG::Formulation::add_scenario_to_master(const std::list<GeneratedScenario>::iterator& t_iterator_in_pool, Uncertainty& t_uncertainty) {

    const auto& model = m_parent.parent();
    const auto& description = m_parent.description();
    const auto& [scenario, master_solution] = *t_iterator_in_pool;

    const LinExpr<Var, LinExpr<Var>>* unc_row_coeffs = nullptr;
    const LinExpr<Var>* unc_constant = nullptr;
    CtrType type;

    LinExpr<Var> row;
    double constant = 0.;

    if (t_uncertainty.is_constraint()) {

        // We have an uncertain constraint : a^\top x \le b or a^T x >= b

        const auto& ctr = t_uncertainty.ctr();
        row = model.get_ctr_row(ctr);
        type = model.get_ctr_type(ctr);
        constant = model.get_ctr_rhs(ctr);
        unc_row_coeffs = &description.uncertain_mat_coeffs(ctr);
        unc_constant = &description.uncertain_rhs(ctr);

    } else {

        // We rewrite the "objective" in epigraph as c^\top x - theta \le -c_0

        if (!m_epigraph_variable) {
            m_epigraph_variable = m_master.add_var(m_global_lower_bound, Inf, Continuous, 1, "__epigraph");
        }

        row = model.get_obj_expr().affine().linear() - *m_epigraph_variable;
        type = LessOrEqual;
        constant = -model.get_obj_expr().affine().constant();
        unc_row_coeffs = &description.uncertain_objs();
        unc_constant = nullptr;

    }

    // Add uncertain coefficients terms
    if (unc_row_coeffs) {
        for (const auto& [var, unc_coeff] : *unc_row_coeffs) {
            row += evaluate(unc_coeff, scenario) * var;
        }
    }

    // Add uncertain constant terms
    if (unc_constant) {
        for (const auto& [unc_param, coeff] : *unc_constant) {
            constant += coeff * scenario.get(unc_param);
        }
    }

    const double penalty = (type == LessOrEqual ? -1. : 1.) * compute_penalty(row, type, constant);

    if (uses_indicator()) { // Add indicator

        for (const auto& var : m_linking_variables) {
            const double val = master_solution.get(var);
            row += penalty * ( var - 2 * val * var );
            constant -= penalty * val;
        }

        const auto cut = m_master.add_ctr(std::move(row), type, constant);
        t_uncertainty.add_currently_present_cut(cut, t_iterator_in_pool, penalty);

    } else { // Add critical value

        for (auto& linking : m_linking_constraints) {

            const double critical_value = compute_critical_value(linking.ctr_in_uncertainty_set, scenario);

            for (const auto& [local_critical_value, pair] : linking.critical_values) {
                if ((long int) critical_value <= (long int) local_critical_value) {
                    row += penalty * pair.first;
                }
            }

        }

        const auto cut = m_master.add_ctr(std::move(row), type, constant );
        t_uncertainty.add_currently_present_cut(cut, t_iterator_in_pool, penalty);

    }

}


bool idol::CVCCG::Formulation::uses_indicator() const {
    return m_linking_variables.empty() || m_parent.use_indicator() || !m_all_data_in_linking_constraints_is_integer;
}
