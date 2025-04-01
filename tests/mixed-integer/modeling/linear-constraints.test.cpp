//
// Created by henri on 31.03.25.
//
#include <catch2/catch_all.hpp>
#include "idol/modeling.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"

#define STRINGIFY(x) #x
#define TO_STRING(x) STRINGIFY(x)

#ifdef OPTIMIZER
#define TEST_CASE_WITH_OPTIMIZER(title) TEST_CASE(std::string(title) + ", " + TO_STRING(OPTIMIZER), "[constraints]")
#else
#define TEST_CASE_WITH_OPTIMIZER(title) TEST_CASE(title, "[constraints]")
#endif

using namespace Catch::literals;
using namespace idol;


TEST_CASE_WITH_OPTIMIZER("Can add a linear constraint to a model") {

    Env env;
    Model model(env);
    const auto x = model.add_vars(Dim<1>(3), -Inf, Inf, Continuous, 0, "x");

#ifdef OPTIMIZER
    model.use(OPTIMIZER());
    model.update();
#endif

    SECTION("Can add a new linear constraint with type and expression") {

        const auto c1 = model.add_ctr(x[0] + x[1] <= 10, "c1");

        CHECK(model.ctrs().size() == 1);
        CHECK(model.get_ctr_type(c1) == LessOrEqual);
        CHECK(model.get_ctr_rhs(c1) == 10_a);
        CHECK(model.get_ctr_row(c1).get(x[0]) == 1_a);
        CHECK(model.get_ctr_row(c1).get(x[1]) == 1_a);
        CHECK(model.get_ctr_row(c1).get(x[2]) == 0_a);
        CHECK(model.get_var_column(x[0]).get(c1) == 1_a);
        CHECK(model.get_var_column(x[1]).get(c1) == 1_a);
        CHECK(model.get_var_column(x[2]).get(c1) == 0_a);
        CHECK(model.get_mat_coeff(c1, x[0]) == 1_a);
        CHECK(model.get_mat_coeff(c1, x[1]) == 1_a);
        CHECK(model.get_mat_coeff(c1, x[2]) == 0_a);

        const auto c2 = model.add_ctr(x[0] + 2 * x[1] >= 5, "c2");
        CHECK(model.ctrs().size() == 2);
        CHECK(model.get_ctr_type(c2) == GreaterOrEqual);
        CHECK(model.get_ctr_rhs(c2) == 5_a);
        CHECK(model.get_ctr_row(c2).get(x[0]) == 1_a);
        CHECK(model.get_ctr_row(c2).get(x[1]) == 2_a);
        CHECK(model.get_ctr_row(c2).get(x[2]) == 0_a);
        CHECK(model.get_var_column(x[0]).get(c2) == 1_a);
        CHECK(model.get_var_column(x[1]).get(c2) == 2_a);
        CHECK(model.get_var_column(x[2]).get(c2) == 0_a);
        CHECK(model.get_mat_coeff(c2, x[0]) == 1_a);
        CHECK(model.get_mat_coeff(c2, x[1]) == 2_a);
        CHECK(model.get_mat_coeff(c2, x[2]) == 0_a);

        const auto c3 = model.add_ctr(x[0] + 3 * x[1] == 7, "c3");
        CHECK(model.ctrs().size() == 3);
        CHECK(model.get_ctr_type(c3) == Equal);
        CHECK(model.get_ctr_rhs(c3) == 7_a);
        CHECK(model.get_ctr_row(c3).get(x[0]) == 1_a);
        CHECK(model.get_ctr_row(c3).get(x[1]) == 3_a);
        CHECK(model.get_ctr_row(c3).get(x[2]) == 0_a);
        CHECK(model.get_var_column(x[0]).get(c3) == 1_a);
        CHECK(model.get_var_column(x[1]).get(c3) == 3_a);
        CHECK(model.get_var_column(x[2]).get(c3) == 0_a);
        CHECK(model.get_mat_coeff(c3, x[0]) == 1_a);
        CHECK(model.get_mat_coeff(c3, x[1]) == 3_a);
        CHECK(model.get_mat_coeff(c3, x[2]) == 0_a);

    }

    SECTION("Can add an existing linear constraint") {

        const auto c1 = Ctr(env, x[0] + x[1] <= 10, "c1");
        const auto c2 = Ctr(env, x[0] + 2 * x[1] >= 5, "c2");
        const auto c3 = Ctr(env, x[0] + 3 * x[1] == 7, "c3");

        model.add(c1);
        CHECK(model.ctrs().size() == 1);
        CHECK(model.get_ctr_type(c1) == LessOrEqual);
        CHECK(model.get_ctr_rhs(c1) == 10_a);
        CHECK(model.get_ctr_row(c1).get(x[0]) == 1_a);
        CHECK(model.get_ctr_row(c1).get(x[1]) == 1_a);
        CHECK(model.get_ctr_row(c1).get(x[2]) == 0_a);
        CHECK(model.get_var_column(x[0]).get(c1) == 1_a);
        CHECK(model.get_var_column(x[1]).get(c1) == 1_a);
        CHECK(model.get_var_column(x[2]).get(c1) == 0_a);
        CHECK(model.get_mat_coeff(c1, x[0]) == 1_a);
        CHECK(model.get_mat_coeff(c1, x[1]) == 1_a);
        CHECK(model.get_mat_coeff(c1, x[2]) == 0_a);

        model.add(c2);
        CHECK(model.ctrs().size() == 2);
        CHECK(model.get_ctr_type(c2) == GreaterOrEqual);
        CHECK(model.get_ctr_rhs(c2) == 5_a);
        CHECK(model.get_ctr_row(c2).get(x[0]) == 1_a);
        CHECK(model.get_ctr_row(c2).get(x[1]) == 2_a);
        CHECK(model.get_ctr_row(c2).get(x[2]) == 0_a);
        CHECK(model.get_var_column(x[0]).get(c2) == 1_a);
        CHECK(model.get_var_column(x[1]).get(c2) == 2_a);
        CHECK(model.get_var_column(x[2]).get(c2) == 0_a);
        CHECK(model.get_mat_coeff(c2, x[0]) == 1_a);
        CHECK(model.get_mat_coeff(c2, x[1]) == 2_a);
        CHECK(model.get_mat_coeff(c2, x[2]) == 0_a);

        model.add(c3);
        CHECK(model.ctrs().size() == 3);
        CHECK(model.get_ctr_type(c3) == Equal);
        CHECK(model.get_ctr_rhs(c3) == 7_a);
        CHECK(model.get_ctr_row(c3).get(x[0]) == 1_a);
        CHECK(model.get_ctr_row(c3).get(x[1]) == 3_a);
        CHECK(model.get_ctr_row(c3).get(x[2]) == 0_a);
        CHECK(model.get_var_column(x[0]).get(c3) == 1_a);
        CHECK(model.get_var_column(x[1]).get(c3) == 3_a);
        CHECK(model.get_var_column(x[2]).get(c3) == 0_a);
        CHECK(model.get_mat_coeff(c3, x[0]) == 1_a);
        CHECK(model.get_mat_coeff(c3, x[1]) == 3_a);
        CHECK(model.get_mat_coeff(c3, x[2]) == 0_a);

    }

    SECTION("Can add an existing linear constraint with new type and expression") {

        const auto c1 = Ctr(env, TempCtr(), "c1");
        const auto c2 = Ctr(env, TempCtr(), "c2");
        const auto c3 = Ctr(env, TempCtr(), "c3");

        model.add(c1, x[0] + x[1] <= 10);
        CHECK(model.ctrs().size() == 1);
        CHECK(model.get_ctr_type(c1) == LessOrEqual);
        CHECK(model.get_ctr_rhs(c1) == 10_a);
        CHECK(model.get_ctr_row(c1).get(x[0]) == 1_a);
        CHECK(model.get_ctr_row(c1).get(x[1]) == 1_a);
        CHECK(model.get_ctr_row(c1).get(x[2]) == 0_a);
        CHECK(model.get_var_column(x[0]).get(c1) == 1_a);
        CHECK(model.get_var_column(x[1]).get(c1) == 1_a);
        CHECK(model.get_var_column(x[2]).get(c1) == 0_a);
        CHECK(model.get_mat_coeff(c1, x[0]) == 1_a);
        CHECK(model.get_mat_coeff(c1, x[1]) == 1_a);
        CHECK(model.get_mat_coeff(c1, x[2]) == 0_a);

        model.add(c2, x[0] + 2 * x[1] >= 5);
        CHECK(model.ctrs().size() == 2);
        CHECK(model.get_ctr_type(c2) == GreaterOrEqual);
        CHECK(model.get_ctr_rhs(c2) == 5_a);
        CHECK(model.get_ctr_row(c2).get(x[0]) == 1_a);
        CHECK(model.get_ctr_row(c2).get(x[1]) == 2_a);
        CHECK(model.get_ctr_row(c2).get(x[2]) == 0_a);
        CHECK(model.get_var_column(x[0]).get(c2) == 1_a);
        CHECK(model.get_var_column(x[1]).get(c2) == 2_a);
        CHECK(model.get_var_column(x[2]).get(c2) == 0_a);
        CHECK(model.get_mat_coeff(c2, x[0]) == 1_a);
        CHECK(model.get_mat_coeff(c2, x[1]) == 2_a);
        CHECK(model.get_mat_coeff(c2, x[2]) == 0_a);

        model.add(c3, x[0] + 3 * x[1] == 7);
        CHECK(model.ctrs().size() == 3);
        CHECK(model.get_ctr_type(c3) == Equal);
        CHECK(model.get_ctr_rhs(c3) == 7_a);
        CHECK(model.get_ctr_row(c3).get(x[0]) == 1_a);
        CHECK(model.get_ctr_row(c3).get(x[1]) == 3_a);
        CHECK(model.get_ctr_row(c3).get(x[2]) == 0_a);
        CHECK(model.get_var_column(x[0]).get(c3) == 1_a);
        CHECK(model.get_var_column(x[1]).get(c3) == 3_a);
        CHECK(model.get_var_column(x[2]).get(c3) == 0_a);
        CHECK(model.get_mat_coeff(c3, x[0]) == 1_a);
        CHECK(model.get_mat_coeff(c3, x[1]) == 3_a);
        CHECK(model.get_mat_coeff(c3, x[2]) == 0_a);

    }

    SECTION("Can add multiple new linear constraints with type and right-hand side") {

        const auto c1 = model.add_ctrs(Dim<1>(5), LessOrEqual, 1, "c1");
        CHECK(model.ctrs().size() == 5);
        for (const auto& c : c1) {
            CHECK(model.get_ctr_type(c) == LessOrEqual);
            CHECK(model.get_ctr_rhs(c) == 1_a);
        }

        const auto c2 = model.add_ctrs(Dim<1>(10), GreaterOrEqual, 2, "c2");
        CHECK(model.ctrs().size() == 15);
        for (const auto& c : c2) {
            CHECK(model.get_ctr_type(c) == GreaterOrEqual);
            CHECK(model.get_ctr_rhs(c) == 2_a);
        }

        const auto c3 = model.add_ctrs(Dim<1>(15), Equal, 3, "c3");
        CHECK(model.ctrs().size() == 30);
        for (const auto& c : c3) {
            CHECK(model.get_ctr_type(c) == Equal);
            CHECK(model.get_ctr_rhs(c) == 3_a);
        }

    }

    SECTION("Can add multiple existing linear constraints with type and right-hand side") {

        const auto c1 = Ctr::make_vector(env, Dim<1>(5), LessOrEqual, 1, "c1");
        const auto c2 = Ctr::make_vector(env, Dim<1>(10), GreaterOrEqual, 2, "c2");
        const auto c3 = Ctr::make_vector(env, Dim<1>(15), Equal, 3, "c3");

        model.add_vector<Ctr, 1>(c1);
        CHECK(model.ctrs().size() == 5);
        for (const auto& c : c1) {
            CHECK(model.get_ctr_type(c) == LessOrEqual);
            CHECK(model.get_ctr_rhs(c) == 1_a);
        }

        model.add_vector<Ctr, 1>(c2);
        CHECK(model.ctrs().size() == 15);
        for (const auto& c : c2) {
            CHECK(model.get_ctr_type(c) == GreaterOrEqual);
            CHECK(model.get_ctr_rhs(c) == 2_a);
        }

        model.add_vector<Ctr, 1>(c3);
        CHECK(model.ctrs().size() == 30);
        for (const auto& c : c3) {
            CHECK(model.get_ctr_type(c) == Equal);
            CHECK(model.get_ctr_rhs(c) == 3_a);
        }

    }

}

