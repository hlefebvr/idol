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

        const auto type = m_master->get(Attr::Ctr::Type, ctr);

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

        if (status() == Infeasible) {
            remove_artificial_variables();
            terminate_for_master_infeasible_with_artificial_variables();
            return;

        }

    }

    // min e^Ts
    idol_Log(Debug, "Current status is " << (SolutionStatus) status() << ", switching to pure phase I.")

    set_phase_I_objective_function();

    restart();
    hook_before_optimize();
    run_column_generation();

    remove_artificial_variables();

    if (status() != Optimal) {
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

    set(Attr::Obj::Expr, std::move(objective));

}

void Optimizers::ColumnGeneration::restore_objective_function() {
    set(Attr::Obj::Expr, parent().get(Attr::Obj::Expr));
}

bool Optimizers::ColumnGeneration::has_artificial_variable_in_basis() const {

    std::function<double(const Var&)> value;

    if (status() == Unbounded) {
        value = [this](const Var& t_var) { return m_master->get(Attr::Solution::Ray, t_var); };
    } else {
        value = [this](const Var& t_var) { return m_master->get(Attr::Solution::Primal, t_var); };
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

    auto status = (SolutionStatus) m_master->get(Attr::Solution::Status);

    std::stringstream objective_value;
    if (status == InfeasibleOrUnbounded || status == Fail || status == Unknown) {
        objective_value << "-";
    } else {
        objective_value << m_master->get(Attr::Solution::ObjVal);
    }

    idol_Log(Info,
             "<Type=Master> "
             << "<Iter=" << m_iteration_count << "> "
             << "<TimT=" << time().count() << "> "
             << "<TimI=" << m_master->optimizer().time().count() << "> "
             << "<Stat=" << status << "> "
             << "<Reas=" << (SolutionReason) m_master->get(Attr::Solution::Reason) << "> "
             << "<ObjVal=" << objective_value.str() << "> "
             << "<NGen=" << m_n_generated_columns_at_last_iteration << "> "
             << "<BestBnd=" << best_bound() << "> "
             << "<BestObj=" << best_obj() << "> "
             << "<RGap=" << get(Attr::Solution::RelGap) * 100 << "> "
             << "<AGap=" << get(Attr::Solution::AbsGap) << "> "
    );

}

void Optimizers::ColumnGeneration::log_subproblem_solution(const Optimizers::ColumnGeneration::Subproblem &t_subproblem,
                                                           bool t_force) const {

    if (!t_force && m_iteration_count % m_log_frequency != 0) {
        return;
    }

    const auto& pricing = t_subproblem.m_model;

    idol_Log(Info,
             "<Type=Pricing> "
             << "<Iter=" << m_iteration_count << "> "
             << "<TimT=" << time().count() << "> "
             << "<TimI=" << pricing->optimizer().time().count() << "> "
             << "<Stat=" << (SolutionStatus) pricing->get(Attr::Solution::Status) << "> "
             << "<Reas=" << (SolutionReason) pricing->get(Attr::Solution::Reason) << "> "
             << "<Obj=" << std::setprecision(5)  << pricing->get(Attr::Solution::ObjVal) << "> "
             << "<NGen=" << m_n_generated_columns_at_last_iteration << "> "
             << "<BestBnd=" << best_bound() << "> "
             << "<BestObj=" << best_obj() << "> "
             << "<RGap=" << get(Attr::Solution::RelGap) * 100 << "> "
             << "<AGap=" << get(Attr::Solution::AbsGap) << "> "
    );

}

void Optimizers::ColumnGeneration::analyze_master_problem_solution() {

    auto status = m_master->get(Attr::Solution::Status);

    set_status(status);

    if (status == Optimal) {

        m_iter_upper_bound = m_master->get(Attr::Solution::ObjVal);

        set_best_obj(std::min(m_iter_upper_bound, best_obj()));

        m_current_dual_solution = save(*m_master, Attr::Solution::Dual);

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
        m_current_dual_solution = save(*m_master, Attr::Solution::Farkas);
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
    idol_Log(Trace, "Terminate. Master problem could not be solved to optimality (status = " << (SolutionStatus) status << ").");
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
    for (auto & subproblem : m_subproblems) {
        idol_Log(Trace, "Beginning to solve subproblem " << subproblem.m_index << ".")
        subproblem.optimize();
        idol_Log(Trace, "Subproblem " << subproblem.m_index << " has been solved.")
    }

    for (auto& subproblem : m_subproblems) {

        log_subproblem_solution(subproblem);

        if (is_terminated()) { break; }

    }
}

void Optimizers::ColumnGeneration::analyze_subproblems_solution() {

    double reduced_costs = 0;

    for (auto& subproblem : m_subproblems) {

        const auto status = subproblem.m_model->get(Attr::Solution::Status);

        if (status == Optimal) {

            const double pricing_optimum = subproblem.m_model->get(Attr::Solution::ObjVal);
            reduced_costs += std::min(0., pricing_optimum);

        } else {
            set_status(status);
            set_reason(Proved);
            terminate();
            return;
        }

    }

    if (!m_current_iteration_is_farkas_pricing) {

        const double lower_bound = m_master->get(Attr::Solution::ObjVal) + reduced_costs;

        m_iter_lower_bound = lower_bound;

        if (best_bound() < lower_bound) {
            set_best_bound(lower_bound);
        }

        if (best_bound() > best_obj() + 1e-3) {
            idol_Log(Fatal, "Terminate. Best bound is strictly greater than best obj.");
            set_status(Fail);
            set_reason(NotSpecified);
            terminate();
            return;
        }

        if (m_artificial_variables.empty() && best_bound() > best_bound_stop()) {
            set_reason(SolutionReason::UserObjLimit);
            terminate();
            idol_Log(Trace,
                     "Terminate. Given best_bound_stop is " << best_bound_stop()
                    << " while current best bound is " << best_bound()
            )
        }

    }

}

void Optimizers::ColumnGeneration::enrich_master_problem() {

    m_n_generated_columns_at_last_iteration = 0;

    for (auto& subproblem : m_subproblems) {

        bool can_enrich_master;

        if (m_current_iteration_is_farkas_pricing) {
            can_enrich_master = subproblem.m_model->get(Attr::Solution::ObjVal) < -1e-6;
        } else {
            can_enrich_master = subproblem.compute_reduced_cost(m_current_dual_solution.value()) < 0;
        }

        if (can_enrich_master) {
            subproblem.enrich_master_problem();
            ++m_n_generated_columns_at_last_iteration;
            ++m_n_generated_columns;
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
    return get(Attr::Solution::AbsGap) <= ToleranceForAbsoluteGapPricing
           || get(Attr::Solution::RelGap) <= ToleranceForRelativeGapPricing
           || remaining_time() == 0;
}

double Optimizers::ColumnGeneration::get(const Req<double, Var> &t_attr, const Var &t_var) const {
    return m_master->get(t_attr, t_var);
}

int Optimizers::ColumnGeneration::get(const Req<int, Var> &t_attr, const Var &t_var) const {
    return m_master->get(t_attr, t_var);;
}

void Optimizers::ColumnGeneration::set(const Req<double, Var> &t_attr, const Var &t_var, double t_value) {
    m_master->set(t_attr, t_var, t_value);
}

void Optimizers::ColumnGeneration::set(const Req<int, Var> &t_attr, const Var &t_var, int t_value) {
    m_master->set(t_attr, t_var, t_value);
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

void Optimizers::ColumnGeneration::Subproblem::hook_before_optimize() {}

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

    m_model->set(::Attr::Obj::Expr, std::move(objective));

}

void Optimizers::ColumnGeneration::Subproblem::optimize() {

    const double remaining_time = m_parent.remaining_time();
    m_model->optimizer().set_time_limit(remaining_time);
    m_model->optimize();

}

double Optimizers::ColumnGeneration::Subproblem::compute_reduced_cost(const Solution::Dual &t_duals) const {

    double result = 0.;

    const auto& primals = save(*m_model, ::Attr::Solution::Primal);

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

    const int status = m_model->get(::Attr::Solution::Status);

    Solution::Primal generator;
    if (status == Unbounded) {
        generator = save(*m_model, ::Attr::Solution::Ray);
    } else {
        generator = save(*m_model, ::Attr::Solution::Primal);
    }

    auto column = create_column_from_generator(generator);

    Var alpha(m_model->env(), 0., Inf, Continuous, "_alpha_" + std::to_string(m_index) + "_" + std::to_string(m_pool.size()));

    auto& master = *m_parent.m_master;

    master.add(alpha, std::move(column));
    m_pool.add(alpha, std::move(generator));
    m_present_generators.emplace_back(alpha, m_pool.last_inserted());
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

            if (master.get(::Attr::Solution::Primal, it->first) > 0) {

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
        m_parent.m_master->set(Attr::Var::Obj, var, t_objective.fix(generator));
    }

    m_generation_pattern.set_obj(std::move(t_objective));

}
