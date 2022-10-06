//
// Created by henri on 19/09/22.
//

#include "../test_utils.h"
#include "algorithms/branch-and-bound/BranchAndBound.h"
#include "algorithms/branch-and-bound/NodeStrategies_Basic.h"
#include "algorithms/column-generation/ColumnGeneration.h"
#include "algorithms/decomposition/Decomposition.h"
#include "algorithms/branch-and-bound/BranchingStrategies_MostInfeasible.h"
#include "algorithms/branch-and-bound/Nodes_Basic.h"
#include "algorithms/branch-and-bound/NodeUpdators_ByBound.h"
#include "algorithms/column-generation/ColumnGenerators_DantzigWolfeSP.h"
#include "algorithms/column-generation/ColumnGenerators_DantzigWolfeRMP.h"
#include "algorithms/column-generation/ColumnGenerators_Basic.h"

TEMPLATE_LIST_TEST_CASE("ColumnGeneration", "[generation-strategies][algorithms]", available_solvers) {

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
                auto& node_strategy = solver.set_node_strategy<NodeStrategies::Basic<Nodes::Basic>>();
                node_strategy.set_active_node_manager_strategy<ActiveNodesManagers::Heap>();
                node_strategy.set_node_updator_strategy<NodeUpdators::ByBoundVar>();
                node_strategy.set_branching_strategy<BranchingStrategies::BranchingStrategies_MostInfeasible>(std::vector<Var> {x_0, x_1 });

                auto& decomposition = solver.set_solution_strategy<Decomposition>();
                auto& rmp_solver = decomposition.template set_rmp_solution_strategy<ExternalSolver<TestType>>(rmp);
                auto& generation = decomposition.template add_generation_strategy<ColumnGeneration>();
                auto& subproblem = generation.add_subproblem();
                auto& sp_solver = subproblem.template set_solution_strategy<ExternalSolver<TestType>>(sp);
                auto& generator = subproblem.template set_generation_strategy<ColumnGenerators::Basic>(rmp, sp);
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
                auto& node_strategy = solver.set_node_strategy<NodeStrategies::Basic<Nodes::Basic>>();
                node_strategy.set_active_node_manager_strategy<ActiveNodesManagers::Heap>();
                node_strategy.set_node_updator_strategy<NodeUpdators::ByBoundVar>();
                node_strategy.set_branching_strategy<BranchingStrategies::BranchingStrategies_MostInfeasible>(std::vector<Var> {x_0, x_1 });

                auto& decomposition = solver.set_solution_strategy<Decomposition>();
                auto& rmp_solver = decomposition.template set_rmp_solution_strategy<ExternalSolver<TestType>>(rmp);
                auto& generation = decomposition.template add_generation_strategy<ColumnGeneration>();
                auto& subproblem = generation.add_subproblem();
                auto& sp_solver = subproblem.template set_solution_strategy<ExternalSolver<TestType>>(sp);
                auto& generator = subproblem.template set_generation_strategy<ColumnGenerators::DantzigWolfeSP>(rmp, sp);

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
            auto& node_strategy = solver.set_node_strategy<NodeStrategies::Basic<Nodes::Basic>>();
            node_strategy.set_active_node_manager_strategy<ActiveNodesManagers::Heap>();
            node_strategy.set_node_updator_strategy<NodeUpdators::ByBoundVar>();
            node_strategy.set_branching_strategy<BranchingStrategies::BranchingStrategies_MostInfeasible>(std::vector<Var> {x_0, x_1 });

            auto& decomposition = solver.set_solution_strategy<Decomposition>();
            auto& rmp_solver = decomposition.template set_rmp_solution_strategy<ExternalSolver<TestType>>(rmp);
            auto& generation = decomposition.template add_generation_strategy<ColumnGeneration>();
            auto& subproblem = generation.add_subproblem();
            auto& sp_solver = subproblem.template set_solution_strategy<ExternalSolver<TestType>>(sp);
            auto& generator = subproblem.template set_generation_strategy<ColumnGenerators::DantzigWolfeRMP>(rmp, sp);

            solver.solve();

            CHECK(solver.status() == Optimal);
            CHECK(solver.objective_value() == -3._a);

        }



    }

}