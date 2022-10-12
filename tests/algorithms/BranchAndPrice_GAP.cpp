//
// Created by henri on 22/09/22.
//
#include "../test_utils.h"

#include "../instances/generalized-assignment-problem/InstanceGAP_0.h"
#include "../instances/generalized-assignment-problem/InstanceGAP_1.h"
#include "../instances/generalized-assignment-problem/InstanceGAP_2.h"
#include "../instances/generalized-assignment-problem/InstanceGAP_3.h"

using configurations =
        cartesian_product<
                lp_solvers,
                std::tuple<ColumnGenerationBranchingSchemes::SP, ColumnGenerationBranchingSchemes::RMP>,
                std::tuple<BranchingStrategies::MostInfeasible>,
                std::tuple<NodeStrategies::Basic<Nodes::Basic>>,
                std::tuple<ActiveNodesManagers::Heap>,
                std::tuple<NodeUpdators::ByBoundVar, NodeUpdators::ByBoundCtr>
        >;


TEMPLATE_LIST_TEST_CASE("BranchAndPrice: GAP", has_lp_solver ? "[column-generation]" : "[.]", configurations) {

    using SolverT             = std::tuple_element_t<0, TestType>;
    using GenerationStrategyT = std::tuple_element_t<1, TestType>;
    using BranchingStrategyT  = std::tuple_element_t<2, TestType>;
    using NodeStrategyT       = std::tuple_element_t<3, TestType>;
    using ActiveNodeManagerT  = std::tuple_element_t<4, TestType>;
    using NodeUpdatorT        = std::tuple_element_t<5, TestType>;

    AbstractInstanceGAP* t_instance = GENERATE(
            new InstanceGAP<0>(),
            new InstanceGAP<1>(),
            new InstanceGAP<2>()
            //,new InstanceGAP<3>()
    );

    const unsigned int n_knapsacks = t_instance->n_knapsacks();
    const unsigned int n_items = t_instance->n_items();
    const auto p = t_instance->p();
    const auto w = t_instance->w();
    const auto c = t_instance->c();

    SECTION("should solve to optimum") {

        std::vector<Var> branching_candidates;

        Model rmp;
        std::vector<Var> alpha;
        alpha.reserve(n_knapsacks);

        // SP
        std::vector<Model> subproblems;
        subproblems.reserve(n_knapsacks);
        std::vector<std::vector<Var>> x(n_knapsacks);

        for (unsigned int i = 0 ; i < n_knapsacks ; ++i) {
            subproblems.emplace_back();

            Constant objective_cost;

            x[i].reserve(n_items);

            for (unsigned int j = 0 ; j < n_items ; ++j) {
                x[i].emplace_back(subproblems.back().add_variable(0., 1., Continuous, 0., "x(" + std::to_string(i) + "," + std::to_string(j) + ")") );

                objective_cost += p[i][j] * !x[i][j];

                branching_candidates.emplace_back(x[i].back());
            }

            Expr expr;
            for (unsigned int j = 0 ; j < n_items ; ++j) {
                expr += w[i][j] * x[i][j];
            }
            subproblems.back().add_constraint(expr <= c[i]);

            alpha.emplace_back( rmp.add_variable(0., 1., Continuous, std::move(objective_cost), "alpha") );

        }

        for (unsigned int j = 0 ; j < n_items ; ++j) {
            Expr expr;
            for (unsigned int i = 0 ; i < n_knapsacks ; ++i) {
                expr += !x[i][j] * alpha[i];
            }
            rmp.add_constraint(expr == 1., "assign(" + std::to_string(j) + ")");
        }

        for (unsigned int i = 0 ; i < n_knapsacks ; ++i) {
            rmp.add_constraint( alpha[i] == 1., "convexity(" + std::to_string(i) + ")" );
        }

        // Algorithm

        auto solver = branch_and_price<
                SolverT,
                SolverT,
                GenerationStrategyT,
                BranchingStrategyT,
                NodeStrategyT,
                ActiveNodeManagerT,
                NodeUpdatorT
        >(rmp, alpha, subproblems, branching_candidates);

        solver.solve();

        CHECK(solver.status() == Optimal);
        CHECK(solver.objective_value() == Catch::Approx(t_instance->optimum()));

    }

    delete t_instance;

}