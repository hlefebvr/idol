//
// Created by henri on 21/12/22.
//

#ifndef IDOL_BRANCHANDBOUND_EVENTS_H
#define IDOL_BRANCHANDBOUND_EVENTS_H

#include "../callbacks/Events.h"

IDOL_CREATE_EVENT_CLASS(BranchAndBound, 6)

IDOL_CREATE_EVENT(BranchAndBound, RootNodeSolved, 0)
IDOL_CREATE_EVENT(BranchAndBound, RelaxationSolved, 1)
IDOL_CREATE_EVENT(BranchAndBound, NewIncumbentFound, 2)

#endif //IDOL_BRANCHANDBOUND_EVENTS_H
