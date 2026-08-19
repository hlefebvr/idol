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
#define TEST_CASE_WITH_OPTIMIZER(title) TEST_CASE(title, "[quadratic-constraints]")
#endif

using namespace Catch::literals;
using namespace idol;

TEST_CASE_WITH_OPTIMIZER("Can add a quadratic constraint to a model") {

    Env env;
    Model model(env);
    const auto x = model.add_vars(Dim<1>(3), -Inf, Inf, Continuous, 0, "x");

#ifdef OPTIMIZER
    model.use(OPTIMIZER());
    model.update();
#endif

    SECTION("Can add a new quadratic constraint with type and expression") {
        const auto c = model.add_qctr(TempQCtr(x[0] * x[0] + x[1] - 10., LessOrEqual), "c");
        CHECK(model.has(c));
        CHECK(model.get_qctr_type(c) == LessOrEqual);
        CHECK(model.get_qctr_expr(c).get(CommutativePair<Var>(x[0], x[0])) == 1.);
        CHECK(model.get_qctr_expr(c).affine().linear().get(x[1]) == 1.);
        CHECK(model.get_qctr_expr(c).affine().constant() == -10.);
    }

    SECTION("Can add an existing quadratic constraint") {
        const QCtr c(env, TempQCtr(x[0] * x[1] - 3., GreaterOrEqual), "c");
        model.add(c);
        CHECK(model.has(c));
        CHECK(model.get_qctr_type(c) == GreaterOrEqual);
        CHECK(model.get_qctr_expr(c).get(CommutativePair<Var>(x[0], x[1])) == 1.);
    }

    SECTION("Can add an existing quadratic constraint with new type and expression") {
        const QCtr c(env, LessOrEqual, "c");
        model.add(c, TempQCtr(x[1] * x[2] + x[0] - 4., Equal));
        CHECK(model.get_qctr_type(c) == Equal);
        CHECK(model.get_qctr_expr(c).get(CommutativePair<Var>(x[1], x[2])) == 1.);
    }

    SECTION("Can add multiple new quadratic constraints with type and right-hand side") {
        const auto constraints = model.add_qctrs(Dim<1>(3), LessOrEqual, "c");
        CHECK(constraints.size() == 3);
        for (const auto& c : constraints) {
            CHECK(model.has(c));
            CHECK(model.get_qctr_type(c) == LessOrEqual);
        }
    }

    SECTION("Can add multiple existing quadratic constraints with type and right-hand side") {
        const auto constraints = QCtr::make_vector(env, Dim<1>(3), GreaterOrEqual, "c");
        for (const auto& c : constraints) {
            model.add(c);
            CHECK(model.has(c));
            CHECK(model.get_qctr_type(c) == GreaterOrEqual);
        }
    }

}

TEST_CASE_WITH_OPTIMIZER("Can remove a quadratic constraint from a model") {

    Env env;
    Model model(env);
    const auto x = model.add_vars(Dim<1>(3), -Inf, Inf, Continuous, 0, "x");

#ifdef OPTIMIZER
    model.use(OPTIMIZER());
    model.update();
#endif

    SECTION("Can remove a quadratic constraint") {
        const auto c = model.add_qctr(TempQCtr(x[0] * x[0] - 4., LessOrEqual));
        model.remove(c);
        CHECK_FALSE(model.has(c));
    }

    SECTION("Throws an exception if trying to remove a quadratic constraint which is not in the model") {

        const auto c1 = QCtr(env, TempQCtr(x[0] * x[0] + x[1] - 10, LessOrEqual), "c1");
        CHECK_THROWS_AS(model.remove(c1), Exception);

    }

}

TEST_CASE_WITH_OPTIMIZER("Can access a quadratic constraint in a model") {

    Env env;
    Model model(env);
    const auto x = model.add_vars(Dim<1>(3), -Inf, Inf, Continuous, 0, "x");

#ifdef OPTIMIZER
    model.use(OPTIMIZER());
    model.update();
#endif

    SECTION("Can retrieve a quadratic constraint's type") {
        const auto c = model.add_qctr(TempQCtr(x[0] * x[0] - 2., GreaterOrEqual));
        CHECK(model.get_qctr_type(c) == GreaterOrEqual);
    }

    SECTION("Can retrieve a quadratic constraint's expression") {
        const auto c = model.add_qctr(TempQCtr(x[0] * x[1] + 2 * x[2] - 6., LessOrEqual));
        const auto& expression = model.get_qctr_expr(c);
        CHECK(expression.get(CommutativePair<Var>(x[0], x[1])) == 1.);
        CHECK(expression.affine().linear().get(x[2]) == 2.);
        CHECK(expression.affine().constant() == -6.);
    }

    SECTION("Can retrieve a quadratic constraint's index") {
        const auto c = model.add_qctr(TempQCtr(x[0] * x[0] - 1., LessOrEqual));
        CHECK(model.get_qctr_index(c) == 0);
    }

    SECTION("Can retrieve a quadratic constraint by index") {
        const auto c = model.add_qctr(TempQCtr(x[0] * x[0] - 1., LessOrEqual));
        CHECK(model.get_qctr_by_index(0).id() == c.id());
    }

    SECTION("Can iterate over quadratic constraints in a model") {
        const auto c1 = model.add_qctr(TempQCtr(x[0] * x[0] - 1., LessOrEqual));
        const auto c2 = model.add_qctr(TempQCtr(x[1] * x[1] - 1., LessOrEqual));
        std::vector<QCtr> actual;
        for (const auto& c : model.qctrs()) {
            actual.emplace_back(c);
        }
        REQUIRE(actual.size() == 2);
        CHECK(actual[0].id() == c1.id());
        CHECK(actual[1].id() == c2.id());
    }

}
