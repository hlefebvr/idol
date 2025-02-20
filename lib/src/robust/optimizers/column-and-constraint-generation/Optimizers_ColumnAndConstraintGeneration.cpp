//
// Created by henri on 11.12.24.
//
#include <idol/robust/optimizers/column-and-constraint-generation/Optimizers_ColumnAndConstraintGeneration.h>
#include <idol/mixed-integer/modeling/expressions/operations/operators.h>
#include <idol/bilevel/optimizers/BilevelOptimizerInterface.h>
#include <idol/bilevel/optimizers/PessimisticAsOptimistic/PessimisticAsOptimistic.h>
#include <idol/bilevel/modeling/write_to_file.h>
#include "idol/general/optimizers/logs.h"

idol::Optimizers::Robust::ColumnAndConstraintGeneration::ColumnAndConstraintGeneration(const idol::Model &t_parent,
                                                                                       const idol::Robust::Description &t_robust_description,
                                                                                       const idol::Bilevel::Description &t_bilevel_description,
                                                                                       const idol::OptimizerFactory &t_master_optimizer,
                                                                                       std::vector<Point<Var>> t_initial_scenarios,
                                                                                       OptimizerFactory* t_initial_scenario_by_minimization,
                                                                                       OptimizerFactory* t_initial_scenario_by_maximization,
                                                                                       const std::list<std::unique_ptr<OptimizerFactory>>& t_optimizer_feasibility_separation,
                                                                                       const std::list<std::unique_ptr<OptimizerFactory>>& t_optimizer_optimality_separation)
                                                                                       : Algorithm(t_parent),
                                                                                         m_robust_description(t_robust_description),
                                                                                         m_bilevel_description(t_bilevel_description),
                                                                                         m_master_optimizer(t_master_optimizer.clone()),
                                                                                         m_initial_scenario_by_minimization(t_initial_scenario_by_minimization),
                                                                                         m_initial_scenario_by_maximization(t_initial_scenario_by_maximization),
                                                                                         m_initial_scenarios(std::move(t_initial_scenarios)) {

    m_optimizer_feasibility_separation.reserve(t_optimizer_feasibility_separation.size());
    for (const auto& optimizer : t_optimizer_feasibility_separation) {
        m_optimizer_feasibility_separation.emplace_back(optimizer->clone());
    }

    m_optimizer_optimality_separation.reserve(t_optimizer_optimality_separation.size());
    for (const auto& optimizer : t_optimizer_optimality_separation) {
        m_optimizer_optimality_separation.emplace_back(optimizer->clone());
    }

}

std::string idol::Optimizers::Robust::ColumnAndConstraintGeneration::name() const {
    return "column-and-constraint generation";
}

double idol::Optimizers::Robust::ColumnAndConstraintGeneration::get_var_primal(const idol::Var &t_var) const {
    throw Exception("Not implemented get_var_primal");
}

double idol::Optimizers::Robust::ColumnAndConstraintGeneration::get_var_reduced_cost(const idol::Var &t_var) const {
    throw Exception("Not implemented get_var_reduced_cost");
}

double idol::Optimizers::Robust::ColumnAndConstraintGeneration::get_var_ray(const idol::Var &t_var) const {
    throw Exception("Not implemented get_var_ray");
}

double idol::Optimizers::Robust::ColumnAndConstraintGeneration::get_ctr_dual(const idol::Ctr &t_ctr) const {
    throw Exception("Not implemented get_ctr_dual");
}

double idol::Optimizers::Robust::ColumnAndConstraintGeneration::get_ctr_farkas(const idol::Ctr &t_ctr) const {
    throw Exception("Not implemented get_ctr_farkas");
}

unsigned int idol::Optimizers::Robust::ColumnAndConstraintGeneration::get_n_solutions() const {
    throw Exception("Not implemented get_n_solutions");
}

