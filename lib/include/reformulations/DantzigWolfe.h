//
// Created by henri on 06/10/22.
//

#ifndef OPTIMIZE_DANTZIGWOLFE_H
#define OPTIMIZE_DANTZIGWOLFE_H

#include "modeling/constraints/Constraint.h"
#include "algorithms/column-generation/ColumnGenerators_Basic.h"
#include <vector>
#include <functional>

class Model;

class DantzigWolfe {
    std::unique_ptr<Ctr> m_convexification_constraint;
    std::unique_ptr<ColumnGenerators::Basic> m_generator;
public:
    Model reformulate(Model& t_rmp, const std::function<bool(const Ctr&)>& t_complicating_constraints_indicator);
};

#endif //OPTIMIZE_DANTZIGWOLFE_H
