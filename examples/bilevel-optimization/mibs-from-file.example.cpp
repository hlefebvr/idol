//
// Created by henri on 01.10.24.
//
#include <iostream>
#include "idol/modeling.h"
#include "idol/modeling/bilevel-optimization/read_from_file.h"
#include "idol/optimizers/mixed-integer-optimization/wrappers/Gurobi/Gurobi.h"
#include "idol/optimizers/bilevel-optimization/wrappers/MibS/MibS.h"

using namespace idol;

int main(int t_argc, const char** t_argv) {

    const std::string aux_filename = "mibs-from-file.data.aux";

    Env env;
    auto [model, description] = Bilevel::read_from_file<Gurobi>(env, aux_filename);

    model.use(Bilevel::MibS(description).with_logs(true));

    model.optimize();

    if (model.get_status() == Optimal) {
        std::cout << save_primal(model) << std::endl;
    } else {
        std::cout << "No optimal solution found." << std::endl;
    }

    return 0;
}