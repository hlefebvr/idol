//
// Created by henri on 27/01/23.
//
#include "modeling/models2/MyModel.h"
#include "modeling/models2/MyEnv.h"
#include "modeling/matrix/Column.h"
#include "modeling/constraints/TempCtr.h"

MyModel::MyModel(MyEnv &t_env) : m_env(t_env), m_id(t_env.create_model_id()) {}

MyModel::~MyModel() {
    for (const auto& var : m_variables) {
        m_env.remove_version(*this, var);
    }
    m_env.free_model_id(*this);
}

MyVar MyModel::create_var(double t_lb, double t_ub, int t_type, Column &&t_column, std::string t_name) {
    auto result = m_env.create<MyVar>(std::move(t_name));
    add_var(result, t_lb, t_ub, t_type, std::move(t_column));
    return result;
}

MyVar MyModel::create_var(double t_lb, double t_ub, int t_type, const Column &t_column, std::string t_name) {
    return create_var(t_lb, t_ub, t_type, Column(t_column), std::move(t_name));
}

MyVar MyModel::create_var(double t_lb, double t_ub, int t_type, std::string t_name) {
    return create_var(t_lb, t_ub, t_type, Column(0.), std::move(t_name));
}

void MyModel::add_var(const MyVar &t_var, double t_lb, double t_ub, int t_type, Column&& t_column) {
    m_env.create_version(*this, t_var, m_variables.size(), t_lb, t_ub);
    m_variables.emplace_back(t_var);
}

void MyModel::add_var(const MyVar &t_var, double t_lb, double t_ub, int t_type, const Column &t_column) {
    add_var(t_var, t_lb, t_ub, t_type, Column(t_column));
}

void MyModel::add_var(const MyVar &t_var, double t_lb, double t_ub, int t_type) {
    add_var(t_var, t_lb, t_ub, t_type, Column(0.));
}

void MyModel::remove(const MyVar &t_var) {
    const auto index = m_env.version(*this, t_var).index();
    m_env.version(*this, m_variables.back()).set_index(index);
    m_variables[index] = m_variables.back();
    m_variables.pop_back();
    m_env.remove_version(*this, t_var);
}

const VarVersion &MyModel::get(const MyVar &t_var) const {
    return m_env.version(*this, t_var);
}

MyCtr MyModel::create_ctr(TempCtr &&t_temp_ctr, std::string t_name) {
    auto result = m_env.create<MyCtr>(std::move(t_name));
    add_ctr(result, std::move(t_temp_ctr));
    return result;
}

MyCtr MyModel::create_ctr(const TempCtr &t_temp_ctr, std::string t_name) {
    return create_ctr(TempCtr(t_temp_ctr), std::move(t_name));
}

MyCtr MyModel::create_ctr(int t_type, std::string t_name) {
    return create_ctr(TempCtr(Row(0, 0), t_type), std::move(t_name));
}

void MyModel::add_ctr(const MyCtr &t_ctr, TempCtr &&t_temp_ctr) {
    m_env.create_version(*this, t_ctr, m_constraints.size(), std::move(t_temp_ctr));
    m_constraints.emplace_back(t_ctr);
}

void MyModel::add_ctr(const MyCtr &t_ctr, const TempCtr &t_temp_ctr) {
    return add_ctr(t_ctr, TempCtr(t_temp_ctr));
}

void MyModel::add_ctr(const MyCtr &t_ctr, int t_type) {
    return add_ctr(t_ctr, TempCtr(Row(0., 0.), t_type));
}
