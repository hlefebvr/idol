//
// Created by henri on 09/09/22.
//
#include "../test_utils.h"

TEST_CASE("05. Model", "[model][modeling]") {

    Model model;

    SECTION("remove a variable") {
        auto x = model.add_variable(0., 1., Continuous, 0.);
        CHECK(model.variables().size() == 1);
        model.remove(x);
        CHECK(model.variables().size() == 0);
    }

    SECTION("remove a constraint") {
        auto ctr = model.add_constraint(GreaterOrEqual, 0.);
        CHECK(model.constraints().size() == 1);
        model.remove(ctr);
        CHECK(model.constraints().size() == 0);
    }

    SECTION("iterate over variables") {

        model.add_variable(0., 1., Continuous, 0., "x");
        model.add_variable(0., 1., Continuous, 0., "y");

        bool x_has_been_found = false;
        bool y_has_been_found = false;

        for (const auto& var : model.variables()) {
            if (var.name() == "x") {
                x_has_been_found = true;
            } else if (var.name() == "y") {
                y_has_been_found = true;
            }
        }

        CHECK(x_has_been_found);
        CHECK(y_has_been_found);

    }

    SECTION("iterate over constraints") {

        model.add_constraint(GreaterOrEqual, 0., "c1");
        model.add_constraint(GreaterOrEqual, 0., "c2");

        bool c1_has_been_found = false;
        bool c2_has_been_found = false;

        for (const auto& ctr : model.constraints()) {
            if (ctr.name() == "c1") {
                c1_has_been_found = true;
            } else if (ctr.name() == "c2") {
                c2_has_been_found = true;
            }
        }

        CHECK(c1_has_been_found);
        CHECK(c2_has_been_found);

    }

    SECTION("update coefficient") {

        auto x = model.add_variable(0., 1., Continuous, 0.);
        auto y = model.add_variable(0., 1., Continuous, 0.);
        auto c1 = model.add_constraint(x + y >= 1);
        auto c2 = model.add_constraint(x <= 1);

        SECTION("insert a new coefficient") {

            SECTION("with a non-zero coefficient") {

                model.update_matrix_coeff(c2, y, 2.);

                CHECK(c2.get(x).numerical() == 1._a);
                CHECK(c2.row().lhs().get(x).numerical() == 1._a);
                CHECK(c2.get(y).numerical() == 2._a);
                CHECK(c2.row().lhs().get(y).numerical() == 2._a);

                CHECK(x.get(c2).numerical() == 1._a);
                CHECK(x.column().components().get(c2).numerical() == 1._a);
                CHECK(y.get(c2).numerical() == 2._a);
                CHECK(y.column().components().get(c2).numerical() == 2._a);

                CHECK(c2.row().lhs().size() == 2);
                CHECK(y.column().components().size() == 2);

            }

            SECTION("with a non-zero coefficient") {

                model.update_matrix_coeff(c2, y, 0.);

                CHECK(c2.get(x).numerical() == 1._a);
                CHECK(c2.row().lhs().get(x).numerical() == 1._a);
                CHECK(c2.get(y).numerical() == 0._a);
                CHECK(c2.row().lhs().get(y).numerical() == 0._a);

                CHECK(x.get(c2).numerical() == 1._a);
                CHECK(x.column().components().get(c2).numerical() == 1._a);
                CHECK(y.get(c2).numerical() == 0._a);
                CHECK(y.column().components().get(c2).numerical() == 0._a);

                CHECK(c2.row().lhs().size() == 1);
                CHECK(y.column().components().size() == 1);

            }

        }

        SECTION("update a new coefficient") {

            SECTION("with a non-zero coefficient") {

                model.update_matrix_coeff(c2, x, 2.);

                CHECK(c2.get(x).numerical() == 2._a);
                CHECK(c2.row().lhs().get(x).numerical() == 2._a);
                CHECK(c2.get(y).numerical() == 0._a);
                CHECK(c2.row().lhs().get(y).numerical() == 0._a);

                CHECK(x.get(c2).numerical() == 2._a);
                CHECK(x.column().components().get(c2).numerical() == 2._a);
                CHECK(y.get(c2).numerical() == 0._a);
                CHECK(y.column().components().get(c2).numerical() == 0._a);

                CHECK(c2.row().lhs().size() == 1);
                CHECK(x.column().components().size() == 2);

            }

            SECTION("with a non-zero coefficient") {

                model.update_matrix_coeff(c2, x, 0.);

                CHECK(c2.get(x).numerical() == 0._a);
                CHECK(c2.row().lhs().get(x).numerical() == 0._a);
                CHECK(c2.get(y).numerical() == 0._a);
                CHECK(c2.row().lhs().get(y).numerical() == 0._a);

                CHECK(x.get(c2).numerical() == 0._a);
                CHECK(x.column().components().get(c2).numerical() == 0._a);
                CHECK(y.get(c2).numerical() == 0._a);
                CHECK(y.column().components().get(c2).numerical() == 0._a);

                CHECK(c2.row().lhs().size() == 0);
                CHECK(x.column().components().size() == 1);

            }

        }

    }

    SECTION("create objective") {

        SECTION("initial empty objective") {

            auto x = model.add_variable(0., 1., Continuous, 0.);
            auto y = model.add_variable(0., 1., Continuous, 0.);

            CHECK(model.obj().linear().size() == 0);
            CHECK(x.column().objective_coefficient().numerical() == 0._a);
            CHECK(y.column().objective_coefficient().numerical() == 0._a);

        }

        SECTION("initial existing objective") {

            auto x = model.add_variable(0., 1., Continuous, 1.);
            auto y = model.add_variable(0., 1., Continuous, 2.);

            CHECK(model.obj().linear().size() == 2);
            CHECK(model.obj().linear().get(x).numerical() == 1._a);
            CHECK(model.obj().linear().get(y).numerical() == 2._a);
            CHECK(x.column().objective_coefficient().numerical() == 1._a);
            CHECK(y.column().objective_coefficient().numerical() == 2._a);

        }

    }

    SECTION("update objective") {

        SECTION("update from 0 to full entries") {

            auto x = model.add_variable(0., 1., Continuous, 0.);
            auto y = model.add_variable(0., 1., Continuous, 0.);

            Expr<Var> obj = x + 2 * y;

            model.update_obj(obj);

            CHECK(model.obj().linear().size() == 2);
            CHECK(model.obj().linear().get(x).numerical() == 1._a);
            CHECK(model.obj().linear().get(y).numerical() == 2._a);
            CHECK(x.column().objective_coefficient().numerical() == 1._a);
            CHECK(y.column().objective_coefficient().numerical() == 2._a);

        }

        SECTION("update from 0 to partial entries") {

            auto x = model.add_variable(0., 1., Continuous, 0.);
            auto y = model.add_variable(0., 1., Continuous, 0.);

            Expr<Var> obj = 2 * y;

            model.update_obj(obj);

            CHECK(model.obj().linear().size() == 1);
            CHECK(model.obj().linear().get(x).numerical() == 0._a);
            CHECK(model.obj().linear().get(y).numerical() == 2._a);
            CHECK(x.column().objective_coefficient().numerical() == 0._a);
            CHECK(y.column().objective_coefficient().numerical() == 2._a);

        }

        SECTION("update from existing to 0.") {

            auto x = model.add_variable(0., 1., Continuous, -1.);
            auto y = model.add_variable(0., 1., Continuous, -1.);

            Expr<Var> obj;

            model.update_obj(obj);

            CHECK(model.obj().linear().size() == 0);
            CHECK(model.obj().linear().get(x).numerical() == 0._a);
            CHECK(model.obj().linear().get(y).numerical() == 0._a);
            CHECK(x.column().objective_coefficient().numerical() == 0._a);
            CHECK(y.column().objective_coefficient().numerical() == 0._a);

        }

        SECTION("update from existing to full entries") {

            auto x = model.add_variable(0., 1., Continuous, -1.);
            auto y = model.add_variable(0., 1., Continuous, -1.);

            Expr<Var> obj = x + 2 * y;

            model.update_obj(obj);

            CHECK(model.obj().linear().size() == 2);
            CHECK(model.obj().linear().get(x).numerical() == 1._a);
            CHECK(model.obj().linear().get(y).numerical() == 2._a);
            CHECK(x.column().objective_coefficient().numerical() == 1._a);
            CHECK(y.column().objective_coefficient().numerical() == 2._a);

        }

        SECTION("update from existing to partial entries") {

            auto x = model.add_variable(0., 1., Continuous, -1.);
            auto y = model.add_variable(0., 1., Continuous, -1.);

            Expr<Var> obj = 2 * y;

            model.update_obj(obj);

            CHECK(model.obj().linear().size() == 1);
            CHECK(model.obj().linear().get(x).numerical() == 0._a);
            CHECK(model.obj().linear().get(y).numerical() == 2._a);
            CHECK(x.column().objective_coefficient().numerical() == 0._a);
            CHECK(y.column().objective_coefficient().numerical() == 2._a);

        }

    }

}