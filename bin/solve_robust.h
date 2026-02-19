//
// Created by Henri on 19/02/2026.
//

#ifndef IDOL_SOLVE_ROBUST_H
#define IDOL_SOLVE_ROBUST_H

#include "cxxopts.hpp"
#include "idol/modeling.h"
#include "idol/bilevel/modeling/read_from_file.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"

inline void solve_robust(const cxxopts::ParseResult& t_args) {

    using namespace idol;

    Env env;
    // auto robust_description = Robust::read_from_file(model, uncertainty_param, uncertainty_set);

    throw Exception("Not implemented.");
    
}

#endif //IDOL_SOLVE_ROBUST_H