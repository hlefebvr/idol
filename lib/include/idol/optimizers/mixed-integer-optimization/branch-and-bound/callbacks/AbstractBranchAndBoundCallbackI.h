//
// Created by henri on 12/04/23.
//

#ifndef IDOL_ABSTRACTBRANCHANDBOUNDCALLBACKI_H
#define IDOL_ABSTRACTBRANCHANDBOUNDCALLBACKI_H

#include "idol/modeling/solutions/Point.h"
#include "idol/optimizers/mixed-integer-optimization/callbacks/Callback.h"

namespace idol {

    template<class NodeInfoT>
    class BranchAndBoundCallback;

    namespace Optimizers {
        template<class NodeInfoT>
        class BranchAndBound;
    }

    struct SideEffectRegistry;

    template<class NodeInfoT>
    class AbstractBranchAndBoundCallbackI;

}

struct idol::SideEffectRegistry {
    unsigned int n_added_lazy_cuts = 0;
    unsigned int n_added_user_cuts = 0;
};

template<class NodeInfoT>
class idol::AbstractBranchAndBoundCallbackI {
public:
    virtual ~AbstractBranchAndBoundCallbackI() = default;
protected:
    virtual SideEffectRegistry operator()(
            Optimizers::BranchAndBound<NodeInfoT>* t_parent,
            CallbackEvent t_event,
            const Node<NodeInfoT>& t_current_node,
            Model* t_relaxation) = 0;

    virtual void add_callback(BranchAndBoundCallback<NodeInfoT>* t_cb) = 0;

    virtual void initialize(Optimizers::BranchAndBound<NodeInfoT>* t_parent) = 0;

    virtual void log_after_termination() = 0;

    friend class Optimizers::BranchAndBound<NodeInfoT>;
};

#endif //IDOL_ABSTRACTBRANCHANDBOUNDCALLBACKI_H
