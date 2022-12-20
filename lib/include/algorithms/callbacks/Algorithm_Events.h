//
// Created by henri on 20/12/22.
//

#ifndef IDOL_ALGORITHM_EVENTS_H
#define IDOL_ALGORITHM_EVENTS_H

#include "Events.h"

IDOL_CREATE_EVENT_CLASS(Algorithm, 3)

IDOL_CREATE_EVENT(Algorithm, Begin, 0)
IDOL_CREATE_EVENT(Algorithm, End, 1)
IDOL_CREATE_EVENT(Algorithm, Iter, 2)

#endif //IDOL_ALGORITHM_EVENTS_H
