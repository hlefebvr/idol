//
// Created by henri on 12/04/23.
//

#ifndef IDOL_BRANCHANDBOUNDCALLBACK_H
#define IDOL_BRANCHANDBOUNDCALLBACK_H

#include "idol/optimizers/mixed-integer-optimization/callbacks/Callback.h"
#include "idol/optimizers/mixed-integer-optimization/branch-and-bound/nodes/Node.h"
#include "idol/optimizers/mixed-integer-optimization/branch-and-bound/Optimizers_BranchAndBound.h"
#include "AbstractBranchAndBoundCallbackI.h"
#include "CallbackAsBranchAndBoundCallback.h"

namespace idol {

    template<class NodeInfoT>
    class BranchAndBoundCallback;

    template<class NodeInfoT>
    class BranchAndBoundCallbackI;

}
/**
 * @tparam NodeInfoT the class used to store each branch-and-tree node's information
 */
template<class NodeInfoT>
class idol::BranchAndBoundCallbackI : public AbstractBranchAndBoundCallbackI<NodeInfoT> {
    friend class BranchAndBoundCallback<NodeInfoT>;

    std::list<std::unique_ptr<BranchAndBoundCallback<NodeInfoT>>> m_callbacks;

    Optimizers::BranchAndBound<NodeInfoT>* m_parent = nullptr;
    std::optional<Node<NodeInfoT>> m_node;
    Model* m_relaxation = nullptr;
    SideEffectRegistry* m_registry = nullptr;

public:
    virtual ~BranchAndBoundCallbackI() = default;
protected:
    void add_user_cut(const TempCtr &t_cut);

    void add_lazy_cut(const TempCtr &t_cut);

    [[nodiscard]] const Node<NodeInfoT>& node() const;

    [[nodiscard]] const Model& relaxation() const;

    [[nodiscard]] const Model& original_model() const;

    [[nodiscard]] const Timer& time() const;

    [[nodiscard]] const SideEffectRegistry& side_effect_registry() const;

    void submit_heuristic_solution(NodeInfoT* t_info);

    void submit_bound(double t_bound);

    SideEffectRegistry operator()(
            Optimizers::BranchAndBound<NodeInfoT> *t_parent,
            CallbackEvent t_event,
            const Node<NodeInfoT>& t_current_node,
            Model *t_relaxation) override;

    void add_callback(BranchAndBoundCallback<NodeInfoT> *t_cb) override;

    void initialize(Optimizers::BranchAndBound<NodeInfoT>* t_parent) override;

    void log_after_termination() override;
};

template<class NodeInfoT>
void idol::BranchAndBoundCallbackI<NodeInfoT>::log_after_termination() {

    if (m_callbacks.empty()) {
        return;
    }

    std::cout << "Callbacks:" << std::endl;

    for (auto& ptr_to_cb : m_callbacks) {
        ptr_to_cb->log_after_termination();
    }

    std::cout << std::endl;

}

template<class NodeInfoT>
const idol::SideEffectRegistry &idol::BranchAndBoundCallbackI<NodeInfoT>::side_effect_registry() const {
    if (!m_registry) {
        throw Exception("No side effect registry was found");
    }
    return *m_registry;
}

template<class NodeInfoT>
const idol::Timer &idol::BranchAndBoundCallbackI<NodeInfoT>::time() const {
    return m_parent->time();
}

template<class NodeInfoT>
class idol::BranchAndBoundCallback {
public:
    virtual ~BranchAndBoundCallback() = default;
protected:

    /**
     * This method is called at the very beginning of the Branch-and-Bound algorithm
     */
    virtual void initialize() {}

    /**
     * This method is left for the user to write and consists in the main execution block of the callback.
     * @param t_event The event triggering the callback
     */
    virtual void operator()(CallbackEvent t_event) = 0;

    virtual void log_after_termination() {}

    /**
     * Adds a user cut to the relaxation
     * @param t_cut the cut to be added
     */
    void add_user_cut(const TempCtr& t_cut);

    /**
     * Adds a lazy cut to the relaxation
     * @param t_cut the cut to be added
     */
    void add_lazy_cut(const TempCtr& t_cut);

    /**
     * Returns the node which is currently explored
     * @return the node which is currently explored
     */
    [[nodiscard]] const Node<NodeInfoT>& node() const;

    /**
     * Returns the current node's model being solved
     * @return the current node's model being solved
     */
    [[nodiscard]] const Model& relaxation() const;

    /**
     * Returns the original model from which the branch-and-bound algorithm started (i.e., the original non-relaxed model)
     * @return the original model
     */
    [[nodiscard]] const Model& original_model() const;

    /**
     * Submits a new solution to the branch-and-bound tree algorithm.
     *
     * The solution is checked for validity according to the branch-and-bound tree branching rule and is set as incumbent
     * if and only if it is valid and improves the current best objective.
     * @param t_info a node information storing the solution
     */
    void submit_heuristic_solution(NodeInfoT* t_info);

    /**
     * Submits a new proven bound.
     *
     * The given bound is set as best bound if and only if it improves the current best bound.
     * @param t_bound a proven bound
     */
    void submit_bound(double t_bound);

    /**
     * Returns the side effect registry
     * @return the side effect registry
     */
     const SideEffectRegistry& side_effect_registry() const;

    [[nodiscard]] const Timer& time() const;
private:
    BranchAndBoundCallbackI<NodeInfoT>* m_interface = nullptr;

    void throw_if_no_interface() const;

    friend class BranchAndBoundCallbackI<NodeInfoT>;
};

