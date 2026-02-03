//
// Created by Henri on 18/01/2026.
//
#include <iostream>
#include <idol/mixed-integer/optimizers/padm/PADM.h>

#include "cxxopts.hpp"
#include "idol/modeling.h"
#include "idol/bilevel/modeling/read_from_file.h"
#include "idol/bilevel/optimizers/KKT/KKT.h"
#include "idol/bilevel/optimizers/StrongDuality/StrongDuality.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"
#include "idol/mixed-integer/optimizers/wrappers/HiGHS/HiGHS.h"
#include "idol/robust/modeling/read_from_file.h"
#include "idol/robust/optimizers/column-and-constraint-generation/ColumnAndConstraintGeneration.h"
#include "idol/robust/optimizers/column-and-constraint-generation/separation/BigMFreeSeparation.h"
#include "idol/robust/optimizers/column-and-constraint-generation/separation/OptimalitySeparation.h"

using namespace idol;

int main(int t_argc, const char ** t_argv) {

    cxxopts::Options options("idol_cli", "idol command line interface");

    options.add_options()
        ("mps", ".mps file for the deterministic model", cxxopts::value<std::string>())
        ("aux", ".aux file for the decision stages", cxxopts::value<std::string>())
        ("par", ".par file for the uncertainty parametrization", cxxopts::value<std::string>())
        ("unc", ".mps file for the uncertainty set", cxxopts::value<std::string>())
        ("method", "Method", cxxopts::value<std::string>()->default_value("ccg"))
        ("view", "View")
        ("h,help", "Print help")
        ("v,verbose", "Verbose mode")
        ("version", "Version");

    const auto result = options.parse(t_argc, t_argv);

    if (result.count("help")) {
        std::cout << options.help() << "\n";
        return 0;
    }

    if (result.count("version")) {
        std::cout << "idol " << IDOL_VERSION << "\n";
        return 0;
    }

    const auto mps = result["mps"].as<std::string>();
    const auto aux = result["aux"].as<std::string>();
    const auto unc_par = result["par"].as<std::string>();
    const auto unc_mps = result["unc"].as<std::string>();
    const auto method = result["method"].as<std::string>();
    const auto view = result["view"].count() > 0;

    Env env;
    auto [model, bilevel_description] = Bilevel::read_from_file(env, aux);
    auto robust_description = Robust::read_from_file(model, unc_par, unc_mps);

    if (view) {
        std::cout << "Parameterized Model:\n";
        std::cout << Robust::Description::View(model, robust_description) << '\n';
        std::cout << "Uncertainty Set:\n";
        std::cout << robust_description.uncertainty_set() << std::endl;
    }

    if (method != "ccg") {
        throw Exception("Unknown method: " + method);
    }

    auto ccg = Robust::ColumnAndConstraintGeneration(robust_description, bilevel_description);
    ccg.with_master_optimizer(Gurobi());
    ccg.with_initial_scenario_by_maximization(Gurobi());
    //ccg.add_separation(Robust::CCG::OptimalitySeparation().with_bilevel_optimizer(kkt));
    ccg.add_separation(
        Robust::CCG::BigMFreeSeparation()
            .with_single_level_optimizer(Gurobi())
            .with_zero_one_uncertainty_set(false)
        );
    ccg.with_check_for_repeated_scenarios(false);
    ccg.with_logs(result["verbose"].count() > 0);

    model.use(ccg);

    model.optimize();

    std::cout << "Total time: " << model.optimizer().time().count() << std::endl;
    std::cout << "Status: " << model.get_status() << " (" << model.get_reason() << ")" << std::endl;
    std::cout << "Objective: " << model.get_best_obj() << std::endl;
    std::cout << "Bound: " << model.get_best_bound() << std::endl;

    return 0;
}