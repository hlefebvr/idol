//
// Created by henri on 27/01/23.
//
#include "modeling/models/Model.h"
#include "modeling/objects/Env.h"
#include "backends/parameters/Parameters_Algorithm.h"

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


void Model::add(const Var &t_var, TempVar &&t_temp_var) {
    m_env.create_version(*this,
                         t_var,
                         m_variables.size(),
                         t_temp_var.lb(),
                         t_temp_var.ub(),
                         t_temp_var.type(),
                         std::move(t_temp_var.column())
    );
    m_variables.emplace_back(t_var);

    if (has_backend()) {
        backend().add(t_var);
    }

    add_column_to_rows(t_var);
}

void Model::add(const Var &t_var) {
    const auto& default_version = m_env[t_var];
    add(t_var, TempVar(
             default_version.lb(),
             default_version.ub(),
             default_version.type(),
             Column(default_version.column())
    ));
}

void Model::remove(const Var &t_var) {

    if (has_backend()) {
        backend().remove(t_var);
    }

    remove_column_from_rows(t_var);
    const auto index = m_env.version(*this, t_var).index();
    m_env.version(*this, m_variables.back()).set_index(index);
    m_variables[index] = m_variables.back();
    m_variables.pop_back();
    m_env.remove_version(*this, t_var);
}

void Model::remove(const Ctr &t_ctr) {

    if (has_backend()) {
        backend().remove(t_ctr);
    }

    remove_row_from_columns(t_ctr);
    const auto index = m_env.version(*this, t_ctr).index();
    m_env.version(*this, m_constraints.back()).set_index(index);
    m_constraints[index] = m_constraints.back();
    m_constraints.pop_back();
    m_env.remove_version(*this, t_ctr);
}

void Model::add(const Ctr &t_ctr, TempCtr &&t_temp_ctr) {
    m_env.create_version(*this, t_ctr, m_constraints.size(), std::move(t_temp_ctr));
    m_constraints.emplace_back(t_ctr);

    if (has_backend()) {
        backend().add(t_ctr);
    }

    add_row_to_columns(t_ctr);
}

