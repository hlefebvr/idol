//
// Created by henri on 24/03/23.
//
#include "optimizers/column-generation/Optimizers_ColumnGeneration.h"
#include "optimizers/Logger.h"
#include "modeling/expressions/operations/operators.h"

Optimizers::ColumnGeneration::ColumnGeneration(const Model& t_model,
                                               Model *t_master_problem,
                                               const std::vector<Model *> &t_subproblems,
                                               std::vector<Column> t_generation_patterns)
     : Algorithm(t_model),
       m_master(t_master_problem) {

    const unsigned int n_subproblems = t_subproblems.size();

    m_subproblems.reserve(t_subproblems.capacity());

    for (unsigned int i = 0 ; i < n_subproblems ; ++i) {
        add_subproblem(t_subproblems[i], std::move(t_generation_patterns[i]));
    }


}

void Optimizers::ColumnGeneration::add(const Var &t_var) {
    m_master->add(t_var);
}

void Optimizers::ColumnGeneration::add(const Ctr &t_ctr) {
    m_master->add(t_ctr);
}

void Optimizers::ColumnGeneration::remove(const Var &t_var) {
    m_master->remove(t_var);
}

void Optimizers::ColumnGeneration::remove(const Ctr &t_ctr) {
    m_master->remove(t_ctr);
}

void Optimizers::ColumnGeneration::update() {
    m_master->update();
}

void Optimizers::ColumnGeneration::write(const std::string &t_name) {
    m_master->write(t_name);
}

void Optimizers::ColumnGeneration::hook_before_optimize() {

    set_best_bound(-Inf);
    set_best_obj(+Inf);
    m_iter_lower_bound = -Inf;
    m_iter_upper_bound = +Inf;
    m_iteration_count = 0;
    m_n_generated_columns_at_last_iteration = 0;
    m_n_generated_columns = 0;
    m_current_dual_solution.reset();
    m_adjusted_dual_solution.reset();
    m_current_iteration_is_farkas_pricing = false;

    if (m_farkas_pricing && !m_master->optimizer().infeasible_or_unbounded_info()) {
        m_master->optimizer().set_infeasible_or_unbounded_info(true);
    }

    for (auto& subproblem : m_subproblems) {
        subproblem.hook_before_optimize();
    }

    Algorithm::hook_before_optimize();
}

void Optimizers::ColumnGeneration::add_artificial_variables() {

    auto& env = m_master->env();
    const auto add_to = [&](const Ctr& t_ctr, double t_sign) {

        Column column(m_artificial_variables_cost);
        column.linear().set(t_ctr, t_sign);

        Var artifical_var(env, 0., Inf, Continuous, std::move(column), "_artificial_" + std::to_string(m_artificial_variables.size()));
        m_master->add(artifical_var);

        m_artificial_variables.emplace_back(artifical_var);

    };

    for (const auto& ctr : m_master->ctrs()) {

        const auto type = m_master->get_ctr_type(ctr);

        if (type == LessOrEqual) {
            add_to(ctr, -1);
        } else if (type == GreaterOrEqual) {
            add_to(ctr, 1);
        } else {
            add_to(ctr, -1);
            add_to(ctr, 1);
        }

    }

}

void Optimizers::ColumnGeneration::hook_optimize() {

    if (m_farkas_pricing) {
        run_column_generation();
        return;
    }

    add_artificial_variables();

    // min c^Tx + M^Ts
    if (m_artificial_variables_cost > 0) {

        run_column_generation();

        if (m_artificial_variables.empty()) {
            return;
        }

        if (get_status() == Infeasible) {
            remove_artificial_variables();
            terminate_for_master_infeasible_with_artificial_variables();
            return;

        }

    }

    // min e^Ts
    idol_Log(Debug, "Current status is " << get_status() << ", switching to pure phase I.")

    set_phase_I_objective_function();

    restart();
    hook_before_optimize();
    run_column_generation();

    remove_artificial_variables();

    if (get_status() != Optimal) {
        return;
    }

    restore_objective_function();

    restart();
    hook_before_optimize();
    run_column_generation();

}

