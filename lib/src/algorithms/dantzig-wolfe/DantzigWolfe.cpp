//
// Created by henri on 14/12/22.
//
#include <cassert>
#include "algorithms/dantzig-wolfe/DantzigWolfe.h"
#include "algorithms/callbacks/Algorithm_Events.h"

DantzigWolfe::DantzigWolfe(Model &t_model, const UserAttr &t_complicating_constraint)
    : m_reformulation(t_model, t_complicating_constraint) {

    const unsigned int n_subproblems = m_reformulation.subproblems().size();
    m_subproblems.reserve(n_subproblems);
    for (unsigned int i = 1 ; i <= n_subproblems ; ++i) {
        m_subproblems.emplace_back(*this, i);
    }

}

void DantzigWolfe::initialize() {

    m_best_lower_bound = -Inf;
    m_best_upper_bound = +Inf;
    m_iter_lower_bound = -Inf;
    m_iter_upper_bound = +Inf;
    m_iteration_count = 0;
    m_n_generated_columns_at_last_iteration = 0;
    m_current_dual_solution.reset();
    m_adjusted_dual_solution.reset();
    m_current_is_farkas_pricing = false;

    if (!m_master_solution_strategy) {
        throw Exception("No solution strategy at hand for solving master problem.");
    }

    if (!get(Param::DantzigWolfe::FarkasPricing)) {
        add_artificial_variables();
    }

    for (auto& subproblem : m_subproblems) {
        subproblem.initialize();
    }

}

