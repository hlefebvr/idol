//
// Created by henri on 31/01/23.
//

#ifndef IDOL_OPTIMIZERWITHLAZYUPDATES_H
#define IDOL_OPTIMIZERWITHLAZYUPDATES_H

#include "idol/general/optimizers/Optimizer.h"

#include "idol/mixed-integer/modeling/models/Model.h"

#include <vector>
#include <list>
#include <optional>

namespace idol {
    template<class T, class ImplT>
    class Lazy;

    template<class VarImplT, class CtrImplT, class QCtrImplT>
    class OptimizerWithLazyUpdates;
}

template<class T, class ImplT>
class idol::Lazy {
    T m_object;
    std::optional<ImplT> m_impl;
    std::optional<std::list<unsigned int>::iterator> m_to_be_updated_flag;
public:
    Lazy(const T& t_object, std::list<unsigned int>::iterator t_to_be_updated_flag)
        : m_object(t_object), m_to_be_updated_flag(t_to_be_updated_flag) {}

    [[nodiscard]] bool has_impl() const { return m_impl.has_value(); }

    ImplT& impl() { return m_impl.value(); }

    const ImplT& impl() const { return m_impl.value(); }

    [[nodiscard]] bool is_to_be_updated() const { return m_to_be_updated_flag.has_value(); }

    void set_as_to_be_updated(std::list<unsigned int>::iterator t_flag) { m_to_be_updated_flag = t_flag; }

    void set_as_updated() { return m_to_be_updated_flag.reset(); }

    [[nodiscard]] std::list<unsigned int>::iterator to_be_updated_flag() const { return m_to_be_updated_flag.value(); }

    void set_impl(ImplT&& t_impl) { m_impl = std::move(t_impl); }

    const T& object() const { return m_object; }
};

template<class VarImplT, class CtrImplT, class QCtrImplT>
class idol::OptimizerWithLazyUpdates : public Optimizer {
    std::vector<Lazy<Var, VarImplT>> m_variables;
    std::list<unsigned int> m_variables_to_update;

    std::vector<Lazy<Ctr, CtrImplT>> m_constraints;
    std::list<unsigned int> m_constraints_to_update;

    std::vector<Lazy<QCtr, QCtrImplT>> m_qconstraints;
    std::list<unsigned int> m_qconstraints_to_update;

    bool m_is_initialized = false;
    bool m_is_objective_to_be_updated = true;
    bool m_is_rhs_to_be_updated = true;

    void update_vars();
    void update_ctrs();
    void update_qctrs();

    void lazy_update_objective_sense();
    void lazy_update_matrix(const Ctr& t_ctr, const Var &t_var, double t_constant);
    void lazy_update(const Var& t_var);
    void lazy_update(const Ctr& t_ctr);
    void lazy_update_objective();
    void lazy_update_rhs();
protected:
    explicit OptimizerWithLazyUpdates(const Model& t_parent);

    void build() final;
    virtual void hook_build() = 0;

    void write(const std::string &t_name) final;
    virtual void hook_write(const std::string& t_name) = 0;

    void add(const Var &t_var) final;
    virtual VarImplT hook_add(const Var& t_var, bool t_add_column) = 0;

    void add(const Ctr &t_ctr) final;
    virtual CtrImplT hook_add(const Ctr& t_ctr) = 0;

    void add(const QCtr& t_ctr) final;
    virtual QCtrImplT hook_add(const QCtr& t_ctr) = 0;

    virtual void hook_update_objective_sense() = 0;
    virtual void hook_update_matrix(const Ctr &t_ctr, const Var &t_var, double t_constant) = 0;

    void update() final;
    virtual void hook_update() = 0;
    virtual void hook_update(const Var& t_var) = 0;
    virtual void hook_update(const Ctr& t_ctr) = 0;

    virtual void hook_update_objective() = 0;

    virtual void hook_update_rhs() = 0;

    void remove(const Var& t_var) final;
    virtual void hook_remove(const Var& t_var) = 0;

    void remove(const Ctr& t_ctr) final;
    virtual void hook_remove(const Ctr& t_ctr) = 0;

    void remove(const QCtr& t_ctr) final;
    virtual void hook_remove(const QCtr& t_ctr) = 0;

    [[nodiscard]] bool has_lazy(const Var& t_var) const {
        const unsigned int index = parent().get_var_index(t_var);
        return index < m_variables.size() && m_variables[index].object().id() == t_var.id();
    }

    [[nodiscard]] bool has_lazy(const Ctr& t_ctr) const {
        const unsigned int index = parent().get_ctr_index(t_ctr);
        return index < m_constraints.size() && m_constraints[index].object().id() == t_ctr.id();
    }

