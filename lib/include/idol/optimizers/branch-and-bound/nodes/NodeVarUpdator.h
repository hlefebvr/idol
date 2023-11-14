//
// Created by henri on 18.10.23.
//

#ifndef IDOL_NODEVARUPDATOR_H
#define IDOL_NODEVARUPDATOR_H

#include "NodeUpdator.h"
#include "idol/modeling/models/Model.h"
#include "Node.h"

namespace idol {
    template<class NodeT>
    class NodeVarUpdator;
}

template<class NodeVarInfoT>
class idol::NodeVarUpdator : public NodeUpdator<NodeVarInfoT> {

    Model& m_relaxation;

    Map<Var, double> m_changed_lower_bounds; // variable -> old_bound
    Map<Var, double> m_changed_upper_bounds;


    void apply_local_updates(const Node<NodeVarInfoT>& t_node,
                             Map<Var, double> &t_changed_lower_bounds,
                             Map<Var, double> &t_changed_upper_bounds);

    void apply_local_update(const typename NodeVarInfoT::BranchingDecision& t_branching_decision,
                             Map<Var, double> &t_current_changed_bounds,
                             Map<Var, double> &t_changed_bounds);
public:
    explicit NodeVarUpdator(Model& t_relaxation);

    NodeVarUpdator(const NodeVarUpdator&) = delete;

    void apply_local_updates(const Node<NodeVarInfoT> &t_node) override;

    void clear_local_updates() override;

protected:
    Model& relaxation() { return m_relaxation; }

    const Model& relaxation() const { return m_relaxation; }
};

template<class NodeT>
idol::NodeVarUpdator<NodeT>::NodeVarUpdator(idol::Model &t_relaxation) : m_relaxation(t_relaxation) {

}

template<class NodeVarInfoT>
void idol::NodeVarUpdator<NodeVarInfoT>::clear_local_updates() {

    for (const auto& [var, lb] : m_changed_lower_bounds) {
        m_relaxation.set_var_lb(var, lb);
    }
    m_changed_lower_bounds.clear();

    for (const auto& [var, ub] : m_changed_upper_bounds) {
        m_relaxation.set_var_ub(var, ub);
    }
    m_changed_upper_bounds.clear();

}

template<class NodeVarInfoT>
void idol::NodeVarUpdator<NodeVarInfoT>::apply_local_updates(const Node<NodeVarInfoT> &t_node) {

    Map<Var, double> changed_lower_bounds;
    Map<Var, double> changed_upper_bounds;

    apply_local_updates(t_node, changed_lower_bounds, changed_upper_bounds);
    clear_local_updates();
    m_changed_lower_bounds = changed_lower_bounds;
    m_changed_upper_bounds = changed_upper_bounds;

}

template<class NodeVarInfoT>
void idol::NodeVarUpdator<NodeVarInfoT>::apply_local_updates(const idol::Node<NodeVarInfoT> &t_node,
                                                          Map<Var, double> &t_changed_lower_bounds,
                                                          Map<Var, double> &t_changed_upper_bounds
                                                          ) {

    if (t_node.level() == 0) {
        return;
    }

    if (t_node.info().has_branching_decision()) {

        const auto &branching_decision = t_node.info().branching_decision();

        switch (branching_decision.type) {
            case LessOrEqual:
                apply_local_update(branching_decision, m_changed_upper_bounds, t_changed_upper_bounds);
                break;
            case GreaterOrEqual:
                apply_local_update(branching_decision, m_changed_lower_bounds, t_changed_lower_bounds);
                break;
            default:
                throw Exception("Branching on equality constraints is not implemented.");
        }

    }

    apply_local_updates(t_node.parent(), t_changed_lower_bounds, t_changed_upper_bounds);

}

template<class NodeVarInfoT>
void idol::NodeVarUpdator<NodeVarInfoT>::apply_local_update(const typename NodeVarInfoT::BranchingDecision &t_branching_decision,
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

#endif //IDOL_NODEVARUPDATOR_H
