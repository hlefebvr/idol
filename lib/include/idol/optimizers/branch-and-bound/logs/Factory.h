//
// Created by henri on 23.11.23.
//

#ifndef IDOL_BRANCHADNDBOUND_LOGGERFACTORY_H
#define IDOL_BRANCHADNDBOUND_LOGGERFACTORY_H

#include "idol/optimizers/branch-and-bound/nodes/DefaultNodeInfo.h"

namespace idol {
    namespace Optimizers {
        template<class NodeInfoT> class BranchAndBound;
    }
    namespace Logs::BranchAndBound {
        template<class NodeInfoT> class Factory;
    }
}

template<class NodeInfoT = idol::DefaultNodeInfo>
class idol::Logs::BranchAndBound::Factory {
public:
    virtual ~Factory() = default;

    class Strategy {
        Optimizers::BranchAndBound<NodeInfoT>& m_parent;
    protected:
        Optimizers::BranchAndBound<NodeInfoT>& parent() { return m_parent; }
        const Optimizers::BranchAndBound<NodeInfoT>& parent() const { return m_parent; }
    public:
        Strategy(Optimizers::BranchAndBound<NodeInfoT>& t_parent) : m_parent(t_parent) {}

        virtual ~Strategy() = default;

        virtual void initialize() {}

        virtual void log_node_after_solve(const Node<NodeInfoT>& t_node) {}

        virtual void log_after_termination() {}
    };

    virtual Strategy* operator()(Optimizers::BranchAndBound<NodeInfoT>& t_parent) const = 0;

    virtual Factory* clone() const = 0;
};

#endif //IDOL_BRANCHADNDBOUND_LOGGERFACTORY_H
