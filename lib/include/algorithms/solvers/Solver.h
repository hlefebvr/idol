//
// Created by henri on 11/10/22.
//

#ifndef OPTIMIZE_SOLVER2_H
#define OPTIMIZE_SOLVER2_H

#include "../Algorithm.h"
#include "../../modeling/models/Model.h"
#include "../attributes/Attributes_Base.h"

template<class VarT, class CtrT>
class Solver : public Algorithm {
    Model& m_src_model;
    std::vector<VarT> m_variables;
    std::vector<CtrT> m_constraints;

protected:
    void add_variable_impl(const VarT& t_var);
    void add_constraint_impl(const CtrT& t_ctr);
    void remove_variable_impl(const Var& t_var);
    void remove_constraint_impl(const Ctr& t_ctr);
    [[nodiscard]] double value(const Constant& t_constant) const;
public:
    explicit Solver(Model& t_model);

    [[nodiscard]] const Model& model() const { return m_src_model; }

    [[nodiscard]] Model& model() { return m_src_model; }

    VarT& get(const Var& t_var) { return m_variables[t_var.index()]; }

    const VarT& get(const Var& t_var) const { return m_variables[t_var.index()]; }

    CtrT& get(const Ctr& t_ctr) { return m_constraints[t_ctr.index()];}

    const CtrT& get(const Ctr& t_ctr) const { return m_constraints[t_ctr.index()]; }
};

template<class VarT, class CtrT>
Solver<VarT, CtrT>::Solver(Model &t_model) : m_src_model(t_model) {

}

template<class VarT, class CtrT>
void Solver<VarT, CtrT>::add_variable_impl(const VarT &t_var) {
    m_variables.template emplace_back(t_var);
}

template<class VarT, class CtrT>
void Solver<VarT, CtrT>::add_constraint_impl(const CtrT &t_ctr) {
    m_constraints.template emplace_back(t_ctr);
}

template<class VarT, class CtrT>
void Solver<VarT, CtrT>::remove_variable_impl(const Var &t_var) {
    m_variables[t_var.index()] = std::move(m_variables.back());
    m_variables.pop_back();
}

template<class VarT, class CtrT>
void Solver<VarT, CtrT>::remove_constraint_impl(const Ctr &t_ctr) {
    m_constraints[t_ctr.index()] = std::move(m_constraints.back());
    m_constraints.pop_back();
}

template<class VarT, class CtrT>
double Solver<VarT, CtrT>::value(const Constant &t_constant) const {
    // TODO uncomment this
    /*if (!t_constant.is_numerical()) {
        throw Exception("Uncountered a constant with parameters in external solver.");
    }*/
    return t_constant.numerical();
}

#endif //OPTIMIZE_SOLVER2_H
