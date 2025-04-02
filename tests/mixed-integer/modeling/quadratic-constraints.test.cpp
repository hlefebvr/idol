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
#define TEST_CASE_WITH_OPTIMIZER(title) TEST_CASE(title, "[quadratic-constraints]")
#endif

using namespace Catch::literals;
using namespace idol;

TEST_CASE_WITH_OPTIMIZER("Can add a quadratic constraint to a model") {

    Env env;
    Model model(env);
    const auto x = model.add_vars(Dim<1>(3), -Inf, Inf, Continuous, 0, "x");

#ifdef OPTIMIZER
    model.use(OPTIMIZER());
    model.update();
#endif

    SECTION("Can add a new quadratic constraint with type and expression") {
        CHECK(false);
    }

    SECTION("Can add an existing quadratic constraint") {
        CHECK(false);
    }

    SECTION("Can add an existing quadratic constraint with new type and expression") {
        CHECK(false);
    }

    SECTION("Can add multiple new quadratic constraints with type and right-hand side") {
        CHECK(false);
    }

    SECTION("Can add multiple existing quadratic constraints with type and right-hand side") {
        CHECK(false);
    }

}

TEST_CASE_WITH_OPTIMIZER("Can remove a quadratic constraint from a model") {

    Env env;
    Model model(env);
    const auto x = model.add_vars(Dim<1>(3), -Inf, Inf, Continuous, 0, "x");

#ifdef OPTIMIZER
    model.use(OPTIMIZER());
    model.update();
#endif

    SECTION("Can remove a quadratic constraint") {
        CHECK(false);
    }

    SECTION("Throws an exception if trying to remove a quadratic constraint which is not in the model") {

        const auto c1 = QCtr(env, TempQCtr(x[0] * x[0] + x[1] - 10, LessOrEqual), "c1");
        CHECK_THROWS_AS(model.remove(c1), Exception);

    }

}

TEST_CASE_WITH_OPTIMIZER("Can access a quadratic constraint in a model") {

    Env env;
    Model model(env);
    const auto x = model.add_vars(Dim<1>(3), -Inf, Inf, Continuous, 0, "x");

#ifdef OPTIMIZER
    model.use(OPTIMIZER());
    model.update();
#endif

    SECTION("Can retrieve a quadratic constraint's type") {
        CHECK(false);
    }

    SECTION("Can retrieve a quadratic constraint's expression") {
        CHECK(false);
    }

    SECTION("Can retrieve a quadratic constraint's index") {
        CHECK(false);
    }

    SECTION("Can retrieve a quadratic constraint by index") {
        CHECK(false);
    }

    SECTION("Can iterate over quadratic constraints in a model") {
        CHECK(false);
    }

}
