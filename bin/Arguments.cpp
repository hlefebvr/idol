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

#include "idol/mixed-integer/optimizers/wrappers/JuMP/Optimizers_JuMP.h"

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
    if (idol::Optimizers::JuMP::is_available()) {
        std::cout << "available (version " << idol::Optimizers::JuMP::get_version() << ")";
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

    std::cout << "--\tLocal-MIP: ";
#ifdef IDOL_USE_LOCAL_MIP
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

void add_default_args(CLI::App* t_app, Arguments& t_result) {
    t_app->add_option("--method", t_result.method, "Solution method")->configurable();
    t_app->add_option("--time-limit", t_result.time_limit, "Time limit in seconds")->configurable();
    t_app->add_option("--tol-feasibility", t_result.tol_feasibility, "Tolerance used to check feasibility")->configurable();
    t_app->add_flag("--csv-report", t_result.csv_report, "If set to true, additionally prints a report in csv format")->configurable();
}

Arguments Arguments::parse(int t_argc, const char** t_argv) {

    Arguments result;
    bool show_version = false;

    CLI::App app {"idol command line interface"};

    // Configuration file and default tools (help, version, etc.)
    app.require_subcommand(0, 2);
    app.allow_config_extras(false);
    app.set_help_flag("--help, -h", "Prints help.");
    app.set_help_all_flag("--help-all", "Prints help of all sub-commands.");
    app.add_flag("--version,-v", show_version, "Prints the version and third-party tools versions.");
    auto* config = app.set_config("--config,-c", "", "Reads a configuration file (.ini).")->check(CLI::ExistingFile);

    auto* solve = app.add_subcommand("solve", "Solves a given problem.");
    auto* list = app.add_subcommand("list", "Lists available methods for a given problem.");

    /// MILP
    const auto add_args_for_milp = [&](auto* t_target) {

        t_target->add_option("file",  result.file, ".mps or .lp file")->required()->check(CLI::ExistingFile);
        t_target->add_option("--jump-optimizer", result.jump_optimizer, "JuMP optimizer to be used (required if --method JUMP)")->configurable();
        t_target->add_option("--default-method", result.default_milp_method, "Specifies a MILP method that is used to solved underlying MILPs in, e.g., decomposition algorithms, reformulation approaches, etc.")->configurable();

        add_default_args(t_target, result);

        return t_target;
    };
    auto* solve_milp = add_args_for_milp(solve->add_subcommand("milp", "MILP"));
    auto* list_milp = add_args_for_milp(list->add_subcommand("milp", "MILP"));

    /// Bilevel
    const auto add_args_for_bilevel = [&](auto* t_target) {

        t_target->add_option("file",  result.file, ".mps or .lp file containing the single-level relaxation")->required()->check(CLI::ExistingFile);
        auto* aux = t_target->add_option("--aux", result.aux_file, ".aux file")->configurable()->check(CLI::ExistingFile);
        auto* ce = t_target->add_option("--ce", result.ce_file, "counterfactual explanation input file")->configurable()->check(CLI::ExistingFile);
        t_target->add_flag("--pessimistic", result.pessimistic, "For bilevel problems, indicates to solve the pessimistic variant of the problem")->configurable();
        t_target->add_flag("--no-kleinert-vi", result.no_kleinert_vi, "For bilevel problems, for KKT-based approaches, indicates to not use the valid inequalities from Kleinart et al. (2020) [https://doi.org/10.1007/s11590-020-01660-6]")->configurable();
        t_target->add_option("--bound-provider", result.bound_provider, "Provides a file storing big-M values for KKT-based reformulations.")->configurable();
        t_target->add_option("--default-method", result.default_milp_method, "Specifies a MILP method that is used to solved underlying MILPs in, e.g., decomposition algorithms, reformulation approaches, etc.")->configurable();

        add_default_args(t_target, result);

        aux->excludes(ce);
        ce->excludes(aux);

        t_target->require_option(2, 0);

        return t_target;
    };
    auto* solve_bilevel = add_args_for_bilevel(solve->add_subcommand("bilevel", "Bilevel"));
    auto* list_bilevel = add_args_for_bilevel(list->add_subcommand("bilevel", "Bilevel"));

    /// Robust
    const auto add_args_for_robust = [&](auto* t_target) {
        t_target->add_option("file",  result.file, ".mps or .lp file containing the deterministic problem")->required()->check(CLI::ExistingFile);
        t_target->add_option("--aux", result.aux_file, ".aux file used to indicate stages")->configurable();
        auto* uncertainty_param = t_target->add_option("--uncertainty-param,--par", result.uncertainty_param_file, ".par file for bilevel and two-stage robust problems")->configurable();
        auto* uncertainty_set = t_target->add_option("--uncertainty-set,--unc", result.uncertainty_set_file, ".mps/.lp file for the uncertainty set in robust problems")->configurable();
        t_target->add_option("--bound-provider", result.bound_provider, "For two-stage robust problems, provides a file storing big-m values for KKT-based reformulations.")->configurable();
        t_target->add_option("--default-method", result.default_milp_method, "Specifies a MILP method that is used to solved underlying MILPs in, e.g., decomposition algorithms, reformulation approaches, etc.")->configurable();
        t_target->add_flag("--complete-recourse", result.complete_recourse, "If present, indicates that the two-stage robust problem has complete recourse.")->configurable();

        uncertainty_set->needs(uncertainty_param);
        uncertainty_param->needs(uncertainty_set);

        add_default_args(t_target, result);
        return t_target;
    };
    auto* solve_robust = add_args_for_robust(solve->add_subcommand("robust", "Robust"));
    auto* list_robust = add_args_for_robust(list->add_subcommand("robust", "Robust"));

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

    if (!*list && !*solve) {
        std::cerr << "a subcommand is mandatory\nRun with --help or --help-all for more information." << std::endl;
        exit(1);
    }

    if ((*solve && !*solve_milp && !*solve_bilevel && !*solve_robust)
        || (*list && !*list_milp && !*list_bilevel && !*list_robust)
    ) {
        std::cerr << "a problem type is mandatory\nRun with --help or --help-all for more information." << std::endl;
        exit(1);
    }

    if (*list) {
        result.solve = false;
    }

    if (*config) {
        std::cout << "-- The configuration file is " << config->as<std::string>() << std::endl;
    } else {
        std::cout << "-- No configuration file loaded" << std::endl;
    }

    if (*solve_robust || *list_robust) {
        result.problem_type = RobustProblem;
        if (!result.aux_file.empty()) {
            result.problem_type = AdjustableRobustProblem;
        }
    } else if (*solve_bilevel || *list_bilevel) {
        result.problem_type = BilevelProblem;
    }

    std::cout << "-- The main input file is " << result.file << std::endl;
    std::cout << "-- Detected: problem type is " << result.problem_type << std::endl;

    return result;
}
