//
// Created by henri on 31.10.23.
//
#include "idol/optimizers/dantzig-wolfe/ColumnGeneration.h"

idol::Optimizers::DantzigWolfeDecomposition::ColumnGeneration::ColumnGeneration(DantzigWolfeDecomposition &t_parent,
                                                                                bool t_use_farkas_for_infeasibility,
                                                                                double t_best_bound_stop)
        : m_parent(t_parent),
          m_use_farkas_for_infeasibility(t_use_farkas_for_infeasibility),
          m_best_bound_stop(t_best_bound_stop)
{

}

void idol::Optimizers::DantzigWolfeDecomposition::ColumnGeneration::execute() {

    m_status = Loaded;
    m_reason = NotSpecified;
    m_last_master_solution.reset();
    m_iteration_count = 0;
    m_n_generated_columns = 0;
    m_solve_dual_master = true;
    m_best_bound = -Inf;
    m_best_obj = +Inf;
    m_is_terminated = false;
    m_current_iteration_is_using_farkas = false;
    initialize_sub_problem_phases();
    m_parent.m_stabilization->initialize();

    //if (m_use_farkas_for_infeasibility) {
        m_parent.m_formulation.master().optimizer().set_param_infeasible_or_unbounded_info(true);
    //}

    log_init();

    while (true) {

        if (m_solve_dual_master) { solve_dual_master(); }

        log_master();

        ++m_iteration_count;

        if (check_stopping_criterion()) { break; }

        update_sub_problems();

        solve_sub_problems_in_parallel();

        analyze_sub_problems();

        log_sub_problems();

        if (check_stopping_criterion()) { break; }

        enrich_master();

        pool_clean_up();

    }

    if (m_status == Feasible || m_status == Optimal) {
        m_master_primal_solution = save_primal(m_parent.m_formulation.master());
    }

    log_master();

    log_end();

}

void idol::Optimizers::DantzigWolfeDecomposition::ColumnGeneration::solve_dual_master() {

    auto& master = m_parent.m_formulation.master();

    master.optimize();

    const auto status = master.get_status();

    m_current_iteration_is_using_farkas = false;

    if (status == Optimal || status == Feasible) {

        m_status = Feasible;

        const double iter_upper_bound = master.get_best_obj();

        m_best_obj = std::min(m_best_obj, iter_upper_bound);

        m_last_master_solution = save_dual(master);

        return;
    }

    m_status = status;
    m_reason = master.get_reason();

    if (m_status == Infeasible && m_use_farkas_for_infeasibility) {

        m_current_iteration_is_using_farkas = true;

        m_last_master_solution = save_farkas(master);

        return;
    }

    m_last_master_solution.reset();

    m_is_terminated = true;

}

void idol::Optimizers::DantzigWolfeDecomposition::ColumnGeneration::update_sub_problems() {

    auto& formulation = m_parent.m_formulation;
    auto dual_values = m_current_iteration_is_using_farkas ? m_last_master_solution.value() : m_parent.m_stabilization->compute_smoothed_dual_solution(m_last_master_solution.value());

    for (unsigned int i = 0, n = formulation.n_sub_problems() ; i < n ; ++i) {
        formulation.update_sub_problem_objective(i, dual_values, m_current_iteration_is_using_farkas);
    }

}

void idol::Optimizers::DantzigWolfeDecomposition::ColumnGeneration::solve_sub_problems_in_parallel() {

    const unsigned int n_threads = std::min(m_parent.get_param_threads(), m_parent.m_max_parallel_pricing);

    #pragma omp parallel for num_threads(n_threads) default(none)
    for (auto& sub_problem : m_parent.m_formulation.sub_problems()) {
        sub_problem.optimizer().set_param_time_limit(m_parent.get_remaining_time());
        sub_problem.optimize();
    }

}

void idol::Optimizers::DantzigWolfeDecomposition::ColumnGeneration::analyze_sub_problems() {

    bool all_sub_problems_have_optimal_status = true;
    double sum_reduced_costs = 0.;

    auto& formulation = m_parent.m_formulation;

    for (unsigned int i = 0, n = formulation.n_sub_problems() ; i < n ; ++i) {

        const auto& model = formulation.sub_problem(i);
        const auto status = model.get_status();
        const auto reason = model.get_reason();

        if (status == Optimal) {
            const double upper_multiplicity = m_parent.m_sub_problem_specifications[i].upper_multiplicity();
            const double objective_value = model.get_best_obj();
            sum_reduced_costs += upper_multiplicity * std::min(0., objective_value);
            continue;
        }

        all_sub_problems_have_optimal_status = false;

        if (status == Feasible) {
            continue;
        }

        if (status == Unbounded) {
            continue;
        }

        m_status = status;
        m_reason = reason;
        m_is_terminated = true;
        return;

    }

    if (!all_sub_problems_have_optimal_status || m_current_iteration_is_using_farkas) {
        return;
    }

    const double iter_lower_bound = m_last_master_solution->objective_value() + sum_reduced_costs;

    if (m_best_bound <= iter_lower_bound) {

        m_parent.m_stabilization->update_stability_center(m_last_master_solution.value());
        m_best_bound = std::min(m_best_obj, iter_lower_bound);

    }

}

bool idol::Optimizers::DantzigWolfeDecomposition::ColumnGeneration::check_stopping_criterion() {

    if (m_is_terminated) {
        return true;
    }

    if (gap_is_closed()) {
        m_status = Optimal;
        m_reason = Proved;
        return true;
    }

    if (m_parent.get_remaining_time() <= 0) {
        m_reason = TimeLimit;
        return true;
    }

    if (m_iteration_count >= m_parent.get_param_iteration_limit()) {
        m_reason = IterLimit;
        return true;
    }

    if (m_best_bound > m_best_bound_stop) {
        m_reason = ObjLimit;
        return true;
    }

    return false;
}

