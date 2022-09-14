//
// Created by henri on 13/09/22.
//

#ifndef OPTIMIZE_SOLUTION_H
#define OPTIMIZE_SOLUTION_H

#include "modeling/variables/Variable.h"
#include "modeling/constraints/Constraint.h"
#include "AbstractSolution.h"

namespace Solution {
    class Primal : public AbstractSolution<Var, Primal> {};
    class Dual : public AbstractSolution<Ctr, Dual> {};
}

#endif //OPTIMIZE_SOLUTION_H
