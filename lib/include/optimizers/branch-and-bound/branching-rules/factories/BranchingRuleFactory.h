//
// Created by henri on 21/03/23.
//

#ifndef IDOL_BRANCHINGRULEFACTORY_H
#define IDOL_BRANCHINGRULEFACTORY_H

namespace idol {
    template<class NodeT>
    class BranchingRule;

    template<class NodeT>
    class BranchingRuleFactory;
}
template<class NodeT>
class idol::BranchingRuleFactory {
public:
    virtual ~BranchingRuleFactory() = default;

    virtual BranchingRule<NodeT>* operator()(const Model& t_model) const = 0;

    virtual BranchingRuleFactory* clone() const = 0;
};

#endif //IDOL_BRANCHINGRULEFACTORY_H
