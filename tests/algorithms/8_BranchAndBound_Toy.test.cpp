//
// Created by henri on 15/09/22.
//

#include "../test_utils.h"

using configurations =
        cartesian_product<
                lp_solvers,
                std::tuple<BranchingStrategies::MostInfeasible>,
                std::tuple<NodeStrategies::Basic<Nodes::Basic>>,
                std::tuple<ActiveNodesManagers::Basic>,
                std::tuple<NodeUpdators::ByBoundVar, NodeUpdators::ByBoundCtr>
            >;

TEMPLATE_LIST_TEST_CASE("08. B&B: Toy", has_lp_solver ? "[MILP][branch-and-bound][algorithms]" : "[.]", configurations) {

    using SolverT             = std::tuple_element_t<0, TestType>;
    using BranchingStrategyT  = std::tuple_element_t<1, TestType>;
    using NodeStrategyT       = std::tuple_element_t<2, TestType>;
    using ActiveNodeManagerT  = std::tuple_element_t<3, TestType>;
    using NodeUpdatorT        = std::tuple_element_t<4, TestType>;

    Model model;

    SECTION("solving bounded feasible MILP") {

        SECTION("solved at root node") {
            // Taken from https://www.gurobi.com/documentation/9.5/examples/mip1_cpp_cpp.html#subsubsection:mip1_c++.cpp

            auto x = model.add_var(0., 1., Continuous, -1, "x");
            auto y = model.add_var(0., 1., Continuous, -1, "y");
            auto z = model.add_var(0., 1., Continuous, -2, "z");
            auto c1 = model.add_ctr(x + 2 * y + 3 * z <= 4);
            auto c2 = model.add_ctr(x + y >= 1);

            auto solver = branch_and_bound<
                    SolverT,
                    BranchingStrategyT,
                    NodeStrategyT,
                    ActiveNodeManagerT,
                    NodeUpdatorT
                >(model, { x, y, z });
            solver.solve();

            CHECK(solver.n_created_nodes() == 1);
            CHECK(solver.status() == Optimal);
            CHECK(solver.objective_value() == -3._a);

            const auto primal_solution = solver.primal_solution();

            CHECK(primal_solution.get(x) == 1._a);
            CHECK(primal_solution.get(y) == 0._a);
            CHECK(primal_solution.get(z) == 1._a);
        }

        SECTION("solved at fist level") {

            auto x = model.add_var(0., 1., Continuous, -1, "x");
            auto y = model.add_var(0., 1., Continuous, -1, "y");
            auto z = model.add_var(0., 1., Continuous, -2, "z");
            auto c1 = model.add_ctr(x + 2 * y + 2.5 * z <= 4);
            auto c2 = model.add_ctr(x + y >= 1);

            auto solver = branch_and_bound<
                                SolverT,
                                BranchingStrategyT,
                                NodeStrategyT,
                                ActiveNodeManagerT,
                                NodeUpdatorT
                        >(model, { x, y, z });
            solver.solve();

            CHECK(solver.n_created_nodes() == 3);
            CHECK(solver.status() == Optimal);
            CHECK(solver.objective_value() == -3._a);

            const auto primal_solution = solver.primal_solution();

            CHECK(primal_solution.get(x) == 1._a);
            CHECK(primal_solution.get(y) == 0._a);
            CHECK(primal_solution.get(z) == 1._a);
        }

        SECTION("knapsack") {
            // Taken from https://www.geeksforgeeks.org/implementation-of-0-1-knapsack-using-branch-and-bound/

            const std::vector<std::pair<double, double>> items = { {2, 40}, {3.14, 50}, {1.98, 100}, {5, 95}, {3, 30} }; // (weight, value)
            const double capacity = 10.;

            std::vector<Var> x;
            x.reserve(items.size());

            LinExpr sum_weight;

            for (const auto& [weight, profit] : items) {
                auto var = model.add_var(0., 1., Continuous, -profit);
                sum_weight += weight * var;
                x.emplace_back(var);
            }

            model.add_ctr(sum_weight <= capacity);

            auto solver = branch_and_bound<
                                SolverT,
                                BranchingStrategyT,
                                NodeStrategyT,
                                ActiveNodeManagerT,
                                NodeUpdatorT
                        >(model, x);
            solver.solve();

            CHECK(solver.status() == Optimal);
            CHECK(solver.objective_value() == -235._a);

            const auto primal_solution = solver.primal_solution();

            CHECK(primal_solution.get(x[0]) == 1._a);
            CHECK(primal_solution.get(x[1]) == 0._a);
            CHECK(primal_solution.get(x[2]) == 1._a);
            CHECK(primal_solution.get(x[3]) == 1._a);
            CHECK(primal_solution.get(x[4]) == 0._a);
        }

    }

    SECTION("Solving infeasible MILP with infeasible root node") {

        SECTION("infeasible at root node") {

            auto x = model.add_var(0., 1., Continuous, 0.);
            model.add_ctr(x >= 1);
            model.add_ctr(x <= 0);

            auto solver = branch_and_bound<
                                SolverT,
                                BranchingStrategyT,
                                NodeStrategyT,
                                ActiveNodeManagerT,
                                NodeUpdatorT
                        >(model, { x });
            solver.solve();

            CHECK(solver.status() == Infeasible);
            CHECK(is_pos_inf(solver.objective_value()));
        }

        SECTION("infeasible at first level") {
            auto x = model.add_var(0., 1., Continuous, 0.);
            model.add_ctr(x >= .1);
            model.add_ctr(x <= .9);

            auto solver = branch_and_bound<
                                SolverT,
                                BranchingStrategyT,
                                NodeStrategyT,
                                ActiveNodeManagerT,
                                NodeUpdatorT
                        >(model, { x });
            solver.solve();


            CHECK(solver.status() == Infeasible);
            CHECK(is_pos_inf(solver.objective_value()));
        }

    }

    SECTION("Solving an unbounded MILP") {

        auto x = model.add_var(-Inf, Inf, Continuous, -1.);

        auto solver = branch_and_bound<
                            SolverT,
                            BranchingStrategyT,
                            NodeStrategyT,
                            ActiveNodeManagerT,
                            NodeUpdatorT
                    >(model, { x });
        solver.solve();

        CHECK(solver.status() == Unbounded);
        CHECK(is_neg_inf(solver.objective_value()));

    }

}