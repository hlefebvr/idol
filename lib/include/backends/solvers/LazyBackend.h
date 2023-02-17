//
// Created by henri on 31/01/23.
//

#ifndef IDOL_LAZYBACKEND_H
#define IDOL_LAZYBACKEND_H

#include "backends/Backend.h"

#include "modeling/models/Model.h"

#include <vector>
#include <list>
#include <optional>

template<class T, class ImplT>
class Lazy {
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

template<class VarImplT, class CtrImplT>
class LazyBackend : public Backend {
    std::vector<Lazy<Var, VarImplT>> m_variables;
    std::list<unsigned int> m_variables_to_update;

    std::vector<Lazy<Ctr, CtrImplT>> m_constraints;
    std::list<unsigned int> m_constraints_to_update;

    bool m_is_initialized = false;
    bool m_is_objective_to_be_updated = true;
    bool m_is_rhs_to_be_updated = true;

    void update_vars();
    void update_ctrs();

    void update_objective_sense();
    void update_matrix(const Ctr& t_ctr, const Var &t_var, const Constant &t_constant);
    void update(const Var& t_var);
    void update(const Ctr& t_ctr);
    void update_objective();
    void update_rhs();
protected:
    explicit LazyBackend(const AbstractModel& t_parent);

    void initialize() final;
    virtual void hook_initialize() = 0;

    void optimize() final;
    virtual void hook_optimize() = 0;

    void write(const std::string &t_name) final;
    virtual void hook_write(const std::string& t_name) = 0;

    void add(const Var &t_var) final;
    virtual VarImplT hook_add(const Var& t_var, bool t_add_column) = 0;

    void add(const Ctr &t_ctr) final;
    virtual CtrImplT hook_add(const Ctr& t_ctr) = 0;

    virtual void hook_update_objective_sense() = 0;
    virtual void hook_update_matrix(const Ctr &t_ctr, const Var &t_var, const Constant &t_constant) = 0;

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

    auto& lazy(const Var& t_var) { return m_variables[parent().get(Attr::Var::Index, t_var)]; }
    const auto& lazy(const Var& t_var) const { return m_variables[parent().get(Attr::Var::Index, t_var)]; }

    auto& lazy(const Ctr& t_ctr) { return m_constraints[parent().get(Attr::Ctr::Index, t_ctr)]; }
    const auto& lazy(const Ctr& t_ctr) const { return m_constraints[parent().get(Attr::Ctr::Index, t_ctr)]; }

    void set_objective_to_be_updated() { m_is_objective_to_be_updated = true; }
    [[nodiscard]] bool is_objective_to_be_updated() const { return m_is_objective_to_be_updated; }
    void set_objective_as_updated() { m_is_objective_to_be_updated = false; }

    void set_rhs_to_be_updated() { m_is_rhs_to_be_updated = true; }
    [[nodiscard]] bool is_rhs_to_be_updated() const { return m_is_rhs_to_be_updated; }
    void set_rhs_as_updated() { m_is_rhs_to_be_updated = false; }

    using Backend::set;
    using Backend::get;

    // Model
    void set(const Req<Constant, Ctr, Var> &t_attr, const Ctr &t_ctr, const Var &t_var, Constant && t_value) override;
    void set(const Req<Constant, void> &t_attr, Constant &&t_value) override;
    void set(const Req<int, void> &t_attr, int t_value) override;

    // Variables
    void set(const Req<double, Var> &t_attr, const Var &t_var, double t_value) override;
    void set(const Req<Expr<Var, Var>, void> &t_attr, Expr<Var, Var> &&t_value) override;
    void set(const Req<Constant, Var> &t_attr, const Var &t_var, Constant &&t_value) override;
    void set(const Req<int, Var> &t_attr, const Var &t_var, int t_value) override;

    // Constraints
    void set(const Req<Constant, Ctr> &t_attr, const Ctr &t_ctr, Constant &&t_value) override;
    void set(const Req<LinExpr<Ctr>, void> &t_attr, LinExpr<Ctr> &&t_value) override;
    void set(const Req<int, Ctr> &t_attr, const Ctr &t_ctr, int t_value) override;
public:
    VarImplT& operator[](const Var& t_var) { return lazy(t_var).impl(); }
    const VarImplT& operator[](const Var& t_var) const { return lazy(t_var).impl(); }

