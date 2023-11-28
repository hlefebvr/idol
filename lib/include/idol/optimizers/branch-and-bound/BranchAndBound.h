//
// Created by henri on 21/03/23.
//

#ifndef IDOL_BRANCHANDBOUND_H
#define IDOL_BRANCHANDBOUND_H

#include <memory>
#include "idol/optimizers/OptimizerFactory.h"
#include "Optimizers_BranchAndBound.h"
#include "idol/optimizers/branch-and-bound/callbacks/BranchAndBoundCallbackFactory.h"
#include "idol/optimizers/branch-and-bound/callbacks/BranchAndBoundCallback.h"
#include "idol/optimizers/branch-and-bound/callbacks/CallbackAsBranchAndBoundCallback.h"
#include "idol/optimizers/callbacks/CallbackFactory.h"
#include "idol/optimizers/branch-and-bound/nodes/DefaultNodeInfo.h"
#include "idol/optimizers/branch-and-bound/logs/Factory.h"
#include "idol/optimizers/branch-and-bound/logs/Info.h"

namespace idol {
    template<class NodeT>
    class BranchAndBound;
}

/**
 * @tparam NodeT the class used to store nodes information.
 * It is strongly advised to inherit from NodeVarInfo in order to create your own node type.
 */
template<class NodeT = idol::DefaultNodeInfo>
class idol::BranchAndBound : public OptimizerFactoryWithDefaultParameters<BranchAndBound<NodeT>> {
    std::unique_ptr<OptimizerFactory> m_relaxation_optimizer_factory;
    std::unique_ptr<BranchingRuleFactory<NodeT>> m_branching_rule_factory;
    std::unique_ptr<NodeSelectionRuleFactory<NodeT>> m_node_selection_rule_factory;
    std::unique_ptr<Logs::BranchAndBound::Factory<NodeT>> m_logger_factory;

    std::list<std::unique_ptr<BranchAndBoundCallbackFactory<NodeT>>> m_callbacks;

    std::optional<unsigned int> m_subtree_depth;
    std::optional<unsigned int> m_log_frequency;
    std::optional<bool> m_scaling;
public:
    /**
     * This type is used to exploit [SFINAE](https://en.wikipedia.org/wiki/Substitution_failure_is_not_an_error)
     * in order to identify classes having a sub-class named `Strategy<NodeInfoT>`.
     * This is used to make calls like `.with_node_selection_rule(DepthFirst());` which will actually call
     * `.with_node_selection_rule(DepthFirst::Strategy<NodeInfoT>())`.
     */
    template<class ReturnT, class T> using only_if_has_Strategy = typename std::pair<typename T::template Strategy<NodeT>, ReturnT>::second_type;

    /**
     * Creates a new branch-and-bound algorithm.
     *
     * Example:
     * ```cpp
     * model.use( BranchAndBound() );
     * ```
     */
    BranchAndBound() = default;

    /**
     * Copy constructor
     * @param t_rhs the object to copy
     */
    BranchAndBound(const BranchAndBound& t_rhs);

    /**
     * Sets the optimizer for solving each of the branch-and-bound tree nodes
     *
     * Example:
     * ```cpp
     * auto algorithm = BranchAndBound()
     *                      .with_node_optimizer(GLPK::ContinuousRelaxation());
     * ```
     *
     * @param t_node_optimizer the optimizer factory the node problems
     * @return the optimizer factory itself
     */
    BranchAndBound<NodeT>& with_node_optimizer(const OptimizerFactory& t_node_optimizer);

    BranchAndBound<NodeT>& operator+=(const OptimizerFactory& t_node_optimizer);

    /**
     * Sets the branching rule used to create child nodes
     *
     * Example:
     * ```cpp
     * auto algorithm = BranchAndBound<NodeVarInfo>()
     *                      .with_branching_rule(MostInfeasible::Strategy<NodeVarInfo>());
     * ```
     * @param t_branching_rule the branching rule
     * @return the optimizer factory itself
     */
    BranchAndBound<NodeT>& with_branching_rule(const BranchingRuleFactory<NodeT>& t_branching_rule);

