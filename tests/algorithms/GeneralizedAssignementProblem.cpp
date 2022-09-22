//
// Created by henri on 22/09/22.
//
#include "../test_utils.h"

using configurations =
        cartesian_product<
                available_solvers,
                std::tuple<DantzigWolfe_RMP_Strategy, DantzigWolfe_SP_Strategy>,
                std::tuple<MostInfeasible>,
                std::tuple<NodeStrategy<NodeByBound>>,
                std::tuple<ActiveNodeManager_Heap>,
                std::tuple<NodeUpdatorByBound, NodeUpdatorByBoundCtr>
        >;

TEMPLATE_LIST_TEST_CASE("Dantzig-Wolfe", "[column-generation]", configurations) {

    using SolverT             = std::tuple_element_t<0, TestType>;
    using GenerationStrategyT = std::tuple_element_t<1, TestType>;
    using BranchingStrategyT  = std::tuple_element_t<2, TestType>;
    using NodeStrategyT       = std::tuple_element_t<3, TestType>;
    using ActiveNodeManagerT  = std::tuple_element_t<4, TestType>;
    using NodeUpdatorT        = std::tuple_element_t<5, TestType>;

    SECTION("Generalized Assignment Problem") {


        /* INSTANCE */
        const unsigned int n_knapsacks = 3;
        const unsigned int n_items = 8;

        const std::vector<std::vector<double>> p = {
                { 27, 12, 12, 16, 24, 31, 41, 13 },
                { 14,  5, 37,  9, 36, 25,  1, 35 },
                { 34, 34, 20,  9, 19, 19,  3, 24 }
        };

        const std::vector<std::vector<double>> w = {
                { 21, 13,  9,  5,  7, 15,  5, 24 },
                { 20,  8, 18, 25,  6,  6,  9,  6 },
                { 16, 16, 18, 24, 11, 11, 16, 18 }
        };

        const std::vector<double> c = { 26, 25, 34 };

        /* MODEL */

        Env env;

        std::vector<Var> branching_candidates;

        // SP
        std::vector<Model> subproblems;
        subproblems.reserve(n_knapsacks);
        std::vector<std::vector<Var>> x(n_knapsacks);

        for (unsigned int i = 0 ; i < n_knapsacks ; ++i) {
            subproblems.emplace_back(env);

            x[i].reserve(n_items);

            for (unsigned int j = 0 ; j < n_items ; ++j) {
                x[i].emplace_back(subproblems.back().add_variable(0., 1., Binary, -p[i][j], "x(" + std::to_string(i) + "," + std::to_string(j) + ")") );

                branching_candidates.emplace_back(x[i].back());
            }

            Expr expr;
            for (unsigned int j = 0 ; j < n_items ; ++j) {
                expr += w[i][j] * x[i][j];
            }
            subproblems.back().add_constraint(expr <= c[i]);

        }

        // RMP
        Model rmp(env);

        std::vector<std::vector<Param>> param_x(n_knapsacks);

        for (unsigned int i = 0 ; i < n_knapsacks ; ++i) {
            param_x[i].reserve(n_items);
            for (unsigned int j = 0 ; j < n_items ; ++j) {
                param_x[i].emplace_back( rmp.add_parameter(x[i][j]) );
            }
        };

        for (unsigned int j = 0 ; j < n_items ; ++j) {
            Coefficient expr = 1;
            for (unsigned int i = 0 ; i < n_knapsacks ; ++i) {
                expr += -1. * param_x[i][j];
            }
            rmp.add_constraint(Expr() == expr, "assign(" + std::to_string(j) + ")");
        }

        auto solver = branch_and_price<
                ExternalSolverStrategy<SolverT>,
                ExternalSolverStrategy<SolverT>,
                GenerationStrategyT,
                BranchingStrategyT,
                NodeStrategyT,
                ActiveNodeManagerT,
                NodeUpdatorT
        >(rmp, subproblems.begin(), subproblems.end(), branching_candidates);

        solver.solve();

        CHECK(solver.status() == Optimal);
        CHECK(solver.objective_value() == -233._a);

    }

}
