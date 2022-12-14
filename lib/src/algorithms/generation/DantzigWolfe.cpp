//
// Created by henri on 14/12/22.
//
#include "algorithms/generation/DantzigWolfe.h"
#include "algorithms/generation/Generation.h"

DantzigWolfe::DantzigWolfe(Model &t_model, const UserAttr &t_complicating_constraint)
    : m_reformulation(t_model, t_complicating_constraint) {

    const unsigned int n_subproblems = m_reformulation.subproblems().size();
    m_subproblems.reserve(n_subproblems);
    for (unsigned int i = 1 ; i <= n_subproblems ; ++i) {
        m_subproblems.emplace_back(*this, i);
    }

}

void DantzigWolfe::initialize() {

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

        analyze_master_problem_solution();

        if (is_terminated()) { break; }

        update_subproblems();

        solve_subproblems();

        analyze_subproblems_solution();

        if (is_terminated()) { break; }

        enrich_master_problem();

    }

}

void DantzigWolfe::solve_master_problem() {

    m_master_solution_strategy->solve();

}

void DantzigWolfe::analyze_master_problem_solution() {

    auto status = m_master_solution_strategy->status();

    if (status == Optimal || status == Infeasible) {
        return;
    }

    if (status == Unbounded) {

        set_status(Unbounded);
        set_reason(Proved);

        idol_Log(Trace, "column-generation", "Terminate. Unbounded master problem.");

    } else {

        set_status(Fail);
        set_reason(NotSpecified);

        idol_Log(Trace, "column-generation", "Terminate. Master problem could not be solved to optimality.");

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

    for (auto& subproblem : m_subproblems) {
        if (auto status = subproblem.status() ; status != Optimal) {
            set_status(status);
            set_reason(Proved);
            return;
        }
    }

}

void DantzigWolfe::enrich_master_problem() {


    bool master_has_been_enriched = false;

    for (auto& subproblem : m_subproblems) {

        if (subproblem.can_enrich_master()) {
            subproblem.enrich_master_problem();
            master_has_been_enriched = true;
        }

    }

    if (!master_has_been_enriched) {
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

    std::cout << result << std::endl;

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

        if (get(Param::DantzigWolfe::BranchingOnMaster)) {
            subproblem.apply_original_space_bound_on_master(t_attr, t_var, t_value);
        } else {
            subproblem.apply_original_space_bound_on_pricing(t_attr, t_var, t_value);
        }

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

bool DantzigWolfe::get(const Parameter<bool> &t_param) const {

    if (t_param.is_in_section(Param::Sections::DantzigWolfe)) {
        return m_bool_parameters.get(t_param);
    }

    return Algorithm::get(t_param);
}

void DantzigWolfe::set(const Parameter<bool> &t_param, bool t_value) {

    if (t_param.is_in_section(Param::Sections::DantzigWolfe)) {
        m_bool_parameters.set(t_param, t_value);
        return;
    }

    Algorithm::set(t_param, t_value);
}

