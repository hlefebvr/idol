//
// Created by henri on 30/03/23.
//

#ifndef IDOL_CALLBACK_H
#define IDOL_CALLBACK_H

#include "optimizers/branch-and-bound/callbacks/CallbackI.h"
#include "optimizers/branch-and-bound/Optimizers_BranchAndBound.h"

template<class NodeInfoT>
class Callback : public CallbackI<NodeInfoT> {
    // Attributes
    Node<NodeInfoT>* m_node = nullptr;
    Model* m_relaxation = nullptr;
    Optimizers::BranchAndBound<NodeInfoT>* m_parent = nullptr;

    // CallbackI overrides
    void set_node(Node<NodeInfoT> *t_node) override { m_node = t_node; }
    void set_relaxation(Model *t_relaxation) override { m_relaxation = t_relaxation; }
    void set_parent(Optimizers::BranchAndBound<NodeInfoT> *t_parent) override { m_parent = t_parent; }
protected:
    [[nodiscard]] const Node<NodeInfoT>& node() const;

    [[nodiscard]] const Model& relaxation() const;

    [[nodiscard]] const Model& original_model() const;

    void submit_heuristic_solution(NodeInfoT* t_info);
};

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

#endif //IDOL_CALLBACK_H
