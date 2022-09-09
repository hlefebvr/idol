//
// Created by henri on 09/09/22.
//
#include "solvers/ModelListener.h"
#include "solvers/ModelListenerManager.h"

ModelListener::Id::Id(ModelListenerManager &t_manager, typename std::list<ModelListener *>::iterator &&t_it)
    : m_id(t_it), m_manager(&t_manager) {

}

ModelListener::~ModelListener() {
    for (auto& id : m_ids) {
        id.free();
    }
}

void ModelListener::Id::free() {
    m_manager->free(*this);
}

