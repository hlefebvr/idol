//
// Created by henri on 31/01/23.
//
#include "optimizers/Backend.h"
#include "modeling/models/AbstractModel.h"

double impl::Backend::as_numeric(const Constant &t_constant) {

    if (!t_constant.is_numerical()) {
        throw Exception("Constant is not numeric.");
    }

    return t_constant.numerical();
}
