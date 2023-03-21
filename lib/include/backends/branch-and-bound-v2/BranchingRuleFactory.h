//
// Created by henri on 21/03/23.
//

#ifndef IDOL_BRANCHINGRULEFACTORY_H
#define IDOL_BRANCHINGRULEFACTORY_H

template<class NodeT>
class BranchingRuleFactory {
public:
    virtual ~BranchingRuleFactory() = default;

    virtual BranchingRuleFactory* clone() const = 0;
};

#endif //IDOL_BRANCHINGRULEFACTORY_H
