//
// Created by henri on 03.04.25.
//
#include <catch2/catch_all.hpp>
#include "idol/modeling.h"
#include "idol/mixed-integer/optimizers/wrappers/GLPK/GLPK.h"
#include "idol/mixed-integer/optimizers/wrappers/Mosek/Mosek.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"
#include "idol/mixed-integer/optimizers/wrappers/HiGHS/HiGHS.h"
#include "idol/mixed-integer/optimizers/wrappers/Osi/Osi.h"

using namespace Catch::literals;
using namespace idol;

TEST_CASE("Can add universal callbacks to monitor the execution of the algorithm", "[callbacks]") {

    SECTION("Can retrieve root node gap") {
        CHECK(false);
    }

    SECTION("Can retrieve root node solution") {
        CHECK(false);
    }

    SECTION("Can access nodes' solutions") {
        CHECK(false);
    }

    SECTION("Can access integer solutions") {
        CHECK(false);
    }

}

TEST_CASE("Can add universal callbacks to influence the execution of the algorithm", "[callbacks]") {

    SECTION("Can add user cut") {

        if (std::is_same_v<OPTIMIZER, Gurobi> || std::is_same_v<OPTIMIZER, GLPK>) {
            CHECK(false);
        } else {
           SKIP("The solver does not implement user cut.");
        }

    }

    SECTION("Can add lazy constraint") {

        if (std::is_same_v<OPTIMIZER, Gurobi> || std::is_same_v<OPTIMIZER, GLPK>) {
            CHECK(false);
        } else {
            SKIP("The solver does not implement lazy cut.");
        }

    }

    SECTION("Can terminate the algorithm") {


        if (std::is_same_v<OPTIMIZER, Gurobi> || std::is_same_v<OPTIMIZER, GLPK>) {
            CHECK(false);
        } else {
            SKIP("The solver does not implement early termination by user.");
        }

    }

}
