//
// Created by henri on 31.03.25.
//
#include <catch2/catch_all.hpp>
#include "idol/modeling.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"

#define STRINGIFY(x) #x
#define TO_STRING(x) STRINGIFY(x)

#ifdef OPTIMIZER
#define TEST_CASE_WITH_OPTIMIZER(title) TEST_CASE(std::string(title) + ", " + TO_STRING(OPTIMIZER), "[variables]")
#else
#define TEST_CASE_WITH_OPTIMIZER(title) TEST_CASE(title, "[variables]")
#endif

using namespace Catch::literals;
using namespace idol;

TEST_CASE_WITH_OPTIMIZER("Can add variables to a model") {

    Env env;
    Model model(env);

#ifdef OPTIMIZER
    model.use(OPTIMIZER());
#endif

    SECTION("Can add a new variable with bounds and type") {

        const auto x = model.add_var(-1, 1, Integer, 10, "x");
        CHECK(model.vars().size() == 1);
        CHECK(model.get_var_lb(x) == -1_a);
        CHECK(model.get_var_ub(x) == 1_a);
        CHECK(model.get_var_obj(x) == 10_a);
        CHECK(model.get_obj_expr().affine().linear().get(x) == 10_a);
        CHECK(model.get_var_type(x) == Integer);

        const auto y = model.add_var(0, 10, Continuous, 2, "y");
        CHECK(model.vars().size() == 2);
        CHECK(model.get_var_lb(y) == 0_a);
        CHECK(model.get_var_ub(y) == 10_a);
        CHECK(model.get_var_obj(y) == 2_a);
        CHECK(model.get_obj_expr().affine().linear().get(y) == 2_a);
        CHECK(model.get_var_type(y) == Continuous);

        const auto z = model.add_var(-2, 2, Binary, 1, "z");
        CHECK(model.vars().size() == 3);
        CHECK(model.get_var_lb(z) == -2_a);
        CHECK(model.get_var_ub(z) == 2_a);
        CHECK(model.get_var_obj(z) == 1_a);
        CHECK(model.get_obj_expr().affine().linear().get(z) == 1_a);
        CHECK(model.get_var_type(z) == Binary);

    }

    SECTION("Can add a new variable with bounds, type and column") {

        const auto c1 = model.add_ctr(LinExpr<Var>(), Equal, 0, "c1");
        const auto c2 = model.add_ctr(LinExpr<Var>(), Equal, 0, "c2");

        LinExpr<Ctr> column;
        column.set(c1, 1);

        const auto x = model.add_var(-1, 1, Integer, 10, column, "x");
        CHECK(model.vars().size() == 1);
        CHECK(model.get_var_lb(x) == -1_a);
        CHECK(model.get_var_ub(x) == 1_a);
        CHECK(model.get_var_obj(x) == 10_a);
        CHECK(model.get_obj_expr().affine().linear().get(x) == 10_a);
        CHECK(model.get_mat_coeff(c1, x) == 1_a);
        CHECK(model.get_mat_coeff(c2, x) == 0_a);
        CHECK(model.get_var_column(x).get(c1) == 1_a);
        CHECK(model.get_var_column(x).get(c2) == 0_a);
        CHECK(model.get_ctr_row(c1).get(x) == 1_a);
        CHECK(model.get_ctr_row(c2).get(x) == 0_a);
        CHECK(model.get_var_type(x) == Integer);

        column.clear();
        column.set(c2, 2);

        const auto y = model.add_var(0, 10, Continuous, 2, column,"y");
        CHECK(model.vars().size() == 2);
        CHECK(model.get_var_lb(y) == 0_a);
        CHECK(model.get_var_ub(y) == 10_a);
        CHECK(model.get_var_obj(y) == 2_a);
        CHECK(model.get_obj_expr().affine().linear().get(y) == 2_a);
        CHECK(model.get_mat_coeff(c1, y) == 0_a);
        CHECK(model.get_mat_coeff(c2, y) == 2_a);
        CHECK(model.get_var_column(y).get(c1) == 0_a);
        CHECK(model.get_var_column(y).get(c2) == 2_a);
        CHECK(model.get_ctr_row(c1).get(y) == 0_a);
        CHECK(model.get_ctr_row(c2).get(y) == 2_a);
        CHECK(model.get_var_type(y) == Continuous);

        column.clear();

        const auto z = model.add_var(-2, 2, Binary, 1, column,"z");
        CHECK(model.vars().size() == 3);
        CHECK(model.get_var_lb(z) == -2_a);
        CHECK(model.get_var_ub(z) == 2_a);
        CHECK(model.get_var_obj(z) == 1_a);
        CHECK(model.get_obj_expr().affine().linear().get(z) == 1_a);
        CHECK(model.get_mat_coeff(c1, z) == 0_a);
        CHECK(model.get_mat_coeff(c2, z) == 0_a);
        CHECK(model.get_var_column(z).get(c1) == 0_a);
        CHECK(model.get_var_column(z).get(c2) == 0_a);
        CHECK(model.get_ctr_row(c1).get(z) == 0_a);
        CHECK(model.get_ctr_row(c2).get(z) == 0_a);
        CHECK(model.get_var_type(z) == Binary);

    }

    SECTION("Can add an existing variable") {

        const auto x = Var(env, -1, 1, Integer, 10, "x");
        const auto y = Var(env, 0, 10, Continuous, 2, "y");
        const auto z = Var(env, -2, 2, Binary, 1, "z");

        model.add(x);
        CHECK(model.vars().size() == 1);
        CHECK(model.get_var_lb(x) == -1_a);
        CHECK(model.get_var_ub(x) == 1_a);
        CHECK(model.get_var_obj(x) == 10_a);
        CHECK(model.get_obj_expr().affine().linear().get(x) == 10_a);
        CHECK(model.get_var_type(x) == Integer);

        model.add(y);
        CHECK(model.vars().size() == 2);
        CHECK(model.get_var_lb(y) == 0_a);
        CHECK(model.get_var_ub(y) == 10_a);
        CHECK(model.get_var_obj(y) == 2_a);
        CHECK(model.get_obj_expr().affine().linear().get(y) == 2_a);
        CHECK(model.get_var_type(y) == Continuous);

        model.add(z);
        CHECK(model.vars().size() == 3);
        CHECK(model.get_var_lb(z) == -2_a);
        CHECK(model.get_var_ub(z) == 2_a);
        CHECK(model.get_var_obj(z) == 1_a);
        CHECK(model.get_obj_expr().affine().linear().get(z) == 1_a);
        CHECK(model.get_var_type(z) == Binary);

    }

    SECTION("Can add an existing variable with new bounds and type") {

        const auto x = Var(env, -Inf, Inf, Continuous, 0, "x");
        const auto y = Var(env, -Inf, Inf, Continuous, 0, "y");
        const auto z = Var(env, -Inf, Inf, Continuous, 0, "z");

        model.add(x, TempVar(-1, 1, Integer, 10, LinExpr<Ctr>()));
        CHECK(model.vars().size() == 1);
        CHECK(model.get_var_lb(x) == -1_a);
        CHECK(model.get_var_ub(x) == 1_a);
        CHECK(model.get_var_obj(x) == 10_a);
        CHECK(model.get_obj_expr().affine().linear().get(x) == 10_a);
        CHECK(model.get_var_type(x) == Integer);

        model.add(y, TempVar(0, 10, Continuous, 2, LinExpr<Ctr>()));
        CHECK(model.vars().size() == 2);
        CHECK(model.get_var_lb(y) == 0_a);
        CHECK(model.get_var_ub(y) == 10_a);
        CHECK(model.get_var_obj(y) == 2_a);
        CHECK(model.get_obj_expr().affine().linear().get(y) == 2_a);
        CHECK(model.get_var_type(y) == Continuous);

        model.add(z, TempVar(-2, 2, Binary, 1, LinExpr<Ctr>()));
        CHECK(model.vars().size() == 3);
        CHECK(model.get_var_lb(z) == -2_a);
        CHECK(model.get_var_ub(z) == 2_a);
        CHECK(model.get_var_obj(z) == 1_a);
        CHECK(model.get_obj_expr().affine().linear().get(z) == 1_a);
        CHECK(model.get_var_type(z) == Binary);

    }

    SECTION("Can add an existing variable with new bounds, type and column") {

        const auto c1 = model.add_ctr(LinExpr<Var>(), Equal, 0, "c1");
        const auto c2 = model.add_ctr(LinExpr<Var>(), Equal, 0, "c2");

        const auto x = Var(env, -Inf, Inf, Continuous, 0, "x");
        const auto y = Var(env, -Inf, Inf, Continuous, 0, "y");
        const auto z = Var(env, -Inf, Inf, Continuous, 0, "z");

        LinExpr<Ctr> column;
        column.set(c1, 1);

        model.add(x, TempVar(-1, 1, Integer, 10, std::move(column)));
        CHECK(model.vars().size() == 1);
        CHECK(model.get_var_lb(x) == -1_a);
        CHECK(model.get_var_ub(x) == 1_a);
        CHECK(model.get_var_obj(x) == 10_a);
        CHECK(model.get_obj_expr().affine().linear().get(x) == 10_a);
        CHECK(model.get_mat_coeff(c1, x) == 1_a);
        CHECK(model.get_mat_coeff(c2, x) == 0_a);
        CHECK(model.get_var_column(x).get(c1) == 1_a);
        CHECK(model.get_var_column(x).get(c2) == 0_a);
        CHECK(model.get_ctr_row(c1).get(x) == 1_a);
        CHECK(model.get_ctr_row(c2).get(x) == 0_a);
        CHECK(model.get_var_type(x) == Integer);

        column = LinExpr<Ctr>();
        column.set(c2, 2);

        model.add(y, TempVar(0, 10, Continuous, 2, std::move(column)));
        CHECK(model.vars().size() == 2);
        CHECK(model.get_var_lb(y) == 0_a);
        CHECK(model.get_var_ub(y) == 10_a);
        CHECK(model.get_var_obj(y) == 2_a);
        CHECK(model.get_obj_expr().affine().linear().get(y) == 2_a);
        CHECK(model.get_mat_coeff(c1, y) == 0_a);
        CHECK(model.get_mat_coeff(c2, y) == 2_a);
        CHECK(model.get_var_column(y).get(c1) == 0_a);
        CHECK(model.get_var_column(y).get(c2) == 2_a);
        CHECK(model.get_ctr_row(c1).get(y) == 0_a);
        CHECK(model.get_ctr_row(c2).get(y) == 2_a);
        CHECK(model.get_var_type(y) == Continuous);

        column = LinExpr<Ctr>();

        model.add(z, TempVar(-2, 2, Binary, 1, std::move(column)));
        CHECK(model.vars().size() == 3);
        CHECK(model.get_var_lb(z) == -2_a);
        CHECK(model.get_var_ub(z) == 2_a);
        CHECK(model.get_var_obj(z) == 1_a);
        CHECK(model.get_obj_expr().affine().linear().get(z) == 1_a);
        CHECK(model.get_mat_coeff(c1, z) == 0_a);
        CHECK(model.get_mat_coeff(c2, z) == 0_a);
        CHECK(model.get_var_column(z).get(c1) == 0_a);
        CHECK(model.get_var_column(z).get(c2) == 0_a);
        CHECK(model.get_ctr_row(c1).get(z) == 0_a);
        CHECK(model.get_ctr_row(c2).get(z) == 0_a);
        CHECK(model.get_var_type(z) == Binary);

    }

    SECTION("Can add a multiple new variables with bounds and type") {

        const auto x = model.add_vars(Dim<1>(5), -1, 1, Integer, 10, "x");
        CHECK(model.vars().size() == 5);
        for (const auto& x_i : x) {
            CHECK(model.get_var_lb(x_i) == -1_a);
            CHECK(model.get_var_ub(x_i) == 1_a);
            CHECK(model.get_var_obj(x_i) == 10_a);
            CHECK(model.get_obj_expr().affine().linear().get(x_i) == 10_a);
            CHECK(model.get_var_type(x_i) == Integer);
        }

        const auto y = model.add_vars(Dim<1>(10), 0, 10, Continuous, 2, "y");
        CHECK(model.vars().size() == 15);
        for (const auto& y_i : y) {
            CHECK(model.get_var_lb(y_i) == 0_a);
            CHECK(model.get_var_ub(y_i) == 10_a);
            CHECK(model.get_var_obj(y_i) == 2_a);
            CHECK(model.get_obj_expr().affine().linear().get(y_i) == 2_a);
            CHECK(model.get_var_type(y_i) == Continuous);
        }

        const auto z = model.add_vars(Dim<1>(15), -2, 2, Binary, 1, "z");
        CHECK(model.vars().size() == 30);
        for (const auto& z_i : z) {
            CHECK(model.get_var_lb(z_i) == -2_a);
            CHECK(model.get_var_ub(z_i) == 2_a);
            CHECK(model.get_var_obj(z_i) == 1_a);
            CHECK(model.get_obj_expr().affine().linear().get(z_i) == 1_a);
            CHECK(model.get_var_type(z_i) == Binary);
        }

    }

    SECTION("Can add multiple existing variables") {

        const auto x = Var::make_vector(env, Dim<1>(5), -1, 1, Integer, 10, "x");
        const auto y = Var::make_vector(env, Dim<1>(10), 0, 10, Continuous, 2, "y");
        const auto z = Var::make_vector(env, Dim<1>(15), -2, 2, Binary, 1, "z");

        model.add_vector<Var, 1>(x);
        CHECK(model.vars().size() == 5);
        for (const auto& x_i : x) {
            CHECK(model.get_var_lb(x_i) == -1_a);
            CHECK(model.get_var_ub(x_i) == 1_a);
            CHECK(model.get_var_obj(x_i) == 10_a);
            CHECK(model.get_obj_expr().affine().linear().get(x_i) == 10_a);
            CHECK(model.get_var_type(x_i) == Integer);
        }

        model.add_vector<Var, 1>(y);
        CHECK(model.vars().size() == 15);
        for (const auto& y_i : y) {
            CHECK(model.get_var_lb(y_i) == 0_a);
            CHECK(model.get_var_ub(y_i) == 10_a);
            CHECK(model.get_var_obj(y_i) == 2_a);
            CHECK(model.get_obj_expr().affine().linear().get(y_i) == 2_a);
            CHECK(model.get_var_type(y_i) == Continuous);
        }

        model.add_vector<Var, 1>(z);
        CHECK(model.vars().size() == 30);
        for (const auto& z_i : z) {
            CHECK(model.get_var_lb(z_i) == -2_a);
            CHECK(model.get_var_ub(z_i) == 2_a);
            CHECK(model.get_var_obj(z_i) == 1_a);
            CHECK(model.get_obj_expr().affine().linear().get(z_i) == 1_a);
            CHECK(model.get_var_type(z_i) == Binary);
        }

    }

    SECTION("Throws an exception if a variable is added twice") {
        const auto x = model.add_var(0, 1, Binary, Continuous, "x");
        CHECK_THROWS_AS(model.add(x), Exception);
    }

}

