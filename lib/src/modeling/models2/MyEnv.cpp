//
// Created by henri on 27/01/23.
//
#include "modeling/models2/MyEnv.h"
#include "modeling/models2/MyModel.h"

unsigned int impl::MyEnv::create_model_id() {
    if (m_free_model_ids.empty()) {
        return m_max_model_id++;
    }

    auto result = m_free_model_ids.back();
    m_free_model_ids.pop_back();
    return result;
}

void impl::MyEnv::free_model_id(const ::MyModel &t_model)  {
    m_free_model_ids.emplace_back(t_model.id());
}

void impl::MyEnv::remove_version(const MyModel &t_model, const MyVar &t_var) {
    t_var.remove_version(t_model);
}

const VarVersion &impl::MyEnv::version(const MyModel &t_model, const MyVar &t_var) const {
    return t_var.versions().get(t_model);
}

VarVersion &impl::MyEnv::version(const MyModel &t_model, const MyVar &t_var) {
    return const_cast<MyVar&>(t_var).versions().get(t_model);
}

MyVar impl::MyEnv::create_var(std::string t_name) {

    const unsigned int id = m_max_object_id++;
    std::string name = t_name.empty() ? "Var_" + std::to_string(id) : std::move(t_name);

    m_variables.emplace_front();

    return { m_variables.begin(), m_max_object_id, std::move(name) };
}
