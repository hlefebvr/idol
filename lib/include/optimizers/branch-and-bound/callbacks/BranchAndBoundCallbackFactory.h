//
// Created by henri on 30/03/23.
//

#ifndef IDOL_BRANCHANDBOUNDCALLBACKFACTORY_H
#define IDOL_BRANCHANDBOUNDCALLBACKFACTORY_H

template<class NodeInfoT> class BranchAndBoundCallback;

template<class NodeInfoT>
class BranchAndBoundCallbackFactory {
public:
    virtual ~BranchAndBoundCallbackFactory() = default;

    virtual BranchAndBoundCallback<NodeInfoT>* operator()() = 0;

    virtual BranchAndBoundCallbackFactory<NodeInfoT>* clone() const = 0;
};

#endif //IDOL_BRANCHANDBOUNDCALLBACKFACTORY_H
