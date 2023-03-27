//
// Created by henri on 07/02/23.
//
#include <cassert>
#include "optimizers/column-generation/ColumnGeneration.h"
#include "optimizers/parameters/Logs.h"
#include "modeling/objects/Versions.h"
#include "modeling/expressions/operations/operators.h"

Backends::ColumnGeneration::ColumnGeneration(const BlockModel<Ctr> &t_model) : Algorithm(t_model) {

    m_master.reset(t_model.master().clone());

    const unsigned int n_blocks = t_model.n_blocks();
    m_subproblems.reserve(n_blocks);
    for (unsigned int i = 0 ; i < n_blocks ; ++i) {
        m_subproblems.emplace_back(*this, i);
    }

}

Backends::ColumnGeneration::ColumnGeneration(const BlockModel<Ctr> &t_model,
                                   const OptimizerFactory &t_master_optimizer,
                                   const OptimizerFactory &t_pricing_optimizer)
                                   : Algorithm(t_model),
                                     m_pricing_optimizer(t_pricing_optimizer.clone()) {

    m_master.reset(t_model.master().clone());
    m_master->use(t_master_optimizer);

    const unsigned int n_blocks = t_model.n_blocks();
    m_subproblems.reserve(n_blocks);
    for (unsigned int i = 0 ; i < n_blocks ; ++i) {
        m_subproblems.emplace_back(*this, i, t_pricing_optimizer);
    }

}

