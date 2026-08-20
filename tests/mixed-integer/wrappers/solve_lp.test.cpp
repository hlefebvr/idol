#include <catch2/catch_all.hpp>

#include "idol/modeling.h"
#include "idol/mixed-integer/optimizers/wrappers/Cplex/Cplex.h"
#include "idol/mixed-integer/optimizers/wrappers/GLPK/GLPK.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"
#include "idol/mixed-integer/optimizers/wrappers/HiGHS/HiGHS.h"
#include "idol/mixed-integer/optimizers/wrappers/Osi/Osi.h"

using namespace Catch::literals;
using namespace idol;

TEST_CASE("LP: bounded model has a unique optimum", "[LP]") {
    // min -3 x - 2 y; x + y <= 4, x <= 2, y <= 3, x,y >= 0.
    // Unique optimum: (x,y) = (2,2), objective -10.
    Env env;
    Model model(env);
    const auto x = model.add_var(0, Inf, Continuous, -3, "x");
    const auto y = model.add_var(0, Inf, Continuous, -2, "y");
    model.add_ctr(x + y <= 4);
    model.add_ctr(x <= 2);
    model.add_ctr(y <= 3);
    model.use(OPTIMIZER());
    model.optimize();
    CHECK(model.get_status() == Optimal);
    CHECK(model.get_best_obj() == -10_a);
    CHECK(model.get_best_bound() == -10_a);
    CHECK(model.get_var_primal(x) == 2_a);
    CHECK(model.get_var_primal(y) == 2_a);
}

TEST_CASE("LP: equality and mixed row types", "[LP]") {
    // x + 2y = 4, x + y >= 3, x - y <= 1, x,y >= 0.
    // These rows uniquely imply (x,y) = (2,1); min x+y = 3.
    Env env;
    Model model(env);
    const auto x = model.add_var(0, Inf, Continuous, 1, "x");
    const auto y = model.add_var(0, Inf, Continuous, 1, "y");
    model.add_ctr(x + 2 * y == 4);
    model.add_ctr(x + y >= 3);
    model.add_ctr(x - y <= 1);
    model.use(OPTIMIZER());
    model.optimize();
    CHECK(model.get_status() == Optimal);
    CHECK(model.get_best_obj() == 3_a);
    CHECK(model.get_var_primal(x) == 2_a);
    CHECK(model.get_var_primal(y) == 1_a);
}

TEST_CASE("LP: variable-bound corner cases", "[LP]") {
    // min x + 2y - z with x=2, y>=-1, z<=3, and free f fixed by f=0.
    // Unique optimum: (2,-1,3,0), objective -3.
    Env env;
    Model model(env);
    const auto x = model.add_var(2, 2, Continuous, 1, "fixed");
    const auto y = model.add_var(-1, Inf, Continuous, 2, "lower_bounded");
    const auto z = model.add_var(-Inf, 3, Continuous, -1, "upper_bounded");
    const auto f = model.add_var(-Inf, Inf, Continuous, 0, "free");
    model.add_ctr(f == 0);
    model.use(OPTIMIZER());
    model.optimize();
    CHECK(model.get_status() == Optimal);
    CHECK(model.get_best_obj() == -3_a);
    CHECK(model.get_var_primal(x) == 2_a);
    CHECK(model.get_var_primal(y) == -1_a);
    CHECK(model.get_var_primal(z) == 3_a);
    CHECK(model.get_var_primal(f) == 0_a);
}

TEST_CASE("LP: objective and repeated model updates", "[LP]") {
    // Initially min x+5 over 0<=x<=10. Every later optimum is unique.
    Env env;
    Model model(env);
    const auto x = model.add_var(0, 10, Continuous, 1, "x");
    const auto c = model.add_ctr(x >= 0);
    model.set_obj_const(5);
    model.use(OPTIMIZER());
    model.optimize();
    CHECK(model.get_best_obj() == 5_a);
    CHECK(model.get_var_primal(x) == 0_a);

    model.set_var_lb(x, 2);
    model.optimize();
    CHECK(model.get_best_obj() == 7_a);
    CHECK(model.get_var_primal(x) == 2_a);

    model.set_var_ub(x, 4);
    model.set_var_obj(x, -1);
    model.optimize();
    CHECK(model.get_best_obj() == 1_a);
    CHECK(model.get_var_primal(x) == 4_a);

    model.set_obj_expr(2 * x + 1);
    model.optimize();
    CHECK(model.get_best_obj() == 5_a);
    CHECK(model.get_var_primal(x) == 2_a);

    model.set_ctr_rhs(c, 3);
    model.optimize();
    CHECK(model.get_best_obj() == 7_a);
    CHECK(model.get_var_primal(x) == 3_a);

    model.set_ctr_type(c, LessOrEqual);
    model.set_ctr_rhs(c, 3.5);
    model.set_var_lb(x, 0);
    model.set_obj_expr(-x);
    model.optimize();
    CHECK(model.get_best_obj() == -3.5_a);
    CHECK(model.get_var_primal(x) == 3.5_a);

    model.set_mat_coeff(c, x, 2);
    model.optimize();
    CHECK(model.get_best_obj() == -1.75_a);
    CHECK(model.get_var_primal(x) == 1.75_a);
}

