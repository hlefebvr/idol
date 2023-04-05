//
// Created by henri on 21/03/23.
//

#ifndef IDOL_BRANCHINGRULE_H
#define IDOL_BRANCHINGRULE_H

#include "../../nodes/Node.h"

namespace Optimizers {
    template<class NodeInfoT> class BranchAndBound;
}

template<class NodeInfoT>
class BranchingRule {
    friend class Optimizers::BranchAndBound<NodeInfoT>;
    const Model& m_model;
    LogLevel m_log_level = Warn;
    Color m_log_color = Default;
protected:
    [[nodiscard]] LogLevel log_level() const { return m_log_level; }
    [[nodiscard]] Color log_color() const { return m_log_color; }
    [[nodiscard]] std::string name() const { return "branching-rule"; }
public:
    explicit BranchingRule(const Model& t_model) : m_model(t_model) {}
    virtual ~BranchingRule() = default;

    [[nodiscard]] const Model& model() const { return m_model; }

    [[nodiscard]] virtual bool is_valid(const Node<NodeInfoT>& t_node) const = 0;

    [[nodiscard]] virtual std::list<NodeInfoT*> create_child_nodes(const Node<NodeInfoT>& t_node) = 0;
};

#endif //IDOL_BRANCHINGRULE_H
