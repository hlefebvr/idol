#include <catch2/catch_all.hpp>

#include "idol/modeling.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/DantzigWolfeDecomposition.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/SubProblem.h"
#include "idol/mixed-integer/optimizers/wrappers/GLPK/GLPK.h"

using namespace idol;

namespace {

Var variable_named(const Model& t_model, const std::string& t_name) {
    for (const auto& var : t_model.vars()) {
        if (var.name() == t_name) {
            return var;
        }
    }
    throw std::runtime_error("Variable not found.");
}

Ctr constraint_named(const Model& t_model, const std::string& t_name) {
    for (const auto& ctr : t_model.ctrs()) {
        if (ctr.name() == t_name) {
            return ctr;
        }
    }
    throw std::runtime_error("Constraint not found.");
}

}

TEST_CASE("Read a Striplib model and decomposition", "[striplib]") {
    Env env;
    auto [model, decomposition] = DantzigWolfe::read_from_striplib(
            env, "../../data/striplib/tiny.lp", "../../data/striplib/tiny.dec");

    REQUIRE(model.vars().size() == 4);
    REQUIRE(model.ctrs().size() == 3);
    CHECK(model.get_var_type(variable_named(model, "x_1")) == Binary);
    CHECK(model.get_var_lb(variable_named(model, "x_1")) == 0.);
    CHECK(model.get_var_ub(variable_named(model, "x_1")) == 1.);
    CHECK(model.get_var_obj(variable_named(model, "x_2")) == 2.);
    CHECK(model.get_ctr_type(constraint_named(model, "block_1")) == GreaterOrEqual);
    CHECK(model.get_ctr_rhs(constraint_named(model, "block_1")) == 1.);
    CHECK(model.get_mat_coeff(constraint_named(model, "block_1"), variable_named(model, "x_1")) == 1.);

    CHECK(constraint_named(model, "block_1").get(decomposition) == 0);
    CHECK(constraint_named(model, "block_2").get(decomposition) == 1);
    CHECK(constraint_named(model, "linking").get(decomposition) == MasterId);
    CHECK(variable_named(model, "x_1").get(decomposition) == 0);
    CHECK(variable_named(model, "x_2").get(decomposition) == 0);
    CHECK(variable_named(model, "x_3").get(decomposition) == 1);
    CHECK(variable_named(model, "x_4").get(decomposition) == 1);
}

TEST_CASE("Solve a small imported Striplib decomposition", "[striplib]") {
    Env env;
    auto [direct, decomposition] = DantzigWolfe::read_from_striplib(
            env, "../../data/striplib/tiny.lp", "../../data/striplib/tiny.dec");
    auto decomposed = direct.copy();

    direct.use(GLPK());
    direct.optimize();

    decomposed.use(DantzigWolfeDecomposition(decomposition)
                           .with_master_optimizer(GLPK::ContinuousRelaxation())
                           .with_default_sub_problem_spec(DantzigWolfe::SubProblem().add_optimizer(GLPK())));
    decomposed.optimize();

    REQUIRE(direct.get_status() == Optimal);
    REQUIRE(decomposed.get_status() == Optimal);
    CHECK(direct.get_best_obj() == Catch::Approx(2.));
    CHECK(decomposed.get_best_obj() == Catch::Approx(direct.get_best_obj()));
}

TEST_CASE("Empty trailing block is accepted with a warning", "[striplib]") {
    // HF3 regression: a DEC file declaring NBLOCKS=3 but only populating
    // blocks 1 and 2 must not throw. The third block has no variables or
    // constraints; its PricingSetupVar provides the convexity row.
    Env env;
    auto model = Model::read_from_file(env, "../../data/striplib/tiny.lp");
    CHECK_NOTHROW(DantzigWolfe::read_decomposition(model, "../../data/striplib/empty-trailing-block.dec"));
}

TEST_CASE("Reject unsupported or invalid Striplib decompositions", "[striplib]") {
    Env env;
    auto model = Model::read_from_file(env, "../../data/striplib/tiny.lp");

    SECTION("presolved decomposition") {
        CHECK_THROWS_WITH(DantzigWolfe::read_decomposition(model, "../../data/striplib/presolved.dec"),
                          Catch::Matchers::ContainsSubstring("PRESOLVED decompositions are not supported"));
    }
    SECTION("explicit linking variable") {
        CHECK_THROWS_WITH(DantzigWolfe::read_decomposition(model, "../../data/striplib/linking-variable.dec"),
                          Catch::Matchers::ContainsSubstring("Linking variable \"x_1\""));
    }
    SECTION("inferred linking variable") {
        CHECK_THROWS_WITH(DantzigWolfe::read_decomposition(model, "../../data/striplib/inferred-linking-variable.dec"),
                          Catch::Matchers::ContainsSubstring("occurs in multiple blocks"));
    }
    SECTION("unassigned constraint") {
        CHECK_THROWS_WITH(DantzigWolfe::read_decomposition(model, "../../data/striplib/unassigned.dec"),
                          Catch::Matchers::ContainsSubstring("is unassigned and CONSDEFAULTMASTER is 0"));
    }
    SECTION("unknown constraint") {
        CHECK_THROWS_WITH(DantzigWolfe::read_decomposition(model, "../../data/striplib/unknown-name.dec"),
                          Catch::Matchers::ContainsSubstring("Unknown constraint \"does_not_exist\""));
    }
}
