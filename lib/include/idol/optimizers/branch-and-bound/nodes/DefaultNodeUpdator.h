//
// Created by henri on 18.10.23.
//

#ifndef IDOL_DEFAULTNODEUPDATOR_H
#define IDOL_DEFAULTNODEUPDATOR_H

#include "NodeUpdator.h"
#include "idol/modeling/models/Model.h"
#include "Node.h"
#include "DefaultNodeInfo.h"
#include "BranchingDecision.h"

namespace idol {
    template<class NodeT>
    class DefaultNodeUpdator;
}

template<class NodeInfoT>
class idol::DefaultNodeUpdator : public NodeUpdator<NodeInfoT> {

    Model& m_relaxation;

    Map<Var, double> m_changed_lower_bounds; // variable -> old_bound
    Map<Var, double> m_changed_upper_bounds;
    std::list<Ctr> m_added_constraints;


    void apply_variable_branching_decisions(const Node<NodeInfoT> &t_node,
                                            Map<Var, double> &t_changed_lower_bounds,
                                            Map<Var, double> &t_changed_upper_bounds);

    void apply_variable_branching_decision(const VarBranchingDecision& t_branching_decision,
                                           Map<Var, double> &t_current_changed_bounds,
                                           Map<Var, double> &t_changed_bounds);

    void apply_constraint_branching_decisions(const Node<NodeInfoT> &t_node);
public:
    explicit DefaultNodeUpdator(Model& t_relaxation);

    DefaultNodeUpdator(const DefaultNodeUpdator&) = delete;

    void prepare(const Node<NodeInfoT> &t_node) override;

    void clear() override;

protected:
    Model& relaxation() { return m_relaxation; }

    const Model& relaxation() const { return m_relaxation; }
};

template<class NodeInfoT>
void
idol::DefaultNodeUpdator<NodeInfoT>::apply_constraint_branching_decisions(const idol::Node<NodeInfoT> &t_node) {

    if (t_node.level() == 0) {
        return;
    }

    apply_constraint_branching_decisions(t_node.parent());

    for (const auto& [ctr, temporary_constraint] : t_node.info().constraint_branching_decisions()) {
        m_relaxation.add(ctr, temporary_constraint);
        m_added_constraints.emplace_back(ctr);
    }

}

template<class NodeT>
idol::DefaultNodeUpdator<NodeT>::DefaultNodeUpdator(idol::Model &t_relaxation) : m_relaxation(t_relaxation) {

}

template<class NodeInfoT>
void idol::DefaultNodeUpdator<NodeInfoT>::clear() {

    // Clear LB
    for (const auto& [var, lb] : m_changed_lower_bounds) {
        m_relaxation.set_var_lb(var, lb);
    }
    m_changed_lower_bounds.clear();

    // Clear UB
    for (const auto& [var, ub] : m_changed_upper_bounds) {
        m_relaxation.set_var_ub(var, ub);
    }
    m_changed_upper_bounds.clear();

    // Clear Ctr
    for (const auto& ctr : m_added_constraints) {
        m_relaxation.remove(ctr);
    }
    m_added_constraints.clear();
}

template<class NodeInfoT>
void idol::DefaultNodeUpdator<NodeInfoT>::prepare(const Node<NodeInfoT> &t_node) {

    Map<Var, double> changed_lower_bounds;
    Map<Var, double> changed_upper_bounds;

    apply_variable_branching_decisions(t_node, changed_lower_bounds, changed_upper_bounds);
    clear();
    m_changed_lower_bounds = changed_lower_bounds;
    m_changed_upper_bounds = changed_upper_bounds;

    apply_constraint_branching_decisions(t_node);
}

template<class NodeInfoT>
void idol::DefaultNodeUpdator<NodeInfoT>::apply_variable_branching_decisions(const idol::Node<NodeInfoT> &t_node,
                                                                                Map<Var, double> &t_changed_lower_bounds,
                                                                                Map<Var, double> &t_changed_upper_bounds
                                                                                ) {

    if (t_node.level() == 0) {
        return;
    }

    for (const auto& branching_decision : t_node.info().variable_branching_decisions()) {

        switch (branching_decision.type) {
            case LessOrEqual:
                apply_variable_branching_decision(branching_decision, m_changed_upper_bounds, t_changed_upper_bounds);
                break;
            case GreaterOrEqual:
                apply_variable_branching_decision(branching_decision, m_changed_lower_bounds, t_changed_lower_bounds);
                break;
            default:
                throw Exception("Branching on equality constraints is not implemented.");
        }

    }

    apply_variable_branching_decisions(t_node.parent(), t_changed_lower_bounds, t_changed_upper_bounds);

}

template<class NodeInfoT>
void idol::DefaultNodeUpdator<NodeInfoT>::apply_variable_branching_decision(const VarBranchingDecision &t_branching_decision,
                                                                               idol::Map<idol::Var, double> &t_current_changed_bounds,
                                                                               idol::Map<idol::Var, double> &t_changed_bounds) {

    const auto& var = t_branching_decision.variable;

    if (const auto it = t_changed_bounds.find(var) ; it != t_changed_bounds.end()) {
        return;
    }

    const double bound = t_branching_decision.bound;
    double original_bound;

    if (t_branching_decision.type == LessOrEqual) {

        original_bound = m_relaxation.get_var_ub(var);
        m_relaxation.set_var_ub(var, bound);

    } else {

        original_bound = m_relaxation.get_var_lb(var);
        m_relaxation.set_var_lb(var, bound);

    }

    if (const auto it = t_current_changed_bounds.find(var) ; it != t_current_changed_bounds.end()) {
        t_changed_bounds.emplace(*it);
        t_current_changed_bounds.erase(it);
    } else {
        t_changed_bounds.emplace(var, original_bound);
    }

}

#endif //IDOL_DEFAULTNODEUPDATOR_H
