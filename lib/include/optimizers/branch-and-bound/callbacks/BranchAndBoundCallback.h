//
// Created by henri on 30/03/23.
//

#ifndef IDOL_BRANCHANDBOUNDCALLBACK_H
#define IDOL_BRANCHANDBOUNDCALLBACK_H

#include "optimizers/branch-and-bound/callbacks/BranchAndBoundCallbackI.h"
#include "optimizers/branch-and-bound/Optimizers_BranchAndBound.h"

template<class NodeInfoT>
class BranchAndBoundCallback : public BranchAndBoundCallbackI<NodeInfoT> {
protected:
    BranchAndBoundCallback();

    class TemporaryInterface;
    class AdvancedInterface;

    using SideEffectRegistry = typename BranchAndBoundCallbackI<NodeInfoT>::SideEffectRegistry;

    [[nodiscard]] const Node<NodeInfoT>& node() const;

    [[nodiscard]] const Model& relaxation() const;

    [[nodiscard]] const Model& original_model() const;

    void submit_heuristic_solution(NodeInfoT* t_info);

    void submit_lower_bound(double t_lower_bound);

    TemporaryInterface temporary_interface();

    AdvancedInterface& advanced_interface() { return *m_advanced_interface; }

    const AdvancedInterface& advanced_interface() const { return *m_advanced_interface; }
private:
    std::unique_ptr<AdvancedInterface> m_advanced_interface;

    // Attributes
    Node<NodeInfoT>* m_node = nullptr;
    Model* m_relaxation = nullptr;
    Optimizers::BranchAndBound<NodeInfoT>* m_parent = nullptr;
    typename BranchAndBoundCallbackI<NodeInfoT>::SideEffectRegistry* m_side_effect_registry = nullptr;

    // CallbackI overrides
    void set_node(Node<NodeInfoT> *t_node) final { m_node = t_node; }
    void set_relaxation(Model *t_relaxation) final { m_relaxation = t_relaxation; }
    void set_parent(Optimizers::BranchAndBound<NodeInfoT> *t_parent) final { m_parent = t_parent; }
    void set_side_effect_registry(typename BranchAndBoundCallbackI<NodeInfoT>::SideEffectRegistry* t_registry) final { m_side_effect_registry = t_registry; }
};

template<class NodeInfoT>
void BranchAndBoundCallback<NodeInfoT>::submit_lower_bound(double t_lower_bound) {
    m_parent->submit_lower_bound(t_lower_bound);
}

template<class NodeInfoT>
BranchAndBoundCallback<NodeInfoT>::BranchAndBoundCallback() : m_advanced_interface(new AdvancedInterface(*this)) {

}

template<class NodeInfoT>
void BranchAndBoundCallback<NodeInfoT>::submit_heuristic_solution(NodeInfoT* t_info) {
    m_parent->submit_heuristic_solution(t_info);
}

template<class NodeInfoT>
const Model &BranchAndBoundCallback<NodeInfoT>::original_model() const {
    return m_parent->parent();
}

template<class NodeInfoT>
const Model &BranchAndBoundCallback<NodeInfoT>::relaxation() const {

    if (!m_relaxation) {
        throw Exception("Relaxation is not accessible in this context.");
    }

    return *m_relaxation;
}

template<class NodeInfoT>
const Node<NodeInfoT>& BranchAndBoundCallback<NodeInfoT>::node() const {

    if (!m_node) {
        throw Exception("Node is not accessible in this context.");
    }

    return *m_node;
}

template<class NodeInfoT>
class BranchAndBoundCallback<NodeInfoT>::AdvancedInterface {
    friend class BranchAndBoundCallback<NodeInfoT>;

    BranchAndBoundCallback<NodeInfoT>& m_parent;

    explicit AdvancedInterface(BranchAndBoundCallback<NodeInfoT>& t_parent);
public:
    SideEffectRegistry &side_effect_registry();

    const SideEffectRegistry &side_effect_registry() const;

    Model& relaxation();

    Node<NodeInfoT>& node();
};