    /**
     * Sets the branching rule used to create child nodes.
     *
     * Here, the function is called only when `BranchingRuleFactoryT` has a nested template class named `Strategy<NodeInfoT>`.
     * In such a case, the branching rule is created by calling `BranchingRuleFactoryT::Strategy<NodeInfoT>(t_branching_rule)`.
     * This is used to avoid the user repeating the node type `NodeInfoT` being used.
     *
     * Example:
     * ```cpp
     * auto algorithm = BranchAndBound<NodeVarInfo>()
     *                      .with_branching_rule(MostInfeasible());
     * ```
     * @tparam BranchingRuleFactoryT the class containing a nested template class named Strategy
     * @param t_branching_rule the branching rule
     * @return the optimizer factory itself
     */
    template<class BranchingRuleFactoryT>
    only_if_has_Strategy<BranchAndBound<NodeT>&, BranchingRuleFactoryT> with_branching_rule(const BranchingRuleFactoryT& t_branching_rule);

    /**
     * Sets the node selection rule to explore the branch and bound tree.
     *
     * ```cpp
     * auto algorithm = BranchAndBound<NodeVarInfo>()
     *                      .with_node_selection(BestBound::Strategy<NodeVarInfo>());
     * ```
     * @param t_node_selection the node selection rule
     * @return the optimizer factory itself
     */
    BranchAndBound<NodeT>& with_node_selection_rule(const NodeSelectionRuleFactory<NodeT>& t_node_selection);

    /**
     * Sets the node selection rule to explore the branch and bound tree.
     *
     * Here, the function is called only when `NodeSelectionRuleFactoryT` has a nested template class named `Strategy<NodeInfoT>`.
     * In such a case, the node selection rule is created by calling `NodeSelectionRuleFactoryT::Strategy<NodeInfoT>(t_node_selection_rule)`.
     * This is used to avoid the user repeating the node type `NodeInfoT` being used.
     *
     * Example:
     * ```cpp
     * auto algorithm = BranchAndBound<NodeVarInfo>()
     *                      .with_node_selection_rule(BestBound());
     * ```
     * @tparam NodeSelectionRuleFactoryT the class containing a nested template class named Strategy
     * @param t_node_selection_rule the node selection rule
     * @return the optimizer factory itself
     */
    template<class NodeSelectionRuleFactoryT>
    only_if_has_Strategy<BranchAndBound<NodeT>&, NodeSelectionRuleFactoryT> with_node_selection_rule(const NodeSelectionRuleFactoryT& t_node_selection_rule);

    BranchAndBound(BranchAndBound&&) noexcept = default;
    BranchAndBound& operator=(const BranchAndBound&) = delete;
    BranchAndBound& operator=(BranchAndBound&&) noexcept = delete;

    Optimizer *operator()(const Model &t_model) const override;

    [[nodiscard]] OptimizerFactory *clone() const override;

    /**
     * Sets the depth for sub-tree exploration.
     *
     * When a node is selected for branching, each of its children is explored. This exploration takes the form of a
     * sub-tree exploration. When this parameter is set to 0, only the root node of this sub-tree is solved. Thus
     * every child node are solved and the branch-and-bound algorithm is continued.
     *
     * For strictly greater values of this parameter, the sub-tree is explored with a maximum depth equal to the
     * value of this parameter.
     *
     * For example, with a value of 1, each child node is solved along with its child nodes.
     *
     * Example:
     * ```cpp
     * auto algorithm = BranchAndBound()
     *                      .with_subtree_depth(1);
     * ```
     * @param t_depth the maximum sub-tree exploration depth
     * @return the optimizer factory itself
     */
    BranchAndBound<NodeT>& with_subtree_depth(unsigned int t_depth);

    BranchAndBound<NodeT>& with_logger(const Logs::BranchAndBound::Factory<NodeT>& t_log_factory);

    BranchAndBound<NodeT>& with_scaling(bool t_value);