TEST_CASE("LP: redundant and zero-coefficient rows", "[LP]") {
    // min x subject to x>=1, x<=2, and 0*x<=0. Optimum x=1.
    Env env;
    Model model(env);
    const auto x = model.add_var(-Inf, Inf, Continuous, 1, "x");
    model.add_ctr(x >= 1);
    model.add_ctr(x <= 2);
    model.add_ctr(0 * x <= 0);
    model.use(OPTIMIZER());
    model.optimize();
    CHECK(model.get_status() == Optimal);
    CHECK(model.get_best_obj() == 1_a);
    CHECK(model.get_var_primal(x) == 1_a);
}

TEST_CASE("LP: infeasible status", "[LP]") {
    // x>=1 and x<=0 is infeasible.
    Env env;
    Model model(env);
    const auto x = model.add_var(-Inf, Inf, Continuous, 0, "x");
    model.add_ctr(x >= 1);
    model.add_ctr(x <= 0);
    model.use(OPTIMIZER());
    model.optimize();
    CHECK(model.get_status() == Infeasible);
    CHECK(is_pos_inf(model.get_best_obj()));
}

TEST_CASE("LP: unbounded status", "[LP]") {
    // min -x with x>=0 is unbounded below.
    Env env;
    Model model(env);
    const auto x = model.add_var(-Inf, Inf, Continuous, -1, "x");
    model.add_ctr(x >= 0);
    model.use(OPTIMIZER().with_presolve(false));
    model.optimize();
    CHECK(model.get_status() == Unbounded);
    CHECK(is_neg_inf(model.get_best_obj()));
}

TEST_CASE("LP: dual values follow idol row conventions", "[LP]") {
    // min x subject to x>=1 and x<=2. Duals are 1 and 0, respectively.
    Env env;
    Model model(env);
    const auto x = model.add_var(-Inf, Inf, Continuous, 1, "x");
    const auto lower = model.add_ctr(x >= 1);
    const auto upper = model.add_ctr(x <= 2);
    model.use(OPTIMIZER());
    model.optimize();
    REQUIRE(model.get_status() == Optimal);
    CHECK(model.get_ctr_dual(lower) == 1_a);
    CHECK(model.get_ctr_dual(upper) == 0_a);
}

TEST_CASE("LP: reduced costs follow minimization convention", "[LP]") {
    // min 2x+y subject to y>=1, x,y>=0. At (0,1), rc(x)=2.
    Env env;
    Model model(env);
    const auto x = model.add_var(0, Inf, Continuous, 2, "x");
    const auto y = model.add_var(0, Inf, Continuous, 1, "y");
    model.add_ctr(y >= 1);
    model.use(OPTIMIZER());
    model.optimize();
    REQUIRE(model.get_status() == Optimal);
    CHECK(model.get_var_primal(x) == 0_a);
    CHECK(model.get_var_reduced_cost(x) == 2_a);
    CHECK(model.get_var_reduced_cost(y) == 0_a);
}

#ifdef WRAPPER_HAS_FARKAS
TEST_CASE("LP: Farkas certificate proves infeasibility", "[LP]") {
    // u-2v-w>=3, -2u+v-w>=2, u,v,w>=0 is infeasible. For returned
    // multipliers p,q, A^T(p,q)<=0 and b^T(p,q)>0 certify infeasibility.
    Env env;
    Model model(env);
    const auto u = model.add_var(0, Inf, Continuous, 0, "u");
    const auto v = model.add_var(0, Inf, Continuous, 0, "v");
    const auto w = model.add_var(0, Inf, Continuous, 0, "w");
    const auto c1 = model.add_ctr(u - 2 * v - w >= 3);
    const auto c2 = model.add_ctr(-2 * u + v - w >= 2);
    model.use(OPTIMIZER().with_infeasible_or_unbounded_info(true));
    model.optimize();
    REQUIRE(model.get_status() == Infeasible);
    const double p = model.get_ctr_farkas(c1);
    const double q = model.get_ctr_farkas(c2);
    CHECK(p - 2 * q <= 1e-7);
    CHECK(-2 * p + q <= 1e-7);
    CHECK(-p - q <= 1e-7);
    CHECK(3 * p + 2 * q > 1e-7);
}
#endif
