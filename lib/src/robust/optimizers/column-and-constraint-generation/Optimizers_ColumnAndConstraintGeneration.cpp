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
                                                                                       const std::list<std::unique_ptr<OptimizerFactory>>& t_optimizer_optimality_separation,
                                                                                       const std::list<std::unique_ptr<OptimizerFactory>>& t_optimizer_joint_separation)
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

    m_optimizer_joint_separation.reserve(t_optimizer_joint_separation.size());
    for (const auto& optimizer : t_optimizer_joint_separation) {
        m_optimizer_joint_separation.emplace_back(optimizer->clone());
    }

}

std::string idol::Optimizers::Robust::ColumnAndConstraintGeneration::name() const {
    return "column-and-constraint generation";
}

double idol::Optimizers::Robust::ColumnAndConstraintGeneration::get_var_primal(const idol::Var &t_var) const {
    return m_formulation->master().get_var_primal(t_var);
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

    if (m_formulation->is_wait_and_see_follower()) {
        if (!m_master_optimizer->is<::idol::Bilevel::OptimizerInterface>()) {
            std::cerr << "Warning: the optimizer for the master problem is not a bilevel optimizer. "
                         "However, the overall problem is a robust bilevel problem with wait-and-see follower." << std::endl;
        } else {
            auto& bilevel_interface = m_master_optimizer->as<::idol::Bilevel::OptimizerInterface>();
            bilevel_interface.set_bilevel_description(m_formulation->bilevel_description_master());
        }
    }

    m_formulation->master().use(*m_master_optimizer);

    m_index_feasibility_separation = 0;
    m_index_optimality_separation = 0;
    m_index_joint_separation = 0;

}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::hook_optimize() {

    add_initial_scenarios();

    log_banner();

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

    if (get_param_logs()) {
        std::cout << "Done in " << m_n_iterations << " iterations and " << time().count() << " seconds." << std::endl;
        std::cout << "Reported status is " << get_status() << " (" << get_reason() << ")." << std::endl;
        std::cout << "Spent " << m_master_timer.cumulative_count() << " solving the master problem and "
                  << m_separation_timer.cumulative_count() << " solving the separation problem." << std::endl;
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
        m_formulation->add_scenario_to_master(scenario, true);
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

    m_formulation->add_scenario_to_master(save_primal(model), true);

}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::solve_master_problem() {

    auto& master = m_formulation->master();
    master.optimizer().set_param_time_limit(get_remaining_time());

    m_master_timer.start();
    master.optimize();
    m_master_timer.stop();

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

    std::cout << std::setw(12) << pretty_double(time().count(), 5) << "s\t"
              << std::setw(5) << m_n_iterations << "\t"
              << std::setw(5) << m_formulation->n_added_scenarios() << "\t"
              << std::setw(12) << pretty_double(get_best_bound(), 5) << "\t"
              << std::setw(12) << pretty_double(get_best_obj(), 5) << "\t"
              << std::setw(12) << pretty_double(get_relative_gap() * 100, 5) << "\t"
              << std::setw(12) << pretty_double(get_absolute_gap(), 5) << "\t"
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

    if (m_with_joint_separation_loop_reset) {
        m_index_joint_separation = 0;
    }

    const unsigned int n_feasibility_separation_optimizers = m_optimizer_feasibility_separation.size();
    const unsigned int n_optimality_separation_optimizers = m_optimizer_optimality_separation.size();
    const unsigned int n_joint_separation_optimizers = m_optimizer_joint_separation.size();

    unsigned int n_added_scenario;
    if (n_feasibility_separation_optimizers > 0) {
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
    }

    if (n_optimality_separation_optimizers > 0) {
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

    if (n_joint_separation_optimizers > 0) {
        for (;;) {

            const bool is_last_optimizer = m_index_joint_separation + 1 == n_joint_separation_optimizers;
            n_added_scenario = solve_joint_adversarial_problem();

            if (n_added_scenario > 0 || is_terminated()) {
                return;
            }

            if (is_last_optimizer) {
                break;
            }

        }
    }

}

unsigned int idol::Optimizers::Robust::ColumnAndConstraintGeneration::solve_feasibility_adversarial_problem() {

    const auto& master = m_formulation->master();
    const auto upper_level_solution = save_primal(master);

    auto [high_point_relaxation, slack_variables] = m_formulation->build_feasibility_separation_problem(upper_level_solution);

    // Set bilevel description
    const auto& separation_bilevel_description = m_formulation->bilevel_description_separation();
    m_optimizer_feasibility_separation[m_index_feasibility_separation]->as<Bilevel::OptimizerInterface>().set_bilevel_description(separation_bilevel_description);

    // Set optimizer
    high_point_relaxation.use(*m_optimizer_feasibility_separation[m_index_feasibility_separation]);
    high_point_relaxation.optimizer().set_param_time_limit(get_remaining_time());

    // Solve adversarial problem
    m_separation_timer.start();
    high_point_relaxation.optimize();
    m_separation_timer.stop();

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
        m_formulation->add_scenario_to_master(scenario, m_with_annotation_for_infeasible_scenario);
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

unsigned int idol::Optimizers::Robust::ColumnAndConstraintGeneration::solve_joint_adversarial_problem() {

    const auto& master = m_formulation->master();
    const auto upper_level_solution = save_primal(master);

    auto [high_point_relaxation, slack_variables] = m_formulation->build_joint_separation_problem(upper_level_solution);

    // Set bilevel description
    const auto& separation_bilevel_description = m_formulation->bilevel_description_separation();
    m_optimizer_joint_separation[m_index_joint_separation]->as<Bilevel::OptimizerInterface>().set_bilevel_description(separation_bilevel_description);

    // Set optimizer
    high_point_relaxation.use(*m_optimizer_joint_separation[m_index_joint_separation]);
    high_point_relaxation.optimizer().set_param_time_limit(get_remaining_time());

    // Solve adversarial problem
    m_separation_timer.start();
    high_point_relaxation.optimize();
    m_separation_timer.stop();

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

    const auto scenario = save_primal(m_robust_description.uncertainty_set(), high_point_relaxation);

    if (m_formulation->should_have_epigraph_and_epigraph_is_not_in_master()) {
        log_iteration(true, high_point_relaxation.optimizer().name(), status, high_point_relaxation.get_reason(), false);
        m_formulation->add_scenario_to_master(scenario, m_with_annotation_for_infeasible_scenario);
        return 1;
    }

    bool is_feasible = true;
    for (unsigned int i = 0, n = std::max<unsigned int>(slack_variables.size(), 1) - 1 ; i < n ; ++i) {
        if (high_point_relaxation.get_var_primal(slack_variables[i]) > Tolerance::Feasibility) {
            is_feasible = false;
            break;
        }
    }

    if (!is_feasible) {
        log_iteration(true, high_point_relaxation.optimizer().name(), status, high_point_relaxation.get_reason(), is_feasible);
        m_formulation->add_scenario_to_master(scenario, m_with_annotation_for_infeasible_scenario);
        return 1;
    }

    log_iteration(false, high_point_relaxation.optimizer().name(), status, high_point_relaxation.get_reason(), is_feasible);

    const double LB = m_formulation->master().get_best_obj();
    const double UB = LB - high_point_relaxation.get_best_obj();
    const bool add_scenario = UB > LB && (absolute_gap(LB, UB) > Tolerance::MIPAbsoluteGap || relative_gap(LB, UB) > Tolerance::MIPRelativeGap);

    if (status == Optimal) {
        set_best_obj(std::min(get_best_obj(), UB));
    }

    if (add_scenario) {
        m_formulation->add_scenario_to_master(scenario, m_with_annotation_for_infeasible_scenario);
        return 1;
    }

    if (status == Feasible) {
        ++m_index_joint_separation;
    }

    return 0;
}

unsigned int
idol::Optimizers::Robust::ColumnAndConstraintGeneration::solve_optimality_adversarial_problem(
        const idol::Point<idol::Var> &t_upper_level_solution, unsigned int t_coupling_constraint_index) {

    Model high_point_relaxation = m_formulation->build_optimality_separation_problem_for_adjustable_robust_problem(
            t_upper_level_solution,
            t_coupling_constraint_index
    );

    // Set bilevel description
    const auto& separation_bilevel_description = m_formulation->bilevel_description_separation();
    m_optimizer_optimality_separation[m_index_optimality_separation]->as<Bilevel::OptimizerInterface>().set_bilevel_description(separation_bilevel_description);

    // Set optimizer
    high_point_relaxation.use(*m_optimizer_optimality_separation[m_index_optimality_separation]);
    high_point_relaxation.optimizer().set_param_time_limit(get_remaining_time());

    // Solve adversarial problem
    m_separation_timer.start();
    high_point_relaxation.optimize();
    m_separation_timer.stop();

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
        m_formulation->add_scenario_to_master(scenario, true);
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
