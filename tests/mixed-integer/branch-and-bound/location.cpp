//
// Created by henri on 22.10.23.
//

#include <catch2/catch_all.hpp>
#include "../../cartesian_product.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/factories/DepthFirst.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/factories/BreadthFirst.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/factories/BestBound.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/factories/WorstBound.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/BranchAndBound.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/MostInfeasible.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/LeastInfeasible.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/UniformlyRandom.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/StrongBranching.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/PseudoCost.h"
#include "idol/modeling.h"
#include "idol/mixed-integer/problems/facility-location-problem/FLP_Instance.h"
#include "idol/mixed-integer/optimizers/wrappers/GLPK/GLPK.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"
#include "idol/mixed-integer/optimizers/wrappers/HiGHS/HiGHS.h"
#include "idol/mixed-integer/optimizers/wrappers/Osi/Osi.h"
#include "idol/mixed-integer/optimizers/callbacks/ReducedCostFixing.h"

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

TEMPLATE_LIST_TEST_CASE("Solve Facility Location Problem instances with different node selection rules and branching rules",
                        "[branch-and-bound][location]",
                        test_parameters) {

    using NodeSelectionRuleT = std::tuple_element_t<0, TestType>;
    using BranchingRuleT = std::tuple_element_t<1, TestType>;

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
    const auto subtree_depth = GENERATE(0, 1);

    // Read instance
    const auto instance = read_instance_1991_Cornuejols_et_al("../../data/facility-location-problem/" + filename);
    const unsigned int n_customers = instance.n_customers();
    const unsigned int n_facilities = instance.n_facilities();

    // Make model
    auto x = Var::make_vector(env, Dim<1>(n_facilities), 0., 1., Binary, 0., "x");
    auto y = Var::make_vector(env, Dim<2>(n_facilities, n_customers), 0., 1., Continuous, 0., "y");

    Model model(env);

    model.add_vector<Var, 1>(x);
    model.add_vector<Var, 2>(y);

    for (unsigned int i = 0 ; i < n_facilities ; ++i) {
        model.add(Ctr(env, idol_Sum(j, Range(n_customers), instance.demand(j) * y[i][j]) <= instance.capacity(i) * x[i]));
    }

    for (unsigned int j = 0 ; j < n_customers ; ++j) {
        model.add(Ctr(env, idol_Sum(i, Range(n_facilities), y[i][j]) == 1));
    }

    model.set_obj_expr(idol_Sum(i, Range(n_facilities), instance.fixed_cost(i) * x[i] + idol_Sum(j, Range(n_customers),
                                                                                                 instance.per_unit_transportation_cost(
                                                                                                         i, j) *
                                                                                                 instance.demand(j) *
                                                                                                 y[i][j])));

    // Set backend options
    model.use(
            BranchAndBound<DefaultNodeInfo>()
                    .with_node_optimizer(OPTIMIZER::ContinuousRelaxation())
                    .with_branching_rule(BranchingRuleT())
                    .with_node_selection_rule(NodeSelectionRuleT())
                    .add_callback(ReducedCostFixing())
                    .with_subtree_depth(subtree_depth)
    );

    WHEN("The instance \"" + filename + "\" is solved") {

        model.optimize();

        THEN("The solution status is Optimal") {

            CHECK(model.get_status() == Optimal);

        }

        AND_THEN("The objective value is " + std::to_string(objective_value)) {

            CHECK(model.get_best_obj() == Catch::Approx(objective_value));

        }

    }

}