TEST_CASE_WITH_OPTIMIZER("Can update a linear constraint in a model") {

    Env env;
    Model model(env);
    const auto x = model.add_vars(Dim<1>(3), -Inf, Inf, Continuous, 0, "x");

#ifdef OPTIMIZER
    model.use(OPTIMIZER());
    model.update();
#endif

    SECTION("Can update a coefficient in the linear constraints matrix") {

        const auto c1 = model.add_ctr(x[0] + x[1] <= 10, "c1");

        model.set_mat_coeff(c1, x[1], 5);
        CHECK(model.get_mat_coeff(c1, x[1]) == 5_a);
        CHECK(model.get_ctr_row(c1).get(x[0]) == 1_a);
        CHECK(model.get_ctr_row(c1).get(x[1]) == 5_a);
        CHECK(model.get_var_column(x[0]).get(c1) == 1_a);
        CHECK(model.get_var_column(x[1]).get(c1) == 5_a);

    }

    SECTION("Can update the left-hand side of a linear constraint") {

        const auto c1 = model.add_ctr(x[0] + x[1] <= 10, "c1");

        model.set_ctr_row(c1, x[0] + 5 * x[1]);
        CHECK(model.get_mat_coeff(c1, x[1]) == 5_a);
        CHECK(model.get_ctr_row(c1).get(x[0]) == 1_a);
        CHECK(model.get_ctr_row(c1).get(x[1]) == 5_a);
        CHECK(model.get_var_column(x[0]).get(c1) == 1_a);
        CHECK(model.get_var_column(x[1]).get(c1) == 5_a);

    }

    SECTION("Can update the right-hand side of a linear constraint") {


        const auto c1 = model.add_ctr(x[0] + x[1] <= 10, "c1");

        model.set_ctr_rhs(c1, 20);
        CHECK(model.get_ctr_rhs(c1) == 20_a);

    }

    SECTION("Can update the type of a linear constraint") {

        const auto c1 = model.add_ctr(x[0] + x[1] <= 10, "c1");

        model.set_ctr_type(c1, GreaterOrEqual);
        CHECK(model.get_ctr_type(c1) == GreaterOrEqual);

        model.set_ctr_type(c1, Equal);
        CHECK(model.get_ctr_type(c1) == Equal);

        model.set_ctr_type(c1, LessOrEqual);
        CHECK(model.get_ctr_type(c1) == LessOrEqual);

    }

}

