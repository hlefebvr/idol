//
// Created by henri on 21/03/23.
//

#ifndef IDOL_BRANCHANDBOUND_H
#define IDOL_BRANCHANDBOUND_H

#include <memory>
#include "optimizers/OptimizerFactory.h"
#include "Optimizers_BranchAndBound.h"
#include "optimizers/branch-and-bound/nodes/NodeInfo.h"
#include "optimizers/branch-and-bound/callbacks/Callback.h"

template<class NodeT = NodeInfo>
class BranchAndBound : public OptimizerFactoryWithDefaultParameters<BranchAndBound<NodeT>> {
    std::unique_ptr<OptimizerFactory> m_relaxation_optimizer_factory;
    std::unique_ptr<BranchingRuleFactory<NodeT>> m_branching_rule_factory;
    std::unique_ptr<NodeSelectionRuleFactory<NodeT>> m_node_selection_rule_factory;

    std::list<Callback<NodeT>*> m_callbacks;

    std::optional<unsigned int> m_subtree_depth;
    std::optional<unsigned int> m_log_frequency;
protected:
    BranchAndBound(const BranchAndBound& t_rhs);
public:
    /* The following is used to exploit SFINAE in order to detect when a class has a sub-class named Strategy<NodeT> */
    template<class ReturnT, class T> using only_if_has_Strategy = typename std::pair<typename T::template Strategy<NodeT>, ReturnT>::second_type;

    BranchAndBound() = default;

    BranchAndBound<NodeT>& with_node_solver(const OptimizerFactory& t_node_solver);

    BranchAndBound<NodeT>& with_branching_rule(const BranchingRuleFactory<NodeT>& t_branching_rule);

    template<class BranchingRuleFactoryT>
    only_if_has_Strategy<BranchAndBound<NodeT>&, BranchingRuleFactoryT> with_branching_rule(const BranchingRuleFactoryT& t_branching_rule);

    BranchAndBound<NodeT>& with_node_selection_rule(const NodeSelectionRuleFactory<NodeT>& t_node_selection);

    template<class NodeSelectionRuleFactoryT>
    only_if_has_Strategy<BranchAndBound<NodeT>&, NodeSelectionRuleFactoryT> with_node_selection_rule(const NodeSelectionRuleFactoryT& t_node_selection_rule);

    BranchAndBound(BranchAndBound&&) noexcept = delete;
    BranchAndBound& operator=(const BranchAndBound&) = delete;
    BranchAndBound& operator=(BranchAndBound&&) noexcept = delete;

    Optimizer *operator()(const Model &t_model) const override;

    [[nodiscard]] OptimizerFactory *clone() const override;

    BranchAndBound<NodeT>& with_subtree_depth(unsigned int t_depth);

    BranchAndBound<NodeT>& with_log_frequency(unsigned int t_log_frequency);

    BranchAndBound<NodeT>& with_callback(Callback<NodeT>* t_callback);
};

template<class NodeT>
BranchAndBound<NodeT> &BranchAndBound<NodeT>::with_callback(Callback<NodeT> *t_callback) {

    m_callbacks.emplace_back(t_callback);

    return *this;
}

template<class NodeT>
BranchAndBound<NodeT> &BranchAndBound<NodeT>::with_log_frequency(unsigned int t_log_frequency) {

    if (m_log_frequency.has_value()) {
        throw Exception("A log frequency has already been given.");
    }

    m_log_frequency = t_log_frequency;

    return *this;
}

template<class NodeT>
BranchAndBound<NodeT> &BranchAndBound<NodeT>::with_subtree_depth(unsigned int t_depth) {

    if (m_subtree_depth.has_value()) {
        throw Exception("A subtree depth has already been given");
    }

    m_subtree_depth = t_depth;

    return *this;
}

