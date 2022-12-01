//
// Created by henri on 21/09/22.
//

#ifndef OPTIMIZE_BRANCHINGSTRATEGIES_MOSTINFEASIBLE_H
#define OPTIMIZE_BRANCHINGSTRATEGIES_MOSTINFEASIBLE_H

#include "BranchingStrategies_VariableBranchingBase.h"
#include <vector>
#include <list>

namespace BranchingStrategies {
    class MostInfeasible;
}

class BranchingStrategies::MostInfeasible {
public:
    template<class NodeT> class Strategy;
};

template<class NodeT>
class BranchingStrategies::MostInfeasible::Strategy : public BranchingStrategies::Base::OnVariables::Strategy<NodeT> {
    std::vector<Var> m_branching_candidates;
protected:
    static double fractional_part(double t_x);
    static bool is_integer(double t_x);
    virtual double most_infeasible_score(const Var& t_var, const NodeT& t_node) const;
public:
    explicit Strategy(std::vector<Var> t_branching_candidates) : m_branching_candidates(std::move(t_branching_candidates)) {}

    bool is_valid(const NodeT &t_node) const override;

    std::list<NodeT *> create_child_nodes(const NodeT &t_node, const std::function<unsigned int()>& t_id_provider) override;
};

template<class NodeT>
bool BranchingStrategies::MostInfeasible::Strategy<NodeT>::is_valid(const NodeT &t_node) const {
    const auto& primal = t_node.primal_solution();

    for (const auto& var : m_branching_candidates) {
        if (double value = primal.get(var) ; !is_integer(value)) {
            idol_Log(Trace, "branch-and-bound", "Node " << t_node.id() << " solution not valid (" << var << " = " << value << ")." );
            return false;
        }
    }

    return true;
}

template<class NodeT>
std::list<NodeT *> BranchingStrategies::MostInfeasible::Strategy<NodeT>::create_child_nodes(
        const NodeT &t_node,
        const std::function<unsigned int()>& t_id_provider
    ) {

    this->select_variable_for_branching(
            m_branching_candidates.begin(),
            m_branching_candidates.end(),
            [&](const Var& t_var){ return most_infeasible_score(t_var, t_node); }
        );

    if (!this->has_variable_selected_for_branching()) {
        throw Exception("No variable selected for branching.");
    }

    const auto [variable, score] = this->variable_selected_for_branching();

    if (score <= ToleranceForIntegrality) {
        throw Exception("Maximum infeasibility is less than ToleranceForIntegrality.");
    }

    const double value = t_node.primal_solution().get(variable);
    const double lb = std::ceil(value);
    const double ub = std::floor(value);

    return this->create_child_nodes_by_bound(t_node, t_id_provider, variable, value, lb, ub);
}

template<class NodeT>
double BranchingStrategies::MostInfeasible::Strategy<NodeT>::fractional_part(double t_x) {
    return std::abs(std::round(t_x) - t_x);
}

template<class NodeT>
bool BranchingStrategies::MostInfeasible::Strategy<NodeT>::is_integer(double t_x) {
    return fractional_part(t_x) <= ToleranceForIntegrality;
}

template<class NodeT>
double BranchingStrategies::MostInfeasible::Strategy<NodeT>::most_infeasible_score(const Var &t_var, const NodeT &t_node) const {
    const double frac_value = fractional_part(t_node.primal_solution().get(t_var));
    if (frac_value <= ToleranceForIntegrality) {
        return -Inf;
    }
    return std::max(frac_value, 1. - frac_value);
}

#endif //OPTIMIZE_BRANCHINGSTRATEGIES_MOSTINFEASIBLE_H
