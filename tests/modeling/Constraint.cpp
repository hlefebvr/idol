//
// Created by henri on 05/09/22.
//

#include <iostream>
#include "../test_utils.h"

TEMPLATE_LIST_TEST_CASE("Constraint", "[constraints][modeling]", for_each_player) {

    Env env;
    Model<TestType::PlayerT> model(env);
    auto x = model.add_variable(0., Inf, Continuous, "x");
    auto y = model.add_variable(0., Inf, Continuous, "y");
    auto xi = model.add_parameter(0., Inf, Continuous, "xi");

    SECTION("should create new constraint") {

        SECTION("from [Variable<PlayerT>] [sign] [double]") {

            SECTION(">=") {
                auto ctr = model.add_constraint(x >= 3);
                CHECK(ctr.type() == GreaterOrEqual);
                CHECK(ctr.expr()[x].constant() == 1._a);
                CHECK(ctr.expr().constant().constant() == -3._a);
            }

            SECTION("<=") {
                auto ctr = model.add_constraint(x <= 3);
                CHECK(ctr.type() == LessOrEqual);
                CHECK(ctr.expr()[x].constant() == 1._a);
                CHECK(ctr.expr().constant().constant() == -3._a);
            }

            SECTION("==") {
                auto ctr = model.add_constraint(x == 3);
                CHECK(ctr.type() == Equal);
                CHECK(ctr.expr()[x].constant() == 1._a);
                CHECK(ctr.expr().constant().constant() == -3._a);
            }

        }

        SECTION("from [LinExpr<PlayerT>] [sign] [double]") {

            SECTION(">=") {
                auto ctr = model.add_constraint(x + y >= 3);
                CHECK(ctr.type() == GreaterOrEqual);
                CHECK(ctr.expr()[x].constant() == 1._a);
                CHECK(ctr.expr()[y].constant() == 1._a);
                CHECK(ctr.expr().constant().constant() == -3._a);
            }

            SECTION("<=") {
                auto ctr = model.add_constraint(x + y <= 3);
                CHECK(ctr.type() == LessOrEqual);
                CHECK(ctr.expr()[x].constant() == 1._a);
                CHECK(ctr.expr()[y].constant() == 1._a);
                CHECK(ctr.expr().constant().constant() == -3._a);
            }

            SECTION("==") {
                auto ctr = model.add_constraint(x + y == 3);
                CHECK(ctr.type() == Equal);
                CHECK(ctr.expr()[x].constant() == 1._a);
                CHECK(ctr.expr()[y].constant() == 1._a);
                CHECK(ctr.expr().constant().constant() == -3._a);
            }

        }

        SECTION("from [Expr<PlayerT>] [sign] [double]") {

            SECTION(">=") {
                auto ctr = model.add_constraint((1. + 2. * xi) * x + y >= 3);
                CHECK(ctr.type() == GreaterOrEqual);
                CHECK(ctr.expr()[x].constant() == 1._a);
                CHECK(ctr.expr()[x][xi] == 2._a);
                CHECK(ctr.expr()[y].constant() == 1._a);
                CHECK(ctr.expr().constant().constant() == -3._a);
            }

            SECTION("<=") {
                auto ctr = model.add_constraint((1. + 2. * xi) * x + y <= 3);
                CHECK(ctr.type() == LessOrEqual);
                CHECK(ctr.expr()[x].constant() == 1._a);
                CHECK(ctr.expr()[x][xi] == 2._a);
                CHECK(ctr.expr()[y].constant() == 1._a);
                CHECK(ctr.expr().constant().constant() == -3._a);
            }

            SECTION("==") {
                auto ctr = model.add_constraint((1. + 2. * xi) * x + y == 3);
                CHECK(ctr.type() == Equal);
                CHECK(ctr.expr()[x].constant() == 1._a);
                CHECK(ctr.expr()[x][xi] == 2._a);
                CHECK(ctr.expr()[y].constant() == 1._a);
                CHECK(ctr.expr().constant().constant() == -3._a);
            }

        }

        SECTION("should set name at birth") {

            SECTION("default name") {
                auto ctr = model.add_constraint(x <= 1.);
                CHECK(ctr.name() == "Ctr(" + std::to_string(ctr.id()) + ")");
                std::stringstream stream;
                stream << ctr;
                CHECK(stream.str() == ctr.name() + " : -1 + 1 x <= 0");
            }

            SECTION("given name") {
                auto ctr = model.add_constraint(x <= 1., "ctr");
                CHECK(ctr.name() == "ctr");
                std::stringstream stream;
                stream << ctr;
                CHECK(stream.str() == ctr.name() + " : -1 + 1 x <= 0");
            }

        }

    }

    SECTION("should update existing constraint") {

        auto ctr = model.add_constraint(x <= 1.);

        SECTION("should update constant") {

            SECTION("with double") {
                model.update(ctr, -2.);
                CHECK(ctr.expr().constant().constant() == -2._a);
            }

            SECTION("with parameter") {
                model.update(ctr, xi, 1.);
                CHECK(ctr.expr().constant()[xi] == 1._a);
            }

            SECTION("with exact expression") {
                model.update(ctr, -2. + xi);
                CHECK(ctr.expr().constant().constant() == -2._a);
                CHECK(ctr.expr().constant()[xi] == 1._a);
            }

        }

        SECTION("should update coefficient") {

            SECTION("with double") {
                model.update(ctr, x, -2.);
                CHECK(ctr.expr()[x].constant() == -2._a);
            }

            SECTION("with parameter") {
                model.update(ctr, x, xi, 1.);
                CHECK(ctr.expr()[x][xi] == 1._a);
            }

            SECTION("with exact expression") {
                model.update(ctr, x, -2. + xi);
                CHECK(ctr.expr()[x].constant() == -2._a);
                CHECK(ctr.expr()[x][xi] == 1._a);
            }

        }

    }

}