    [[nodiscard]] bool has_lazy(const QCtr& t_qctr) const {
        const unsigned int index = parent().get_qctr_index(t_qctr);
        return index < m_qconstraints.size() && m_qconstraints[index].object().id() == t_qctr.id();
    }

    auto& lazy(const Var& t_var) { return m_variables[parent().get_var_index(t_var)]; }
    const auto& lazy(const Var& t_var) const { return m_variables[parent().get_var_index(t_var)]; }

    auto& lazy(const Ctr& t_ctr) { return m_constraints[parent().get_ctr_index(t_ctr)]; }
    const auto& lazy(const Ctr& t_ctr) const { return m_constraints[parent().get_ctr_index(t_ctr)]; }

    auto& lazy(const QCtr& t_ctr) { return m_qconstraints[parent().get_qctr_index(t_ctr)]; }
    const auto& lazy(const QCtr& t_ctr) const { return m_qconstraints[parent().get_qctr_index(t_ctr)]; }

    auto& lazy_vars() { return m_variables; }
    const auto& lazy_vars() const { return m_variables; }

    auto& lazy_ctrs() { return m_constraints; }
    const auto& lazy_ctrs() const { return m_constraints; }

    auto& lazy_qctrs() { return m_qconstraints; }
    const auto& lazy_qctrs() const { return m_qconstraints; }

    void set_objective_to_be_updated() { m_is_objective_to_be_updated = true; }
    [[nodiscard]] bool is_objective_to_be_updated() const { return m_is_objective_to_be_updated; }
    void set_objective_as_updated() { m_is_objective_to_be_updated = false; }

    void set_rhs_to_be_updated() { m_is_rhs_to_be_updated = true; }
    [[nodiscard]] bool is_rhs_to_be_updated() const { return m_is_rhs_to_be_updated; }
    void set_rhs_as_updated() { m_is_rhs_to_be_updated = false; }

    void update_obj_sense() override;
    void update_obj() override;
    void update_rhs() override;
    void update_obj_constant() override;
    void update_mat_coeff(const Ctr &t_ctr, const Var &t_var) override;
    void update_ctr_type(const Ctr &t_ctr) override;
    void update_ctr_rhs(const Ctr &t_ctr) override;
    void update_var_type(const Var &t_var) override;
    void update_var_lb(const Var &t_var) override;
    void update_var_ub(const Var &t_var) override;
    void update_var_obj(const Var &t_var) override;
public:
    VarImplT& operator[](const Var& t_var) { return lazy(t_var).impl(); }
    const VarImplT& operator[](const Var& t_var) const { return lazy(t_var).impl(); }

    CtrImplT& operator[](const Ctr& t_ctr) { return lazy(t_ctr).impl(); }
    const CtrImplT& operator[](const Ctr& t_ctr) const { return lazy(t_ctr).impl(); }
};

template<class VarImplT, class CtrImplT, class QCtrImplT>
void idol::OptimizerWithLazyUpdates<VarImplT, CtrImplT, QCtrImplT>::update_var_ub(const Var &t_var) {
    lazy_update(t_var);
}

template<class VarImplT, class CtrImplT, class QCtrImplT>
void idol::OptimizerWithLazyUpdates<VarImplT, CtrImplT, QCtrImplT>::update_var_obj(const Var &t_var) {
    lazy_update(t_var);
}

template<class VarImplT, class CtrImplT, class QCtrImplT>
void idol::OptimizerWithLazyUpdates<VarImplT, CtrImplT, QCtrImplT>::update_var_lb(const Var &t_var) {
    lazy_update(t_var);
}

template<class VarImplT, class CtrImplT, class QCtrImplT>
void idol::OptimizerWithLazyUpdates<VarImplT, CtrImplT, QCtrImplT>::update_var_type(const Var &t_var) {
    lazy_update(t_var);
}

template<class VarImplT, class CtrImplT, class QCtrImplT>
void idol::OptimizerWithLazyUpdates<VarImplT, CtrImplT, QCtrImplT>::update_ctr_rhs(const Ctr &t_ctr) {
    lazy_update(t_ctr);
}

template<class VarImplT, class CtrImplT, class QCtrImplT>
void idol::OptimizerWithLazyUpdates<VarImplT, CtrImplT, QCtrImplT>::update_ctr_type(const Ctr &t_ctr) {
    lazy_update(t_ctr);
}

template<class VarImplT, class CtrImplT, class QCtrImplT>
void idol::OptimizerWithLazyUpdates<VarImplT, CtrImplT, QCtrImplT>::update_mat_coeff(const Ctr &t_ctr, const Var &t_var) {
    lazy_update_matrix(t_ctr, t_var, parent().get_mat_coeff(t_ctr, t_var));
}

