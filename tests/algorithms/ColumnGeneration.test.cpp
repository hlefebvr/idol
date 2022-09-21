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
#include "algorithms/branch-and-cut-and-price/DantzigWolfe_RMP_Strategy.h"
#include "algorithms/branch-and-cut-and-price/DantzigWolfe_SP_Strategy.h"

TEMPLATE_LIST_TEST_CASE("ColumnGeneration", "[column-generation][algorithms]", available_solvers) {

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

                BranchAndBound solver;
                solver.set_node_strategy<NodeByBoundStrategy>();
                solver.set_branching_strategy<MostInfeasible>(std::vector<Var> { x_0, x_1 });

                auto& decomposition = solver.set_solution_strategy<DecompositionStrategy>();
                auto& rmp_solver = decomposition.template set_rmp_solution_strategy<ExternalSolverStrategy<TestType>>(rmp);
                auto& generation = decomposition.template add_generation_strategy<ColumnGenerationStrategy>();
                auto& subproblem = generation.add_subproblem();
                auto& sp_solver = subproblem.template set_solution_strategy<ExternalSolverStrategy<TestType>>(sp);
                auto& generator = subproblem.template set_generation_strategy<ColumnGenerator>(rmp, sp);
                generator.set(ctr_rmp, -2. * x_0 + 2. * x_1);
                generator.set(ctr_con, Expr(), 1.);

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

                BranchAndBound solver;
                solver.set_node_strategy<NodeByBoundStrategy>();
                solver.set_branching_strategy<MostInfeasible>(std::vector<Var> { x_0, x_1 });

                auto& decomposition = solver.set_solution_strategy<DecompositionStrategy>();
                auto& rmp_solver = decomposition.template set_rmp_solution_strategy<ExternalSolverStrategy<TestType>>(rmp);
                auto& generation = decomposition.template add_generation_strategy<ColumnGenerationStrategy>();
                auto& subproblem = generation.add_subproblem();
                auto& sp_solver = subproblem.template set_solution_strategy<ExternalSolverStrategy<TestType>>(sp);
                auto& generator = subproblem.template set_generation_strategy<DantzigWolfe_SP_Strategy>(rmp, sp);

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

            BranchAndBound solver;
            solver.set_node_strategy<NodeByBoundStrategy>();
            solver.set_branching_strategy<MostInfeasible>(std::vector<Var> { x_0, x_1 });

            auto& decomposition = solver.set_solution_strategy<DecompositionStrategy>();
            auto& rmp_solver = decomposition.template set_rmp_solution_strategy<ExternalSolverStrategy<TestType>>(rmp);
            auto& generation = decomposition.template add_generation_strategy<ColumnGenerationStrategy>();
            auto& subproblem = generation.add_subproblem();
            auto& sp_solver = subproblem.template set_solution_strategy<ExternalSolverStrategy<TestType>>(sp);
            auto& generator = subproblem.template set_generation_strategy<DantzigWolfe_RMP_Strategy>(rmp, sp);

            solver.solve();

            CHECK(solver.status() == Optimal);
            CHECK(solver.objective_value() == -3._a);

        }



    }

}