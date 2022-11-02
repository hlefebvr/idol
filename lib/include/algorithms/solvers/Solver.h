//
// Created by henri on 11/10/22.
//

#ifndef OPTIMIZE_SOLVER2_H
#define OPTIMIZE_SOLVER2_H

#include "../Algorithm.h"
#include "../../modeling/models/Model.h"
#include "../attributes/Attributes_Base.h"
#include "../callbacks/Callback.h"
#include <cassert>

template<class T, class SolverImplT>
class Future {
    bool m_with_collaterals;
    T m_object;
    std::optional<SolverImplT> m_impl;
public:
    explicit Future(const T& t_object, bool t_with_collaterals) : m_object(t_object), m_with_collaterals(t_with_collaterals) {}

    [[nodiscard]] bool has_impl() const { return m_impl.has_value(); }
    void set_impl(const SolverImplT& t_impl) { m_impl = t_impl; }
    SolverImplT& impl() { return m_impl.value(); }
    [[nodiscard]] const SolverImplT& impl() const { return m_impl.value(); }

    const T& object() const { return m_object; }

    [[nodiscard]] bool with_collaterals() const { return m_with_collaterals; }
};

template<class VarT, class CtrT>
class Solver : public Algorithm {
    Model& m_src_model;
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
public:
    explicit Solver(Model& t_model);

    ObjSense sense() const override;

    [[nodiscard]] const Model& model() const { return m_src_model; }

    [[nodiscard]] Model& model() { return m_src_model; }

    virtual void update();

    Future<Var, VarT>& future(const Var& t_var) { return m_variables[t_var.index()]; }

    const Future<Var, VarT>& future(const Var& t_var) const { return m_variables[t_var.index()]; }

    Future<Ctr, CtrT>& future(const Ctr& t_ctr) { return m_constraints[t_ctr.index()];}

    const Future<Ctr, CtrT>& future(const Ctr& t_ctr) const { return m_constraints[t_ctr.index()]; }

    void remove(const Var &t_variable) override;

    void remove(const Ctr &t_constraint) override;

    Var add_column(TempVar t_temporary_variable) override;

    Ctr add_row(TempCtr t_temporary_constraint) override;

    void update_objective(const Expr<Var> &t_objective) override;
};

template<class VarT, class CtrT>
Solver<VarT, CtrT>::Solver(Model &t_model) : m_src_model(t_model) {

}

template<class VarT, class CtrT>
void Solver<VarT, CtrT>::add_future(const Var &t_var, bool t_with_collaterals) {
    m_variables.template emplace_back(Future<Var, VarT>(t_var, t_with_collaterals));
    m_variables_to_update.template emplace_back(t_var);
}

template<class VarT, class CtrT>
void Solver<VarT, CtrT>::add_future(const Ctr &t_ctr, bool t_with_collaterals) {
    m_constraints.template emplace_back(Future<Ctr, CtrT>(t_ctr, t_with_collaterals));
    m_constraints_to_update.template emplace_back(t_ctr);
}

template<class VarT, class CtrT>
void Solver<VarT, CtrT>::remove_future(const Var &t_var) {
    m_variables[t_var.index()] = std::move(m_variables.back());
    m_variables.pop_back();
}

template<class VarT, class CtrT>
void Solver<VarT, CtrT>::remove_future(const Ctr &t_ctr) {
    m_constraints[t_ctr.index()] = std::move(m_constraints.back());
    m_constraints.pop_back();
}

template<class VarT, class CtrT>
double Solver<VarT, CtrT>::value(const Constant &t_constant) const {
    if (!t_constant.is_numerical()) {
        EASY_LOG(Warn, "external-solver", "Constant " << t_constant << ", found in external solver, was "
                                          "implicitly converted to " << t_constant.numerical());
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

    for (const auto& var : m_variables_to_update) {
        if (var.status() == Removed) { continue; }
        if (auto& f = future(var) ; !f.has_impl()) {
            auto impl = create(var, m_is_built);
            f.set_impl(impl);
        } else {
            update(var, f.impl());
        }
    }

    for (const auto& ctr : m_constraints_to_update) {
        if (ctr.status() == Removed) { continue; }
        if (auto& f = future(ctr) ; !f.has_impl()) {
            auto impl = create(ctr, true);
            f.set_impl(impl);
        } else {
            update(ctr, f.impl());
        }
    }

    if (m_update_objective) {
        update_obj();
        m_update_objective = false;
    }

    m_variables_to_update.clear();
    m_constraints_to_update.clear();
    m_is_built = true;

}

template<class VarT, class CtrT>
void Solver<VarT, CtrT>::remove(const Var &t_variable) {
    if (auto& f = future(t_variable) ; f.has_impl()) {
        remove(t_variable, future(t_variable).impl());
    }
    remove_future(t_variable);
    m_src_model.remove(t_variable);
}

template<class VarT, class CtrT>
void Solver<VarT, CtrT>::remove(const Ctr &t_constraint) {
    if (auto& f = future(t_constraint) ; f.has_impl()) {
        remove(t_constraint, future(t_constraint).impl());
    }
    remove_future(t_constraint);
    m_src_model.remove(t_constraint);
}

template<class VarT, class CtrT>
Var Solver<VarT, CtrT>::add_column(TempVar t_temporary_variable) {
    auto result = m_src_model.add_variable(std::move(t_temporary_variable));
    add_future(result);
    return result;
}

template<class VarT, class CtrT>
Ctr Solver<VarT, CtrT>::add_row(TempCtr t_temporary_constraint) {
    auto result = m_src_model.add_constraint(std::move(t_temporary_constraint));
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
void Solver<VarT, CtrT>::update_objective(const Expr<Var> &t_objective) {
    m_src_model.update_obj(t_objective);
    add_future_obj();
}

template<class VarT, class CtrT>
ObjSense Solver<VarT, CtrT>::sense() const {
    return m_src_model.sense();
}

#endif //OPTIMIZE_SOLVER2_H
