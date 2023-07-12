//
// Created by henri on 13/09/22.
//

#ifndef OPTIMIZE_SOLUTION_H
#define OPTIMIZE_SOLUTION_H

#include "../variables/Var.h"
#include "../constraints/Ctr.h"
#include "AbstractSolution.h"

namespace idol::Solution {
    class Primal : public AbstractSolution<Var, Primal> {};
    class Dual : public AbstractSolution<Ctr, Dual> {};
}

static idol::Solution::Primal operator+(idol::Solution::Primal t_a, const idol::Solution::Primal& t_b) {
    return t_a += t_b;
}

static idol::Solution::Dual operator+(idol::Solution::Dual t_a, const idol::Solution::Dual& t_b) {
    return t_a += t_b;
}

static idol::Solution::Primal operator*(double t_factor, idol::Solution::Primal t_solution) {
    return t_solution *= t_factor;
}

static idol::Solution::Dual operator*(double t_factor, idol::Solution::Dual t_solution) {
    return t_solution *= t_factor;
}

#endif //OPTIMIZE_SOLUTION_H
