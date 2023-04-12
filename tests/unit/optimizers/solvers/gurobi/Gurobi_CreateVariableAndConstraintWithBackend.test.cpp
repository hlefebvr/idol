//
// Created by henri on 31/01/23.
//

#include "../../../../test_utils.h"
#include "optimizers/solvers/gurobi/Optimizers_Gurobi.h"
#include "optimizers/solvers/DefaultOptimizer.h"

#ifdef IDOL_USE_GUROBI

SCENARIO("Gurobi: Create variables and constraints intertwined with backend", "[unit][backend][Gurobi]") {

    GIVEN("A model with a variable y, a constraint c1 (y >= 2), and an initialized Gurobi backend") {

        Env env;
        Model model(env);

        model.use(Gurobi());

        const auto& backend = ((const Model &) model).optimizer().as<Optimizers::Gurobi>();

        Var y(env, 0., 1., Continuous, "y");
        model.add(y);

        Ctr c1(env, y >= 2, "c1");
        model.add(c1);

        model.update();

        WHEN("A variable x and constraint c2 (2y + x <= 10) are added to the model and the variable appears in the constraint") {

            Var x(env, 0., 1., Continuous, "x");
            model.add(x);

            Ctr c2(env, 2 * y + x <= 10, "c2");
            model.add(c2);

            WHEN("Model::update_objective() is called") {

                model.update();

                THEN("The variable has been added") {
                    CHECK_NOTHROW(backend[x]);
                }

                AND_THEN("The constraint has been added") {
                    CHECK_NOTHROW(backend[c2]);
                }

                AND_THEN("The x-variable's coefficient in the c2-constraint should be 1") {
                    const auto& gurobi_ctr = std::get<GRBConstr>(backend[c2]);
                    const auto& gurobi_var = backend[x];
                    CHECK_NOTHROW(backend.model().getCoeff(gurobi_ctr, gurobi_var) == 1._a);
                }

                AND_THEN("The y-variable's coefficient in the c2-constraint should be 2") {
                    const auto& gurobi_ctr = std::get<GRBConstr>(backend[c2]);
                    const auto& gurobi_var = backend[y];
                    CHECK_NOTHROW(backend.model().getCoeff(gurobi_ctr, gurobi_var) == 2._a);
                }

                AND_THEN("The y-variable's coefficient in the c1-constraint should be 1") {
                    const auto& gurobi_ctr = std::get<GRBConstr>(backend[c1]);
                    const auto& gurobi_var = backend[y];
                    CHECK_NOTHROW(backend.model().getCoeff(gurobi_ctr, gurobi_var) == 1._a);
                }
            }

        }


        WHEN("A variable x and constraint c2 are added to the model and the constraint appears in the variable") {

            Ctr c2(env, LessOrEqual, 10, "c");
            model.add(c2);

            Column column;
            column.linear().set(c2, 1);

            Var x(env, 0., 1., Continuous, std::move(column), "x");
            model.add(x);

            WHEN("Model::update_objective() is called") {

                model.update();

                THEN("The variable has been added") {
                    CHECK_NOTHROW(backend[x]);
                }

                AND_THEN("The constraint has been added") {
                    CHECK_NOTHROW(backend[c2]);
                }

                AND_THEN("The variable's coefficient in the constraint should be 1") {
                    const auto& gurobi_ctr = std::get<GRBConstr>(backend[c2]);
                    const auto& gurobi_var = backend[x];
                    CHECK_NOTHROW(backend.model().getCoeff(gurobi_ctr, gurobi_var) == 1._a);
                }

            }

        }

    }

}

#endif