template<class VarImplT, class CtrImplT, class QCtrImplT>
void idol::OptimizerWithLazyUpdates<VarImplT, CtrImplT, QCtrImplT>::update_obj_constant() {
    lazy_update_objective();
}

template<class VarImplT, class CtrImplT, class QCtrImplT>
void idol::OptimizerWithLazyUpdates<VarImplT, CtrImplT, QCtrImplT>::update_obj() {
    lazy_update_objective();
}

template<class VarImplT, class CtrImplT, class QCtrImplT>
void idol::OptimizerWithLazyUpdates<VarImplT, CtrImplT, QCtrImplT>::update_obj_sense() {
    lazy_update_objective_sense();
}


template<class VarImplT, class CtrImplT, class QCtrImplT>
void idol::OptimizerWithLazyUpdates<VarImplT, CtrImplT, QCtrImplT>::update_rhs() {
    lazy_update_rhs();
}

template<class VarImplT, class CtrImplT, class QCtrImplT>
idol::OptimizerWithLazyUpdates<VarImplT, CtrImplT, QCtrImplT>::OptimizerWithLazyUpdates(const Model &t_parent) : Optimizer(t_parent) {

}

template<class VarImplT, class CtrImplT, class QCtrImplT>
void idol::OptimizerWithLazyUpdates<VarImplT, CtrImplT, QCtrImplT>::build() {

    const auto& parent = this->parent();

    m_variables.reserve(parent.vars().size());
    for (const auto& var : parent.vars()) {
        add(var);
    }

    m_constraints.reserve(parent.ctrs().size());
    for (const auto& ctr : parent.ctrs()) {
        add(ctr);
    }

    m_qconstraints.reserve(parent.qctrs().size());
    for (const auto& qctr : parent.qctrs()) {
        add(qctr);
    }

    set_objective_to_be_updated();
    set_rhs_to_be_updated();

    hook_build();
}

template<class VarImplT, class CtrImplT, class QCtrImplT>
void idol::OptimizerWithLazyUpdates<VarImplT, CtrImplT, QCtrImplT>::write(const std::string &t_name) {
    update();
    hook_write(t_name);
}

template<class VarImplT, class CtrImplT, class QCtrImplT>
void idol::OptimizerWithLazyUpdates<VarImplT, CtrImplT, QCtrImplT>::update() {

    update_vars();

    update_ctrs();

    update_qctrs();

    if (is_objective_to_be_updated()) {
        hook_update_objective();
        set_objective_as_updated();
    }

    if (is_rhs_to_be_updated()) {
        hook_update_rhs();
        set_rhs_as_updated();
    }

    hook_update();

    m_is_initialized = true;
}

template<class VarImplT, class CtrImplT, class QCtrImplT>
void idol::OptimizerWithLazyUpdates<VarImplT, CtrImplT, QCtrImplT>::update_vars() {

    for (const unsigned int index : m_variables_to_update) {

        if (m_variables[index].has_impl()) {
            hook_update(m_variables[index].object());
        } else {
            auto impl = hook_add(m_variables[index].object(), m_is_initialized);
            m_variables[index].set_impl(std::move(impl));
        }

        m_variables[index].set_as_updated();
    }

    m_variables_to_update.clear();

}

template<class VarImplT, class CtrImplT, class QCtrImplT>
void idol::OptimizerWithLazyUpdates<VarImplT, CtrImplT, QCtrImplT>::update_ctrs() {

    for (const unsigned int index : m_constraints_to_update) {

        if (m_constraints[index].has_impl()) {
            hook_update(m_constraints[index].object());
        } else {
            auto impl = hook_add(m_constraints[index].object());
            m_constraints[index].set_impl(std::move(impl));
        }

        m_constraints[index].set_as_updated();

    }

    m_constraints_to_update.clear();

}

template<class VarImplT, class CtrImplT, class QCtrImplT>
void idol::OptimizerWithLazyUpdates<VarImplT, CtrImplT, QCtrImplT>::update_qctrs() {

    for (const unsigned int index : m_qconstraints_to_update) {

        if (m_qconstraints[index].has_impl()) {
            throw Exception("Updating quadratic constraints is not supported.");
            //hook_update(m_qconstraints[index].object());
        } else {
            auto impl = hook_add(m_qconstraints[index].object());
            m_qconstraints[index].set_impl(std::move(impl));
        }

        m_qconstraints[index].set_as_updated();

    }

    m_qconstraints_to_update.clear();

}

template<class VarImplT, class CtrImplT, class QCtrImplT>
void idol::OptimizerWithLazyUpdates<VarImplT, CtrImplT, QCtrImplT>::lazy_update_rhs() {
    set_rhs_to_be_updated();
}