template<class NodeInfoT>
const idol::SideEffectRegistry &idol::BranchAndBoundCallback<NodeInfoT>::side_effect_registry() const {
    throw_if_no_interface();
    return m_interface->side_effect_registry();
}

template<class NodeInfoT>
const idol::Timer &idol::BranchAndBoundCallback<NodeInfoT>::time() const {
    throw_if_no_interface();
    return m_interface->time();
}

template<class NodeInfoT>
void idol::BranchAndBoundCallback<NodeInfoT>::submit_bound(double t_bound) {
    throw_if_no_interface();
    m_interface->submit_bound(t_bound);
}

template<class NodeInfoT>
void idol::BranchAndBoundCallback<NodeInfoT>::submit_heuristic_solution(NodeInfoT *t_info) {
    throw_if_no_interface();
    m_interface->submit_heuristic_solution(t_info);
}

template<class NodeInfoT>
const idol::Model &idol::BranchAndBoundCallback<NodeInfoT>::original_model() const {
    throw_if_no_interface();
    return m_interface->original_model();
}

template<class NodeInfoT>
const idol::Model &idol::BranchAndBoundCallback<NodeInfoT>::relaxation() const {
    throw_if_no_interface();
    return m_interface->relaxation();
}

template<class NodeInfoT>
const idol::Node<NodeInfoT> &idol::BranchAndBoundCallback<NodeInfoT>::node() const {
    throw_if_no_interface();
    return m_interface->node();
}

template<class NodeInfoT>
void idol::BranchAndBoundCallback<NodeInfoT>::add_lazy_cut(const TempCtr &t_cut) {
    throw_if_no_interface();
    m_interface->add_lazy_cut(t_cut);
}

template<class NodeInfoT>
void idol::BranchAndBoundCallback<NodeInfoT>::add_user_cut(const TempCtr &t_cut) {
    throw_if_no_interface();
    m_interface->add_user_cut(t_cut);
}

template<class NodeInfoT>
void idol::BranchAndBoundCallback<NodeInfoT>::throw_if_no_interface() const {
    if (!m_interface) {
        throw Exception("No interface was found.");
    }
}

template<class NodeInfoT>
idol::SideEffectRegistry
idol::BranchAndBoundCallbackI<NodeInfoT>::operator()(Optimizers::BranchAndBound<NodeInfoT> *t_parent,
                                                     CallbackEvent t_event,
                                                     const Node<NodeInfoT> &t_current_node,
                                                     Model *t_relaxation) {
    SideEffectRegistry result;

    m_parent = t_parent;
    m_node = t_current_node;
    m_relaxation = t_relaxation;
    m_registry = &result;

    for (auto &cb: m_callbacks) {
        cb->m_interface = this;
        cb->operator()(t_event);
        cb->m_interface = nullptr;
    }

    m_parent = nullptr;
    m_node.reset();
    m_relaxation = nullptr;
    m_registry = nullptr;

    return result;
}

template<class NodeInfoT>
void idol::BranchAndBoundCallbackI<NodeInfoT>::add_callback(BranchAndBoundCallback<NodeInfoT> *t_cb) {
    m_callbacks.emplace_back(t_cb);
}

template<class NodeInfoT>
void idol::BranchAndBoundCallbackI<NodeInfoT>::initialize(Optimizers::BranchAndBound<NodeInfoT>* t_parent) {

    m_parent = t_parent;

    for (auto& ptr_callback : m_callbacks) {
        ptr_callback->m_interface = this;
        ptr_callback->initialize();
        ptr_callback->m_interface = nullptr;
    }

    m_parent = nullptr;

}

template<class NodeInfoT>
void idol::BranchAndBoundCallbackI<NodeInfoT>::submit_bound(double t_bound) {
    if (!m_parent) {
        throw Exception("submit_bound is not accessible in this context.");
    }
    m_parent->submit_lower_bound(t_bound);
}

template<class NodeInfoT>
void idol::BranchAndBoundCallbackI<NodeInfoT>::submit_heuristic_solution(NodeInfoT *t_info) {
    if (!m_parent) {
        throw Exception("submit_heuristic_solution is not accessible in this context.");
    }
    m_parent->submit_heuristic_solution(t_info);
}

template<class NodeInfoT>
const idol::Model &idol::BranchAndBoundCallbackI<NodeInfoT>::original_model() const {
    if (!m_parent) {
        throw Exception("original_model is not accessible in this context.");
    }
    return m_parent->parent();
}

template<class NodeInfoT>
const idol::Model &idol::BranchAndBoundCallbackI<NodeInfoT>::relaxation() const {
    if (!m_relaxation) {
        throw Exception("relaxation is not accessible in this context.");
    }
    return *m_relaxation;
}

template<class NodeInfoT>
const idol::Node<NodeInfoT> &idol::BranchAndBoundCallbackI<NodeInfoT>::node() const {
    if (!m_node) {
        throw Exception("node is not accessible in this context.");
    }
    return *m_node;
}

template<class NodeInfoT>
void idol::BranchAndBoundCallbackI<NodeInfoT>::add_lazy_cut(const TempCtr &t_cut) {
    m_relaxation->add_ctr(t_cut);
    ++m_registry->n_added_lazy_cuts;
}

template<class NodeInfoT>
void idol::BranchAndBoundCallbackI<NodeInfoT>::add_user_cut(const TempCtr &t_cut) {
    m_relaxation->add_ctr(t_cut);
    ++m_registry->n_added_user_cuts;
}

#endif //IDOL_BRANCHANDBOUNDCALLBACK_H
