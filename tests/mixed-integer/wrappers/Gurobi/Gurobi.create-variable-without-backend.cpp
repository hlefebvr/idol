//
// Created by henri on 31/01/23.
//

#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Optimizers_Gurobi.h"
#include <catch2/catch_all.hpp>
#include "idol/modeling.h"

using namespace Catch::literals;
using namespace idol;

#ifdef IDOL_USE_GUROBI

SCENARIO("Gurobi: Create a variable without backend", "[unit][backend][Gurobi]") {

    GIVEN("A initially empty model with no backend") {

        Env env;
        Model model(env);

        WHEN("A continuous variable (lb=-15,ub=15) is added to the model") {

            Var x(env, -15, 30, Continuous, 0., "x");
            model.add(x);

            model.use(Gurobi());

            const auto& backend = ((const Model &) model).optimizer().as<Optimizers::Gurobi>();

            model.update();

            THEN("The backend's model should have the added variable") {
                CHECK_NOTHROW(backend[x]);
            }

            AND_THEN("The variable's lower bound should be -15") {
                CHECK(backend[x].get(GRB_DoubleAttr_LB) == -15_a);
            }

            AND_THEN("The variable's upper bound should be 30") {
                CHECK(backend[x].get(GRB_DoubleAttr_UB) == 30_a);
            }

            AND_THEN("The variable's type should be continuous") {
                CHECK(backend[x].get(GRB_CharAttr_VType) == GRB_CONTINUOUS);
            }

            WHEN("The variable is removed") {
                model.remove(x);
                model.update();

                THEN("The variable should not be part of the model") {
                    CHECK_THROWS(backend[x]);
                }
            }

        }

        WHEN("An integer variable (lb=-inf,ub=inf) is added to the model") {

            Var x(env, -Inf, Inf, Integer, 0., "x");
            model.add(x);

            model.use(Gurobi());

            const auto& backend = ((const Model &) model).optimizer().as<Optimizers::Gurobi>();

            model.update();

            THEN("The backend's model should have the added variable") {
                CHECK_NOTHROW(backend[x]);
            }

            AND_THEN("The variable's lower bound should be -GRB_INFINITY") {
                CHECK(backend[x].get(GRB_DoubleAttr_LB) == -GRB_INFINITY);
            }

            AND_THEN("The variable's upper bound should be GRB_INFINITY") {
                CHECK(backend[x].get(GRB_DoubleAttr_UB) == GRB_INFINITY);
            }

            AND_THEN("The variable's type should be integer") {
                CHECK(backend[x].get(GRB_CharAttr_VType) == GRB_INTEGER);
            }

            WHEN("The variable is removed") {
                model.remove(x);
                model.update();

                THEN("The variable should not be part of the model") {
                    CHECK_THROWS(backend[x]);
                }
            }

        }

        WHEN("A binary variable (lb=1,ub=1) is added to the model") {

            Var x(env, 1, 1, Binary, 0., "x");
            model.add(x);

            model.use(Gurobi());

            const auto& backend = ((const Model &) model).optimizer().as<Optimizers::Gurobi>();

            model.update();

            THEN("The backend's model should have the added variable") {
                CHECK_NOTHROW(backend[x]);
            }

            AND_THEN("The variable's lower bound should be 1") {
                CHECK(backend[x].get(GRB_DoubleAttr_LB) == 1);
            }

            AND_THEN("The variable's upper bound should be 1") {
                CHECK(backend[x].get(GRB_DoubleAttr_UB) == 1);
            }

            AND_THEN("The variable's type should be integer") {
                CHECK(backend[x].get(GRB_CharAttr_VType) == GRB_BINARY);
            }

            WHEN("The variable is removed") {
                model.remove(x);
                model.update();

                THEN("The variable should not be part of the model") {
                    CHECK_THROWS(backend[x]);
                }
            }

        }

    }

    GIVEN("A model with some constraints and no backend") {

        Env env;
        Model model(env);

        auto c = Ctr::make_vector(env, Dim<1>(3), LessOrEqual, 0.);
        model.add_vector<Ctr, 1>(c);

        WHEN("A continuous variable (lb=0,ub=inf) is added with a Column") {

            LinExpr<Ctr> column;
            column.set(c[0], 0);
            column.set(c[1], 1);
            column.set(c[2], 2);

            Var x(env, 0, Inf, Continuous, 1., std::move(column), "x");
            model.add(x);

            model.use(Gurobi());

            const auto& backend = ((const Model &) model).optimizer().as<Optimizers::Gurobi>();

            model.update();

            THEN("The backend's model should have the added variable") {
                CHECK_NOTHROW(backend[x]);
            }

            AND_THEN("The variable's lower bound should be 0") {
                CHECK(backend[x].get(GRB_DoubleAttr_LB) == 0._a);
            }

            AND_THEN("The variable's upper bound should be GRB_INFINITY") {
                CHECK(backend[x].get(GRB_DoubleAttr_UB) == GRB_INFINITY);
            }

            AND_THEN("The variable's type should be GRB_CONTINUOUS") {
                CHECK(backend[x].get(GRB_CharAttr_VType) == GRB_CONTINUOUS);
            }

            AND_THEN("The variable's coefficient for constraint c_0 should be 0") {
                const auto& gurobi_var = backend[x];
                const auto& gurobi_ctr = std::get<GRBConstr>(backend[c[0]]);
                CHECK(backend.model().getCoeff(gurobi_ctr, gurobi_var) == 0);
            }

            AND_THEN("The variable's coefficient for constraint c_1 should be 1") {
                const auto& gurobi_var = backend[x];
                const auto& gurobi_ctr = std::get<GRBConstr>(backend[c[1]]);
                CHECK(backend.model().getCoeff(gurobi_ctr, gurobi_var) == 1);
            }

            AND_THEN("The variable's coefficient for constraint c_2 should be 2") {
                const auto& gurobi_var = backend[x];
                const auto& gurobi_ctr = std::get<GRBConstr>(backend[c[2]]);
                CHECK(backend.model().getCoeff(gurobi_ctr, gurobi_var) == 2);
            }

            AND_THEN("The variable's objective coefficient should be 1") {
                CHECK(backend[x].get(GRB_DoubleAttr_Obj) == 1._a);
            }

            WHEN("The variable is removed") {
                model.remove(x);
                model.update();

                THEN("The variable should not be part of the model") {
                    CHECK_THROWS(backend[x]);
                }
            }

        }

    }

}

#endif
