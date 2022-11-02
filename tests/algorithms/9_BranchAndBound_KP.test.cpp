//
// Created by henri on 12/10/22.
//

#include "../test_utils.h"
#include "problems/kp/KP_Instance.h"
#include "algorithms/callbacks/Callbacks_RoundingHeuristic.h"

using configurations =
        cartesian_product<
                lp_solvers,
                std::tuple<BranchingStrategies::MostInfeasible>,
                std::tuple<NodeStrategies::Basic<Nodes::Basic>>,
                std::tuple<ActiveNodesManagers::Basic>,
                std::tuple<NodeUpdators::ByBoundVar, NodeUpdators::ByBoundCtr>,
                std::tuple<std::true_type, std::false_type>
        >;

TEMPLATE_LIST_TEST_CASE("09. B&B: KP", has_lp_solver ? "[MILP][branch-and-bound][algorithms]" : "[.]", configurations) {

    using SolverT             = std::tuple_element_t<0, TestType>;
    using BranchingStrategyT  = std::tuple_element_t<1, TestType>;
    using NodeStrategyT       = std::tuple_element_t<2, TestType>;
    using ActiveNodeManagerT  = std::tuple_element_t<3, TestType>;
    using NodeUpdatorT        = std::tuple_element_t<4, TestType>;
    using RoundingHeuristicT  = std::tuple_element_t<5, TestType>;

    Model model;

    using namespace Problems::KP;

    auto test = GENERATE(
            std::make_pair<Instance, double>(read_instance("instances/KP/KP_instance0.txt"), -235.)
        );

    const auto& instance = test.first;
    const double optimum = test.second;
    const unsigned int n_items = instance.n_items();

    std::vector<Var> x;
    x.reserve(n_items);

    LinExpr sum_weight;

    for (unsigned int i = 0 ; i < n_items ; ++i) {
        auto var = model.add_var(0., 1., Continuous, -instance.p(i));
        sum_weight += instance.w(i) * var;
        x.emplace_back(var);
    }

    model.add_ctr(sum_weight <= instance.t());

    auto solver = branch_and_bound<
            SolverT,
            BranchingStrategyT,
            NodeStrategyT,
            ActiveNodeManagerT,
            NodeUpdatorT
    >(model, x);

    if (RoundingHeuristicT {}) {
        solver.template add_callback<Callbacks::RoundingHeuristic>(x);
    }

    solver.solve();

    CHECK(solver.status() == Optimal);
    CHECK(solver.objective_value() == Catch::Approx(optimum));

}