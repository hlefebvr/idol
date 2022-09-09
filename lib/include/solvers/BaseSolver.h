//
// Created by henri on 09/09/22.
//

#ifndef OPTIMIZE_BASESOLVER_H
#define OPTIMIZE_BASESOLVER_H

#include <vector>
#include "modeling/Types.h"
#include "../modeling/models/Model.h"

template<class VarT, class CtrT>
class BaseSolver : public Listener {
    std::vector<VarT> m_variables;
    std::vector<CtrT> m_constraints;
protected:
    explicit BaseSolver(Model& t_model);

    void on_add(const Var &t_var) final;
    void on_add(const Ctr &t_ctr) final;
    void on_remove(const Var& t_var) final;
    void on_remove(const Ctr& t_ctr) final;
    void on_update_objective(const Var &t_var, const Coefficient &t_coeff) final;
    void on_update_rhs(const Ctr &t_ctr, const Coefficient &t_coeff) final;
    void on_update_coefficient(const Ctr &t_ctr, const Var &t_var, const Coefficient &t_coefficient) final;
    void on_update_lb(const Var &t_var, double t_lb) final;
    void on_update_ub(const Var &t_var, double t_ub) final;
    void on_update_type(const Var &t_var, VarType t_type) final;
    void on_update_type(const Ctr &t_ctr, CtrType t_type) final;

    void init_model(const Model& t_model);
    virtual VarT create_variable(const Var& t_var) = 0;
    virtual CtrT create_constraint(const Ctr& t_ctr) = 0;
    virtual void fill_column(const Var& t_var) = 0;
    virtual void fill_row(const Ctr& t_ctr) = 0;
    virtual void remove_variable(const Var& t_var) = 0;
    virtual void remove_constraint(const Ctr& t_ctr) = 0;
    virtual void set_objective_coefficient(const Var &t_var, const Coefficient &t_coeff) = 0;
    virtual void set_rhs(const Ctr &t_ctr, const Coefficient &t_coeff) = 0;
    virtual void set_coefficient(const Ctr &t_ctr, const Var &t_var, const Coefficient &t_coefficient) = 0;
    virtual void set_lb(const Var &t_var, double t_lb) = 0;
    virtual void set_ub(const Var &t_var, double t_ub) = 0;
    virtual void set_type(const Var &t_var, VarType t_type) = 0;
    virtual void set_type(const Ctr &t_ctr, CtrType t_type) = 0;
public:

    VarT& get(const Var& t_var);
    const VarT& get(const Var& t_var) const;
    CtrT& get(const Ctr& t_ctr);
    const CtrT& get(const Ctr& t_ctr) const;

    virtual void write(const std::string& t_filename) = 0;
    virtual void solve() = 0;
};

template<class VarT, class CtrT>
void BaseSolver<VarT, CtrT>::on_add(const Var &t_var) {
    m_variables.template emplace_back(create_variable(t_var));
    fill_column(t_var);
}

template<class VarT, class CtrT>
void BaseSolver<VarT, CtrT>::on_add(const Ctr &t_ctr) {
    m_constraints.template emplace_back(create_constraint(t_ctr));
    fill_row(t_ctr);
}

template<class VarT, class CtrT>
VarT &BaseSolver<VarT, CtrT>::get(const Var &t_var) {
    return m_variables[t_var.index()];
}

template<class VarT, class CtrT>
const VarT &BaseSolver<VarT, CtrT>::get(const Var &t_var) const {
    return m_variables[t_var.index()];
}

template<class VarT, class CtrT>
CtrT &BaseSolver<VarT, CtrT>::get(const Ctr &t_ctr) {
    return m_constraints[t_ctr.index()];
}

template<class VarT, class CtrT>
const CtrT &BaseSolver<VarT, CtrT>::get(const Ctr &t_ctr) const {
    return m_constraints[t_ctr.index()];
}

template<class VarT, class CtrT>
void BaseSolver<VarT, CtrT>::on_remove(const Var &t_var) {
    remove_variable(t_var);

    m_variables[t_var.index()] = std::move(m_variables.back());
    m_variables.pop_back();
}

template<class VarT, class CtrT>
void BaseSolver<VarT, CtrT>::on_remove(const Ctr &t_ctr) {
    remove_constraint(t_ctr);

    m_constraints[t_ctr.index()] = std::move(m_constraints.back());
    m_constraints.pop_back();
}

template<class VarT, class CtrT>
void BaseSolver<VarT, CtrT>::on_update_objective(const Var &t_var, const Coefficient &t_coeff) {
    set_objective_coefficient(t_var, t_coeff);
}

template<class VarT, class CtrT>
void BaseSolver<VarT, CtrT>::on_update_rhs(const Ctr &t_ctr, const Coefficient &t_coeff) {
    set_rhs(t_ctr, t_coeff);
}

template<class VarT, class CtrT>
void
BaseSolver<VarT, CtrT>::on_update_coefficient(const Ctr &t_ctr, const Var &t_var, const Coefficient &t_coefficient) {
    set_coefficient(t_ctr, t_var, t_coefficient);
}

template<class VarT, class CtrT>
void BaseSolver<VarT, CtrT>::on_update_lb(const Var &t_var, double t_lb) {
    set_lb(t_var, t_lb);
}

template<class VarT, class CtrT>
void BaseSolver<VarT, CtrT>::on_update_ub(const Var &t_var, double t_ub) {
    set_ub(t_var, t_ub);
}

template<class VarT, class CtrT>
void BaseSolver<VarT, CtrT>::on_update_type(const Var &t_var, VarType t_type) {
    set_type(t_var, t_type);
}

template<class VarT, class CtrT>
void BaseSolver<VarT, CtrT>::on_update_type(const Ctr &t_ctr, CtrType t_type) {
    set_type(t_ctr, t_type);
}

template<class VarT, class CtrT>
BaseSolver<VarT, CtrT>::BaseSolver(Model &t_model) {
    t_model.add_listener(*this);
}

template<class VarT, class CtrT>
void BaseSolver<VarT, CtrT>::init_model(const Model &t_model) {
    for (const auto& var : t_model.variables()) {
        m_variables.template emplace_back(create_variable(var));
    }

    for (const auto& ctr : t_model.constraints()) {
        m_constraints.template emplace_back(create_constraint(ctr));
        fill_row(ctr);
    }

}

#endif //OPTIMIZE_BASESOLVER_H
