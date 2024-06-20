//
// Created by henri on 31/01/23.
//
#include "idol/optimizers/mixed-integer-optimization/wrappers/Gurobi/Gurobi.h"
#include <catch2/catch_all.hpp>
#include <idol/modeling.h>

using namespace Catch::literals;
using namespace idol;

#ifdef IDOL_USE_GUROBI

SCENARIO("Gurobi: Create a constraint with backend", "[unit][backend][Gurobi]") {

    GIVEN("A initially empty model with a Gurobi backend") {

        Env env;
        Model model(env);

        model.use(Gurobi());

        const auto& backend = ((const Model &) model).optimizer().as<Optimizers::Gurobi>();

        WHEN("A <=-constraint (rhs=10) is added to the model") {

            Ctr c(env, LessOrEqual, 10, "c");
            model.add(c);

            model.update();

            THEN("The backend's model should have the added constraint") {
                CHECK_NOTHROW(backend[c]);
            }

            AND_THEN("The constraint should be <=") {
                CHECK(std::get<GRBConstr>(backend[c]).get(GRB_CharAttr_Sense) == GRB_LESS_EQUAL);
            }

            AND_THEN("The constraint rhs should be 10") {
                CHECK(std::get<GRBConstr>(backend[c]).get(GRB_DoubleAttr_RHS) == 10_a);
            }

            WHEN("The constraint is removed") {
                model.remove(c);
                model.update();

                THEN("The constraint should not be part of the model") {
                    CHECK_THROWS(backend[c]);
                }
            }

        }

        WHEN("A >=-constraint (rhs=-5) is added to the model") {

            Ctr c(env, GreaterOrEqual, -5, "c");
            model.add(c);

            model.update();

            THEN("The backend's model should have the added constraint") {
                CHECK_NOTHROW(backend[c]);
            }

            AND_THEN("The constraint should be <=") {
                CHECK(std::get<GRBConstr>(backend[c]).get(GRB_CharAttr_Sense) == GRB_GREATER_EQUAL);
            }

            AND_THEN("The constraint rhs should be 10") {
                CHECK(std::get<GRBConstr>(backend[c]).get(GRB_DoubleAttr_RHS) == -5_a);
            }

            WHEN("The constraint is removed") {
                model.remove(c);
                model.update();

                THEN("The constraint should not be part of the model") {
                    CHECK_THROWS(backend[c]);
                }
            }

        }


        WHEN("An ==-constraint (rhs=0) is added to the model") {

            Ctr c(env, Equal, 0, "c");
            model.add(c);

            model.update();

            THEN("The backend's model should have the added constraint") {
                CHECK_NOTHROW(backend[c]);
            }

            AND_THEN("The constraint should be <=") {
                CHECK(std::get<GRBConstr>(backend[c]).get(GRB_CharAttr_Sense) == GRB_EQUAL);
            }

            AND_THEN("The constraint rhs should be 10") {
                CHECK(std::get<GRBConstr>(backend[c]).get(GRB_DoubleAttr_RHS) == 0_a);
            }

            WHEN("The constraint is removed") {
                model.remove(c);
                model.update();

                THEN("The constraint should not be part of the model") {
                    CHECK_THROWS(backend[c]);
                }
            }

        }

    }

    GIVEN("A model with some variables and Gurobi backend") {

        Env env;
        Model model(env);

        model.use(Gurobi());

        const auto& backend = ((const Model &) model).optimizer().as<Optimizers::Gurobi>();

        auto x = Var::make_vector(env, Dim<1>(3), 0., 1., Continuous);
        model.add_vector<Var, 1>(x);

        WHEN("A <=-constraint (rhs=1) is added with a Row") {

            Ctr c(env, 0 * x[0] + 1 * x[1] + 2 * x[2] <= 1);
            model.add(c);

            model.update();

            THEN("The backend's model should have the added constraint") {
                CHECK_NOTHROW(backend[c]);
            }

            AND_THEN("The constraint should be <=") {
                CHECK(std::get<GRBConstr>(backend[c]).get(GRB_CharAttr_Sense) == GRB_LESS_EQUAL);
            }

            AND_THEN("The constraint rhs should be 1") {
                CHECK(std::get<GRBConstr>(backend[c]).get(GRB_DoubleAttr_RHS) == 1_a);
            }

            AND_THEN("The constraint's coefficient for variable x_0 should be 0") {
                const auto& gurobi_ctr = std::get<GRBConstr>(backend[c]);
                CHECK(backend.model().getCoeff(gurobi_ctr, backend[x[0]]) == 0_a);
            }

            AND_THEN("The constraint's coefficient for variable x_1 should be 1") {
                const auto& gurobi_ctr = std::get<GRBConstr>(backend[c]);
                CHECK(backend.model().getCoeff(gurobi_ctr, backend[x[1]]) == 1_a);
            }

            AND_THEN("The constraint's coefficient for variable x_2 should be 2") {
                const auto& gurobi_ctr = std::get<GRBConstr>(backend[c]);
                CHECK(backend.model().getCoeff(gurobi_ctr, backend[x[2]]) == 2_a);
            }

            WHEN("The constraint is removed") {
                model.remove(c);
                model.update();

                THEN("The constraint should not be part of the model") {
                    CHECK_THROWS(backend[c]);
                }
            }

        }


    }

}

#endif