void Optimizers::ColumnGeneration::set_phase_I_objective_function() {

    Expr objective;
    for (const auto& var : m_artificial_variables) {
        objective += var;
    }

    set_objective(std::move(objective));

}

void Optimizers::ColumnGeneration::restore_objective_function() {
    set_objective(parent().get_obj_expr());
}

bool Optimizers::ColumnGeneration::has_artificial_variable_in_basis() const {

    std::function<double(const Var&)> value;

    if (get_status() == Unbounded) {
        value = [this](const Var& t_var) { return m_master->get_var_ray(t_var); };
    } else {
        value = [this](const Var& t_var) { return m_master->get_var_primal(t_var); };
    }

    for (const auto& var : m_artificial_variables) {
        if (!equals(value(var), 0, 1e-8)) {
            return true;
        }
    }

    return false;

}

void Optimizers::ColumnGeneration::run_column_generation() {

    do {

        if (m_n_generated_columns_at_last_iteration > 0 || m_iteration_count == 0) {

            idol_Log(Trace, "Beginning to solve master problem.");
            solve_master_problem();
            idol_Log(Trace, "Master problem has been solved.");

            analyze_master_problem_solution();

            log_master_solution();

        } else if (m_current_iteration_is_farkas_pricing) {

            set_status(Infeasible);
            set_reason(Proved);
            terminate();

        }

        if (is_terminated() || stopping_condition()) { break; }

        update_subproblems();

        solve_subproblems();

        analyze_subproblems_solution();

        if (is_terminated() || stopping_condition()) { break; }

        clean_up();

        enrich_master_problem();

        ++m_iteration_count;

        if (m_iteration_count >= iteration_count_limit()) {
            set_reason(IterationCount);
            terminate();
            break;
        }

    } while (true);

    log_master_solution(true);

}

void Optimizers::ColumnGeneration::solve_master_problem() {
    m_master->optimize();
}

void Optimizers::ColumnGeneration::log_master_solution(bool t_force) const {

    if (!t_force && m_iteration_count % m_log_frequency != 0) {
        return;
    }

    auto status = m_master->get_status();

    std::stringstream objective_value;
    if (status == InfeasibleOrUnbounded || status == Fail || status == Unknown) {
        objective_value << "-";
    } else {
        objective_value << m_master->get_best_obj();
    }

    idol_Log(Info,
             "<Type=Master> "
             << "<Iter=" << m_iteration_count << "> "
             << "<TimT=" << time().count() << "> "
             << "<TimI=" << m_master->optimizer().time().count() << "> "
             << "<Stat=" << status << "> "
             << "<Reas=" << m_master->get_reason() << "> "
             << "<ObjVal=" << objective_value.str() << "> "
             << "<NGen=" << m_n_generated_columns_at_last_iteration << "> "
             << "<BestBnd=" << get_best_bound() << "> "
             << "<BestObj=" << get_best_obj() << "> "
             << "<RGap=" << get_relative_gap() * 100 << " %> "
             << "<AGap=" << get_absolute_gap() << "> "
    );

}

void Optimizers::ColumnGeneration::log_subproblem_solution(const Optimizers::ColumnGeneration::Subproblem &t_subproblem,
                                                           bool t_force) const {

    if (!t_force && m_iteration_count % m_log_frequency != 0) {
        return;
    }

    const auto& pricing = t_subproblem.m_model;

    idol_Log(Info,
             "<Type=Pricing_" << t_subproblem.m_index << "> "
             << "<Iter=" << m_iteration_count << "> "
             << "<TimT=" << time().count() << "> "
             << "<TimI=" << pricing->optimizer().time().count() << "> "
             << "<Stat=" << pricing->get_status() << "> "
             << "<Reas=" << pricing->get_reason() << "> "
             << "<Obj=" << std::setprecision(5)  << pricing->get_best_obj() << "> "
             << "<NGen=" << m_n_generated_columns_at_last_iteration << "> "
             << "<BestBnd=" << get_best_bound() << "> "
             << "<BestObj=" << get_best_obj() << "> "
             << "<RGap=" << get_relative_gap() * 100 << " %> "
             << "<AGap=" << get_absolute_gap() << "> "
    );

}