bool idol::Optimizers::DantzigWolfeDecomposition::ColumnGeneration::gap_is_closed() const {
    return relative_gap(m_best_bound, m_best_obj) < m_parent.get_tol_mip_relative_gap()
           || absolute_gap(m_best_bound, m_best_obj) < m_parent.get_tol_mip_absolute_gap();
}

void idol::Optimizers::DantzigWolfeDecomposition::ColumnGeneration::enrich_master() {

    auto& formulation = m_parent.m_formulation;

    bool at_least_one_column_have_been_generated = false;

    for (unsigned int i = 0, n = formulation.n_sub_problems() ; i < n ; ++i) {

        auto& model = formulation.sub_problem(i);

        if (m_current_iteration_is_using_farkas) {

            if (model.get_best_obj() < -Tolerance::Feasibility) {

                auto generator = save_primal(model);
                formulation.generate_column(i, std::move(generator));
                at_least_one_column_have_been_generated = true;
                ++m_n_generated_columns;
                continue;

            }

        }

        const auto n_solutions = model.get_n_solutions();

        for (unsigned int k = 0 ; k < n_solutions && k < m_parent.m_sub_problem_specifications[i].max_column_per_pricing() ; ++k) {

            model.set_solution_index(k);

            auto generator = save_primal(model);

            const double reduced_cost = formulation.compute_reduced_cost(i, m_last_master_solution.value(), generator);

            if (reduced_cost < -Tolerance::Optimality) {
                formulation.generate_column(i, std::move(generator));
                at_least_one_column_have_been_generated = true;
                ++m_n_generated_columns;
            }

        }

    }

    if (m_current_iteration_is_using_farkas && !at_least_one_column_have_been_generated) {
        m_status = Infeasible;
        m_reason = Proved;
        m_is_terminated = true;
        return;
    }

    m_solve_dual_master = at_least_one_column_have_been_generated;

}

void idol::Optimizers::DantzigWolfeDecomposition::ColumnGeneration::initialize_sub_problem_phases() {

    // TODO handle phase restart here

    auto& formulation = m_parent.m_formulation;
    unsigned int n_sub_problems = formulation.n_sub_problems();

    m_sub_problems_phases.clear();
    m_sub_problems_phases.reserve(n_sub_problems);
    for (unsigned int i = 0 ; i < n_sub_problems ; ++i) {
        const auto phase = m_parent.m_sub_problem_specifications[i].phases().begin();
        if (formulation.sub_problem(i).has_optimizer()) { continue; }
        formulation.sub_problem(i).use(**phase);
        m_sub_problems_phases.emplace_back(phase);
    }

}

void idol::Optimizers::DantzigWolfeDecomposition::ColumnGeneration::pool_clean_up() {

    auto& formulation = m_parent.m_formulation;
    unsigned int n_sub_problems = formulation.n_sub_problems();
    for (unsigned int i = 0 ; i < n_sub_problems ; ++i) {
        const auto& sub_problem_specifications = m_parent.m_sub_problem_specifications[i];
        const auto& column_pool = formulation.column_pool(i);
        const unsigned int threshold = sub_problem_specifications.column_pool_clean_up_threshold();
        if (column_pool.size() >= threshold) {
            const double ratio = sub_problem_specifications.column_pool_clean_up_ratio();
            formulation.clean_up(i, ratio);
        }
    }

}

void idol::Optimizers::DantzigWolfeDecomposition::ColumnGeneration::log_init() {

    if (m_parent.log_level() == Mute || !m_parent.m_logger) {
        return;
    }

    m_parent.m_logger->log_init(
            m_parent.m_formulation.n_sub_problems()
    );
}

void idol::Optimizers::DantzigWolfeDecomposition::ColumnGeneration::log_master() {

    if (m_parent.log_level() == Mute || !m_parent.m_logger || !m_last_master_solution.has_value()) {
        return;
    }

    const auto& formulation = m_parent.m_formulation;

    m_parent.m_logger->log_master(
                m_iteration_count,
                m_parent.time().count(),
                m_status,
                m_last_master_solution->status(),
                m_last_master_solution->reason(),
                m_last_master_solution->objective_value(),
                formulation.master().optimizer().time().count(),
                m_best_bound,
                m_best_obj,
                m_n_generated_columns,
                formulation.get_n_present_generators()
            );

}

void idol::Optimizers::DantzigWolfeDecomposition::ColumnGeneration::log_sub_problems() {

    if (m_parent.log_level() == Mute || !m_parent.m_logger) {
        return;
    }

    const auto& formulation = m_parent.m_formulation;

    for (unsigned int i = 0, n = formulation.n_sub_problems() ; i < n ; ++i) {
        const auto& sub_problem = formulation.sub_problem(i);
        m_parent.m_logger->log_sub_problem(
            m_iteration_count,
            m_parent.time().count(),
            i,
            m_status,
            sub_problem.get_status(),
            sub_problem.get_reason(),
            sub_problem.get_best_obj(),
            sub_problem.optimizer().time().count(),
            m_best_bound,
            m_best_obj,
            m_n_generated_columns,
            formulation.get_n_present_generators()
        );
    }

}

void idol::Optimizers::DantzigWolfeDecomposition::ColumnGeneration::log_end() {

    if (m_parent.log_level() == Mute || !m_parent.m_logger) {
        return;
    }

    m_parent.m_logger->log_end();
}
