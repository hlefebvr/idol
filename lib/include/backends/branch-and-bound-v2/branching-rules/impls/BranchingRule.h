//
// Created by henri on 21/03/23.
//

#ifndef IDOL_BRANCHINGRULE_H
#define IDOL_BRANCHINGRULE_H

#include "../../nodes/Node2.h"

template<class NodeInfoT>
class BranchingRule {
    const AbstractModel& m_model;
public:
    explicit BranchingRule(const AbstractModel& t_model) : m_model(t_model) {}
    virtual ~BranchingRule() = default;

    [[nodiscard]] const AbstractModel& model() const { return m_model; }

    [[nodiscard]] virtual bool is_valid(const Node2<NodeInfoT>& t_node) const = 0;

    [[nodiscard]] virtual std::list<NodeInfoT*> create_child_nodes(const Node2<NodeInfoT>& t_node) = 0;
};

#endif //IDOL_BRANCHINGRULE_H
