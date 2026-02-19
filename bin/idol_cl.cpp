//
// Created by Henri on 18/01/2026.
//
#include <iostream>
#include "solve_milp.h"
#include "solve_robust.h"
#include "solve_bilevel.h"
#include "idol/general/optimizers/logs.h"

enum ProblemType { MILP, RobustProblem, BilevelProblem, RobustBilevelProblem };

std::ostream& operator<<(std::ostream& t_os, ProblemType t_problem_type) {
    switch (t_problem_type) {
        case MILP: return t_os << "MILP";
        case RobustProblem: return t_os << "robust problem";
        case BilevelProblem: return t_os << "bilevel problem";
        default:;
    }
    throw std::runtime_error("Undefined problem type.");
}

void print_splash() {

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

int main(int t_argc, const char ** t_argv) {

    print_splash();

    cxxopts::Options options("idol_cli", "idol command line interface");

    options.add_options()
        ("file", "Main model (.mps/.lp)", cxxopts::value<std::string>())
        ("bilevel", "Bilevel structure / decision stages (.aux)", cxxopts::value<std::string>())
        ("uncertainty-param", "Uncertainty parametrization (.par)", cxxopts::value<std::string>())
        ("uncertainty-set", "Uncertainty set (.mps)", cxxopts::value<std::string>())
        ("help", "Print help")
        ("mute", "Mute mode")
        ("version", "Version");

    options.parse_positional({"file"});
    options.positional_help("file");

    const auto args = options.parse(t_argc, t_argv);

    if (args.count("help")) {
        std::cout << options.help() << "\n";
        return 0;
    }

    if (args.count("version")) {
        std::cout << "idol " << IDOL_VERSION << "\n";
        return 0;
    }

    const bool has_file = args.count("file") > 0;
    const bool has_bilevel = args.count("bilevel") > 0;
    const bool has_uncertainty_param = args.count("uncertainty-param") > 0;
    const bool has_uncertainty_set = args.count("uncertainty-set") > 0;

    ProblemType problem_type = MILP;

    if (!has_file) {
        std::cerr << "An .lp/.mps file is mandatory." << std::endl;
        std::cout << options.help() << "\n";
        return 1;
    }

    if (has_bilevel) {
        problem_type = BilevelProblem;
    }

    if (has_uncertainty_param || has_uncertainty_set) {
        problem_type = RobustProblem;
        if (!has_uncertainty_param || !has_uncertainty_set) {
            std::cerr << "For robust problems, you must provide both an uncertainty parametrization and an uncertainty set." << std::endl;
            std::cout << options.help() << "\n";
            return 1;
        }
    }

    std::cout << "-- The main input file is " << args["file"].as<std::string>() << ".\n";

    std::cout << '\n';

    switch (problem_type) {
        case MILP: solve_milp(args); break;
        case BilevelProblem: solve_bilevel(args); break;
        case RobustProblem: solve_robust(args); break;
        default: throw std::runtime_error("Sorry, an error occurred... Undefined problem type.");
    }

    return 0;
}