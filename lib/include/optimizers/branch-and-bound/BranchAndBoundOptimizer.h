//
// Created by henri on 21/03/23.
//

#ifndef IDOL_BRANCHANDBOUNDOPTIMIZER_H
#define IDOL_BRANCHANDBOUNDOPTIMIZER_H

#include <memory>
#include "optimizers/OptimizerFactory.h"
#include "BranchAndBound.h"
#include "optimizers/branch-and-bound/nodes/NodeInfo.h"

template<class NodeT = NodeInfo>
class BranchAndBoundOptimizer : public OptimizerFactory {
    std::unique_ptr<OptimizerFactory> m_relaxation_optimizer_factory;
    std::unique_ptr<BranchingRuleFactory<NodeT>> m_branching_rule_factory;
    std::unique_ptr<NodeSelectionRuleFactory<NodeT>> m_node_selection_rule_factory;
protected:
    BranchAndBoundOptimizer(const BranchAndBoundOptimizer& t_rhs);
public:
    BranchAndBoundOptimizer(const OptimizerFactory& t_relaxation_optimizer_factory,
                                     const BranchingRuleFactory<NodeT>& t_branching_rule_factory,
                                     const NodeSelectionRuleFactory<NodeT>& t_node_selection_rule_factory);

    template<class BranchingRuleFactoryT, class NodeSelectionRuleFactoryT>
    BranchAndBoundOptimizer(const OptimizerFactory& t_relaxation_optimizer_factory,
                                     const BranchingRuleFactoryT& t_branching_rule_factory,
                                     const NodeSelectionRuleFactoryT& t_node_selection_rule_factory);

    BranchAndBoundOptimizer(BranchAndBoundOptimizer&&) noexcept = delete;
    BranchAndBoundOptimizer& operator=(const BranchAndBoundOptimizer&) = delete;
    BranchAndBoundOptimizer& operator=(BranchAndBoundOptimizer&&) noexcept = delete;

    Backend *operator()(const AbstractModel &t_model) const override;

    [[nodiscard]] OptimizerFactory *clone() const override;
};

template<class NodeT>
template<class BranchingRuleFactoryT, class NodeSelectionRuleFactoryT>
BranchAndBoundOptimizer<NodeT>::BranchAndBoundOptimizer(const OptimizerFactory &t_relaxation_optimizer_factory,
                                                        const BranchingRuleFactoryT &t_branching_rule_factory,
                                                        const NodeSelectionRuleFactoryT &t_node_selection_rule_factory)

        : m_relaxation_optimizer_factory(t_relaxation_optimizer_factory.clone()),
          m_branching_rule_factory(new typename BranchingRuleFactoryT::template Strategy<NodeT>(t_branching_rule_factory)),
          m_node_selection_rule_factory(new typename NodeSelectionRuleFactoryT::template Strategy<NodeT>(t_node_selection_rule_factory)) {}

template<class NodeT>
BranchAndBoundOptimizer<NodeT>::BranchAndBoundOptimizer(const OptimizerFactory &t_relaxation_optimizer_factory,
                                                        const BranchingRuleFactory<NodeT> &t_branching_rule_factory,
                                                        const NodeSelectionRuleFactory<NodeT> &t_node_selection_rule_factory)

        : m_relaxation_optimizer_factory(t_relaxation_optimizer_factory.clone()),
          m_branching_rule_factory(t_branching_rule_factory.clone()),
          m_node_selection_rule_factory(t_node_selection_rule_factory.clone()) {}

template<class NodeT>
BranchAndBoundOptimizer<NodeT>::BranchAndBoundOptimizer(const BranchAndBoundOptimizer &t_rhs)

        : m_relaxation_optimizer_factory(t_rhs.m_relaxation_optimizer_factory->clone()),
          m_branching_rule_factory(t_rhs.m_branching_rule_factory->clone()),
          m_node_selection_rule_factory(t_rhs.m_node_selection_rule_factory->clone()) {}

template<class NodeT>
Backend *BranchAndBoundOptimizer<NodeT>::operator()(const AbstractModel &t_model) const {
    return new Backends::BranchAndBound<NodeT>(t_model,
                                               *m_relaxation_optimizer_factory,
                                               *m_branching_rule_factory,
                                               *m_node_selection_rule_factory);
}

template<class NodeT>
OptimizerFactory *BranchAndBoundOptimizer<NodeT>::clone() const {
    return new BranchAndBoundOptimizer(*this);
}

#endif //IDOL_BRANCHANDBOUNDOPTIMIZER_H
