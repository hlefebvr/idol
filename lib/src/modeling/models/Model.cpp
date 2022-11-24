//
// Created by henri on 07/09/22.
//
#include "../../../include/modeling/models/Model.h"
#include "../../../include/modeling/matrix/Matrix.h"
#include "../../../include/modeling/expressions/operations/operators.h"

Model::Model(Sense t_sense) : m_objective_sense(t_sense) {

}

Var Model::add_var(double t_lb, double t_ub, VarType t_type, Column t_column, std::string t_name) {
    return add_var(TempVar(t_lb, t_ub, t_type, std::move(t_column)), std::move(t_name));
}

Var Model::add_var(double t_lb, double t_ub, VarType t_type, Constant t_objective_coefficient, std::string t_name) {
    return add_var(TempVar(t_lb, t_ub, t_type, Column(std::move(t_objective_coefficient))), std::move(t_name));
}

Var Model::add_var(TempVar t_temporary_variable, std::string t_name) {

    auto ref = m_variables.add_attributes(std::move(t_name), "Var", std::move(t_temporary_variable));
    Var variable(std::move(ref));
    m_variables.add_object(variable);

    m_listeners.broadcast_add(variable);
    add_column_to_rows(variable);

    return variable;
}

void Model::remove(const Var &t_var) {
    m_listeners.broadcast_remove(t_var);
    auto& attributes = m_variables.attributes(t_var);
    remove_column_from_rows(t_var);
    m_variables.remove(t_var);
}

Ctr Model::add_ctr(CtrType t_type, Constant t_rhs, std::string t_name) {
    if (t_type == Equal) {
        return add_ctr(LinExpr() == std::move(t_rhs), std::move(t_name));
    }
    if (t_type == LessOrEqual) {
        return add_ctr(LinExpr() <= std::move(t_rhs), std::move(t_name));
    }
    return add_ctr(LinExpr() >= std::move(t_rhs), std::move(t_name));
}

Ctr Model::add_ctr(TempCtr t_temporary_constraint, std::string t_name) {

    auto ref = m_constraints.add_attributes(std::move(t_name), "Ctr", std::move(t_temporary_constraint));
    Ctr constraint(std::move(ref));
    m_constraints.add_object(constraint);

    m_listeners.broadcast_add(constraint);
    add_row_to_columns(constraint);

    return constraint;
}

void Model::remove(const Ctr &t_ctr) {
    m_listeners.broadcast_remove(t_ctr);
    auto& attributes = m_constraints.attributes(t_ctr);
    remove_row_from_columns(t_ctr);
    m_constraints.remove(t_ctr);
}

void Model::update_obj_sense(Sense t_sense) {
    m_objective_sense = t_sense;
}

void Model::update_obj(Expr<Var> &&t_obj) {
    replace_objective(std::move(t_obj));
}

void Model::update_obj(const Expr<Var> &t_obj) {
    update_obj(Expr<Var>(t_obj));
}

void Model::update_rhs(LinExpr<Ctr> &&t_obj) {
    replace_right_handside(std::move(t_obj));
}

void Model::update_rhs(const LinExpr<Ctr> &t_obj) {
    update_rhs(LinExpr<Ctr>(t_obj));
}

void Model::update_obj_const(Constant t_offset) {
    m_listeners.broadcast_update_objective_offset(t_offset);
    m_objective.constant() = std::move(t_offset);
}

void Model::update_obj_coeff(const Var &t_var, Constant t_coefficient) {
    m_listeners.broadcast_update_objective(t_var, t_coefficient);
    add_to_obj(t_var, std::move(t_coefficient));
}

void Model::update_rhs_coeff(const Ctr &t_ctr, Constant t_coefficient) {
    m_listeners.broadcast_update_rhs(t_ctr, t_coefficient);
    add_to_rhs(t_ctr, std::move(t_coefficient));
}

void Model::update_var_lb(const Var &t_var, double t_lb) {
    m_listeners.broadcast_update_lb(t_var, t_lb);
    m_variables.attributes(t_var).set_lb(t_lb);
}

void Model::update_var_ub(const Var &t_var, double t_ub) {
    m_listeners.broadcast_update_ub(t_var, t_ub);
    m_variables.attributes(t_var).set_ub(t_ub);
}

void Model::update_var_type(const Var &t_var, VarType t_type) {
    m_listeners.broadcast_update_type(t_var, t_type);
    m_variables.attributes(t_var).set_type(t_type);
}

void Model::update_ctr_type(const Ctr &t_ctr, CtrType t_type) {
    m_listeners.broadcast_update_type(t_ctr, t_type);
    m_constraints.attributes(t_ctr).set_type(t_type);
}

void Model::add_listener(Listener &t_listener) const {
    m_listeners.add(t_listener);
}

double Model::get_lb(const Var &t_var) const {
    return m_variables.attributes(t_var).lb();
}

double Model::get_ub(const Var &t_var) const {
    return m_variables.attributes(t_var).ub();
}

VarType Model::get_type(const Var &t_var) const {
    return m_variables.attributes(t_var).type();
}

const Column &Model::get_column(const Var &t_var) const {
    return m_variables.attributes(t_var).column();
}

bool Model::has(const Var &t_var) const {
    return m_variables.has(t_var);
}

const Row &Model::get_row(const Ctr &t_ctr) const {
    return m_constraints.attributes(t_ctr).row();
}

CtrType Model::get_type(const Ctr &t_ctr) const {
    return m_constraints.attributes(t_ctr).type();
}

bool Model::has(const Ctr &t_ctr) const {
    return m_constraints.has(t_ctr);
}

Column &Model::access_column(const Var &t_var) {
    return m_variables.attributes(t_var).column();
}

Row &Model::access_row(const Ctr &t_ctr) {
    return m_constraints.attributes(t_ctr).row();
}

Expr<Var> &Model::access_obj() {
    return m_objective;
}

LinExpr<Ctr> &Model::access_rhs() {
    return m_rhs;
}

void Model::update_matrix_coeff(const Ctr &t_ctr, const Var &t_var, Constant t_coefficient) {
    update_matrix_coefficient(t_ctr, t_var, std::move(t_coefficient));
}
