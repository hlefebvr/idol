//
// Created by henri on 22.10.23.
//
#include <catch2/catch_all.hpp>
#include "idol/modeling.h"
#include "idol/mixed-integer/optimizers/wrappers/GLPK/GLPK.h"
#include "idol/mixed-integer/optimizers/wrappers/Mosek/Mosek.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"
#include "idol/mixed-integer/optimizers/wrappers/HiGHS/HiGHS.h"
#include "idol/mixed-integer/optimizers/wrappers/Osi/Osi.h"
#include "idol/mixed-integer/optimizers/wrappers/Cplex/Cplex.h"
#include "idol/mixed-integer/optimizers/wrappers/JuMP/JuMP.h"

using namespace Catch::literals;
using namespace idol;

/*
 * - Test parameters
 */

TEST_CASE("Can solve a feasible MIP which is integer at the root node", "[MILP]") {

    // Taken from https://www.gurobi.com/documentation/9.5/examples/mip1_cpp_cpp.html#subsubsection:mip1_c++.cpp
    Env env;

    Var x(env, 0., 1., Binary, 0., "x");
    Var y(env, 0., 1., Binary, 0., "y");
    Var z(env, 0., 1., Binary, 0., "z");
    Ctr c1(env, x + 2 * y + 3 * z <= 4);
    Ctr c2(env, x + y >= 1);
    auto objective = -x - y - 2 * z;

    Model model(env);
    model.add(x);
    model.add(y);
    model.add(z);
    model.add(c1);
    model.add(c2);
    model.set_obj_expr(objective);

    model.use(OPTIMIZER());

    model.optimize();

    SECTION("Can retrieve primal solution") {
        CHECK(model.get_status() == Optimal);
        CHECK(model.get_best_obj() == -3_a);

        PrimalPoint primal_solution;
        CHECK_NOTHROW(primal_solution = save_primal(model));

        CHECK(primal_solution.get(x) == 1._a);
        CHECK(primal_solution.get(y) == 0._a);
        CHECK(primal_solution.get(z) == 1._a);

    }

    SECTION("Can retrieve the number of solutions") {
        CHECK(model.get_n_solutions() >= 1);
    }

    SECTION("Can iterate over solutions") {
        for (unsigned int k = 0, n = model.get_n_solutions() ; k < n ; ++k) {
            CHECK_NOTHROW(model.set_solution_index(k));
        }
    }

}


TEST_CASE("Can solve a feasible MIP which is not integer at the root node", "[MILP]") {

    Env env;

    Var x(env, 0., 1., Binary, 0., "x");
    Var y(env, 0., 1., Binary, 0., "y");
    Var z(env, 0., 1., Binary, 0., "z");
    Ctr c1(env, x + 2 * y + 2.5 * z <= 4);
    Ctr c2(env, x + y >= 1);
    auto objective = -x - y - 2 * z;

    Model model(env);
    model.add(x);
    model.add(y);
    model.add(z);
    model.add(c1);
    model.add(c2);
    model.set_obj_expr(objective);

    model.use(OPTIMIZER());

    model.optimize();

    SECTION("Can retrieve the primal solution") {

        CHECK(model.get_status() == Optimal);
        CHECK(model.get_best_obj() == -3_a);

        PrimalPoint primal_solution;
        CHECK_NOTHROW(primal_solution = save_primal(model));

        CHECK(primal_solution.get(x) == 1._a);
        CHECK(primal_solution.get(y) == 0._a);
        CHECK(primal_solution.get(z) == 1._a);

        model.use(OPTIMIZER::ContinuousRelaxation());
        model.optimize();

        CHECK(model.get_status() == Optimal);
        CHECK(model.get_best_obj() == -3.25_a);

        primal_solution = save_primal(model);

        CHECK(primal_solution.get(x) == 1._a);
        CHECK(primal_solution.get(y) == 0.25_a);
        CHECK(primal_solution.get(z) == 1._a);

    }

    SECTION("Can retrieve the number of solutions") {
        CHECK(model.get_n_solutions() >= 1);
    }

    SECTION("Can iterate over solutions") {
        for (unsigned int k = 0, n = model.get_n_solutions() ; k < n ; ++k) {
            CHECK_NOTHROW(model.set_solution_index(k));
        }
    }

}

TEST_CASE("Can solve an infeasible MIP which is infeasible at root node", "[MILP]") {

    Env env;

    Var x(env, 0., 1., Binary, 0., "x");
    Ctr c1(env, x >= 1);
    Ctr c2(env, x <= 0);

    Model model(env);
    model.add(x);
    model.add(c1);
    model.add(c2);

    model.use(OPTIMIZER());

    model.optimize();

    SECTION("Can retrieve the solution status") {
        SolutionStatus status;
        CHECK_NOTHROW(status = model.get_status());
        CHECK((status == Infeasible || status == InfOrUnbnd));

        SECTION("Can retrieve the solution status") {
            CHECK_NOTHROW(status = model.get_status());
            CHECK((status == Infeasible || status  == InfOrUnbnd));
            if (status == Infeasible) {
                CHECK(is_pos_inf(model.get_best_obj()));
            } else {
                CHECK(is_neg_inf(model.get_best_obj()));
            }
        }

    }

    SECTION("Can retrieve the number of solutions") {
        CHECK(model.get_n_solutions() == 0);
    }

    SECTION("Can compute an irreducible infeasible sub-system") {
        if (std::is_same_v<OPTIMIZER, Gurobi>) {
            CHECK(false);
        } else {
            SKIP("The solver does not implement IIS computation.");
        }
    }

}

