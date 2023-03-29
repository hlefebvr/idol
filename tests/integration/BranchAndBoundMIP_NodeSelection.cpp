//
// Created by henri on 06/02/23.
//

#include "../test_utils.h"
#include "problems/knapsack-problem/KP_Instance.h"
#include "problems/facility-location-problem/FLP_Instance.h"
#include "optimizers/branch-and-bound/node-selection-rules/factories/DepthFirst.h"
#include "optimizers/branch-and-bound/node-selection-rules/factories/BreadthFirst.h"
#include "optimizers/branch-and-bound/node-selection-rules/factories/BestBound.h"
#include "optimizers/branch-and-bound/node-selection-rules/factories/WorstBound.h"
#include "optimizers/branch-and-bound/BranchAndBound.h"
#include "optimizers/branch-and-bound/branching-rules/factories/MostInfeasible.h"
#include "optimizers/solvers/DefaultOptimizer.h"

using node_selection_rules = std::tuple<DepthFirst, BreadthFirst, BestBound, WorstBound>;

using test_parameters = cartesian_product<lp_solvers, node_selection_rules>;

TEMPLATE_LIST_TEST_CASE("BranchAndBoundMIP: solve Knapsack Problem with different node selection strategies",
                        "[integration][backend][solver]",
                        test_parameters) {

    using OptimizerT = std::tuple_element_t<0, TestType>;
    using NodeSelectionRuleT = std::tuple_element_t<1, TestType>;

    Env env;

    using namespace Problems::KP;

    const auto [filename, objective_value] = GENERATE(
            std::make_pair<std::string, double>("KP_instance0.txt", -235.)
    );
    const auto subtree_depth = GENERATE(0, 2);

    const auto instance = read_instance("instances/knapsack-problem/" + filename);
    const unsigned int n_items = instance.n_items();

    auto x = Var::array(env, Dim<1>(n_items), 0., 1., Binary, "x");
    Ctr c(env, idol_Sum(j, Range(n_items), instance.weight(j) * x[j]) <= instance.capacity());

    Model model(env);
    model.add_array<Var, 1>(x);
    model.add(c);
    model.set(Attr::Obj::Expr, idol_Sum(j, Range(n_items), -instance.profit(j) * x[j]));

    model.use(BranchAndBound<NodeInfo>()
                .with_node_solver(OptimizerT::ContinuousRelaxation())
                .with_branching_rule(MostInfeasible())
                .with_node_selection_rule(NodeSelectionRuleT())
                .with_subtree_depth(subtree_depth)
            );

    WHEN("The instance \"" + filename + "\" is solved with subtree depth of " + std::to_string(subtree_depth)) {

        model.optimize();

        THEN("The solution status is Optimal") {

            CHECK(model.get(Attr::Solution::Status) == Optimal);

        }

        AND_THEN("The objective value is " + std::to_string(objective_value)) {

            CHECK(model.get(Attr::Solution::ObjVal) == objective_value);

        }

    }


}


TEMPLATE_LIST_TEST_CASE("BranchAndBoundMIP: solve Facility Location Problem with different node selection strategies",
                        "[integration][backend][solver]",
                        test_parameters) {

    using OptimizerT = std::tuple_element_t<0, TestType>;
    using NodeSelectionRuleT = std::tuple_element_t<1, TestType>;

    Env env;

    using namespace Problems::FLP;

    // Generate parameters
    const auto [filename, objective_value] = GENERATE(
            std::make_pair<std::string, double>("instance_F10_C20__0.txt", 235.114),
            std::make_pair<std::string, double>("instance_F10_C20__1.txt", 245.29),
            std::make_pair<std::string, double>("instance_F10_C20__2.txt", 287.37),
            std::make_pair<std::string, double>("instance_F10_C20__3.txt", 333.86),
            std::make_pair<std::string, double>("instance_F10_C20__4.txt", 202.11)
    );

    // Read instance
    const auto instance = read_instance_1991_Cornuejols_et_al("instances/facility-location-problem/" + filename);
    const unsigned int n_customers = instance.n_customers();
    const unsigned int n_facilities = instance.n_facilities();

    // Make model
    auto x = Var::array(env, Dim<1>(n_facilities), 0., 1., Binary, "x");
    auto y = Var::array(env, Dim<2>(n_facilities, n_customers), 0., 1., Continuous, "y");

    Model model(env);

    model.add_array<Var, 1>(x);
    model.add_array<Var, 2>(y);

    for (unsigned int i = 0 ; i < n_facilities ; ++i) {
        model.add(Ctr(env, idol_Sum(j, Range(n_customers), instance.demand(j) * y[i][j]) <= instance.capacity(i) * x[i]));
    }

    for (unsigned int j = 0 ; j < n_customers ; ++j) {
        model.add(Ctr(env, idol_Sum(i, Range(n_facilities), y[i][j]) == 1));
    }

    model.set(Attr::Obj::Expr, idol_Sum(i, Range(n_facilities), instance.fixed_cost(i) * x[i] + idol_Sum(j, Range(n_customers), instance.per_unit_transportation_cost(i, j) * instance.demand(j) * y[i][j])));

    // Set backend options
    model.use(
            BranchAndBound<NodeInfo>()
                .with_node_solver(OptimizerT::ContinuousRelaxation())
                .with_branching_rule(MostInfeasible())
                .with_node_selection_rule(NodeSelectionRuleT())
        );

    WHEN("The instance \"" + filename + "\" is solved") {

        std::cout << "Solving " << filename << "..." << std::endl;
        model.optimize();
        std::cout << "Done." << std::endl;

        THEN("The solution status is Optimal") {

            CHECK(model.get(Attr::Solution::Status) == Optimal);

        }

        AND_THEN("The objective value is " + std::to_string(objective_value)) {

            CHECK(model.get(Attr::Solution::ObjVal) == Catch::Approx(objective_value));

        }

    }


}