unsigned int idol::Optimizers::Robust::ColumnAndConstraintGeneration::get_solution_index() const {
    throw Exception("Not implemented get_solution_index");
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::add(const idol::Var &t_var) {
    throw Exception("Not implemented add");
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::add(const idol::Ctr &t_ctr) {
    throw Exception("Not implemented add");
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::add(const idol::QCtr &t_ctr) {
    throw Exception("Not implemented add");
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::remove(const idol::Var &t_var) {
    throw Exception("Not implemented remove");
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::remove(const idol::Ctr &t_ctr) {
    throw Exception("Not implemented remove");
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::remove(const idol::QCtr &t_ctr) {
    throw Exception("Not implemented remove");
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::update() {

}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::write(const std::string &t_name) {
    throw Exception("Not implemented write");
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::hook_before_optimize() {
    Optimizer::hook_before_optimize();

    m_formulation = std::make_unique<CCG::Formulation>(parent(),
                                                       m_robust_description,
                                                       m_bilevel_description);
    m_formulation->master().use(*m_master_optimizer);

    m_index_feasibility_separation = 0;
    m_index_optimality_separation = 0;

    log_banner();

}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::hook_optimize() {

    add_initial_scenarios();

    while (true) {

        solve_master_problem();

        check_termination_criteria();

        if (is_terminated()) {
            break;
        }

        solve_adversarial_problem();

        log_iteration_separator();

        check_termination_criteria();

        if (is_terminated()) {
            break;
        }

        ++m_n_iterations;
    }

}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::set_solution_index(unsigned int t_index) {
    throw Exception("Not implemented set_solution_index");
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::update_obj_sense() {
    throw Exception("Not implemented update_obj_sense");
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::update_obj() {
    throw Exception("Not implemented update_obj");
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::update_rhs() {
    throw Exception("Not implemented update_rhs");
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::update_obj_constant() {
    throw Exception("Not implemented update_obj_constant");
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::update_mat_coeff(const idol::Ctr &t_ctr,
                                                                               const idol::Var &t_var) {
    throw Exception("Not implemented update_mat_coeff");
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::update_ctr_type(const idol::Ctr &t_ctr) {
    throw Exception("Not implemented update_ctr_type");
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::update_ctr_rhs(const idol::Ctr &t_ctr) {
    throw Exception("Not implemented update_ctr_rhs");
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::update_var_type(const idol::Var &t_var) {
    throw Exception("Not implemented update_var_type");
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::update_var_lb(const idol::Var &t_var) {
    throw Exception("Not implemented update_var_lb");
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::update_var_ub(const idol::Var &t_var) {
    throw Exception("Not implemented update_var_ub");
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::update_var_obj(const idol::Var &t_var) {
    throw Exception("Not implemented update_var_obj");
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::add_initial_scenarios() {

    for (const auto& scenario : m_initial_scenarios) {
        m_formulation->add_scenario_to_master(scenario);
    }

    if (m_initial_scenario_by_minimization) {
        add_initial_scenario_by_min_or_max(*m_initial_scenario_by_minimization, 1.);
    }

    if (m_initial_scenario_by_maximization) {
        add_initial_scenario_by_min_or_max(*m_initial_scenario_by_maximization, -1.);
    }

}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::add_initial_scenario_by_min_or_max(const OptimizerFactory& t_optimizer, double t_coefficient) {

    Model model = m_robust_description.uncertainty_set().copy();
    model.set_obj_expr(idol_Sum(var, model.vars(), t_coefficient * var));
    model.use(t_optimizer);
    const double time_limit = std::min<double>(model.optimizer().get_remaining_time(), get_remaining_time());
    model.optimizer().set_param_time_limit(time_limit);
    model.optimize();

    const auto status = model.get_status();
    if (status != Optimal && status != Feasible) {
        throw Exception("Initial scenario by minimization failed.");
    }

    m_formulation->add_scenario_to_master(save_primal(model));

}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::solve_master_problem() {

    auto& master = m_formulation->master();
    master.optimizer().set_param_time_limit(get_remaining_time());
    master.optimize();

    const auto status = master.get_status();

    if (status != Optimal) {
        set_status(status);
        set_reason(master.get_reason());
        terminate();
        return;
    }

    set_best_bound(master.get_best_bound());

}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::check_termination_criteria() {

    if (get_best_bound() > get_best_obj() + 1e-3) {
        set_status(Fail);
        set_reason(Numerical);
        terminate();
        return;
    }

    if (get_remaining_time() == 0) {
        set_reason(TimeLimit);
        terminate();
        return;
    }

    if (m_n_iterations > get_param_iteration_limit()) {
        set_reason(IterLimit);
        terminate();
        return;
    }

    if (Algorithm::get_relative_gap() <= get_tol_mip_relative_gap() || Algorithm::get_absolute_gap() <= get_tol_mip_absolute_gap()) {
        set_status(Optimal);
        set_reason(Proved);
        terminate();
        return;
    }

}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::log_banner() {

    if (!get_param_logs()) {
        return;
    }

    center(std::cout, "", 182, '*') << std::endl;
    center(std::cout, " Column-and-constraint Generation ", 182) << std::endl;
    center(std::cout, "", 182, '*') << std::endl;

    std::cout << std::setw(12) << "Time (s)" << "\t"
              << std::setw(5) << "# Iter." << "\t"
              << std::setw(5) << "# Scen." << "\t"
              << std::setw(12) << "LB" << "\t"
              << std::setw(12) << "UB" << "\t"
              << std::setw(12) << "Gap (%)" << "\t"
              << std::setw(12) << "Abs. Gap" << "\t"
              << std::setw(35) << "Master" << "\t"
              << std::setw(35) << "Separation" << "\t"
              << std::setw(8) << "Iter. status"
              << std::endl;
    log_iteration_separator();
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::log_iteration(
            bool t_is_feasibility_separation,
            const std::string& t_optimizer_name,
            const SolutionStatus& t_status,
            const SolutionReason& t_reason,
            bool t_separation_outcome
        ) {

    if (!get_param_logs()) {
        return;
    }

    const auto& master = m_formulation->master();
    std::stringstream master_status;
    master_status << "<" << master.optimizer().name() << "," << master.get_status() << "," << master.get_reason() << ">";

    std::stringstream separation_status;
    separation_status << "<" << t_optimizer_name << "," << t_status << "," << t_reason << ">";

    std::stringstream separation_outcome;
    if (t_is_feasibility_separation) {
        separation_outcome << "feasibility=";
        if (t_separation_outcome) {
            separation_outcome << (t_status == Optimal ? "yes" : "?");
        } else {
            separation_outcome << "no";
        }
    } else {
        separation_outcome << "optimality=";
        if (t_separation_outcome) {
            separation_outcome << "yes";
        } else {
            separation_outcome << (t_status == Optimal ? "no" : "?");
        }
    }

    std::cout << std::setw(12) << time().count() << "s\t"
              << std::setw(5) << m_n_iterations << "\t"
              << std::setw(5) << m_formulation->n_added_scenarios() << "\t"
              << std::setw(12) << get_best_bound() << "\t"
              << std::setw(12) << get_best_obj() << "\t"
              << std::setw(12) << get_relative_gap() * 100 << "\t"
              << std::setw(12) << get_absolute_gap() << "\t"
              << std::setw(35) << master_status.str() << "\t"
              << std::setw(35) << separation_status.str() << "\t"
              << std::setw(8) << separation_outcome.str()
              << std::endl;

}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::solve_adversarial_problem() {

    if (m_with_feasibility_separation_loop_reset) {
        m_index_feasibility_separation = 0;
    }

    if (m_with_optimality_separation_loop_reset) {
        m_index_optimality_separation = 0;
    }

    const unsigned int n_feasibility_separation_optimizers = m_optimizer_feasibility_separation.size();
    const unsigned int n_optimality_separation_optimizers = m_optimizer_optimality_separation.size();

    unsigned int n_added_scenario;
    for (;;) {

        const bool is_last_optimizer = m_index_feasibility_separation + 1 == n_feasibility_separation_optimizers;
        n_added_scenario = solve_feasibility_adversarial_problem();

        if (n_added_scenario > 0 || is_terminated()) {
            return;
        }

        if (is_last_optimizer) {
            break;
        }

    }

    for (;;) {

        const bool is_last_optimizer = m_index_optimality_separation + 1 == n_optimality_separation_optimizers;
        n_added_scenario = solve_optimality_adversarial_problem();

        if (n_added_scenario > 0 || is_terminated()) {
            return;
        }

        if (is_last_optimizer) {
            break;
        }

    }

}

unsigned int idol::Optimizers::Robust::ColumnAndConstraintGeneration::solve_feasibility_adversarial_problem() {

    const auto& master = m_formulation->master();
    const auto upper_level_solution = save_primal(master);

    Model high_point_relaxation = m_formulation->build_feasibility_separation_problem(upper_level_solution);

    // Set bilevel description
    const auto& separation_bilevel_description = m_formulation->separation_bilevel_description();
    m_optimizer_feasibility_separation[m_index_feasibility_separation]->as<Bilevel::OptimizerInterface>().set_bilevel_description(separation_bilevel_description);

    // Set optimizer
    high_point_relaxation.use(*m_optimizer_feasibility_separation[m_index_feasibility_separation]);
    high_point_relaxation.optimizer().set_param_time_limit(get_remaining_time());

    // Solve adversarial problem
    high_point_relaxation.optimize();

    // Analyze results
    const auto status = high_point_relaxation.get_status();

    const bool is_last_optimizer = m_index_feasibility_separation == m_optimizer_feasibility_separation.size() - 1;

    if (status != Optimal && status != Feasible) {
        if (!is_last_optimizer) { // If we can, skip this optimizer
            ++m_index_feasibility_separation;
        } else { // otherwise, it's a fail
            set_status(Fail);
            set_reason(high_point_relaxation.get_reason());
            terminate();
            return 0;
        }
    }

    if (is_last_optimizer && status != Optimal) { // if the last optimizer is not reporting optimal, it's a fail
        set_status(Fail);
        set_reason(high_point_relaxation.get_reason());
        terminate();
        return 0;
    }

    const double objective_value = -high_point_relaxation.get_best_obj();
    const bool is_feasible = objective_value <= Tolerance::Feasibility;

    log_iteration(true, high_point_relaxation.optimizer().name(), status, high_point_relaxation.get_reason(), is_feasible);

    if (!is_feasible) {
        const auto scenario = save_primal(m_robust_description.uncertainty_set(), high_point_relaxation);
        m_formulation->add_scenario_to_master(scenario);
        return 1;
    }

    if (status == Feasible) {
        ++m_index_feasibility_separation;
    }

    return 0;
}

unsigned int idol::Optimizers::Robust::ColumnAndConstraintGeneration::solve_optimality_adversarial_problem() {

    const auto& master = m_formulation->master();
    const auto upper_level_solution = save_primal(master);

    return solve_optimality_adversarial_problem(upper_level_solution);

}

bool idol::Optimizers::Robust::ColumnAndConstraintGeneration::is_adjustable_robust_problem() const {
    return m_bilevel_description.lower_level_obj().is_zero(Tolerance::Feasibility);
}

unsigned int
idol::Optimizers::Robust::ColumnAndConstraintGeneration::solve_optimality_adversarial_problem(const Point<Var>& t_upper_level_solution) {

    const unsigned int n_coupling_constraints = m_formulation->n_coupling_constraints();
    unsigned int total_n_added_scenarios = 0;

    for (unsigned int k = 0 ; k < n_coupling_constraints ; ++k) {
        const unsigned int n_added_scenarios = solve_optimality_adversarial_problem(t_upper_level_solution, k);
        total_n_added_scenarios += n_added_scenarios;
    }

    return total_n_added_scenarios;
}

unsigned int
idol::Optimizers::Robust::ColumnAndConstraintGeneration::solve_optimality_adversarial_problem(
        const idol::Point<idol::Var> &t_upper_level_solution, unsigned int t_coupling_constraint_index) {

    Model high_point_relaxation = m_formulation->build_optimality_separation_problem_for_adjustable_robust_problem(
            t_upper_level_solution,
            t_coupling_constraint_index
    );

    // Set bilevel description
    const auto& separation_bilevel_description = m_formulation->separation_bilevel_description();
    m_optimizer_optimality_separation[m_index_optimality_separation]->as<Bilevel::OptimizerInterface>().set_bilevel_description(separation_bilevel_description);

    // Set optimizer
    high_point_relaxation.use(*m_optimizer_optimality_separation[m_index_optimality_separation]);
    high_point_relaxation.optimizer().set_param_time_limit(get_remaining_time());

    // Solve adversarial problem
    high_point_relaxation.optimize();

    // Analyze results
    const auto status = high_point_relaxation.get_status();

    const bool is_last_optimizer = m_index_optimality_separation == m_optimizer_optimality_separation.size() - 1;

    if (status != Optimal && status != Feasible) {
        if (!is_last_optimizer) { // If we can, skip this optimizer
            ++m_index_feasibility_separation;
        } else { // otherwise, it's a fail
            set_status(Fail);
            set_reason(high_point_relaxation.get_reason());
            terminate();
            return 1;
        }
    }

    if (is_last_optimizer && status != Optimal) { // if the last optimizer is not reporting optimal, it's a fail
        set_status(Fail);
        set_reason(high_point_relaxation.get_reason());
        terminate();
        return 1;
    }

    bool add_scenario;
    if (t_coupling_constraint_index == 0) {
        const double LB = m_formulation->master().get_best_obj();
        const double UB = -high_point_relaxation.get_best_obj();
        add_scenario = UB > LB && (absolute_gap(LB, UB) > Tolerance::MIPAbsoluteGap || relative_gap(LB, UB) > Tolerance::MIPRelativeGap);
        if (status == Optimal) {
            set_best_obj(std::min(get_best_obj(), -high_point_relaxation.get_best_obj()));
        }
    } else {
        // TODO check with Tolerance::Feasiblity
        throw Exception("Coupling constraints not yet implemented");
    }

    log_iteration(false, high_point_relaxation.optimizer().name(), status, high_point_relaxation.get_reason(), add_scenario);

    if (add_scenario) {
        const auto scenario = save_primal(m_robust_description.uncertainty_set(), high_point_relaxation);
        m_formulation->add_scenario_to_master(scenario);
        return 1;
    }

    if (status == Feasible) {
        ++m_index_optimality_separation;
    }

    return 0;
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::log_iteration_separator() {

    if (!get_param_logs()) {
        return;
    }

    center(std::cout, "-", 182, '-') << std::endl;

}