void Optimizers::ColumnGeneration::analyze_master_problem_solution() {

    auto status = m_master->get_status();

    set_status(status);

    if (status == Optimal) {

        m_iter_upper_bound = m_master->get_best_obj();

        set_best_obj(std::min(m_iter_upper_bound, get_best_obj()));

        m_current_dual_solution = save_dual(*m_master);

        if (m_current_iteration_is_farkas_pricing) {
            m_adjusted_dual_solution.reset();
        }

        if (!m_artificial_variables.empty() && !has_artificial_variable_in_basis()) {
            idol_Log(Trace, "Master problem is feasible, removing artificial variables.");
            remove_artificial_variables();
        }

        m_current_iteration_is_farkas_pricing = false;

        return;
    }

    if (status == Infeasible) {

        if (!m_farkas_pricing) {
            set_status(Infeasible);
            set_reason(Proved);
            terminate();
            return;
        }

        m_current_iteration_is_farkas_pricing = true;
        m_current_dual_solution = save_farkas(*m_master);
        m_adjusted_dual_solution.reset();

        return;
    }

    if (status == Unbounded) {

        set_reason(Proved);
        idol_Log(Trace, "Terminate. Unbounded master problem.");
        terminate();
        return;

    }

    set_status(status);
    set_reason(NotSpecified);
    idol_Log(Trace, "Terminate. Master problem could not be solved to optimality (status = " << status << ").");
    terminate();

}

void Optimizers::ColumnGeneration::update_subproblems() {

    if (!m_adjusted_dual_solution) {

        m_adjusted_dual_solution = m_current_dual_solution;

    } else {

        if (m_smoothing_factor == 0.) {
            m_adjusted_dual_solution = m_current_dual_solution.value();
        } else {
            m_adjusted_dual_solution = m_smoothing_factor * m_adjusted_dual_solution.value() + (1. - m_smoothing_factor) * m_current_dual_solution.value();
        }

    }

    for (auto& subproblem : m_subproblems) {
        subproblem.update_objective(m_current_iteration_is_farkas_pricing, m_adjusted_dual_solution.value());
    }

}

void Optimizers::ColumnGeneration::solve_subproblems() {

    const unsigned int n_threads = std::min(thread_limit(), m_parallel_pricing_limit);

#pragma omp parallel for num_threads(n_threads) default(none)
    for (auto& subproblem : m_subproblems) {

        if (subproblem.m_skip) {
            idol_Log(Trace, "Skipping subproblem " << subproblem.m_index << ".")
            continue;
        }

        idol_Log(Trace, "Beginning to solve subproblem " << subproblem.m_index << ".")
        subproblem.optimize();
        idol_Log(Trace, "Subproblem " << subproblem.m_index << " has been solved.")
    }

    for (auto& subproblem : m_subproblems) {

        if (subproblem.m_skip) {
            continue;
        }

        log_subproblem_solution(subproblem);

        if (is_terminated()) { break; }

    }

}

