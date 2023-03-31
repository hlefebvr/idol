//
// Created by henri on 30/03/23.
//

#ifndef IDOL_CALLBACK_H
#define IDOL_CALLBACK_H

#include "optimizers/branch-and-bound/callbacks/CallbackI.h"
#include "optimizers/branch-and-bound/Optimizers_BranchAndBound.h"

template<class NodeInfoT>
class Callback : public CallbackI<NodeInfoT> {
protected:
    Callback();

    class AdvancedInterface;

    using SideEffectRegistry = typename CallbackI<NodeInfoT>::SideEffectRegistry;

    [[nodiscard]] const Node<NodeInfoT>& node() const;

    [[nodiscard]] const Model& relaxation() const;

    [[nodiscard]] const Model& original_model() const;

    void submit_heuristic_solution(NodeInfoT* t_info);

    void submit_lower_bound(double t_lower_bound);

    AdvancedInterface& advanced_interface() { return *m_advanced_interface; }

    const AdvancedInterface& advanced_interface() const { return *m_advanced_interface; }
private:
    std::unique_ptr<AdvancedInterface> m_advanced_interface;

    // Attributes
    Node<NodeInfoT>* m_node = nullptr;
    Model* m_relaxation = nullptr;
    Optimizers::BranchAndBound<NodeInfoT>* m_parent = nullptr;
    typename CallbackI<NodeInfoT>::SideEffectRegistry* m_side_effect_registry = nullptr;

    // CallbackI overrides
    void set_node(Node<NodeInfoT> *t_node) final { m_node = t_node; }
    void set_relaxation(Model *t_relaxation) final { m_relaxation = t_relaxation; }
    void set_parent(Optimizers::BranchAndBound<NodeInfoT> *t_parent) final { m_parent = t_parent; }
    void set_side_effect_registry(typename CallbackI<NodeInfoT>::SideEffectRegistry* t_registry) { m_side_effect_registry = t_registry; }
};

template<class NodeInfoT>
void Callback<NodeInfoT>::submit_lower_bound(double t_lower_bound) {
    m_parent->submit_lower_bound(t_lower_bound);
}

template<class NodeInfoT>
Callback<NodeInfoT>::Callback() : m_advanced_interface(new AdvancedInterface(*this)) {

}

template<class NodeInfoT>
void Callback<NodeInfoT>::submit_heuristic_solution(NodeInfoT* t_info) {
    m_parent->submit_heuristic_solution(t_info);
}

template<class NodeInfoT>
const Model &Callback<NodeInfoT>::original_model() const {
    return m_parent->parent();
}

template<class NodeInfoT>
const Model &Callback<NodeInfoT>::relaxation() const {

    if (!m_relaxation) {
        throw Exception("Relaxation is not accessible in this context.");
    }

    return *m_relaxation;
}

template<class NodeInfoT>
const Node<NodeInfoT>& Callback<NodeInfoT>::node() const {

    if (!m_node) {
        throw Exception("Node is not accessible in this context.");
    }

    return *m_node;
}

template<class NodeInfoT>
class Callback<NodeInfoT>::AdvancedInterface {
    friend class Callback<NodeInfoT>;

    Callback<NodeInfoT>& m_parent;

    explicit AdvancedInterface(Callback<NodeInfoT>& t_parent);
public:
    SideEffectRegistry &side_effect_registry();

    const SideEffectRegistry &side_effect_registry() const;

    Model& relaxation();

    Node<NodeInfoT>& node();
};

template<class NodeInfoT>
Node<NodeInfoT> &Callback<NodeInfoT>::AdvancedInterface::node() {

    if (m_parent.m_node) {
        throw Exception("Node is not accessible in this context.");
    }

    return *m_parent.m_node;
}

template<class NodeInfoT>
Model &Callback<NodeInfoT>::AdvancedInterface::relaxation() {

    if (!m_parent.m_relaxation) {
        throw Exception("Relaxation is not accessible in this context.");
    }

    return *m_parent.m_relaxation;
}

template<class NodeInfoT>
const typename Callback<NodeInfoT>::SideEffectRegistry &Callback<NodeInfoT>::AdvancedInterface::side_effect_registry() const {

    if (!m_parent.m_side_effect_registry) {
        throw Exception("Side effect registry not accessible in this context.");
    }

    return *m_parent.m_side_effect_registry;
}

template<class NodeInfoT>
typename Callback<NodeInfoT>::SideEffectRegistry &Callback<NodeInfoT>::AdvancedInterface::side_effect_registry() {

    if (!m_parent.m_side_effect_registry) {
        throw Exception("Side effect registry not accessible in this context.");
    }

    return *m_parent.m_side_effect_registry;
}

template<class NodeInfoT>
Callback<NodeInfoT>::AdvancedInterface::AdvancedInterface(Callback<NodeInfoT> &t_parent)
    : m_parent(t_parent) {

}

#endif //IDOL_CALLBACK_H
