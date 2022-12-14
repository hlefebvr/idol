//
// Created by henri on 14/12/22.
//
#include "algorithms/generation/BranchingManagers_OnMaster.h"
#include "algorithms/generation/DantzigWolfe.h"

double BranchingManagers::OnMaster::get_bound(const Map<Var, Ctr>& t_container,
                                              const AttributeWithTypeAndArguments<double, Var> &t_attr,
                                              const Var &t_var) const {

    const auto it = t_container.find(t_var);

    if (it == t_container.end()) {
        return m_parent.exact_solution_strategy().get(t_attr, t_var);
    }

    const auto& rhs = m_parent.parent().master_solution_strategy().get(Attr::Ctr::Rhs, it->second);

    if (!rhs.is_numerical()) {
        throw Exception("Unexpected parametrized constant for bound constraint.");
    }

    return rhs.numerical();
}

double BranchingManagers::OnMaster::get_lb(const Var &t_var) const {
    return get_bound(m_lower_bound_constraints, Attr::Var::Lb, t_var);
}

double BranchingManagers::OnMaster::get_ub(const Var &t_var) const {
    return get_bound(m_upper_bound_constraints, Attr::Var::Ub, t_var);
}

void BranchingManagers::OnMaster::set_lb(const Var &t_var, double t_lb) {
    set_bound(m_lower_bound_constraints, Attr::Var::Lb, t_var, t_lb);
}

void BranchingManagers::OnMaster::set_ub(const Var &t_var, double t_ub) {
    set_bound(m_upper_bound_constraints, Attr::Var::Ub, t_var, t_ub);
}

void BranchingManagers::OnMaster::set_bound(Map<Var, Ctr> &t_container,
                                            const AttributeWithTypeAndArguments<double, Var> &t_attr,
                                            const Var &t_var,
                                            double t_value) {

    auto& master = m_parent.parent().master_solution_strategy();

    const auto it = t_container.find(t_var);

    if (it == t_container.end()) { // Create a new constraint

        auto expanded = m_parent.expand_variable(t_var);

        auto ctr = master.add_ctr(t_attr == Attr::Var::Lb ? std::move(expanded) >= t_value : std::move(expanded) <= t_value);
        m_parent.column_template().column().linear().set(ctr, !t_var);
        t_container.emplace(t_var, ctr);

        return;
    }

    if (equals(t_value, get_bound(t_container, t_attr, t_var), ToleranceForIntegrality)) { // Remove existing constraint for it is not useful anymore

        m_parent.column_template().column().linear().remove(it->second);
        master.remove(it->second);
        t_container.erase(it);

        return;
    }

    master.set(Attr::Ctr::Rhs, it->second, t_value);

}

