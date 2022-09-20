//
// Created by henri on 19/09/22.
//

#include "../test_utils.h"
#include "algorithms/branch-and-bound/BranchAndBound.h"
#include "algorithms/branch-and-cut-and-price/ColumnGenerator.h"
#include "algorithms/branch-and-bound/MostInfeasible.h"
#include "algorithms/branch-and-bound/NodeByBoundStrategy.h"
#include "algorithms/branch-and-cut-and-price/ColumnGenerationStrategy.h"
#include "algorithms/branch-and-cut-and-price/DecompositionStrategy.h"
#include "algorithms/branch-and-cut-and-price/DantzigWolfeGenerator.h"
#include "algorithms/branch-and-cut-and-price/DantzigWolfeGeneratorSP.h"

TEMPLATE_LIST_TEST_CASE("ColumnGeneration", "[column-generation][algorithms]", available_solvers) {

    Log::set_level(Trace);

    Env env;
    Model model(env);

    SECTION("Branching on subproblem") {

        SECTION("with objective cost") {

            SECTION("explicit column generation scheme") {

                Model rmp(env);
                auto ctr_rmp = rmp.add_constraint(GreaterOrEqual, 1., "rmp_ctr");
                auto ctr_con = rmp.add_constraint(Equal, 1);

                Model sp(env);
                auto x_0 = sp.add_variable(0., 10., Continuous, -1., "x_0");
                auto x_1 = sp.add_variable(0., 10., Continuous, -1., "x_1");
                auto sp_ctr = sp.add_constraint(-8 * x_0 + 10. * x_1 <= 13.);

                ColumnGenerator generator(rmp, sp);
                generator.set(ctr_rmp, -2. * x_0 + 2. * x_1);
                generator.set(ctr_con, Expr(), 1.);

                BranchAndBound solver;
                solver.set_node_strategy<NodeByBoundStrategy>();
                solver.set_branching_strategy<MostInfeasible>(std::vector<Var> { x_0, x_1 });

                solver.set_solution_strategy<DecompositionStrategy<Gurobi>>(rmp)
                        .template add_generation_strategy<ColumnGenerationStrategy>()
                        .template add_subproblem<ExternalSolverStrategy<TestType>>(generator, sp);

                solver.solve();

                CHECK(solver.status() == Optimal);
                CHECK(solver.objective_value() == -3._a);

            }

            SECTION("Dantzig-Wolfe") {

                Model sp(env);
                auto x_0 = sp.add_variable(0., 10., Continuous, -1., "x_0");
                auto x_1 = sp.add_variable(0., 10., Continuous, -1., "x_1");
                auto sp_ctr = sp.add_constraint(-8 * x_0 + 10. * x_1 <= 13.);

                Model rmp(env);
                auto x_bar_0 = rmp.add_parameter( x_0 );
                auto x_bar_1 = rmp.add_parameter( x_1 );
                auto ctr_rmp = rmp.add_constraint(Expr() >= 1 + 2. * x_bar_0 + -2 * x_bar_1, "rmp_ctr");

                DantzigWolfeGeneratorSP generator(rmp, sp);

                BranchAndBound solver;
                solver.set_node_strategy<NodeByBoundStrategy>();
                solver.set_branching_strategy<MostInfeasible>(std::vector<Var> { x_0, x_1 });

                solver.set_solution_strategy<DecompositionStrategy<TestType>>(rmp)
                        .template add_generation_strategy<ColumnGenerationStrategy>()
                        .template add_subproblem<ExternalSolverStrategy<TestType>>(generator, sp);

                solver.solve();

                CHECK(solver.status() == Optimal);
                CHECK(solver.objective_value() == -3._a);

            }

        }

    }

    SECTION("Branching on rmp") {

        SECTION("Dantzig-Wolfe") {


            Model sp(env);
            auto x_0 = sp.add_variable(0., 10., Continuous, -1., "x_0");
            auto x_1 = sp.add_variable(0., 10., Continuous, -1., "x_1");
            auto sp_ctr = sp.add_constraint(-8 * x_0 + 10. * x_1 <= 13.);

            Model rmp(env);
            auto x_bar_0 = rmp.add_parameter( x_0 );
            auto x_bar_1 = rmp.add_parameter( x_1 );
            auto ctr_rmp = rmp.add_constraint(Expr() >= 1 + 2. * x_bar_0 + -2 * x_bar_1, "rmp_ctr");

            DantzigWolfeGenerator generator(rmp, sp);

            BranchAndBound solver;
            solver.set_node_strategy<NodeByBoundStrategy>();
            solver.set_branching_strategy<MostInfeasible>(std::vector<Var> { x_0, x_1 });

            solver.set_solution_strategy<DecompositionStrategy<TestType>>(rmp)
                    .template add_generation_strategy<ColumnGenerationStrategy>()
                    .template add_subproblem<ExternalSolverStrategy<TestType>>(generator, sp);

            solver.solve();

            CHECK(solver.status() == Optimal);
            CHECK(solver.objective_value() == -3._a);

        }



    }

}