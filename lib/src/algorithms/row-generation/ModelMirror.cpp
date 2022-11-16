//
// Created by henri on 16/11/22.
//
#include "../../../include/algorithms/row-generation/ModelMirror.h"
#include "../../../include/modeling/expressions/operators.h"

ModelMirror::ModelMirror(const Model &t_model) : Model() {

    for (const auto& var : t_model.vars()) {
        add_var_clone(var);
    }

    for (const auto& ctr : t_model.ctrs()) {
        add_ctr_clone(ctr);
    }

    t_model.add_listener(*this);
}

Var ModelMirror::add_var_clone(const Var &t_var) {
    auto result = add_var(t_var.lb(), t_var.ub(), t_var.type(), t_var.obj(), t_var.name());
    m_variables_map.emplace(t_var, result);
    return result;
}

Ctr ModelMirror::add_ctr_clone(const Ctr &t_ctr) {
    Ctr result = add_ctr(TempCtr(clone(t_ctr.row()), t_ctr.type()));
    m_constraints_map.emplace(t_ctr, result);
    return result;
}

Row ModelMirror::clone(const Row &t_row) const {
    return { clone(t_row.lhs()), clone(t_row.rhs()) };
}

LinExpr<Var> ModelMirror::clone(const LinExpr<Var> &t_expr) const {
    LinExpr<Var> result;
    for (const auto& [var, constant] : t_expr) {
        result += clone(constant) * get_cloned(var);
    }
    return result;
}

Constant ModelMirror::clone(const Constant &t_constant) const {
    Constant result(t_constant.numerical());
    for (const auto& [param, coeff] : t_constant) {
        result += coeff * get_cloned(param);
    }
    return result;
}

Var ModelMirror::get_cloned(const Var &t_var) const {
    auto it = m_variables_map.find(t_var);
    if (it == m_variables_map.end()) {
        throw Exception("Requested cloned version of variable " + t_var.name() + ", yet it has not been cloned.");
    }
    return it->second;
}

Ctr ModelMirror::get_cloned(const Ctr &t_ctr) const {
    auto it = m_constraints_map.find(t_ctr);
    if (it == m_constraints_map.end()) {
        throw Exception("Requested cloned version of constraint " + t_ctr.name() + ", yet it has not been cloned.");
    }
    return it->second;
}

Param ModelMirror::get_cloned(const Param &t_param) const {
    if (t_param.is<Var>()) {
        return !get_cloned(t_param.as<Var>());
    }
    if (t_param.is<Ctr>()) {
        return !get_cloned(t_param.as<Ctr>());
    }
    throw Exception("Unexpected Param type.");
}

void ModelMirror::on_add(const Ctr &t_ctr) {
    add_ctr_clone(t_ctr);
}

void ModelMirror::on_remove(const Ctr &t_ctr) {
    remove(get_cloned(t_ctr));
    m_constraints_map.erase(t_ctr);
}

Solution::Primal ModelMirror::clone(const Solution::Primal &t_primal_solution) const {
    Solution::Primal result;

    result.set_status(t_primal_solution.status());
    result.set_reason(t_primal_solution.reason());

    for (const auto& [var, val] : t_primal_solution) {
        result.set(get_cloned(var), val);
    }

    return result;
}
