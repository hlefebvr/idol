//
// Created by henri on 13/02/23.
//

#include "../test_utils.h"
#include "problems/generalized-assignment-problem/GAP_Instance.h"

TEMPLATE_LIST_TEST_CASE("BranchAndPriceMIP: solve Generalized Assignment Problem with different stabilizations and branching schemes",
                        "[integration][backend][solver]",
                        branch_and_price_solvers) {

    Env env;

    using namespace Problems::GAP;

    const auto [filename, objective_value] = GENERATE(
            std::make_pair<std::string, double>("GAP_instance0.txt", -233.)
    );
    const auto farkas_pricing = GENERATE(false, true);
    const auto branching_on_master = GENERATE(true, false);
    const double smoothing_factor = GENERATE(0., .3, .5, .8);

    const auto instance = read_instance("instances/generalized-assignment-problem/GAP_instance0.txt");
    const unsigned int n_agents = instance.n_agents();
    const unsigned int n_jobs = instance.n_jobs();


    Annotation<Ctr> decomposition(env, "by_machines", MasterId);

    Model model(env);

    auto x = Var::array(env, Dim<2>(n_agents, n_jobs), 0., 1., Binary, "x");
    model.add_array<Var, 2>(x);

    for (unsigned int i = 0 ; i < n_agents ; ++i) {
        Ctr capacity(env, idol_Sum(j, Range(n_jobs), instance.resource_consumption(i, j) * x[i][j]) <= instance.capacity(i), "capacity_" + std::to_string(i));
        capacity.set(decomposition, i);
        model.add(capacity);
    }

    for (unsigned int j = 0 ; j < n_jobs ; ++j) {
        Ctr assignment(env, idol_Sum(i, Range(n_agents), x[i][j]) == 1, "assignment_" + std::to_string(j));
        model.add(assignment);
    }

    model.set(Attr::Obj::Expr, idol_Sum(i, Range(n_agents), idol_Sum(j, Range(n_jobs), instance.cost(i, j) * x[i][j])));

    Idol::set_optimizer<TestType>(model, decomposition);

    model.set(Param::ColumnGeneration::LogFrequency, 1);
    model.set(Param::ColumnGeneration::BranchingOnMaster, branching_on_master);
    model.set(Param::ColumnGeneration::FarkasPricing, farkas_pricing);
    model.set(Param::ColumnGeneration::SmoothingFactor, smoothing_factor);

    WHEN("The instance \"" + filename + "\" is solved") {

        std::cout << "Solving " << filename
                  << (farkas_pricing ? " with farkas pricing" : " with artificial variables")
                  << (branching_on_master ? " branching applied to master" : "branching applied to pricing")
                  << ", stabilization factor = " << smoothing_factor
                  << "..." << std::endl;
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