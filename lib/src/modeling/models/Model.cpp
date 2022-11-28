//
// Created by henri on 07/09/22.
//
#include "../../../include/modeling/models/Model.h"
#include "../../../include/modeling/matrix/Matrix.h"
#include "../../../include/modeling/expressions/operations/operators.h"
#include "../../../include/modeling/variables/Attributes_Var.h"
#include "../../../include/modeling/constraints/Attributes_Ctr.h"
#include "../../../include/modeling/models/Attributes_Model.h"

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

    add_column_to_rows(variable);

    return variable;
}

void Model::remove(const Var &t_var) {
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

    add_row_to_columns(constraint);

    return constraint;
}

void Model::remove(const Ctr &t_ctr) {
    auto& attributes = m_constraints.attributes(t_ctr);
    remove_row_from_columns(t_ctr);
    m_constraints.remove(t_ctr);
}

bool Model::has(const Var &t_var) const {
    return m_variables.has(t_var);
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

void Model::set_attr_var_double(const Attribute<double> &t_attr, const Var &t_var, double t_value) {

    if (t_attr == Attr::Var::Lb) {
        m_variables.attributes(t_var).set_lb(t_value);
        return;
    }

    if (t_attr == Attr::Var::Ub) {
        m_variables.attributes(t_var).set_ub(t_value);
        return;
    }

    AttributeManager::set_attr_var_double(t_attr, t_var, t_value);
}

double Model::get_attr_var_double(const Attribute<double> &t_attr, const Var &t_var) const {

    if (t_attr == Attr::Var::Lb) {
        return m_variables.attributes(t_var).lb();
    }

    if (t_attr == Attr::Var::Ub) {
        return m_variables.attributes(t_var).ub();
    }

    return AttributeManager::get_attr_var_double(t_attr, t_var);
}

const Column &Model::get_attr_var_Column(const Attribute<Column> &t_attr, const Var &t_var) const {

    if (t_attr == Attr::Var::Column) {
        return m_variables.attributes(t_var).column();
    }

    return AttributeManager::get_attr_var_Column(t_attr, t_var);
}

const Row &Model::get_attr_ctr_Row(const Attribute<Row> &t_attr, const Ctr &t_ctr) const {

    if (t_attr == Attr::Ctr::Row) {
        return m_constraints.attributes(t_ctr).row();
    }

    return AttributeManager::get_attr_ctr_Row(t_attr, t_ctr);
}

const Constant & Model::get_attr_ctr_var_Constant(const Attribute<Constant> &t_attr, const Ctr &t_ctr, const Var &t_var) const {

    if (t_attr == Attr::Matrix::Coeff) {
        return m_constraints.attributes(t_ctr).row().linear().get(t_var);
    }

    return AttributeManager::get_attr_ctr_var_Constant(t_attr, t_ctr, t_var);
}

void Model::set_attr_ctr_var_Constant(const Attribute<Constant> &t_attr, const Ctr &t_ctr, const Var &t_var,
                                      Constant &&t_value) {

    if (t_attr == Attr::Matrix::Coeff) {
        update_matrix_coefficient(t_ctr, t_var, std::move(t_value));
        return;
    }

    AttributeManager::set_attr_ctr_var_Constant(t_attr, t_ctr, t_var, std::move(t_value));
}

void Model::set_attr_ctr_Constant(const Attribute<Constant> &t_attr, const Ctr &t_ctr, Constant &&t_value) {

    if (t_attr == Attr::Ctr::Rhs) {
        add_to_rhs(t_ctr, std::move(t_value));
        return;
    }

    AttributeManager::set_attr_ctr_Constant(t_attr, t_ctr, std::move(t_value));
}

const Expr<Var, Var> &Model::get_attr_Expr_Var_Var(const Attribute<Expr<Var, Var>> &t_attr) const {

    if (t_attr == Attr::Obj::Expr) {
        return m_objective;
    }

    return AttributeManager::get_attr_Expr_Var_Var(t_attr);
}

void Model::set_attr_Expr_Var_Var(const Attribute<Expr<Var, Var>> &t_attr, Expr<Var, Var> &&t_value) {

    if (t_attr == Attr::Obj::Expr) {
        replace_objective(std::move(t_value));
        return;
    }

    AttributeManager::set_attr_Expr_Var_Var(t_attr, std::move(t_value));
}

void Model::set_attr_LinExpr_Ctr(const Attribute<LinExpr<Ctr>> &t_attr, LinExpr<Ctr> &&t_value) {

    if (t_attr == Attr::Rhs::Expr) {
        replace_right_handside(std::move(t_value));
        return;
    }

    AttributeManager::set_attr_LinExpr_Ctr(t_attr, std::move(t_value));
}

const LinExpr<Ctr> &Model::get_attr_LinExpr_Ctr(const Attribute<LinExpr<Ctr>> &t_attr) const {

    if (t_attr == Attr::Rhs::Expr) {
        return m_rhs;
    }

    return AttributeManager::get_attr_LinExpr_Ctr(t_attr);
}

void Model::set_attr_Constant(const Attribute<Constant> &t_attr, Constant &&t_value) {

    if (t_attr == Attr::Obj::Const) {
        m_objective.constant() = std::move(t_value);
        return;
    }

    AttributeManager::set_attr_Constant(t_attr, std::move(t_value));
}

void Model::set_attr_var_Constant(const Attribute<Constant> &t_attr, const Var &t_var, Constant &&t_value) {

    if (t_attr == Attr::Var::Obj) {
        add_to_obj(t_var, std::move(t_value));
        return;
    }

    AttributeManager::set_attr_var_Constant(t_attr, t_var, std::move(t_value));
}

void Model::set_attr_var_int(const Attribute<int> &t_attr, const Var &t_var, int t_value) {

    if (t_attr == Attr::Var::Type) {
        m_variables.attributes(t_var).set_type(t_value);
        return;
    }

    AttributeManager::set_attr_var_int(t_attr, t_var, t_value);
}

void Model::set_attr_int(const Attribute<int> &t_attr, int t_value) {

    if (t_attr == Attr::Obj::Sense) {
        if (t_value < 0 || t_value > 1) {
            throw Exception("Objective sense out of bounds.");
        }
        m_objective_sense = t_value;
        return;
    }

    AttributeManager::set_attr_int(t_attr, t_value);
}

void Model::set_attr_ctr_int(const Attribute<int> &t_attr, const Ctr &t_ctr, int t_value) {

    if (t_attr == Attr::Ctr::Type) {
        m_constraints.attributes(t_ctr).set_type(t_value);
        return;
    }

    AttributeManager::set_attr_ctr_int(t_attr, t_ctr, t_value);
}

int Model::get_attr_ctr_int(const Attribute<int> &t_attr, const Ctr &t_ctr) const {

    if (t_attr == Attr::Ctr::Type) {
        return m_constraints.attributes(t_ctr).type();
    }

    return AttributeManager::get_attr_ctr_int(t_attr, t_ctr);
}

const Constant &Model::get_attr_ctr_Constant(const Attribute<Constant> &t_attr, const Ctr &t_ctr) const {

    if (t_attr == Attr::Ctr::Rhs) {
        return m_constraints.attributes(t_ctr).row().rhs();
    }

    return AttributeManager::get_attr_ctr_Constant(t_attr, t_ctr);
}

const Constant &Model::get_attr_var_Constant(const Attribute<Constant> &t_attr, const Var &t_var) const {

    if (t_attr == Attr::Var::Obj) {
        return m_variables.attributes(t_var).column().obj();
    }

    return AttributeManager::get_attr_var_Constant(t_attr, t_var);
}

int Model::get_attr_int(const Attribute<int> &t_attr) const {

    if (t_attr == Attr::Obj::Sense) {
        return m_objective_sense;
    }

    return AttributeManager::get_attr_int(t_attr);
}

const Constant &Model::get_attr_Constant(const Attribute<Constant> &t_attr) const {

    if (t_attr == Attr::Obj::Sense) {
        return m_objective.constant();
    }

    return AttributeManager::get_attr_Constant(t_attr);
}

int Model::get_attr_var_int(const Attribute<int> &t_attr, const Var &t_var) const {

    if (t_attr == Attr::Var::Type) {
        return m_variables.attributes(t_var).type();
    }

    return AttributeManager::get_attr_var_int(t_attr, t_var);
}
