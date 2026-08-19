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
        model.set_obj_expr(2 * x[0] - 3 * x[1]);
        CHECK(model.get_var_obj(x[0]) == 2.);
        CHECK(model.get_var_obj(x[1]) == -3.);
        CHECK(model.get_obj_expr().affine().constant() == 0.);
    }

    SECTION("Can set an affine objective function") {
        model.set_obj_expr(2 * x[0] - 3 * x[1] + 4.);
        CHECK(model.get_var_obj(x[0]) == 2.);
        CHECK(model.get_obj_expr().affine().constant() == 4.);
    }

    SECTION("Can set a quadratic objective function") {
        model.set_obj_expr(x[0] * x[0] + 2 * x[0] * x[1] + x[2] + 5.);
        CHECK(model.get_obj_expr().get(CommutativePair<Var>(x[0], x[0])) == 1.);
        CHECK(model.get_obj_expr().get(CommutativePair<Var>(x[0], x[1])) == 2.);
        CHECK(model.get_obj_expr().affine().linear().get(x[2]) == 1.);
        CHECK(model.get_obj_expr().affine().constant() == 5.);
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
        model.set_var_obj(x[1], 7.);
        CHECK(model.get_var_obj(x[1]) == 7.);
        CHECK(model.get_obj_expr().affine().linear().get(x[1]) == 7.);
    }

    SECTION("Can update the objective constant") {
        model.set_obj_expr(x[0] + 2.);
        model.set_obj_const(9.);
        CHECK(model.get_obj_expr().affine().constant() == 9.);
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
        const QuadExpr<Var> expected = x[0] * x[1] + 2 * x[2] + 3.;
        model.set_obj_expr(expected);
        CHECK(model.get_obj_expr().get(CommutativePair<Var>(x[0], x[1])) == 1.);
        CHECK(model.get_obj_expr().affine().linear().get(x[2]) == 2.);
        CHECK(model.get_obj_expr().affine().constant() == 3.);
    }

    SECTION("Can retrieve the objective coefficient of a variable") {
        model.set_obj_expr(4 * x[0] - x[2]);
        CHECK(model.get_var_obj(x[0]) == 4.);
        CHECK(model.get_var_obj(x[2]) == -1.);
    }

}
