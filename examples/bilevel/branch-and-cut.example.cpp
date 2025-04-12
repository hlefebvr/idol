//
// Created by henri on 12.04.25.
//
#include <iostream>
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"
#include "idol/bilevel/optimizers/BranchAndCut/BranchAndCut.h"
#include "idol/bilevel/modeling/read_from_file.h"

using namespace idol;

int main(int t_argc, const char** t_argv) {

    Env env;

    auto [high_point_relaxation, description] = idol::Bilevel::read_from_file<Gurobi>(env, "branch-and-cut.data.aux");

    high_point_relaxation.use(
            Bilevel::BranchAndCut(description)
                    .with_sub_problem_optimizer(Gurobi())
                    .with_logs(true)
    );

    // Optimize and print solution
    high_point_relaxation.optimize();

    std::cout << high_point_relaxation.get_status() << std::endl;
    std::cout << high_point_relaxation.get_reason() << std::endl;
    std::cout << save_primal(high_point_relaxation) << std::endl;

    return 0;
}
