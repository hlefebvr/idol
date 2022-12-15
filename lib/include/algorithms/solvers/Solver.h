//
// Created by henri on 11/10/22.
//

#ifndef OPTIMIZE_SOLVER2_H
#define OPTIMIZE_SOLVER2_H

#include "../Algorithm.h"
#include "../../modeling/models/Model.h"
#include "../callbacks/Callback.h"
#include "../../modeling/variables/Attributes_Var.h"
#include "../../modeling/models/Attributes_Model.h"
#include "../../algorithms/parameters/Logs.h"
#include <cassert>
#include <sstream>

template<class T, class SolverImplT>
class Future {
    bool m_with_collaterals = true;
    std::optional<T> m_object;
    std::optional<SolverImplT> m_impl;
public:
    Future() = default;
    explicit Future(const T& t_object, bool t_with_collaterals) : m_object(t_object), m_with_collaterals(t_with_collaterals) {}

    [[nodiscard]] bool has_impl() const { return m_impl.has_value(); }
    void set_impl(const SolverImplT& t_impl) { m_impl = t_impl; }
    SolverImplT& impl() { return m_impl.value(); }
    [[nodiscard]] const SolverImplT& impl() const { return m_impl.value(); }

    const T& object() const { return m_object.value(); }

    [[nodiscard]] bool with_collaterals() const { return m_with_collaterals; }
};

template<class VarT, class CtrT>
class Solver : public Algorithm {
    Model& m_src_model;

    const unsigned int m_buffer_size = 200;
    std::vector<Future<Var, VarT>> m_variables;
    std::vector<Future<Ctr, CtrT>> m_constraints;

    bool m_is_built = false;
    bool m_update_objective = false;
    std::list<Ctr> m_constraints_to_update;
    std::list<Var> m_variables_to_update;
protected:
    void add_future(const Var& t_var, bool t_with_collaterals = true);
    void remove_future(const Var& t_var);
    [[nodiscard]] bool has_future(const Var& t_ctr) const;

    void add_future(const Ctr& t_ctr, bool t_with_collaterals = true);
    void remove_future(const Ctr& t_ctr);
    [[nodiscard]] bool has_future(const Ctr& t_ctr) const;

    std::list<std::unique_ptr<Callback>> m_callbacks; // TODO make this private

    void add_future_obj();

    void save_callback(Callback* t_cb);

    virtual void update(const Var& t_var, VarT& t_impl) = 0;
    virtual void update(const Ctr& t_var, CtrT& t_impl) = 0;
    virtual void update_obj() = 0;
    virtual void remove(const Var& t_var, VarT& t_impl) = 0;
    virtual void remove(const Ctr& t_var, CtrT& t_impl) = 0;
    virtual VarT create(const Var& t_var, bool t_with_collaterals) = 0;
    virtual CtrT create(const Ctr& t_ctr, bool t_with_collaterals) = 0;

    [[nodiscard]] double value(const Constant& t_constant) const;

    AttributeManager &attribute_delegate(const Attribute &t_attribute) override { return m_src_model; }
    AttributeManager &attribute_delegate(const Attribute &t_attribute, const Var &t_object) override { return m_src_model; }
    AttributeManager &attribute_delegate(const Attribute &t_attribute, const Ctr &t_object) override { return m_src_model; }

    virtual void update_variables();
    virtual void update_constraints();
public:
    explicit Solver(Model& t_model);

    [[nodiscard]] int sense() const override;

    [[nodiscard]] const Model& model() const { return m_src_model; }

    [[nodiscard]] Model& model() { return m_src_model; }

    virtual void update();

    Future<Var, VarT>& future(const Var& t_var) { return m_variables[t_var.index()]; }

    const Future<Var, VarT>& future(const Var& t_var) const { return m_variables[t_var.index()]; }

    Future<Ctr, CtrT>& future(const Ctr& t_ctr) { return m_constraints[t_ctr.index()];}

    const Future<Ctr, CtrT>& future(const Ctr& t_ctr) const { return m_constraints[t_ctr.index()]; }

    void remove(const Var &t_variable) override;

    void remove(const Ctr &t_constraint) override;

    Var add_var(TempVar&& t_temporary_variable) override;

    Ctr add_ctr(TempCtr&& t_temporary_constraint) override;

    Callback& callback() { return *m_callbacks.front(); }

    using Algorithm::set;
    using Algorithm::get;

    void set(const AttributeWithTypeAndArguments<Expr<Var, Var>, void> &t_attr, Expr<Var, Var> &&t_value) override;
};


template<class VarT, class CtrT>
Solver<VarT, CtrT>::Solver(Model &t_model) : m_src_model(t_model) {

}

template<class VarT, class CtrT>
void Solver<VarT, CtrT>::add_future(const Var &t_var, bool t_with_collaterals) {

    const unsigned int index = t_var.index();
    const unsigned int size = m_variables.size();

    if (index >= size) {
        m_variables.resize(size + m_buffer_size );
    }

    m_variables.at(index) = Future<Var, VarT>(t_var, t_with_collaterals);
    m_variables_to_update.template emplace_back(t_var);
}

