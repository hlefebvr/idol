//
// Created by henri on 26/10/22.
//

#ifndef IDOL_OPERATORS_H
#define IDOL_OPERATORS_H

#include "operators_Constant.h"
#include "operators_Var.h"
#include "operators_Ctr.h"
#include "operators_utils.h"

namespace idol {
    double evaluate(const Expr<Var, Var>& t_expr, const Solution::Primal& t_values);
}

#endif //IDOL_OPERATORS_H
