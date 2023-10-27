//
// Created by henri on 12/04/23.
//

#ifndef IDOL_BRANCHANDBOUNDCALLBACK_H
#define IDOL_BRANCHANDBOUNDCALLBACK_H

#include "idol/optimizers/callbacks/Callback.h"
#include "idol/optimizers/branch-and-bound/nodes/Node.h"
#include "idol/optimizers/branch-and-bound/Optimizers_BranchAndBound.h"
#include "AbstractBranchAndBoundCallbackI.h"
#include "idol/optimizers/branch-and-bound/cutting-planes/CuttingPlaneGenerator.h"
#include "CallbackAsBranchAndBoundCallback.h"

namespace idol {

    template<class NodeVarInfoT>
    class BranchAndBoundCallback;

    template<class NodeVarInfoT>
    class BranchAndBoundCallbackI;

}
/**
 * @tparam NodeVarInfoT the class used to store each branch-and-tree node's information
 */
template<class NodeVarInfoT>
class idol::BranchAndBoundCallbackI : public AbstractBranchAndBoundCallbackI<NodeVarInfoT> {
    friend class BranchAndBoundCallback<NodeVarInfoT>;

    std::list<std::unique_ptr<BranchAndBoundCallback<NodeVarInfoT>>> m_callbacks;
    std::list<std::unique_ptr<BranchAndBoundCallback<NodeVarInfoT>>> m_cutting_plane_callbacks;
    std::list<std::unique_ptr<CuttingPlaneGenerator>> m_cutting_plane_generators;

    Optimizers::BranchAndBound<NodeVarInfoT>* m_parent = nullptr;
    std::optional<Node<NodeVarInfoT>> m_node;
    Model* m_relaxation = nullptr;
    SideEffectRegistry* m_registry = nullptr;

public:
    virtual ~BranchAndBoundCallbackI() = default;
protected:
    void add_user_cut(const TempCtr &t_cut);

    void add_lazy_cut(const TempCtr &t_cut);

    [[nodiscard]] const Node<NodeVarInfoT>& node() const;

    [[nodiscard]] const Model& relaxation() const;

    [[nodiscard]] const Model& original_model() const;

    [[nodiscard]] const Timer& time() const;

    [[nodiscard]] const SideEffectRegistry& side_effect_registry() const;

    void submit_heuristic_solution(NodeVarInfoT* t_info);

    void submit_bound(double t_bound);

    SideEffectRegistry operator()(
            Optimizers::BranchAndBound<NodeVarInfoT> *t_parent,
            CallbackEvent t_event,
            const Node<NodeVarInfoT>& t_current_node,
            Model *t_relaxation) override;

    void add_callback(BranchAndBoundCallback<NodeVarInfoT> *t_cb) override;

    void add_cutting_plane_generator(const CuttingPlaneGenerator &t_cutting_plane_generator) override;

    void initialize(const Model& t_model) override;
};

template<class NodeVarInfoT>
const idol::SideEffectRegistry &idol::BranchAndBoundCallbackI<NodeVarInfoT>::side_effect_registry() const {
    if (!m_registry) {
        throw Exception("No side effect registry was found");
    }
    return *m_registry;
}

template<class NodeVarInfoT>
const idol::Timer &idol::BranchAndBoundCallbackI<NodeVarInfoT>::time() const {
    return m_parent->time();
}

template<class NodeVarInfoT>
void idol::BranchAndBoundCallbackI<NodeVarInfoT>::initialize(const Model& t_model) {

    m_cutting_plane_callbacks.clear();

    for (auto& generator : m_cutting_plane_generators) {
        generator->operator()(t_model);
        for (auto& cb : generator->generated_callbacks()) {
            m_cutting_plane_callbacks.emplace_back(CallbackAsBranchAndBoundCallback<NodeVarInfoT>(*cb).operator()());
        }
        generator->clear_generated_callbacks();
    }

}

template<class NodeVarInfoT>
void idol::BranchAndBoundCallbackI<NodeVarInfoT>::add_cutting_plane_generator(const CuttingPlaneGenerator &t_cutting_plane_generator) {
    m_cutting_plane_generators.emplace_back(t_cutting_plane_generator.clone());
}

template<class NodeVarInfoT>
class idol::BranchAndBoundCallback {
public:
    virtual ~BranchAndBoundCallback() = default;
protected:

    /**
     * This method is left for the user to write and consists in the main execution block of the callback.
     */
    virtual void operator()(CallbackEvent t_event) = 0;

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
    [[nodiscard]] const Node<NodeVarInfoT>& node() const;

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
    void submit_heuristic_solution(NodeVarInfoT* t_info);

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
    BranchAndBoundCallbackI<NodeVarInfoT>* m_interface = nullptr;

    void throw_if_no_interface() const;

    friend class BranchAndBoundCallbackI<NodeVarInfoT>;
};

template<class NodeVarInfoT>
const idol::SideEffectRegistry &idol::BranchAndBoundCallback<NodeVarInfoT>::side_effect_registry() const {
    throw_if_no_interface();
    return m_interface->side_effect_registry();
}

template<class NodeVarInfoT>
const idol::Timer &idol::BranchAndBoundCallback<NodeVarInfoT>::time() const {
    throw_if_no_interface();
    return m_interface->time();
}