template<class VarT, class CtrT>
void Solver<VarT, CtrT>::add_future(const Ctr &t_ctr, bool t_with_collaterals) {

    const unsigned int index = t_ctr.index();
    const unsigned int size = m_constraints.size();

    if (index >= size) {
        m_constraints.resize(size + m_buffer_size );
    }

    m_constraints.at(index) = Future<Ctr, CtrT>(t_ctr, t_with_collaterals);
    m_constraints_to_update.template emplace_back(t_ctr);
}

template<class VarT, class CtrT>
void Solver<VarT, CtrT>::remove_future(const Var &t_var) {
    m_variables[t_var.index()] = Future<Var, VarT>();
}

template<class VarT, class CtrT>
void Solver<VarT, CtrT>::remove_future(const Ctr &t_ctr) {
    m_constraints[t_ctr.index()] = Future<Ctr, CtrT>();
}

template<class VarT, class CtrT>
double Solver<VarT, CtrT>::value(const Constant &t_constant) const {
    if (!t_constant.is_numerical()) {
        std::stringstream ss;
        ss << "Constant " << t_constant << ", found in external solver, was "
                                           "implicitly converted to " << t_constant.numerical();
        throw Exception(ss.str());
    }
    return t_constant.numerical();
}

template<class VarT, class CtrT>
bool Solver<VarT, CtrT>::has_future(const Ctr &t_ctr) const {
    const auto index = t_ctr.index();
    if (index >= m_constraints.size()) {
        return false;
    }
    return m_constraints[index].object().index() == index;
}

template<class VarT, class CtrT>
bool Solver<VarT, CtrT>::has_future(const Var &t_ctr) const {
    const auto index = t_ctr.index();
    if (index >= m_variables.size()) {
        return false;
    }
    return m_variables[index].object().index() == index;
}

template<class VarT, class CtrT>
void Solver<VarT, CtrT>::update() {

    update_variables();
    update_constraints();

    if (m_update_objective) {
        update_obj();
        m_update_objective = false;
    }

    m_is_built = true;

}

template<class VarT, class CtrT>
void Solver<VarT, CtrT>::update_constraints() {

    for (const auto& ctr : m_constraints_to_update) {
        if (!m_src_model.get(Attr::Ctr::Status, ctr)) { continue; }
        if (auto& f = future(ctr) ; !f.has_impl()) {
            auto impl = create(ctr, true);
            f.set_impl(impl);
        } else {
            update(ctr, f.impl());
        }
    }

    m_constraints_to_update.clear();
    m_is_built = true;

}

template<class VarT, class CtrT>
void Solver<VarT, CtrT>::update_variables() {

    for (const auto& var : m_variables_to_update) {
        if (!m_src_model.get(Attr::Var::Status, var)) { continue; }
        if (auto& f = future(var) ; !f.has_impl()) {
            auto impl = create(var, m_is_built);
            f.set_impl(impl);
        } else {
            update(var, f.impl());
        }
    }

    m_variables_to_update.clear();
    m_is_built = true;
}
template<class VarT, class CtrT>
void Solver<VarT, CtrT>::remove(const Var &t_variable) {
    if (auto& f = future(t_variable) ; f.has_impl()) {
        remove(t_variable, f.impl());
    }
    remove_future(t_variable);
    m_src_model.remove(t_variable);
}

template<class VarT, class CtrT>
void Solver<VarT, CtrT>::remove(const Ctr &t_constraint) {
    if (auto& f = future(t_constraint) ; f.has_impl()) {
        remove(t_constraint, f.impl());
    }
    remove_future(t_constraint);
    m_src_model.remove(t_constraint);
}

template<class VarT, class CtrT>
Var Solver<VarT, CtrT>::add_var(TempVar&& t_temporary_variable) {
    update_constraints();
    auto result = m_src_model.add_var(std::move(t_temporary_variable));
    add_future(result);
    return result;
}

template<class VarT, class CtrT>
Ctr Solver<VarT, CtrT>::add_ctr(TempCtr&& t_temporary_constraint) {
    update_variables();
    auto result = m_src_model.add_ctr(std::move(t_temporary_constraint));
    add_future(result);
    return result;
}

template<class VarT, class CtrT>
void Solver<VarT, CtrT>::add_future_obj() {
    m_update_objective = true;
}

template<class VarT, class CtrT>
void Solver<VarT, CtrT>::save_callback(Callback *t_cb) {
    m_callbacks.template emplace_back(t_cb);
}

template<class VarT, class CtrT>
void Solver<VarT, CtrT>::set(const AttributeWithTypeAndArguments<Expr<Var, Var>, void> &t_attr, Expr<Var, Var> &&t_value) {

    if (t_attr == Attr::Obj::Expr) {
        add_future_obj();
    }

    Delegate::set(t_attr, std::move(t_value));
}

template<class VarT, class CtrT>
int Solver<VarT, CtrT>::sense() const {
    return m_src_model.get(Attr::Obj::Sense);
}

#endif //OPTIMIZE_SOLVER2_H
