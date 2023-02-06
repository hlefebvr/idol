//
// Created by henri on 06/02/23.
//

#include "../test_utils.h"
#include "problems/KP/KP_Instance.h"

TEMPLATE_LIST_TEST_CASE("BranchAndBoundMIP: solve Knapsack problem with different node selection strategies",
                        "[integration][backend][solver]",
                        std::tuple<Gurobi>) {

    Env env;

    using namespace Problems::KP;

    const auto [filename, objective_value] = GENERATE(
            std::make_pair<std::string, double>("KP_instance0.txt", -235.)
    );

    const auto instance = read_instance("instances/KP/" + filename);
    const unsigned int n_items = instance.n_items();

    auto node_selection = GENERATE(
            NodeSelections::Automatic,
            NodeSelections::DepthFirst,
            NodeSelections::BreadthFirst,
            NodeSelections::BestBound,
            NodeSelections::WorstBound
    );

    auto x = Var::array(env, Dim<1>(n_items), 0., 1., Binary, "x");
    Ctr c(env, idol_Sum(j, Range(n_items), instance.weight(j) * x[j]) <= instance.capacity());

    Model model(env);
    model.add<Var, 1>(x);
    model.add(c);
    model.set(Attr::Obj::Expr, idol_Sum(j, Range(n_items), -instance.profit(j) * x[j]));

    model.set_backend<BranchAndBoundMIP<TestType>>();
    model.set(Param::BranchAndBound::NodeSelection, node_selection);

    WHEN("The instance \"" + filename + "\" is solved") {

        model.optimize();

        THEN("The solution status is Optimal") {

            CHECK(model.get(Attr::Solution::Status) == Optimal);

        }

        AND_THEN("The objective value is " + std::to_string(objective_value)) {

            CHECK(model.get(Attr::Solution::ObjVal) == objective_value);

        }

    }


}