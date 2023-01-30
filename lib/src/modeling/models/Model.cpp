//
// Created by henri on 27/01/23.
//
#include "modeling/models/Model.h"
#include "modeling/models/Env.h"
#include "modeling/constraints/TempCtr.h"

Model::Model(Env &t_env) : m_env(t_env), m_id(t_env.create_model_id()) {}

Model::~Model() {

    for (const auto& var : m_variables) {
        m_env.remove_version(*this, var);
    }
    m_variables.clear();

    for (const auto& ctr : m_constraints) {
        m_env.remove_version(*this, ctr);
    }
    m_constraints.clear();

    m_env.free_model_id(*this);
}

Var Model::create_var(double t_lb, double t_ub, int t_type, Column &&t_column, std::string t_name) {
    auto result = m_env.create<Var>(std::move(t_name));
    add_var(result, t_lb, t_ub, t_type, std::move(t_column));
    return result;
}

Var Model::create_var(double t_lb, double t_ub, int t_type, const Column &t_column, std::string t_name) {
    return create_var(t_lb, t_ub, t_type, Column(t_column), std::move(t_name));
}

Var Model::create_var(double t_lb, double t_ub, int t_type, std::string t_name) {
    return create_var(t_lb, t_ub, t_type, Column(0.), std::move(t_name));
}

void Model::add_var(const Var &t_var, double t_lb, double t_ub, int t_type, Column&& t_column) {
    m_env.create_version(*this, t_var, m_variables.size(), TempVar(t_lb, t_ub, t_type, std::move(t_column)));
    m_variables.emplace_back(t_var);
    add_column_to_rows(t_var);
}

void Model::add_var(const Var &t_var, double t_lb, double t_ub, int t_type, const Column &t_column) {
    add_var(t_var, t_lb, t_ub, t_type, Column(t_column));
}

void Model::add_var(const Var &t_var, double t_lb, double t_ub, int t_type) {
    add_var(t_var, t_lb, t_ub, t_type, Column(0.));
}

void Model::remove(const Var &t_var) {
    const auto index = m_env.version(*this, t_var).index();
    m_env.version(*this, m_variables.back()).set_index(index);
    m_variables[index] = m_variables.back();
    m_variables.pop_back();
    m_env.remove_version(*this, t_var);
}

void Model::remove(const Ctr &t_ctr) {
    const auto index = m_env.version(*this, t_ctr).index();
    m_env.version(*this, m_constraints.back()).set_index(index);
    m_constraints[index] = m_constraints.back();
    m_constraints.pop_back();
    m_env.remove_version(*this, t_ctr);
}

Ctr Model::create_ctr(TempCtr &&t_temp_ctr, std::string t_name) {
    auto result = m_env.create<Ctr>(std::move(t_name));
    add_ctr(result, std::move(t_temp_ctr));
    return result;
}

Ctr Model::create_ctr(const TempCtr &t_temp_ctr, std::string t_name) {
    return create_ctr(TempCtr(t_temp_ctr), std::move(t_name));
}

Ctr Model::create_ctr(int t_type, std::string t_name) {
    return create_ctr(TempCtr(Row(0, 0), t_type), std::move(t_name));
}

void Model::add_ctr(const Ctr &t_ctr, TempCtr &&t_temp_ctr) {
    m_env.create_version(*this, t_ctr, m_constraints.size(), std::move(t_temp_ctr));
    m_constraints.emplace_back(t_ctr);
    add_row_to_columns(t_ctr);
}

void Model::add_ctr(const Ctr &t_ctr, const TempCtr &t_temp_ctr) {
    return add_ctr(t_ctr, TempCtr(t_temp_ctr));
}

void Model::add_ctr(const Ctr &t_ctr, int t_type) {
    return add_ctr(t_ctr, TempCtr(Row(0., 0.), t_type));
}

Expr<Var> &Model::access_obj() {
    return m_objective;
}

