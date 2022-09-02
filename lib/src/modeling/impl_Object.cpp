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
    throw std::runtime_error("not implemented");
    ((ObjectDestructor&) m_env).free(m_it);
}
