//
// Created by henri on 05/01/23.
//
#include "algorithms/benders/Callbacks_Benders.h"
#include "algorithms/callbacks/Events.h"
#include "algorithms/benders/Benders.h"
#include "algorithms/branch-and-bound/BranchAndBound_Events.h"

Callbacks::Benders::Benders(::Benders &t_parent) : m_benders(t_parent) {

}

void Callbacks::Benders::execute(const EventType &t_event) {

    if (t_event == Event_::BranchAndBound::NewIncumbentFound) {
        m_benders.main_loop(false);
    }

}