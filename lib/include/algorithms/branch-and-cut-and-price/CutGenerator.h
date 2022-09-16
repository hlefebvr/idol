//
// Created by henri on 16/09/22.
//

#ifndef OPTIMIZE_CUTGENERATOR_H
#define OPTIMIZE_CUTGENERATOR_H

#include "BaseGenerator.h"
#include "modeling/variables/Variable.h"

class CutGenerator : public BaseGenerator<Var> {
    CtrType m_type = LessOrEqual;
public:
    CutGenerator() = default;

    CtrType type() const { return m_type; }
};

#endif //OPTIMIZE_CUTGENERATOR_H
