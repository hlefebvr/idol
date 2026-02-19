//
// Created by Henri on 19/02/2026.
//

#ifndef IDOL_SOLVE_BILEVEL_H
#define IDOL_SOLVE_BILEVEL_H

#include "cxxopts.hpp"
#include "idol/modeling.h"
#include "idol/bilevel/modeling/read_from_file.h"
#include "idol/bilevel/optimizers/wrappers/MibS/MibS.h"

inline void solve_bilevel(const cxxopts::ParseResult& t_args) {

    using namespace idol;

    const bool verbose = t_args.count("mute") == 0;

    Env env;
    auto model = GLPK::read_from_file(env, t_args["file"].as<std::string>());
    auto bilevel_description = Bilevel::read_bilevel_description(model, t_args["bilevel"].as<std::string>());

    auto mibs = Bilevel::MibS(bilevel_description);
    mibs.with_logs(verbose);

    model.use(mibs);
    model.optimize();

    report_standard_output(model);
}

#endif //IDOL_SOLVE_BILEVEL_H