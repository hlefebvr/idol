#include <catch2/catch_all.hpp>

#include <filesystem>
#include <fstream>
#include <map>

#include "idol/modeling.h"
#include "idol/mixed-integer/optimizers/wrappers/GLPK/GLPK.h"

using namespace idol;

namespace {

std::filesystem::path solution_path(const std::string& t_name) {
    return std::filesystem::temp_directory_path() / ("idol-" + t_name + ".sol");
}

}

TEST_CASE("Write a primal solution in Gurobi SOL format", "[write-primal-solution]") {
    Env env;
    Model model(env);
    const auto precise = model.add_var(0., Inf, Continuous, 1., "precise_value");
    const auto zero = model.add_var(0., Inf, Continuous, 0., "zero_value");
    model.add_ctr(precise == 0.12345678901234566);
    model.use(GLPK());
    model.optimize();
    REQUIRE(model.get_status() == Optimal);

    const auto path = solution_path("primal-writer");
    model.write(path.string());

    std::ifstream file(path);
    REQUIRE(file);
    std::string line;
    REQUIRE(std::getline(file, line));
    REQUIRE(line.starts_with("# Objective value = "));
    CHECK(std::stod(line.substr(20)) == model.get_best_obj());

    std::map<std::string, double> values;
    std::string name;
    double value;
    while (file >> name >> value) {
        values[name] = value;
    }
    REQUIRE(values.size() == 2);
    CHECK(values.at(precise.name()) == model.get_var_primal(precise));
    CHECK(values.at(zero.name()) == 0.);
    std::filesystem::remove(path);
}

TEST_CASE("Reject primal solution output without an incumbent", "[write-primal-solution]") {
    Env env;
    Model model(env);
    model.add_var(0., 1., Continuous, 0., "x");
    model.use(GLPK());
    CHECK_THROWS_WITH(model.write(solution_path("no-incumbent").string()),
                      "Cannot write primal solution: no incumbent is available.");
}

TEST_CASE("Report primal solution file errors", "[write-primal-solution]") {
    Env env;
    Model model(env);
    model.add_var(1., 1., Continuous, 0., "x");
    model.use(GLPK());
    model.optimize();
    REQUIRE(model.get_status() == Optimal);

    const auto path = solution_path("missing-directory") / "solution.sol";
    CHECK_THROWS_WITH(model.write(path.string()),
                      "Could not open primal solution file \"" + path.string() + "\".");
}
