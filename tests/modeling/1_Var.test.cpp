//
// Created by henri on 08/09/22.
//

#include "../test_utils.h"

TEST_CASE("01. Var", "[variables][modeling]") {

    Model sp;
    Model model;

    auto xi = Param(sp.add_var(0., 1., Continuous, 0.) );
    auto c1 = model.add_ctr(GreaterOrEqual, 0.);
    auto c2 = model.add_ctr(GreaterOrEqual, 0.);

    SECTION("create a new variable with double objective coefficient") {

        auto x = model.add_var(0., 1., Continuous, 1.);

        CHECK(model.get_column(x).objective_coefficient().numerical() == 1.);
        CHECK(model.get_lb(x) == 0._a);
        CHECK(model.get_ub(x) == 1._a);
        CHECK(model.get_type(x) == Continuous);

    }

    SECTION("create a new variable with Param objective coefficient") {

        auto x = model.add_var(0., 1., Continuous, xi);

        CHECK(model.get_column(x).objective_coefficient().numerical() == 0.);
        CHECK(model.get_column(x).objective_coefficient().get(xi) == 1.);

    }

    SECTION("create a new variable with Coefficient objective coefficient") {

        auto x = model.add_var(0., 1., Continuous, 1 + 2 * xi);

        CHECK(model.get_column(x).objective_coefficient().numerical() == 1.);
        CHECK(model.get_column(x).objective_coefficient().get(xi) == 2.);

    }

    SECTION("create a new variable with Column") {

        Column column(xi);
        column.components().linear().set(c1, 1 + 2 * xi);
        column.components().linear().set(c2, 3);

        auto x = model.add_var(0., 1., Continuous, column);

        CHECK(model.get_column(x).objective_coefficient().numerical() == 0._a);
        CHECK(model.get_column(x).objective_coefficient().get(xi) == 1._a);
        CHECK(model.get_column(x).components().linear().get(c1).numerical() == 1._a);
        CHECK(model.get_column(x).components().linear().get(c1).get(xi) == 2._a);
        CHECK(model.get_column(x).components().linear().get(c2).numerical() == 3._a);
        CHECK(model.get_row(c1).lhs().linear().get(x).numerical() == 1._a);
        CHECK(model.get_row(c1).lhs().linear().get(x).get(xi) == 2._a);
        CHECK(model.get_row(c2).lhs().linear().get(x).numerical() == 3._a);

        column.components().linear().set(c1, 0.);

        CHECK(model.get_column(x).components().linear().get(c1).numerical() == 1._a);

    }

    SECTION("create a new variable with type Integer") {

        auto x = model.add_var(0., 1., Integer, 0);
        CHECK(model.get_type(x) == Integer);

    }

    SECTION("create a new variable with type Binary") {

        auto x = model.add_var(0., 1., Binary, 0);
        CHECK(model.get_type(x) == Binary);

    }

    SECTION("update an existing variable") {

        auto x = model.add_var(0., 1., Continuous, 0.);

        SECTION("update lb") {
            model.update_var_lb(x, -1.);
            CHECK(model.get_lb(x) == -1._a);
        }

        SECTION("update ub") {
            model.update_var_ub(x, 3.);
            CHECK(model.get_ub(x) == 3._a);
        }

        SECTION("update type") {

            SECTION("Continuous") {
                model.update_var_type(x, Continuous);
                CHECK(model.get_type(x) == Continuous);
            }

            SECTION("Integer") {
                model.update_var_type(x, Integer);
                CHECK(model.get_type(x) == Integer);
            }

            SECTION("Binary") {
                model.update_var_type(x, Binary);
                CHECK(model.get_type(x) == Binary);
            }
        }


    }

    SECTION("cout") {

        std::stringstream stream;

        SECTION("with a given name") {
            auto x = model.add_var(0., 1., Continuous, 0., "x");
            stream << x;
            CHECK(stream.str() == "x");
        }

        SECTION("without any given name") {
            auto x = model.add_var(0., 1., Continuous, 0.);
            stream << x;
            CHECK(stream.str() == "Var__" + std::to_string(x.id()));
        }

    }

}