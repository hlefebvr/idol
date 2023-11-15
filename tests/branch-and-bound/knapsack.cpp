//
// Created by henri on 22.10.23.
//

#include <catch2/catch_all.hpp>
#include "../cartesian_product.h"
#include "idol/optimizers/branch-and-bound/node-selection-rules/factories/DepthFirst.h"
#include "idol/optimizers/branch-and-bound/node-selection-rules/factories/BreadthFirst.h"
#include "idol/optimizers/branch-and-bound/node-selection-rules/factories/BestBound.h"
#include "idol/optimizers/branch-and-bound/node-selection-rules/factories/WorstBound.h"
#include "idol/optimizers/branch-and-bound/BranchAndBound.h"
#include "idol/optimizers/branch-and-bound/branching-rules/factories/MostInfeasible.h"
#include "idol/optimizers/branch-and-bound/branching-rules/factories/LeastInfeasible.h"
#include "idol/optimizers/branch-and-bound/branching-rules/factories/UniformlyRandom.h"
#include "idol/optimizers/branch-and-bound/branching-rules/factories/StrongBranching.h"
#include "idol/optimizers/branch-and-bound/branching-rules/factories/PseudoCost.h"
#include <idol/modeling.h>
#include <idol/problems/knapsack-problem/KP_Instance.h>
#include <idol/optimizers/wrappers/GLPK/GLPK.h>
#include <idol/optimizers/wrappers/Mosek/Mosek.h>
#include <idol/optimizers/wrappers/Gurobi/Gurobi.h>
#include <idol/optimizers/wrappers/HiGHS/HiGHS.h>

using namespace Catch::literals;
using namespace idol;

using node_selection_rules = std::tuple<
        DepthFirst,
        BreadthFirst,
        BestBound,
        WorstBound
>;

using branching_rules = std::tuple<
        MostInfeasible::Strategy<DefaultNodeInfo>,
        LeastInfeasible::Strategy<DefaultNodeInfo>,
        UniformlyRandom::Strategy<DefaultNodeInfo>,
        StrongBranching::Strategy<DefaultNodeInfo>,
        PseudoCost::Strategy<DefaultNodeInfo>
>;

using test_parameters = cartesian_product<node_selection_rules, branching_rules>;

TEMPLATE_LIST_TEST_CASE("Solve Knapsack Problem instances with different node selection rules and branching rules",
                        "[branch-and-bound][knapsack]",
                        test_parameters) {

    using NodeSelectionRuleT = std::tuple_element_t<0, TestType>;
    using BranchingRuleT = std::tuple_element_t<1, TestType>;

    Env env;

    using namespace idol::Problems::KP;

    const auto [filename, objective_value] = GENERATE(
            std::make_pair<std::string, double>("s000.kp", -828.),
            std::make_pair<std::string, double>("s001.kp", -6635.),
            std::make_pair<std::string, double>("s002.kp", -25468.)
    );
    const auto subtree_depth = GENERATE(0, 1);

    const auto instance = read_instance("../data/knapsack-problem/" + filename);
    const unsigned int n_items = instance.n_items();

    auto x = Var::make_vector(env, Dim<1>(n_items), 0., 1., Binary, "x");
    Ctr c(env, idol_Sum(j, Range(n_items), instance.weight(j) * x[j]) <= instance.capacity());

    Model model(env);
    model.add_vector<Var, 1>(x);
    model.add(c);
    model.set_obj_expr(idol_Sum(j, Range(n_items), -instance.profit(j) * x[j]));

    model.use(BranchAndBound<DefaultNodeInfo>()
                      .with_node_optimizer(OPTIMIZER::ContinuousRelaxation())
                      .with_branching_rule(BranchingRuleT())
                      .with_node_selection_rule(NodeSelectionRuleT())
                      .with_subtree_depth(subtree_depth)
    );

    WHEN("The instance \"" + filename + "\" is solved with subtree depth of " + std::to_string(subtree_depth)) {

        model.optimize();

        THEN("The solution status is Optimal and the objective value is " + std::to_string(objective_value)) {

            CHECK(model.get_status() == Optimal);
            CHECK(model.get_best_obj() == objective_value);

        }

    }


}
