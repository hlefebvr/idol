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

#include <iostream>

void Env::free(const std::list<impl::Object *>::iterator& t_it) {
    auto* ptr = *t_it;
    m_objects.erase(t_it);
    delete ptr;
}

void Env::save_object(impl::Object *t_ptr_to_object) {
    m_objects.emplace_front(t_ptr_to_object);
    ((impl::Destructible*) t_ptr_to_object)->set_death_id(m_objects.begin());
}
