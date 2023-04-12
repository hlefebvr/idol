//
// Created by henri on 12/04/23.
//

#ifndef IDOL_BABCALLBACK_H
#define IDOL_BABCALLBACK_H

#include "optimizers/callbacks/Callback.h"
#include "optimizers/branch-and-bound/nodes/Node.h"
#include "optimizers/branch-and-bound/Optimizers_BranchAndBound.h"
#include "AbstractBaBCallbackI.h"

template<class NodeInfoT>
class BaBCallback;

template<class NodeInfoT>
class BaBCallbackI : public AbstractBaBCallbackI<NodeInfoT> {
    friend class BaBCallback<NodeInfoT>;

    std::list<std::unique_ptr<BaBCallback<NodeInfoT>>> m_callbacks;

    Optimizers::BranchAndBound<NodeInfoT>* m_parent = nullptr;
    Node<NodeInfoT>* m_node = nullptr;
    Model* m_relaxation = nullptr;
    SideEffectRegistry* m_registry = nullptr;

public:
    virtual ~BaBCallbackI() = default;
protected:
    void add_user_cut(const TempCtr &t_cut);

    void add_lazy_cut(const TempCtr &t_cut);

    [[nodiscard]] const Node<NodeInfoT>& node() const;

    [[nodiscard]] const Model& relaxation() const;

    [[nodiscard]] const Model& original_model() const;

    void submit_heuristic_solution(NodeInfoT* t_info);

    void submit_lower_bound(double t_lower_bound);

    SideEffectRegistry operator()(
            Optimizers::BranchAndBound<NodeInfoT> *t_parent,
            CallbackEvent t_event,
            Node<NodeInfoT> *t_current_node,
            Model *t_relaxation) override;

    void add_callback(BaBCallback<NodeInfoT> *t_cb) override;
};

template<class NodeInfoT>
class BaBCallback {
public:
    virtual ~BaBCallback() = default;
protected:

    virtual void operator()(CallbackEvent t_event) = 0;

    void add_user_cut(const TempCtr& t_cut);

    void add_lazy_cut(const TempCtr& t_cut);

    [[nodiscard]] const Node<NodeInfoT>& node() const;

    [[nodiscard]] const Model& relaxation() const;

    [[nodiscard]] const Model& original_model() const;

    void submit_heuristic_solution(NodeInfoT* t_info);

    void submit_lower_bound(double t_lower_bound);
private:
    BaBCallbackI<NodeInfoT>* m_interface = nullptr;

    void throw_if_no_interface() const;

    friend class BaBCallbackI<NodeInfoT>;
};

template<class NodeInfoT>
void BaBCallback<NodeInfoT>::submit_lower_bound(double t_lower_bound) {
    throw_if_no_interface();
    m_interface->submit_lower_bound(t_lower_bound);
}

template<class NodeInfoT>
void BaBCallback<NodeInfoT>::submit_heuristic_solution(NodeInfoT *t_info) {
    throw_if_no_interface();
    m_interface->submit_heuristic_solution(t_info);
}

template<class NodeInfoT>
const Model &BaBCallback<NodeInfoT>::original_model() const {
    throw_if_no_interface();
    return m_interface->original_model();
}

template<class NodeInfoT>
const Model &BaBCallback<NodeInfoT>::relaxation() const {
    throw_if_no_interface();
    return m_interface->relaxation();
}

template<class NodeInfoT>
const Node<NodeInfoT> &BaBCallback<NodeInfoT>::node() const {
    throw_if_no_interface();
    return m_interface->node();
}

template<class NodeInfoT>
void BaBCallback<NodeInfoT>::add_lazy_cut(const TempCtr &t_cut) {
    throw_if_no_interface();
    m_interface->add_lazy_cut(t_cut);
}

template<class NodeInfoT>
void BaBCallback<NodeInfoT>::add_user_cut(const TempCtr &t_cut) {
    throw_if_no_interface();
    m_interface->add_user_cut(t_cut);
}

template<class NodeInfoT>
void BaBCallback<NodeInfoT>::throw_if_no_interface() const {
    if (!m_interface) {
        throw Exception("No interface was found.");
    }
}

template<class NodeInfoT>
SideEffectRegistry
BaBCallbackI<NodeInfoT>::operator()(Optimizers::BranchAndBound<NodeInfoT> *t_parent, CallbackEvent t_event,
                                    Node<NodeInfoT> *t_current_node, Model *t_relaxation) {
    SideEffectRegistry result;

    m_parent = t_parent;
    m_node = t_current_node;
    m_relaxation = t_relaxation;
    m_registry = &result;

    for (auto& cb : m_callbacks) {
        cb->m_interface = this;
        cb->operator()(t_event);
        cb->m_interface = nullptr;
    }

    m_parent = nullptr;
    m_node = nullptr;
    m_relaxation = nullptr;
    m_registry = nullptr;

    return result;
}

template<class NodeInfoT>
void BaBCallbackI<NodeInfoT>::add_callback(BaBCallback<NodeInfoT> *t_cb) {
    m_callbacks.emplace_back(t_cb);
}

template<class NodeInfoT>
void BaBCallbackI<NodeInfoT>::submit_lower_bound(double t_lower_bound) {
    if (!m_parent) {
        throw Exception("submit_lower_bound is not accessible in this context.");
    }
    m_parent->submit_lower_bound(t_lower_bound);
}

template<class NodeInfoT>
void BaBCallbackI<NodeInfoT>::submit_heuristic_solution(NodeInfoT *t_info) {
    if (!m_parent) {
        throw Exception("submit_heuristic_solution is not accessible in this context.");
    }
    m_parent->submit_heuristic_solution(t_info);
}

template<class NodeInfoT>
const Model &BaBCallbackI<NodeInfoT>::original_model() const {
    if (!m_parent) {
        throw Exception("original_model is not accessible in this context.");
    }
    return m_parent->parent();
}

template<class NodeInfoT>
const Model &BaBCallbackI<NodeInfoT>::relaxation() const {
    if (!m_relaxation) {
        throw Exception("relaxation is not accessible in this context.");
    }
    return *m_relaxation;
}

template<class NodeInfoT>
const Node<NodeInfoT> &BaBCallbackI<NodeInfoT>::node() const {
    if (!m_node) {
        throw Exception("node is not accessible in this context.");
    }
    return *m_node;
}

template<class NodeInfoT>
void BaBCallbackI<NodeInfoT>::add_lazy_cut(const TempCtr &t_cut) {
    m_relaxation->add_ctr(t_cut);
    m_registry->relaxation_was_modified = true;
}

template<class NodeInfoT>
void BaBCallbackI<NodeInfoT>::add_user_cut(const TempCtr &t_cut) {
    m_relaxation->add_ctr(t_cut);
    m_registry->relaxation_was_modified = true;
}

#endif //IDOL_BABCALLBACK_H
