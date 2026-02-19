//
// Created by Henri on 19/02/2026.
//

#ifndef IDOL_SOLVE_MILP_H
#define IDOL_SOLVE_MILP_H

#include "cxxopts.hpp"
#include "output.h"
#include "idol/modeling.h"
#include "idol/bilevel/modeling/read_from_file.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"

inline void solve_milp(const cxxopts::ParseResult& t_args) {

    using namespace idol;

    const bool verbose = t_args.count("mute") == 0;

    Env env;
    auto model = GLPK::read_from_file(env, t_args["file"].as<std::string>());

    auto gurobi = Gurobi();
    gurobi.with_logs(verbose);

    model.use(gurobi);
    model.optimize();

    report_standard_output(model);

}

#endif //IDOL_SOLVE_MILP_H