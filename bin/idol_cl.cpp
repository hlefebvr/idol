//
// Created by Henri on 18/01/2026.
//
#include <iostream>

#include "Arguments.h"
#include "solve_milp.h"
#include "solve_robust.h"
#include "solve_bilevel.h"

int main(int t_argc, const char** t_argv) {

    const auto args = Arguments::parse(t_argc, t_argv);

    switch (args.problem_type) {
        case MILP: solve_milp(args); break;
        case BilevelProblem: solve_bilevel(args); break;
        case RobustProblem: throw idol::Exception("Not available yet.");
        case AdjustableRobustProblem: solve_adjustable_robust(args); break;
        default: throw std::runtime_error("Sorry, an error occurred... Undefined problem type.");
    }

    return 0;
}