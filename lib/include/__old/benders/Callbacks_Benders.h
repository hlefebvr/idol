//
// Created by henri on 05/01/23.
//

#ifndef IDOL_CALLBACKS_BENDERS_H
#define IDOL_CALLBACKS_BENDERS_H

#include "../Algorithm.h"
#include "algorithms/callbacks/UserCallback.h"

class Benders;

namespace Callbacks {
    class Benders;
}

class Callbacks::Benders : public UserCallback<Algorithm> {
    ::Benders& m_benders;
public:
    explicit Benders(::Benders& t_parent);

    void execute(const EventType &t_event) override;
};

#endif //IDOL_CALLBACKS_BENDERS_H