TEST_CASE_WITH_OPTIMIZER("Can remove a linear constraint from a model") {

    Env env;
    Model model(env);
    const auto x = model.add_vars(Dim<1>(3), -Inf, Inf, Continuous, 0, "x");

#ifdef OPTIMIZER
    model.use(OPTIMIZER());
    model.update();
#endif

    SECTION("Can remove a linear constraint") {

        const auto c1 = model.add_ctr(x[0] + x[1] <= 10, "c1");
        CHECK(model.ctrs().size() == 1);
        CHECK(model.has(c1));
        model.remove(c1);
        CHECK(model.ctrs().size() == 0);
        CHECK(!model.has(c1));

    }

    SECTION("Throws an exception if trying to remove a linear constraint which is not in the model") {

        const auto c1 = Ctr(env, x[0] + x[1] <= 10, "c1");
        CHECK_THROWS_AS(model.remove(c1), Exception);

    }

}

TEST_CASE_WITH_OPTIMIZER("Can access a linear constraint in a model") {

    Env env;
    Model model(env);
    const auto x = model.add_vars(Dim<1>(3), -Inf, Inf, Continuous, 0, "x");

#ifdef OPTIMIZER
    model.use(OPTIMIZER());
    model.update();
#endif

    const auto c1 = model.add_ctr(x[0] + x[1] <= 10, "c1");
    const auto c2 = model.add_ctr(x[0] + 2 * x[1] >= 5, "c2");
    const auto c3 = model.add_ctr(x[0] + 3 * x[1] == 7, "c3");

    SECTION("Can retrieve a linear constraint's type") {
        CHECK(model.get_ctr_type(c1) == LessOrEqual);
        CHECK(model.get_ctr_type(c2) == GreaterOrEqual);
        CHECK(model.get_ctr_type(c3) == Equal);
    }

    SECTION("Can retrieve a linear constraint's right-hand side") {
        CHECK(model.get_ctr_rhs(c1) == 10_a);
        CHECK(model.get_ctr_rhs(c2) == 5_a);
        CHECK(model.get_ctr_rhs(c3) == 7_a);
    }

    SECTION("Can retrieve a linear constraint's left-hand side") {
        CHECK(model.get_ctr_row(c1).get(x[0]) == 1_a);
        CHECK(model.get_ctr_row(c1).get(x[1]) == 1_a);
        CHECK(model.get_ctr_row(c1).get(x[2]) == 0_a);

        CHECK(model.get_ctr_row(c2).get(x[0]) == 1_a);
        CHECK(model.get_ctr_row(c2).get(x[1]) == 2_a);
        CHECK(model.get_ctr_row(c2).get(x[2]) == 0_a);

        CHECK(model.get_ctr_row(c3).get(x[0]) == 1_a);
        CHECK(model.get_ctr_row(c3).get(x[1]) == 3_a);
        CHECK(model.get_ctr_row(c3).get(x[2]) == 0_a);
    }

    SECTION("Can retrieve a linear constraint's index") {
        CHECK(model.get_ctr_index(c1) == 0);
        CHECK(model.get_ctr_index(c2) == 1);
        CHECK(model.get_ctr_index(c3) == 2);
    }

    SECTION("Can retrieve a linear constraint by index") {
        CHECK(model.get_ctr_by_index(0).id() == c1.id());
        CHECK(model.get_ctr_by_index(1).id() == c2.id());
        CHECK(model.get_ctr_by_index(2).id() == c3.id());
    }

    SECTION("Can iterate over linear constraints in a model") {

        std::vector<Ctr> ctrs;
        for (const auto& ctr : model.ctrs()) {
            ctrs.push_back(ctr);
        }

        CHECK(ctrs.size() == 3);
        CHECK(ctrs[0].id() == c1.id());
        CHECK(ctrs[1].id() == c2.id());
        CHECK(ctrs[2].id() == c3.id());

    }

}