    /**
     * Adds a callback which will be called by the optimizer.
     *
     * Note that this method can be called multiple times so that multiple callbacks can be added.
     *
     * ```cpp
     * auto algorithm = BranchAndBound()
     *                      .with_callback(IntegerMaster.rst());
     * ```
     * @param t_callback the callback factory
     * @return the optimizer factory itself
     */
    BranchAndBound<NodeT>& add_callback(const BranchAndBoundCallbackFactory<NodeT> & t_callback);

    /**
     * Adds a (solver independent) callback which will be called by the optimizer.
     *
     * Note that this method can be called multiple times so that multiple callbacks can be added.
     *
     * Here, the `Callback` is automatically converted into a `BranchAndBoundCallback<NodeInfoT>`.
     *
     * ```cpp
     * auto algorithm = BranchAndBound()
     *                      .with_callback(UserCutCallback(separation_model, my_cut));
     * ```
     * @param t_callback the callback factory
     * @return the optimizer factory itself
     */
    BranchAndBound<NodeT>& add_callback(const CallbackFactory& t_callback);
};

template<class NodeT>
idol::BranchAndBound<NodeT> &
idol::BranchAndBound<NodeT>::with_logger(const typename idol::Logs::BranchAndBound::Factory<NodeT> &t_log_factory) {

    if (m_logger_factory) {
        throw Exception("Logs have already been configured.");
    }

    m_logger_factory.reset(t_log_factory.clone());

    return *this;
}

template<class NodeT>
idol::BranchAndBound<NodeT> &idol::BranchAndBound<NodeT>::with_scaling(bool t_value) {

    if (m_scaling.has_value()) {
        throw Exception("Scaling has already been configured.");
    }

    m_scaling = t_value;

    return *this;
}

template<class NodeT>
idol::BranchAndBound<NodeT> &idol::BranchAndBound<NodeT>::operator+=(const idol::OptimizerFactory &t_node_optimizer) {
    return with_node_optimizer(t_node_optimizer);
}

template<class NodeT>
idol::BranchAndBound<NodeT> &
idol::BranchAndBound<NodeT>::add_callback(const CallbackFactory &t_callback) {
    return add_callback(CallbackAsBranchAndBoundCallback<NodeT>(t_callback));
}

template<class NodeT>
idol::BranchAndBound<NodeT> &idol::BranchAndBound<NodeT>::add_callback(const BranchAndBoundCallbackFactory<NodeT> &t_callback) {

    m_callbacks.emplace_back(t_callback.clone());

    return *this;
}

template<class NodeT>
idol::BranchAndBound<NodeT> &idol::BranchAndBound<NodeT>::with_subtree_depth(unsigned int t_depth) {

    if (m_subtree_depth.has_value()) {
        throw Exception("A subtree depth has already been given");
    }

    m_subtree_depth = t_depth;

    return *this;
}

template<class NodeT>
template<class NodeSelectionRuleFactoryT>
typename idol::BranchAndBound<NodeT>::template only_if_has_Strategy<idol::BranchAndBound<NodeT>&, NodeSelectionRuleFactoryT>
idol::BranchAndBound<NodeT>::with_node_selection_rule(const NodeSelectionRuleFactoryT &t_node_selection_rule) {
    return with_node_selection_rule(typename NodeSelectionRuleFactoryT::template Strategy<NodeT>(t_node_selection_rule));
}

template<class NodeT>
idol::BranchAndBound<NodeT> &
idol::BranchAndBound<NodeT>::with_node_selection_rule(const NodeSelectionRuleFactory<NodeT> &t_node_selection) {

    if (m_node_selection_rule_factory) {
        throw Exception("A node selection rule has already been set.");
    }

    m_node_selection_rule_factory.reset(t_node_selection.clone());

    return *this;
}

template<class NodeT>
template<class BranchingRuleFactoryT>
typename idol::BranchAndBound<NodeT>::template only_if_has_Strategy<idol::BranchAndBound<NodeT>&, BranchingRuleFactoryT>
idol::BranchAndBound<NodeT>::with_branching_rule(const BranchingRuleFactoryT &t_branching_rule) {
    return with_branching_rule(typename BranchingRuleFactoryT::template Strategy<NodeT>(t_branching_rule));
}

