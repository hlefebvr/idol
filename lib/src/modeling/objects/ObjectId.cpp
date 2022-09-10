//
// Created by henri on 07/09/22.
//
#include "modeling/objects/ObjectId.h"
#include "modeling/environment/Env.h"

unsigned int ObjectId::s_id = 0;

ObjectId::ObjectId(const std::list<std::unique_ptr<impl::Object>>::iterator &t_it, std::string &&t_name, const std::string& t_default_name)
        : m_it(t_it),
          m_name(t_name.empty() ? t_default_name + '(' + std::to_string(m_id) + ')' : std::move(t_name)) {

}

const std::string &ObjectId::name() const {
    return m_name;
}

unsigned int ObjectId::id() const {
    return m_id;
}