LinExpr<Ctr> &Model::access_rhs() {
    return m_rhs;
}

Column &Model::access_column(const Var &t_var) {
    return m_env.version(*this, t_var).column();
}

Row &Model::access_row(const Ctr &t_ctr) {
    return m_env.version(*this, t_ctr).row();
}

int Model::get(const AttributeWithTypeAndArguments<int, void> &t_attr) const {

    if (t_attr == Attr::Obj::Sense) {
        return m_sense;
    }

    return Base::get(t_attr);
}

const Expr<Var, Var> &Model::get(const AttributeWithTypeAndArguments<Expr<Var, Var>, void> &t_attr) const {

    if (t_attr == Attr::Obj::Expr) {
        return m_objective;
    }

    return Base::get(t_attr);
}

const Row &Model::get(const AttributeWithTypeAndArguments<Row, Ctr> &t_attr, const Ctr &t_ctr) const {

    if (t_attr == Attr::Ctr::Row) {
        return m_env.version(*this, t_ctr).row();
    }

    return Base::get(t_attr, t_ctr);
}

int Model::get(const AttributeWithTypeAndArguments<int, Ctr> &t_attr, const Ctr &t_ctr) const {

    if (t_attr == Attr::Ctr::Type) {
        return m_env.version(*this, t_ctr).type();
    }

    return Base::get(t_attr, t_ctr);
}

double Model::get(const AttributeWithTypeAndArguments<double, Var> &t_attr, const Var &t_var) const {

    if (t_attr == Attr::Var::Lb) {
        return m_env.version(*this, t_var).lb();
    }

    if (t_attr == Attr::Var::Ub) {
        return m_env.version(*this, t_var).ub();
    }

    return Base::get(t_attr, t_var);
}

const Column &Model::get(const AttributeWithTypeAndArguments<Column, Var> &t_attr, const Var &t_var) const {

    if (t_attr == Attr::Var::Column) {
        return m_env.version(*this, t_var).column();
    }

    return Base::get(t_attr, t_var);
}

const Constant &
Model::get(const AttributeWithTypeAndArguments<Constant, Ctr, Var> &t_attr, const Ctr &t_ctr, const Var &t_var) const {

    if (t_attr == Attr::Matrix::Coeff) {
        return m_env.version(*this, t_ctr).row().linear().get(t_var);
    }

    return Base::get(t_attr, t_ctr, t_var);
}

const LinExpr<Ctr> &Model::get(const AttributeWithTypeAndArguments<LinExpr<Ctr>, void> &t_attr) const {

    if (t_attr == Attr::Rhs::Expr) {
        return m_rhs;
    }

    return Base::get(t_attr);
}

const Constant &Model::get(const AttributeWithTypeAndArguments<Constant, Var> &t_attr, const Var &t_var) const {

    if (t_attr == Attr::Var::Obj) {
        return m_env.version(*this, t_var).column().obj();
    }

    return Base::get(t_attr, t_var);
}

const Constant &Model::get(const AttributeWithTypeAndArguments<Constant, void> &t_attr) const {

    if (t_attr == Attr::Obj::Const) {
        return m_objective.constant();
    }

    return Base::get(t_attr);
}

int Model::get(const AttributeWithTypeAndArguments<int, Var> &t_attr, const Var &t_var) const {

    if (t_attr == Attr::Var::Type) {
        return m_env.version(*this, t_var).type();
    }

    return Base::get(t_attr, t_var);
}

void Model::set(const AttributeWithTypeAndArguments<double, Var> &t_attr, const Var &t_var, double t_value) {


    if (t_attr == Attr::Var::Lb) {
        m_env.version(*this, t_var).set_lb(t_value);
        return;
    }

    if (t_attr == Attr::Var::Ub) {
        m_env.version(*this, t_var).set_ub(t_value);
        return;
    }

    Base::set(t_attr, t_var, t_value);
}

