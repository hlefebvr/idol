//
// Created by henri on 03.04.25.
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
#include "idol/mixed-integer/optimizers/callbacks/cutting-planes/LazyCutCallback.h"

using namespace Catch::literals;
using namespace idol;

#ifndef IDOL_USE_GUROBI
namespace idol {
    class Gurobi;
}
#endif

#ifndef IDOL_USE_CPLEX
namespace idol {
    class Cplex;
}
#endif

#ifndef IDOL_USE_JUMP
namespace idol {
    class JuMP;
}
#endif

TEST_CASE("Can add universal callbacks to monitor the execution of the algorithm", "[callbacks]") {

    SECTION("Can retrieve root node gap") {
        CHECK(false);
    }

    SECTION("Can retrieve root node solution") {
        CHECK(false);
    }

    SECTION("Can access nodes' solutions") {
        CHECK(false);
    }

    SECTION("Can access integer solutions") {
        CHECK(false);
    }

}

TEST_CASE("Can add universal callbacks to influence the execution of the algorithm", "[callbacks]") {

    Env env;
    Model model(env);

    SECTION("Can add user cut") {

        if (std::is_same_v<OPTIMIZER, Gurobi> || std::is_same_v<OPTIMIZER, GLPK> || std::is_same_v<OPTIMIZER, Cplex>) {
            CHECK(false);
        } else {
           SKIP("The solver does not implement user cut.");
        }

    }

    SECTION("Can add lazy constraint") {

#if defined(OPTIMIZER_IS_Gurobi) || defined(OPTIMIZER_IS_Cplex)

        const auto y = model.add_var(0, Inf, Integer, 2, "y");
        const auto z = model.add_var(0, Inf, Continuous, 1, "z");

        Model separation(env);
        const auto lambda = separation.add_vars(Dim<1>(2), 0, Inf, Continuous, 0, "lambda");
        separation.add_ctr(lambda[0] + 2 * lambda[1] <= 2);
        separation.add_ctr(2 * lambda[0] - lambda[1] <= 3);
        const auto benders_cut = z - (!lambda[0] * 3 - !lambda[0] * y + !lambda[1] * 4 - !lambda[1] * 3 * y);

        auto cb = LazyCutCallback(separation, benders_cut, GreaterOrEqual);
        cb.with_separation_optimizer(Gurobi());

        auto optimizer = OPTIMIZER();
        optimizer.add_callback(cb);
#if defined(OPTIMIZER_IS_Gurobi) || defined(OPTIMIZER_IS_Cplex)
        optimizer.with_lazy_cut(true);
#endif
        optimizer.with_logs(true);

        model.use(optimizer);
        model.optimize();

        CHECK(model.get_status() == Optimal);
        CHECK(model.get_best_obj() == 5.4_a);

#elif defined(OPTIMIZER_IS_GLPK)
        CHECK(false);
#else
        SKIP("The solver des not implement lazy cuts");
#endif

    }

    SECTION("Can terminate the algorithm") {


        if (std::is_same_v<OPTIMIZER, Gurobi> || std::is_same_v<OPTIMIZER, GLPK> || std::is_same_v<OPTIMIZER, Cplex>) {
            CHECK(false);
        } else {
            SKIP("The solver does not implement early termination by user.");
        }

    }

}
