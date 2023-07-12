//
// Created by henri on 30/03/23.
//

#ifndef IDOL_BRANCHANDBOUNDCALLBACKFACTORY_H
#define IDOL_BRANCHANDBOUNDCALLBACKFACTORY_H

namespace idol {
    template<class NodeInfoT>
    class BranchAndBoundCallback;

    template<class NodeInfoT>
    class BranchAndBoundCallbackFactory;
}

template<class NodeInfoT>
class idol::BranchAndBoundCallbackFactory {
public:
    virtual ~BranchAndBoundCallbackFactory() = default;

    virtual BranchAndBoundCallback<NodeInfoT>* operator()() = 0;

    virtual BranchAndBoundCallbackFactory<NodeInfoT>* clone() const = 0;
};

#endif //IDOL_BRANCHANDBOUNDCALLBACKFACTORY_H
