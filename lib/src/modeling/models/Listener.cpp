//
// Created by henri on 09/09/22.
//
#include "modeling/models/Listener.h"
#include "modeling/models/ListenerManager.h"

Listener::Id::Id(ListenerManager &t_manager, typename std::list<Listener *>::iterator &&t_it)
    : m_id(t_it), m_manager(&t_manager) {

}

Listener::~Listener() {
    for (auto& id : m_ids) {
        id.free();
    }
}

void Listener::Id::free() {
    m_manager->free(*this);
}

