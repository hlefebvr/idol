//
// Created by henri on 12/04/23.
//

#ifndef IDOL_ABSTRACTBRANCHANDBOUNDCALLBACKI_H
#define IDOL_ABSTRACTBRANCHANDBOUNDCALLBACKI_H

#include "modeling/solutions/Solution.h"
#include "optimizers/callbacks/Callback.h"

namespace idol {

    template<class NodeInfoT>
    class BranchAndBoundCallback;

    class CuttingPlaneGenerator;

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

    virtual void add_cutting_plane_generator(const CuttingPlaneGenerator& t_cutting_plane_generator) = 0;

    virtual void initialize(const Model& t_model) = 0;

    friend class Optimizers::BranchAndBound<NodeInfoT>;
};

#endif //IDOL_ABSTRACTBRANCHANDBOUNDCALLBACKI_H
