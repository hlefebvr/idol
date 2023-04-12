//
// Created by henri on 12/04/23.
//

#ifndef IDOL_ABSTRACTBABCALLBACKI_H
#define IDOL_ABSTRACTBABCALLBACKI_H

#include "modeling/solutions/Solution.h"
#include "optimizers/callbacks/Callback.h"

template<class NodeInfoT>
class BaBCallback;

namespace Optimizers {
    template<class NodeInfoT> class BranchAndBound;
}

struct SideEffectRegistry {
    bool relaxation_was_modified = false;
};

template<class NodeInfoT>
class AbstractBaBCallbackI {
public:
    virtual ~AbstractBaBCallbackI() = default;
protected:
    virtual SideEffectRegistry operator()(
            Optimizers::BranchAndBound<NodeInfoT>* t_parent,
            CallbackEvent t_event,
            Node<NodeInfoT>* t_current_node,
            Model* t_relaxation) = 0;

    virtual void add_callback(BaBCallback<NodeInfoT>* t_cb) = 0;

    friend class Optimizers::BranchAndBound<NodeInfoT>;
};

#endif //IDOL_ABSTRACTBABCALLBACKI_H