    CtrImplT& operator[](const Ctr& t_ctr) { return lazy(t_ctr).impl(); }
    const CtrImplT& operator[](const Ctr& t_ctr) const { return lazy(t_ctr).impl(); }
};

template<class VarImplT, class CtrImplT>
LazyBackend<VarImplT, CtrImplT>::LazyBackend(const AbstractModel &t_parent) : Backend(t_parent) {

}

template<class VarImplT, class CtrImplT>
void LazyBackend<VarImplT, CtrImplT>::initialize() {

    std::cout << "Initialize" << std::endl;

    for (const auto& var : parent().vars()) {
        add(var);
    }

    for (const auto& ctr : parent().ctrs()) {
        add(ctr);
    }

    set_objective_to_be_updated();
    set_rhs_to_be_updated();

    hook_initialize();
}

template<class VarImplT, class CtrImplT>
void LazyBackend<VarImplT, CtrImplT>::write(const std::string &t_name) {
    update();
    hook_write(t_name);
}

template<class VarImplT, class CtrImplT>
void LazyBackend<VarImplT, CtrImplT>::update() {

    update_vars();

    update_ctrs();

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

template<class VarImplT, class CtrImplT>
void LazyBackend<VarImplT, CtrImplT>::update_vars() {

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

template<class VarImplT, class CtrImplT>
void LazyBackend<VarImplT, CtrImplT>::update_ctrs() {

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

template<class VarImplT, class CtrImplT>
void LazyBackend<VarImplT, CtrImplT>::update_rhs() {
    set_rhs_to_be_updated();
}

template<class VarImplT, class CtrImplT>
void LazyBackend<VarImplT, CtrImplT>::update_objective() {
    set_objective_to_be_updated();
}

template<class VarImplT, class CtrImplT>
void LazyBackend<VarImplT, CtrImplT>::update_objective_sense() {
    hook_update_objective_sense();
}

template<class VarImplT, class CtrImplT>
void LazyBackend<VarImplT, CtrImplT>::update(const Var &t_var) {
    const unsigned int index = parent().get(Attr::Var::Index, t_var);
    m_variables_to_update.emplace_front(index);
    m_variables[index].set_as_to_be_updated(m_variables_to_update.begin());
}

template<class VarImplT, class CtrImplT>
void LazyBackend<VarImplT, CtrImplT>::update(const Ctr &t_ctr) {
    const unsigned int index = parent().get(Attr::Ctr::Index, t_ctr);
    m_constraints_to_update.emplace_front(index);
    m_constraints[index].set_as_to_be_updated(m_constraints_to_update.begin());
}

template<class VarImplT, class CtrImplT>
void LazyBackend<VarImplT, CtrImplT>::update_matrix(const Ctr &t_ctr, const Var &t_var, const Constant &t_constant) {
    hook_update_matrix(t_ctr, t_var, t_constant);
}

template<class VarImplT, class CtrImplT>
void LazyBackend<VarImplT, CtrImplT>::optimize() {
    update();
    hook_optimize();
}

template<class VarImplT, class CtrImplT>
void LazyBackend<VarImplT, CtrImplT>::add(const Ctr &t_ctr) {
    if (m_is_initialized) {
        update_vars();
    }
    const unsigned int index = m_constraints.size();
    m_constraints_to_update.emplace_front(index);
    m_constraints.emplace_back(t_ctr, m_constraints_to_update.begin());
}

template<class VarImplT, class CtrImplT>
void LazyBackend<VarImplT, CtrImplT>::add(const Var &t_var) {
    if (m_is_initialized) {
        update_ctrs();
    }
    const unsigned int index = m_variables.size();
    m_variables_to_update.emplace_front(index);
    m_variables.emplace_back(t_var, m_variables_to_update.begin());
}

template<class VarImplT, class CtrImplT>
void LazyBackend<VarImplT, CtrImplT>::remove(const Var& t_var) {

    const unsigned index = parent().get(Attr::Var::Index, t_var);

    if (m_variables[index].is_to_be_updated()) {
        m_variables_to_update.erase(m_variables[index].to_be_updated_flag());
    }

    if (m_variables[index].has_impl()) {
        hook_remove(m_variables[index].object());
    }

    m_variables[index] = std::move(m_variables.back());
    m_variables.pop_back();
}

template<class VarImplT, class CtrImplT>
void LazyBackend<VarImplT, CtrImplT>::remove(const Ctr& t_ctr) {

    const unsigned index = parent().get(Attr::Ctr::Index, t_ctr);

    if (m_constraints[index].is_to_be_updated()) {
        m_constraints_to_update.erase(m_constraints[index].to_be_updated_flag());
    }

    if (m_constraints[index].has_impl()) {
        hook_remove(m_constraints[index].object());
    }

    m_constraints[index] = std::move(m_constraints.back());
    m_constraints.pop_back();
}


template<class VarImplT, class CtrImplT>
void LazyBackend<VarImplT, CtrImplT>::set(const Req<int, Ctr> &t_attr, const Ctr &t_ctr, int t_value) {

    if (t_attr == Attr::Ctr::Type) {
        update(t_ctr);
        return;
    }

    Base::set(t_attr, t_ctr, t_value);
}

template<class VarImplT, class CtrImplT>
void LazyBackend<VarImplT, CtrImplT>::set(const Req<int, void> &t_attr, int t_value) {

    if (t_attr == Attr::Obj::Sense) {
        update_objective_sense();
        return;
    }

    Base::set(t_attr, t_value);
}

template<class VarImplT, class CtrImplT>
void LazyBackend<VarImplT, CtrImplT>::set(const Req<int, Var> &t_attr, const Var &t_var, int t_value) {

    if (t_attr == Attr::Var::Type) {
        update(t_var);
        return;
    }

    Base::set(t_attr, t_var, t_value);
}

template<class VarImplT, class CtrImplT>
void LazyBackend<VarImplT, CtrImplT>::set(const Req<Constant, Var> &t_attr, const Var &t_var, Constant &&t_value) {

    if (t_attr == Attr::Var::Obj) {
        update(t_var);
        return;
    }

    Base::set(t_attr, t_var, t_value);
}

template<class VarImplT, class CtrImplT>
void LazyBackend<VarImplT, CtrImplT>::set(const Req<Constant, void> &t_attr, Constant &&t_value) {

    if (t_attr == Attr::Obj::Const) {
        update_objective();
        return;
    }

    Base::set(t_attr, t_value);
}

template<class VarImplT, class CtrImplT>
void LazyBackend<VarImplT, CtrImplT>::set(const Req<LinExpr<Ctr>, void> &t_attr, LinExpr<Ctr> &&t_value) {

    if (t_attr == Attr::Rhs::Expr) {
        update_rhs();
        return;
    }

    Base::set(t_attr, t_value);
}

template<class VarImplT, class CtrImplT>
void LazyBackend<VarImplT, CtrImplT>::set(const Req<Expr<Var, Var>, void> &t_attr, Expr<Var, Var> &&t_value) {

    if (t_attr == Attr::Obj::Expr) {
        update_objective();
        return;
    }

    Base::set(t_attr, t_value);
}

template<class VarImplT, class CtrImplT>
void LazyBackend<VarImplT, CtrImplT>::set(const Req<Constant, Ctr> &t_attr, const Ctr &t_ctr, Constant &&t_value) {

    if (t_attr == Attr::Ctr::Rhs) {
        update(t_ctr);
        return;
    }

    Base::set(t_attr, t_ctr, t_value);
}

template<class VarImplT, class CtrImplT>
void LazyBackend<VarImplT, CtrImplT>::set(const Req<Constant, Ctr, Var> &t_attr, const Ctr &t_ctr, const Var &t_var, Constant &&t_value) {

    if (t_attr == Attr::Matrix::Coeff) {
        update_matrix(t_ctr, t_var, t_value);
        return;
    }

    Base::set(t_attr, t_ctr, t_var, t_value);
}

template<class VarImplT, class CtrImplT>
void LazyBackend<VarImplT, CtrImplT>::set(const Req<double, Var> &t_attr, const Var &t_var, double t_value) {

    if (t_attr == Attr::Var::Lb || t_attr == Attr::Var::Ub) {
        update(t_var);
        return;
    }

    Base::set(t_attr, t_var, t_value);
}

#endif //IDOL_LAZYBACKEND_H
