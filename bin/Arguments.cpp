//
// Created by Henri on 02/03/2026.
//
#include "Arguments.h"
#include "idol/general/optimizers/logs.h"
#include "idol/general/utils/exceptions/Exception.h"
#include "idol/mixed-integer/optimizers/wrappers/GLPK/Optimizers_GLPK.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Optimizers_Gurobi.h"
#include "idol/mixed-integer/optimizers/wrappers/HiGHS/Optimizers_HiGHS.h"

#include <CLI/CLI.hpp>

void Arguments::print_splash() {

    constexpr int width = 40;

    // Top border
    std::cout << "╔════════════════════════════════════════╗\n";

    // Title
    std::cout << "║";
    idol::center(std::cout, std::string("idol ") + IDOL_VERSION, width);
    std::cout << "║\n";

    // Subtitle
    std::cout << "║";
    idol::center(std::cout, "A C++ Framework for Optimization", width);
    std::cout << "║\n";

    // Credit
    std::cout << "║";
    idol::center(std::cout, "by Henri Lefebvre, 2026", width);
    std::cout << "║\n";

    // Website
    std::cout << "║";
    idol::center(std::cout, "https://henrilefebvre.com/idol", width);
    std::cout << "║\n";

    // Bottom border
    std::cout << "╚════════════════════════════════════════╝\n";
}

void Arguments::print_versions() {

    std::cout << "-- Dependencies\n";

    std::cout << "--\tCplex: ";
    if (false /* || idol::Optimizers::Cplex::is_available() */) {
        //std::cout << "available (version " << idol::Optimizers::Cplex::get_version() << ")";
    } else {
        std::cout << "not found";
    }
    std::cout << "\n";

    std::cout << "--\tGLPK: ";
    if (idol::Optimizers::GLPK::is_available()) {
        std::cout << "available (version " << idol::Optimizers::GLPK::get_version() << ")";
    } else {
        std::cout << "not found";
    }
    std::cout << "\n";

    std::cout << "--\tGurobi: ";
    if (idol::Optimizers::Gurobi::is_available()) {
        std::cout << "available (version " << idol::Optimizers::Gurobi::get_version() << ")";
    } else {
        std::cout << "not found";
    }
    std::cout << "\n";

    std::cout << "--\tHiGHS: ";
    if (idol::Optimizers::HiGHS::is_available()) {
        std::cout << "available (version " << idol::Optimizers::HiGHS::get_version() << ")";
    } else {
        std::cout << "not found";
    }
    std::cout << "\n";

    std::cout << "--\tJulia: ";
    if (false /* || idol::Optimizers::Julia::is_available() */) {
        //std::cout << "available (version " << idol::Optimizers::Julia::get_version() << ")";
    } else {
        std::cout << "not found";
    }
    std::cout << "\n";

    std::cout << "--\tMibS: ";
#ifdef IDOL_USE_MIBS
    std::cout << "shipped with idol";
#else
    std::cout << "not found";
#endif
    std::cout << "\n";
}

Arguments::Arguments() {
    print_splash();
}

std::ostream& operator<<(std::ostream& t_os, ProblemType t_problem_type) {
    switch (t_problem_type) {
    case MILP: return t_os << "MILP";
    case RobustProblem: return t_os << "robust problem";
    case AdjustableRobustProblem: return t_os << "adjustable robust problem";
    case BilevelProblem: return t_os << "bilevel problem";
    default:;
    }
    throw std::runtime_error("Undefined problem type.");
}

void add_file_options(CLI::App* t_app, Arguments& t_result) {
    auto file = t_app->add_option("file",  t_result.file, "Input file")->required()->check(CLI::ExistingFile);
    auto bilevel = t_app->add_option("--bilevel,--stages,--aux", t_result.aux_file, ".aux file for bilevel and two-stage robust problems");
    auto uncertainty_param = t_app->add_option("--uncertainty-param,--par", t_result.uncertainty_param_file, ".par file for bilevel and two-stage robust problems");
    auto uncertainty_set = t_app->add_option("--uncertainty-set,--unc", t_result.uncertainty_set_file, ".mps/.lp file for the uncertainty set in robust problems");

    uncertainty_set->needs(uncertainty_param);
    uncertainty_param->needs(uncertainty_set);
}

Arguments Arguments::parse(int t_argc, const char** t_argv) {

    Arguments result;
    bool show_version;

    CLI::App app {"idol command line interface"};

    app.require_subcommand(0, 1);
    app.set_help_flag("--help, -h");
    app.set_help_all_flag("--help-all");
    app.add_flag("--version,-v", show_version, "Print version and third party tools versions.");

    /* solve subcommand */
    auto solve = app.add_subcommand("solve", "Solve an instance");
    add_file_options(solve, result);
    solve->add_option("--time-limit", result.time_limit, "Time limit in seconds");
    solve->add_option("--method", result.method, "Solution method");

    /* list-method subcommand */
    auto list_methods = app.add_subcommand("list-methods", "List available methods for the given problem");
    add_file_options(list_methods, result);

    /* PARSE ARGUMENTS */
    try {
        app.parse(t_argc, t_argv);
    } catch (const CLI::ParseError &e) {
        app.exit(e);
        exit(e.get_exit_code());
    }

    if (show_version) {
        print_versions();
        exit(0);
    }

    if (*list_methods) {
        result.solve = false;
    }

    if (!result.uncertainty_param_file.empty()) {
        result.problem_type = RobustProblem;
        if (!result.aux_file.empty()) {
            result.problem_type = AdjustableRobustProblem;
        }
    } else if (!result.aux_file.empty()) {
        result.problem_type = BilevelProblem;
    }

    std::cout << "-- The main input file is " << result.file << std::endl;
    std::cout << "-- Detected: problem type is " << result.problem_type << std::endl;

    return result;
}
