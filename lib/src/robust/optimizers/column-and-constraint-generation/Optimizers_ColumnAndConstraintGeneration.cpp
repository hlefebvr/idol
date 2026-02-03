//
// Created by henri on 11.12.24.
//
#include <idol/robust/optimizers/column-and-constraint-generation/Optimizers_ColumnAndConstraintGeneration.h>
#include <idol/mixed-integer/modeling/expressions/operations/operators.h>
#include <idol/bilevel/optimizers/BilevelOptimizerInterface.h>
#include <idol/bilevel/optimizers/PessimisticAsOptimistic/PessimisticAsOptimistic.h>
#include <idol/bilevel/modeling/write_to_file.h>
#include <cassert>
#include "idol/general/optimizers/logs.h"

idol::Optimizers::Robust::ColumnAndConstraintGeneration::ColumnAndConstraintGeneration(const idol::Model &t_parent,
                                                                                       const idol::Robust::Description &t_robust_description,
                                                                                       const idol::Bilevel::Description &t_bilevel_description,
                                                                                       const idol::OptimizerFactory &t_master_optimizer,
                                                                                       std::vector<Point<Var>> t_initial_scenarios,
                                                                                       OptimizerFactory* t_initial_scenario_by_minimization,
                                                                                       OptimizerFactory* t_initial_scenario_by_maximization,
                                                                                       const std::list<std::unique_ptr<idol::Robust::CCG::Separation>>& t_separations,
                                                                                       bool t_check_for_repeated_scenarios)
                                                                                       : Algorithm(t_parent),
                                                                                         m_robust_description(t_robust_description),
                                                                                         m_bilevel_description(t_bilevel_description),
                                                                                         m_master_optimizer(t_master_optimizer.clone()),
                                                                                         m_initial_scenario_by_minimization(t_initial_scenario_by_minimization),
                                                                                         m_initial_scenario_by_maximization(t_initial_scenario_by_maximization),
                                                                                         m_initial_scenarios(std::move(t_initial_scenarios)),
                                                                                         m_check_for_repeated_scenarios(t_check_for_repeated_scenarios) {

    m_separations.reserve(t_separations.size());
    for (const auto& optimizer : t_separations) {
        m_separations.emplace_back(optimizer->clone());
    }

}

const idol::PrimalPoint& idol::Optimizers::Robust::ColumnAndConstraintGeneration::get_last_master_solution() const {
    if (!m_last_master_solution) {
        throw Exception("Last master solution is not available.");
    }
    return *m_last_master_solution;
}

std::string idol::Optimizers::Robust::ColumnAndConstraintGeneration::name() const {
    return "column-and-constraint generation";
}

