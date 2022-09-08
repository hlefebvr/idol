//
// Created by henri on 07/09/22.
//
#include "modeling/models/Model.h"
#include "modeling/columns_and_rows/ColumnOrRowReference.h"

Model::Model(Env &t_env) : m_objects(t_env) {

}

Model::~Model() {
    free(m_parameters);
    free(m_variables);
    free(m_constraints);
}

Param Model::add_parameter(double t_lb, double t_ub, VarType t_type, std::string t_name) {
    auto result = m_objects.create<Param>(std::move(t_name), t_lb, t_ub, t_type);
    add_object(m_parameters, result);
    return result;
}

void Model::remove(const Param &t_param) {
    remove_object(m_parameters, t_param);
}

Var Model::add_variable(double t_lb, double t_ub, VarType t_type, Column t_column, std::string t_name) {
    auto variable = m_objects.create<Var>(std::move(t_name), t_lb, t_ub, t_type, std::move(t_column));
    add_object(m_variables, variable);
    add_column_to_rows(variable);
    return variable;
}

Var Model::add_variable(double t_lb, double t_ub, VarType t_type, Coefficient t_objective_coefficient, std::string t_name) {
    return add_variable(t_lb, t_ub, t_type, Column(std::move(t_objective_coefficient)), std::move(t_name));
}


void Model::add_column_to_rows(const Var &t_var) {
    auto& impl = m_objects.impl(t_var);
    for (auto [ctr, ref_to_coef] : ColumnOrRowReference(impl.column())) {
        m_objects.impl(ctr).row().set(t_var, std::move(ref_to_coef));
    }
}

void Model::remove(const Var &t_var) {
    remove_object(m_variables, t_var);
}

Ctr Model::add_constraint(TempCtr t_temporary_constraint, std::string t_name) {
    auto constraint = m_objects.create<Ctr>(std::move(t_name), std::move(t_temporary_constraint));
    add_object(m_constraints, constraint);
    add_row_to_columns(constraint);
    return constraint;
}

Ctr Model::add_constraint(CtrType t_type, Coefficient t_rhs, std::string t_name) {
    if (t_type == Equal) {
        return add_constraint(Expr() == std::move(t_rhs), std::move(t_name));
    }
    if (t_type == LessOrEqual) {
        return add_constraint(Expr() <= std::move(t_rhs), std::move(t_name));
    }
    return add_constraint(Expr() >= std::move(t_rhs), std::move(t_name));
}

void Model::add_row_to_columns(const Ctr &t_ctr) {
    auto& impl = m_objects.impl(t_ctr);
    for (auto [ctr, ref_to_coef] : ColumnOrRowReference(impl.row())) {
        m_objects.impl(ctr).column().set(t_ctr, std::move(ref_to_coef));
    }
}

void Model::remove(const Ctr &t_ctr) {
    remove_object(m_constraints, t_ctr);
}

void Model::update_coefficient(const Ctr &t_ctr, const Var &t_var, Coefficient t_coefficient) {
    if (t_coefficient.is_zero()) {
        m_objects.impl(t_var).column().set(t_ctr, 0.);
        m_objects.impl(t_ctr).row().set(t_var, 0.);
        return;
    }
    m_objects.impl(t_ctr).row().set(t_var, std::move(t_coefficient));
}

void Model::update_objective(const Var &t_var, Coefficient t_coefficient) {
    m_objects.impl(t_var).column().set_constant(std::move(t_coefficient));
}

void Model::update_rhs(const Ctr &t_ctr, Coefficient t_coefficient) {
    m_objects.impl(t_ctr).row().set_constant(std::move(t_coefficient));
}

void Model::update_lb(const Var &t_var, double t_lb) {
    m_objects.impl(t_var).set_lb(t_lb);
}

void Model::update_ub(const Var &t_var, double t_ub) {
    m_objects.impl(t_var).set_ub(t_ub);
}

void Model::update_type(const Var &t_var, VarType t_type) {
    m_objects.impl(t_var).set_type(t_type);
}

void Model::update_type(const Ctr &t_ctr, CtrType t_type) {
    m_objects.impl(t_ctr).set_type(t_type);
}