template<class NodeVarInfoT>
void idol::BranchAndBoundCallback<NodeVarInfoT>::submit_bound(double t_bound) {
    throw_if_no_interface();
    m_interface->submit_bound(t_bound);
}

template<class NodeVarInfoT>
void idol::BranchAndBoundCallback<NodeVarInfoT>::submit_heuristic_solution(NodeVarInfoT *t_info) {
    throw_if_no_interface();
    m_interface->submit_heuristic_solution(t_info);
}

template<class NodeVarInfoT>
const idol::Model &idol::BranchAndBoundCallback<NodeVarInfoT>::original_model() const {
    throw_if_no_interface();
    return m_interface->original_model();
}

template<class NodeVarInfoT>
const idol::Model &idol::BranchAndBoundCallback<NodeVarInfoT>::relaxation() const {
    throw_if_no_interface();
    return m_interface->relaxation();
}

template<class NodeVarInfoT>
const idol::Node<NodeVarInfoT> &idol::BranchAndBoundCallback<NodeVarInfoT>::node() const {
    throw_if_no_interface();
    return m_interface->node();
}

template<class NodeVarInfoT>
void idol::BranchAndBoundCallback<NodeVarInfoT>::add_lazy_cut(const TempCtr &t_cut) {
    throw_if_no_interface();
    m_interface->add_lazy_cut(t_cut);
}

template<class NodeVarInfoT>
void idol::BranchAndBoundCallback<NodeVarInfoT>::add_user_cut(const TempCtr &t_cut) {
    throw_if_no_interface();
    m_interface->add_user_cut(t_cut);
}

template<class NodeVarInfoT>
void idol::BranchAndBoundCallback<NodeVarInfoT>::throw_if_no_interface() const {
    if (!m_interface) {
        throw Exception("No interface was found.");
    }
}

template<class NodeVarInfoT>
idol::SideEffectRegistry
idol::BranchAndBoundCallbackI<NodeVarInfoT>::operator()(Optimizers::BranchAndBound<NodeVarInfoT> *t_parent, CallbackEvent t_event,
                                               const Node<NodeVarInfoT> &t_current_node, Model *t_relaxation) {
    SideEffectRegistry result;

    m_parent = t_parent;
    m_node = t_current_node;
    m_relaxation = t_relaxation;
    m_registry = &result;

    for (auto& cb : m_cutting_plane_callbacks) {
        cb->m_interface = this;
        cb->operator()(t_event);
        cb->m_interface = nullptr;
    }

    if (result.n_added_user_cuts == 0) {

        for (auto &cb: m_callbacks) {
            cb->m_interface = this;
            cb->operator()(t_event);
            cb->m_interface = nullptr;
        }

    }

    m_parent = nullptr;
    m_node.reset();
    m_relaxation = nullptr;
    m_registry = nullptr;

    return result;
}

template<class NodeVarInfoT>
void idol::BranchAndBoundCallbackI<NodeVarInfoT>::add_callback(BranchAndBoundCallback<NodeVarInfoT> *t_cb) {
    m_callbacks.emplace_back(t_cb);
}

template<class NodeVarInfoT>
void idol::BranchAndBoundCallbackI<NodeVarInfoT>::submit_bound(double t_bound) {
    if (!m_parent) {
        throw Exception("submit_bound is not accessible in this context.");
    }
    m_parent->submit_lower_bound(t_bound);
}

template<class NodeVarInfoT>
void idol::BranchAndBoundCallbackI<NodeVarInfoT>::submit_heuristic_solution(NodeVarInfoT *t_info) {
    if (!m_parent) {
        throw Exception("submit_heuristic_solution is not accessible in this context.");
    }
    m_parent->submit_heuristic_solution(t_info);
}

template<class NodeVarInfoT>
const idol::Model &idol::BranchAndBoundCallbackI<NodeVarInfoT>::original_model() const {
    if (!m_parent) {
        throw Exception("original_model is not accessible in this context.");
    }
    return m_parent->parent();
}

template<class NodeVarInfoT>
const idol::Model &idol::BranchAndBoundCallbackI<NodeVarInfoT>::relaxation() const {
    if (!m_relaxation) {
        throw Exception("relaxation is not accessible in this context.");
    }
    return *m_relaxation;
}

template<class NodeVarInfoT>
const idol::Node<NodeVarInfoT> &idol::BranchAndBoundCallbackI<NodeVarInfoT>::node() const {
    if (!m_node) {
        throw Exception("node is not accessible in this context.");
    }
    return *m_node;
}

template<class NodeVarInfoT>
void idol::BranchAndBoundCallbackI<NodeVarInfoT>::add_lazy_cut(const TempCtr &t_cut) {
    m_relaxation->add_ctr(t_cut);
    ++m_registry->n_added_lazy_cuts;
}

template<class NodeVarInfoT>
void idol::BranchAndBoundCallbackI<NodeVarInfoT>::add_user_cut(const TempCtr &t_cut) {
    m_relaxation->add_ctr(t_cut);
    ++m_registry->n_added_user_cuts;
}

#endif //IDOL_BRANCHANDBOUNDCALLBACK_H
