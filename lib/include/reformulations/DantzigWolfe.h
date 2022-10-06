//
// Created by henri on 06/10/22.
//

#ifndef OPTIMIZE_DANTZIGWOLFE_H
#define OPTIMIZE_DANTZIGWOLFE_H

#include "modeling/constraints/Constraint.h"
#include "algorithms/column-generation/ColumnGenerators_Basic.h"
#include <vector>

class Model;

class DantzigWolfe {
    Ctr m_convexification_constraint;
    Model m_subproblem;
    ColumnGenerators::Basic m_generator;
public:
    DantzigWolfe(Model& t_rmp, const std::vector<Var>& t_subproblem_variables);
};

#endif //OPTIMIZE_DANTZIGWOLFE_H
