//
// Created by henri on 13/09/22.
//

#ifndef OPTIMIZE_SOLUTION_H
#define OPTIMIZE_SOLUTION_H

#include "../variables/Var.h"
#include "../constraints/Ctr.h"
#include "AbstractSolution.h"

namespace Solution {
    class Primal : public AbstractSolution<Var, Primal> {};
    class Dual : public AbstractSolution<Ctr, Dual> {};
}

static Solution::Primal operator*(double t_factor, Solution::Primal t_solution) {
    return t_solution *= t_factor;
}

static Solution::Dual operator*(double t_factor, Solution::Dual t_solution) {
    return t_solution *= t_factor;
}

#endif //OPTIMIZE_SOLUTION_H
