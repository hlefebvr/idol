//
// Created by henri on 06/12/22.
//
#include "algorithms/Algorithm.h"
#include "algorithms/callbacks/Events.h"

void Algorithm::call_callback(const EventType& t_event) {
    if (!m_callback) {
        return;
    }
    m_callback->execute(t_event);
}
