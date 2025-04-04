//
// Created by henri on 04.04.25.
//

#include <catch2/catch_all.hpp>
#include "idol/modeling.h"
#include "idol/bilevel/optimizers/wrappers/MibS/MibS.h"

using namespace Catch::literals;
using namespace idol;

TEST_CASE("Can solve a feasible bilevel problem", "[solving-bilevel]") {

    SECTION("Can solve a bilevel problem with continuous lower level") {
        CHECK(false);
    }

    SECTION("Can solve a bilevel problem with mixed-integer lower level") {
        CHECK(false);
    }

    SECTION("Can solve a bilevel problem with no lower level problem") {
        CHECK(false);
    }

    SECTION("Throws an exception if some linking variables are continuous") {
        CHECK(false);
    }

}

TEST_CASE("Can update a problem and re-optimize", "[solving-bilevel]") {

    SECTION("Can update the objective function of the upper level and re-optimize") {
        CHECK(false);
    }

    SECTION("Can update the objective function of the lower level and re-optimize") {
        CHECK(false);
    }

    SECTION("Can update the right-hand side of the upper level and re-optimize") {
        CHECK(false);
    }

    SECTION("Can update the right-hand side of the lower level and re-optimize") {
        CHECK(false);
    }

    SECTION("Can update the left-hand side of the upper level and re-optimize") {
        CHECK(false);
    }

    SECTION("Can update the left-hand side of the lower level and re-optimize") {
        CHECK(false);
    }

    SECTION("Can add a cut to the upper level and re-optimize") {
        CHECK(false);
    }

    SECTION("Can add a cut to the lower level and re-optimize") {
        CHECK(false);
    }

    SECTION("Can add a column to the upper level and re-optimize") {
        CHECK(false);
    }

    SECTION("Can add a column to the lower level and re-optimize") {
        CHECK(false);
    }

    SECTION("Can change the bounds of the upper level and re-optimize") {
        CHECK(false);
    }

    SECTION("Can change the bounds of the lower level and re-optimize") {
        CHECK(false);
    }

    SECTION("Can remove a constraint from the upper level and re-optimize") {
        CHECK(false);
    }

    SECTION("Can remove a constraint from the lower level and re-optimize") {
        CHECK(false);
    }

    SECTION("Can remove a variable from the upper level and re-optimize") {
        CHECK(false);
    }

    SECTION("Can remove a variable from the lower level and re-optimize") {
        CHECK(false);
    }

    SECTION("Can add and remove the same variable and re-optimize") {
        CHECK(false);
    }

    SECTION("Can add and remove a different variable and re-optimize") {
        CHECK(false);
    }

    SECTION("Can add and remove the same constraint and re-optimize") {
        CHECK(false);
    }

    SECTION("Can add and remove a different constraint and re-optimize") {
        CHECK(false);
    }

    SECTION("Can add a constraint which depends on a variable that is then removed") {
        CHECK(false);
    }

    SECTION("Can add a variable which depends on a constraint that is then removed") {
        CHECK(false);
    }

    SECTION("Can change the objective function then remove a variable which appears in the objective function") {
        CHECK(false);
    }

    SECTION("Can update the objective function constant") {
        CHECK(false);
    }

}

TEST_CASE("Can solve a bilevel problem with infeasible upper level", "[solving-bilevel]") {

    SECTION("Can retrieve the solution status") {
        CHECK(false);
    }

}

TEST_CASE("Can solve a bilevel problem with infeasible lower level", "[solving-bilevel]") {

    SECTION("Can retrieve the solution status") {
        CHECK(false);
    }

}

TEST_CASE("Can solve an infeasible bilevel problem with feasible upper level and lower level", "[solving-bilevel]") {

    SECTION("Can retrieve the solution status") {
        CHECK(false);
    }

}
