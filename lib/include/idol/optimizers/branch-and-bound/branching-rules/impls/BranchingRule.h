//
// Created by henri on 21/03/23.
//

#ifndef IDOL_BRANCHINGRULE_H
#define IDOL_BRANCHINGRULE_H

#include "idol/optimizers/branch-and-bound/nodes/Node.h"
#include "idol/optimizers/Logger.h"
#include <list>

namespace idol {
    namespace Optimizers {
        template<class NodeInfoT>
        class BranchAndBound;
    }

    template<class NodeInfoT>
    class BranchingRule;
}

template<class NodeInfoT>
class idol::BranchingRule {
    friend class Optimizers::BranchAndBound<NodeInfoT>;
    const Optimizers::BranchAndBound<NodeInfoT>& m_parent;
    LogLevel m_log_level = Warn;
    Color m_log_color = Default;
protected:
    [[nodiscard]] LogLevel log_level() const { return m_log_level; }
    [[nodiscard]] Color log_color() const { return m_log_color; }
    [[nodiscard]] std::string name() const { return "branching-rule"; }
public:
    explicit BranchingRule(const Optimizers::BranchAndBound<NodeInfoT>& t_parent) : m_parent(t_parent) {}
    virtual ~BranchingRule() = default;

    [[nodiscard]] const Optimizers::BranchAndBound<NodeInfoT>& parent() const { return m_parent; }

    [[nodiscard]] const Model& model() const { return m_parent.parent(); }

    virtual void initialize() {}

    [[nodiscard]] virtual bool is_valid(const Node<NodeInfoT>& t_node) const = 0;

    [[nodiscard]] virtual std::list<NodeInfoT*> create_child_nodes(const Node<NodeInfoT>& t_node) = 0;

    virtual void on_node_solved(const Node<NodeInfoT>& t_node) {}

    virtual void on_nodes_have_been_created() {}

    template<class T> T& as() {
        auto* result = dynamic_cast<T*>(this);
        if (!result) {
            throw Exception("Bad cast.");
        }
        return *result;
    }

    template<class T> const T& as() const {
        auto* result = dynamic_cast<const T*>(this);
        if (!result) {
            throw Exception("Bad cast.");
        }
        return *result;
    }

    template<class T> [[nodiscard]] bool is() const {
        return dynamic_cast<const T*>(this);
    }
};

#endif //IDOL_BRANCHINGRULE_H