void Backends::ColumnGeneration::hook_optimize() {

    //call_callback(Event_::Algorithm::Begin);

    do {

        if (m_n_generated_columns_at_last_iteration > 0 || m_iteration_count == 0) {

            idol_Log(Trace, ColumnGeneration, "Solving master problem.");
            solve_master_problem();
            idol_Log(Trace, ColumnGeneration, "Master problem has been solved.");

            analyze_master_problem_solution();

            log_master_solution();

        } else if (m_current_is_farkas_pricing) {

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

    } while (true);

    log_master_solution(true);

    //call_callback(Event_::Algorithm::End);

    remove_artificial_variables();

}

void Backends::ColumnGeneration::add(const Var &t_var) {
    throw Exception("Not implemented");
}

void Backends::ColumnGeneration::add(const Ctr &t_ctr) {
    m_master->add(t_ctr);
}

void Backends::ColumnGeneration::remove(const Var &t_var) {
    throw Exception("Not implemented");
}

void Backends::ColumnGeneration::remove(const Ctr &t_ctr) {
    throw Exception("Not implemented");
}

void Backends::ColumnGeneration::update() {

    const unsigned int current_n_blocks = m_subproblems.size();
    const unsigned int parent_n_blocks = parent().n_blocks();

    for (unsigned int k = current_n_blocks ; k < parent_n_blocks ; ++k) {
        m_subproblems.emplace_back(*this, k);
    }

}

void Backends::ColumnGeneration::write(const std::string &t_name) {
    m_master->write(t_name);
}

void Backends::ColumnGeneration::hook_before_optimize() {

    set_best_bound(-Inf);
    set_best_obj(+Inf);
    m_iter_lower_bound = -Inf;
    m_iter_upper_bound = +Inf;
    m_iteration_count = 0;
    m_n_generated_columns_at_last_iteration = 0;
    m_current_dual_solution.reset();
    m_adjusted_dual_solution.reset();
    m_current_is_farkas_pricing = false;

    if (!get(Param::ColumnGeneration::FarkasPricing)) {
        add_artificial_variables();
    }

    for (auto& subproblem : m_subproblems) {
        subproblem.hook_before_solve();
    }

    Algorithm::hook_before_optimize();
}

void Backends::ColumnGeneration::add_artificial_variables() {

    auto& env = m_master->env();
    const double artificial_cost = get(Param::ColumnGeneration::ArtificialVarCost);
    const auto add_to = [&](const Ctr& t_ctr, double t_sign) {

        Column column(artificial_cost);
        column.linear().set(t_ctr, t_sign);

        Var artifical_var(env, 0., Inf, Continuous, std::move(column));
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

void Backends::ColumnGeneration::set(const Parameter<bool> &t_param, bool t_value) {

    if (t_param.is_in_section(Param::Sections::ColumnGeneration)) {

        m_bool_parameters.set(t_param, t_value);

        if (t_param == Param::ColumnGeneration::FarkasPricing) {
            m_master->set(Param::Algorithm::InfeasibleOrUnboundedInfo, true);
        }

        return;
    }

    Algorithm::set(t_param, t_value);
}

void Backends::ColumnGeneration::set(const Parameter<double> &t_param, double t_value) {

    if (t_param.is_in_section(Param::Sections::ColumnGeneration)) {
        m_double_parameters.set(t_param, t_value);
        return;
    }

    Algorithm::set(t_param, t_value);
}

void Backends::ColumnGeneration::set(const Parameter<int> &t_param, int t_value) {

    if (t_param.is_in_section(Param::Sections::ColumnGeneration)) {
        m_int_parameters.set(t_param, t_value);
        return;
    }

    Algorithm::set(t_param, t_value);
}

bool Backends::ColumnGeneration::get(const Parameter<bool> &t_param) const {

    if (t_param.is_in_section(Param::Sections::ColumnGeneration)) {
        return m_bool_parameters.get(t_param);
    }

    return Algorithm::get(t_param);
}

double Backends::ColumnGeneration::get(const Parameter<double> &t_param) const {

    if (t_param.is_in_section(Param::Sections::ColumnGeneration)) {
        return m_double_parameters.get(t_param);
    }

    return Algorithm::get(t_param);
}

int Backends::ColumnGeneration::get(const Parameter<int> &t_param) const {

    if (t_param.is_in_section(Param::Sections::ColumnGeneration)) {
        return m_int_parameters.get(t_param);
    }

    return Algorithm::get(t_param);
}

const BlockModel<Ctr>&Backends::ColumnGeneration::parent() const {
    return dynamic_cast<const BlockModel<Ctr>&>(Backend::parent());
}

void Backends::ColumnGeneration::solve_master_problem() {
    m_master->optimize();
}

void Backends::ColumnGeneration::log_master_solution(bool t_force) const {

    if (!t_force && m_iteration_count % get(Param::ColumnGeneration::LogFrequency) != 0) {
        return;
    }

    idol_Log(Info,
             ColumnGeneration,
             "<Type=Master> "
             << "<Iter=" << m_iteration_count << "> "
             << "<TimT=" << parent().time().count() << "> "
             << "<TimI=" << m_master->time().count() << "> "
             << "<Stat=" << (SolutionStatus) m_master->get(Attr::Solution::Status) << "> "
             << "<Reas=" << (SolutionReason) m_master->get(Attr::Solution::Reason) << "> "
             << "<Obj=" << m_master->get(Attr::Solution::ObjVal) << "> "
             << "<NGen=" << m_n_generated_columns_at_last_iteration << "> "
             << "<BestBnd=" << best_bound() << "> "
             << "<BestObj=" << best_obj() << "> "
             << "<RGap=" << get(Attr::Solution::RelGap) * 100 << "> "
             << "<AGap=" << get(Attr::Solution::AbsGap) << "> "
    );

}

void Backends::ColumnGeneration::log_subproblem_solution(const ColumnGenerationSP& t_subproblem, bool t_force) const {

    if (!t_force && m_iteration_count % get(Param::ColumnGeneration::LogFrequency) != 0) {
        return;
    }

    const auto& pricing = t_subproblem.model();

    idol_Log(Debug,
             ColumnGeneration,
             "<Type=Pricing> "
             << "<Iter=" << m_iteration_count << "> "
             << "<TimT=" << parent().time().count() << "> "
             << "<TimI=" << pricing.time().count() << "> "
             << "<Stat=" << (SolutionStatus) pricing.get(Attr::Solution::Status) << "> "
             << "<Reas=" << (SolutionReason) pricing.get(Attr::Solution::Reason) << "> "
             << "<Obj=" << pricing.get(Attr::Solution::ObjVal) << "> "
             << "<NGen=" << m_n_generated_columns_at_last_iteration << "> "
             << "<BestBnd=" << best_bound() << "> "
             << "<BestObj=" << best_obj() << "> "
             << "<RGap=" << get(Attr::Solution::RelGap) * 100 << "> "
             << "<AGap=" << get(Attr::Solution::AbsGap) << "> "
    );

}

void Backends::ColumnGeneration::analyze_master_problem_solution() {
    
    auto status = m_master->get(Attr::Solution::Status);

    set_status(status);

    if (status == Optimal) {

        m_iter_upper_bound = m_master->get(Attr::Solution::ObjVal);

        set_best_obj(std::min(m_iter_upper_bound, best_obj()));

        m_current_dual_solution = save(*m_master, Attr::Solution::Dual);

        if (m_current_is_farkas_pricing) {
            m_adjusted_dual_solution.reset();
        }

        m_current_is_farkas_pricing = false;

        if (!get(Param::ColumnGeneration::FarkasPricing)) {

            analyze_feasibility_with_artificial_variables();

            if (this->status() != Infeasible) {
                //call_callback(Event_::Algorithm::NewIterUb);
                //call_callback(Event_::Algorithm::NewBestUb);
            }

        } else {

            //call_callback(Event_::Algorithm::NewIterUb);
            //call_callback(Event_::Algorithm::NewBestUb);

        }

        return;
    }

    if (status == Infeasible) {

        if (get(Param::ColumnGeneration::FarkasPricing)) {

            m_current_is_farkas_pricing = true;
            m_current_dual_solution = save(*m_master, Attr::Solution::Farkas);
            m_adjusted_dual_solution.reset();

        } else {

            idol_Log(Fatal, ColumnGeneration, "ERROR. Master problem should not be infeasible when using artificial variables.");
            set_status(Fail);
            set_reason(NotSpecified);
            terminate();

        }

        return;
    }

    if (status == Unbounded) {

        switch_to_pure_phase_I();

        set_reason(Proved);

        idol_Log(Trace, ColumnGeneration, "Terminate. Unbounded master problem.");

    } else {

        set_status(Fail);
        set_reason(NotSpecified);

        idol_Log(Trace, ColumnGeneration, "Terminate. Master problem could not be solved to optimality.");

    }

    terminate();

}

void Backends::ColumnGeneration::update_subproblems() {

    if (!m_adjusted_dual_solution) {

        m_adjusted_dual_solution = m_current_dual_solution;

    } else {

        const double factor = get(Param::ColumnGeneration::SmoothingFactor);
        if (factor == 0.) {
            m_adjusted_dual_solution = m_current_dual_solution.value();
        } else {
            m_adjusted_dual_solution = factor * m_adjusted_dual_solution.value() + (1. - factor) * m_current_dual_solution.value();
        }

    }

    for (auto& subproblem : m_subproblems) {
        subproblem.update_objective(m_current_is_farkas_pricing, m_adjusted_dual_solution.value());
    }

}

void Backends::ColumnGeneration::solve_subproblems() {

    const unsigned int n_threads = get(Param::ColumnGeneration::NumParallelPricing);

    #pragma omp parallel for num_threads(n_threads) default(none)
    for (auto & subproblem : m_subproblems) {
        subproblem.solve();
    }

    for (auto& subproblem : m_subproblems) {

        log_subproblem_solution(subproblem);

        if (is_terminated()) { break; }

    }

}

void Backends::ColumnGeneration::analyze_subproblems_solution() {

    double reduced_costs = 0;

    for (auto& subproblem : m_subproblems) {

        const auto status = subproblem.model().get(Attr::Solution::Status);

        if (status == Optimal) {
            reduced_costs += subproblem.model().get(Attr::Solution::ObjVal);
        } else {
            set_status(status);
            set_reason(Proved);
            terminate();
            return;
        }

    }

    if (!m_current_is_farkas_pricing) {

        const double lower_bound = m_master->get(Attr::Solution::ObjVal) + reduced_costs;

        m_iter_lower_bound = lower_bound;

        //call_callback(Event_::Algorithm::NewIterLb);

        if (best_bound() < lower_bound) {
            set_best_bound(lower_bound);
            //call_callback(Event_::Algorithm::NewBestLb);
        }

        const double best_bound_stop = get(Param::Algorithm::BestBoundStop);
        if (best_bound() > best_bound_stop) {
            set_reason(SolutionReason::UserObjLimit);
            terminate();
            idol_Log(Trace,
                     ColumnGeneration,
                     "Terminate. Given Param::Algorithm::BestBoundStop is " << best_bound_stop
                        << " while current best bound is " << best_bound()
            )
        }

    }

}

void Backends::ColumnGeneration::enrich_master_problem() {

    m_n_generated_columns_at_last_iteration = 0;

    for (auto& subproblem : m_subproblems) {

        bool can_enrich_master;

        if (m_current_is_farkas_pricing) {
            can_enrich_master = subproblem.model().get(Attr::Solution::ObjVal) < -1e-6;
        } else {
            can_enrich_master = subproblem.compute_reduced_cost(m_current_dual_solution.value()) < 0;
        }

        if (can_enrich_master) {
            subproblem.enrich_master_problem();
            ++m_n_generated_columns_at_last_iteration;
        }

    }

}

void Backends::ColumnGeneration::clean_up() {

    for (auto& subproblem : m_subproblems) {
        subproblem.clean_up();
    }

}

void Backends::ColumnGeneration::analyze_feasibility_with_artificial_variables() {

    if (m_artificial_variables.empty()) { return; }

    const auto& primals = save(*m_master, Attr::Solution::Primal);
    bool has_positive_artificial_variable = false;

    for (const auto& var : m_artificial_variables) {
        if (primals.get(var) > 0) {
            has_positive_artificial_variable = true;
            break;
        }
    }

    if (has_positive_artificial_variable) {
        set_status(Infeasible);
    } else {
        remove_artificial_variables();
    }

}

void Backends::ColumnGeneration::remove_artificial_variables() {

    for (const Var& var : m_artificial_variables) {
        m_master->remove(var);
    }

    m_artificial_variables.clear();
}

bool Backends::ColumnGeneration::stopping_condition() const {
    return get(Attr::Solution::AbsGap) <= ToleranceForAbsoluteGapPricing
           || get(Attr::Solution::RelGap) <= ToleranceForRelativeGapPricing
           || parent().remaining_time() == 0;
}

double Backends::ColumnGeneration::get(const Req<double, Var> &t_attr, const Var &t_var) const {

    const unsigned int subproblem_id = parent().has_opposite_axis() ? t_var.get(parent().opposite_axis()) : MasterId;

    if (subproblem_id == MasterId) {
        return m_master->get(t_attr, t_var);
    }

    if (t_attr == Attr::Solution::Primal) {
        return m_subproblems[subproblem_id].compute_original_space_primal(t_var);
    }

    if (t_attr == Attr::Var::Lb || t_attr == Attr::Var::Ub) {
        return parent().block(subproblem_id).model().get(t_attr, t_var);
    }

    return m_subproblems[subproblem_id].model().get(t_attr, t_var);

}

void Backends::ColumnGeneration::set(const Req<double, Var> &t_attr, const Var &t_var, double t_value) {

    const unsigned int subproblem_id = parent().has_opposite_axis() ? t_var.get(parent().opposite_axis()) : MasterId;

    if (subproblem_id == MasterId) {
        m_master->set(t_attr, t_var, t_value);
        return;
    }

    if (t_attr == Attr::Var::Lb) {
        m_subproblems[subproblem_id].apply_lb(t_var, t_value);
        return;
    }

    if (t_attr == Attr::Var::Ub) {
        m_subproblems[subproblem_id].apply_ub(t_var, t_value);
        return;
    }

    Algorithm::set(t_attr, t_var, t_value);

}

void Backends::ColumnGeneration::set(const Req<Expr<Var, Var>, void> &t_attr, Expr<Var, Var> &&t_expr) {

    if (t_attr == Attr::Obj::Expr) {

        std::cout << "Updating obj for " << t_expr << std::endl;

        const unsigned int n_subproblems = m_subproblems.size();

        Expr<Var, Var> master_obj = std::move(t_expr.constant());
        std::vector<Constant> pricing_obj(n_subproblems);

        for (auto [var, coeff] : t_expr.linear()) {
            const unsigned int subproblem_id = parent().has_opposite_axis() ? var.get(parent().opposite_axis()) : MasterId;
            if (subproblem_id == MasterId) {
                master_obj += coeff * var;
            } else {
                if (!coeff.is_numerical()) {
                    throw Exception("Could not handle non-numerical objective coefficient as generation pattern.");
                }
                pricing_obj[subproblem_id] += coeff.numerical() * !var;
            }
        }

        std::cout << "master -> " << master_obj << std::endl;

        m_master->set(t_attr, std::move(master_obj));

        for (unsigned int k = 0 ; k < n_subproblems ; ++k) {
            std::cout << k << " -> " << pricing_obj[k] << std::endl;
            m_subproblems[k].generation_pattern().set_obj(std::move(pricing_obj[k]));
        }

        return;
    }

    Base::set(t_attr, t_expr);
}

void Backends::ColumnGeneration::switch_to_pure_phase_I() {

    Expr<Var, Var> objective;

    for (const auto& var : m_artificial_variables) {
        objective += var;
    }

    set(Attr::Obj::Expr, std::move(objective));

    m_current_is_phase_I = true;
}

void Backends::ColumnGeneration::restore_from_pure_phase_I() {

    set(Attr::Obj::Expr, parent().get(Attr::Obj::Expr));

    m_current_is_phase_I = false;

}
