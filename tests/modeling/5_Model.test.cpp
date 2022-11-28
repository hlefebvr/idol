//
// Created by henri on 09/09/22.
//
#include "../test_utils.h"

TEST_CASE("05. Model", "[model][modeling]") {

    Model model;

    SECTION("remove a variable") {
        auto x = model.add_var(0., 1., Continuous, 0.);
        CHECK(model.vars().size() == 1);
        model.remove(x);
        CHECK(model.vars().size() == 0);
    }

    SECTION("remove a constraint") {
        auto ctr = model.add_ctr(GreaterOrEqual, 0.);
        CHECK(model.ctrs().size() == 1);
        model.remove(ctr);
        CHECK(model.ctrs().size() == 0);
    }

    SECTION("iterate over variables") {

        model.add_var(0., 1., Continuous, 0., "x");
        model.add_var(0., 1., Continuous, 0., "y");

        bool x_has_been_found = false;
        bool y_has_been_found = false;

        for (const auto& var : model.vars()) {
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

        model.add_ctr(GreaterOrEqual, 0., "c1");
        model.add_ctr(GreaterOrEqual, 0., "c2");

        bool c1_has_been_found = false;
        bool c2_has_been_found = false;

        for (const auto& ctr : model.ctrs()) {
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

        auto x = model.add_var(0., 1., Continuous, 0.);
        auto y = model.add_var(0., 1., Continuous, 0.);
        auto c1 = model.add_ctr(x + y >= 1);
        auto c2 = model.add_ctr(x <= 1);

        SECTION("insert a new coefficient") {

            SECTION("with a non-zero coefficient") {

                model.set(Attr::Matrix::Coeff, c2, y, 2.);
                
                CHECK(model.get(Attr::Ctr::Row, c2).linear().get(x).numerical() == 1._a);
                CHECK(model.get(Attr::Ctr::Row, c2).linear().get(y).numerical() == 2._a);

                CHECK(model.get(Attr::Var::Column, x).linear().get(c2).numerical() == 1._a);
                CHECK(model.get(Attr::Var::Column, x).linear().get(c2).numerical() == 1._a);
                CHECK(model.get(Attr::Var::Column, y).linear().get(c2).numerical() == 2._a);
                CHECK(model.get(Attr::Var::Column, y).linear().get(c2).numerical() == 2._a);

                CHECK(model.get(Attr::Ctr::Row, c2).linear().size() == 2);
                CHECK(model.get(Attr::Var::Column, y).linear().size() == 2);

            }

            SECTION("with a non-zero coefficient") {

                model.set(Attr::Matrix::Coeff, c2, y, 0.);

                CHECK(model.get(Attr::Ctr::Row, c2).linear().get(x).numerical() == 1._a);
                CHECK(model.get(Attr::Ctr::Row, c2).linear().get(y).numerical() == 0._a);

                CHECK(model.get(Attr::Var::Column, x).linear().get(c2).numerical() == 1._a);
                CHECK(model.get(Attr::Var::Column, x).linear().get(c2).numerical() == 1._a);
                CHECK(model.get(Attr::Var::Column, y).linear().get(c2).numerical() == 0._a);
                CHECK(model.get(Attr::Var::Column, y).linear().get(c2).numerical() == 0._a);

                CHECK(model.get(Attr::Ctr::Row, c2).linear().size() == 1);
                CHECK(model.get(Attr::Var::Column, y).linear().size() == 1);

            }

        }

        SECTION("update a new coefficient") {

            SECTION("with a non-zero coefficient") {

                model.set(Attr::Matrix::Coeff, c2, x, 2.);

                CHECK(model.get(Attr::Ctr::Row, c2).linear().get(x).numerical() == 2._a);
                CHECK(model.get(Attr::Ctr::Row, c2).linear().get(y).numerical() == 0._a);

                CHECK(model.get(Attr::Var::Column, x).linear().get(c2).numerical() == 2._a);
                CHECK(model.get(Attr::Var::Column, x).linear().get(c2).numerical() == 2._a);
                CHECK(model.get(Attr::Var::Column, y).linear().get(c2).numerical() == 0._a);
                CHECK(model.get(Attr::Var::Column, y).linear().get(c2).numerical() == 0._a);

                CHECK(model.get(Attr::Ctr::Row, c2).linear().size() == 1);
                CHECK(model.get(Attr::Var::Column, x).linear().size() == 2);

            }

            SECTION("with a non-zero coefficient") {

                model.set(Attr::Matrix::Coeff, c2, x, 0.);

                CHECK(model.get(Attr::Ctr::Row, c2).linear().get(x).numerical() == 0._a);
                CHECK(model.get(Attr::Ctr::Row, c2).linear().get(y).numerical() == 0._a);

                CHECK(model.get(Attr::Var::Column, x).linear().get(c2).numerical() == 0._a);
                CHECK(model.get(Attr::Var::Column, x).linear().get(c2).numerical() == 0._a);
                CHECK(model.get(Attr::Var::Column, y).linear().get(c2).numerical() == 0._a);
                CHECK(model.get(Attr::Var::Column, y).linear().get(c2).numerical() == 0._a);

                CHECK(model.get(Attr::Ctr::Row, c2).linear().size() == 0);
                CHECK(model.get(Attr::Var::Column, x).linear().size() == 1);

            }

        }

    }

    SECTION("create objective") {

        SECTION("initial empty objective") {

            auto x = model.add_var(0., 1., Continuous, 0.);
            auto y = model.add_var(0., 1., Continuous, 0.);

            CHECK(model.get(Attr::Obj::Expr).linear().size() == 0);
            CHECK(model.get(Attr::Var::Column, x).obj().numerical() == 0._a);
            CHECK(model.get(Attr::Var::Column, y).obj().numerical() == 0._a);

        }

        SECTION("initial existing objective") {

            auto x = model.add_var(0., 1., Continuous, 1.);
            auto y = model.add_var(0., 1., Continuous, 2.);

            CHECK(model.get(Attr::Obj::Expr).linear().size() == 2);
            CHECK(model.get(Attr::Obj::Expr).linear().get(x).numerical() == 1._a);
            CHECK(model.get(Attr::Obj::Expr).linear().get(y).numerical() == 2._a);
            CHECK(model.get(Attr::Var::Column, x).obj().numerical() == 1._a);
            CHECK(model.get(Attr::Var::Column, y).obj().numerical() == 2._a);

        }

    }

    SECTION("update objective") {

        SECTION("update from 0 to full entries") {

            auto x = model.add_var(0., 1., Continuous, 0.);
            auto y = model.add_var(0., 1., Continuous, 0.);

            Expr<Var> obj = x + 2 * y;

            model.set(Attr::Obj::Expr, obj);

            CHECK(model.get(Attr::Obj::Expr).linear().size() == 2);
            CHECK(model.get(Attr::Obj::Expr).linear().get(x).numerical() == 1._a);
            CHECK(model.get(Attr::Obj::Expr).linear().get(y).numerical() == 2._a);
            CHECK(model.get(Attr::Var::Column, x).obj().numerical() == 1._a);
            CHECK(model.get(Attr::Var::Column, y).obj().numerical() == 2._a);

        }

        SECTION("update from 0 to partial entries") {

            auto x = model.add_var(0., 1., Continuous, 0.);
            auto y = model.add_var(0., 1., Continuous, 0.);

            Expr<Var> obj = 2 * y;

            model.set(Attr::Obj::Expr, obj);

            CHECK(model.get(Attr::Obj::Expr).linear().size() == 1);
            CHECK(model.get(Attr::Obj::Expr).linear().get(x).numerical() == 0._a);
            CHECK(model.get(Attr::Obj::Expr).linear().get(y).numerical() == 2._a);
            CHECK(model.get(Attr::Var::Column, x).obj().numerical() == 0._a);
            CHECK(model.get(Attr::Var::Column, y).obj().numerical() == 2._a);

        }

        SECTION("update from existing to 0.") {

            auto x = model.add_var(0., 1., Continuous, -1.);
            auto y = model.add_var(0., 1., Continuous, -1.);

            Expr<Var> obj;

            model.set(Attr::Obj::Expr, obj);

            CHECK(model.get(Attr::Obj::Expr).linear().size() == 0);
            CHECK(model.get(Attr::Obj::Expr).linear().get(x).numerical() == 0._a);
            CHECK(model.get(Attr::Obj::Expr).linear().get(y).numerical() == 0._a);
            CHECK(model.get(Attr::Var::Column, x).obj().numerical() == 0._a);
            CHECK(model.get(Attr::Var::Column, y).obj().numerical() == 0._a);

        }

        SECTION("update from existing to full entries") {

            auto x = model.add_var(0., 1., Continuous, -1.);
            auto y = model.add_var(0., 1., Continuous, -1.);

            Expr<Var> obj = x + 2 * y;

            model.set(Attr::Obj::Expr, obj);

            CHECK(model.get(Attr::Obj::Expr).linear().size() == 2);
            CHECK(model.get(Attr::Obj::Expr).linear().get(x).numerical() == 1._a);
            CHECK(model.get(Attr::Obj::Expr).linear().get(y).numerical() == 2._a);
            CHECK(model.get(Attr::Var::Column, x).obj().numerical() == 1._a);
            CHECK(model.get(Attr::Var::Column, y).obj().numerical() == 2._a);

        }

        SECTION("update from existing to partial entries") {

            auto x = model.add_var(0., 1., Continuous, -1.);
            auto y = model.add_var(0., 1., Continuous, -1.);

            Expr<Var> obj = 2 * y;

            model.set(Attr::Obj::Expr, obj);

            CHECK(model.get(Attr::Obj::Expr).linear().size() == 1);
            CHECK(model.get(Attr::Obj::Expr).linear().get(x).numerical() == 0._a);
            CHECK(model.get(Attr::Obj::Expr).linear().get(y).numerical() == 2._a);
            CHECK(model.get(Attr::Var::Column, x).obj().numerical() == 0._a);
            CHECK(model.get(Attr::Var::Column, y).obj().numerical() == 2._a);

        }

    }

}