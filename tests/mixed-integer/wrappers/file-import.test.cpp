#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <filesystem>

#include "idol/modeling.h"
#include "idol/mixed-integer/optimizers/wrappers/GLPK/GLPK.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"
#include "idol/mixed-integer/optimizers/wrappers/Cplex/Cplex.h"
#ifdef TEST_GUROBI_IMPORT
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Optimizers_Gurobi.h"
#endif

using namespace idol;

namespace {
Model read_model(Env& t_env, const std::string& t_path) {
#if defined(TEST_GUROBI_IMPORT)
    return Optimizers::Gurobi::read_from_file(t_env, t_path);
#elif defined(TEST_CPLEX_IMPORT)
    return Cplex::read_from_file(t_env, t_path);
#else
    return GLPK::read_from_file(t_env, t_path);
#endif
}

auto optimizer() {
#if defined(TEST_GUROBI_IMPORT)
    return Gurobi();
#elif defined(TEST_CPLEX_IMPORT)
    return Cplex();
#else
    return GLPK();
#endif
}
}

TEST_CASE("Imported minimization objective is unchanged", "[basic][file-import][minimization]") {
    Env env;
    Model model = read_model(env, TEST_DATA_DIR "/minimization.lp");
    REQUIRE(model.vars().size() == 1);
    const auto x = model.get_var_by_index(0);
    CHECK(model.get_var_obj(x) == Catch::Approx(2.));
    CHECK(model.get_obj_expr().affine().constant() == Catch::Approx(0.));
    model.use(optimizer());
    model.optimize();
    CHECK(model.get_status() == Optimal);
    CHECK(model.get_best_obj() == Catch::Approx(2.));
    CHECK(model.get_var_primal(x) == Catch::Approx(1.));
}

TEST_CASE("Imported maximization objective is negated into minimization", "[basic][file-import][maximization]") {
    Env env;
    Model model = read_model(env, TEST_DATA_DIR "/maximization.lp");
    REQUIRE(model.vars().size() == 1);
    const auto x = model.get_var_by_index(0);
    CHECK(model.get_var_obj(x) == Catch::Approx(-2.));
    CHECK(model.get_obj_expr().affine().constant() == Catch::Approx(0.));
    model.use(optimizer());
    model.optimize();
    CHECK(model.get_status() == Optimal);
    CHECK(model.get_best_obj() == Catch::Approx(-6.));
    CHECK(model.get_var_primal(x) == Catch::Approx(3.));
}

#ifdef TEST_CPLEX_IMPORT
TEST_CASE("CPLEX round-trips idol quadratic and SOS constructs", "[file-import][quadratic][sos]") {
    Env env;
    const auto path = (std::filesystem::temp_directory_path() / "idol-cplex-roundtrip.lp").string();
    {
        Model source(env);
        const auto x = source.add_var(0, 4, Continuous, -4, "x");
        const auto y = source.add_var(0, 1, Binary, 0, "y");
        const auto z = source.add_var(0, 1, Binary, 0, "z");
        source.set_obj_expr(x * x - 4 * x);
        source.add_qctr(TempQCtr(x * x - 4, LessOrEqual), "ball");
        source.add_sosctr(true, {y, z}, {1, 2}, "choice");
        source.use(Cplex()); source.write(path);
    }
    Model imported = Cplex::read_from_file(env, path);
    std::filesystem::remove(path);
    CHECK(imported.qctrs().size() == 1);
    CHECK(imported.sosctrs().size() == 1);
    CHECK(imported.get_qctr_by_index(0).name() == "ball");
    CHECK(imported.get_sosctr_by_index(0).name() == "choice");
    imported.use(Cplex()); imported.optimize();
    REQUIRE(imported.get_status() == Optimal);
    CHECK(imported.get_best_obj() == Catch::Approx(-4.));
}
#endif
