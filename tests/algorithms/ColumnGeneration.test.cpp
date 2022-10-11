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
#include "algorithms/branch-and-bound/NodeUpdators_ByBoundVar.h"
#include "algorithms/column-generation/ColumnGenerationBranchingSchemes_RMP.h"
#include "algorithms/column-generation/ColumnGenerationBranchingSchemes_SP.h"

TEMPLATE_LIST_TEST_CASE("ColumnGeneration", "[generation-strategies][algorithms]", std::tuple<Gurobi>) {

    Model model;

    SECTION("Branching on SP") {

        Model sp;
        auto x_0 = sp.add_variable(0., 10., Continuous, 0., "x_0");
        auto x_1 = sp.add_variable(0., 10., Continuous, 0., "x_1");
        auto sp_ctr = sp.add_constraint(-8 * x_0 + 10. * x_1 <= 13.);

        Model rmp;
        auto alpha = rmp.add_variable(0., 1., Continuous, -1 * !x_0 + -1 * !x_1, "alpha");
        auto ctr_rmp = rmp.add_constraint( (-2 * !x_0 + 2 * !x_1) * alpha >= 1., "rmp_ctr");
        auto ctr_con = rmp.add_constraint( alpha == 1 , "rmp_convex");

        BranchAndBound solver;
        auto& node_strategy = solver.set_node_strategy<NodeStrategies::Basic<Nodes::Basic>>();
        node_strategy.set_active_node_manager_strategy<ActiveNodesManagers::Heap>();
        node_strategy.set_node_updator_strategy<NodeUpdators::ByBoundVar>();
        node_strategy.set_branching_strategy<BranchingStrategies::MostInfeasible>(std::vector<Var> {x_0, x_1 });

        auto& decomposition = solver.set_solution_strategy<Decomposition>();
        decomposition.set_rmp_solution_strategy<ExternalSolver<TestType>>(rmp);
        auto& generation = decomposition.add_generation_strategy<ColumnGeneration>();
        auto& subproblem = generation.add_subproblem(alpha);
        subproblem.set_solution_strategy<ExternalSolver<TestType>>(sp);
        auto& generator = subproblem.set_branching_scheme<ColumnGenerationBranchingSchemes::SP>();

        solver.solve();

        CHECK(solver.status() == Optimal);
        CHECK(solver.objective_value() == -3._a);

    }

    SECTION("Branching on RMP") {

        Model sp;
        auto x_0 = sp.add_variable(0., 10., Continuous, 0., "x_0");
        auto x_1 = sp.add_variable(0., 10., Continuous, 0., "x_1");
        auto sp_ctr = sp.add_constraint(-8 * x_0 + 10. * x_1 <= 13.);

        Model rmp;
        auto alpha = rmp.add_variable(0., 1., Continuous, -1 * !x_0 + -1 * !x_1, "alpha");
        auto ctr_rmp = rmp.add_constraint( (-2 * !x_0 + 2 * !x_1) * alpha >= 1., "rmp_ctr");
        auto ctr_con = rmp.add_constraint( alpha == 1 , "rmp_convex");

        BranchAndBound solver;
        auto& node_strategy = solver.set_node_strategy<NodeStrategies::Basic<Nodes::Basic>>();
        node_strategy.set_active_node_manager_strategy<ActiveNodesManagers::Heap>();
        node_strategy.set_node_updator_strategy<NodeUpdators::ByBoundVar>();
        node_strategy.set_branching_strategy<BranchingStrategies::MostInfeasible>(std::vector<Var> {x_0, x_1 });

        auto& decomposition = solver.set_solution_strategy<Decomposition>();
        decomposition.set_rmp_solution_strategy<ExternalSolver<TestType>>(rmp);
        auto& generation = decomposition.add_generation_strategy<ColumnGeneration>();
        auto& subproblem = generation.add_subproblem(alpha);
        subproblem.set_solution_strategy<ExternalSolver<TestType>>(sp);
        auto& generator = subproblem.set_branching_scheme<ColumnGenerationBranchingSchemes::RMP>();

        solver.solve();

        CHECK(solver.status() == Optimal);
        CHECK(solver.objective_value() == -3._a);

    }

}