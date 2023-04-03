//
// Created by henri on 21/03/23.
//

#ifndef IDOL_MOSTINFEASBILE_IMPL_H
#define IDOL_MOSTINFEASBILE_IMPL_H

#include "VariableBranchingRule.h"
#include <iostream>
#include <list>

namespace BranchingRules {
    template<class NodeT> class MostInfeasible;
}

template<class NodeInfoT>
class BranchingRules::MostInfeasible : public VariableBranchingRule<NodeInfoT> {
    std::list<Var> m_branching_candidates;
protected:
    static double fractional_part(double t_x);
    static bool is_integer(double t_x);
    virtual double most_infeasible_score(const Var& t_var, const Node<NodeInfoT>& t_node) const;

    virtual void select_integer_variable_for_branching(const Node<NodeInfoT>& t_node);
    virtual std::list<NodeInfoT *> create_child_nodes_by_bound_for_integer_variable(
            const Node<NodeInfoT> &t_node,
            const Var& t_var,
            double t_value);
public:
    MostInfeasible(const Model& t_model, std::list<Var> t_explicit_branching_candidates)
        : VariableBranchingRule<NodeInfoT>(t_model), m_branching_candidates(std::move(t_explicit_branching_candidates)) {

    }

    explicit MostInfeasible(const Model& t_model) : VariableBranchingRule<NodeInfoT>(t_model) {

        for (const auto& var : t_model.vars()) {
            if (t_model.get(Attr::Var::Type, var) != Continuous) {
                m_branching_candidates.emplace_back(var);
            }
        }

    }

    bool is_valid(const Node<NodeInfoT> &t_node) const override;

    [[nodiscard]] std::list<NodeInfoT*> create_child_nodes(const Node<NodeInfoT>& t_node) override;
};

template<class NodeInfoT>
std::list<NodeInfoT *> BranchingRules::MostInfeasible<NodeInfoT>::create_child_nodes_by_bound_for_integer_variable(
        const Node<NodeInfoT> &t_node, const Var &t_var, double t_value) {

    const double lb = std::ceil(t_value);
    const double ub = std::floor(t_value);

    return this->create_child_nodes_by_bound(t_node, t_var, t_value, lb, ub);
}

template<class NodeInfoT>
void BranchingRules::MostInfeasible<NodeInfoT>::select_integer_variable_for_branching(const Node<NodeInfoT> &t_node) {

    this->select_variable_for_branching(
            m_branching_candidates.begin(),
            m_branching_candidates.end(),
            [&](const Var& t_var){ return most_infeasible_score(t_var, t_node); }
    );

}

template<class NodeT>
double BranchingRules::MostInfeasible<NodeT>::most_infeasible_score(const Var &t_var, const Node<NodeT> &t_node) const {
    const double frac_value = fractional_part(t_node.info().primal_solution().get(t_var));
    if (frac_value <= ToleranceForIntegrality) { return -Inf; }
    return .5 - std::abs(.5 - frac_value);
}

template<class NodeT>
bool BranchingRules::MostInfeasible<NodeT>::is_integer(double t_x) {
    return fractional_part(t_x) <= ToleranceForIntegrality;
}

template<class NodeT>
double BranchingRules::MostInfeasible<NodeT>::fractional_part(double t_x) {
    return std::abs(std::round(t_x) - t_x);
}

template<class NodeT>
bool BranchingRules::MostInfeasible<NodeT>::is_valid(const Node<NodeT> &t_node) const {

    const auto& primal = t_node.info().primal_solution();

    for (const auto& var : m_branching_candidates) {
        if (double value = primal.get(var) ; !is_integer(value)) {
            idol_Log2(Trace, "Node " << t_node.id() << " solution not valid (" << var << " = " << value << ")." );
            return false;
        }
    }

    return true;
}

template<class NodeT>
std::list<NodeT *>
BranchingRules::MostInfeasible<NodeT>::create_child_nodes(const Node<NodeT> &t_node) {

    this->reset_variable_selected_for_branching();

    this->select_integer_variable_for_branching(t_node);

    if (!this->has_variable_selected_for_branching()) {
        throw Exception("No variable selected for branching.");
    }

    const auto [variable, score] = this->variable_selected_for_branching();

    if (score <= ToleranceForIntegrality) {
        throw Exception("Maximum infeasibility is less than ToleranceForIntegrality.");
    }

    const double value = t_node.info().primal_solution().get(variable);

    return create_child_nodes_by_bound_for_integer_variable(t_node, variable, value);
}

#endif //IDOL_MOSTINFEASBILE_IMPL_H
