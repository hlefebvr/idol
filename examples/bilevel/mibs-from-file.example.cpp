//
// Created by henri on 01.10.24.
//
#include <iostream>
#include "idol/modeling.h"
#include "idol/bilevel/modeling/read_from_file.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"
#include "idol/bilevel/optimizers/wrappers/MibS/MibS.h"

using namespace idol;

int main(int t_argc, const char** t_argv) {

    const std::string aux_filename = "mibs-from-file.data.aux";

    Env env;
    auto [model, description] = Bilevel::read_from_file(env, aux_filename);

    model.use(Bilevel::MibS(description).with_logs(true));

    model.optimize();

    if (model.get_status() == Optimal) {
        std::cout << save_primal(model) << std::endl;
    } else {
        std::cout << "No optimal solution found." << std::endl;
    }

    return 0;
}