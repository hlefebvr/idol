//
// Created by henri on 31.03.25.
//
#include <catch2/catch_all.hpp>
#include "idol/modeling.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"

#define STRINGIFY(x) #x
#define TO_STRING(x) STRINGIFY(x)

#ifdef OPTIMIZER
#define TEST_CASE_WITH_OPTIMIZER(title) TEST_CASE(std::string(title) + ", " + TO_STRING(OPTIMIZER), "[quadratic-constraints]")
#else
#define TEST_CASE_WITH_OPTIMIZER(title) TEST_CASE(title, "[objective]")
#endif

using namespace Catch::literals;
using namespace idol;

TEST_CASE_WITH_OPTIMIZER("Can set the objective function of a model") {

    Env env;
    Model model(env);
    const auto x = model.add_vars(Dim<1>(3), -Inf, Inf, Continuous, 0, "x");

#ifdef OPTIMIZER
    model.use(OPTIMIZER());
    model.update();
#endif

    SECTION("Can set a linear objective function") {
        CHECK(false);
    }

    SECTION("Can set an affine objective function") {
        CHECK(false);
    }

    SECTION("Can set a quadratic objective function") {
        CHECK(false);
    }

    SECTION("Can set the objective sense") {
        CHECK(false);
    }

}

TEST_CASE_WITH_OPTIMIZER("Can update the objective function of a model") {

    Env env;
    Model model(env);
    const auto x = model.add_vars(Dim<1>(3), -Inf, Inf, Continuous, 0, "x");

#ifdef OPTIMIZER
    model.use(OPTIMIZER());
    model.update();
#endif

    SECTION("Can update the objective coefficient of a variable") {
        CHECK(false);
    }

    SECTION("Can update the objective constant") {
        CHECK(false);
    }

    SECTION("Can update the objective sense") {
        CHECK(false);
    }

}

TEST_CASE_WITH_OPTIMIZER("Can access the objective function of a model") {

    Env env;
    Model model(env);
    const auto x = model.add_vars(Dim<1>(3), -Inf, Inf, Continuous, 0, "x");

#ifdef OPTIMIZER
    model.use(OPTIMIZER());
    model.update();
#endif

    SECTION("Can retrieve the objective function") {
        CHECK(false);
    }

    SECTION("Can retrieve the objective coefficient of a variable") {
        CHECK(false);
    }

    SECTION("Can retrieve the objective sense") {
        CHECK(false);
    }

}
