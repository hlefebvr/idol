//
// Created by Henri on 02/03/2026.
//

#ifndef IDOL_PARSE_ARGUMENTS_H
#define IDOL_PARSE_ARGUMENTS_H

#include <string>
#include "idol/mixed-integer/optimizers/branch-and-bound/logs/Info.h"

enum ProblemType { MILP, RobustProblem, AdjustableRobustProblem, BilevelProblem };

std::ostream& operator<<(std::ostream& t_os, ProblemType t_problem_type);

class Arguments {
    static void print_splash();
    static void print_versions();
public:
    Arguments();

    ProblemType problem_type = MILP;
    bool mute = false;
    bool solve = true;
    std::string method;

    // MILPs
    std::string file;
    double time_limit = idol::Inf;
    std::string jump_optimizer;
    std::string default_milp_method;

    // Bilevel
    std::string aux_file;
    bool pessimistic = false;
    bool no_kleinert_vi = false;
    std::string bound_provider;

    // Robust
    std::string uncertainty_set_file;
    std::string uncertainty_param_file;

    static Arguments parse(int t_argc, const char** t_argv);
};

#endif //IDOL_PARSE_ARGUMENTS_H