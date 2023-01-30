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
