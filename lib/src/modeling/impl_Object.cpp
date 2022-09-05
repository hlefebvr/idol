//
// Created by henri on 01/09/22.
//

#include <stdexcept>
#include "../../include/modeling/impl_Object.h"
#include "../../include/modeling/Env.h"

unsigned int impl::Object::s_id = 0;

impl::Object::Object(Env &t_env, unsigned int t_index, std::string&& t_name)
    : m_env(t_env), m_index(t_index), m_name(std::move(t_name)) {

}

void impl::Object::free() {
    ((ObjectDestructor&) m_env).free(m_death_id);
}

void impl::Object::set_death_id(std::list<impl::Object *>::iterator &&t_id) {
    m_death_id = std::move(t_id);
}
