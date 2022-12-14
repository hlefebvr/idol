//
// Created by henri on 14/12/22.
//
#include "algorithms/dantzig-wolfe/DantzigWolfe.h"

DantzigWolfe::DantzigWolfe(Model &t_model, const UserAttr &t_complicating_constraint)
    : m_reformulation(t_model, t_complicating_constraint) {

    const unsigned int n_subproblems = m_reformulation.subproblems().size();
    m_subproblems.reserve(n_subproblems);
    for (unsigned int i = 1 ; i <= n_subproblems ; ++i) {
        m_subproblems.emplace_back(*this, i);
    }

}

void DantzigWolfe::initialize() {

    m_lower_bound = -Inf;
    m_upper_bound = +Inf;
    m_iteration_count = 0;
    m_n_generated_columns_at_last_iteration = 0;

    if (!m_master_solution_strategy) {
        throw Exception("No solution strategy at hand for solving master problem.");
    }

    for (auto& subproblem : m_subproblems) {
        subproblem.initialize();
    }

}

void DantzigWolfe::execute() {

    initialize();

    while (true) {

        solve_master_problem();

        log_master_solution();

        analyze_master_problem_solution();

        if (is_terminated()) { break; }

        update_subproblems();

        solve_subproblems();

        analyze_subproblems_solution();

        if (is_terminated()) { break; }

        clean_up();

        enrich_master_problem();

        ++m_iteration_count;
    }

    log_master_solution(true);

}

void DantzigWolfe::solve_master_problem() {

    m_master_solution_strategy->solve();

}

void DantzigWolfe::analyze_master_problem_solution() {

    auto status = m_master_solution_strategy->status();

    if (status == Optimal) {
        m_upper_bound = std::min(m_master_solution_strategy->get(Attr::Solution::ObjVal), m_upper_bound);
        return;
    }

    if (status == Infeasible) {
        return;
    }

    if (status == Unbounded) {

        set_status(Unbounded);
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

    for (auto& subproblem : m_subproblems) {
        subproblem.update();
    }

}

void DantzigWolfe::solve_subproblems() {

    for (auto& subproblem : m_subproblems) {

        subproblem.solve();

        if (is_terminated()) { break; }
    }

}

void DantzigWolfe::analyze_subproblems_solution() {

    double reduced_costs = 0;

    for (auto& subproblem : m_subproblems) {
        const auto status = subproblem.status();

        if (status == Optimal) {
            reduced_costs += subproblem.last_computed_reduced_cost();
        } else {
            set_status(status);
            set_reason(Proved);
            return;
        }

    }

    if (m_master_solution_strategy->status() == Optimal) {

        const double lower_bound = m_master_solution_strategy->get(Attr::Solution::ObjVal) + reduced_costs;
        m_lower_bound = std::max(lower_bound, m_lower_bound);

    }

}

void DantzigWolfe::enrich_master_problem() {


    m_n_generated_columns_at_last_iteration = 0;

    for (auto& subproblem : m_subproblems) {

        if (subproblem.last_computed_reduced_cost() < -ToleranceForAbsoluteGapPricing) {
            subproblem.enrich_master_problem();
            ++m_n_generated_columns_at_last_iteration;
        }

    }

    if (m_n_generated_columns_at_last_iteration == 0) {
        set_reason(Proved);
        terminate();
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

    for (const auto& subproblem : m_subproblems) {
        subproblem.contribute_to_primal_solution(result);
    }

    return result;
}

Solution::Dual DantzigWolfe::dual_solution() const {
    return m_master_solution_strategy->dual_solution();
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
             << "<Time=" << time().count() << "> "
             << "<Stat=" << m_master_solution_strategy->status() << "> "
             << "<Reas=" << m_master_solution_strategy->reason() << "> "
             << "<ObjV=" << m_master_solution_strategy->get(Attr::Solution::ObjVal) << "> "
             << "<NGen=" << m_n_generated_columns_at_last_iteration << "> "
             << "<Lb=" << m_lower_bound << "> "
             << "<Ub=" << m_upper_bound << "> "
             << "<RGap=" << relative_gap(m_lower_bound, m_upper_bound) * 100 << "> "
             << "<AGap=" << absolute_gap(m_lower_bound, m_upper_bound) << "> "
             );

}
