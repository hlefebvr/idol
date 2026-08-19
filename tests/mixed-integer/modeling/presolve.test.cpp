#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "idol/modeling.h"
#include "idol/mixed-integer/optimizers/presolve/OneRowBoundTightening.h"

using namespace idol;

namespace {
void run_one_row_bound_tightening(Model& t_model) {
    Presolvers::OneRowBoundTightening presolver;
    presolver.execute(t_model);
}
}

TEST_CASE("One-row bound tightening handles a single-variable <= constraint", "[basic][presolve]") {
    Env env;
    Model model(env);
    const auto x = model.add_var(0., 10., Continuous);
    model.add_ctr(x <= 5.);
    run_one_row_bound_tightening(model);
    CHECK(model.get_var_ub(x) == Catch::Approx(5.));
}

TEST_CASE("One-row bound tightening negates a single-variable >= constraint", "[basic][presolve]") {
    Env env;
    Model model(env);
    const auto x = model.add_var(0., 10., Continuous);
    model.add_ctr(x >= 5.);
    run_one_row_bound_tightening(model);
    CHECK(model.get_var_lb(x) == Catch::Approx(5.));
}

TEST_CASE("One-row bound tightening processes both sides of an equality", "[basic][presolve]") {
    Env env;
    Model model(env);
    const auto x = model.add_var(0., 10., Continuous);
    model.add_ctr(x == 5.);
    run_one_row_bound_tightening(model);
    CHECK(model.get_var_lb(x) == Catch::Approx(5.));
    CHECK(model.get_var_ub(x) == Catch::Approx(5.));
}

TEST_CASE("One-row bound tightening handles a negative coefficient", "[basic][presolve]") {
    Env env;
    Model model(env);
    const auto x = model.add_var(0., 10., Continuous);
    model.add_ctr(-x <= -4.);
    run_one_row_bound_tightening(model);
    CHECK(model.get_var_lb(x) == Catch::Approx(4.));
}

TEST_CASE("One-row bound tightening uses other variable bounds for <=", "[basic][presolve]") {
    Env env;
    Model model(env);
    const auto x = model.add_var(0., 10., Continuous);
    const auto y = model.add_var(2., 10., Continuous);
    model.add_ctr(x + 2 * y <= 8.);
    run_one_row_bound_tightening(model);
    CHECK(model.get_var_ub(x) == Catch::Approx(4.));
    CHECK(model.get_var_ub(y) == Catch::Approx(4.));
}

TEST_CASE("One-row bound tightening negates a multi-variable >= row", "[basic][presolve]") {
    Env env;
    Model model(env);
    const auto x = model.add_var(0., 10., Continuous);
    const auto y = model.add_var(0., 4., Continuous);
    model.add_ctr(x + 2 * y >= 12.);
    run_one_row_bound_tightening(model);
    CHECK(model.get_var_lb(x) == Catch::Approx(4.));
    CHECK(model.get_var_lb(y) == Catch::Approx(1.));
}

TEST_CASE("One-row bound tightening ignores a row with infinite minimum activity", "[basic][presolve]") {
    Env env;
    Model model(env);
    const auto x = model.add_var(0., Inf, Continuous);
    model.add_ctr(-x <= -4.);
    run_one_row_bound_tightening(model);
    CHECK(model.get_var_lb(x) == Catch::Approx(0.));
}

TEST_CASE("One-row bound tightening respects its improvement tolerance", "[basic][presolve]") {
    Env env;
    Model model(env);
    const auto x = model.add_var(0., 10., Continuous);
    model.add_ctr(x <= 9.999999);
    run_one_row_bound_tightening(model);
    CHECK(model.get_var_ub(x) == Catch::Approx(10.));
}

TEST_CASE("One-row bound tightening preserves an explicit feasible point", "[basic][presolve]") {
    Env env;
    Model model(env);
    const auto x = model.add_var(0., 10., Continuous);
    const auto y = model.add_var(2., 10., Continuous);
    model.add_ctr(x + y <= 5.);
    run_one_row_bound_tightening(model);
    CHECK(model.get_var_ub(x) == Catch::Approx(3.));
    CHECK(model.get_var_lb(x) <= 3.);
    CHECK(model.get_var_lb(y) <= 2.);
    CHECK(model.get_var_ub(y) >= 2.);
}
