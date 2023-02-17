//
// Created by henri on 31/01/23.
//
#include "backends/Backend.h"
#include "modeling/expressions/Constant.h"

double impl::Backend::as_numeric(const Constant &t_constant) {

    if (!t_constant.is_numerical()) {
        throw Exception("Constant is not numeric.");
    }

    return t_constant.numerical();
}