template<class NodeT>
template<class NodeSelectionRuleFactoryT>
typename BranchAndBound<NodeT>::template only_if_has_Strategy<BranchAndBound<NodeT>&, NodeSelectionRuleFactoryT>
BranchAndBound<NodeT>::with_node_selection_rule(const NodeSelectionRuleFactoryT &t_node_selection_rule) {
    return with_node_selection_rule(typename NodeSelectionRuleFactoryT::template Strategy<NodeT>(t_node_selection_rule));
}

template<class NodeT>
BranchAndBound<NodeT> &
BranchAndBound<NodeT>::with_node_selection_rule(const NodeSelectionRuleFactory<NodeT> &t_node_selection) {

    if (m_node_selection_rule_factory) {
        throw Exception("A node selection rule has already been set.");
    }

    m_node_selection_rule_factory.reset(t_node_selection.clone());

    return *this;
}

template<class NodeT>
template<class BranchingRuleFactoryT>
typename BranchAndBound<NodeT>::template only_if_has_Strategy<BranchAndBound<NodeT>&, BranchingRuleFactoryT>
BranchAndBound<NodeT>::with_branching_rule(const BranchingRuleFactoryT &t_branching_rule) {
    return with_branching_rule(typename BranchingRuleFactoryT::template Strategy<NodeT>(t_branching_rule));
}

template<class NodeT>
BranchAndBound<NodeT> &BranchAndBound<NodeT>::with_branching_rule(const BranchingRuleFactory<NodeT> &t_branching_rule) {

    if (m_branching_rule_factory) {
        throw Exception("A branching rule has already been set.");
    }

    m_branching_rule_factory.reset(t_branching_rule.clone());

    return *this;
}

template<class NodeT>
BranchAndBound<NodeT> &BranchAndBound<NodeT>::with_node_solver(const OptimizerFactory &t_node_solver) {

    if (m_relaxation_optimizer_factory) {
        throw Exception("A node solver has already been set.");
    }

    m_relaxation_optimizer_factory.reset(t_node_solver.clone());

    return *this;
}

template<class NodeT>
BranchAndBound<NodeT>::BranchAndBound(const BranchAndBound &t_rhs)

        : OptimizerFactoryWithDefaultParameters<BranchAndBound<NodeT>>(t_rhs),
          m_relaxation_optimizer_factory(t_rhs.m_relaxation_optimizer_factory ? t_rhs.m_relaxation_optimizer_factory->clone() : nullptr),
          m_branching_rule_factory(t_rhs.m_branching_rule_factory ? t_rhs.m_branching_rule_factory->clone() : nullptr),
          m_node_selection_rule_factory(t_rhs.m_node_selection_rule_factory ? t_rhs.m_node_selection_rule_factory->clone() : nullptr) {}

template<class NodeT>
Optimizer *BranchAndBound<NodeT>::operator()(const Model &t_model) const {

    if (!m_relaxation_optimizer_factory) {
        throw Exception("No node solver has been given, please call BranchAndBound::with_node_solver to configure.");
    }

    if (!m_branching_rule_factory) {
        throw Exception("No branching rule has been given, please call BranchAndBound::with_branching_rule to configure.");
    }

    if (!m_node_selection_rule_factory) {
        throw Exception("No node selection rule has been given, please call BranchAndBound::with_node_selection_rule to configure.");
    }

    auto* result = new Optimizers::BranchAndBound<NodeT>(t_model,
                                     *m_relaxation_optimizer_factory,
                                     *m_branching_rule_factory,
                                     *m_node_selection_rule_factory);

    this->handle_default_parameters(result);

    if (m_log_frequency.has_value()) {
        result->set_log_frequency(m_log_frequency.value());
    }

    if (m_subtree_depth.has_value()) {
        result->set_subtree_depth(m_subtree_depth.value());
    }

    for (auto* cb : m_callbacks) {
        result->add_callback(cb);
    }

    return result;
}

template<class NodeT>
OptimizerFactory *BranchAndBound<NodeT>::clone() const {
    return new BranchAndBound(*this);
}

#endif //IDOL_BRANCHANDBOUND_H
