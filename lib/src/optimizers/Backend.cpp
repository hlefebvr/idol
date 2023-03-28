//
// Created by henri on 31/01/23.
//
#include "optimizers/Optimizer.h"
#include "modeling/models/Model.h"

double impl::Optimizer::as_numeric(const Constant &t_constant) {

    if (!t_constant.is_numerical()) {
        throw Exception("Constant is not numeric.");
    }

    return t_constant.numerical();
}