TEST_CASE_WITH_OPTIMIZER("Can update variables in a model") {

    Env env;
    Model model(env);

#ifdef OPTIMIZER
    model.use(OPTIMIZER());
#endif

    const auto x = model.add_var(-Inf, Inf, Continuous, 0, "x");

    SECTION("Can update a variable's bound") {
        model.set_var_lb(x, -1);
        CHECK(model.get_var_lb(x) == -1_a);
        model.set_var_ub(x, 1);
        CHECK(model.get_var_ub(x) == 1_a);
    }

    SECTION("Can update a variable's type") {
        model.set_var_type(x, Integer);
        CHECK(model.get_var_type(x) == Integer);
        model.set_var_type(x, Binary);
        CHECK(model.get_var_type(x) == Binary);
        model.set_var_type(x, Continuous);
        CHECK(model.get_var_type(x) == Continuous);
    }

    SECTION("Can update a variable's column") {
        const auto c1 = model.add_ctr(LinExpr<Var>(), Equal, 0, "c1");
        const auto c2 = model.add_ctr(LinExpr<Var>(), Equal, 0, "c2");

        LinExpr<Ctr> column;
        column.set(c1, 1);

        model.set_var_column(x, std::move(column));
        CHECK(model.get_var_column(x).get(c1) == 1_a);
        CHECK(model.get_var_column(x).get(c2) == 0_a);
        CHECK(model.get_ctr_row(c1).get(x) == 1_a);
        CHECK(model.get_ctr_row(c2).get(x) == 0_a);
        CHECK(model.get_mat_coeff(c1, x) == 1_a);
        CHECK(model.get_mat_coeff(c2, x) == 0_a);

        column.clear();
        column.set(c2, 2);

        model.set_var_column(x, std::move(column));
        CHECK(model.get_var_column(x).get(c1) == 0_a);
        CHECK(model.get_var_column(x).get(c2) == 2_a);
        CHECK(model.get_ctr_row(c1).get(x) == 0_a);
        CHECK(model.get_ctr_row(c2).get(x) == 2_a);
        CHECK(model.get_mat_coeff(c1, x) == 0_a);
        CHECK(model.get_mat_coeff(c2, x) == 2_a);
    }

    SECTION("Can update a variable's objective coefficient") {
        model.set_var_obj(x, 1);
        CHECK(model.get_var_obj(x) == 1_a);
        CHECK(model.get_obj_expr().affine().linear().get(x) == 1_a);
    }

    SECTION("Throws an exception if trying to update a variable that is not in the model") {
        const auto y = Var(env, -Inf, Inf, Continuous, 0, "y");
        CHECK_THROWS_AS(model.set_var_lb(y, -1), Exception);
        CHECK_THROWS_AS(model.set_var_ub(y, 1), Exception);
        CHECK_THROWS_AS(model.set_var_type(y, Integer), Exception);
        CHECK_THROWS_AS(model.set_var_obj(y, 1), Exception);
    }


}

