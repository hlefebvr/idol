//
// Created by henri on 09.11.23.
//

#ifndef IDOL_IMPL_BRANCHINGWITHPRIORITY_H
#define IDOL_IMPL_BRANCHINGWITHPRIORITY_H


#include <list>
#include "BranchingRule.h"
#include "idol/modeling/variables/Var.h"
#include "idol/modeling/solutions/Solution.h"
#include "idol/optimizers/mixed-integer-programming/branch-and-bound/branching-rules/factories/BranchingRuleFactory.h"

namespace idol::BranchingRules {
    template<class>
    class BranchingWithPriority;
}

template<class NodeInfoT>
class idol::BranchingRules::BranchingWithPriority : public BranchingRule<NodeInfoT> {

    using ListOfBranchingRules = std::list<std::unique_ptr<BranchingRule<NodeInfoT>>>;
    using BranchingRuleIterator = typename ListOfBranchingRules::const_iterator;

    ListOfBranchingRules m_branching_rules;
public:
    explicit BranchingWithPriority(const Optimizers::BranchAndBound<NodeInfoT> &t_parent,
                                   const std::list<std::unique_ptr<::idol::BranchingRuleFactory<NodeInfoT>>>& t_factories)
        : BranchingRule<NodeInfoT>(t_parent)
    {
        for (auto& factory : t_factories) {
            m_branching_rules.emplace_back(factory->operator()(t_parent));
        }
    }

    void initialize() override {

        BranchingRule<NodeInfoT>::initialize();

        for (auto& branching_rule : m_branching_rules) {
            branching_rule->initialize();
        }

    }

    bool is_valid(const Node<NodeInfoT> &t_node) override {

        for (auto& branching_rule : m_branching_rules) {

            if (!branching_rule->is_valid(t_node)) {
                return false;
            }

        }

        return true;
    }

    std::list<NodeInfoT*> create_child_nodes(const Node<NodeInfoT> &t_node) override {

        for (auto& branching_rule : m_branching_rules) {

            auto result = branching_rule->create_child_nodes(t_node);

            if (!result.empty()) {
                return result;
            }

        }

        return {};
    }

};

#endif //IDOL_IMPL_BRANCHINGWITHPRIORITY_H
