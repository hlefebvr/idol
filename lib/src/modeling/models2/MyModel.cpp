//
// Created by henri on 27/01/23.
//
#include "modeling/models2/MyModel.h"
#include "modeling/models2/MyEnv.h"

MyModel::MyModel(MyEnv &t_env) : m_env(t_env), m_id(t_env.create_model_id()) {}

MyModel::~MyModel() {
    for (const auto& var : m_variables) {
        m_env.remove_version(*this, var);
    }
    m_env.free_model_id(*this);
}

MyVar MyModel::create_var(double t_lb, double t_ub, std::string t_name) {
    auto result = m_env.create_var(std::move(t_name));
    m_env.create_version(*this, result, m_variables.size(), t_lb, t_ub);
    m_variables.emplace_back(result);
    return result;
}

void MyModel::add_var(const MyVar &t_var, double t_lb, double t_ub) {
    m_env.create_version(*this, t_var, m_variables.size(), t_lb, t_ub);
    m_variables.emplace_back(t_var);
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