template<class VarImplT, class CtrImplT, class QCtrImplT>
void idol::OptimizerWithLazyUpdates<VarImplT, CtrImplT, QCtrImplT>::lazy_update_objective() {
    set_objective_to_be_updated();
}

template<class VarImplT, class CtrImplT, class QCtrImplT>
void idol::OptimizerWithLazyUpdates<VarImplT, CtrImplT, QCtrImplT>::lazy_update_objective_sense() {
    hook_update_objective_sense();
}

template<class VarImplT, class CtrImplT, class QCtrImplT>
void idol::OptimizerWithLazyUpdates<VarImplT, CtrImplT, QCtrImplT>::lazy_update(const Var &t_var) {
    const unsigned int index = parent().get_var_index(t_var);
    m_variables_to_update.emplace_front(index);
    m_variables[index].set_as_to_be_updated(m_variables_to_update.begin());
}

template<class VarImplT, class CtrImplT, class QCtrImplT>
void idol::OptimizerWithLazyUpdates<VarImplT, CtrImplT, QCtrImplT>::lazy_update(const Ctr &t_ctr) {
    const unsigned int index = parent().get_ctr_index(t_ctr);
    m_constraints_to_update.emplace_front(index);
    m_constraints[index].set_as_to_be_updated(m_constraints_to_update.begin());
}

template<class VarImplT, class CtrImplT, class QCtrImplT>
void idol::OptimizerWithLazyUpdates<VarImplT, CtrImplT, QCtrImplT>::lazy_update_matrix(const Ctr &t_ctr, const Var &t_var, double t_constant) {
    hook_update_matrix(t_ctr, t_var, t_constant);
}

template<class VarImplT, class CtrImplT, class QCtrImplT>
void idol::OptimizerWithLazyUpdates<VarImplT, CtrImplT, QCtrImplT>::add(const Ctr &t_ctr) {

    if (m_is_initialized) {
        update_vars();
    }

    const unsigned int index = parent().get_ctr_index(t_ctr);
    m_constraints_to_update.emplace_front(index);
    m_constraints.emplace_back(t_ctr, m_constraints_to_update.begin());
}

template<class VarImplT, class CtrImplT, class QCtrImplT>
void idol::OptimizerWithLazyUpdates<VarImplT, CtrImplT, QCtrImplT>::add(const idol::QCtr &t_ctr) {

    if (m_is_initialized) {
        update_vars();
    }

    const unsigned int index = parent().get_qctr_index(t_ctr);
    m_qconstraints_to_update.emplace_front(index);
    m_qconstraints.emplace_back(t_ctr, m_qconstraints_to_update.begin());

}

template<class VarImplT, class CtrImplT, class QCtrImplT>
void idol::OptimizerWithLazyUpdates<VarImplT, CtrImplT, QCtrImplT>::add(const Var &t_var) {

    if (m_is_initialized) {
        update_ctrs();
    }

    const unsigned int index = parent().get_var_index(t_var);
    m_variables_to_update.emplace_front(index);
    m_variables.emplace_back(t_var, m_variables_to_update.begin());
}

template<class VarImplT, class CtrImplT, class QCtrImplT>
void idol::OptimizerWithLazyUpdates<VarImplT, CtrImplT, QCtrImplT>::remove(const Var& t_var) {

    update();

    const unsigned index = parent().get_var_index(t_var);

    if (m_variables[index].has_impl()) {
        hook_remove(m_variables[index].object());
    }

    m_variables[index] = std::move(m_variables.back());
    m_variables.pop_back();
}

template<class VarImplT, class CtrImplT, class QCtrImplT>
void idol::OptimizerWithLazyUpdates<VarImplT, CtrImplT, QCtrImplT>::remove(const Ctr& t_ctr) {

    update();

    const unsigned index = parent().get_ctr_index(t_ctr);

    if (m_constraints[index].has_impl()) {
        hook_remove(m_constraints[index].object());
    }

    m_constraints[index] = std::move(m_constraints.back());
    m_constraints.pop_back();
}

template<class VarImplT, class CtrImplT, class QCtrImplT>
void idol::OptimizerWithLazyUpdates<VarImplT, CtrImplT, QCtrImplT>::remove(const idol::QCtr &t_ctr) {

    update();

    const unsigned index = parent().get_qctr_index(t_ctr);

    if (m_qconstraints[index].has_impl()) {
        hook_remove(m_qconstraints[index].object());
    }

    m_qconstraints[index] = std::move(m_qconstraints.back());
    m_qconstraints.pop_back();
}

#endif //IDOL_OPTIMIZERWITHLAZYUPDATES_H
