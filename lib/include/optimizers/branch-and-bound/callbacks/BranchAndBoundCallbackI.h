//
// Created by henri on 30/03/23.
//

#ifndef IDOL_BRANCHANDBOUNDCALLBACKI_H
#define IDOL_BRANCHANDBOUNDCALLBACKI_H

#include <ostream>
#include "errors/Exception.h"
#include "optimizers/branch-and-bound/nodes/Node.h"
#include "optimizers/callbacks/Callback.h"

class Model;

namespace Optimizers {
    template<class NodeInfoT> class BranchAndBound;
}

template<class NodeInfoT>
class BranchAndBoundCallbackI {
    friend class Optimizers::BranchAndBound<NodeInfoT>;

    virtual void operator()(BranchAndBoundEvent t_event) = 0;
public:
    virtual ~BranchAndBoundCallbackI() = default;

    struct SideEffectRegistry {
        bool relaxation_was_modified = false;
    };

private:
    virtual void set_node(Node<NodeInfoT>* t_node) = 0;
    virtual void set_relaxation(Model* t_relaxation) = 0;
    virtual void set_parent(Optimizers::BranchAndBound<NodeInfoT>* t_parent) = 0;
    virtual void set_side_effect_registry(SideEffectRegistry* t_registry) = 0;
};

#endif //IDOL_BRANCHANDBOUNDCALLBACKI_H
