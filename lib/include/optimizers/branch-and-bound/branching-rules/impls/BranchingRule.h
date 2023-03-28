//
// Created by henri on 21/03/23.
//

#ifndef IDOL_BRANCHINGRULE_H
#define IDOL_BRANCHINGRULE_H

#include "../../nodes/Node.h"

template<class NodeInfoT>
class BranchingRule {
    const Model& m_model;
public:
    explicit BranchingRule(const Model& t_model) : m_model(t_model) {}
    virtual ~BranchingRule() = default;

    [[nodiscard]] const Model& model() const { return m_model; }

    [[nodiscard]] virtual bool is_valid(const Node<NodeInfoT>& t_node) const = 0;

    [[nodiscard]] virtual std::list<NodeInfoT*> create_child_nodes(const Node<NodeInfoT>& t_node) = 0;
};

#endif //IDOL_BRANCHINGRULE_H
