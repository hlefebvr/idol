//
// Created by henri on 07/09/22.
//
#include "../../../include/modeling/models/Model.h"
#include "../../../include/modeling/matrix/Matrix.h"
#include "../../../include/modeling/expressions/operators.h"

unsigned int Model::s_id = 0;

Model::Model(Sense t_sense) : m_objects(Env::get()), m_objective_sense(t_sense) {

}

Model::~Model() {
    while (!m_variables.empty()) { remove(m_variables.back()); }
    while (!m_constraints.empty()) { remove(m_constraints.back()); }
}

Var Model::add_var(double t_lb, double t_ub, VarType t_type, Column t_column, std::string t_name) {
    auto variable = m_objects.create<Var>(m_id, std::move(t_name), t_lb, t_ub, t_type, std::move(t_column));
    add_created_variable(variable);
    return variable;
}

Var Model::add_var(TempVar t_temporary_variable, std::string t_name) {
    auto variable = m_objects.create<Var>(m_id, std::move(t_name), std::move(t_temporary_variable));
    add_created_variable(variable);
    return variable;
}

Var Model::add_var(double t_lb, double t_ub, VarType t_type, Constant t_objective_coefficient, std::string t_name) {
    return add_var(t_lb, t_ub, t_type, Column(std::move(t_objective_coefficient)), std::move(t_name));
}

void Model::add_created_variable(const Var &t_var) {
    add_object(m_variables, t_var);
    m_listeners.broadcast_add(t_var);
    add_column_to_rows(t_var);
}

void Model::add_column_to_rows(const Var &t_var) {
    auto& impl = m_objects.impl(t_var);

    Matrix::apply_on_column(t_var, [&](const Ctr &t_ctr, MatrixCoefficientReference&& t_coefficient) {
        m_objects.impl(t_ctr).row().lhs().set(t_var, std::move(t_coefficient));
    });

    Matrix::add_to_obj(m_objective, t_var, impl.column());
}

void Model::remove(const Var &t_var) {
    m_listeners.broadcast_remove(t_var);
    for (const auto& [ctr, val] : t_var.column().components()) {
        m_objects.impl(ctr).row().lhs().set(t_var, 0.);
    }
    m_objective.linear().set(t_var, 0.);
    m_objects.impl(t_var).column() = Column();
    remove_object(m_variables, t_var);
}

Ctr Model::add_ctr(TempCtr t_temporary_constraint, std::string t_name) {
    auto constraint = m_objects.create<Ctr>(m_id, std::move(t_name), std::move(t_temporary_constraint));
    add_created_constraint(constraint);
    return constraint;
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

void Model::add_created_constraint(const Ctr &t_ctr) {
    add_object(m_constraints, t_ctr);
    m_listeners.broadcast_add(t_ctr);
    add_row_to_columns(t_ctr);
}

void Model::add_row_to_columns(const Ctr &t_ctr) {
    auto& impl = m_objects.impl(t_ctr);

    Matrix::apply_on_row(t_ctr, [&](const Var& t_var, MatrixCoefficientReference&& t_coefficient){
        m_objects.impl(t_var).column().components().set(t_ctr, std::move(t_coefficient));
    });
}

void Model::remove(const Ctr &t_ctr) {
    m_listeners.broadcast_remove(t_ctr);
    for (const auto& [var, val] : t_ctr.row().lhs()) {
        m_objects.impl(var).column().components().set(t_ctr, 0.);
    }
    m_objects.impl(t_ctr).row() = Row();
    remove_object(m_constraints, t_ctr);
}

void Model::update_matrix_coeff(const Ctr &t_ctr, const Var &t_var, Constant t_coefficient) {

    m_listeners.broadcast_update_coefficient(t_ctr, t_var, t_coefficient);

    Matrix::update_coefficient(
            t_var,
            t_ctr,
            m_objects.impl(t_var).column().components(),
            m_objects.impl(t_ctr).row().lhs(),
            std::move(t_coefficient)
        );

}

void Model::update_obj_sense(Sense t_sense) {
    m_objective_sense = t_sense;
}

void Model::update_obj_coeff(const Var &t_var, Constant t_coefficient) {
    m_listeners.broadcast_update_objective(t_var, t_coefficient);
    if (t_coefficient.is_zero()) {
        m_objective.linear().set(t_var, 0.);
    }
    m_objects.impl(t_var).column().set_objective_coefficient(std::move(t_coefficient));
}

void Model::update_obj(Expr<Var> &&t_obj) {
    for (const auto& [var, coeff] : m_objective.linear()) {
        m_objects.impl(var).column().set_objective_coefficient(0.);
    }
    m_objective = std::move(t_obj);
    Matrix::apply_obj(m_objective, [&](const Var& t_var, MatrixCoefficientReference&& t_coeff){
        m_objects.impl(t_var).column().set_objective_coefficient(std::move(t_coeff));
    });
}

void Model::update_obj(const Expr<Var> &t_obj) {
    update_obj(Expr<Var>(t_obj));
}

void Model::update_obj_const(Constant t_offset) {
    m_listeners.broadcast_update_objective_offset(t_offset);
    m_objective.constant() = std::move(t_offset);
}


void Model::update_rhs_coeff(const Ctr &t_ctr, Constant t_coefficient) {
    m_listeners.broadcast_update_rhs(t_ctr, t_coefficient);
    m_objects.impl(t_ctr).row().set_rhs(std::move(t_coefficient));
}

void Model::update_var_lb(const Var &t_var, double t_lb) {
    m_listeners.broadcast_update_lb(t_var, t_lb);
    m_objects.impl(t_var).set_lb(t_lb);
}

void Model::update_var_ub(const Var &t_var, double t_ub) {
    m_listeners.broadcast_update_ub(t_var, t_ub);
    m_objects.impl(t_var).set_ub(t_ub);
}

void Model::update_var_type(const Var &t_var, VarType t_type) {
    m_listeners.broadcast_update_type(t_var, t_type);
    m_objects.impl(t_var).set_type(t_type);
}

void Model::update_ctr_type(const Ctr &t_ctr, CtrType t_type) {
    m_listeners.broadcast_update_type(t_ctr, t_type);
    m_objects.impl(t_ctr).set_type(t_type);
}

void Model::add_listener(Listener &t_listener) const {
    m_listeners.add(t_listener);
}
