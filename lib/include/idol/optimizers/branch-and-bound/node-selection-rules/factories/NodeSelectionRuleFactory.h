//
// Created by henri on 21/03/23.
//

#ifndef IDOL_NODESELECTIONRULEFACTORY_H
#define IDOL_NODESELECTIONRULEFACTORY_H

namespace idol {
    template<class NodeT>
    class NodeSelectionRule;

    template<class NodeT>
    class NodeSelectionRuleFactory;

    namespace Optimizers {
        template<class NodeT>
        class BranchAndBound;
    }
}

template<class NodeT>
class idol::NodeSelectionRuleFactory {
public:
    virtual ~NodeSelectionRuleFactory() = default;

    virtual NodeSelectionRule<NodeT>* operator()(Optimizers::BranchAndBound<NodeT>& t_parent) const = 0;

    virtual NodeSelectionRuleFactory* clone() const = 0;
};

#endif //IDOL_NODESELECTIONRULEFACTORY_H
