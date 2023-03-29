//
// Created by henri on 21/03/23.
//

#ifndef IDOL_NODESELECTIONRULEFACTORY_H
#define IDOL_NODESELECTIONRULEFACTORY_H

template<class NodeT> class NodeSelectionRule;

template<class NodeT>
class NodeSelectionRuleFactory {
public:
    virtual ~NodeSelectionRuleFactory() = default;

    virtual NodeSelectionRule<NodeT>* operator()() const = 0;

    virtual NodeSelectionRuleFactory* clone() const = 0;
};

#endif //IDOL_NODESELECTIONRULEFACTORY_H
