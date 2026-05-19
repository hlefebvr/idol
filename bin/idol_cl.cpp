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

    switch (args.problem_type) {
        case MILP: milp(args); break;
        case BilevelProblem: solve_bilevel(args); break;
        case RobustProblem: [[fallthrough]];
        case AdjustableRobustProblem: robust(args); break;
        default: throw std::runtime_error("Undefined problem type.");
    }

    return 0;
}