void DantzigWolfe::execute() {

    initialize();

    call_callback(Event_::Algorithm::Begin);

    do {

        if (m_n_generated_columns_at_last_iteration > 0 || m_iteration_count == 0) {

            solve_master_problem();

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

    call_callback(Event_::Algorithm::End);

    remove_artificial_variables();
}

void DantzigWolfe::solve_master_problem() {

    m_master_solution_strategy->solve();

}

void DantzigWolfe::analyze_master_problem_solution() {

    auto status = m_master_solution_strategy->status();

    set_status(status);

    if (status == Optimal) {

        m_iter_upper_bound = m_master_solution_strategy->get(Attr::Solution::ObjVal);

        m_best_upper_bound = std::min(m_iter_upper_bound, m_best_upper_bound);

        m_current_dual_solution = m_master_solution_strategy->dual_solution();

        if (m_current_is_farkas_pricing) {
            m_adjusted_dual_solution.reset();
        }

        m_current_is_farkas_pricing = false;

        if (!get(Param::DantzigWolfe::FarkasPricing)) {

            analyze_feasibility_with_artificial_variables();

            if (this->status() != Infeasible) {
                call_callback(Event_::Algorithm::NewIterUb);
                call_callback(Event_::Algorithm::NewBestUb);
            }

        } else {

            call_callback(Event_::Algorithm::NewIterUb);
            call_callback(Event_::Algorithm::NewBestUb);

        }

        return;
    }

    if (status == Infeasible) {

        if (get(Param::DantzigWolfe::FarkasPricing)) {

            m_current_is_farkas_pricing = true;
            m_current_dual_solution = m_master_solution_strategy->farkas_certificate();
            m_adjusted_dual_solution.reset();

        } else {

            idol_Log(Fatal, DantzigWolfe, "Master problem should not be infeasible when using artificial variables.");
            set_status(Fail);
            set_reason(NotSpecified);
            terminate();

        }

        return;
    }

    if (status == Unbounded) {

        set_reason(Proved);

        idol_Log(Trace, DantzigWolfe, "Terminate. Unbounded master problem.");

    } else {

        set_status(Fail);
        set_reason(NotSpecified);

        idol_Log(Trace, DantzigWolfe, "Terminate. Master problem could not be solved to optimality.");

    }

    terminate();
}

void DantzigWolfe::update_subproblems() {

    if (!m_adjusted_dual_solution) {

        m_adjusted_dual_solution = m_current_dual_solution;

    } else {

        const double factor = get(Param::DantzigWolfe::SmoothingFactor);
        if (factor == 0.) {
            m_adjusted_dual_solution = m_current_dual_solution.value();
        } else {
            m_adjusted_dual_solution = factor * m_adjusted_dual_solution.value() + (1. - factor) * m_current_dual_solution.value();
        }

    }

    for (auto& subproblem : m_subproblems) {
        subproblem.update(m_current_is_farkas_pricing, m_adjusted_dual_solution.value());
    }

}

void DantzigWolfe::solve_subproblems() {

    for (auto& subproblem : m_subproblems) {

        subproblem.solve();

        log_subproblem_solution(subproblem);

        if (is_terminated()) { break; }
    }

}

void DantzigWolfe::analyze_subproblems_solution() {

    double reduced_costs = 0;

    for (auto& subproblem : m_subproblems) {

        const auto status = subproblem.status();

        if (status == Optimal) {
            reduced_costs += subproblem.objective_value();
        } else {
            set_status(status);
            set_reason(Proved);
            terminate();
            return;
        }

    }

    if (!m_current_is_farkas_pricing) {

        const double lower_bound = m_master_solution_strategy->get(Attr::Solution::ObjVal) + reduced_costs;

        m_iter_lower_bound = lower_bound;

        call_callback(Event_::Algorithm::NewIterLb);

        if (m_best_lower_bound < lower_bound) {
            m_best_lower_bound = lower_bound;
            call_callback(Event_::Algorithm::NewBestLb);
        }

        const double best_bound_stop = get(Param::Algorithm::BestBoundStop);
        if (m_best_lower_bound > best_bound_stop) {
            set_reason(Reason::UserObjLimit);
            terminate();
            idol_Log(Trace,
                     DantzigWolfe,
                     "Terminate. Given Param::Algorithm::BestBoundStop is " << best_bound_stop
                     << " while current lower bound is " << m_best_lower_bound
            )
        }

    }

}

void DantzigWolfe::enrich_master_problem() {

    m_n_generated_columns_at_last_iteration = 0;

    for (auto& subproblem : m_subproblems) {

        bool can_enrich_master;

        if (m_current_is_farkas_pricing) {
            can_enrich_master = subproblem.objective_value() < 0;
        } else {
            can_enrich_master = subproblem.reduced_cost(m_current_dual_solution.value()) < 0;
        }

        if (can_enrich_master) {
            subproblem.enrich_master_problem();
            ++m_n_generated_columns_at_last_iteration;
        }

    }

}

AttributeManager &DantzigWolfe::attribute_delegate(const Attribute &t_attribute) {
    return *m_master_solution_strategy;
}

AttributeManager &DantzigWolfe::attribute_delegate(const Attribute &t_attribute, const Var &t_object) {
    if (m_reformulation.problem_id(t_object) == 0) {
        return *m_master_solution_strategy;
    }
    throw Exception("Accessing attribute " + t_attribute.name() + " on sub problem variable is not available.");
}

AttributeManager &DantzigWolfe::attribute_delegate(const Attribute &t_attribute, const Ctr &t_object) {
    if (m_reformulation.problem_id(t_object) == 0) {
        return *m_master_solution_strategy;
    }
    throw Exception("Accessing attribute " + t_attribute.name() + " on sub problem constraint is not available.");
}

Solution::Primal DantzigWolfe::primal_solution() const {
    auto result = m_master_solution_strategy->primal_solution();

    result.set_status(status());

    for (const auto& var : m_artificial_variables) {
        result.set(var, 0.);
    }

    for (const auto& subproblem : m_subproblems) {
        subproblem.contribute_to_primal_solution(result);
    }

    return result;
}

Solution::Dual DantzigWolfe::dual_solution() const {
    auto result = m_master_solution_strategy->dual_solution();
    return result;
}

double DantzigWolfe::get(const AttributeWithTypeAndArguments<double, Var> &t_attr, const Var &t_var) const {

    if (m_reformulation.original_formulation().get(Attr::Var::Status, t_var)) {
        return get_original_formulation(t_attr, t_var);
    }

    return Delegate::get(t_attr, t_var);
}

void DantzigWolfe::set(const AttributeWithTypeAndArguments<double, Var> &t_attr, const Var &t_var, double t_value) {

    if (m_reformulation.original_formulation().get(Attr::Var::Status, t_var)) {
        set_original_formulation(t_attr, t_var, t_value);
        return;
    }

    Delegate::set(t_attr, t_var, t_value);
}

void DantzigWolfe::set_original_formulation(const AttributeWithTypeAndArguments<double, Var> &t_attr, const Var &t_var, double t_value) {

    if (t_attr == Attr::Var::Lb || t_attr == Attr::Var::Ub) {

        auto& subproblem = m_subproblems[m_reformulation.problem_id(t_var) - 1];
        subproblem.apply_bound_expressed_in_original_space(t_attr, t_var, t_value);

        return;
    }

    throw Exception("Setting attribute " + t_attr.name() + " on variables from original formulation is not available.");

}

double DantzigWolfe::get_original_formulation(const AttributeWithTypeAndArguments<double, Var> &t_attr, const Var &t_var) const {

    if (t_attr == Attr::Var::Lb || t_attr == Attr::Var::Ub) {
        return m_reformulation.original_formulation().get(t_attr, t_var);
    }

    throw Exception("Getting attribute " + t_attr.name() + " on variables from original formulation is not available.");
}

void DantzigWolfe::clean_up() {

    for (auto& subproblem : m_subproblems) {
        subproblem.clean_up();
    }

}

void DantzigWolfe::set(const Parameter<double> &t_param, double t_value) {

    if (t_param.is_in_section(Param::Sections::DantzigWolfe)) {
        m_double_parameters.set(t_param, t_value);
        return;
    }

    Algorithm::set(t_param, t_value);
}

void DantzigWolfe::set(const Parameter<int> &t_param, int t_value) {

    if (t_param.is_in_section(Param::Sections::DantzigWolfe)) {
        m_int_parameters.set(t_param, t_value);
        return;
    }

    Algorithm::set(t_param, t_value);
}

void DantzigWolfe::set(const Parameter<bool> &t_param, bool t_value) {

    if (t_param.is_in_section(Param::Sections::DantzigWolfe)) {
        m_bool_parameters.set(t_param, t_value);
        return;
    }

    Algorithm::set(t_param, t_value);
}

bool DantzigWolfe::get(const Parameter<bool> &t_param) const {

    if (t_param.is_in_section(Param::Sections::DantzigWolfe)) {
        return m_bool_parameters.get(t_param);
    }

    return Algorithm::get(t_param);
}

double DantzigWolfe::get(const Parameter<double> &t_param) const {

    if (t_param.is_in_section(Param::Sections::DantzigWolfe)) {
        return m_double_parameters.get(t_param);
    }

    return Algorithm::get(t_param);
}

int DantzigWolfe::get(const Parameter<int> &t_param) const {

    if (t_param.is_in_section(Param::Sections::DantzigWolfe)) {
        return m_int_parameters.get(t_param);
    }

    return Algorithm::get(t_param);
}

void DantzigWolfe::log_master_solution(bool t_force) {

    if (!t_force && m_iteration_count % get(Param::DantzigWolfe::LogFrequency) != 0) {
        return;
    }

    idol_Log(Info,
             DantzigWolfe,
             "<Type=Master> "
                     << "<Iter=" << m_iteration_count << "> "
                     << "<TimT=" << time().count() << "> "
                     << "<TimI=" << m_master_solution_strategy->time().count() << "> "
                     << "<Stat=" << status() << "> "
                     << "<Reas=" << m_master_solution_strategy->reason() << "> "
                     << "<ObjV=" << m_master_solution_strategy->get(Attr::Solution::ObjVal) << "> "
                     << "<NGen=" << m_n_generated_columns_at_last_iteration << "> "
                     << "<Lb=" << m_best_lower_bound << "> "
                     << "<Ub=" << m_best_upper_bound << "> "
                     << "<RGap=" << relative_gap(m_best_lower_bound, m_best_upper_bound) * 100 << "> "
                     << "<AGap=" << absolute_gap(m_best_lower_bound, m_best_upper_bound) << "> "
             );

}

void DantzigWolfe::log_subproblem_solution(const DantzigWolfeSP &t_subproblem, bool t_force) {

    if (!t_force && m_iteration_count % get(Param::DantzigWolfe::LogFrequency) != 0) {
        return;
    }

    const auto& pricing = t_subproblem.exact_solution_strategy();

    idol_Log(Debug,
             DantzigWolfe,
             "<Type=Pricing> "
                     << "<Iter=" << m_iteration_count << "> "
                     << "<TimT=" << time().count() << "> "
                     << "<TimI=" << pricing.time().count() << "> "
                     << "<Stat=" << pricing.status() << "> "
                     << "<Reas=" << pricing.reason() << "> "
                     << "<ObjV=" << pricing.get(Attr::Solution::ObjVal) << "> "
                     << "<NGen=" << m_n_generated_columns_at_last_iteration << "> "
                     << "<Lb=" << m_best_lower_bound << "> "
                     << "<Ub=" << m_best_upper_bound << "> "
                     << "<RGap=" << relative_gap(m_best_lower_bound, m_best_upper_bound) * 100 << "> "
                     << "<AGap=" << absolute_gap(m_best_lower_bound, m_best_upper_bound) << "> "
    );

}

void DantzigWolfe::add_artificial_variables() {

    const auto& master = m_reformulation.master_problem();
    const double artificial_cost = get(Param::DantzigWolfe::ArtificialVarCost);
    const auto add_to = [&](const Ctr& t_ctr, double t_sign) {

        Column column(artificial_cost);
        column.linear().set(t_ctr, t_sign);

        auto var = m_master_solution_strategy->add_var(
                TempVar(0., Inf, Continuous, std::move(column))
        );

        m_artificial_variables.emplace_back(var);

    };

    for (const auto& ctr : master.ctrs()) {

        const auto type = master.get(Attr::Ctr::Type, ctr);

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

void DantzigWolfe::remove_artificial_variables() {

    for (const Var& var : m_artificial_variables) {
        m_master_solution_strategy->remove(var);
    }

    m_artificial_variables.clear();

}

void DantzigWolfe::analyze_feasibility_with_artificial_variables() {

    if (m_artificial_variables.empty()) { return; }

    const auto& primals = m_master_solution_strategy->primal_solution();
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

bool DantzigWolfe::stopping_condition() const {
    return absolute_gap(m_best_lower_bound, m_best_upper_bound) <= ToleranceForAbsoluteGapPricing
        || relative_gap(m_best_lower_bound, m_best_upper_bound) <= ToleranceForRelativeGapPricing
        || remaining_time() == 0;
}

double DantzigWolfe::get(const AttributeWithTypeAndArguments<double, void> &t_attr) const {

    if (t_attr.is_in_section(Attr::Sections::Algorithm)) {

        if (t_attr == Attr::Algorithm::BestLb) {
            return m_best_lower_bound;
        }

        if (t_attr == Attr::Algorithm::BestUb) {
            return m_best_upper_bound;
        }

        if (t_attr == Attr::Algorithm::IterLb) {
            return m_iter_lower_bound;
        }

        if (t_attr == Attr::Algorithm::IterUb) {
            return m_iter_upper_bound;
        }

        if (t_attr == Attr::Algorithm::RelativeGap) {
            return relative_gap(m_best_lower_bound, m_best_upper_bound);
        }

        if (t_attr == Attr::Algorithm::AbsoluteGap) {
            return absolute_gap(m_best_lower_bound, m_best_upper_bound);
        }

    }

    return Delegate::get(t_attr);
}
