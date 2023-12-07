//
// Created by henri on 09.11.23.
//

#ifndef IDOL_BRANCHINGWITHPRIORITY_H
#define IDOL_BRANCHINGWITHPRIORITY_H

#include "VariableBranching.h"
#include "idol/optimizers/branch-and-bound/branching-rules/impls/LeastInfeasible.h"
#include "idol/optimizers/branch-and-bound/branching-rules/impls/BranchingWithPriority.h"

namespace idol {
    class DefaultNodeInfo;

    template<class NodeT>
    class BranchingWithPriority;
}

template<class NodeT = idol::DefaultNodeInfo>
class idol::BranchingWithPriority : public BranchingRuleFactory<NodeT> {
    std::list<std::unique_ptr<BranchingRuleFactory<NodeT>>> m_branching_rules;
public:
    BranchingWithPriority() = default;

    BranchingWithPriority(const BranchingWithPriority& t_src) {

        for (auto& branching_rule : t_src.m_branching_rules) {
            m_branching_rules.emplace_back(branching_rule->clone());
        }

    }

    /**
     * This type is used to exploit [SFINAE](https://en.wikipedia.org/wiki/Substitution_failure_is_not_an_error)
     * in order to identify classes having a sub-class named `Strategy<NodeInfoT>`.
     * This is used to make calls like `.with_node_selection_rule(DepthFirst());` which will actually call
     * `.with_node_selection_rule(DepthFirst::Strategy<NodeInfoT>())`.
     */
    template<class ReturnT, class T> using only_if_has_Strategy = typename std::pair<typename T::template Strategy<NodeT>, ReturnT>::second_type;

    BranchingWithPriority<NodeT>& add_branching_rule(const BranchingRuleFactory<NodeT>& t_branching_rule) {
        m_branching_rules.emplace_back(t_branching_rule.clone());
        return *this;
    }

    template<class BranchingRuleFactoryT>
    only_if_has_Strategy<BranchingWithPriority<NodeT>&, BranchingRuleFactoryT> add_branching_rule(const BranchingRuleFactoryT& t_branching_rule) {
        return add_branching_rule(typename BranchingRuleFactoryT::template Strategy<NodeT>(t_branching_rule));
    }

    BranchingRule<NodeT> *operator()(const Optimizers::BranchAndBound<NodeT> &t_parent) const override {
        return new BranchingRules::BranchingWithPriority<NodeT>(t_parent, m_branching_rules);
    }

    BranchingRuleFactory<NodeT> *clone() const override {
        return new BranchingWithPriority<NodeT>(*this);
    }
};

#endif //IDOL_BRANCHINGWITHPRIORITY_H
