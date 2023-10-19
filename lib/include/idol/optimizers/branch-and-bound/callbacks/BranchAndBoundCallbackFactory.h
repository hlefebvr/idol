//
// Created by henri on 30/03/23.
//

#ifndef IDOL_BRANCHANDBOUNDCALLBACKFACTORY_H
#define IDOL_BRANCHANDBOUNDCALLBACKFACTORY_H

namespace idol {
    template<class NodeVarInfoT>
    class BranchAndBoundCallback;

    template<class NodeVarInfoT>
    class BranchAndBoundCallbackFactory;
}

template<class NodeVarInfoT>
class idol::BranchAndBoundCallbackFactory {
public:
    virtual ~BranchAndBoundCallbackFactory() = default;

    virtual BranchAndBoundCallback<NodeVarInfoT>* operator()() = 0;

    virtual BranchAndBoundCallbackFactory<NodeVarInfoT>* clone() const = 0;
};

#endif //IDOL_BRANCHANDBOUNDCALLBACKFACTORY_H
