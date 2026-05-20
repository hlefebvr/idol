//
// Created by Henri on 18/01/2026.
//
#include <iostream>

#include "Arguments.h"
#include "milp.h"
#include "robust.h"
#include "solve_bilevel.h"

int main(int t_argc, const char** t_argv) {

    const auto args = Arguments::parse(t_argc, t_argv);

    if (args.problem_type == MILP) {
        milp(args);
    } else if (args.problem_type == BilevelProblem) {
        bilevel(args);
    } else if (args.problem_type == RobustProblem || args.problem_type == AdjustableRobustProblem) {
        robust(args);
    } else {
        std::cerr << "Unknown problem type." << std::endl;
        exit(1);
    }

    return 0;
}