template<class NodeInfoT>
Node<NodeInfoT> &BranchAndBoundCallback<NodeInfoT>::AdvancedInterface::node() {

    if (m_parent.m_node) {
        throw Exception("Node is not accessible in this context.");
    }

    return *m_parent.m_node;
}

template<class NodeInfoT>
Model &BranchAndBoundCallback<NodeInfoT>::AdvancedInterface::relaxation() {

    if (!m_parent.m_relaxation) {
        throw Exception("Relaxation is not accessible in this context.");
    }

    return *m_parent.m_relaxation;
}

template<class NodeInfoT>
const typename BranchAndBoundCallback<NodeInfoT>::SideEffectRegistry &BranchAndBoundCallback<NodeInfoT>::AdvancedInterface::side_effect_registry() const {

    if (!m_parent.m_side_effect_registry) {
        throw Exception("Side effect registry not accessible in this context.");
    }

    return *m_parent.m_side_effect_registry;
}

template<class NodeInfoT>
typename BranchAndBoundCallback<NodeInfoT>::SideEffectRegistry &BranchAndBoundCallback<NodeInfoT>::AdvancedInterface::side_effect_registry() {

    if (!m_parent.m_side_effect_registry) {
        throw Exception("Side effect registry not accessible in this context.");
    }

    return *m_parent.m_side_effect_registry;
}

template<class NodeInfoT>
BranchAndBoundCallback<NodeInfoT>::AdvancedInterface::AdvancedInterface(BranchAndBoundCallback<NodeInfoT> &t_parent)
    : m_parent(t_parent) {

}

template<class NodeInfoT>
class BranchAndBoundCallback<NodeInfoT>::TemporaryInterface {
    friend class BranchAndBoundCallback<NodeInfoT>;

    BranchAndBoundCallback<NodeInfoT>& m_parent;
    std::list< std::pair<Var, double> > m_var_lb_history;
    std::list< std::pair<Var, double> > m_var_ub_history;

    explicit TemporaryInterface(BranchAndBoundCallback<NodeInfoT>& t_parent);
public:
    ~TemporaryInterface();

    void reoptimize();

    void set_var_lb(const Var& t_var, double t_value);
    void set_var_ub(const Var& t_var, double t_value);
};

template<class NodeInfoT>
void BranchAndBoundCallback<NodeInfoT>::TemporaryInterface::reoptimize() {
    m_parent.m_relaxation->optimize();
}

template<class NodeInfoT>
BranchAndBoundCallback<NodeInfoT>::TemporaryInterface::~TemporaryInterface() {

    for (const auto& [var, val] : m_var_lb_history) {
        m_parent.m_relaxation->set_var_lb(var, val);
    }

    for (const auto& [var, val] : m_var_ub_history) {
        m_parent.m_relaxation->set_var_ub(var, val);
    }

}

template<class NodeInfoT>
void BranchAndBoundCallback<NodeInfoT>::TemporaryInterface::set_var_lb(const Var& t_var, double t_value) {
    const double current_value = m_parent.m_relaxation->get_var_lb(t_var);
    m_var_lb_history.emplace_front(t_var, current_value);
    m_parent.m_relaxation->set_var_lb(t_var, t_value);
}

template<class NodeInfoT>
void BranchAndBoundCallback<NodeInfoT>::TemporaryInterface::set_var_ub(const Var& t_var, double t_value) {
    const double current_value = m_parent.m_relaxation->get_var_ub(t_var);
    m_var_ub_history.emplace_front(t_var, current_value);
    m_parent.m_relaxation->set_var_ub(t_var, t_value);
}

template<class NodeInfoT>
BranchAndBoundCallback<NodeInfoT>::TemporaryInterface::TemporaryInterface(BranchAndBoundCallback<NodeInfoT> &t_parent) : m_parent(t_parent) {

}

template<class NodeInfoT>
typename BranchAndBoundCallback<NodeInfoT>::TemporaryInterface BranchAndBoundCallback<NodeInfoT>::temporary_interface() {
    return BranchAndBoundCallback::TemporaryInterface(*this);
}

#endif //IDOL_BRANCHANDBOUNDCALLBACK_H
