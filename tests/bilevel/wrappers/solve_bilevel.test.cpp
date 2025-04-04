//
// Created by henri on 04.04.25.
//

#include <catch2/catch_all.hpp>
#include "idol/modeling.h"
#include "idol/bilevel/optimizers/wrappers/MibS/MibS.h"

using namespace Catch::literals;
using namespace idol;

TEST_CASE("Can solve a feasible bilevel problem", "[solving-bilevel]") {

    SECTION("Throws an exception if some linking variables are continuous") {
        CHECK(false);
    }

    SECTION("Can solve a bilevel problem with no lower level problem") {
        CHECK(false);
    }

    SECTION("Can solve a bilevel problem with continuous lower level") {
        CHECK(false);
    }

    SECTION("Can solve a bilevel problem with mixed-integer lower level") {
        CHECK(false);
    }

}