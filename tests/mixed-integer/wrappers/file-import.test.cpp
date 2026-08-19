#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "idol/modeling.h"
#include "idol/mixed-integer/optimizers/wrappers/GLPK/GLPK.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"
#ifdef TEST_GUROBI_IMPORT
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Optimizers_Gurobi.h"
#endif

using namespace idol;

namespace {
Model read_model(Env& t_env, const std::string& t_path) {
#ifdef TEST_GUROBI_IMPORT
    return Optimizers::Gurobi::read_from_file(t_env, t_path);
#else
    return GLPK::read_from_file(t_env, t_path);
#endif
}

auto optimizer() {
#ifdef TEST_GUROBI_IMPORT
    return Gurobi();
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
