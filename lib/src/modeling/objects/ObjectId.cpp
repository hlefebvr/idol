//
// Created by henri on 07/09/22.
//
#include "modeling/objects/ObjectId.h"
#include "modeling/environment/Env.h"

unsigned int ObjectId::s_id = 0;

ObjectId::ObjectId(const std::list<std::unique_ptr<impl::Object>>::iterator &t_it,
                   unsigned int t_model_id,
                   std::string &&t_name,
                   const std::string& t_default_name)
        : m_it(t_it),
          m_name(t_name.empty() ? t_default_name + '(' + std::to_string(m_id) + ')' : std::move(t_name)),
          m_model_id(t_model_id) {

}

const std::string &ObjectId::name() const {
    return m_name;
}

unsigned int ObjectId::id() const {
    return m_id;
}

unsigned int ObjectId::model_id() const {
    return m_model_id;
}

void ObjectId::set_model_id(unsigned int t_id) {
    m_model_id = t_id;
}
