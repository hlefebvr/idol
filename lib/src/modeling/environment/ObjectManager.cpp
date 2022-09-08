//
// Created by henri on 07/09/22.
//
#include "modeling/environment/ObjectManager.h"

std::list<std::unique_ptr<impl::Object>>::iterator impl::ObjectManager::create_placeholder() {
    m_env.m_objects.template emplace_front();
    return m_env.m_objects.begin();
}
