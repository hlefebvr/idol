//
// Created by henri on 01/09/22.
//

#include "../../include/modeling/Env.h"
#include "../../include/modeling/impl_Object.h"

Env::~Env() {
    for (auto* ptr : m_objects) {
        delete ptr;
    }
}

void Env::free(const std::list<impl::Object *>::iterator& t_it) {
    delete *t_it;
    m_objects.erase(t_it);
}

void Env::save_object(impl::Object *t_ptr_to_object) {
    m_objects.emplace_back(t_ptr_to_object);
}
