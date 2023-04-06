//
// Created by henri on 27/01/23.
//
#include "modeling/models/Model.h"
#include "modeling/objects/Env.h"

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

    if (has_optimizer()) {
        optimizer().add(t_var);
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

    if (has_optimizer()) {
        optimizer().remove(t_var);
    }

    remove_column_from_rows(t_var);
    const auto index = m_env.version(*this, t_var).index();
    m_env.version(*this, m_variables.back()).set_index(index);
    m_variables[index] = m_variables.back();
    m_variables.pop_back();
    m_env.remove_version(*this, t_var);
}

void Model::remove(const Ctr &t_ctr) {

    if (has_optimizer()) {
        optimizer().remove(t_ctr);
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

    if (has_optimizer()) {
        optimizer().add(t_ctr);
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

int Model::get_obj_sense() const {
    return m_sense;
}

const Expr<Var, Var> &Model::get_obj() const {
    return m_objective;
}

const LinExpr<Ctr> &Model::get_rhs() const {
    return m_rhs;
}

const Constant &Model::get_mat_coeff(const Ctr &t_ctr, const Var &t_var) const {
    return m_env.version(*this, t_ctr).row().linear().get(t_var);
}

const Row &Model::get_ctr_row(const Ctr &t_ctr) const {
    return m_env.version(*this, t_ctr).row();
}

int Model::get_ctr_type(const Ctr &t_ctr) const {
    return m_env.version(*this, t_ctr).type();
}

unsigned int Model::get_ctr_index(const Ctr &t_ctr) const {
    return (int) m_env.version(*this, t_ctr).index();
}

double Model::get_var_lb(const Var &t_var) const {
    return m_env.version(*this, t_var).lb();
}

double Model::get_var_ub(const Var &t_var) const {
    return m_env.version(*this, t_var).ub();
}

const Column &Model::get_var_column(const Var &t_var) const {
    return m_env.version(*this, t_var).column();
}

unsigned int Model::get_var_index(const Var &t_var) const {
    return m_env.version(*this, t_var).index();
}

int Model::get_var_type(const Var &t_var) const {
    return m_env.version(*this, t_var).type();
}

void Model::set(const Req<double, Var> &t_attr, const Var &t_var, double t_value) {

    if (t_attr == Attr::Var::Lb) {
        m_env.version(*this, t_var).set_lb(t_value);
        if (has_optimizer()) { optimizer().set(t_attr, t_var, t_value); }
        return;
    }

    if (t_attr == Attr::Var::Ub) {
        m_env.version(*this, t_var).set_ub(t_value);
        if (has_optimizer()) { optimizer().set(t_attr, t_var, t_value); }
        return;
    }

    AttributeManagers::Delegate::set(t_attr, t_var, t_value);
}

void Model::set(const Req<Constant, Ctr, Var> &t_attr, const Ctr &t_ctr, const Var &t_var, Constant && t_value) {

    if (t_attr == Attr::Matrix::Coeff) {

        if (has_optimizer()) {
            optimizer().update();
            optimizer().set(t_attr, t_ctr, t_var, t_value);
        }

        update_matrix_coefficient(t_ctr, t_var, std::move(t_value));

        return;
    }

    AttributeManagers::Delegate::set(t_attr, t_ctr, t_var, std::move(t_value));
}

void Model::set(const Req<Constant, Ctr> &t_attr, const Ctr &t_ctr, Constant &&t_value) {

    if (t_attr == Attr::Ctr::Rhs) {
        if (has_optimizer()) {
            add_to_rhs(t_ctr, Constant(t_value));
            optimizer().set(t_attr, t_ctr, std::move(t_value));
        } else {
            add_to_rhs(t_ctr, std::move(t_value));
        }
        return;
    }

    AttributeManagers::Delegate::set(t_attr, t_ctr, std::move(t_value));
}

void Model::set(const Req<Expr<Var, Var>, void> &t_attr, Expr<Var, Var> &&t_value) {

    if (t_attr == Attr::Obj::Expr) {

        if (has_optimizer()) {
            replace_objective(Expr<Var, Var>(t_value));
            optimizer().set(t_attr, std::move(t_value));
        } else {
            replace_objective(std::move(t_value));
        }

        return;
    }

    AttributeManagers::Delegate::set(t_attr, std::move(t_value));
}

void Model::set(const Req<LinExpr<Ctr>, void> &t_attr, LinExpr<Ctr> &&t_value) {

    if (t_attr == Attr::Rhs::Expr) {

        if (has_optimizer()) {
            replace_right_handside(LinExpr<Ctr>(t_value));
            optimizer().set(t_attr, std::move(t_value));
        } else {
            replace_right_handside(std::move(t_value));
        }

        return;
    }

    AttributeManagers::Delegate::set(t_attr, t_value);
}

void Model::set(const Req<Constant, void> &t_attr, Constant &&t_value) {

    if (t_attr == Attr::Obj::Const) {

        if (has_optimizer()) {
            m_objective.constant() = Constant(t_value);
            optimizer().set(t_attr, std::move(t_value));
        } else {
            m_objective.constant() = std::move(t_value);
        }

        return;
    }

    AttributeManagers::Delegate::set(t_attr, t_value);
}

void Model::set(const Req<Constant, Var> &t_attr, const Var &t_var, Constant &&t_value) {

    if (t_attr == Attr::Var::Obj) {

        if (has_optimizer()) {
            add_to_obj(t_var, Constant(t_value));
            optimizer().set(t_attr, t_var, std::move(t_value));
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
        if (has_optimizer()) { optimizer().set(t_attr, t_var, t_value); }
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
        if (has_optimizer()) { optimizer().set(t_attr, t_value); }
        return;
    }

    AttributeManagers::Delegate::set(t_attr, t_value);
}

void Model::set(const Req<int, Ctr> &t_attr, const Ctr &t_ctr, int t_value) {

    if (t_attr == Attr::Ctr::Type) {
        m_env.version(*this, t_ctr).set_type(t_value);
        if (has_optimizer()) { optimizer().set(t_attr, t_ctr, t_value); }
        return;
    }

    AttributeManagers::Delegate::set(t_attr, t_ctr, t_value);
}

void Model::set(const Req<Row, Ctr> &t_attr, const Ctr &t_ctr, Row &&t_value) {

    if (t_attr == Attr::Ctr::Row) {

        remove_row_from_columns(t_ctr);
        m_env.version(*this, t_ctr).row() = std::move(t_value);
        add_row_to_columns(t_ctr);
        if (has_optimizer()) {
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
        if (has_optimizer()) {
            throw Exception("Updating column is not implemented.");
        }

        return;
    }

    AttributeManagers::Delegate::set(t_attr, t_var, t_value);
}

AttributeManager &Model::attribute_delegate(const Attribute &t_attribute) {
    return optimizer();
}

AttributeManager &Model::attribute_delegate(const Attribute &t_attribute, const Var &t_object) {
    return optimizer();
}

AttributeManager &Model::attribute_delegate(const Attribute &t_attribute, const Ctr &t_object) {
    return optimizer();
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
                    get_var_lb(var),
                    get_var_ub(var),
                    get_var_type(var),
                    Column()
                ));
    }

    for (const auto& ctr : ctrs()) {
        result->add(ctr, TempCtr(
                Row(get_ctr_row(ctr)),
                get_ctr_type(ctr)
            ));
    }

    result->set(Attr::Obj::Sense, get_obj_sense());
    result->set(Attr::Obj::Expr, get_obj());

    if (m_optimizer_factory) {
        result->use(*m_optimizer_factory);
    }

    return result;
}

void Model::optimize() {
    throw_if_no_optimizer();
    optimizer().optimize();
}

void Model::write(const std::string& t_name) {
    throw_if_no_optimizer();
    m_optimizer->write(t_name);
}

void Model::update() {
    throw_if_no_optimizer();
    optimizer().update();
}

void Model::use(const OptimizerFactory &t_optimizer_factory) {
    m_optimizer.reset(t_optimizer_factory(*this));
    m_optimizer->build();
    m_optimizer_factory.reset(t_optimizer_factory.clone());
}

void Model::unuse() {
    m_optimizer.reset();
    m_optimizer.reset();
}

bool Model::has_optimizer() const {
    return bool(m_optimizer);
}

int Model::get_status() const {
    throw_if_no_optimizer();
    return m_optimizer->get_status();
}

int Model::get_reason() const {
    throw_if_no_optimizer();
    return m_optimizer->get_reason();
}

double Model::get_best_obj() const {
    throw_if_no_optimizer();
    return m_optimizer->get_best_obj();
}

double Model::get_var_val(const Var &t_var) const {
    throw_if_no_optimizer();
    return m_optimizer->get_var_val(t_var);
}

double Model::get_ctr_farkas(const Ctr &t_ctr) const {
    throw_if_no_optimizer();
    return m_optimizer->get_ctr_farkas(t_ctr);
}

double Model::get_ctr_val(const Ctr &t_ctr) const {
    throw_if_no_optimizer();
    return m_optimizer->get_ctr_val(t_ctr);
}

double Model::get_var_ray(const Var &t_var) const {
    throw_if_no_optimizer();
    return m_optimizer->get_var_ray(t_var);
}

double Model::get_best_bound() const {
    throw_if_no_optimizer();
    return m_optimizer->get_best_bound();
}
