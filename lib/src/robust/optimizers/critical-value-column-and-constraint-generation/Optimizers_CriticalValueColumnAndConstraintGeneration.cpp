//
// Created by Henri on 17/04/2026.
//
#include "idol/robust/optimizers/critical-value-column-and-constraint-generation/Optimizers_CriticalValueColumnAndConstraintGeneration.h"

#include "idol/general/optimizers/logs.h"

idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::CriticalValueColumnAndConstraintGeneration(const Model& t_model, const idol::Robust::Description& t_description, const OptimizerFactory& t_master_optimizer, const OptimizerFactory& t_deterministic_optimizer, bool t_use_indicator) :
    Algorithm(t_model),
    m_description(t_description),
    m_master_optimizer_factory(t_master_optimizer.clone()),
    m_deterministic_optimizer_factory(t_deterministic_optimizer.clone()),
    m_use_indicator(t_use_indicator)
{

}

void idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::hook_optimize() {

    m_formulation = std::make_unique<CVCCG::Formulation>(*this);

    log_banner();

    do {

        solve_master_problem();

        analyze_master_problem();

        solve_sub_problems();

        m_n_iterations++;

        log_iteration();

        check_termination_criterion();

        if (is_terminated()) { break; }

    } while (true);
}

void idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::solve_master_problem() {
    m_formulation->master().optimize();
}

void idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::analyze_master_problem() {

    const auto& master = m_formulation->master();
    const auto status = master.get_status();
    const auto reason = master.get_reason();

    if (status != Optimal) {
        set_status(status);
        set_reason(reason);
        terminate();
        return;
    }

    const double objective_value = master.get_best_obj();

    set_best_bound(objective_value);

}

void idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::check_termination_criterion() {

    if (is_terminated()) {
        return;
    }

    if (get_best_bound() > get_best_obj() + get_tol_mip_absolute_gap()) {
        std::cerr << "The current best bound is larger than current best obj. This should should not happen. Terminating..." << std::endl;
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

void idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::solve_sub_problems() {

    const auto& master = m_formulation->master();
    const auto& master_solution = save_primal(master);

    m_formulation->update_sub_problem_constraints(master_solution);

    std::list<PrimalPoint> scenarios;
    for (const auto& uncertainty : m_formulation->uncertainties()) {

        m_formulation->update_sub_problem_objective(master_solution, uncertainty);

        auto& sub_problem = m_formulation->sub_problem();
        sub_problem.optimize();

        auto scenario = save_primal(sub_problem);

        if (scenario.status() != Optimal) {
            set_status(scenario.status());
            set_reason(scenario.reason());
            terminate();
            return;
        }

        if (-scenario.objective_value() > get_tol_feasibility()) {
            scenarios.emplace_back(std::move(scenario));
        }

    }

    if (scenarios.empty()) {
        set_status(Optimal);
        set_reason(Proved);
        set_best_obj(get_best_bound());
        terminate();
        return;
    }

    assert(scenarios.size() == 1); // TODO: should apply unique() to avoid adding twice the same scenario

    for (auto&& scenario : scenarios) {
        const auto it = m_formulation->add_scenario_to_pool(std::move(scenario), PrimalPoint(master_solution));
        m_formulation->add_scenario_to_master(it);
    }

}

void idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::log_banner() {

    if (!get_param_logs()) {
        return;
    }

    center(std::cout, "", 100, '*') << std::endl;
    center(std::cout, " Critical-Value Column-and-constraint Generation ", 100) << std::endl;
    center(std::cout, "", 100, '*') << std::endl;

    std::cout << std::setw(12) << "Time (s)" << "\t"
              << std::setw(5) << "# Iter." << "\t"
              << std::setw(5) << "# Scen." << "\t"
              << std::setw(5) << "# CV" << "\t"
              << std::setw(12) << "LB" << "\t"
              << std::setw(12) << "UB" << "\t"
              << std::setw(12) << "Gap (%)" << "\t"
              << std::setw(12) << "Abs. Gap"
              << std::endl;
    log_iteration_separator();
}

void idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::log_iteration_separator() {

    if (!get_param_logs()) {
        return;
    }

    center(std::cout, "-", 100, '-') << std::endl;

}

void idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::log_iteration() {

    if (!get_param_logs()) {
        return;
    }

    std::cout << std::setw(12) << pretty_double(time().count(), 5) << "s\t"
              << std::setw(5) << m_n_iterations << "\t"
              << std::setw(5) << m_formulation->n_scenarios_in_pool() << "\t"
              << std::setw(5) << m_formulation->n_critical_values() << "\t"
              << std::setw(12) << pretty_double(get_best_bound(), 5) << "\t"
              << std::setw(12) << pretty_double(get_best_obj(), 5) << "\t"
              << std::setw(12) << pretty_double(get_relative_gap() * 100, 5) << "\t"
              << std::setw(12) << pretty_double(get_absolute_gap(), 5)
              << std::endl;

}

void idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::write(const std::string& t_name) {
    if (!m_formulation) {
        m_formulation = std::make_unique<CVCCG::Formulation>(*this);
    }
    m_formulation->master().write(t_name);
}

void idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::hook_before_optimize() {
    Algorithm::hook_before_optimize();

    m_n_iterations = 0;
    set_best_obj(Inf);
    set_best_bound(-Inf);

}

void idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::set_solution_index(unsigned t_index) {
    if (t_index == 0) {
        return;
    }
    throw Exception("Solution index out of bounds.");
}

double idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::get_var_primal(const Var& t_var) const {
    if (parent().has(t_var)) {
        return m_formulation->master().get_var_primal(t_var);
    }
    return m_formulation->get_scenario_var_primal(t_var);
}

double idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::get_var_reduced_cost(const Var& t_var) const {
    throw Exception("Not implemented get_var_reduced_cost");
}

double idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::get_var_ray(const Var& t_var) const {
    throw Exception("Not implemented get_var_ray");
}

double idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::get_ctr_dual(const Ctr& t_ctr) const {
    throw Exception("Not implemented get_ctr_dual");
}

double idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::get_ctr_farkas(const Ctr& t_ctr) const {
    throw Exception("Not implemented get_ctr_farkas");
}

unsigned idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::get_n_solutions() const {
    if (const auto status = get_solution_index() ; status == Feasible || status == Optimal) {
        return 1;
    }
    return 0;
}

unsigned idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::get_solution_index() const {
    return 0;
}
