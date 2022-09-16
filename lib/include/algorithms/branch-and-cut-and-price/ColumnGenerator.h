//
// Created by henri on 15/09/22.
//

#ifndef OPTIMIZE_COLUMNGENERATOR_H
#define OPTIMIZE_COLUMNGENERATOR_H

#include "BaseGenerator.h"
#include "modeling/constraints/Constraint.h"

class ColumnGenerator : public BaseGenerator<Ctr> {
    VarType m_type = Continuous;
    double m_lower_bound = 0.;
    double m_upper_bound = Inf;
public:
    ColumnGenerator() = default;

    VarType type() const { return m_type; }

    double lb() const { return m_lower_bound; }

    double ub() const { return m_upper_bound; }
};

#endif //OPTIMIZE_COLUMNGENERATOR_H
