//
// Created by henri on 11.12.24.
//

#ifndef IDOL_CCG_FORMULATION_H
#define IDOL_CCG_FORMULATION_H

#include "idol/mixed-integer/modeling/models/Model.h"
#include "idol/robust/modeling/Description.h"

namespace idol::CCG {
    class Formulation;
}

class idol::CCG::Formulation {
    const Model& m_parent;
    const ::idol::Robust::Description &m_description;
public:
    Formulation(const Model& t_parent, const ::idol::Robust::Description &t_description);
};

#endif //IDOL_CCG_FORMULATION_H