void Optimizers::ColumnGeneration::analyze_subproblems_solution() {

    double reduced_costs = 0;
    double min_reduced_costs = +Inf;
    double max_reduced_costs = -Inf;
    bool some_subproblems_had_been_skipped = false;

    for (auto& subproblem : m_subproblems) {

        if (subproblem.m_skip) {
            some_subproblems_had_been_skipped = true;
        }

        const auto status = subproblem.m_model->get_status();

        if (status == Optimal) {

            const double pricing_optimum = subproblem.m_model->get_best_obj();
            reduced_costs += std::min(0., pricing_optimum);

            if (max_reduced_costs < reduced_costs) {
                max_reduced_costs = reduced_costs;
            }

            if (min_reduced_costs > reduced_costs) {
                min_reduced_costs = reduced_costs;
            }

        } else {
            set_status(status);
            set_reason(Proved);
            terminate();
            return;
        }

    }

    if (max_reduced_costs > -1e-4 && min_reduced_costs < -1e-2) {

        for (auto &subproblem: m_subproblems) {

            if (subproblem.m_model->get_best_obj() > -1e-4) {
                subproblem.m_skip = true;
            } else {
                subproblem.m_skip = false;
            }

        }

    } else {

        for (auto &subproblem: m_subproblems) {
            subproblem.m_skip = false;
        }

    }

    if (some_subproblems_had_been_skipped) {
        return;
    }

    if (!m_current_iteration_is_farkas_pricing) {

        const double lower_bound = m_master->get_best_obj() + reduced_costs;

        m_iter_lower_bound = lower_bound;

        if (get_best_bound() <= lower_bound) {
            set_best_bound(lower_bound);
        }

        if (get_best_bound() > get_best_obj() + 1e-3) {
            idol_Log(Fatal, "Terminate. Best bound is strictly greater than best obj.");
            set_status(Fail);
            set_reason(NotSpecified);
            terminate();
            return;
        }

        if (m_artificial_variables.empty() && get_best_bound() > best_bound_stop()) {
            set_reason(SolutionReason::UserObjLimit);
            terminate();
            idol_Log(Trace,
                     "Terminate. Given best_bound_stop is " << best_bound_stop()
                    << " while current best bound is " << get_best_bound()
            )
        }

    }

}

void Optimizers::ColumnGeneration::enrich_master_problem() {

    m_n_generated_columns_at_last_iteration = 0;

    for (auto& subproblem : m_subproblems) {

        bool can_enrich_master;

        // TODO add rules for adding columns

        if (m_current_iteration_is_farkas_pricing) {
            can_enrich_master = subproblem.m_model->get_best_obj() < -1e-4;
        } else {
            can_enrich_master = subproblem.compute_reduced_cost(m_current_dual_solution.value()) < 0;
        }

        if (can_enrich_master) {
            subproblem.enrich_master_problem();
        }

    }

}

void Optimizers::ColumnGeneration::clean_up() {

    for (auto& subproblem : m_subproblems) {
        subproblem.clean_up();
    }

}

void Optimizers::ColumnGeneration::remove_artificial_variables() {

    for (const Var& var : m_artificial_variables) {
        m_master->remove(var);
    }

    m_artificial_variables.clear();
}

bool Optimizers::ColumnGeneration::stopping_condition() const {
    return get_absolute_gap() <= ToleranceForAbsoluteGapPricing
           || get_relative_gap() <= ToleranceForRelativeGapPricing
           || remaining_time() == 0;
}

Optimizers::ColumnGeneration::Subproblem& Optimizers::ColumnGeneration::add_subproblem(Model *t_sub_problem_model, Column t_generation_pattern) {
    m_subproblems.emplace_back(*this, m_subproblems.size(), t_sub_problem_model, std::move(t_generation_pattern));
    return m_subproblems.back();
}

void Optimizers::ColumnGeneration::terminate_for_master_infeasible_with_artificial_variables() {

    idol_Log(Fatal, "Master problem should not be infeasible when using artificial variables.");
    set_status(Fail);
    set_reason(NotSpecified);
    terminate();

}

double Optimizers::ColumnGeneration::get_var_primal(const Var &t_var) const {
    return m_master->get_var_primal(t_var);
}

double Optimizers::ColumnGeneration::get_var_ray(const Var &t_var) const {
    return m_master->get_var_ray(t_var);
}

double Optimizers::ColumnGeneration::get_ctr_dual(const Ctr &t_ctr) const {
    return m_master->get_ctr_dual(t_ctr);
}

double Optimizers::ColumnGeneration::get_ctr_farkas(const Ctr &t_ctr) const {
    return m_master->get_ctr_farkas(t_ctr);
}

