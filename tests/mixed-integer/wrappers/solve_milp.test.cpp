#include <catch2/catch_all.hpp>

#include "idol/modeling.h"
#include "idol/mixed-integer/optimizers/wrappers/Cplex/Cplex.h"
#include "idol/mixed-integer/optimizers/wrappers/GLPK/GLPK.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"
#include "idol/mixed-integer/optimizers/wrappers/HiGHS/HiGHS.h"
#include "idol/mixed-integer/optimizers/wrappers/Osi/Osi.h"

using namespace Catch::literals;
using namespace idol;

TEST_CASE("MILP: binary model has a unique optimum", "[MILP]") {
    // min -x-y-2z; x+2y+3z<=4, x+y>=1, x,y,z binary.
    // Unique optimum: (1,0,1), objective -3.
    Env env;
    Model model(env);
    const auto x = model.add_var(0, 1, Binary, -1, "x");
    const auto y = model.add_var(0, 1, Binary, -1, "y");
    const auto z = model.add_var(0, 1, Binary, -2, "z");
    model.add_ctr(x + 2 * y + 3 * z <= 4);
    model.add_ctr(x + y >= 1);
    model.use(OPTIMIZER());
    model.optimize();
    CHECK(model.get_status() == Optimal);
    CHECK(model.get_best_obj() == -3_a);
    CHECK(model.get_var_primal(x) == 1_a);
    CHECK(model.get_var_primal(y) == 0_a);
    CHECK(model.get_var_primal(z) == 1_a);
}

TEST_CASE("MILP: general integer and negative domain", "[MILP]") {
    // min x subject to -3<=x<=4 and x integer. Optimum x=-3.
    Env env;
    Model model(env);
    const auto x = model.add_var(-3, 4, Integer, 1, "x");
    model.use(OPTIMIZER());
    model.optimize();
    CHECK(model.get_status() == Optimal);
    CHECK(model.get_best_obj() == -3_a);
    CHECK(model.get_var_primal(x) == -3_a);
}

TEST_CASE("MILP: continuous and integer variables both matter", "[MILP]") {
    // min -2x-y; 2x+y<=5, x integer in [0,2], y continuous in [0,2].
    // Unique optimum: x=2,y=1, objective -5.
    Env env;
    Model model(env);
    const auto x = model.add_var(0, 2, Integer, -2, "x");
    const auto y = model.add_var(0, 2, Continuous, -1, "y");
    model.add_ctr(2 * x + y <= 5);
    model.use(OPTIMIZER());
    model.optimize();
    CHECK(model.get_status() == Optimal);
    CHECK(model.get_best_obj() == -5_a);
    CHECK(model.get_var_primal(x) == 2_a);
    CHECK(model.get_var_primal(y) == 1_a);
}

TEST_CASE("MILP: LP relaxation is strictly better", "[MILP]") {
    // min -x-y; 2x+2y<=3, x,y binary. MILP optimum -1; LP optimum -1.5.
    Env env;
    Model model(env);
    const auto x = model.add_var(0, 1, Binary, -1, "x");
    const auto y = model.add_var(0, 1, Binary, -1, "y");
    model.add_ctr(2 * x + 2 * y <= 3);
    model.use(OPTIMIZER());
    model.optimize();
    CHECK(model.get_status() == Optimal);
    CHECK(model.get_best_obj() == -1_a);
    CHECK(model.get_var_primal(x) + model.get_var_primal(y) == 1_a);
}

TEST_CASE("MILP: fixed discrete variables and binary semantics", "[MILP]") {
    // x is binary and fixed to 1; y is integer and fixed to -2.
    Env env;
    Model model(env);
    const auto x = model.add_var(1, 1, Binary, 3, "x");
    const auto y = model.add_var(-2, -2, Integer, 1, "y");
    model.use(OPTIMIZER());
    model.optimize();
    CHECK(model.get_status() == Optimal);
    CHECK(model.get_best_obj() == 1_a);
    CHECK(model.get_var_primal(x) == 1_a);
    CHECK(model.get_var_primal(y) == -2_a);
}

TEST_CASE("MILP: repeated bound, objective, and RHS updates", "[MILP]") {
    // min -x-y; x,y binary; x+y<=1. Initially objective -1.
    Env env;
    Model model(env);
    const auto x = model.add_var(0, 1, Binary, -1, "x");
    const auto y = model.add_var(0, 1, Binary, -1, "y");
    const auto c = model.add_ctr(x + y <= 1);
    model.use(OPTIMIZER());
    model.optimize();
    CHECK(model.get_status() == Optimal);
    CHECK(model.get_best_obj() == -1_a);

    model.set_var_lb(x, 1);
    model.optimize();
    CHECK(model.get_var_primal(x) == 1_a);
    CHECK(model.get_var_primal(y) == 0_a);

    model.set_var_lb(x, 0);
    model.set_var_ub(x, 0);
    model.optimize();
    CHECK(model.get_best_obj() == -1_a);
    CHECK(model.get_var_primal(x) == 0_a);
    CHECK(model.get_var_primal(y) == 1_a);

    model.set_var_ub(x, 1);
    model.set_var_obj(y, -2);
    model.optimize();
    CHECK(model.get_best_obj() == -2_a);
    CHECK(model.get_var_primal(x) == 0_a);
    CHECK(model.get_var_primal(y) == 1_a);

    model.set_ctr_rhs(c, 2);
    model.optimize();
    CHECK(model.get_best_obj() == -3_a);
    CHECK(model.get_var_primal(x) == 1_a);
    CHECK(model.get_var_primal(y) == 1_a);
}

TEST_CASE("MILP: infeasible status", "[MILP]") {
    // Binary x with x>=1 and x<=0 is infeasible.
    Env env;
    Model model(env);
    const auto x = model.add_var(0, 1, Binary, 0, "x");
    model.add_ctr(x >= 1);
    model.add_ctr(x <= 0);
    model.use(OPTIMIZER());
    model.optimize();
    CHECK(model.get_status() == Infeasible);
    CHECK(is_pos_inf(model.get_best_obj()));
}
