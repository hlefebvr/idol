//
// Created by henri on 27/01/23.
//
#include "modeling/models/Env.h"
#include "modeling/models/Model.h"

unsigned int impl::Env::create_model_id() {
    if (m_free_model_ids.empty()) {
        return m_max_model_id++;
    }

    auto result = m_free_model_ids.back();
    m_free_model_ids.pop_back();
    return result;
}

void impl::Env::free_model_id(const ::Model &t_model)  {
    m_free_model_ids.emplace_back(t_model.id());
}