template<class NodeT>
idol::BranchAndBound<NodeT> &idol::BranchAndBound<NodeT>::with_branching_rule(const BranchingRuleFactory<NodeT> &t_branching_rule) {

    if (m_branching_rule_factory) {
        throw Exception("A branching rule has already been set.");
    }

    m_branching_rule_factory.reset(t_branching_rule.clone());

    return *this;
}

template<class NodeT>
idol::BranchAndBound<NodeT> &idol::BranchAndBound<NodeT>::with_node_optimizer(const OptimizerFactory &t_node_optimizer) {

    if (m_relaxation_optimizer_factory) {
        throw Exception("A node solver has already been set.");
    }

    m_relaxation_optimizer_factory.reset(t_node_optimizer.clone());

    return *this;
}

template<class NodeT>
idol::BranchAndBound<NodeT>::BranchAndBound(const BranchAndBound &t_rhs)

        : OptimizerFactoryWithDefaultParameters<BranchAndBound<NodeT>>(t_rhs),
          m_relaxation_optimizer_factory(t_rhs.m_relaxation_optimizer_factory ? t_rhs.m_relaxation_optimizer_factory->clone() : nullptr),
          m_branching_rule_factory(t_rhs.m_branching_rule_factory ? t_rhs.m_branching_rule_factory->clone() : nullptr),
          m_node_selection_rule_factory(t_rhs.m_node_selection_rule_factory ? t_rhs.m_node_selection_rule_factory->clone() : nullptr),
          m_subtree_depth(t_rhs.m_subtree_depth),
          m_scaling(t_rhs.m_scaling),
          m_logger_factory(t_rhs.m_logger_factory ? t_rhs.m_logger_factory->clone() : nullptr) {

    for (auto& cb : t_rhs.m_callbacks) {
        m_callbacks.emplace_back(cb->clone());
    }

}

template<class NodeT>
idol::Optimizer *idol::BranchAndBound<NodeT>::operator()(const Model &t_model) const {

    if (!m_relaxation_optimizer_factory) {
        throw Exception("No node solver has been given, please call BranchAndBound::with_node_optimizer to configure.");
    }

    if (!m_branching_rule_factory) {
        throw Exception("No branching rule has been given, please call BranchAndBound::with_branching_rule to configure.");
    }

    if (!m_node_selection_rule_factory) {
        throw Exception("No node selection rule has been given, please call BranchAndBound::with_node_selection_rule to configure.");
    }

    auto* callback_interface = new BranchAndBoundCallbackI<NodeT>();

    std::unique_ptr<Logs::BranchAndBound::Factory<NodeT>> default_logger_factory;
    if (!m_logger_factory) {
        default_logger_factory = std::make_unique<Logs::BranchAndBound::Info<NodeT>>();
    }

    auto* result = new Optimizers::BranchAndBound<NodeT>(t_model,
                                     *m_relaxation_optimizer_factory,
                                     *m_branching_rule_factory,
                                     *m_node_selection_rule_factory,
                                     callback_interface,
                                     m_scaling.has_value() && m_scaling.value(),
                                     m_logger_factory ? *m_logger_factory : *default_logger_factory);

    this->handle_default_parameters(result);

    if (m_subtree_depth.has_value()) {
        result->set_subtree_depth(m_subtree_depth.value());
    }

    for (auto& cb : m_callbacks) {
        result->add_callback(cb->operator()());
    }

    return result;
}

template<class NodeT>
idol::OptimizerFactory *idol::BranchAndBound<NodeT>::clone() const {
    return new BranchAndBound(*this);
}

namespace idol {
    template<class NodeInfoT>
    BranchAndBound<NodeInfoT> operator+(const BranchAndBound<NodeInfoT>& t_branch_and_bound, const OptimizerFactory& t_node_optimizer) {
        BranchAndBound<NodeInfoT> result(t_branch_and_bound);
        result += t_node_optimizer;
        return result;
    }
}

#endif //IDOL_BRANCHANDBOUND_H