double idol::Optimizers::Robust::ColumnAndConstraintGeneration::get_var_primal(const idol::Var &t_var) const {
    if (m_bilevel_description.is_lower(t_var)) {
        return 0;
    }
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

    m_separation_index = 0;

}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::hook_optimize() {

    add_initial_scenarios();

    log_banner();

    while (true) {

        solve_master_problem();

        if (is_terminated()) { break; }

        solve_adversarial_problem();

        if (is_terminated()) { break; }

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
        m_formulation->add_scenario_to_master(scenario, true, m_check_for_repeated_scenarios);
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

    m_formulation->add_scenario_to_master(save_primal(model), true, m_check_for_repeated_scenarios);

}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::solve_master_problem() {

    auto& master = m_formulation->master();
    master.optimizer().set_param_time_limit(get_remaining_time());

    m_master_timer.start();
    master.optimize();
    m_master_timer.stop();

    const auto status = master.get_status();
    const auto reason = master.get_reason();

    if (status != Optimal) {
        if (reason == TimeLimit) {
            set_status(is_pos_inf(get_best_obj()) ? Infeasible : Feasible);
        } else {
            set_status(status);
        }
        set_reason(reason);
        terminate();
        return;
    }

    // Save master solution
    m_last_master_solution = PrimalPoint();
    m_last_master_solution->set_objective_value(master.get_best_obj());
    m_last_master_solution->set_status(status);
    m_last_master_solution->set_reason(reason);
    for (const auto& var : parent().vars()) {
        if (get_bilevel_description().is_upper(var)) {
            m_last_master_solution->set(var, master.get_var_primal(var));
        }
    }
    if (m_formulation->has_second_stage_epigraph()) {
        const auto& epigraph = m_formulation->second_stage_epigraph();
        m_last_master_solution->set(epigraph, master.get_var_primal(epigraph));
    }

    // Update lower bound
    if (!m_formulation->should_have_epigraph_and_epigraph_is_not_in_master()) {
        set_best_bound(master.get_best_bound());
    }

    check_termination_criteria();
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::check_termination_criteria() {
    
    if (is_terminated()) {
        return;
    }

    if (get_best_bound() > get_best_obj() + get_tol_mip_absolute_gap()) {
        std::cerr << "The current best obj. is larger than current best bound. This should should not happen. Terminating..." << std::endl;
        set_status(Fail);
        set_reason(Numerical);
        terminate();
        return;
    }

    set_status(is_pos_inf(get_best_obj()) ? Infeasible : Feasible);

    if (get_remaining_time() == 0) {
        std::cout << "The time limit has been reached. Terminating..." << std::endl;
        set_reason(TimeLimit);
        terminate();
        return;
    }

    if (m_n_iterations > get_param_iteration_limit()) {
        std::cout << "The iteration limit has been reached. Terminating..." << std::endl;
        set_reason(IterLimit);
        terminate();
        return;
    }

    if (Algorithm::get_relative_gap() <= get_tol_mip_relative_gap() || Algorithm::get_absolute_gap() <= get_tol_mip_absolute_gap()) {
        std::cout << "The optimality gap has been closed. Terminating..." << std::endl;
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

    center(std::cout, "", 92, '*') << std::endl;
    center(std::cout, " Column-and-constraint Generation ", 92) << std::endl;
    center(std::cout, "", 92, '*') << std::endl;

    std::cout << std::setw(12) << "Time (s)" << "\t"
              << std::setw(5) << "# Iter." << "\t"
              << std::setw(5) << "# Scen." << "\t"
              << std::setw(12) << "LB" << "\t"
              << std::setw(12) << "UB" << "\t"
              << std::setw(12) << "Gap (%)" << "\t"
              << std::setw(12) << "Abs. Gap"
              << std::endl;
    log_iteration_separator();
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::log_iteration() {

    if (!get_param_logs()) {
        return;
    }

    std::cout << std::setw(12) << pretty_double(time().count(), 5) << "s\t"
              << std::setw(5) << m_n_iterations << "\t"
              << std::setw(5) << m_formulation->n_added_scenarios() << "\t"
              << std::setw(12) << pretty_double(get_best_bound(), 5) << "\t"
              << std::setw(12) << pretty_double(get_best_obj(), 5) << "\t"
              << std::setw(12) << pretty_double(get_relative_gap() * 100, 5) << "\t"
              << std::setw(12) << pretty_double(get_absolute_gap(), 5)
              << std::endl;

}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::solve_adversarial_problem() {

    /*
    TODO: should we do smth like this again ?
    if (m_with_feasibility_separation_loop_reset) {
        m_index_feasibility_separation = 0;
    }
    */

    const unsigned int n_separations = m_separations.size();

    for (unsigned int i = 0 ; i < n_separations ; ++i) {

        auto& separation = *m_separations[i];

        separation.m_scenarios.clear();
        separation.m_submitted_upper_bound.reset();
        separation.m_parent = this;
        separation.m_status = Loaded;
        separation.m_reason = NotSpecified;

        m_separation_timer.start();
        separation();
        m_separation_timer.stop();

        if (separation.m_submitted_upper_bound && *separation.m_submitted_upper_bound < get_best_obj()) {
            set_best_obj(*separation.m_submitted_upper_bound);
            set_status(Feasible);
        }

        const bool is_last_separation = i == n_separations - 1;
        const auto& scenarios_to_add = separation.scenarios();

        if (is_last_separation && separation.m_status != Optimal && scenarios_to_add.size() == 0) {
            set_reason(separation.m_reason);
            terminate();
        } else if (separation.m_status != Feasible && separation.m_status != Optimal) {
            set_status(separation.m_status);
            set_reason(separation.m_reason);
            std::cerr << "Separation reported status " << separation.m_status << " (" << separation.m_reason << ")." << std::endl;
            terminate();
        }

        if (check_termination_criteria() ; is_terminated()) {
            return;
        }

        for (const auto& scenario : scenarios_to_add) {
            m_formulation->add_scenario_to_master(scenario, m_with_annotation_for_infeasible_scenario, m_check_for_repeated_scenarios);
        }

        log_iteration();

        if (scenarios_to_add.size() > 0) {
            return;
        }

        if (is_last_separation) {
            set_best_obj(get_best_bound());
            set_status(Optimal);
            set_reason(Proved);
            terminate();
            return;
        }

    }

    set_status(Fail);
    set_reason(NotSpecified);
    terminate();

}

/*
unsigned int idol::Optimizers::Robust::ColumnAndConstraintGeneration::solve_feasibility_adversarial_problem() {

    auto [high_point_relaxation, slack_variables] = m_formulation->build_feasibility_separation_problem(*m_last_master_solution);

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
        m_formulation->add_scenario_to_master(scenario, m_with_annotation_for_infeasible_scenario, m_check_for_repeated_scenarios);
        return 1;
    }

    if (status == Feasible) {
        ++m_index_feasibility_separation;
    }

    return 0;
}

unsigned int idol::Optimizers::Robust::ColumnAndConstraintGeneration::solve_optimality_adversarial_problem() {

    const auto& master = m_formulation->master();

    auto high_point_relaxation = m_formulation->build_optimality_separation_problem(*m_last_master_solution);

    // Set bilevel description and optimizer
    const auto& separation_bilevel_description = m_formulation->bilevel_description_separation();
    if (m_formulation->is_adjustable_robust_problem()) {
        m_optimizer_optimality_separation[m_index_optimality_separation]->as<Bilevel::OptimizerInterface>().set_bilevel_description(separation_bilevel_description);
        high_point_relaxation.use(*m_optimizer_optimality_separation[m_index_optimality_separation]);
    } else {
        high_point_relaxation.use(Bilevel::PessimisticAsOptimistic(separation_bilevel_description) + *m_optimizer_optimality_separation[m_index_optimality_separation]);
    }
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
        } else { // otherwise, it's a failure
            std::cerr << "Last optimizer is nor optimal nor feasible (status=" << high_point_relaxation.get_status() << ", reason=" << high_point_relaxation.get_reason() << ")." << std::endl;
            set_status(Fail);
            set_reason(high_point_relaxation.get_reason());
            terminate();
            return 1;
        }
    }

    if (is_last_optimizer && status != Optimal) { // if the last optimizer is not reporting optimal, it's a fail
        std::cerr << "Last optimizer is not optimal. " << high_point_relaxation.get_reason() << "." << std::endl;
        set_status(Fail);
        set_reason(high_point_relaxation.get_reason());
        terminate();
        return 1;
    }

    const double LB = get_best_bound();
    const double UB = -high_point_relaxation.get_best_obj();
    const bool add_scenario = m_formulation->should_have_epigraph_and_epigraph_is_not_in_master() || (absolute_gap(LB, UB) >= Tolerance::MIPAbsoluteGap || relative_gap(LB, UB) >= Tolerance::MIPRelativeGap);

    if (status == Optimal) {
        set_best_obj(std::min(get_best_obj(), UB));
    } else if (status == Feasible) {
        ++m_index_optimality_separation;
    }

    log_iteration(false, high_point_relaxation.optimizer().name(), status, high_point_relaxation.get_reason(), add_scenario);

    if (add_scenario) {
        const auto scenario = save_primal(m_robust_description.uncertainty_set(), high_point_relaxation);
        m_formulation->add_scenario_to_master(scenario, true, m_check_for_repeated_scenarios);
        return 1;
    }

    return 0;

}

unsigned int idol::Optimizers::Robust::ColumnAndConstraintGeneration::solve_joint_adversarial_problem() {

    const auto& master = m_formulation->master();

    auto [high_point_relaxation, slack_variables] = m_formulation->build_joint_separation_problem(*m_last_master_solution);

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

    const bool is_last_optimizer = m_index_joint_separation == m_optimizer_joint_separation.size() - 1;

    if (status != Optimal && status != Feasible) {
        if (!is_last_optimizer) { // If we can, skip this optimizer
            ++m_index_joint_separation;
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
        m_formulation->add_scenario_to_master(scenario, m_with_annotation_for_infeasible_scenario, m_check_for_repeated_scenarios);
        return 1;
    }

    bool is_feasible = true;
    for (unsigned int i = 0, n = std::max<unsigned int>(slack_variables.size(), 1) ; i < n ; ++i) {
        if (high_point_relaxation.get_var_primal(slack_variables[i]) > Tolerance::Feasibility) {
            is_feasible = false;
            break;
        }
    }

    if (!is_feasible) {
        log_iteration(true, high_point_relaxation.optimizer().name(), status, high_point_relaxation.get_reason(), is_feasible);
        m_formulation->add_scenario_to_master(scenario, m_with_annotation_for_infeasible_scenario, m_check_for_repeated_scenarios);
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
        m_formulation->add_scenario_to_master(scenario, m_with_annotation_for_infeasible_scenario, m_check_for_repeated_scenarios);
        return 1;
    }

    if (status == Feasible) {
        ++m_index_joint_separation;
    }

    return 0;
}
*/

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::log_iteration_separator() {

    if (!get_param_logs()) {
        return;
    }

    center(std::cout, "-", 92, '-') << std::endl;

}