TEST_CASE_WITH_OPTIMIZER("Can remove a variable from a model") {

    Env env;
    Model model(env);

#ifdef OPTIMIZER
    model.use(OPTIMIZER());
#endif

    SECTION("Can remove a variable") {
        const auto x = model.add_var(-Inf, Inf, Continuous, 0, "x");
        CHECK(model.vars().size() == 1);
        CHECK(model.has(x));
        model.remove(x);
        CHECK(model.vars().size() == 0);
        CHECK(!model.has(x));
    }

    SECTION("Throws an exception if trying to remove a variable that is not in the model") {
        const auto x = Var(env, -Inf, Inf, Continuous, 0, "x");
        CHECK_THROWS_AS(model.remove(x), Exception);
    }

}

TEST_CASE_WITH_OPTIMIZER("Can access a variable in a model") {

    Env env;
    Model model(env);

#ifdef OPTIMIZER
    model.use(OPTIMIZER());
#endif

    const auto c1 = model.add_ctr(LinExpr<Var>(), Equal, 0, "c1");
    const auto c2 = model.add_ctr(LinExpr<Var>(), Equal, 0, "c2");

    LinExpr<Ctr> column;
    column.set(c1, 1);
    column.set(c2, 2);

    const auto x = model.add_var(-1, 1, Integer, 10, std::move(column),"x");
    const auto y = model.add_var(0, 10, Continuous, 2, "y");
    const auto z = model.add_var(-2, 2, Binary, 1, "z");


    SECTION("Can retrieve a variable's bound") {
        CHECK(model.get_var_lb(x) == -1_a);
        CHECK(model.get_var_ub(x) == 1_a);
    }

    SECTION("Can retrieve a variable's type") {
        CHECK(model.get_var_type(x) == Integer);
    }

    SECTION("Can retrieve a variable's column") {
        CHECK(model.get_var_column(x).get(c1) == 1_a);
        CHECK(model.get_var_column(x).get(c2) == 2_a);
    }

    SECTION("Can retrieve a variable's objective coefficient") {
        CHECK(model.get_var_obj(x) == 10_a);
        CHECK(model.get_obj_expr().affine().linear().get(x) == 10_a);
    }

    SECTION("Throws an exception if trying to retrieve info of a variable which is not in the model") {
        const auto unknown = Var(env, -Inf, Inf, Continuous, 0, "unknown");
        CHECK_THROWS_AS(model.get_var_lb(unknown), Exception);
        CHECK_THROWS_AS(model.get_var_ub(unknown), Exception);
        CHECK_THROWS_AS(model.get_var_type(unknown), Exception);
        CHECK_THROWS_AS(model.get_var_obj(unknown), Exception);
        CHECK_THROWS_AS(model.get_var_column(unknown), Exception);
    }

    SECTION("Can iterate over variables in a model") {

        std::vector<Var> vars;
        for (const auto& var : model.vars()) {
            vars.push_back(var);
        }

        CHECK(vars.size() == 3);
        CHECK(vars[0].id() == x.id());
        CHECK(vars[1].id() == y.id());
        CHECK(vars[2].id() == z.id());

    }

    SECTION("Can retrieve a variable's index") {
        CHECK(model.get_var_index(x) == 0);
        CHECK(model.get_var_index(y) == 1);
        CHECK(model.get_var_index(z) == 2);
    }

    SECTION("Can retrieve a variable by index") {
        CHECK(model.get_var_by_index(0).id() == x.id());
        CHECK(model.get_var_by_index(1).id() == y.id());
        CHECK(model.get_var_by_index(2).id() == z.id());
    }

}