void Optimizers::ColumnGeneration::set_objective(Expr<Var, Var> &&t_objective) {
    m_master->set_obj_expr(std::move(t_objective));
}

void Optimizers::ColumnGeneration::set_objective(const Expr<Var, Var> &t_objective) {
    set_objective(Expr<Var, Var>(t_objective));
}

void Optimizers::ColumnGeneration::update_obj_sense() {
    throw Exception("Not implemented");
}

void Optimizers::ColumnGeneration::update_obj() {
    set_objective(parent().get_obj_expr());
}

void Optimizers::ColumnGeneration::update_rhs() {
    m_master->set_rhs_expr(parent().get_rhs_expr());
}

void Optimizers::ColumnGeneration::update_obj_constant() {
    m_master->set_obj_const(parent().get_obj_expr().constant());
}

void Optimizers::ColumnGeneration::update_mat_coeff(const Ctr &t_ctr, const Var &t_var) {
    m_master->set_mat_coeff(t_ctr, t_var, parent().get_mat_coeff(t_ctr, t_var));
}

void Optimizers::ColumnGeneration::update_ctr_type(const Ctr &t_ctr) {
    m_master->set_ctr_type(t_ctr, parent().get_ctr_type(t_ctr));
}

void Optimizers::ColumnGeneration::update_ctr_rhs(const Ctr &t_ctr) {
    m_master->set_ctr_rhs(t_ctr, parent().get_ctr_row(t_ctr).rhs());
}

void Optimizers::ColumnGeneration::update_var_type(const Var &t_var) {
    m_master->set_var_type(t_var, parent().get_var_type(t_var));
}

void Optimizers::ColumnGeneration::update_var_lb(const Var &t_var) {
    m_master->set_var_lb(t_var, parent().get_var_lb(t_var));
}

void Optimizers::ColumnGeneration::update_var_ub(const Var &t_var) {
    m_master->set_var_ub(t_var, parent().get_var_ub(t_var));
}

void Optimizers::ColumnGeneration::update_var_obj(const Var &t_var) {
    m_master->set_var_obj(t_var, parent().get_var_column(t_var).obj());
}

unsigned int Optimizers::ColumnGeneration::get_n_solutions() const {
    return 1;
}

unsigned int Optimizers::ColumnGeneration::get_solution_index() const {
    return 0;
}

void Optimizers::ColumnGeneration::set_solution_index(unsigned int t_index) {
    if (t_index != 0) {
        throw Exception("Solution index out of bounds.");
    }
}

void Optimizers::ColumnGeneration::Subproblem::hook_before_optimize() {
    m_skip = false;
}

Optimizers::ColumnGeneration::Subproblem::Subproblem(Optimizers::ColumnGeneration &t_parent,
                                                     unsigned int t_index,
                                                     Model* t_model,
                                                     Column&& t_generation_pattern)
    : m_parent(t_parent),
      m_index(t_index),
      m_model(t_model),
      m_generation_pattern(std::move(t_generation_pattern))

    {

}

void Optimizers::ColumnGeneration::Subproblem::update_objective(bool t_farkas_pricing, const Solution::Dual &t_duals) {

    ::Expr<Var, Var> objective;

    for (const auto &[ctr, constant] : m_generation_pattern.linear()) {
        objective += constant.numerical() * -t_duals.get(ctr);
        for (const auto &[param, coeff]: constant) {
            objective += -t_duals.get(ctr) * coeff * param.as<Var>();
        }
    }

    if (!t_farkas_pricing) {
        for (const auto &[param, coeff] : m_generation_pattern.obj()) {
            objective += coeff * param.as<Var>();
        }
    }

    m_model->set_obj_expr(std::move(objective));

}

void Optimizers::ColumnGeneration::Subproblem::optimize() {

    const double remaining_time = m_parent.remaining_time();
    m_model->optimizer().set_time_limit(remaining_time);
    m_model->optimize();

}

