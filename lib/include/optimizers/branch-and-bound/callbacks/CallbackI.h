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
    IncumbentSolutionFound,
    InvalidSolutionFound
};

static std::ostream &operator<<(std::ostream& t_os, BranchAndBoundEvent t_event) {
    switch (t_event) {
        case IncumbentSolutionFound: return t_os << "IncumbentSolutionFound";
        case InvalidSolutionFound: return t_os << "InvalidSolutionFound";
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
};

#endif //IDOL_CALLBACKI_H
