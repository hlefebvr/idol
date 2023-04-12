//
// Created by henri on 21/03/23.
//

#ifndef IDOL_BRANCHANDBOUND_H
#define IDOL_BRANCHANDBOUND_H

#include <memory>
#include "optimizers/OptimizerFactory.h"
#include "Optimizers_BranchAndBound.h"
#include "optimizers/branch-and-bound/nodes/NodeInfo.h"
#include "optimizers/branch-and-bound/callbacks/BranchAndBoundCallbackFactory.h"
#include "optimizers/branch-and-bound/callbacks/BranchAndBoundCallback.h"

template<class NodeT = NodeInfo>
class BranchAndBound : public OptimizerFactoryWithDefaultParameters<BranchAndBound<NodeT>> {
    std::unique_ptr<OptimizerFactory> m_relaxation_optimizer_factory;
    std::unique_ptr<BranchingRuleFactory<NodeT>> m_branching_rule_factory;
    std::unique_ptr<NodeSelectionRuleFactory<NodeT>> m_node_selection_rule_factory;

    std::list<std::unique_ptr<BranchAndBoundCallbackFactory<NodeT>>> m_callbacks;

    std::optional<unsigned int> m_subtree_depth;
    std::optional<unsigned int> m_log_frequency;
protected:
    BranchAndBound(const BranchAndBound& t_rhs);
public:
    /**
     * This type is used to exploit SFINAE in order to detect when a class has a sub-class named Strategy<NodeT>.
     * This is used to make calls like ".with_node_selection_rule(DepthFirst());" which will actually calls
     * DepthFirst::Strategy<NodeT>().
     */
    template<class ReturnT, class T> using only_if_has_Strategy = typename std::pair<typename T::template Strategy<NodeT>, ReturnT>::second_type;

    BranchAndBound() = default;

    /**
     * Configures the optimizer factory for solving the branch-and-bound tree nodes
     * @param t_node_solver the optimizer factory the node problems
     * @return the optimizer factory itself
     */
    BranchAndBound<NodeT>& with_node_solver(const OptimizerFactory& t_node_solver);

    /**
     * Configures the branching rule used to create child nodes
     * @param t_branching_rule the desired branching rule
     * @return the optimizer factory itself
     */
    BranchAndBound<NodeT>& with_branching_rule(const BranchingRuleFactory<NodeT>& t_branching_rule);

    /**
     * Configures the branching rule used to create child nodes.
     *
     * Here, the function is called only when BranchingRuleFactoryT has a nested template class named Strategy.
     * In such a case, the branching rule is created by calling BranchingRuleFactoryT::Strategy<NodeT>(t_branching_rule).
     * This is used to avoid that the user repeats the used node type NodeT.
     * @tparam BranchingRuleFactoryT the class containing a nested template class named Strategy
     * @param t_branching_rule the desired branching rule
     * @return the optimizer factory itself
     */
    template<class BranchingRuleFactoryT>
    only_if_has_Strategy<BranchAndBound<NodeT>&, BranchingRuleFactoryT> with_branching_rule(const BranchingRuleFactoryT& t_branching_rule);

    /**
     * Configures the node selection rule to explore the branch and bound tree.
     * @param t_node_selection the desired node selection rule
     * @return the optimizer factory itself
     */
    BranchAndBound<NodeT>& with_node_selection_rule(const NodeSelectionRuleFactory<NodeT>& t_node_selection);

    /**
     * Configures the node selection rule to explore the branch and bound tree.
     *
     * Here, the function is called only when NodeSelectionRuleFactoryT has a nested template class named Strategy.
     * In such a case, the node selection rule is created by calling NodeSelectionRuleFactoryT::Strategy<NodeT>(t_node_selection_rule).
     * This is used to avoid that the user repeats the used node type NodeT.
     * @tparam NodeSelectionRuleFactoryT the class containing a nested template class named Strategy
     * @param t_node_selection_rule the desired node selection rule
     * @return the optimizer factory itself
     */
    template<class NodeSelectionRuleFactoryT>
    only_if_has_Strategy<BranchAndBound<NodeT>&, NodeSelectionRuleFactoryT> with_node_selection_rule(const NodeSelectionRuleFactoryT& t_node_selection_rule);

    BranchAndBound(BranchAndBound&&) noexcept = delete;
    BranchAndBound& operator=(const BranchAndBound&) = delete;
    BranchAndBound& operator=(BranchAndBound&&) noexcept = delete;

    Optimizer *operator()(const Model &t_model) const override;

    [[nodiscard]] OptimizerFactory *clone() const override;

    /**
     * Configures the depth for sub-tree exploration.
     *
     * When a node is selected for branching, all of its children are explored. This exploration takes the form of a
     * sub-tree exploration. When this parameter is set to 0, only the root node of this sub-tree is solved. Thus
     * every child node are solved and the branch-and-bound algorithm is continued.
     *
     * For strictly greater values of this parameter, the sub-tree is explored with a maximum depth equal to the
     * value of this parameter.
     *
     * For example, with a value of 1, each child node is solved along with its child nodes.
     * @param t_depth the maximum sub-tree exploration depth
     * @return the optimizer factory itself
     */
    BranchAndBound<NodeT>& with_subtree_depth(unsigned int t_depth);

    /**
     * Configures the log frequency of the optimizer, i.e., nodes are logged every t_log_frequency nodes.
     * @param t_log_frequency the desired frequency for logging
     * @return the optimizer factory itself
     */
    BranchAndBound<NodeT>& with_log_frequency(unsigned int t_log_frequency);

    /**
     * Configures a callback which will be called by the optimizer.
     *
     * Note that this method can be called multiple times so that multiple callbacks can be added.
     * @param t_callback the callback factory
     * @return the optimizer factory itself
     */
    BranchAndBound<NodeT>& with_callback(const BranchAndBoundCallbackFactory<NodeT> & t_callback);
};

template<class NodeT>
BranchAndBound<NodeT> &BranchAndBound<NodeT>::with_callback(const BranchAndBoundCallbackFactory<NodeT> &t_callback) {

    m_callbacks.emplace_back(t_callback.clone());

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
          m_node_selection_rule_factory(t_rhs.m_node_selection_rule_factory ? t_rhs.m_node_selection_rule_factory->clone() : nullptr) {

    for (auto& cb : t_rhs.m_callbacks) {
        m_callbacks.emplace_back(cb->clone());
    }

}

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

    auto* callback_interface = new BranchAndBoundCallbackI<NodeT>();

    auto* result = new Optimizers::BranchAndBound<NodeT>(t_model,
                                     *m_relaxation_optimizer_factory,
                                     *m_branching_rule_factory,
                                     *m_node_selection_rule_factory,
                                     callback_interface);

    this->handle_default_parameters(result);

    if (m_log_frequency.has_value()) {
        result->set_log_frequency(m_log_frequency.value());
    }

    if (m_subtree_depth.has_value()) {
        result->set_subtree_depth(m_subtree_depth.value());
    }

    for (auto& cb : m_callbacks) {
        result->add_callback((BranchAndBoundCallback<NodeT>*) cb->operator()());
    }

    return result;
}

template<class NodeT>
OptimizerFactory *BranchAndBound<NodeT>::clone() const {
    return new BranchAndBound(*this);
}

#endif //IDOL_BRANCHANDBOUND_H
