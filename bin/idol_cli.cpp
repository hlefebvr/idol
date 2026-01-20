//
// Created by Henri on 18/01/2026.
//
#include <iostream>
#include "cxxopts.hpp"
#include "idol/modeling.h"
#include "idol/bilevel/modeling/read_from_file.h"
#include "idol/bilevel/optimizers/KKT/KKT.h"
#include "idol/bilevel/optimizers/StrongDuality/StrongDuality.h"
#include "idol/mixed-integer/optimizers/wrappers/GLPK/GLPK.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"
#include "idol/mixed-integer/optimizers/wrappers/HiGHS/HiGHS.h"
#include "idol/robust/modeling/read_from_file.h"
#include "idol/robust/optimizers/column-and-constraint-generation/ColumnAndConstraintGeneration.h"

using namespace idol;

int main(int t_argc, const char ** t_argv) {

    cxxopts::Options options("idol_cli", "idol command line interface");

    options.add_options()
        ("mps", "mps file", cxxopts::value<std::string>())
        ("aux", "aux file", cxxopts::value<std::string>())
        ("par", "uncertainty parameterization file", cxxopts::value<std::string>())
        ("unc", "uncertainty file", cxxopts::value<std::string>())
        ("method", "Method", cxxopts::value<std::string>()->default_value("ccg"))
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

    Env env;
    auto [model, bilevel_description] = Bilevel::read_from_file(env, aux);
    auto robust_description = Robust::read_from_file(model, unc_par, unc_mps);

    if (method != "ccg") {
        throw Exception("Unknown method: " + method);
    }

    auto kkt = Bilevel::StrongDuality();
    kkt.with_single_level_optimizer(Gurobi());

    auto ccg = Robust::ColumnAndConstraintGeneration(robust_description, bilevel_description);
    ccg.with_master_optimizer(Gurobi());
    ccg.add_joint_separation_optimizer(kkt);
    ccg.with_logs(result["verbose"].count() > 0);

    model.use(ccg);

    model.optimize();

    std::cout << "Total time: " << model.optimizer().time().count() << std::endl;
    std::cout << "Status: " << model.get_status() << std::endl;
    std::cout << "Objective: " << model.get_best_obj() << std::endl;
    std::cout << "Bound: " << model.get_best_bound() << std::endl;

    return 0;
}