void Model::set(const AttributeWithTypeAndArguments<Constant, Ctr, Var> &t_attr, const Ctr &t_ctr, const Var &t_var, Constant && t_value) {

    if (t_attr == Attr::Matrix::Coeff) {
        update_matrix_coefficient(t_ctr, t_var, std::move(t_value));
        return;
    }

    AttributeManagers::Base::set(t_attr, t_ctr, t_var, std::move(t_value));
}

void Model::set(const AttributeWithTypeAndArguments<Constant, Ctr> &t_attr, const Ctr &t_ctr, Constant &&t_value) {

    if (t_attr == Attr::Ctr::Rhs) {
        add_to_rhs(t_ctr, std::move(t_value));
        return;
    }

    Base::set(t_attr, t_ctr, std::move(t_value));
}

void Model::set(const AttributeWithTypeAndArguments<Expr<Var, Var>, void> &t_attr, Expr<Var, Var> &&t_value) {

    if (t_attr == Attr::Obj::Expr) {
        replace_objective(std::move(t_value));
        return;
    }

    Base::set(t_attr, std::move(t_value));
}

void Model::set(const AttributeWithTypeAndArguments<LinExpr<Ctr>, void> &t_attr, LinExpr<Ctr> &&t_value) {

    if (t_attr == Attr::Rhs::Expr) {
        replace_right_handside(std::move(t_value));
        return;
    }

    Base::set(t_attr, t_value);
}

void Model::set(const AttributeWithTypeAndArguments<Constant, void> &t_attr, Constant &&t_value) {

    if (t_attr == Attr::Obj::Const) {
        m_objective.constant() = std::move(t_value);
        return;
    }

    Base::set(t_attr, t_value);
}

void Model::set(const AttributeWithTypeAndArguments<Constant, Var> &t_attr, const Var &t_var, Constant &&t_value) {

    if (t_attr == Attr::Var::Obj) {
        add_to_obj(t_var, std::move(t_value));
        return;
    }

    Base::set(t_attr, t_var, t_value);
}

void Model::set(const AttributeWithTypeAndArguments<int, Var> &t_attr, const Var &t_var, int t_value) {

    if (t_attr == Attr::Var::Type) {
        m_env.version(*this, t_var).set_type(t_value);
        return;
    }

    return Base::set(t_attr, t_var, t_value);
}

void Model::set(const AttributeWithTypeAndArguments<int, void> &t_attr, int t_value) {

    if (t_attr == Attr::Obj::Sense) {
        if (t_value < 0 || t_value > 1) {
            throw Exception("Objective sense out of bounds.");
        }
        m_sense = t_value;
        return;
    }

    Base::set(t_attr, t_value);
}

void Model::set(const AttributeWithTypeAndArguments<int, Ctr> &t_attr, const Ctr &t_ctr, int t_value) {

    if (t_attr == Attr::Ctr::Type) {
        m_env.version(*this, t_ctr).set_type(t_value);
        return;
    }

    Base::set(t_attr, t_ctr, t_value);
}

void Model::set(const AttributeWithTypeAndArguments<Row, Ctr> &t_attr, const Ctr &t_ctr, Row &&t_value) {

    if (t_attr == Attr::Ctr::Row) {
        remove_row_from_columns(t_ctr);
        m_env.version(*this, t_ctr).row() = std::move(t_value);
        add_row_to_columns(t_ctr);
        return;
    }

    Base::set(t_attr, t_ctr, t_value);
}

void Model::set(const AttributeWithTypeAndArguments<Column, Var> &t_attr, const Var &t_var, Column &&t_value) {

    if (t_attr == Attr::Var::Column) {
        remove_column_from_rows(t_var);
        m_env.version(*this, t_var).column() = std::move(t_value);
        add_column_to_rows(t_var);
        return;
    }

    Base::set(t_attr, t_var, t_value);
}
