//
// Created by henri on 30/03/23.
//

#ifndef IDOL_CALLBACKI_H
#define IDOL_CALLBACKI_H

#include <ostream>
#include "errors/Exception.h"
#include "optimizers/branch-and-bound/nodes/Node.h"

class Model;

namespace Optimizers {
    template<class NodeInfoT> class BranchAndBound;
}

enum BranchAndBoundEvent {
    NodeLoaded, /* Called when a node is about to be solved */
    IncumbentSolution, /* Called when an incumbent solution has been found */
    InvalidSolution, /* Called when a solution of the relaxation is not valid (e.g., not integer) */
    OptimalSolution /* Called at the end of the execution, if an optimal solution has been found */
};

static std::ostream &operator<<(std::ostream& t_os, BranchAndBoundEvent t_event) {
    switch (t_event) {
        case IncumbentSolution: return t_os << "IncumbentSolution";
        case InvalidSolution: return t_os << "InvalidSolution";
        case OptimalSolution: return t_os << "OptimalSolution";
        case NodeLoaded: return t_os << "NodeLoaded";
        default:;
    }
    throw Exception("Enum out of bounds.");
}

template<class NodeInfoT>
class CallbackI {
    friend class Optimizers::BranchAndBound<NodeInfoT>;

    virtual void set_node(Node<NodeInfoT>* t_node) = 0;
    virtual void set_relaxation(Model* t_relaxation) = 0;
    virtual void set_parent(Optimizers::BranchAndBound<NodeInfoT>* t_parent) = 0;
protected:
    virtual void operator()(BranchAndBoundEvent t_event) = 0;
public:
    virtual ~CallbackI() = default;
};

#endif //IDOL_CALLBACKI_H
