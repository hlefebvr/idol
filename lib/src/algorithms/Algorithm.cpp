//
// Created by henri on 06/12/22.
//
#include "algorithms/Algorithm.h"

void Algorithm::call_callback(const EventType& t_event) {
    m_callback->m_event = &t_event;
    m_callback->execute();
}
