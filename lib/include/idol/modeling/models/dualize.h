//
// Created by henri on 06.12.23.
//

#ifndef IDOL_DUALIZE_H
#define IDOL_DUALIZE_H

#include "Model.h"

namespace idol {
    Model dualize(const Model& t_primal, bool t_see_parameters_as_variables_in_the_dual = false);
}

#endif //IDOL_DUALIZE_H