TEST_CASE("Can solve an infeasible MIP which is feasible at the root node", "[MILP]") {

    Env env;

    Var x(env, 0., 1., Binary, 0., "x");
    Ctr c1(env, x >= .1);
    Ctr c2(env, x <= .9);

    Model model(env);

    model.add(x);
    model.add(c1);
    model.add(c2);

    model.use(OPTIMIZER());

    model.optimize();

    SECTION("Can retrieve the solution status") {
        SolutionStatus status;
        CHECK_NOTHROW(status = model.get_status());
        CHECK((status == Infeasible || status  == InfOrUnbnd));
        if (status == Infeasible) {
            CHECK(is_pos_inf(model.get_best_obj()));
        } else {
            CHECK(is_neg_inf(model.get_best_obj()));
        }
    }

    SECTION("Can retrieve the number of solutions") {
        CHECK(model.get_n_solutions() == 0);
    }

    SECTION("Can compute an irreducible infeasible sub-system") {
        if (std::is_same_v<OPTIMIZER, Gurobi>) {
            CHECK(false);
        } else {
            SKIP("The solver does not implement IIS computation.");
        }
    }

}

TEST_CASE("Can solve an unbounded MIP", "[MILP]") {

    Env env;
    Var x(env, -Inf, Inf, Integer, 0., "x");
    Model model(env);
    model.add(x);
    model.set_obj_expr(-x);

    model.use(OPTIMIZER());

    model.optimize();

    SECTION("Can retrieve the solution status") {
        SolutionStatus status;
        CHECK_NOTHROW(status = model.get_status());
        CHECK((status == Unbounded || status  == InfOrUnbnd));
        if (status == Unbounded) {
            CHECK(is_neg_inf(model.get_best_obj()));
        } else {
            CHECK(is_pos_inf(model.get_best_obj()));
        }
    }

    SECTION("Can retrieve the number of solutions") {
        CHECK(model.get_n_solutions() == 0);
    }

}

TEST_CASE("Can set parameters of the underlying solver", "[MILP]") {

    Env env;
    Model model(env);

    model.use(OPTIMIZER());

    SECTION("Can set a time limit") {
        CHECK_NOTHROW(model.optimizer().set_param_time_limit(3600));
        CHECK(model.optimizer().get_param_time_limit() == 3600_a);
    }

    SECTION("Can turn off presolve") {
        CHECK_NOTHROW(model.optimizer().set_param_presolve(false));
        CHECK(model.optimizer().get_param_presolve() == false);
    }

    SECTION("Can activate infeasible or unbounded info") {
        CHECK_NOTHROW(model.optimizer().set_param_infeasible_or_unbounded_info(true));
        CHECK(model.optimizer().get_param_infeasible_or_unbounded_info() == true);
    }

    SECTION("Can set a thread limit") {
        CHECK_NOTHROW(model.optimizer().set_param_threads(4));
        CHECK(model.optimizer().get_param_thread_limit() == 4);
    }

    SECTION("Can set a relative MIP gap") {
        CHECK_NOTHROW(model.optimizer().set_tol_mip_relative_gap(0.01));
        CHECK(model.optimizer().get_tol_mip_relative_gap() == 0.01_a);
    }

    SECTION("Can set an absolute MIP gap") {
        CHECK_NOTHROW(model.optimizer().set_tol_mip_absolute_gap(100.0));
        CHECK(model.optimizer().get_tol_mip_absolute_gap() == 100.0_a);
    }

    SECTION("Can set a best bound stop") {
        CHECK_NOTHROW(model.optimizer().set_param_best_bound_stop(5000));
        CHECK(model.optimizer().get_param_best_bound_stop() == 5000_a);
    }

    SECTION("Can set a best obj stop") {
        CHECK_NOTHROW(model.optimizer().set_param_best_obj_stop(10000));
        CHECK(model.optimizer().get_param_best_obj_stop() == 10000_a);
    }

    SECTION("Can set an iteration limit") {
        CHECK_NOTHROW(model.optimizer().set_param_iteration_limit(100000));
        CHECK(model.optimizer().get_param_iteration_limit() == 100000_a);
    }

    SECTION("Can turn on logs") {
        CHECK_NOTHROW(model.optimizer().set_param_logs(true));
        CHECK(model.optimizer().get_param_logs() == true);
        CHECK_NOTHROW(model.optimizer().set_param_logs(false));
        CHECK(model.optimizer().get_param_logs() == false);
    }

}