double Optimizers::ColumnGeneration::Subproblem::compute_reduced_cost(const Solution::Dual &t_duals) const {

    double result = 0.;

    const auto& primals = save_primal(*m_model);

    for (const auto &[ctr, constant] : m_generation_pattern.linear()) {
        result += constant.numerical() * -t_duals.get(ctr);
        for (const auto &[param, coeff]: constant) {
            result += -t_duals.get(ctr) * coeff * primals.get(param.as<Var>());
        }
    }

    for (const auto &[param, coeff] : m_generation_pattern.obj()) {
        result += coeff * primals.get(param.as<Var>());
    }

    return result;
}

void Optimizers::ColumnGeneration::Subproblem::enrich_master_problem() {

    auto& env = m_model->env();
    const unsigned int n_solutions = m_model->get_n_solutions();
    const unsigned int max_columns_added = m_parent.m_max_columns_per_pricing;

    for (unsigned int k = 0 ; k < n_solutions && k < max_columns_added ; ++k) {

        m_model->set_solution_index(k);

        const int status = m_model->get_status();

        Solution::Primal generator;
        if (status == Unbounded) {
            generator = save_ray(*m_model);
        } else {
            generator = save_primal(*m_model);
        }

        auto column = create_column_from_generator(generator);

        Var alpha(env,
                  0.,
                  Inf,
                  Continuous,
                  "_alpha_" + std::to_string(m_index) + "_" + std::to_string(m_pool.size()));

        auto &master = *m_parent.m_master;

        master.add(alpha, std::move(column));
        m_pool.add(alpha, std::move(generator));
        m_present_generators.emplace_back(alpha, m_pool.last_inserted());

        ++m_parent.m_n_generated_columns_at_last_iteration;
        ++m_parent.m_n_generated_columns;
    }
}

TempVar
Optimizers::ColumnGeneration::Subproblem::create_column_from_generator(const Solution::Primal &t_primals) const {
    return {
            0.,
            Inf,
            Continuous,
            m_generation_pattern.fix(t_primals)
    };

}

void Optimizers::ColumnGeneration::Subproblem::clean_up() {

    if (m_pool.size() < m_parent.m_clean_up_threshold) {
        return;
    }

    auto& master = *m_parent.m_master;
    const double ratio = m_parent.m_clean_up_ratio;
    const auto n_to_remove = (unsigned int) (m_pool.size() * (1 - ratio));
    unsigned int n_removed = 0;

    m_present_generators.clear();

    for (auto it = m_pool.values().begin(), end = m_pool.values().end() ; it != end ; ) {

        const bool is_already_in_master = master.has(it->first);
        const bool done_removing = n_removed >= n_to_remove;

        if (done_removing) {

            if (is_already_in_master) {
                m_present_generators.emplace_back(it->first, it->second);
            }

            ++it;
            continue;

        }

        if (is_already_in_master) {

            if (master.get_var_primal(it->first) > 0) {

                m_present_generators.emplace_back(it->first, it->second);
                ++it;
                continue;

            }

            master.remove(it->first);

        }

        it = m_pool.erase(it);
        ++n_removed;

    }


}

void Optimizers::ColumnGeneration::Subproblem::remove_column_if(const std::function<bool(const Var &, const Solution::Primal &)> &t_indicator_for_removal) {

    auto& master = m_parent.m_master;

    auto it = m_present_generators.begin();
    const auto end = m_present_generators.end();

    while (it != end) {
        const auto& [column_variable, ptr_to_column] = *it;
        if (t_indicator_for_removal(column_variable, ptr_to_column)) {
            master->remove(column_variable);
            it = m_present_generators.erase(it);
        } else {
            ++it;
        }
    }

}

void Optimizers::ColumnGeneration::Subproblem::update_generation_pattern_objective(Constant &&t_objective) {

    for (const auto& [var, generator] : m_present_generators) {
        m_parent.m_master->set_var_obj(var, t_objective.fix(generator));
    }

    m_generation_pattern.set_obj(std::move(t_objective));

}
