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
#include "optimizers/solvers/DefaultOptimizer.h"
#include "optimizers/branch-and-bound/branching-rules/factories/MostInfeasible.h"

using node_selection_rules = std::tuple<DepthFirst, BreadthFirst, BestBound, WorstBound>;

using test_parameters = cartesian_product<lp_solvers, node_selection_rules>;

TEMPLATE_LIST_TEST_CASE("BranchAndBound: solve Knapsack Problem with different node selection strategies",
                        "[integration][optimizer][knapsack]",
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

    auto x = Var::make_vector(env, Dim<1>(n_items), 0., 1., Binary, "x");
    Ctr c(env, idol_Sum(j, Range(n_items), instance.weight(j) * x[j]) <= instance.capacity());

    Model model(env);
    model.add_vector<Var, 1>(x);
    model.add(c);
    model.set_obj_expr(idol_Sum(j, Range(n_items), -instance.profit(j) * x[j]));

    model.use(BranchAndBound<NodeVarInfo>()
                .with_node_optimizer(OptimizerT::ContinuousRelaxation())
                .with_branching_rule(MostInfeasible())
                .with_node_selection_rule(NodeSelectionRuleT())
                .with_subtree_depth(subtree_depth)
            );

    WHEN("The instance \"" + filename + "\" is solved with subtree depth of " + std::to_string(subtree_depth)) {

        model.optimize();

        THEN("The solution status is Optimal") {

            CHECK(model.get_status() == Optimal);

        }

        AND_THEN("The objective value is " + std::to_string(objective_value)) {

            CHECK(model.get_best_obj() == objective_value);

        }

    }


}