void Model::add(const Ctr &t_ctr) {
    const auto& default_version = m_env[t_ctr];
    add(t_ctr, TempCtr(
            Row(default_version.row()),
            default_version.type())
        );
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

int Model::get(const Req<int, void> &t_attr) const {

    if (t_attr == Attr::Obj::Sense) {
        return m_sense;
    }

    return AttributeManagers::Delegate::get(t_attr);
}

const Expr<Var, Var> &Model::get(const Req<Expr<Var, Var>, void> &t_attr) const {

    if (t_attr == Attr::Obj::Expr) {
        return m_objective;
    }

    return AttributeManagers::Delegate::get(t_attr);
}

const Row &Model::get(const Req<Row, Ctr> &t_attr, const Ctr &t_ctr) const {

    if (t_attr == Attr::Ctr::Row) {
        return m_env.version(*this, t_ctr).row();
    }

    return AttributeManagers::Delegate::get(t_attr, t_ctr);
}

int Model::get(const Req<int, Ctr> &t_attr, const Ctr &t_ctr) const {

    if (t_attr == Attr::Ctr::Type) {
        return m_env.version(*this, t_ctr).type();
    }

    if (t_attr == Attr::Ctr::Index) {
        return (int) m_env.version(*this, t_ctr).index();
    }

    return AttributeManagers::Delegate::get(t_attr, t_ctr);
}

double Model::get(const Req<double, Var> &t_attr, const Var &t_var) const {

    if (t_attr == Attr::Var::Lb) {
        return m_env.version(*this, t_var).lb();
    }

    if (t_attr == Attr::Var::Ub) {
        return m_env.version(*this, t_var).ub();
    }

    return AttributeManagers::Delegate::get(t_attr, t_var);
}

const Column &Model::get(const Req<Column, Var> &t_attr, const Var &t_var) const {

    if (t_attr == Attr::Var::Column) {
        return m_env.version(*this, t_var).column();
    }

    return AttributeManagers::Delegate::get(t_attr, t_var);
}

const Constant &
Model::get(const Req<Constant, Ctr, Var> &t_attr, const Ctr &t_ctr, const Var &t_var) const {

    if (t_attr == Attr::Matrix::Coeff) {
        return m_env.version(*this, t_ctr).row().linear().get(t_var);
    }

    return AttributeManagers::Delegate::get(t_attr, t_ctr, t_var);
}

const LinExpr<Ctr> &Model::get(const Req<LinExpr<Ctr>, void> &t_attr) const {

    if (t_attr == Attr::Rhs::Expr) {
        return m_rhs;
    }

    return AttributeManagers::Delegate::get(t_attr);
}

const Constant &Model::get(const Req<Constant, Var> &t_attr, const Var &t_var) const {

    if (t_attr == Attr::Var::Obj) {
        //return m_objective.linear().get(t_var);
        return m_env.version(*this, t_var).column().obj();
    }

    return AttributeManagers::Delegate::get(t_attr, t_var);
}

const Constant &Model::get(const Req<Constant, void> &t_attr) const {

    if (t_attr == Attr::Obj::Const) {
        return m_objective.constant();
    }

    return AttributeManagers::Delegate::get(t_attr);
}

int Model::get(const Req<int, Var> &t_attr, const Var &t_var) const {

    if (t_attr == Attr::Var::Type) {
        return m_env.version(*this, t_var).type();
    }

    if (t_attr == Attr::Var::Index) {
        return (int) m_env.version(*this, t_var).index();
    }

    return AttributeManagers::Delegate::get(t_attr, t_var);
}

void Model::set(const Req<double, Var> &t_attr, const Var &t_var, double t_value) {

    if (t_attr == Attr::Var::Lb) {
        m_env.version(*this, t_var).set_lb(t_value);
        if (has_backend()) { backend().set(t_attr, t_var, t_value); }
        return;
    }

    if (t_attr == Attr::Var::Ub) {
        m_env.version(*this, t_var).set_ub(t_value);
        if (has_backend()) { backend().set(t_attr, t_var, t_value); }
        return;
    }

    AttributeManagers::Delegate::set(t_attr, t_var, t_value);
}

void Model::set(const Req<Constant, Ctr, Var> &t_attr, const Ctr &t_ctr, const Var &t_var, Constant && t_value) {

    if (t_attr == Attr::Matrix::Coeff) {

        if (has_backend()) {
            backend().update();
            backend().set(t_attr, t_ctr, t_var, t_value);
        }

        update_matrix_coefficient(t_ctr, t_var, std::move(t_value));

        return;
    }

    AttributeManagers::Delegate::set(t_attr, t_ctr, t_var, std::move(t_value));
}

void Model::set(const Req<Constant, Ctr> &t_attr, const Ctr &t_ctr, Constant &&t_value) {

    if (t_attr == Attr::Ctr::Rhs) {
        if (has_backend()) {
            add_to_rhs(t_ctr, Constant(t_value));
            backend().set(t_attr, t_ctr, std::move(t_value));
        } else {
            add_to_rhs(t_ctr, std::move(t_value));
        }
        return;
    }

    AttributeManagers::Delegate::set(t_attr, t_ctr, std::move(t_value));
}

void Model::set(const Req<Expr<Var, Var>, void> &t_attr, Expr<Var, Var> &&t_value) {

    if (t_attr == Attr::Obj::Expr) {

        if (has_backend()) {
            replace_objective(Expr<Var, Var>(t_value));
            backend().set(t_attr, std::move(t_value));
        } else {
            replace_objective(std::move(t_value));
        }

        return;
    }

    AttributeManagers::Delegate::set(t_attr, std::move(t_value));
}

void Model::set(const Req<LinExpr<Ctr>, void> &t_attr, LinExpr<Ctr> &&t_value) {

    if (t_attr == Attr::Rhs::Expr) {

        if (has_backend()) {
            replace_right_handside(LinExpr<Ctr>(t_value));
            backend().set(t_attr, std::move(t_value));
        } else {
            replace_right_handside(std::move(t_value));
        }

        return;
    }

    AttributeManagers::Delegate::set(t_attr, t_value);
}

void Model::set(const Req<Constant, void> &t_attr, Constant &&t_value) {

    if (t_attr == Attr::Obj::Const) {

        if (has_backend()) {
            m_objective.constant() = Constant(t_value);
            backend().set(t_attr, std::move(t_value));
        } else {
            m_objective.constant() = std::move(t_value);
        }

        return;
    }

    AttributeManagers::Delegate::set(t_attr, t_value);
}

void Model::set(const Req<Constant, Var> &t_attr, const Var &t_var, Constant &&t_value) {

    if (t_attr == Attr::Var::Obj) {

        if (has_backend()) {
            add_to_obj(t_var, Constant(t_value));
            backend().set(t_attr, t_var, std::move(t_value));
        } else {
            add_to_obj(t_var, std::move(t_value));
        }

        return;
    }

    AttributeManagers::Delegate::set(t_attr, t_var, t_value);
}

void Model::set(const Req<int, Var> &t_attr, const Var &t_var, int t_value) {

    if (t_attr == Attr::Var::Type) {

        m_env.version(*this, t_var).set_type(t_value);
        if (has_backend()) { backend().set(t_attr, t_var, t_value); }
        return;
    }

    return AttributeManagers::Delegate::set(t_attr, t_var, t_value);
}

void Model::set(const Req<int, void> &t_attr, int t_value) {

    if (t_attr == Attr::Obj::Sense) {
        if (t_value != Minimize && t_value != Maximize) {
            throw Exception("Unsupported objective sense.");
        }
        m_sense = t_value;
        if (has_backend()) { backend().set(t_attr, t_value); }
        return;
    }

    AttributeManagers::Delegate::set(t_attr, t_value);
}

void Model::set(const Req<int, Ctr> &t_attr, const Ctr &t_ctr, int t_value) {

    if (t_attr == Attr::Ctr::Type) {
        m_env.version(*this, t_ctr).set_type(t_value);
        if (has_backend()) { backend().set(t_attr, t_ctr, t_value); }
        return;
    }

    AttributeManagers::Delegate::set(t_attr, t_ctr, t_value);
}

void Model::set(const Req<Row, Ctr> &t_attr, const Ctr &t_ctr, Row &&t_value) {

    if (t_attr == Attr::Ctr::Row) {

        remove_row_from_columns(t_ctr);
        m_env.version(*this, t_ctr).row() = std::move(t_value);
        add_row_to_columns(t_ctr);
        if (has_backend()) {
            throw Exception("Updating row is not implemented.");
        }

        return;
    }

    AttributeManagers::Delegate::set(t_attr, t_ctr, t_value);
}

void Model::set(const Req<Column, Var> &t_attr, const Var &t_var, Column &&t_value) {

    if (t_attr == Attr::Var::Column) {
        remove_column_from_rows(t_var);
        m_env.version(*this, t_var).column() = std::move(t_value);
        add_column_to_rows(t_var);
        if (has_backend()) {
            throw Exception("Updating column is not implemented.");
        }

        return;
    }

    AttributeManagers::Delegate::set(t_attr, t_var, t_value);
}

const Constant &Model::get(const Req<Constant, Ctr> &t_attr, const Ctr &t_ctr) const {

    if (t_attr == Attr::Ctr::Rhs) {
        return m_env.version(*this, t_ctr).row().rhs();
    }

    return AttributeManagers::Delegate::get(t_attr, t_ctr);
}

void Model::update() {
    backend().update();
}

void Model::write(const std::string &t_name) {
    backend().write(t_name);
}

AttributeManager &Model::attribute_delegate(const Attribute &t_attribute) {
    return backend();
}

AttributeManager &Model::attribute_delegate(const Attribute &t_attribute, const Var &t_object) {
    return backend();
}

AttributeManager &Model::attribute_delegate(const Attribute &t_attribute, const Ctr &t_object) {
    return backend();
}

bool Model::has(const Var &t_var) const {
    return m_env.has_version(*this, t_var);
}

bool Model::has(const Ctr &t_ctr) const {
    return m_env.has_version(*this, t_ctr);
}

Model* Model::clone() const {
    auto* result = new Model(m_env);

    for (const auto& var : vars()) {
        result->add(var, TempVar(
                    get(Attr::Var::Lb, var),
                    get(Attr::Var::Ub, var),
                    get(Attr::Var::Type, var),
                    Column()
                ));
    }

    for (const auto& ctr : ctrs()) {
        result->add(ctr, TempCtr(
                Row(get(Attr::Ctr::Row, ctr)),
                get(Attr::Ctr::Type, ctr)
            ));
    }

    result->set(Attr::Obj::Expr, get(Attr::Obj::Expr));

    return result;
}

AttributeManager &Model::parameter_delegate(const Parameter<double> &t_param) {
    return backend();
}

AttributeManager &Model::parameter_delegate(const Parameter<int> &t_param) {
    return backend();
}

AttributeManager &Model::parameter_delegate(const Parameter<bool> &t_param) {
    return backend();
}
